cmake_minimum_required(VERSION 3.25.1)
# include HunterGate cmake module
include("cmake/HunterGate.cmake")
# List of available packages is defined in hunter archive tar.gz below
HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.24.18.tar.gz"
    SHA1 "1292e4d661e1770d6d6ca08c12c07cf34a0bf718"
    LOCAL # use local cmake config - cmake/Hunter/config.cmake
)
project(hunter-example VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
# Download and build Boost package
# Boost version can be specified in cmake/Hunter/config.cmake
hunter_add_package(fmt)
hunter_add_package(spdlog)
hunter_add_package(Boost COMPONENTS regex)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED regex)

add_executable(cmain src/main.cpp)

# this will add Boost -I -L and .a to c++ c2.cpp build cmd
target_link_libraries(cmain PUBLIC Boost::regex fmt::fmt spdlog::spdlog)

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
