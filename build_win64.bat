@echo off

echo Building windows.

echo Building examples on clang...
call build_clang.bat example_basic_clang.exe "examples/example_basic.c"
call build_clang.bat example_suite_clang.exe "examples/example_suite.c examples/sample_suite.c"
call build_clang.bat example_cpp_clang.exe "examples/example_cpp.cpp examples/sample_suite_cpp.cpp"
echo Done.
echo.

echo Building examples on GCC...
call build_gcc.bat example_basic_gcc.exe "examples/example_basic.c"
call build_gcc.bat example_suite_gcc.exe "examples/example_suite.c examples/sample_suite.c"
call build_gcc.bat example_cpp_gcc.exe "examples/example_cpp.cpp examples/sample_suite_cpp.cpp"
echo Done.
echo.