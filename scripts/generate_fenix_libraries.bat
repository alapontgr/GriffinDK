echo Compiling x32 libraries (Debug, Release)
call compile_engine Win32 Debug
call compile_engine Win32 Release

echo Compiling x64 libraries (Debug, Release)
call compile_engine x64 Debug
call compile_engine x64 Release

