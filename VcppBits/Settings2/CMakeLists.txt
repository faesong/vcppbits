add_library(VcppBits-Settings2 INTERFACE)

target_link_libraries(VcppBits-Settings2 INTERFACE VcppBits-StringUtils)
target_link_libraries(VcppBits-Settings2 INTERFACE VcppBits-KeyFile)

include("../VcppBitsBuildsystemUtils.cmake")

vcppbits_include_toplevel_dir(Settings2 INTERFACE)

