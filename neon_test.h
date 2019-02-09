#pragma once

/*
===========================================================================

neon test.

Distributed under MIT License:
Copyright (c) 2019 Dan Moody

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


CONTENTS:
	1. INTRO
	2. INSTALLATION
	3. CODE USAGE
	4. COMMAND LINE USAGE


1. INTRO:
neon test is a single-header test framework written in C99 for use in your
C or C++ programs.  It is based on "greatest":
https://github.com/silentbicycle/greatest except it is slightly
different in usage.


2. INSTALLATION:
Include "neon_test.h" in your project and you're good to go.


3. CODE USAGE:
neon test requires init and shutdown calls.  These can be done anywhere,
but main() is recommended.  neon test also tracks the return code:

	int main( int argc, char** argv ) {
		NE_TEST_INIT();

		// do your tests...

		NE_TEST_SHUTDOWN();

		return NE_TEST_EXIT_CODE();
	}

To create a test:

	NE_TEST( XShouldEqual0 ) {
		float x = 0.0f;
		NE_TEST_EXPECT_TRUE( floateq( x, 0.0f ) );

		NE_TEST_PASS();
	}

The following assert-style macros are given; they do what you'd expect:
	- NE_TEST_EXPECT_TRUE( condition );
	- NE_TEST_EXPECT_FALSE( condition );

The following macros are given for return results of a test; they do what
you'd expect:
	- NE_TEST_PASS();
	- NE_TEST_FAIL();

In order to make a test suite that runs a series of tests:

	NE_TEST_SUITE( TheSuite ) {
		NE_TEST_RUN_TEST( XShouldEqual0 );
		NE_TEST_RUN_TEST( SomeOtherTest );
		NE_TEST_RUN_TEST( SomeOtherOtherTest );
	}

Tests can be run in and outside a test-suite, just like greatest.

If you want to skip a test:

	NE_TEST_SKIP_TEST( FlakyTest, "TeamCity doesn't like this test for some reason..." );

neon test will then mark the test as skipped and display the reason
message in the console, for example:

	SKIPPED: FlakyTest: "TeamCity doesn't like this test for some reason...".

You can also specify callbacks to run before and after each test and suite
is run:

	NE_TEST_SET_SUITE_START_CALLBACK( OnSuiteStarting, userdata );
	NE_TEST_SET_SUITE_END_CALLBACK( OnSuiteEnded, userdata );

	NE_TEST_SET_TEST_START_CALLBAK( OnTestStarting, userdata );
	NE_TEST_SET_TEST_END_CALLBAK( OnTestEnded, userdata );

The start callback gets called before just the test/suite runs and the end
callback gets called just as the test/suite has finished running, before the
console output.  Both callbacks return void and take void* as arguments so
you may pass through them whatever you like.


4. COMMAND LINE USAGE:
neon test supports a few command line options:

	--help		: Shows the help in console.
	-t <name>	: Only run the test with the given name.
	-s <suite>	: Only run the suite with the given name.
	-a			: Abort immediately on test failure.
	-c			: Enable colored output.

===========================================================================
*/

#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdio.h>
#include <malloc.h>
#include <memory.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined( _WIN32 )
#define NE_TEST_COLOR_DEFAULT	0x07
#define NE_TEST_COLOR_RED		0x0C
#define NE_TEST_COLOR_GREEN		0x02
#define NE_TEST_COLOR_YELLOW	0x0E

typedef uint32_t				neTestConsoleColor_t;
#elif defined( __linux__ )
#define NE_TEST_COLOR_DEFAULT	"\x1B[0m"
#define NE_TEST_COLOR_RED		"\x1B[31m"
#define NE_TEST_COLOR_GREEN		"\x1B[32m"
#define NE_TEST_COLOR_YELLOW	"\x1B[33m"

typedef const char*				neTestConsoleColor_t;
#endif

typedef enum neTestFlagBits_t {
	NE_TEST_FLAGS_ABORT_ON_FAIL	= 1ULL << 1,
	NE_TEST_FLAGS_COLORS		= 1ULL << 2,
} neTestFlagBits_t;
typedef uint32_t neTestFlags_t;

typedef enum neTestResult_t {
	NE_TEST_RESULT_PASSED		= 0,
	NE_TEST_RESULT_FAILED,
	NE_TEST_RESULT_SKIPPED,
} neTestResult_t;

typedef neTestResult_t( *neTestFunc_t )( void );

typedef void( *neTestCallback_t )( void* userdata );

