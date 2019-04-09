#!/bin/bash

echo Building linux.

echo Building examples on clang...
source build_clang.sh example_basic.exe "examples/example_basic.c"
source build_clang.sh example_suite.exe "examples/example_suite.c examples/sample_suite.c"
source build_clang.sh example_basic.exe "examples/example_cpp.cpp examples/sample_suite_cpp.cpp"
echo Done.
echo.

echo Building examples on GCC...
source build_gcc.sh example_basic.exe "examples/example_basic.c"
source build_gcc.sh example_suite.exe "examples/example_suite.c examples/sample_suite.c"
source build_gcc.sh example_basic.exe "examples/example_cpp.cpp examples/sample_suite_cpp.cpp"
echo Done.
echo.