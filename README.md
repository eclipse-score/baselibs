# SCORE baselibs

### Abstract

**baselibs** provides a set of base libraries for both C++ and Rust that can be used by S-CORE components. These libraries offer common functionality, ensuring consistent implementations, reducing code duplication, and promoting interoperability between components.

The base libraries include utilities for bit manipulation, concurrency management, containers, JSON processing, filesystem operations, memory handling, OS abstraction, error handling, serialization, logging, and various other common utilities needed across the S-CORE system.

Each library is designed to be independently usable while integrating seamlessly with the overall architecture.

**features:**

- **Cross-platform compatibility** with support for Linux and QNX operating systems
- **Modular architecture** enabling selective use of individual components
- **Robust error handling** using Result types
- **Extensive testing** with comprehensive mock and fake implementations

### Supported Platforms

| Architecture | Operating System | Environment | Configuration Name   |
| ------------ | ---------------- | ----------- | -------------------- |
| ARM64        | Linux            | Standard    | `arm64-linux`        |
| ARM64        | QNX              | Standard    | `arm64-qnx`          |
| ARM64        | QNX              | Hardware    | `arm64-qnx-hardware` |
| ARM64        | QNX              | Virtual     | `arm64-qnx-virtual`  |
| x86_64       | Linux            | Standard    | `x86_64-linux`       |
| x86_64       | QNX              | Standard    | `x86_64-qnx`         |

---

## 📂 module Structure

Here is the description of the baselibs project directory

| File/Folder                                                  | Description                                   |
| ------------------------------------------------------------ | --------------------------------------------- |
| `README.md`                                                  | Short description & build instructions        |
| `score/`                                                     | Source files for the modules                  |
| `third_party/`                                               | third_party lib like acl, boost, valgrind ... |
| `.github/workflows/`                                         | CI/CD pipelines                               |
| `bazel/`                                                     | Bazel environment and platform configuration  |
| `WORKSPACE`, `MODULE.bazel.lock`, `MODULE.bazel`, `.bazelrc`,  `.bazelversion`, `BUILD` | Bazel configuration & settings                |
| `LICENSE.md`                                                 | Licensing information                         |
| `CONTRIBUTION.md`                                            | Contribution guidelines                       |

The base libraries feature consists of the following libraries, all of which are currently implemented in C++.

| Module                               | Description                                                  |
| ------------------------------------ | ------------------------------------------------------------ |
| analysis                             | Shared memory tracing                                        |
| bitmanipulation                      | Utilities for bit manipulation                               |
| concurrency                          | Provides a generic interface to execute any C++ callable in a parallel context, supporting various execution strategies (e.g., thread pool, timed execution), thread safety, interruption handling, and periodic/delayed task execution. |
| containers                           | Offers a `DynamicArray` (fixed-size array with dynamic construction-time size) and an intrusive linked list implementation conforming to the [P0406R1 proposal](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0406r1.html). |
| filesystem                           | Filesystem manipulation library similar to `std::filesystem`. |
| json                                 | JSON abstraction layer that can switch between different parsers/serializers under the hood. |
| language                             | safety-oriented C++ utilities provided by score-baselibs, specifically safe mathematical operations and scoped function management. |
| memory                               | Utility library for memory handling, including an abstraction layer for shared memory. |
| mw                                   | Logging library                                              |
| os                                   | OS Abstraction Layer (OSAL) to interface with different POSIX-like operating systems such as Linux and QNX |
| quality                              | The code quality tools and static analysis infrastructure, currently in early development. |
| result                               | Provides a unified approach to error handling without exceptions, conforming to C++23 `std::expected`. |
| static_reflection_with_serialization | A header-only library for binary serialization, deserialization, and compile-time type reflection of heterogenuous C++ data structures with focus on compile-time safety and efficiency of serialization, as well as efficiency of filtering by content during deserialization |
| utils                                | Provides a collection of small, reusable utilities that do not fit into the other base libraries. |


---

## 🚀 Getting Started

The project is built using Bazel, so you should first install Bazel on your operation system. We use Bazel version 7.5.0. About how to install Bazel, please refer to the official Bazel documentation: [installing bazel](https://bazel.build/install).


### 1. Clone the Repository

```shell
git clone https://github.com/eclipse-score/baselibs.git
cd baselibs
```

### 2. Build the Examples of module

#### 2.1 x86-64

To build all targets of the module the following command can be used:

```shell
bazel build //score/...
```

This command will instruct Bazel to build all targets that are under Bazel package `score/`. The ideal solution is to provide single target that builds artifacts, for example:

```shell
bazel build //score/<module_name>:release_artifacts
```

where `:release_artifacts` is filegroup target that collects all release artifacts of the module.

Taking `os` as an example, you can use the following command to build the `os` module:

```shell
bazel build //score/os/...
```

Of course, you can also build a specific executable or library individually. You can use the following command to build  `acl` lib of `os`module:

```shell
bazel build //score/os:acl
```

> It is a good habit to use the Tab key frequently.

#### 2.2 ARM64 Cross Compile

Cross-compiling programs and libraries for the ARM64 architecture on an x86_64 platform.

```shell
TODO
```

### 3. Run Tests

To run all test cases of the module the following command can be used:

```shell
bazel test //score/...
```

To run test cases for a specific library, like `containers`:

```shell
bazel test //score/containers/...
```

To run a specific test case individually, like one of test case of `containers`, the following command can be used:

```shell
bazel test //score/containers:intrusive_list_test
```

---

## 🛠 Tools & Linters

The libs integrates **tools and linters** from **centralized repositories** to ensure consistency across projects.

* **C++:** `clang-tidy`, `cppcheck`, `Google Test`
* **CI/CD:** GitHub Actions for automated builds and tests

---

## 📖 Documentation

* A **centralized docs structure** is planned.