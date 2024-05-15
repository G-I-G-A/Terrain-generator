# TerrainGenerator/CMakeLists.txt

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hxx")
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

add_executable(TerrainGenerator)

include(${CMAKE_SOURCE_DIR}/Common.cmake)
configure_target(TerrainGenerator)

target_link_libraries(TerrainGenerator 
    PRIVATE
    MathLibrary
    unofficial::gl3w::gl3w
    glfw
    OpenGL::GL
    imgui::imgui
)