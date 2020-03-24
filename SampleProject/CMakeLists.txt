project(VcppBitsSampleProject)
cmake_minimum_required(VERSION 3.14)


## helpful variable to make sure we have all our dependency directories
## specified:
set(DEPS_FOLDERS_SET True)

if (NOT DEFINED CPPBITS_FOLDER)
  message("CPPBITS_FOLDER is undefined!")
  set(DEPS_FOLDERS_SET False)
endif()

## can add more checks as you go:
#if (NOT DEFINED ANY_OTHER_VAR_THAT_YOU_NEED)
#  message("ANY_OTHER_VAR_THAT_YOU_NEED is undefined!")
#  set(DEPS_FOLDERS_SET False)
#endif()

if (NOT ${DEPS_FOLDERS_SET})
  message(FATAL_ERROR "required variables are undefined!")
endif()

add_executable(${CMAKE_PROJECT_NAME} SampleProject.cpp)

## we need KeyFile part, but StringUtils is a dependency of that, and has to be
## added manually
add_subdirectory(${CPPBITS_FOLDER}/VcppBits/StringUtils StringUtils)

add_subdirectory(${CPPBITS_FOLDER}/VcppBits/KeyFile KeyFile)

## WARNING: somehow, you can put literally ANYTHING as a second argument of
## target_link_libraries, and it will silently ignore it, and (hopefully)
## postpone error discovery to build time (in MSVC case it will just lack
## include paths in our case)
## TLDR make sure second argument to target_link_libraries is typed correctly
## (should be the same as corresponding library target name in
## VcppBits/<library_name>/CMakeLists.txt:add_library(<TARGET_NAME> ...) line)
target_link_libraries(${CMAKE_PROJECT_NAME} VcppBits-KeyFile)

