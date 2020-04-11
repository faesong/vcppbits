add_library(VcppBits-Settings2 INTERFACE)

#TODO2: is this right? (linking to header-only lib)
target_link_libraries(VcppBits-Settings2 VcppBits-StringUtils)
target_link_libraries(VcppBits-Settings2 VcppBits-KeyFile)

include("../VcppBitsBuildsystemUtils.cmake")

vcppbits_include_toplevel_dir(Settings2 INTERFACE)

