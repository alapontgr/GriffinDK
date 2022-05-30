file(GLOB_RECURSE SOURCES *.cpp)

add_library(GeSpirvCross ${SOURCES})

target_include_directories(GeSpirvCross PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
