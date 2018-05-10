cmake_minimum_required (VERSION 2.6)
project (d2grailcheck C)

include_directories(src/)
file(GLOB HEADERS "src/*.h")
file(GLOB SRC "src/*.c")

add_executable( d2grailcheck ${SRC} ${HEADERS} )