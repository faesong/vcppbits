add_library(VcppBits-Translation OBJECT Translation.cpp)
target_link_libraries(VcppBits-Translation VcppBits-StringUtils)

include("../VcppBitsBuildsystemUtils.cmake")

vcppbits_include_toplevel_dir(Translation PUBLIC)

configure_file(Ids.hpp.in Ids.hpp)

file(STRINGS "${TRANSLATION_IDS_FILE}" STRING_IDS)

#populate translation boilerplate...
set(TR_STRING_IDS "")
set(TR_ENGLISH "")
foreach(S ${STRING_IDS})
  string(SUBSTRING ${S} 0 2 FIRST_TWO_SYMBOLS)
  if(NOT ${FIRST_TWO_SYMBOLS} STREQUAL "//")

    #string(FIND <string> <substring> <output_variable> [REVERSE])
    string(FIND ${S} "namespace " ns_decl_found)
    string(FIND ${S} "enum " enum_decl_found)
    string(FIND ${S} "}" close_brace_found)

    if (${ns_decl_found} EQUAL 0
        OR ${enum_decl_found} EQUAL 0
        OR ${close_brace_found} EQUAL 0)
    else ()
      string(FIND ${S} " " S_SPACE_POS)

      math(EXPR S_STRIPPED_LEN_MINUS_ONE "${S_SPACE_POS} - 1")
      string(SUBSTRING ${S} 0 ${S_STRIPPED_LEN_MINUS_ONE} S_STRIPPED)

      string(FIND ${S} "//" DOUBLESLASH_POS)
      math(EXPR V_BEGIN "${DOUBLESLASH_POS} + 2")
      string(SUBSTRING ${S} ${V_BEGIN} -1 VAL)
      string(STRIP "${VAL}" S_VAL)

      #message ("name: \"${S_STRIPPED}\" , value: \"${S_VAL}\"")
      string(APPEND TR_STRING_IDS "\"${S_STRIPPED}\",\n")
      string(APPEND TR_ENGLISH "{ Ids::${S_STRIPPED}, \"${S_VAL}\" },\n")
    endif ()
  endif()
endforeach()

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/TranslationIdsStrings.hpp "${TR_STRING_IDS}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/TranslationEnglishDefault.hpp "${TR_ENGLISH}")

target_include_directories(VcppBits-Translation PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
