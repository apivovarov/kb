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
./build/c2
```


## Hunter package manager
The project uses [Hunter package manager](https://github.com/cpp-pm/hunter). Boost library 1.80.0 will be downloaded and built automatically.
