project(d2itemreader-tests C)

file(GLOB TESTS_SRC "*.c")
foreach(test_case ${TESTS_SRC})
  string(REGEX REPLACE ".+\\/(.+)\\.c" "\\1" test_case_name ${test_case})
  add_executable(${test_case_name} ${test_case})
  target_link_libraries(${test_case_name} PRIVATE d2itemreader)
  target_include_directories(${test_case_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  set_target_properties(${test_case_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME ${test_case_name} COMMAND $<TARGET_FILE:${test_case_name}> WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()
