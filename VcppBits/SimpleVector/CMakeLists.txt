add_library(VcppBits-SimpleVector OBJECT SimpleVector.cpp)

include("../VcppBitsBuildsystemUtils.cmake")

vcppbits_include_toplevel_dir(SimpleVector PUBLIC)
