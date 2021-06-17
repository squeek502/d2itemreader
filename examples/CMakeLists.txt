project(d2itemreader-examples C)

add_custom_target(examples)

file(GLOB EXAMPLES_SRC "*.c")
foreach(example_case ${EXAMPLES_SRC})
  string(REGEX REPLACE ".+\\/(.+)\\.c" "\\1" example_name ${example_case})
  set(example_name "example_${example_name}")
  add_executable(${example_name} EXCLUDE_FROM_ALL ${example_case})
  target_link_libraries(${example_name} PRIVATE d2itemreader)
  target_include_directories(${example_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  set_target_properties(${example_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_dependencies(examples ${example_name})
endforeach()
