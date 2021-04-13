cmake_minimum_required(VERSION 3.18.2)

set(engineName Griffin)

project(GriffinEngine)

set(GfxApi "Vulkan" CACHE STRING "The graphics API Select Gfx backend {Vulkan}")
OPTION(AddTests "Add test projects" ON)

if(${GfxApi} STREQUAL "Vulkan")
	set(UseVulkan TRUE)
else()
    message(FATAL_ERROR "Invalid graphics backend")
endif()

# Some useful macros

macro(SetGfxSdk target)
    if(UseVulkan)
        target_compile_definitions(${target} PUBLIC VK_PROTOTYPES VK_USE_PLATFORM_WIN32_KHR _GFX_API=Vulkan)
        target_include_directories(${target} PUBLIC $ENV{VK_SDK_PATH}/Include)
        target_link_directories(${target} PUBLIC $ENV{VK_SDK_PATH}/Lib)
        target_link_libraries(${target} vulkan-1)
    endif()
endmacro()

# Build Engine static lib
add_subdirectory(Engine)

if (AddTests)
    add_subdirectory(Workspace)
endif()