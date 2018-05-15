cmake_minimum_required (VERSION 2.6)
project (d2itemreader C)

include_directories(src/)
file(GLOB HEADERS "src/*.h")
file(GLOB LIBSRC "src/*.c")

add_library( d2itemreader ${LIBSRC} ${HEADERS} )
target_compile_definitions( d2itemreader PUBLIC _CRT_SECURE_NO_WARNINGS )

add_subdirectory(examples)
add_subdirectory(tools)

enable_testing()
add_subdirectory(tests)
