# Example of C++ project with Conan2 package manager

## Conan 2
The project uses [Conan 2](https://github.com/conan-io) package manager. Boost library 1.80.0 (header only) will be downloaded automatically.

[Conan 2.0 - C and C++ Package Manager Documentation](https://docs.conan.io/2/)

Create default and debug profiles as described it two first steps of Conan2 [tutorial](https://docs.conan.io/2/tutorial.html)

## CMake

To build and run
```bash
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
make VERBOSE=1

# Run
./cmain
```


## VSCode

To integrate VSCode and cmake-tools extension with conan2 toolchain file conan2 automatically creates [CMakePresets.json](CMakePresets.json) file which points to `build/CMakePresets.json` file.

[.vscode/launch.json](.vscode/launch.json) file is added to the project too. It should allow to run/debug cmain program.
