#!/bin/bash

mkdir build
cd build

do_build () {
	compiler=$1
	output_filename=$2
	source_files="${@:3}"

	if [ $compiler == clang* ]; then
		compile_options="-Wall -Wextra -Werror -Wpedantic -Weverything -O3 -ffast-math"
	else
		compile_options="-Wall -Wextra -Werror -O3 -ffast-math"
	fi

	if [ $compiler == *++ ]; then
		# travis doesn't like this one if language "cpp" is selected
		ignore_warnings=-Wno-missing-prototypes
	else
		ignore_warnings=""
	fi

	$compiler -o ${output_filename}_${compiler}.exe ${source_files} ${compile_options} ${ignore_warnings}
}

# clang
do_build clang example_basic ../examples/example_basic.c
do_build clang example_suite "../examples/example_suite.c ../examples/sample_suite.c"
do_build clang++ example_cpp "../examples/example_cpp.cpp ../examples/sample_suite_cpp.cpp"

# gcc
do_build gcc example_basic ../examples/example_basic.c
do_build gcc example_suite "../examples/example_suite.c ../examples/sample_suite.c"
do_build g++ example_cpp "../examples/example_cpp.cpp ../examples/sample_suite_cpp.cpp"