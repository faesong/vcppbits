function (add_bit)
  set(options "")
  set(oneValueArgs TARGET BITS_DIR LIBNAME_PREFIX)
  set(multiValueArgs BITS)
  cmake_parse_arguments(ADD_BIT "${options}" "${oneValueArgs}"
    "${multiValueArgs}" ${ARGN} )

  foreach (BIT ${ADD_BIT_BITS})
    add_subdirectory(${ADD_BIT_BITS_DIR}/${BIT} ${BIT})
    set(LIB_NAME ${ADD_BIT_LIBNAME_PREFIX}-${BIT})

    if (DEFINED ADD_BIT_TARGET)
      target_link_libraries(${ADD_BIT_TARGET} ${LIB_NAME})
      get_property(T TARGET ${LIB_NAME} PROPERTY TYPE)
      if (NOT ${T} STREQUAL INTERFACE_LIBRARY)
        target_compile_options(${LIB_NAME} PRIVATE ${ADDITIONAL_CXX_FLAGS})
      endif ()
    endif()

  endforeach ()
endfunction ()
