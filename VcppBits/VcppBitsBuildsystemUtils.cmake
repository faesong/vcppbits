
# parameter 1: name of the sub-target to add includes to
# parameter 2: INTERFACE, PUBLIC, PRIVATE scope...
macro(vcppbits_include_toplevel_dir)

  get_filename_component(
    VcppBits_TOPLEVEL_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    ABSOLUTE
    )

  if (EXISTS "${VcppBits_TOPLEVEL_DIR}")
    target_include_directories(
      "VcppBits-${ARGV0}"
      ${ARGV1} "${VcppBits_TOPLEVEL_DIR}")
    message("target_include_directories (VcppBits-${ARGV0}  INTERFACE ${VcppBits_TOPLEVEL_DIR})")
  else ()
    message(FATAL_ERROR
      "\
${ARGV0} directory must be within VcppBits/ for all ${ARGV0}'s includes to work\
properly. "
      )
  endif ()
endmacro()
