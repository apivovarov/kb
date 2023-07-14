# cmake -B build -G Ninja \
# -DCMAKE_TOOLCHAIN_FILE=/Users/pivovaa/workspace/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake_minimum_required(VERSION 3.25.1)
project(opengl-example VERSION 0.1.0 LANGUAGES C CXX)
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
find_package(glfw3 CONFIG REQUIRED)

# Use glad2 webservice to generate glad.zip
# and use include/glad/gl.h and src/gl.c from the zip.
# find_library(GLAD NAMES glad REQUIRED)
# if(GLAD)
#   message("The library glad was found at: ${GLAD}")
# endif()

add_library(gl src/gl.c)

add_executable(glmain src/glmain.cpp)
add_executable(test src/test.cpp)
add_executable(shpt src/shpt.cpp)

target_link_libraries(glmain PRIVATE fmt::fmt glfw gl)

target_link_libraries(shpt PRIVATE fmt::fmt)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
