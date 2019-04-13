#!/bin/bash

# can ebe either:
#	- clang
#	- clang++
#	- gcc
#	- g++
compiler=$1

mkdir build
cd build

ignore_warnings=-Wno-missing-prototypes

if [ compiler = "clang" ]; then
	compile_options="-Wall -Wextra -Werror -Wpedantic -Weverything -O3 -ffast-math"
else
	compile_options="-Wall -Wextra -Werror -O3 -ffast-math"
fi

echo Compiling for ${compiler}...

source_files=../examples/example_basic.c
$compiler -o build/example_basic_${compiler}.exe ${source_files} ${compile_options} ${ignore_warnings}

source_files=../examples/example_suite.c ../examples/sample_suite.c
$compiler -o build/example_suite_${compiler}.exe ${source_files} ${compile_options} ${ignore_warnings}

if [[ ${compiler} == ++ ]]; then
source_files=../examples/example_cpp.cpp ../examples/sample_suite_cpp.cpp
$compiler -o build/example_cpp_${compiler}.exe ${source_files} ${compile_options} ${ignore_warnings}
fi

echo Done...