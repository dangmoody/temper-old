@echo off

set build_file=%1

set ignore_warnings=-Wno-missing-prototypes -Wno-newline-eof

gcc -o build/%build_file%.exe examples/%build_file%.c examples/sample_suite.c -Wall -Wextra -Werror -Wpedantic -O3 -ffast-math %ignore_warnings%