cmake_minimum_required (VERSION 2.6)
project (d2itemreader C)

include_directories(src/)
file(GLOB HEADERS "src/*.h")
file(GLOB LIBSRC "src/*.c")

add_library( d2itemreader ${LIBSRC} ${HEADERS} )
target_compile_definitions( d2itemreader PUBLIC _CRT_SECURE_NO_WARNINGS )

add_executable( example_simple examples/simple.c )
target_link_libraries( example_simple PRIVATE d2itemreader )

enable_testing()

file(GLOB TESTS_SRC "tests/*.c")
foreach(test_case ${TESTS_SRC})
  string(REGEX REPLACE ".+\\/(.+)\\.c" "\\1" test_case_name ${test_case})
  add_executable(${test_case_name} ${test_case})
  target_link_libraries(${test_case_name} PRIVATE d2itemreader)
  target_include_directories(${test_case_name} PRIVATE tests)
  add_test(NAME ${test_case_name} COMMAND $<TARGET_FILE:${test_case_name}> WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
endforeach()
