#include "neon_test.h"

#include <math.h>

inline bool floateq( const float a, const float b ) {
	return fabs( a - b ) < 1e-5;
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
	NE_TEST_SKIP_TEST( XShouldEqual3, "Flaky on various people's machines right now..." );
}

int main( int argc, char** argv ) {
	( (void) argc );
	( (void) argv );

	NE_TEST_INIT();

	NE_RUN_TEST_SUITE( TheSuite );

	NE_TEST_SHUTDOWN();

	return NE_TEST_EXIT_CODE();
}