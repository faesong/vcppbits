add_library(VcppBits-TestsDriver OBJECT TestsDriver.cpp)

include("../VcppBitsBuildsystemUtils.cmake")

vcppbits_include_toplevel_dir(TestsDriver PUBLIC)

