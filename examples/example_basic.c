#include "../temper.h"

#include <stdio.h>

TEMPER_TEST( XShouldEqualString0 ) {
	const char* msg = "This is a string";
	TEMPER_EXPECT_TRUE( strcmp( msg, "THiS iS a STRinG" ) == 0 );

	TEMPER_PASS();
}

TEMPER_TEST( XShouldEqualString1 ) {
	const char* msg = "This is a string";
	TEMPER_EXPECT_TRUE( strcmp( msg, "hoolaba)" ) == 0 );

	TEMPER_PASS();
}

TEMPER_TEST( XShouldEqualString2 ) {
	const char* msg = "This is a string";
	TEMPER_EXPECT_TRUE( strcmp( msg, "This is a string" ) == 0 );

	TEMPER_PASS();
}

int main( int argc, char** argv ) {
	TEMPER_INIT();

	TEMPER_SET_COMMAND_LINE_ARGS( argc, argv );

	TEMPER_RUN_TEST( XShouldEqualString0 );
	TEMPER_SKIP_TEST( XShouldEqualString1, "Test skipping test not in a suite." );
	TEMPER_RUN_TEST( XShouldEqualString2 );

	TEMPER_SHUTDOWN();

	return TEMPER_EXIT_CODE();
}