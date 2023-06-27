# cmake -B build -G Ninja \
# -DCMAKE_TOOLCHAIN_FILE=/Users/pivovaa/workspace/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake_minimum_required(VERSION 3.0.0)
project(c1 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

# include(CTest)
# enable_testing()

find_package(Boost REQUIRED COMPONENTS regex)
find_package(spdlog CONFIG REQUIRED)

add_executable(cmain src/main.cpp)

target_link_libraries(cmain PRIVATE Boost::regex spdlog::spdlog)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
