#include "../temper.h"

#include <math.h>

inline bool floateq( const float a, const float b ) {
	return fabsf( a - b ) < 1e-5f;
}

void OnSuiteStarting( void* data ) {
	( (void) data );

	printf( "About to run a test suite.\n" );
}

void OnSuiteEnded( void* data ) {
	( (void) data );

	printf( "Test suite finished.\n" );
}

void OnTestStarting( void* data ) {
	( (void) data );

	printf( "Test starting.\n" );
}

void OnTestEnded( void* data ) {
	( (void) data );

	printf( "Test finished.\n" );
}

TEMPER_TEST( XShouldEqual0 ) {
	float x = 0.0f;
	TEMPER_EXPECT_TRUE( floateq( x, 0.0f ) );

	TEMPER_PASS();
}

TEMPER_TEST( XShouldEqual1 ) {
	float x = 0.0f;
	TEMPER_EXPECT_TRUE( floateq( x, 1.0f ) );

	TEMPER_PASS();
}

TEMPER_TEST( XShouldEqual2 ) {
	float x = 2.0f;
	TEMPER_EXPECT_TRUE( floateq( x, 2.0f ) );

	TEMPER_PASS();
}

TEMPER_TEST( XShouldEqual3 ) {
	float x = 2.0f;
	TEMPER_EXPECT_TRUE( floateq( x, 2.0f ) );

	TEMPER_PASS();
}

TEMPER_SUITE( TheSuite ) {
	TEMPER_RUN_TEST( XShouldEqual0 );
	TEMPER_RUN_TEST( XShouldEqual1 );
	TEMPER_RUN_TEST( XShouldEqual2 );
	TEMPER_SKIP_TEST( XShouldEqual3, "This is a \"skipping test\" test." );
}

int main( int argc, char** argv ) {
	TEMPER_INIT();

	TEMPER_SET_COMMAND_LINE_ARGS( argc, argv );

	// comment these out for better/stock looking output
	// this is just to demo the callbacks
	TEMPER_SET_SUITE_START_CALLBACK( OnSuiteStarting, NULL );
	TEMPER_SET_SUITE_END_CALLBACK( OnSuiteEnded, NULL );

	TEMPER_SET_TEST_START_CALLBACK( OnTestStarting, NULL );
	TEMPER_SET_TEST_END_CALLBACK( OnTestEnded, NULL );

	TEMPER_RUN_SUITE( TheSuite );

	TEMPER_SHUTDOWN();

	return TEMPER_EXIT_CODE();
}