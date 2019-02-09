#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdio.h>
#include <malloc.h>
#include <memory.h>

#include <stdbool.h>
#include <stdint.h>

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
	- test & suite start/end callbacks
	- select specific tests via cmd arg
	- output colors for mac, linux


1. INTRO:
neon test is a single-header test framework written in C99 for use in your
C or C++ programs.  It is based on "greatest":
https://github.com/silentbicycle/greatest except it is slightly different.


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
message.

neon test also tracks the return code:

	int main( int argc, char** argv ) {
		NE_TEST_INIT();

		// do your tests...

		NE_TEST_SHUTDOWN();

		return NE_TEST_EXIT_CODE();
	}

===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif

// TODO(DM): put back in
// TODO(DM): mac, linux, etc.
typedef enum consoleColor_t {
#ifdef _WIN32
	NE_TEST_COLOR_DEFAULT	= 0x07,
	NE_TEST_COLOR_RED		= 0x0C,
	NE_TEST_COLOR_GREEN		= 0x02,
	NE_TEST_COLOR_BLUE		= 0x09,
	NE_TEST_COLOR_YELLOW	= 0x0E,
#endif
} consoleColor_t;

void NE_Test_SetTextColor( const consoleColor_t color ) {
#ifdef _WIN32
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), (WORD) color );
#else
	// TODO(DM): mac, linux, etc.
	( (void) color );
#endif
}

typedef enum neTestResult_t {
	NE_TEST_RESULT_PASSED	= 0,
	NE_TEST_RESULT_FAILED,
	NE_TEST_RESULT_SKIPPED,
} neTestResult_t;

typedef neTestResult_t( *neTestFunc_t )( void );

typedef struct neTestContext_t {
	uint32_t				numPassed;
	uint32_t				numFailed;
	uint32_t				numSkipped;

	uint32_t				line;
	const char*				file;
	const char*				msg;
} neTestContext_t;

neTestContext_t				g_context	= { 0 };
bool						g_allPassed	= true;

#define NE_TEST_INIT() \
	do { \
	} while ( 0 )

#define NE_TEST_SHUTDOWN() \
	do { \
		uint32_t totalTests = g_context.numPassed + g_context.numFailed + g_context.numSkipped; \
		printf( "\n%d tests run in total.  %d passed, %d failed, %d skipped.\n", totalTests, g_context.numPassed, g_context.numFailed, g_context.numSkipped ); \
	} while ( 0 )

#define NE_TEST_EXIT_CODE()						( g_allPassed ? 0 : 1 )

#define NE_TEST_FAIL_TEST( fmt ) \
	do { \
		g_allPassed = false; \
\
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
		suite(); \
	} while ( 0 )

#define NE_TEST( name )							neTestResult_t (name)( void ); neTestResult_t (name)( void )
#define NE_TEST_SUITE( name )					void (name)( void ); void (name)( void )

#define NE_TEST_RUN_TEST( test ) \
	do { \
		neTestResult_t result = test(); \
		switch ( result ) { \
			case NE_TEST_RESULT_PASSED: \
				NE_Test_SetTextColor( NE_TEST_COLOR_GREEN ); \
				printf( "	PASSED:" ); \
				NE_Test_SetTextColor( NE_TEST_COLOR_DEFAULT ); \
				printf( "  %s:\n", #test ); \
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
			default: \
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
