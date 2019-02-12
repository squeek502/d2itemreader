cmake_minimum_required(VERSION 3.0)

project (d2itemreader C)

# Quoted variables like "MSVC" will no longer be dereferenced when the policy is set to NEW
cmake_policy(SET CMP0054 NEW)

file(GLOB HEADERS "src/*.h")
file(GLOB LIBSRC "src/*.c")

add_library( d2itemreader ${LIBSRC} ${HEADERS} )
target_include_directories(d2itemreader PUBLIC src/)
set_target_properties(d2itemreader PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED true)
if (MSVC)
  target_compile_definitions( d2itemreader PUBLIC _CRT_SECURE_NO_WARNINGS )
endif()

if (("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"))
  set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-missing-field-initializers")
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
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address,undefined,fuzzer -fno-sanitize=alignment -fno-omit-frame-pointer")
  endif()

  add_subdirectory(tests/fuzz)
endif()
