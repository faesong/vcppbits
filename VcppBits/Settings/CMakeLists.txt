add_library(VcppBits-Settings OBJECT Settings.cpp Setting.cpp)

target_link_libraries(VcppBits-Settings VcppBits-StringUtils)
target_link_libraries(VcppBits-Settings VcppBits-KeyFile)

include("../VcppBitsBuildsystemUtils.cmake")

vcppbits_include_toplevel_dir(Settings PUBLIC)
