call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat"

REM BUILD VULKAN LIBRARY

cd %VK_SDK_PATH%\glslang
@RD /S /Q "./build"
mkdir build
cd build
cmake -G "Visual Studio 14 Win64" ..
msbuild.exe glslang.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Debug
msbuild.exe glslang.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Release

REM BUILD VULKAN TOOLS

cd %VK_SDK_PATH%\spirv-tools
mkdir build
cd build
cmake -G "Visual Studio 14 Win64" ..
msbuild.exe spirv-tools.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Debug
msbuild.exe spirv-tools.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Release

REM BUILD VULKAN SAMPLES

cd %VK_SDK_PATH%\Samples
mkdir build
cd build
cmake -G "Visual Studio 14 Win64" ..
msbuild.exe VULKAN_SAMPLES.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Debug
msbuild.exe VULKAN_SAMPLES.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Release

pause