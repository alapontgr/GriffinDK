::x86
call ./scripts/compile_engine.bat Win32 Debug ./FenixDK/FenixEngine.sln
call ./scripts/compile_engine.bat Win32 Release ./FenixDK/FenixEngine.sln
:: x64
call ./scripts/compile_engine.bat x64 Debug ./FenixDK/FenixEngine.sln
call ./scripts/compile_engine.bat x64 Release ./FenixDK/FenixEngine.sln
pause