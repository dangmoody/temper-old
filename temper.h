#pragma once

/*
===========================================================================

Temper.

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
Temper is a single-header test framework written in C99 for use in your
C or C++ programs.  It is based on "greatest":
https://github.com/silentbicycle/greatest except it is slightly
different in usage.


2. INSTALLATION:
Include "temper.h" in your project and you're good to go.


3. CODE USAGE:
Temper requires init and shutdown calls.  These can be done anywhere,
but main() is recommended.  Temper also tracks the return code:

	int main( int argc, char** argv ) {
		TEMPER_INIT();

		// do your tests...

		TEMPER_SHUTDOWN();

		return TEMPER_EXIT_CODE();
	}

To create a test:

	TEMPER_TEST( XShouldEqual0 ) {
		float x = 0.0f;
		TEMPER_EXPECT_TRUE( floateq( x, 0.0f ) );

		TEMPER_PASS();
	}

The following assert-style macros are given; they do what you'd expect:
	- TEMPER_EXPECT_TRUE( condition );
	- TEMPER_EXPECT_FALSE( condition );

The following macros are given for return results of a test; they do what
you'd expect:
	- TEMPER_PASS();
	- TEMPER_FAIL();

In order to make a test suite that runs a series of tests:

	TEMPER_SUITE( TheSuite ) {
		TEMPER_RUN_TEST( XShouldEqual0 );
		TEMPER_RUN_TEST( SomeOtherTest );
		TEMPER_RUN_TEST( SomeOtherOtherTest );
	}

Tests can be run in and outside a test-suite, just like greatest.

If you want to skip a test:

	TEMPER_SKIP_TEST( FlakyTest, "TeamCity doesn't like this test for some reason..." );

Temper will then mark the test as skipped and display the reason
message in the console, for example:

	SKIPPED: FlakyTest: "TeamCity doesn't like this test for some reason...".

You can also specify callbacks to run before and after each test and suite
is run:

	TEMPER_SET_SUITE_START_CALLBACK( OnSuiteStarting, userdata );
	TEMPER_SET_SUITE_END_CALLBACK( OnSuiteEnded, userdata );

	TEMPER_SET_TEST_START_CALLBAK( OnTestStarting, userdata );
	TEMPER_SET_TEST_END_CALLBAK( OnTestEnded, userdata );

The start callback gets called before just the test/suite runs and the end
callback gets called just as the test/suite has finished running, before the
console output.  Both callbacks return void and take void* as arguments so
you may pass through them whatever you like.


4. COMMAND LINE USAGE:
Temper supports a few command line options:

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
#define TEMPER_COLOR_DEFAULT		0x07
#define TEMPER_COLOR_RED			0x0C
#define TEMPER_COLOR_GREEN			0x02
#define TEMPER_COLOR_YELLOW			0x0E

typedef uint32_t					temperTestConsoleColor_t;
#elif defined( __linux__ )
#define TEMPER_COLOR_DEFAULT		"\x1B[0m"
#define TEMPER_COLOR_RED			"\x1B[31m"
#define TEMPER_COLOR_GREEN			"\x1B[32m"
#define TEMPER_COLOR_YELLOW			"\x1B[33m"

typedef const char*					temperTestConsoleColor_t;
#endif

typedef enum temperTestFlagBits_t {
	TEMPER_FLAGS_ABORT_ON_FAIL		= 1ULL << 1,
	TEMPER_FLAGS_COLORS				= 1ULL << 2,
} temperTestFlagBits_t;
typedef uint32_t temperTestFlags_t;

typedef enum temperTestResult_t {
	TEMPER_RESULT_PASSED			= 0,
	TEMPER_RESULT_FAILED,
	TEMPER_RESULT_SKIPPED,
} temperTestResult_t;

typedef temperTestResult_t( *temperTestFunc_t )( void );

typedef void( *temperTestCallback_t )( void* userdata );

typedef struct temperTestContext_t {
	void*							suiteFuncStartData;
	void*							suiteFuncEndData;

	void*							testFuncStartData;
	void*							testFuncEndData;

	temperTestCallback_t			suiteFuncStart;
	temperTestCallback_t			suiteFuncEnd;

	temperTestCallback_t			testFuncStart;
	temperTestCallback_t			testFuncEnd;

	uint32_t						numPassed;
	uint32_t						numFailed;
	uint32_t						numSkipped;

	temperTestFlags_t				flags;

	uint32_t						line;
	const char*						file;
	const char*						msg;

	const char*						filteredTest;
	const char*						filteredSuite;
} temperTestContext_t;

static temperTestContext_t			g_testContext	= { 0 };

static void TemperSetTextColorInternal( const temperTestConsoleColor_t color ) {
	if ( ( g_testContext.flags & TEMPER_FLAGS_COLORS ) == 0 ) {
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
static const char* TemperGetNextArgInternal( const int argIndex, const int argc, char** argv ) {
	return ( argIndex + 1 < argc ) ? argv[argIndex + 1] : NULL;
}

static void TemperShowUsageInternal( void ) {
	printf(
		"Temper:\n"
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

#define TEMPER_INIT() \
	do { \
		memset( &g_testContext, 0, sizeof( temperTestContext_t ) ); \
	} while ( 0 )

#define TEMPER_SHUTDOWN() \
	do { \
		uint32_t totalTests = g_testContext.numPassed + g_testContext.numFailed + g_testContext.numSkipped; \
		printf( "\n%d tests run in total.  %d passed, %d failed, %d skipped.\n", totalTests, g_testContext.numPassed, g_testContext.numFailed, g_testContext.numSkipped ); \
	} while ( 0 )

#define TEMPER_EXIT_CODE()			( ( g_testContext.numFailed == 0 ) ? 0 : 1 )

#define TEMPER_TEST( name )			temperTestResult_t (name)( void ); temperTestResult_t (name)( void )
#define TEMPER_SUITE( name )		void (name)( void ); void (name)( void )

#define TEMPER_SET_COMMAND_LINE_ARGS( argc, argv ) \
	do { \
		for ( int i = 0; i < argc; i++ ) { \
			const char* arg = argv[i]; \
\
			if ( arg[0] == '-' ) { \
				switch ( arg[1] ) { \
					case 'a': g_testContext.flags |= TEMPER_FLAGS_ABORT_ON_FAIL; break; \
					case 'c': g_testContext.flags |= TEMPER_FLAGS_COLORS; break; \
\
					case 't': { \
						const char* testName = TemperGetNextArgInternal( i, argc, argv ); \
						if ( !testName ) { \
							TemperShowUsageInternal(); \
							exit( EXIT_FAILURE ); \
						} \
\
						g_testContext.filteredTest = testName; \
						i++; \
						break; \
					} \
\
					case 's': { \
						const char* suiteName = TemperGetNextArgInternal( i, argc, argv ); \
						if ( !suiteName ) { \
							TemperShowUsageInternal(); \
							exit( EXIT_FAILURE ); \
						} \
\
						g_testContext.filteredSuite = suiteName; \
						i++; \
						break; \
					} \
\
					case '-': { \
						if ( strcmp( arg, "--help" ) == 0 ) { \
							TemperShowUsageInternal(); \
							exit( EXIT_SUCCESS ); \
						} \
						break; \
					} \
\
					default: \
						printf( "ERROR: '%s' is an unknown arg.\n", arg ); \
						TemperShowUsageInternal(); \
						exit( EXIT_FAILURE ); \
						break; \
				} \
			} \
		} \
	} while ( 0 )

#define TEMPER_SET_TEST_START_CALLBACK( callback, userdata ) \
	do { \
		g_testContext.testFuncStart = callback; \
		g_testContext.testFuncStartData = userdata; \
	} while ( 0 )

#define TEMPER_SET_TEST_END_CALLBACK( callback, userdata ) \
	do { \
		g_testContext.testFuncEnd = callback; \
		g_testContext.testFuncEndData = userdata; \
	} while ( 0 )

#define TEMPER_SET_SUITE_START_CALLBACK( callback, userdata ) \
	do { \
		g_testContext.suiteFuncStart = callback; \
		g_testContext.suiteFuncStartData = userdata; \
	} while ( 0 )

#define TEMPER_SET_SUITE_END_CALLBACK( callback, userdata ) \
	do { \
		g_testContext.suiteFuncEnd = callback; \
		g_testContext.suiteFuncEndData = userdata; \
	} while ( 0 )

#define TEMPER_FAIL_TEST( fmt ) \
	do { \
		g_testContext.file = __FILE__; \
		g_testContext.line = __LINE__; \
		g_testContext.msg = (fmt); \
\
		g_testContext.numFailed++; \
\
		return TEMPER_RESULT_FAILED; \
	} while ( 0 )

#define TEMPER_RUN_SUITE_INTERNAL( suite ) \
	do { \
		if ( g_testContext.suiteFuncStart ) { \
			g_testContext.suiteFuncStart( g_testContext.suiteFuncStartData ); \
		} \
\
		suite(); \
\
		if ( g_testContext.suiteFuncEnd ) { \
			g_testContext.suiteFuncEnd( g_testContext.suiteFuncEndData ); \
		} \
	} while ( 0 )

#define TEMPER_RUN_SUITE( suite ) \
	do { \
		if ( g_testContext.filteredSuite ) { \
			if ( strcmp( g_testContext.filteredSuite, #suite ) == 0 ) { \
				TEMPER_RUN_SUITE_INTERNAL( suite ); \
			} \
		} else { \
			TEMPER_RUN_SUITE_INTERNAL( suite ); \
		} \
	} while ( 0 )

#define TEMPER_RUN_TEST_INTERNAL( test ) \
	do { \
		if ( g_testContext.testFuncStart ) { \
			g_testContext.testFuncStart( g_testContext.testFuncStartData ); \
		} \
\
		result = test(); \
\
		if ( g_testContext.testFuncEnd ) { \
			g_testContext.testFuncEnd( g_testContext.testFuncEndData ); \
		} \
	} while ( 0 )

#define TEMPER_RUN_TEST( test ) \
	do { \
		if ( ( ( g_testContext.flags & TEMPER_FLAGS_ABORT_ON_FAIL ) == 0 ) || g_testContext.numFailed == 0 ) { \
			temperTestResult_t result = TEMPER_RESULT_SKIPPED; \
			if ( g_testContext.filteredTest ) { \
				if ( strcmp( g_testContext.filteredTest, #test ) == 0 ) { \
					TEMPER_RUN_TEST_INTERNAL( test ); \
				} \
			} else { \
				TEMPER_RUN_TEST_INTERNAL( test ); \
			} \
\
			switch ( result ) { \
				case TEMPER_RESULT_PASSED: { \
					TemperSetTextColorInternal( TEMPER_COLOR_GREEN ); \
					printf( "	PASSED:" ); \
					TemperSetTextColorInternal( TEMPER_COLOR_DEFAULT ); \
					printf( "  %s.\n", #test ); \
					break; \
				} \
\
				case TEMPER_RESULT_FAILED: { \
					TemperSetTextColorInternal( TEMPER_COLOR_RED ); \
					printf( "	FAILED:" ); \
					TemperSetTextColorInternal( TEMPER_COLOR_YELLOW ); \
					printf( "  %s: \"%s\" at %s:%d.\n", #test, g_testContext.msg, g_testContext.file, g_testContext.line ); \
					TemperSetTextColorInternal( TEMPER_COLOR_DEFAULT ); \
					break; \
				} \
\
				case TEMPER_RESULT_SKIPPED: \
					/* skipped is handled differently */ \
					break; \
			} \
		} \
	} while ( 0 )

