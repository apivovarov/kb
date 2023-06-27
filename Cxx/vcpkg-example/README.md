# Example of C++ project with vcpkg package manager

## vcpkg
The project uses [vcpkg](https://vcpkg.io/en/) package manager. The dependency of the project is [spdlog](https://github.com/gabime/spdlog) C++ logging library.

[vcpkg Browse packages](https://vcpkg.io/en/packages.html)

[vcpkg GitHub](https://github.com/microsoft/vcpkg)

[vcpkg Documentation](https://learn.microsoft.com/en-us/vcpkg/)

Clone and Bootstrap vcpkg as described in [Get started with vcpkg](https://vcpkg.io/en/getting-started.html)

Set `VCPKG_ROOT` env var to point to vcpkg root dir

## Packages installation
Required Packages can be installed globally to `$VCPKGROOT/installed` folder.
```
vcpkg search <pkg_name>
vcpkg install <pkg_name>
```

Alternatively, required packages can be installed locally to particular project build folder.
In order to do it we should list required packages in [vcpkg.json](vcpkg.json) file located in the root of the project. `cmake/make` will automatically download the packages to project build folder - `<project_root>/build/vcpkg_installed`.

## CMake

To build and run
```bash
# Run the following commands in the project root

# Download required packages and prepare build folder with vcpkg CMake toolchain

cmake -B build \
-DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
-DCMAKE_BUILD_TYPE=Debug \
-DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE

# Build
cmake --build build
# or
cd build
make VERBOSE=1

# Run
build/cmain
```

## VSCode

To integrate VSCode and cmake-tools extension with vcpkg toolchain file we should create [CMakePresets.json](CMakePresets.json) file.

[.vscode/launch.json](.vscode/launch.json) file is added to the project too. It should allow to run/debug cmain program.
