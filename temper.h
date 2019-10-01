/*
===========================================================================

Temper.
v1.1.1

Distributed under MIT License:
Copyright (c) 2019 Dan Moody (daniel.guy.moody@gmail.com)

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
C or C++ programs.  It is heavily based on "greatest":
https://github.com/silentbicycle/greatest except it is slightly
different in usage.

Go and show the author of greatest some love - I would probably have
never written this library without seeing greatest or how it works.


2. INSTALLATION:
Include "temper.h" in your project and you're good to go.


3. CODE USAGE:
Temper requires only one call to setup "defs".  These must be in the same
file as main().  Temper also tracks the return code:

	TEMPER_DEFS();

	int main( int argc, char** argv ) {
		// do your tests, suites, whatever

		TEMPER_SHOW_STATS();

		return TEMPER_EXIT_CODE();
	}

TEMPER_SHOW_STATS(); prints the stats on passed, failed, and skipped
tests.

To create a test:

	TEMPER_TEST( XShouldEqual0 ) {
		float x = 0.0f;
		TEMPER_EXPECT_TRUE( floateq( x, 0.0f ) );

		TEMPER_PASS();
	}

To then run that test:

	TEMPER_RUN_TEST( XShouldEqual0 );

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

Tests can be run in and outside a test suite, just like greatest.

You can also forward declare tests and suites:

	// forward declare a suite
	TEMPER_SUITE_EXTERN( TheSuite );

	// forward declare a test
	TEMPER_TEST_EXTERN( XShouldEqual0 );

If you want to skip a test:

	TEMPER_SKIP_TEST( XShouldEqual0, "TeamCity doesn't like this test for some reason..." );

Temper will then mark the test as skipped and display the reason
message in the console, for example:

	SKIPPED: XShouldEqual0: "TeamCity doesn't like this test for some reason...".

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

Any functions with "Internal" in the name means you probably shouldn't go
touching it.


4. COMMAND LINE USAGE:
Temper supports a few command line options:

		-h
		--help
			Shows this help and exits the program.

		-t <name>
			Only run the test with the given name.

		-s <suite>
			Only run the suite with the given name.

		-a
			Abort immediately on test failure.

		-c
			Enable colored output.

		--time-unit=<unit>
			Set the timer unit of measurement.
			Can be either: clocks, ns, us, ms, or seconds.

These settings can also be configured via user functions.

If you don't want to set these options via command line and instead do it
via code, you can do that.  Temper has flags that you can set (temperFlags_t):

	// enable color output
	TEMPER_TURN_FLAG_ON( TEMPER_FLAG_COLORS );

	// disable immediate abort on first fail
	TEMPER_TURN_FLAG_OFF( TEMPER_FLAG_ABORT_ON_FAIL );

And to filter tests without command line args:

	// tell Temper that you only want to run this suite
	// you will still need to manually run it
	TEMPER_FILTER_SUITE( TheSuite );

	// tell Temper that you only want to run this test
	// you will still need to manually run it
	TEMPER_FILTER_TEST( XShouldEqual0 );

To set the time unit, you'll need to set the `temperTimeUnit_t` enum via (for example):

	// sets the unit of measurement for how long tests take to run to microseconds
	TEMPER_SET_TIME_UNIT( TEMPER_TIME_UNIT_US );

===========================================================================
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined( __clang__ )
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined( __GNUC__ )
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#ifdef __cplusplus
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#elif defined( _MSC_VER )
#pragma warning( disable : 4505 )	// unused function
#pragma warning( disable : 4551 )	// function call missing argument list
#endif // defined( __clang__ )

#if defined( __linux__ ) || defined( __APPLE__ )
#pragma push_macro( "_POSIX_C_SOURCE" )
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include <string.h>

#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#if defined( __clang__ )
#pragma clang diagnostic push
#elif defined( __GNUC__ )
#pragma GCC diagnostic push
#elif defined( _MSC_VER )
#pragma warning( push, 4 )
#endif // defined( __clang__ )

#if defined( _WIN32 )
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#elif defined( __linux__ ) || defined( __APPLE__ )
#include <time.h>
#endif // defined( _WIN32 )

#if defined( _WIN32 )
#define TEMPER_COLOR_DEFAULT		0x07
#define TEMPER_COLOR_RED			0x0C
#define TEMPER_COLOR_GREEN			0x02
#define TEMPER_COLOR_YELLOW			0x0E

typedef uint32_t					temperTestConsoleColor_t;
#elif defined( __linux__ ) || defined( __APPLE__ )
#define TEMPER_COLOR_DEFAULT		"\x1B[0m"
#define TEMPER_COLOR_RED			"\x1B[31m"
#define TEMPER_COLOR_GREEN			"\x1B[32m"
#define TEMPER_COLOR_YELLOW			"\x1B[33m"

typedef const char*					temperTestConsoleColor_t;
#endif // defined( _WIN32 )

typedef enum temperFlagBits_t {
	TEMPER_FLAG_ABORT_ON_FAIL		= 1 << 1,	// stop testing immediately after a test fails
	TEMPER_FLAG_COLORED_OUTPUT		= 1 << 2	// output to console with colors
} temperFlagBits_t;
typedef uint32_t temperFlags_t;

typedef enum temperTestResult_t {
	TEMPER_RESULT_PASSED			= 0,
	TEMPER_RESULT_FAILED,
	TEMPER_RESULT_SKIPPED
} temperTestResult_t;

typedef enum temperTimeUnit_t {
	TEMPER_TIME_UNIT_CLOCKS			= 0,
	TEMPER_TIME_UNIT_NS,
	TEMPER_TIME_UNIT_US,
	TEMPER_TIME_UNIT_MS,
	TEMPER_TIME_UNIT_SECONDS
} temperTimeUnit_t;

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

	double							testTime;

	temperFlags_t					flags;
	temperTimeUnit_t				timeUnit;

	uint32_t						numPassed;
	uint32_t						numFailed;
	uint32_t						numSkipped;

	uint32_t						line;
	const char*						file;
	const char*						msg;

	const char*						filteredTest;
	const char*						filteredSuite;

	const char*						timeUnitStr;
} temperTestContext_t;

extern temperTestContext_t			g_testContext;

// initialises Temper
// this is required to make Temper work properly
#ifdef __cplusplus
#define TEMPER_DEFS() \
	temperTestContext_t				g_testContext	= {}
#else
#define TEMPER_DEFS() \
	temperTestContext_t				g_testContext	= { 0 }
#endif

// returns the program exit code according to Temper
#define TEMPER_EXIT_CODE()			( ( g_testContext.numFailed == 0 ) ? 0 : 1 )

// forward declare a test suite
#define TEMPER_SUITE_EXTERN( name )	void (name)( void )

// defines a test suite (with your code)
#define TEMPER_SUITE( name )		void (name)( void ); void (name)( void )

// forward declare a test
#define TEMPER_TEST_EXTERN( name )	temperTestResult_t (name)( void )

// defines a test (with your code)
#define TEMPER_TEST( name )			temperTestResult_t (name)( void ); temperTestResult_t (name)( void )

static void TemperSetTextColorInternal( const temperTestConsoleColor_t color ) {
	if ( ( g_testContext.flags & TEMPER_FLAG_COLORED_OUTPUT ) == 0 ) {
		return;
	}

#if defined( _WIN32 )
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), (WORD) color );
#elif defined( __linux__ ) || defined( __APPLE__ )
	printf( "%s", color );
#endif
}

static const char* TemperGetNextArgInternal( const int argIndex, const int argc, char** argv ) {
	return ( argIndex + 1 < argc ) ? argv[argIndex + 1] : NULL;
}

static void TemperShowUsageInternal( void ) {
	printf(
		"Temper:\n"
		"Usage:\n"
		"    -h\n"
		"    --help\n"
		"        Shows this help and exits the program.\n"
		"\n"
		"    -t <name>\n"
		"        Only run the test with the given name.\n"
		"\n"
		"    -s <suite>\n"
		"        Only run the suite with the given name.\n"
		"\n"
		"    -a\n"
		"        Abort immediately on test failure.\n"
		"\n"
		"    -c\n"
		"        Enable colored output.\n"
		"\n"
		"    --time-unit=<unit>\n"
		"        Set the timer unit of measurement.\n"
		"        Can be either: clocks, ns, us, ms, or seconds.\n"
	);
}

static double TemperGetTimestampInternal( void ) {
#if defined( _WIN32 )
	static LARGE_INTEGER frequency;
	if ( frequency.QuadPart == 0 ) {
		QueryPerformanceFrequency( &frequency );
	}

	LARGE_INTEGER now;
	QueryPerformanceCounter( &now );

	switch ( g_testContext.timeUnit ) {
		case TEMPER_TIME_UNIT_CLOCKS:	return (double) ( now.QuadPart );
		case TEMPER_TIME_UNIT_NS:		return (double) ( ( now.QuadPart * 1000000000 ) / frequency.QuadPart );
		case TEMPER_TIME_UNIT_US:		return (double) ( ( now.QuadPart * 1000000 ) / frequency.QuadPart );
		case TEMPER_TIME_UNIT_MS:		return (double) ( ( now.QuadPart * 1000 ) / frequency.QuadPart );
		case TEMPER_TIME_UNIT_SECONDS:	return (double) ( ( now.QuadPart ) / frequency.QuadPart );
	}
#elif defined( __APPLE__ ) || defined( __linux__ )
	struct timespec now;
	clock_gettime( CLOCK_MONOTONIC, &now );

	int64_t clocks = (int64_t) ( now.tv_sec * 1000000000 + now.tv_nsec );

	switch ( g_testContext.timeUnit ) {
		case TEMPER_TIME_UNIT_CLOCKS:	return (double) clocks;
		case TEMPER_TIME_UNIT_NS:		return (double) clocks;
		case TEMPER_TIME_UNIT_US:		return (double) clocks / 1000.0;
		case TEMPER_TIME_UNIT_MS:		return (double) clocks / 1000000.0;
		case TEMPER_TIME_UNIT_SECONDS:	return (double) clocks / 1000000000.0;
	}
#endif

	// should never get here
	assert( false && "Unrecognised time unit passed into TemperGetTimestampInternal().\n" );

	return 0.0;
}

// get the stats from temper on passed, failed, and skipped tests
// you'll probably only want to display this at the end of your test program
#define TEMPER_SHOW_STATS() \
	do { \
		uint32_t totalTests = g_testContext.numPassed + g_testContext.numFailed + g_testContext.numSkipped; \
		printf( "\n%d tests run in total.  %d passed, %d failed, %d skipped.\n", totalTests, g_testContext.numPassed, g_testContext.numFailed, g_testContext.numSkipped ); \
	} while ( 0 )

// enable one or more temperFlags_t
#define TEMPER_TURN_FLAG_ON( flag ) \
	do { \
		g_testContext.flags |= (flag); \
	} while ( 0 )

// disable one or more temperFlags_t
#define TEMPER_TURN_FLAG_OFF( flag ) \
	do { \
		g_testContext.flags &= ~(flag); \
	} while ( 0 )

#define TEMPER_FILTER_SUITE_INTERNAL( suiteName ) \
	do { \
		g_testContext.filteredSuite = suiteName; \
	} while ( 0 )

// tell Temper that you only want to run this suite
// you will still need to manually run it
#define TEMPER_FILTER_SUITE( suite ) \
	TEMPER_FILTER_SUITE_INTERNAL( #suite )

#define TEMPER_FILTER_TEST_INTERNAL( testName ) \
	do { \
		g_testContext.filteredTest = testName; \
	} while ( 0 )

// tell Temper that you only want to run this test
// you will still need to manually run it
#define TEMPER_FILTER_TEST( test ) \
	TEMPER_FILTER_TEST_INTERNAL( #test )

// set Temper's command line args (and therefore settings) in one go
// this call is optional
// you can also call various functions in Temper to do the same things
#define TEMPER_SET_COMMAND_LINE_ARGS( argc, argv ) \
	do { \
		/* set defaults in case args don't get set */ \
		TEMPER_SET_TIME_UNIT( TEMPER_TIME_UNIT_MS ); \
