# Filesystem

Within our platform software there is often the use-case to interact with the filesystem. It shall be noted that for
most adaptive applications only `ara::per` (adaptive AUTOSAR Persistence Module) or our abstraction `mw::storage` shall
be used.

This library shall only be used in cases where the use-cases specified in `ara::per` are not enough. When doing so an
application will no longer be an `adaptive application`, meaning, porting it to other ECUs will be way harder.

Nevertheless, our software platform needs direct access to the filesystem. This library tries to abstract the C-Like
POSIX APIs, which are already wrapped in `lib/os`, in a C++-manner.

We try to orient ourselves on https://en.cppreference.com/w/cpp/filesystem. For testing reasons though, we will not
implement directly the APIs specified by the C++ standard, but rather a mockable alternative.

See design notes [here](./design/README.md).

## Overview

The libary contains the following components:
- The Path class is analogous to `std::filesystem::path`.
- The IStandardFilesytem interface contains analogues of `std::filesystem` free-floating functions.
- The IFileFactory interface allows you to create file streams.
- The directory iterators DirectoryIterator and RecursiveDirectoryIterator are analogues of `std::filesystem::directory_iterator` and `std::filesystem::recursive_directory_iterator`.

The library also provides mock and fake objects for use in unit tests.
The StandardFilesystemFake class implements an in-memory file system that can fake not only the IStandardFilesystem interface,
but also IFileFactory interface and directory iterators.

## Future plans and known issues

Some features are not yet implemented, for example:
- StandardFilesystem: functions for symlink are not implemented.

Also library has some issues, for example:
- FileFactoryFake: the openmode option is not supported.

See the latest updates on Jira epic: broken_link_j/Ticket-32228

## The library entry point

The single entry point helps to reduce dependencies to single parts in the production/test code.
To include the library, use the bazel target "//platform/aas/lib/filesystem"
and the header file "score/filesystem/filesystem.h".
It is recommended to use the IFilesystemFactory class, which provides access to the library's interfaces.

## Singleton (only recommended for migration)

For migration purpose the singleton can be used instead of single entry point.
The singleton available via instance()-method of correcponding interface.

## Usage examples

### How to use IStandardFilesystem

```
// Usage

class Foo
{
public:
    Foo(const filesystem::IStandardFilesystem& filesystem);
    ...
};

bool Foo::Bar()
{
    ...
    // path
    filesystem::Path dir{"/foo/bar"};
    ...
    // create directory
    const auto result = filesystem_.CreateDirectories(dir);
    if(!result.has_value())
    {
        // process error
    }
}

// Test

class FooTest : public ::testing::Test
{
public:
    score::filesystem::StandardFilesystemMock filesystem_mock_;
    Foo unit_{filesystem_mock_};
};

TEST_F(FooTest, SomeTest)
{
    EXPECT_CALL(filesystem_mock_, CreateDirectory, ...);

    unit_.Bar();
    ...
}
```


### How to use IStandardFilesystem singleton (only recommended for migration)

```
// Usage

class Foo
{
public:
    Foo();
    ...
};

bool Foo::Bar()
{
    ...
    // path
    filesystem::Path dir{"/foo/bar"};
    ...
    // create directory
    const auto result = filesystem::IStandardFilesystem::instance().CreateDirectories(dir);
    if(!result.has_value())
    {
        // process error
    }
}

// Test

class FooTest : public ::testing::Test
{
public:
    os:MockGuard<score::filesystem::StandardFilesystemMock> filesystem_mock_;
    Foo unit_;
};

TEST_F(FooTest, SomeTest)
{
    EXPECT_CALL(*filesystem_mock_, CreateDirectory).WillOnce(Return(Result<void>{}));

    unit_.Bar();
    ...
}
```

### How to use IFileFactory

```
// Usage

class Foo
{
public:
    Foo(const filesystem::IFileFactory& file_factory);
    ...
};

bool Foo::Bar()
{
    ...
    // path
    filesystem::Path dir{"/foo/bar"};
    ...
    // create directory
    const auto result = file_factory.Open(fileName, std::ios::in | std::ios::binary);
    if (result.has_value() && result.value()!=nullptr && *result.value())
    {
        std::iostream& file = *result.value();
        ...
    }
}

// Test ()

class FooTest : public ::testing::Test
{
public:
    score::filesystem::FileFactoryMock file_factory_mock_;
    Foo unit_{file_factory_mock_};
};

TEST_F(FooTest, BadFile)
{
    auto bad_file = [](Unused, Unused) -> std::unique_ptr<std::iostream> {
        auto bad_ss_uptr = std::make_unique<std::stringstream>();
        bad_ss_uptr->setstate(std::ios_base::badbit);
        return bad_ss_uptr;
    };

    EXPECT_CALL(*file_factory_mock_, Open(Eq(Path("/foo/bar.csv")), _)).WillOnce(Invoke(bad_file));

    unit_.Bar();
    ...
}
```

### How to test using StandardFilesystemFake + FileFactoryFake

```
// Test ()

class FooTest : public ::testing::Test
{
public:
    // in-memory fake filesystem
    score::filesystem::StandardFilesystemFake file_system_fake_{};
    // create file_factory_fake_ attached to file_system_fake_
    score::filesystem::FileFactoryFake file_factory_fake_{file_system_fake_};

    void SetUp() override
    {
        file_system_fake_.CreateDirectories("/foo/bar");

        // the following file will be readable by absolute or relative path
        *file_factory_fake_.Open("/foo/bar/file.csv").value() << "some data";
    }

    Foo unit_{...};
};
```
