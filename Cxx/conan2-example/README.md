# Example of C++ project with Conan2 package manager

## Conan 2
The project uses [Conan 2](https://github.com/conan-io) pacage manager. Boost library 1.80.0 (header only) will be downloaded automatically.

[Conan 2.0 - C and C++ Package Manager Documentation](https://docs.conan.io/2/)

## CMake

To build and run
```
# Run the following commands in the project root

# Download required packages and prepare build folder with CMake toolchain
conan install . --output-folder=build --build=missing --profile debug

# Generate Makefile
cd build
cmake .. \
-DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
-DCMAKE_BUILD_TYPE=Debug \
-DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE

# Build
cmake --build .
# or
make -j1 -VERBOSE=1

# Run
./c2
```


## VSCode

VSCode and cmake-tools extension should understand generated CMake files

[.vscode/launch.json](.vscode/launch.json) file is added to the project too. It should allow to run/debug c2 programm.