\
		for ( int i = 0; i < argc; i++ ) { \
			const char* arg = argv[i]; \
			const char* nextArg = TemperGetNextArgInternal( i, argc, argv ); \
			size_t arglen = strlen( arg ); \
\
			if ( arg[0] == '-' ) { \
				switch ( arg[1] ) { \
					case 'h': TemperShowUsageInternal(); exit( EXIT_SUCCESS ); \
\
					case 'a': TEMPER_TURN_FLAG_ON( TEMPER_FLAG_ABORT_ON_FAIL ); break; \
					case 'c': TEMPER_TURN_FLAG_ON( TEMPER_FLAG_COLORED_OUTPUT ); break; \
\
					case 't': { \
						if ( !nextArg ) { \
							TemperShowUsageInternal(); \
							exit( EXIT_FAILURE ); \
						} \
\
						TEMPER_FILTER_TEST( nextArg ); \
						i++; \
						break; \
					} \
\
					case 's': { \
						if ( !nextArg ) { \
							TemperShowUsageInternal(); \
							exit( EXIT_FAILURE ); \
						} \
\
						TEMPER_FILTER_SUITE( nextArg ); \
						i++; \
						break; \
					} \
\
					case '-': { \
						if ( strcmp( arg, "--help" ) == 0 ) { \
							TemperShowUsageInternal(); \
							exit( EXIT_SUCCESS ); \
						} else if ( strstr( arg, "--time-unit=" ) ) { \
							const char* unitStart = (const char*) memchr( arg, '=', arglen ); \
							unitStart++; \
\
							char unitStr[1024]; \
							sprintf( unitStr, "%s", unitStart ); \
\
							if ( strcmp( unitStart, "clocks" ) == 0 ) { \
								TEMPER_SET_TIME_UNIT( TEMPER_TIME_UNIT_CLOCKS ); \
							} else if ( strcmp( unitStart, "ns" ) == 0 ) { \
								TEMPER_SET_TIME_UNIT( TEMPER_TIME_UNIT_NS ); \
							} else if ( strcmp( unitStart, "us" ) == 0 ) { \
								TEMPER_SET_TIME_UNIT( TEMPER_TIME_UNIT_US ); \
							} else if ( strcmp( unitStart, "ms" ) == 0 ) { \
								TEMPER_SET_TIME_UNIT( TEMPER_TIME_UNIT_MS ); \
							} else if ( strcmp( unitStart, "seconds" ) == 0 ) { \
								TEMPER_SET_TIME_UNIT( TEMPER_TIME_UNIT_SECONDS ); \
							} else { \
								printf( "ERROR: Unknown time unit passed into Temper.\n" ); \
								TemperShowUsageInternal(); \
								exit( EXIT_FAILURE ); \
							} \
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

// set the function that will get called just before a test starts
#define TEMPER_SET_TEST_START_CALLBACK( callback, userdata ) \
	do { \
		g_testContext.testFuncStart = callback; \
		g_testContext.testFuncStartData = userdata; \
	} while ( 0 )

// set the function that will get called just after a test ends and before console output
#define TEMPER_SET_TEST_END_CALLBACK( callback, userdata ) \
	do { \
		g_testContext.testFuncEnd = callback; \
		g_testContext.testFuncEndData = userdata; \
	} while ( 0 )

// set the function that will get called just before a test suite starts
#define TEMPER_SET_SUITE_START_CALLBACK( callback, userdata ) \
	do { \
		g_testContext.suiteFuncStart = callback; \
		g_testContext.suiteFuncStartData = userdata; \
	} while ( 0 )

// set the function that will get called just after a test suite ends and before console output
#define TEMPER_SET_SUITE_END_CALLBACK( callback, userdata ) \
	do { \
		g_testContext.suiteFuncEnd = callback; \
		g_testContext.suiteFuncEndData = userdata; \
	} while ( 0 )

// set the unit of measurement that Temper will use when recording how long a test takes
#define TEMPER_SET_TIME_UNIT( unit ) \
	do { \
		g_testContext.timeUnit = unit; \
\
		switch ( unit ) { \
			case TEMPER_TIME_UNIT_CLOCKS:	g_testContext.timeUnitStr = "clocks"; break; \
			case TEMPER_TIME_UNIT_NS:		g_testContext.timeUnitStr = "ns"; break; \
			case TEMPER_TIME_UNIT_US:		g_testContext.timeUnitStr = "us"; break; \
			case TEMPER_TIME_UNIT_MS:		g_testContext.timeUnitStr = "ms"; break; \
			case TEMPER_TIME_UNIT_SECONDS:	g_testContext.timeUnitStr = "seconds"; break; \
			\
			default: \
				printf( "ERROR: Unknown time unit specified at %s:%d.\n", __FILE__, __LINE__ ); \
				assert( false ); \
				exit( EXIT_FAILURE ); \
		} \
	} while ( 0 )

#define TEMPER_FAIL_TEST_INTERNAL( fmt ) \
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

// runs the test suite
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
		double start = TemperGetTimestampInternal(); \
		result = test(); /* run the test! */ \
		double end = TemperGetTimestampInternal(); \
\
		g_testContext.testTime = end - start; \
\
		if ( g_testContext.testFuncEnd ) { \
			g_testContext.testFuncEnd( g_testContext.testFuncEndData ); \
		} \
	} while ( 0 )

// runs the test
#define TEMPER_RUN_TEST( test ) \
	do { \
		if ( ( ( g_testContext.flags & TEMPER_FLAG_ABORT_ON_FAIL ) == 0 ) || g_testContext.numFailed == 0 ) { \
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
					printf( "  %s (%f %s).\n", #test, g_testContext.testTime, g_testContext.timeUnitStr ); \
					break; \
				} \
\
				case TEMPER_RESULT_FAILED: { \
					TemperSetTextColorInternal( TEMPER_COLOR_RED ); \
					printf( "	FAILED:" ); \
					TemperSetTextColorInternal( TEMPER_COLOR_YELLOW ); \
					if ( g_testContext.msg ) { \
						printf( "  %s: \"%s\" at %s:%d.\n", #test, g_testContext.msg, g_testContext.file, g_testContext.line ); \
					} else { \
						printf( "  %s: at %s:%d.\n", #test, g_testContext.file, g_testContext.line ); \
					} \
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

#define TEMPER_SKIP_TEST_INTERNAL( test, reasonMsg ) \
	do { \
		printf( "	SKIPPED: %s: \"%s\".\n", #test, reasonMsg ); \
		g_testContext.numSkipped++; \
	} while ( 0 )

// marks this test to be skipped
// the test will still be outputted to console, but it will not actually run
#define TEMPER_SKIP_TEST( test, reasonMsg ) \
	do { \
		if ( ( ( g_testContext.flags & TEMPER_FLAG_ABORT_ON_FAIL ) == 0 ) || g_testContext.numFailed == 0 ) { \
			if ( g_testContext.filteredTest ) { \
				if ( strcmp( g_testContext.filteredTest, #test ) == 0 ) { \
					TEMPER_SKIP_TEST_INTERNAL( test, reasonMsg ); \
				} \
			} else { \
				TEMPER_SKIP_TEST_INTERNAL( test, reasonMsg ); \
			} \
		} \
	} while ( 0 )

// fails the test if the condition is not true
#define TEMPER_EXPECT_TRUE( condition ) \
	do { \
		g_testContext.msg = NULL; \
		if ( !(condition) ) { \
			TEMPER_FAIL_TEST_INTERNAL( #condition ); \
		} \
	} while ( 0 )

// fails the test if the condition is not false
#define TEMPER_EXPECT_FALSE( condition ) \
	do { \
		g_testContext.msg = NULL; \
		if ( (condition) ) { \
			TEMPER_FAIL_TEST_INTERNAL( #condition ); \
		} \
	} while ( 0 )

// exit the test, telling temper that the test has passed
#define TEMPER_PASS() \
	do { \
		g_testContext.numPassed++; \
		return TEMPER_RESULT_PASSED; \
	} while ( 0 )

#define TEMPER_FAIL() \
	TEMPER_FAIL_TEST_INTERNAL( NULL )

#if defined( __linux__ ) || defined( __APPLE__ )
#pragma pop_macro( "_POSIX_C_SOURCE" )
#endif

#if defined( __clang__ )
#pragma clang diagnostic pop
#elif defined( __GNUC__ )
#pragma GCC diagnostic pop
#elif defined( _MSC_VER )
#pragma warning( pop )
#endif // defined( __clang__ )

#ifdef __cplusplus
}
#endif
