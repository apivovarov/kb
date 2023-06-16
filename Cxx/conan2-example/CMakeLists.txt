cmake_minimum_required(VERSION 3.12)
project(c2 VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
# Download and build Boost package
# Boost version can be specified in cmake/Hunter/config.cmake
find_package(Boost CONFIG REQUIRED regex)

add_executable(c2 src/c2.cpp)
add_executable(cmain src/main.cpp)

# this will add Boost -I -L and .a to c++ c2.cpp build cmd
target_link_libraries(c2 PUBLIC Boost::headers)

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
