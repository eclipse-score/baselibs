# CMake Build for Eclipse SCORE Base Libraries

This directory contains a CMake build system for the Eclipse SCORE Base Libraries project, which provides a CMake alternative to the existing Bazel build system.

## Prerequisites

### Required Dependencies
- **CMake** >= 3.20
- **C++ Compiler** with C++20 support (GCC 12+ recommended)
- **Boost** libraries (container, system)
- **nlohmann_json**
- **GoogleTest** (for testing)

### Optional Dependencies
- **libacl-dev** - Access Control Lists support
- **libcap-dev** - POSIX capabilities support  
- **libseccomp-dev** - Secure computing mode support
- **valgrind** - Memory debugging (development only)

### Installing Dependencies on Ubuntu/Debian

```bash
# Required dependencies
sudo apt-get update
sudo apt-get install cmake build-essential
sudo apt-get install libboost-all-dev nlohmann-json3-dev libgtest-dev

# Optional dependencies
sudo apt-get install libacl1-dev libcap-dev libseccomp-dev valgrind
```

## Building the Project

### Quick Start

```bash
# Clone the repository (if not already done)
# cd to the baselibs directory

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run tests (optional)
ctest
```

### Advanced Configuration

```bash
# Configure with specific options
cmake -DCMAKE_BUILD_TYPE=Release \
      -DSCORE_BUILD_TESTING=ON \
      -DSCORE_TREAT_WARNINGS_AS_ERRORS=ON \
      ..

# Build specific targets
cmake --build . --target score_bitmanipulation
cmake --build . --target score_concurrency

# Build and run tests
cmake --build . --target test
```

### Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `SCORE_BUILD_TESTING` | `ON` | Build unit tests |
| `SCORE_TREAT_WARNINGS_AS_ERRORS` | `ON` | Treat compiler warnings as errors |
| `CMAKE_BUILD_TYPE` | `Release` | Build type (Debug/Release/RelWithDebInfo) |

## Project Structure

The CMake build creates the following libraries:

### Core Libraries
- `score_bitmanipulation` - Bit manipulation utilities
- `score_bitmask_operators` - Bitmask operations
- `score_concurrency` - Threading and synchronization primitives
- `score_containers` - Container utilities
- `score_datetime_converter` - Date/time conversion utilities
- `score_filesystem` - File system utilities
- `score_json` - JSON processing utilities
- `score_memory` - Memory management utilities
- `score_os` - Operating system abstractions
- `score_result` - Result type utilities
- `score_utils` - General utilities

### Concurrency Sub-libraries
- `score_concurrency_future` - Future/promise implementations
- `score_thread_load_tracking` - Thread load monitoring
- `score_timed_executor` - Timed task execution

### Language Support
- `score_futurecpp` - Future C++ language features
- `score_safecpp` - Safe C++ utilities

### Meta Library
- `score_all` - Interface library that includes all SCORE libraries

## Installing

```bash
# Install to system directories
cmake --build . --target install

# Install to custom directory
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ..
cmake --build . --target install
```

## Using in Other Projects

After installation, you can use SCORE Base Libraries in other CMake projects:

```cmake
find_package(score-baselibs REQUIRED)

target_link_libraries(your_target 
    PRIVATE 
        score::score_all  # Use all libraries
        # OR specific libraries:
        # score::score_concurrency
        # score::score_bitmanipulation
)
```

## Testing

The project includes comprehensive unit tests. To run them:

```bash
# Build and run all tests
ctest

# Run tests with verbose output
ctest --verbose

# Run specific test
ctest -R BitManipulationTest

# Run tests in parallel
ctest -j$(nproc)
```

## Differences from Bazel Build

This CMake build aims to be equivalent to the Bazel build but with some differences:

1. **Dependencies**: Uses system packages instead of downloading specific versions
2. **Toolchain**: Uses system compiler instead of Bazel's toolchain management
3. **QNX Support**: Currently focuses on Linux; QNX support can be added later
4. **Third-party**: Uses pkg-config to find system libraries instead of downloading .deb packages

## Troubleshooting

### Common Issues

1. **Missing dependencies**: Install all required packages listed above
2. **CMake version**: Ensure CMake >= 3.20 is installed
3. **Compiler support**: Ensure your compiler supports C++17
4. **Boost not found**: Install libboost-all-dev or point CMAKE_PREFIX_PATH to Boost installation

### Debugging Build Issues

```bash
# Configure with verbose makefiles
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..

# See actual compiler commands
cmake --build . -- VERBOSE=1

# Enable debug output from CMake
cmake --debug-output ..
```

## Contributing

This CMake build system is designed to complement the existing Bazel build. When adding new libraries or tests to the Bazel build, please also update the corresponding CMakeLists.txt files.

## License

This project is licensed under the Apache License Version 2.0. See the LICENSE file for details.
