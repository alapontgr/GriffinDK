call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat"

REM BUILD VULKAN LIBRARY

cd C:\VulkanSDK\1.0.51.0\glslang
@RD /S /Q "./build"
mkdir build
cd build
cmake -G "Visual Studio 14 Win64" ..
msbuild.exe glslang.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Debug
msbuild.exe glslang.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Release

REM BUILD VULKAN TOOLS

cd C:\VulkanSDK\1.0.51.0\spirv-tools
mkdir build
cd build
cmake -G "Visual Studio 14 Win64" ..
msbuild.exe spirv-tools.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Debug
msbuild.exe spirv-tools.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Release

REM BUILD VULKAN SAMPLES

cd C:\VulkanSDK\1.0.51.0\Samples
mkdir build
cd build
cmake -G "Visual Studio 14 Win64" ..
msbuild.exe VULKAN_SAMPLES.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Debug
msbuild.exe VULKAN_SAMPLES.sln /verbosity:minimal /m /p:Platform=x64 /p:Configuration=Release

pause