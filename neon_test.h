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


TODO(DM):
	- select specific tests via cmd arg

CONTENTS:
	1. INTRO
	2. INSTALLATION
	3. USAGE


1. INTRO:
neon test is a single-header test framework written in C99 for use in your
C or C++ programs.  It is based on "greatest":
https://github.com/silentbicycle/greatest except it is slightly
different in usage.


2. INSTALLATION:
Include "neon_test.h" in your project and you're good to go.


3. USAGE:
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

neon test also tracks the return code:

	int main( int argc, char** argv ) {
		NE_TEST_INIT();

		// do your tests...

		NE_TEST_SHUTDOWN();

		return NE_TEST_EXIT_CODE();
	}

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

// TODO(DM): put back in
// TODO(DM): mac, linux, etc.
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

void NE_Test_SetTextColor( const neTestConsoleColor_t color ) {
#if defined( _WIN32 )
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), (WORD) color );
#elif defined( __linux__ )
	printf( color );
#endif
}

typedef enum neTestResult_t {
	NE_TEST_RESULT_PASSED		= 0,
	NE_TEST_RESULT_FAILED,
	NE_TEST_RESULT_SKIPPED,
} neTestResult_t;

typedef neTestResult_t( *neTestFunc_t )( void );

typedef void( *testCallback_t )( void* userdata );

typedef struct neTestContext_t {
	void*						suiteFuncStartData;
	void*						suiteFuncEndData;

	void*						testFuncStartData;
	void*						testFuncEndData;

	testCallback_t				suiteFuncStart;
	testCallback_t				suiteFuncEnd;

	testCallback_t				testFuncStart;
	testCallback_t				testFuncEnd;

	uint32_t					numPassed;
	uint32_t					numFailed;
	uint32_t					numSkipped;

	uint32_t					line;
	const char*					file;
	const char*					msg;
} neTestContext_t;

static neTestContext_t			g_context	= { 0 };

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

#define NE_TEST_RUN_SUITE( suite ) \
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

#define NE_TEST( name )			neTestResult_t (name)( void ); neTestResult_t (name)( void )
#define NE_TEST_SUITE( name )	void (name)( void ); void (name)( void )

#define NE_TEST_RUN_TEST( test ) \
	do { \
		if ( g_context.testFuncStart ) { \
			g_context.testFuncStart( g_context.testFuncStartData ); \
		} \
\
		neTestResult_t result = test(); \
\
		if ( g_context.testFuncEnd ) { \
			g_context.testFuncEnd( g_context.testFuncEndData ); \
		} \
\
		switch ( result ) { \
			case NE_TEST_RESULT_PASSED: \
				NE_Test_SetTextColor( NE_TEST_COLOR_GREEN ); \
				printf( "	PASSED:" ); \
				NE_Test_SetTextColor( NE_TEST_COLOR_DEFAULT ); \
				printf( "  %s.\n", #test ); \
				break; \
\
			case NE_TEST_RESULT_FAILED: \
				NE_Test_SetTextColor( NE_TEST_COLOR_RED ); \
				printf( "	FAILED:" ); \
				NE_Test_SetTextColor( NE_TEST_COLOR_YELLOW ); \
				printf( "  %s: \"%s\" at %s:%d.\n", #test, g_context.msg, g_context.file, g_context.line ); \
				NE_Test_SetTextColor( NE_TEST_COLOR_DEFAULT ); \
				break; \
\
			case NE_TEST_RESULT_SKIPPED: \
				/* skipped is handled differently */ \
				break; \
		} \
	} while ( 0 )

#define NE_TEST_SKIP_TEST( test, reasonMsg ) \
	do { \
		printf( "	SKIPPED: %s: \"%s\".\n", #test, reasonMsg ); \
		g_context.numSkipped++; \
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
