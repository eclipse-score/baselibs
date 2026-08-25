# Design - Filesystem

## Main Design Goals

Our library has three main design goals

* enable unit-testability of filesystem interactions
* abstract low-level POSIX interactions
* reduce duplicated code

In order todo so, the current use-cases within the codebase and a first version of the library have been analyzed. It
turns out, that over 90% of the use-cases are covered by the C++17 implementation
of https://en.cppreference.com/w/cpp/filesystem.

Only the part of unit-testability is not covered with above API.

## Considering existing implementations

Boost implements a version of C++17 filesystem APIs in a manner that it can be used within C++14. Due to the fact that
this library will, most certainly, be used by safety components, any third-party library might not be sufficient. Also,
Boost does not solve the unit-testability problem, which would require a custom wrapper anyhow.

Within our code-base, we have already multiple filesystem implementations:

* DeltaInstall -> FileAdapter
* Upd_FileAdapter
* ICompressedFile
* IFileOperations
* FileUtils
* lib/os -> FileSystem
* lib/ifsilibwrapper/include/fs/SystemOperations.h

While all of them share nearly the same functionality, none of them provides a super set. While the DeltaInstaller
FileAdapter nearly comes with the perfect solution, it's error reporting is yet again a component specific solution.

## API based on standards

We don't want to re-invent the wheel on API level. C++ provides already a well-structured API that is also known by a
fair amount of developers. Thus, we want to reuse the `std::filesystem` API where possible for file-system manipulation
and `std::fstream` API for file manipulations.

Some extensions and changes are necessary to enable a smooth integration into the rest of our codebase, which is
explained in following.

## Error-Reporting

A basic decision was taken earlier in this project to abandon exceptions for good. Our error reporting shall be based on
`lib/result`. `std::filesystem` foresees error code reporting, but no solution similar to `score::Result`. Thus, we will adjust
the error reporting accordingly.

## Unit-Testability

In order to achieve unit-testability, we will wrapp the `std::filesystem` free-functions in a single-class,
covered with virtual functions. This will enable to create easily mockable objects, that can then be injected.

For the file-manipulations, we will not mock `std::fstream` but provide an abstract factory, which will
create `std::iostream` objects. In test cases, the factory will then return `std::stringstream`, while in production
code an `std::fstream` will be returned.

## Migration

As of today, our code uses free-floating functions to a big extent. In order to support a step-by-step migration (which
might take multiple years), we will provide an additional singleton solution. Users of free functions will then be able
to migrate to our implementation using the singleton and still follow unit-testability best practices (ObjectSeam).
Then, once our users refactor their applications, they will be able to remove the singleton (bad practice) and use basic
dependency injection.

## Extension of `std::filesystem`

As illustrated earlier we want to orient our API as much as possible on `std::filesystem`.
There are some use-cases that are not yet covered by it, thus we have a custom extension point.

## Single entry point

The single entry point helps to isolate library users from internal changes.
The library's single entry point consists of:
- bazel target: //score/filesystem.
- header file: "score/filesystem/filesystem.h".
- factory: IFilesystemFactory class.

The factory returns a structure containing pointers to library interfaces.

## Class diagram

<img alt="Class Diagram" src="https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/eclipse-score/baselibs/refs/heads/main/score/filesystem/design/structure.puml">
