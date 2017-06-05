REM load visual studio env vars.
call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat"

REM Multithreaded build
REM /maxcpucount:8 /v:m

REM x32 configurations.
msbuild.exe ../FenixDK/FenixEngine.sln /verbosity:minimal /m /p:Platform=Win32 /p:Configuration=Debug
msbuild.exe ../FenixDK/FenixEngine.sln /verbosity:minimal /m /p:Platform=Win32 /p:Configuration=Release