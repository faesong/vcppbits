cmake_minimum_required(VERSION 3.16)

project(vcppbits)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX)
  set(ADDITIONAL_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wno-switch -Wuninitialized  -Wshadow -Wredundant-decls -Wdisabled-optimization -Wnoexcept -Wsign-compare -Wsign-conversion -Wstrict-aliasing")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")

add_subdirectory(VcppBits/StringUtils StringUtils)
add_subdirectory(VcppBits/KeyFile KeyFile)


add_executable(tests
  VcppBits/TestsDriver/TestsDriver.cpp
  VcppBits/Settings/SettingsTests.cpp
  VcppBits/Settings/Setting.cpp
  VcppBits/Settings2/Settings2Tests.cpp
  VcppBits/Settings2/Settings2CustomTypeTests.cpp
  VcppBits/MathUtils/MathUtilsTests.cpp
  )

target_link_libraries(tests VcppBits-KeyFile)
