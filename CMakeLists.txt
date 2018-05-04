cmake_minimum_required (VERSION 2.6)
project (d2grailcheck C)

include_directories(${PROJECT_SOURCE_DIR})

# Our Module
add_executable( d2grailcheck d2grailcheck.c d2txtreader.c bitreader.c d2itemreader.c d2data.c util.c )
add_executable( test_bitreader test/test_bitreader.c bitreader.c )
