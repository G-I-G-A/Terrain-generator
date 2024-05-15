# MathLibrary/CMakeLists.txt

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hxx")
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

add_library(MathLibrary STATIC)

include(${CMAKE_SOURCE_DIR}/Common.cmake)
configure_target(MathLibrary)