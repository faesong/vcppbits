add_library(VcppBits-StateManager OBJECT StateManager.cpp)

include("../VcppBitsBuildsystemUtils.cmake")

vcppbits_include_toplevel_dir(StateManager PUBLIC)