typedef struct neTestContext_t {
	void*						suiteFuncStartData;
	void*						suiteFuncEndData;

	void*						testFuncStartData;
	void*						testFuncEndData;

	neTestCallback_t			suiteFuncStart;
	neTestCallback_t			suiteFuncEnd;

	neTestCallback_t			testFuncStart;
	neTestCallback_t			testFuncEnd;

	uint32_t					numPassed;
	uint32_t					numFailed;
	uint32_t					numSkipped;

	neTestFlags_t				flags;

	uint32_t					line;
	const char*					file;
	const char*					msg;

	const char*					filteredTest;
	const char*					filteredSuite;
} neTestContext_t;

static neTestContext_t			g_context	= { 0 };

static void NE_Test_SetTextColor( const neTestConsoleColor_t color ) {
	if ( ( g_context.flags & NE_TEST_FLAGS_COLORS ) == 0 ) {
		return;
	}

#if defined( _WIN32 )
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), (WORD) color );
#elif defined( __linux__ )
	printf( color );
#endif
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
static const char* NE_Test_GetNextArg( const int argIndex, const int argc, char** argv ) {
	return ( argIndex + 1 < argc ) ? argv[argIndex + 1] : NULL;
}

static void NE_Test_ShowUsage( void ) {
	printf(
		"neon test:\n"
		"Usage:\n"
		"\n"
		"\t--help     : Shows this help.\n"
		"\t-t <name>  : Only run the test with the given name.\n"
		"\t-s <suite> : Only run the suite with the given name.\n"
		"\t-a         : Abort immediately on test failure.\n"
		"\t-c         : Enable colored output.\n"
	);
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#define NE_TEST_INIT() \
	do { \
		memset( &g_context, 0, sizeof( neTestContext_t ) ); \
	} while ( 0 )

#define NE_TEST_SHUTDOWN() \
	do { \
		uint32_t totalTests = g_context.numPassed + g_context.numFailed + g_context.numSkipped; \
		printf( "\n%d tests run in total.  %d passed, %d failed, %d skipped.\n", totalTests, g_context.numPassed, g_context.numFailed, g_context.numSkipped ); \
	} while ( 0 )

#define NE_TEST_EXIT_CODE()		( ( g_context.numFailed == 0 ) ? 0 : 1 )

#define NE_TEST( name )			neTestResult_t (name)( void ); neTestResult_t (name)( void )
#define NE_TEST_SUITE( name )	void (name)( void ); void (name)( void )

#define NE_TEST_SET_COMMAND_LINE_ARGS( argc, argv ) \
	do { \
		for ( int i = 0; i < argc; i++ ) { \
			const char* arg = argv[i]; \
\
			if ( arg[0] == '-' ) { \
				switch ( arg[1] ) { \
					case 'a': g_context.flags |= NE_TEST_FLAGS_ABORT_ON_FAIL; break; \
					case 'c': g_context.flags |= NE_TEST_FLAGS_COLORS; break; \
\
					case 't': { \
						const char* testName = NE_Test_GetNextArg( i, argc, argv ); \
						if ( !testName ) { \
							NE_Test_ShowUsage(); \
							exit( EXIT_FAILURE ); \
						} \
\
						g_context.filteredTest = testName; \
						i++; \
						break; \
					} \
\
					case 's': { \
						const char* suiteName = NE_Test_GetNextArg( i, argc, argv ); \
						if ( !suiteName ) { \
							NE_Test_ShowUsage(); \
							exit( EXIT_FAILURE ); \
						} \
\
						g_context.filteredSuite = suiteName; \
						i++; \
						break; \
					} \
\
					case '-': { \
						if ( strcmp( arg, "--help" ) == 0 ) { \
							NE_Test_ShowUsage(); \
							exit( EXIT_SUCCESS ); \
						} \
						break; \
					} \
\
					default: \
						printf( "ERROR: '%s' is an unknown arg.\n", arg ); \
						NE_Test_ShowUsage(); \
						exit( EXIT_FAILURE ); \
						break; \
				} \
			} \
		} \
	} while ( 0 )

#define NE_TEST_SET_TEST_START_CALLBACK( callback, userdata ) \
	do { \
		g_context.testFuncStart = callback; \
		g_context.testFuncStartData = userdata; \
	} while ( 0 )

#define NE_TEST_SET_TEST_END_CALLBACK( callback, userdata ) \
	do { \
		g_context.testFuncEnd = callback; \
		g_context.testFuncEndData = userdata; \
	} while ( 0 )

#define NE_TEST_SET_SUITE_START_CALLBACK( callback, userdata ) \
	do { \
		g_context.suiteFuncStart = callback; \
		g_context.suiteFuncStartData = userdata; \
	} while ( 0 )

#define NE_TEST_SET_SUITE_END_CALLBACK( callback, userdata ) \
	do { \
		g_context.suiteFuncEnd = callback; \
		g_context.suiteFuncEndData = userdata; \
	} while ( 0 )

#define NE_TEST_FAIL_TEST( fmt ) \
	do { \
		g_context.file = __FILE__; \
		g_context.line = __LINE__; \
		g_context.msg = (fmt); \
\
		g_context.numFailed++; \
\
		return NE_TEST_RESULT_FAILED; \
	} while ( 0 )

#define NE_TEST_RUN_SUITE_INTERNAL( suite ) \
	do { \
		if ( g_context.suiteFuncStart ) { \
			g_context.suiteFuncStart( g_context.suiteFuncStartData ); \
		} \
\
		suite(); \
\
		if ( g_context.suiteFuncEnd ) { \
			g_context.suiteFuncEnd( g_context.suiteFuncEndData ); \
		} \
	} while ( 0 )

#define NE_TEST_RUN_SUITE( suite ) \
	do { \
		if ( g_context.filteredSuite ) { \
			if ( strcmp( g_context.filteredSuite, #suite ) == 0 ) { \
				NE_TEST_RUN_SUITE_INTERNAL( suite ); \
			} \
		} else { \
			NE_TEST_RUN_SUITE_INTERNAL( suite ); \
		} \
	} while ( 0 )

#define NE_TEST_RUN_TEST_INTERNAL( test ) \
	do { \
		if ( g_context.testFuncStart ) { \
			g_context.testFuncStart( g_context.testFuncStartData ); \
		} \
\
		result = test(); \
\
		if ( g_context.testFuncEnd ) { \
			g_context.testFuncEnd( g_context.testFuncEndData ); \
		} \
	} while ( 0 )

#define NE_TEST_RUN_TEST( test ) \
	do { \
		if ( ( ( g_context.flags & NE_TEST_FLAGS_ABORT_ON_FAIL ) == 0 ) || g_context.numFailed == 0 ) { \
			neTestResult_t result = NE_TEST_RESULT_SKIPPED; \
			if ( g_context.filteredTest ) { \
				if ( strcmp( g_context.filteredTest, #test ) == 0 ) { \
					NE_TEST_RUN_TEST_INTERNAL( test ); \
				} \
			} else { \
				NE_TEST_RUN_TEST_INTERNAL( test ); \
			} \
\
			switch ( result ) { \
				case NE_TEST_RESULT_PASSED: { \
					NE_Test_SetTextColor( NE_TEST_COLOR_GREEN ); \
					printf( "	PASSED:" ); \
					NE_Test_SetTextColor( NE_TEST_COLOR_DEFAULT ); \
					printf( "  %s.\n", #test ); \
					break; \
				} \
\
				case NE_TEST_RESULT_FAILED: { \
					NE_Test_SetTextColor( NE_TEST_COLOR_RED ); \
					printf( "	FAILED:" ); \
					NE_Test_SetTextColor( NE_TEST_COLOR_YELLOW ); \
					printf( "  %s: \"%s\" at %s:%d.\n", #test, g_context.msg, g_context.file, g_context.line ); \
					NE_Test_SetTextColor( NE_TEST_COLOR_DEFAULT ); \
					break; \
				} \
\
				case NE_TEST_RESULT_SKIPPED: \
					/* skipped is handled differently */ \
					break; \
			} \
		} \
	} while ( 0 )

#define NE_TEST_SKIP_TEST( test, reasonMsg ) \
	do { \
		if ( ( ( g_context.flags & NE_TEST_FLAGS_ABORT_ON_FAIL ) == 0 ) || g_context.numFailed == 0 ) { \
			if ( g_context.filteredTest ) { \
				if ( strcmp( g_context.filteredTest, #test ) == 0 ) { \
					printf( "	SKIPPED: %s: \"%s\".\n", #test, reasonMsg ); \
					g_context.numSkipped++; \
				} \
			} else { \
				printf( "	SKIPPED: %s: \"%s\".\n", #test, reasonMsg ); \
				g_context.numSkipped++; \
			} \
		} \
	} while ( 0 )

#define NE_TEST_EXPECT_TRUE( condition ) \
	do { \
		g_context.msg = NULL; \
		if ( !(condition) ) { \
			NE_TEST_FAIL_TEST( #condition ); \
		} \
	} while ( 0 )

#define NE_TEST_EXPECT_FALSE( condition ) \
	do { \
		g_context.msg = NULL; \
		if ( (condition) ) { \
			NE_TEST_FAIL_TEST( #condition ); \
		} \
	} while ( 0 )

#define NE_TEST_PASS() \
	do { \
		g_context.numPassed++; \
		return NE_TEST_RESULT_PASSED; \
	} while ( 0 )

#define NE_TEST_FAIL() \
	do { \
		g_context.numFailed++; \
		return NE_TEST_RESULT_FAILED; \
	} while ( 0 )

#ifdef __cplusplus
}
#endif
