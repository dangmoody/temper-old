#include "../temper.h"
#include <conio.h>
#include <math.h>
#include <stdio.h>

TEMPER_DEFS();

//#ifdef __cplusplus
//extern "C" {
//#endif
TEMPER_SUITE_EXTERN( TheSuite );
//#ifdef __cplusplus
//}
//#endif

int main( int argc, char** argv ) {
	( (void) argc );
	( (void) argv );

	TEMPER_RUN_SUITE( TheSuite );

	TEMPER_SHOW_STATS();

	getchar();

	return TEMPER_EXIT_CODE();
}