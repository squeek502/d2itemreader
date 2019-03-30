if (ENABLE_FUZZING)
  enable_language(CXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")

  add_custom_target(fuzz_check_all)

  if (FUZZ_ENTRY)
    add_definitions(-DFUZZ_ENTRY=${FUZZ_ENTRY})
  endif()

  function(add_fuzz name dict corpus)
    if (COVERAGE)
      add_executable(${name} "${name}.cc" StandaloneFuzzTargetMain.c)
      target_link_libraries(${name} d2itemreader ${COVERAGE_LINKER_FLAGS})
      add_custom_target(${name}_cov
        COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${name}" "${corpus}/*"
        DEPENDS ${name}
      )
    else()
      add_executable(${name} "${name}.cc")
      target_link_libraries(${name} d2itemreader pthread ${FUZZ_ENGINE})
      add_custom_target(${name}_run COMMAND ${CMAKE_COMMAND} -E env "ASAN_OPTIONS=allocator_may_return_null=1" "${CMAKE_CURRENT_BINARY_DIR}/${name}" -timeout=2 -dict="${dict}" "${corpus}" DEPENDS ${name})
      set(${name}_new_corpus "${CMAKE_CURRENT_BINARY_DIR}/corpus/${name}")
      add_custom_target(${name}_minimize
        COMMAND mkdir -p "${${name}_new_corpus}"
        COMMAND ${name} -timeout=2 "${${name}_new_corpus}" "${corpus}" -merge=1
        COMMAND rm -rf "${corpus}"
        COMMAND mv "${${name}_new_corpus}" "${corpus}"
      )
      # for checking that the corpus still runs without exploding
      add_custom_target(${name}_check COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${name}" -timeout=2 "${corpus}/*" DEPENDS ${name})
      add_dependencies(fuzz_check_all ${name}_check)
    endif()
  endfunction()

  function(add_fuzz_simple name dict)
    add_fuzz(${name} "${CMAKE_CURRENT_SOURCE_DIR}/dict/${dict}" "${CMAKE_CURRENT_SOURCE_DIR}/corpus/${name}")
  endfunction()

  add_fuzz_simple(fuzz_d2char d2char.dict)
  add_fuzz_simple(fuzz_d2atmastash d2atmastash.dict)
  add_fuzz_simple(fuzz_d2personalstash d2personalstash.dict)
  add_fuzz_simple(fuzz_d2sharedstash d2sharedstash.dict)
  add_fuzz_simple(fuzz_d2txt d2txt.dict)

  # fuzz_d2itemreader uses any format, so generate corpus/dict from others first
  # TODO support for unique-to-fuzz_d2itemreader test cases (minimize/corpus)
  set(fuzz_d2itemreader_dict "${CMAKE_CURRENT_BINARY_DIR}/dict/d2itemreader.dict")
  set(fuzz_d2itemreader_corpus "${CMAKE_CURRENT_BINARY_DIR}/corpus/fuzz_d2itemreader_gen")
  add_fuzz(fuzz_d2itemreader ${fuzz_d2itemreader_dict} ${fuzz_d2itemreader_corpus})
  add_custom_target(fuzz_d2itemreader_gen_corpus
    COMMAND mkdir -p "${fuzz_d2itemreader_corpus}"
    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/corpus/fuzz_d2char/*" "${fuzz_d2itemreader_corpus}"
    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/corpus/fuzz_d2atmastash/*" "${fuzz_d2itemreader_corpus}"
    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/corpus/fuzz_d2personalstash/*" "${fuzz_d2itemreader_corpus}"
    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/corpus/fuzz_d2sharedstash/*" "${fuzz_d2itemreader_corpus}"
  )
  get_filename_component(fuzz_d2itemreader_dict_dir "${fuzz_d2itemreader_dict}" DIRECTORY)
  add_custom_target(fuzz_d2itemreader_gen_dict
    COMMAND mkdir -p "${fuzz_d2itemreader_dict_dir}"
    COMMAND cat "${CMAKE_CURRENT_SOURCE_DIR}/dict/d2char.dict" > "${fuzz_d2itemreader_dict}"
    COMMAND cat "${CMAKE_CURRENT_SOURCE_DIR}/dict/d2atmastash.dict" >> "${fuzz_d2itemreader_dict}"
    COMMAND cat "${CMAKE_CURRENT_SOURCE_DIR}/dict/d2personalstash.dict" >> "${fuzz_d2itemreader_dict}"
    COMMAND cat "${CMAKE_CURRENT_SOURCE_DIR}/dict/d2sharedstash.dict" >> "${fuzz_d2itemreader_dict}"
  )
  add_dependencies(fuzz_d2itemreader fuzz_d2itemreader_gen_dict fuzz_d2itemreader_gen_corpus)
endif()
