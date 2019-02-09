#include "neon_test.h"

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

NE_TEST( XShouldEqual0 ) {
	float x = 0.0f;
	NE_TEST_EXPECT_TRUE( floateq( x, 0.0f ) );

	NE_TEST_PASS();
}

NE_TEST( XShouldEqual1 ) {
	float x = 0.0f;
	NE_TEST_EXPECT_TRUE( floateq( x, 1.0f ) );

	NE_TEST_PASS();
}

NE_TEST( XShouldEqual2 ) {
	float x = 2.0f;
	NE_TEST_EXPECT_TRUE( floateq( x, 2.0f ) );

	NE_TEST_PASS();
}

NE_TEST( XShouldEqual3 ) {
	float x = 2.0f;
	NE_TEST_EXPECT_TRUE( floateq( x, 2.0f ) );

	NE_TEST_PASS();
}

NE_TEST_SUITE( TheSuite ) {
	NE_TEST_RUN_TEST( XShouldEqual0 );
	NE_TEST_RUN_TEST( XShouldEqual1 );
	NE_TEST_RUN_TEST( XShouldEqual2 );
	NE_TEST_SKIP_TEST( XShouldEqual3, "This is a \"skipping test\" test." );
}

int main( int argc, char** argv ) {
	( (void) argc );
	( (void) argv );

	NE_TEST_INIT();

	// comment these out for better/stock looking output
	// this is just to demo the callbacks
	NE_TEST_SET_SUITE_START_CALLBACK( OnSuiteStarting, NULL );
	NE_TEST_SET_SUITE_END_CALLBACK( OnSuiteEnded, NULL );

	NE_TEST_SET_TEST_START_CALLBACK( OnTestStarting, NULL );
	NE_TEST_SET_TEST_END_CALLBACK( OnTestEnded, NULL );

	NE_TEST_RUN_SUITE( TheSuite );

	NE_TEST_SHUTDOWN();

	return NE_TEST_EXIT_CODE();
}