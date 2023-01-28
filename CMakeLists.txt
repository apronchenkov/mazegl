cmake_minimum_required(VERSION 3.24)
project(mazegl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(algorithm INTERFACE algorithm/Matrix.h)

add_library(maze maze/Maze.h maze/Maze.cpp)
add_dependencies(maze algorithm)

add_library(palettes palettes/Palettes.h palettes/Palettes.cpp)

add_executable(mazegl game/main.cpp game/Game.cpp game/Game.h game/KeyStateMachine.h game/Glyph.h game/Glyph.cpp game/SceneView.h game/SceneView.cpp)
add_dependencies(mazegl maze algorithm palettes)
target_link_libraries(mazegl glfw OpenGL::GL maze algorithm palettes)
