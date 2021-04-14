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

# Sync Git submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

# Link against Gfx API
macro(SetGfxSdk target)
    if(UseVulkan)
        target_compile_definitions(${target} PUBLIC VK_PROTOTYPES VK_USE_PLATFORM_WIN32_KHR _GFX_API=Vulkan)
        target_include_directories(${target} PUBLIC $ENV{VK_SDK_PATH}/Include)
        target_link_directories(${target} PUBLIC $ENV{VK_SDK_PATH}/Lib)
        target_link_libraries(${target} vulkan-1)
    endif()
endmacro()

# Add subdirectory of submodule (performs some validation checks)
macro(AddSubmodule submodulePath)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${submodulePath})
        message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
    endif()    
    add_subdirectory(${submodulePath})
endmacro()


##########################
# Setup Engine and tests
##########################

# Build Engine static lib
add_subdirectory(Engine)

if (AddTests)
    add_subdirectory(Workspace)
endif()