REM load visual studio env vars.
call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat"

REM Multithreaded build
REM /maxcpucount:8 /v:m

echo Compiling FenixEngine.sln Platform: %1, Configuration: %2

msbuild.exe %3 /verbosity:minimal /m /p:Platform=%1 /p:Configuration=%2