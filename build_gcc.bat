@echo off

set build_file=%1
set source_files=%~2

set ignore_warnings=-Wno-missing-prototypes
set compile_options=-Wall -Wextra -Werror -O3 -ffast-math

gcc -o build/%build_file% examples/%source_files% %compile_options% %ignore_warnings%