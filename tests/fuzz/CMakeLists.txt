if (ENABLE_FUZZING)
  enable_language(CXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")

  add_custom_target(fuzz_check_all)

  if (FUZZ_ENTRY)
    add_definitions(-DFUZZ_ENTRY=${FUZZ_ENTRY})
  endif()

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
      target_link_libraries(${name} d2itemreader pthread ${FUZZ_ENGINE})
      set(${name}_corpus "${CMAKE_CURRENT_SOURCE_DIR}/corpus/${name}")
      add_custom_target(${name}_run COMMAND ${CMAKE_COMMAND} -E env "ASAN_OPTIONS=allocator_may_return_null=1" "${CMAKE_CURRENT_BINARY_DIR}/${name}" -timeout=2 -dict="${CMAKE_CURRENT_SOURCE_DIR}/dict/${dict}" "${${name}_corpus}" DEPENDS ${name})
      set(${name}_new_corpus "${CMAKE_CURRENT_BINARY_DIR}/corpus/${name}")
      add_custom_target(${name}_minimize
        COMMAND mkdir -p "${${name}_new_corpus}"
        COMMAND ${name} -timeout=2 "${${name}_new_corpus}" "${${name}_corpus}" -merge=1
        COMMAND rm -rf "${${name}_corpus}"
        COMMAND mv "${${name}_new_corpus}" "${CMAKE_CURRENT_SOURCE_DIR}/corpus"
      )
      # for checking that the corpus still runs without exploding
      add_custom_target(${name}_check COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${name}" -timeout=2 "${${name}_corpus}/*" DEPENDS ${name})
      add_dependencies(fuzz_check_all ${name}_check)
    endif()
  endfunction()

  add_fuzz(fuzz_d2char d2char.dict)
  add_fuzz(fuzz_d2atmastash d2atmastash.dict)
  add_fuzz(fuzz_d2personalstash d2personalstash.dict)
  add_fuzz(fuzz_d2sharedstash d2sharedstash.dict)
  add_fuzz(fuzz_d2txt d2txt.dict)
endif()
