cmake_minimum_required (VERSION 2.6)
project (d2itemreader C)

include_directories(src/)
file(GLOB HEADERS "src/*.h")
file(GLOB LIBSRC "src/*.c")

add_library( d2itemreader ${LIBSRC} ${HEADERS} )
if (MSVC)
  target_compile_definitions( d2itemreader PUBLIC _CRT_SECURE_NO_WARNINGS )
endif()

if (("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"))
  set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} /W3 /sdl")
endif()

add_subdirectory(examples)
add_subdirectory(tools)

enable_testing()
add_subdirectory(tests)

option(ENABLE_FUZZING "Create executables and targets for fuzzing." Off)
option(COVERAGE "Enable code coverage tracking" Off)
if (COVERAGE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
  set(COVERAGE_LINKER_FLAGS  "-fprofile-instr-generate -fcoverage-mapping")
endif()
if (ENABLE_FUZZING)
  if(NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    message(FATAL_ERROR "Fuzzing requires the clang compiler")
  endif()

  set(CMAKE_BUILD_TYPE Debug)
  if (NOT COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,undefined -fno-sanitize=alignment -fsanitize-coverage=trace-pc-guard -fno-omit-frame-pointer")
  endif()

  add_subdirectory(tests/fuzz)
endif()
