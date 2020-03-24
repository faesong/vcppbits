add_library(VcppBits-KeyFile OBJECT KeyFile.cpp)
target_link_libraries(VcppBits-KeyFile VcppBits-StringUtils)

include("../VcppBitsBuildsystemUtils.cmake")

vcppbits_include_toplevel_dir(KeyFile PUBLIC)