#define TEMPER_SKIP_TEST( test, reasonMsg ) \
	do { \
		if ( ( ( g_testContext.flags & TEMPER_FLAGS_ABORT_ON_FAIL ) == 0 ) || g_testContext.numFailed == 0 ) { \
			if ( g_testContext.filteredTest ) { \
				if ( strcmp( g_testContext.filteredTest, #test ) == 0 ) { \
					printf( "	SKIPPED: %s: \"%s\".\n", #test, reasonMsg ); \
					g_testContext.numSkipped++; \
				} \
			} else { \
				printf( "	SKIPPED: %s: \"%s\".\n", #test, reasonMsg ); \
				g_testContext.numSkipped++; \
			} \
		} \
	} while ( 0 )

#define TEMPER_EXPECT_TRUE( condition ) \
	do { \
		g_testContext.msg = NULL; \
		if ( !(condition) ) { \
			TEMPER_FAIL_TEST( #condition ); \
		} \
	} while ( 0 )

#define TEMPER_EXPECT_FALSE( condition ) \
	do { \
		g_testContext.msg = NULL; \
		if ( (condition) ) { \
			TEMPER_FAIL_TEST( #condition ); \
		} \
	} while ( 0 )

#define TEMPER_PASS() \
	do { \
		g_testContext.numPassed++; \
		return TEMPER_RESULT_PASSED; \
	} while ( 0 )

#define TEMPER_FAIL() \
	do { \
		g_testContext.numFailed++; \
		return TEMPER_RESULT_FAILED; \
	} while ( 0 )

#ifdef __cplusplus
}
#endif
