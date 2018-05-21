if (ENABLE_FUZZING)
  if(NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    message(FATAL_ERROR "Fuzzing requires the clang compiler")
  endif()

  enable_language(CXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")

  set(fuzzer_dir ${CMAKE_CURRENT_BINARY_DIR}/fuzzer)
  add_custom_command(OUTPUT ${fuzzer_dir}
    COMMAND git clone https://chromium.googlesource.com/chromium/llvm-project/compiler-rt/lib/fuzzer
  )
  add_custom_target(fuzzer_src DEPENDS ${fuzzer_dir})

  set(fuzzer_static ${CMAKE_CURRENT_BINARY_DIR}/libFuzzer.a)
  add_custom_command(OUTPUT ${fuzzer_static} COMMAND "${fuzzer_dir}/build.sh")
  add_custom_target(libFuzzer DEPENDS ${fuzzer_static})
  add_dependencies(libFuzzer fuzzer_src)

  function(add_fuzz name dict)
    if (COVERAGE)
      add_executable(${name} "${name}.cc" StandaloneFuzzTargetMain.c)
      target_link_libraries(${name} d2itemreader ${COVERAGE_LINKER_FLAGS})
      add_custom_target(${name}_cov
        COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${name}" "${CMAKE_CURRENT_SOURCE_DIR}/corpus/${name}/*"
        DEPENDS ${name}
      )
    else()
      add_executable(${name} "${name}.cc")
      target_link_libraries(${name} d2itemreader ${fuzzer_static} pthread)
      set(${name}_corpus "${CMAKE_CURRENT_SOURCE_DIR}/corpus/${name}")
      add_custom_target(${name}_run COMMAND ${CMAKE_COMMAND} -E env "ASAN_OPTIONS=allocator_may_return_null=1" "${CMAKE_CURRENT_BINARY_DIR}/${name}" -timeout=2 -dict="${CMAKE_CURRENT_SOURCE_DIR}/dict/${dict}" "${${name}_corpus}" DEPENDS ${name})
      set(${name}_new_corpus "${CMAKE_CURRENT_BINARY_DIR}/corpus/${name}")
      add_custom_target(${name}_minimize
        COMMAND mkdir -p "${${name}_new_corpus}"
        COMMAND ${name} -timeout=2 "${${name}_new_corpus}" "${${name}_corpus}" -merge=1
        COMMAND rm -rf "${${name}_corpus}"
        COMMAND mv "${${name}_new_corpus}" "${CMAKE_CURRENT_SOURCE_DIR}/corpus"
      )
      add_dependencies(${name} libFuzzer)
    endif()
  endfunction()

  add_fuzz(fuzz_d2char d2char.dict)
  add_fuzz(fuzz_d2atmastash d2atmastash.dict)
  add_fuzz(fuzz_d2personalstash d2personalstash.dict)
  add_fuzz(fuzz_d2sharedstash d2sharedstash.dict)
endif()
