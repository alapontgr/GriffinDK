echo Compiling x32 libraries (Debug, Release)
call compile_engine Win32 Debug ../FenixDK/FenixEngine.sln
call compile_engine Win32 Release ../FenixDK/FenixEngine.sln

echo Compiling x64 libraries (Debug, Release)
call compile_engine x64 Debug ../FenixDK/FenixEngine.sln
call compile_engine x64 Release ../FenixDK/FenixEngine.sln

pause