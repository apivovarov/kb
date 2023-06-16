cmake_minimum_required(VERSION 3.0.0)
project(c1 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

# include(CTest)
# enable_testing()

add_executable(c2 src/c2.cpp)
add_executable(cmain src/main.cpp)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
