# Example of VSCode C++ CMake project

VSCode Extensions:
- CMake
- CMake Tools

Launch configs use lldb-mi in launch mode. It requires [lldb-mi](https://github.com/lldb-tools/lldb-mi) to be installed.

## Configure
```
cmake --no-warn-unused-cli \
 -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
 -DCMAKE_BUILD_TYPE:STRING=Debug \
 -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang \
 -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ \
 -Bbuild \
 -G Ninja
```

## Build (debug mode)
```
cmake --build build --config Debug --target all
```

## Clean
```
cmake --build build --target clean
```

## VSCode CMake tools
To run cmake commands in VSCode hit F1 (to Show Command Palette) and type 
- CMake: Configure
- CMake: Build
- CMake: Clean
- CMake: Debug
