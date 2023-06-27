cmake_minimum_required(VERSION 3.25.1)
project(conan2-example VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
# Download and build Boost package
# Boost version can be specified in cmake/Hunter/config.cmake
find_package(Boost CONFIG REQUIRED regex)
find_package(spdlog CONFIG REQUIRED)

add_executable(cmain src/main.cpp)

# this will add Boost -I -L and .a to c++ main.cpp build cmd
target_link_libraries(cmain PUBLIC Boost::headers spdlog::spdlog)

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
