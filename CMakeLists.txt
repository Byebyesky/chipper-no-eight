cmake_minimum_required(VERSION 3.7)
project(chipper8_project)

set( LIBS "SDL2")
set( CMAKE_C_FLAGS "-Wall" )

file( GLOB SRCS src/*.cpp src/*.h src/*.hpp )

add_executable( chipper8 ${SRCS} )
target_link_libraries( chipper8 ${LIBS} )

