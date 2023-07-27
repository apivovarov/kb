# cmake -B build -G Ninja \
# -DCMAKE_TOOLCHAIN_FILE=/Users/pivovaa/workspace/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake_minimum_required(VERSION 3.25.1)
project(concur-example VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -save-temps")

message("CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

#include_directories("/usr/local/include")
include_directories("include")

# include(CTest)
# enable_testing()

find_package(fmt CONFIG REQUIRED)

# Use glad2 webservice to generate glad.zip
# and use include/glad/gl.h and src/gl.c from the zip.
# find_library(GLAD NAMES glad REQUIRED)
# if(GLAD)
#   message("The library glad was found at: ${GLAD}")
# endif()

add_executable(shpt src/shpt.cpp)
add_executable(mutex_test src/mutex_test.cpp)
add_executable(pool_test src/pool_test.cpp)
add_executable(fork_demo src/fork_demo.c)
add_executable(futex_demo src/futex_demo.c)

target_link_libraries(shpt PRIVATE fmt::fmt)
target_link_libraries(mutex_test PRIVATE fmt::fmt)
target_link_libraries(pool_test PRIVATE fmt::fmt)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
