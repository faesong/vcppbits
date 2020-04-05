cmake_minimum_required(VERSION 3.17)

project(vcppbits)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX)
  set(ADDITIONAL_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wno-switch -Wuninitialized  -Wshadow -Wredundant-decls -Wdisabled-optimization -Wnoexcept -Wsign-compare -Wsign-conversion -Wstrict-aliasing")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")


add_executable(tests
  Tests/TestsDriver.cpp
  VcppBits/Settings/SettingsTests.cpp
  VcppBits/Settings/Setting.cpp)

