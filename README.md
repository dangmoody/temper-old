# neon test

A Single-header-only test framework, written in C99.

Based on [greatest](https://github.com/silentbicycle/greatest).  Go show the author some love.

Distributed under MIT License.  See LICENSE file for details.

Documentation is covered in this README and in the header comment in the main header file.


Another testing framework?
--------------------------
I like greatest, but there were just a couple of things that slightly bothered me.  So I decided to make one for me that fits my needs and I can maintain at my own free will.

I like to open-source most of the things I make in the hope it will be useful to someone else too.


Installation
------------
Download `neon_test.h` and include it.  Done!


Code Usage
----------
neon test requires init and shutdown calls.  These can be done anywhere,
but main() is recommended.  neon test also tracks the return code:
```
int main( int argc, char** argv ) {
	NE_TEST_INIT();

	// do your tests...

	NE_TEST_SHUTDOWN();

	return NE_TEST_EXIT_CODE();
}
```

To create a test:
```
NE_TEST( XShouldEqual0 ) {
	float x = 0.0f;
	NE_TEST_EXPECT_TRUE( floateq( x, 0.0f ) );

	NE_TEST_PASS();
}
```

The following assert-style macros are given; they do what you'd expect:
* `NE_TEST_EXPECT_TRUE( condition );`
* `NE_TEST_EXPECT_FALSE( condition );`

The following macros are given for return results of a test; they do what
you'd expect:
* `NE_TEST_PASS();`
* `NE_TEST_FAIL();`

In order to make a test suite that runs a series of tests:

```
NE_TEST_SUITE( TheSuite ) {
	NE_TEST_RUN_TEST( XShouldEqual0 );
	NE_TEST_RUN_TEST( SomeOtherTest );
	NE_TEST_RUN_TEST( SomeOtherOtherTest );
}
```

Tests can be run in and outside a test-suite, just like greatest.

If you want to skip a test:

```
NE_TEST_SKIP_TEST( FlakyTest, "TeamCity doesn't like this test for some reason..." );
```

neon test will then mark the test as skipped and display the reason
message in the console, for example:

```
SKIPPED: FlakyTest: "TeamCity doesn't like this test for some reason...".
```

You can also specify callbacks to run before and after each test and suite
is run:

```
// per suite
NE_TEST_SET_SUITE_START_CALLBACK( OnSuiteStarting, userdata );
NE_TEST_SET_SUITE_END_CALLBACK( OnSuiteEnded, userdata );

// per test
NE_TEST_SET_TEST_START_CALLBAK( OnTestStarting, userdata );
NE_TEST_SET_TEST_END_CALLBAK( OnTestEnded, userdata );
```

The start callback gets called before just the test/suite runs and the end
callback gets called just as the test/suite has finished running, before the
console output.  Both callbacks return void and take `void*` as arguments so
you may pass through them whatever you like.

Command Line Usage
------------------
neon test supports a few command line options:

```
--help		: Shows the help in console.
-t <name>	: Only run the test with the given name.
-s <suite>	: Only run the suite with the given name.
-a			: Abort immediately on test failure.
-c			: Enable colored output.
```