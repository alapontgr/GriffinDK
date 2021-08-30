# Add some external libraries
set(ENABLE_HLSL OFF)
set(ENABLE_SPVREMAPPER OFF)
set(ENABLE_CTEST OFF)
set(BUILD_TESTING OFF)
AddSubmodule(External/glslang)

set(SPIRV_CROSS_CLI OFF)
set(SPIRV_CROSS_ENABLE_HLSL OFF)
set(SPIRV_CROSS_ENABLE_MSL OFF)
set(SPIRV_CROSS_ENABLE_CPP OFF)
set(SPIRV_CROSS_ENABLE_REFLECT OFF)
set(SPIRV_CROSS_ENABLE_C_API OFF)
set(SPIRV_CROSS_ENABLE_UTIL OFF)
AddSubmodule(External/spirv-cross)

set_target_properties (glslang PROPERTIES FOLDER glslang)
set_target_properties (glslangValidator PROPERTIES FOLDER glslang)
set_target_properties (spirv-cross-core PROPERTIES FOLDER spirv-cross)
set_target_properties (spirv-cross-glsl PROPERTIES FOLDER spirv-cross)

############################
## Setup Engine library
############################

#file(GLOB_RECURSE GF_Srcs ${CMAKE_CURRENT_SOURCE_DIR}/Src/Common)
file(GLOB_RECURSE GF_CommonSrcs  
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Common/*.*)
# Add srcs from external libraries
set(GF_ExternalSrcDirs
    glm
    vma
    stb
    json
)
set(GF_ExternalSrcs "")
foreach(externalDir ${GF_ExternalSrcDirs})
    file(GLOB_RECURSE tmp_list ${CMAKE_CURRENT_SOURCE_DIR}/External/${externalDir}/*.*)
    set(GF_ExternalSrcs ${GF_ExternalSrcs} ${tmp_list})
endforeach()

# Add platform files (TODO make this cross platform)
file(GLOB_RECURSE GF_PlatformSrcs  
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Win64/*.*) 
# Add Gfx files (TODO make this flexible with different Gfx APIs)
if(UseVulkan)
    file(GLOB_RECURSE GF_GfxSrcs  
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/Vulkan/*.*) 
endif()

# Set library files
set(GF_Srcs
    ${GF_CommonSrcs}
    ${GF_PlatformSrcs}
    ${GF_ExternalSrcs}
    ${GF_GfxSrcs}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GF_Srcs})

# Create library
add_library(${engineName} ${GF_Srcs})

# Add include dirs
set(GF_Includes 
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/External
)
target_include_directories(${engineName} PUBLIC ${GF_Includes})
target_link_libraries(${engineName} glslang SPIRV spirv-cross-glsl)
target_compile_definitions(${engineName} PUBLIC _PLATFORM_DIR=Win64)

# Setup Gfx dependencies
SetGfxSdk(${engineName})