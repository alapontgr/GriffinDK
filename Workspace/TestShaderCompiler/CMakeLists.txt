file(GLOB_RECURSE Srcs  
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.*)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Srcs})

# Create library
add_executable(TestShaderCompiler ${Srcs})

# Add include dirs
set(includeDirs
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/Engine/Src
    ${PROJECT_SOURCE_DIR}/Engine/External
)
target_include_directories(TestShaderCompiler PUBLIC ${includeDirs})

# Link against engine
target_link_libraries(TestShaderCompiler ${engineName})