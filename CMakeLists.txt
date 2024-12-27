cmake_minimum_required(VERSION 3.31)
project(mazegl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Lua REQUIRED)

include_directories(${LUA_INCLUDE_DIR})

add_library(algorithm
        INTERFACE
        algorithm/Matrix.h)

add_library(lua_bridge
        lua_bridge/Utils.h
        lua_bridge/Utils.cpp)
target_link_libraries(lua_bridge
        ${LUA_LIBRARY})

add_library(
        palettes
        palettes/Palettes.h
        palettes/Palettes.cpp)

add_library(maze
        maze/Maze.h
        maze/Maze.cpp)
add_dependencies(maze
        algorithm)

add_executable(mazegl
        game/Game.cpp
        game/Game.h
        game/GameMap.cpp
        game/GameMap.h
        game/GameMapProviders.cpp
        game/GameMapProviders.h
        game/Glyph.cpp
        game/Glyph.h
        game/SceneView.cpp
        game/SceneView.h
        game/main.cpp)

add_dependencies(mazegl
        maze
        algorithm
	lua_bridge
        palettes)
target_link_libraries(mazegl
        algorithm
	lua_bridge
        maze
        palettes
        glfw
        OpenGL::GL)
