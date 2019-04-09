#include "../temper.h"

#include <stdio.h>

TEMPER_SUITE_EXTERN( TheSuite );

TEMPER_DEFS();

int main( int argc, char** argv ) {
	TEMPER_SET_COMMAND_LINE_ARGS( argc, argv );

	TEMPER_RUN_SUITE( TheSuite );

	TEMPER_SHOW_STATS();

	return TEMPER_EXIT_CODE();
}
