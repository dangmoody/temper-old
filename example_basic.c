#include <stdio.h>

#include "neon_test.h"

NE_TEST( XShouldEqualString0 ) {
	const char* msg = "This is a string";
	NE_TEST_EXPECT_TRUE( strcmp( msg, "This is a string" ) == 0 );

	NE_TEST_PASS();
}

NE_TEST( XShouldEqualString1 ) {
	const char* msg = "This is a string";
	NE_TEST_EXPECT_TRUE( strcmp( msg, "THiS iS a STRinG" ) == 0 );

	NE_TEST_PASS();
}

NE_TEST( XShouldEqualString2 ) {
	const char* msg = "This is a string";
	NE_TEST_EXPECT_TRUE( strcmp( msg, "hoolaba)" ) == 0 );

	NE_TEST_PASS();
}

int main( int argc, char** argv ) {
	( (void) argc );
	( (void) argv );

	NE_TEST_INIT();

	NE_TEST_RUN_TEST( XShouldEqualString0 );
	NE_TEST_RUN_TEST( XShouldEqualString1 );
	NE_TEST_SKIP_TEST( XShouldEqualString2, "Test skipping test not in a suite." );

	NE_TEST_SHUTDOWN();

	return NE_TEST_EXIT_CODE();
}