@echo off

set build_file=%1

set ignore_warnings=-Wno-missing-prototypes -Wno-newline-eof

clang -o build/%build_file%.exe examples/%build_file%.c examples/sample_suite.c -Wall -Wextra -Werror -Wpedantic -Weverything -O3 -ffast-math %ignore_warnings%