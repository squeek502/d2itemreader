cmake_minimum_required (VERSION 2.6)
project (d2grailcheck C)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable( d2grailcheck src/d2grailcheck.c src/d2txtreader.c src/bitreader.c src/d2itemreader.c src/d2data.c src/util.c )
