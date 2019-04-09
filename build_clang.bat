@echo off

set build_file=%1
set source_files=%~2

set ignore_warnings=-Wno-missing-prototypes -Wno-newline-eof
set compile_options=-Wall -Wextra -Werror -Wpedantic -Weverything -O3 -ffast-math

clang -o build/%build_file% %source_files% %compile_options% %ignore_warnings%