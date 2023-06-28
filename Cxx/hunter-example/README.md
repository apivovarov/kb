# Example of C++ project with Hunter package manager

## CMake

To build and run
```
# Run the following commands in the project root

# Generate Makefile. use -GNinja optionally
cmake -B build

# Build
cmake --build build

# Run
./build/cmain
```


## Hunter package manager
The project uses [Hunter package manager](https://github.com/cpp-pm/hunter). Boost-1.80.0 and spdlog will be downloaded and built automatically.

## VSCode

VSCode and cmake-tools extension should understand generated CMake files automatically. No additional files needed.

[.vscode/launch.json](.vscode/launch.json) file is added to the project too. It should allow to run/debug cmain program.
