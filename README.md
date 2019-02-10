# Temper

A Single-header-only test framework, written in C99.

Based on [greatest](https://github.com/silentbicycle/greatest).  Go show the author some love.

Distributed under MIT License.  See LICENSE file for details.

Documentation is covered in this README and in the main comment at the top of the header.


Another testing framework?
--------------------------
I like greatest, but there were just a couple of things that slightly bothered me.  So I decided to make one for me that fits my needs and I can maintain at my own free will.

I like to open-source most of the things I make in the hope it will be useful to someone else too.


Installation
------------
Download `temper.h` and include it.  Done!


Code Usage
----------
Temper requires init and shutdown calls.  These can be done anywhere,
but main() is recommended.  Temper also tracks the return code:
```C
int main( int argc, char** argv ) {
	TEMPER_INIT();

	// do your tests...

	TEMPER_SHUTDOWN();

	return TEMPER_EXIT_CODE();
}
```

To create a test:
```C
TEMPER_TEST( XShouldEqual0 ) {
	float x = 0.0f;
	TEMPER_EXPECT_TRUE( floateq( x, 0.0f ) );

	TEMPER_PASS();
}
```

The following assert-style macros are given; they do what you'd expect:
* ```TEMPER_EXPECT_TRUE( condition );```
* ```TEMPER_EXPECT_FALSE( condition );```

The following macros are given for return results of a test; they do what
you'd expect:
* ```TEMPER_PASS();```
* ```TEMPER_FAIL();```

In order to make a test suite that runs a series of tests:

```C
TEMPER_SUITE( TheSuite ) {
	TEMPER_RUN_TEST( XShouldEqual0 );
	TEMPER_RUN_TEST( SomeOtherTest );
	TEMPER_RUN_TEST( SomeOtherOtherTest );
}
```

Tests can be run in and outside a test-suite, just like greatest.

If you want to skip a test:

```C
TEMPER_SKIP_TEST( FlakyTest, "TeamCity doesn't like this test for some reason..." );
```

Temper will then mark the test as skipped and display the reason
message in the console, for example:

```C
SKIPPED: FlakyTest: "TeamCity doesn't like this test for some reason...".
```

You can also specify callbacks to run before and after each test and suite
is run:

```C
// per suite
TEMPER_SET_SUITE_START_CALLBACK( OnSuiteStarting, userdata );
TEMPER_SET_SUITE_END_CALLBACK( OnSuiteEnded, userdata );

// per test
TEMPER_SET_TEST_START_CALLBAK( OnTestStarting, userdata );
TEMPER_SET_TEST_END_CALLBAK( OnTestEnded, userdata );
```

The start callback gets called before just the test/suite runs and the end
callback gets called just as the test/suite has finished running, before the
console output.  Both callbacks return void and take `void*` as arguments so
you may pass through them whatever you like.

Any functions with "Internal" in the name means you probably shouldn't go
touching it.

Command Line Usage
------------------
Temper supports a few command line options:

```
--help     : Shows the help in console.
-t <name>  : Only run the test with the given name.
-s <suite> : Only run the suite with the given name.
-a         : Abort immediately on test failure.
-c         : Enable colored output.
```