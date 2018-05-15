project(d2itemreader-tools C)

add_executable( tool_gen_d2data_default EXCLUDE_FROM_ALL gen_d2data_default.c )
target_link_libraries( tool_gen_d2data_default PRIVATE d2itemreader )
set_target_properties(tool_gen_d2data_default PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(tool_gen_d2data_default_run tool_gen_d2data_default > src/d2data_default.inc DEPENDS tool_gen_d2data_default WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})