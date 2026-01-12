/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#include "score/filesystem/path.h"

#include <gtest/gtest.h>
#include <memory>

namespace score
{
namespace filesystem
{
namespace
{

TEST(Path, DefaultConstructedPathIsEmpty)
{
    // Given a default constructed unit
    Path unit{};

    // When requesting it in native format
    const auto& native_path = unit.Native();

    // Then the path is empty
    EXPECT_EQ(native_path, "");
}

TEST(Path, ConstructedByStdString)
{
    // Given a Path that is constructed by a std::string
    Path unit = std::string{"/some/path"};

    // When requesting it in native format
    const auto& native_path = unit.Native();

    // Then the path is equals the provided one
    EXPECT_EQ(native_path, "/some/path");
}

TEST(Path, ConstructedByConstStdString)
{
    // Given a Path that is constructed by a std::string
    const std::string s{"/some/path"};
    Path unit{s};

    // When requesting it in native format
    const auto& native_path = unit.Native();

    // Then the path is equals the provided one
    EXPECT_EQ(native_path, "/some/path");
}

TEST(Path, ConstructedByCharArray)
{
    // Given a Path that is constructed by a const char[?]
    Path unit = "/some/path";

    // When requesting it in native format
    const auto& native_path = unit.Native();

    // Then the path is equals the provided one
    EXPECT_EQ(native_path, "/some/path");
}

TEST(Path, ConstructedByCharPointer)
{
    // Given a Path that is constructed by a const char*
    const char* x = "/some/path";
    Path unit = x;

    // When requesting it in native format
    const auto& native_path = unit.Native();

    // Then the path is equals the provided one
    EXPECT_EQ(native_path, "/some/path");
}

TEST(Path, CopyConstructed)
{
    // Given a Path that is copy constructed
    Path other{std::string{"/some/path"}};
    Path unit = other;

    // When requesting it in native format
    const auto& native_path = unit.Native();

    // Then the path is equals the provided one
    EXPECT_EQ(native_path, "/some/path");
}

TEST(Path, MoveConstructed)
{
    // Given a Path that is move constructed
    Path other{std::string{"/some/path"}};
    Path unit = std::move(other);

    // When requesting it in native format
    const auto& native_path = unit.Native();

    // Then the path is equals the provided one
    EXPECT_EQ(native_path, "/some/path");
}

TEST(Path, AppendRelativePath)
{
    // Given a path
    Path unit{"/foo"};

    // When appending a relative path
    unit /= Path{"bar"};

    // Then the path is appended including a separator
    EXPECT_EQ(unit.Native(), "/foo/bar");
}

TEST(Path, AppendEmptyPath)
{
    // Given a path
    Path unit{"/foo"};

    // When appending an empty path
    unit /= Path{""};

    // Then only a separator is added
    EXPECT_EQ(unit.Native(), "/foo/");
}

TEST(Path, AppendToEmptyPath)
{
    // Given a path
    Path unit{""};

    // When appending an empty path
    unit /= Path{"foo"};

    // Then only a separator is added
    EXPECT_EQ(unit.Native(), "foo");
}

TEST(Path, AppendAbsolutePath)
{
    // Given a path
    Path unit{"/foo"};

    // When appending an absolute path
    unit /= Path{"/bar"};

    // Then the old path is replaced
    EXPECT_EQ(unit.Native(), "/bar");
}

TEST(Path, AppendPathWithExistingSeparator)
{
    // Given a path with a trailing separator
    Path unit{"/foo/"};

    // When appending a path
    unit.Append("bar");

    // Then there is no double separator
    EXPECT_EQ(unit.Native(), "/foo/bar");
}

TEST(Path, AppendString)
{
    // Given a path
    Path unit{"/foo"};

    // When appending a string
    unit /= "/bar";

    // Then it is treated as path
    EXPECT_EQ(unit.Native(), "/bar");
}

TEST(Path, AppendStringViewPointingToStringLiteral)
{
    // Given a path
    Path unit{"/foo"};

    // When appending a string view pointing to a string literal
    unit /= std::string_view{"/bar"};

    // Then it is treated as path
    EXPECT_EQ(unit.Native(), "/bar");
}

TEST(Path, IsAbsoluteOnEmpty)
{
    // Given an empty path
    Path unit{};

    // When checking if it is absolute
    const auto absolute = unit.IsAbsolute();

    // Then it is not
    EXPECT_FALSE(absolute);
}

TEST(Path, IsAbsoluteOnAbsolutePath)
{
    // Given an absolute path
    Path unit{"/home"};

    // When checking if it is absolute
    const auto absolute = unit.IsAbsolute();

    // Then it is
    EXPECT_TRUE(absolute);
}

TEST(Path, IsRelativeOnAbsolutePath)
{
    // Given an absolute path
    Path unit{"/home"};

    // When checking if it is relative
    const auto relative = unit.IsRelative();

    // Then it is not
    EXPECT_FALSE(relative);
}

TEST(Path, IsAbsoluteOnRelativePath)
{
    // Given a relative path
    Path unit{"./home"};

    // When checking if it is absolute
    const auto absolute = unit.IsAbsolute();

    // Then it is not
    EXPECT_FALSE(absolute);
}

TEST(Path, IsRelativeOnRelativePath)
{
    // Given a relative path
    Path unit{"./home"};

    // When checking if it is relative
    const auto relative = unit.IsRelative();

    // Then it is
    EXPECT_TRUE(relative);
}

TEST(Path, CheckForRootName)
{
    // Given a path without root-name
    Path unit{"/home/some/path"};

    // When checking if the path has a root name
    const auto has_root_name = unit.HasRootName();

    // Then it has not
    EXPECT_FALSE(has_root_name);
}

TEST(Path, GetRootDirectoryOnAbsolutePath)
{
    // Given an absolute path
    Path unit{"/home"};

    // When asking for the root directory
    const auto root_directory = unit.RootDirectory();

    // Then the root directory exists and is correct
    EXPECT_EQ(root_directory.Native(), "/");
    EXPECT_TRUE(unit.HasRootDirectory());
}

TEST(Path, GetRootDirectoryOnRelativePath)
{
    // Given a relative path
    Path unit{"home/some/path"};

    // When asking for the root directory
    const auto root_directory = unit.RootDirectory();

    // Then the root directory does not exist and is empty
    EXPECT_EQ(root_directory.Native(), "");
    EXPECT_FALSE(unit.HasRootDirectory());
}

TEST(Path, GetRootPathOnAbsolutePath)
{
    // Given an absolute path
    Path unit{"/home"};

    // When asking for the root path
    const auto root_directory = unit.RootPath();

    // Then the root path exists and is correct
    EXPECT_EQ(root_directory.Native(), "/");
    EXPECT_TRUE(unit.HasRootPath());
}

TEST(Path, GetRootPathOnRelativePath)
{
    // Given a relative path
    Path unit{"home/some/path"};

    // When asking for the root path
    const auto root_directory = unit.RootPath();

    // Then the root path does not exist and is empty
    EXPECT_EQ(root_directory.Native(), "");
    EXPECT_FALSE(unit.HasRootPath());
}

TEST(Path, RelativePathOfAbsoluteOne)
{
    // Given an absolute path
    Path unit{"/home/some/path"};

    // When asking for the relative path
    const auto relative_path = unit.RelativePath();

    // Then it exists and is correct
    EXPECT_EQ(relative_path.Native(), "home/some/path");
    EXPECT_TRUE(unit.HasRelativePath());
}

TEST(Path, RelativePathOfRelativePath)
{
    // Given an relative path
    Path unit{"home/some/path"};

    // When asking for the relative path
    const auto relative_path = unit.RelativePath();

    // Then it exists and is correct
    EXPECT_EQ(relative_path.Native(), "home/some/path");
    EXPECT_TRUE(unit.HasRelativePath());
}

TEST(Path, RelativePathOfEmptyPath)
{
    // Given an empty path
    Path unit{};

    // When asking for the relative path
    const auto relative_path = unit.RelativePath();

    // Then it does not exist and is empty
    EXPECT_EQ(relative_path.Native(), "");
    EXPECT_FALSE(unit.HasRelativePath());
}

TEST(Path, RelativePathOfRootPath)
{
    // Given an root path
    Path unit{"/"};

    // When asking for the relative path
    const auto relative_path = unit.RelativePath();

    // Then it does not exist and is empty
    EXPECT_EQ(relative_path.Native(), "");
    EXPECT_FALSE(unit.HasRelativePath());
}

TEST(Path, ResolveParentPath)
{
    // The test data was checked on page https://en.cppreference.com/w/cpp/filesystem/path/parent_path
    EXPECT_STREQ(Path{"/var/tmp/example.txt"}.ParentPath().CStr(), "/var/tmp");
    EXPECT_STREQ(Path{""}.ParentPath().CStr(), "");
    EXPECT_STREQ(Path{"/"}.ParentPath().CStr(), "/");
    EXPECT_STREQ(Path{"///"}.ParentPath().CStr(), "///");
    EXPECT_STREQ(Path{"/.//"}.ParentPath().CStr(), "/.");
    EXPECT_STREQ(Path{"/tmp"}.ParentPath().CStr(), "/");
    EXPECT_STREQ(Path{"asd"}.ParentPath().CStr(), "");
    EXPECT_STREQ(Path{"aaa/"}.ParentPath().CStr(), "aaa");
    EXPECT_STREQ(Path{"/aaa/bbb/"}.ParentPath().CStr(), "/aaa/bbb");
    EXPECT_STREQ(Path{"../.././"}.ParentPath().CStr(), "../../.");
}

TEST(Path, HasParentPath)
{
    EXPECT_TRUE(Path{"/"}.HasParentPath());
    EXPECT_TRUE(Path{"/tmp"}.HasParentPath());
    EXPECT_TRUE(Path{"/tmp/file"}.HasParentPath());
    EXPECT_FALSE(Path{""}.HasParentPath());
    EXPECT_FALSE(Path{"file"}.HasParentPath());
}

TEST(Path, ResolveFilename)
{
    // Given a path with a filename
    Path unit{"/hello/world"};

    // When asking for the filename
    const auto filename = unit.Filename();

    // Then it exists and is returned
    EXPECT_EQ(filename.Native(), "world");
    EXPECT_TRUE(unit.HasFilename());
}

TEST(Path, ResolveSimpleFilename)
{
    // Given a path with a filename
    Path unit{"world"};

    // When asking for the filename
    const auto filename = unit.Filename();

    // Then it exists and is returned
    EXPECT_EQ(filename.Native(), "world");
    EXPECT_TRUE(unit.HasFilename());
}

TEST(Path, ResolveFilenameWithoutFilename)
{
    // Given a path with an empty filename
    Path unit{"/hello/"};

    // When asking for the filename
    const auto filename = unit.Filename();

    // Then it does not exist and is empty
    EXPECT_EQ(filename.Native(), "");
    EXPECT_FALSE(unit.HasFilename());
}

TEST(Path, FilenameWithExtension)
{
    // Given a filename with an extension
    Path unit{"/foo/bar/test.txt"};

    // When asking for the extension
    const auto extension = unit.Extension();

    // Then the extension is returned and exists
    EXPECT_EQ(extension.Native(), ".txt");
    EXPECT_TRUE(unit.HasExtension());
}

TEST(Path, FilenameStartsWithPeriod)
{
    // Given a path which starts with period (.) character
    Path unit{"/foo/bar/.ssh"};

    // When asking for the extension
    const auto extension = unit.Extension();

    // Then the extension is returned
    EXPECT_EQ(extension.Native(), "");
    EXPECT_FALSE(unit.HasExtension());
}

TEST(Path, NoFileNameOnlyCurrentDir)
{
    // Given a path with no extension but only a dot (current dir)
    Path unit{"/foo/bar/."};

    // When asking for the extension
    const auto extension = unit.Extension();

    // Then it does not exist
    EXPECT_EQ(extension.Native(), "");
    EXPECT_FALSE(unit.HasExtension());
}

TEST(Path, NoFilenameOnlyParentDir)
{
    // Given a path with no extension but only a dot-dot (parent dir)
    Path unit{"/foo/bar/.."};

    // When asking for the extension
    const auto extension = unit.Extension();

    // Then it does not exist
    EXPECT_EQ(extension.Native(), "");
    EXPECT_FALSE(unit.HasExtension());
}

TEST(Path, NoExtension)
{
    // Given a filename without an extension (e.g. a dir)
    Path unit{"/foo/bar"};

    // When asking for an extension
    const auto extension = unit.Extension();

    // Then it does not exist
    EXPECT_EQ(extension.Native(), "");
    EXPECT_FALSE(unit.HasExtension());
}

TEST(Path, CheckStem)
{
    EXPECT_EQ(Path{"foo/bar/filename.ext"}.Stem().Native(), "filename");
    EXPECT_EQ(Path{"foo/bar/filename.ext.ext2"}.Stem().Native(), "filename.ext");
    EXPECT_EQ(Path{"foo/bar/.zzz"}.Stem().Native(), ".zzz");
    EXPECT_EQ(Path{"foo/bar/..zzz"}.Stem().Native(), ".");
    EXPECT_EQ(Path{"foo/bar/."}.Stem().Native(), ".");
    EXPECT_EQ(Path{"foo/bar/.."}.Stem().Native(), "..");
    EXPECT_EQ(Path{"foo/bar/"}.Stem().Native(), "");
    EXPECT_EQ(Path{""}.Stem().Native(), "");
}

std::string LexicallyNormal(const Path& unit)
{
    return unit.LexicallyNormal().Native();
}

TEST(Path, PathsAreLexicallyNormalized)
{
    EXPECT_EQ(LexicallyNormal("./"), ".");
    EXPECT_EQ(LexicallyNormal("hey/bla/../bla2/./../bla-bla/"), "hey/bla-bla/");
    EXPECT_EQ(LexicallyNormal("hey/bla/../bla2/./bla-bla/"), "hey/bla2/bla-bla/");
    EXPECT_EQ(LexicallyNormal("/root/../../../../../../foo/"), "/foo/");
    EXPECT_EQ(LexicallyNormal("/root/../../../../../../foo"), "/foo");
    EXPECT_EQ(LexicallyNormal(""), "");
    EXPECT_EQ(LexicallyNormal("/"), "/");
    EXPECT_EQ(LexicallyNormal("/hello/foo/////bar//.//tar"), "/hello/foo/bar/tar");
    // backslash is not a separator for POSIX
    EXPECT_EQ(LexicallyNormal("/hello/foo/\\//\\//bar//\\.//tar"), "/hello/foo/\\/\\/bar/\\./tar");
    EXPECT_EQ(LexicallyNormal("/hello/foo///////bar///.//tar"), "/hello/foo/bar/tar");
    EXPECT_EQ(LexicallyNormal("./foo/bar/"), "foo/bar/");
    EXPECT_EQ(LexicallyNormal("./foo/bar/."), "foo/bar/");
    EXPECT_EQ(LexicallyNormal("../../../../"), "../../../..");
    EXPECT_EQ(LexicallyNormal("/../../../"), "/");
    EXPECT_EQ(LexicallyNormal("/../../../foo"), "/foo");
    EXPECT_EQ(LexicallyNormal("/../../../foo/../bar/tar"), "/bar/tar");
    EXPECT_EQ(LexicallyNormal("/opt/foo/"), "/opt/foo/");
    EXPECT_EQ(LexicallyNormal("/opt/foo/.."), "/opt/");
    EXPECT_EQ(LexicallyNormal("/opt/foo/../"), "/opt/");
    EXPECT_EQ(LexicallyNormal(".."), "..");
    EXPECT_EQ(LexicallyNormal("../"), "..");
    EXPECT_EQ(LexicallyNormal("/.."), "/");
    EXPECT_EQ(LexicallyNormal("/../"), "/");
    EXPECT_EQ(LexicallyNormal("/."), "/");
    EXPECT_EQ(LexicallyNormal("/home/user/.profile"), "/home/user/.profile");
    EXPECT_EQ(LexicallyNormal("/home/user/..profile"), "/home/user/..profile");
    EXPECT_EQ(LexicallyNormal("ab/../"), ".");
    EXPECT_EQ(LexicallyNormal("ab/.."), ".");
    EXPECT_EQ(LexicallyNormal("foo/ab./../"), "foo/");
    EXPECT_EQ(LexicallyNormal("foo/ab../../"), "foo/");
    EXPECT_EQ(LexicallyNormal("foo/a.b/../"), "foo/");
    EXPECT_EQ(LexicallyNormal("foo/a..b/../"), "foo/");
    EXPECT_EQ(LexicallyNormal("foo/.b/../"), "foo/");
    EXPECT_EQ(LexicallyNormal("foo/..b/../"), "foo/");
    EXPECT_EQ(LexicallyNormal("foo/./../"), ".");
    EXPECT_EQ(LexicallyNormal("./././hello/foo/./bar/././tar/./."), "hello/foo/bar/tar/");
    EXPECT_EQ(LexicallyNormal("./."), ".");
}

TEST(Path, Empty)
{
    Path unit{};

    EXPECT_TRUE(unit.Empty());
}

TEST(Path, NotEmpty)
{
    Path unit{"/foo"};

    EXPECT_FALSE(unit.Empty());
}

TEST(Path, CStrConverter)
{
    Path unit{"/foo"};

    EXPECT_STREQ(unit.CStr(), "/foo");
}

TEST(Path, ImplicitStringConversion)
{
    Path unit{"/foo"};

    std::string string = unit;

    EXPECT_EQ(string, "/foo");
}

TEST(Path, NotEqual)
{
    Path foo{"/foo"};
    Path foo2{"/foo"};
    Path bar{"/bar"};

    EXPECT_TRUE(foo != bar);
    EXPECT_FALSE(foo != foo2);
}

class PathLessThan : public ::testing::TestWithParam<std::tuple<std::string_view, std::string_view>>
{
};

TEST_P(PathLessThan, LessThan)
{
    // Given two strings with arbitrary order
    auto [a, b] = GetParam();
    Path path_a{std::string{a}};
    Path path_b{std::string{b}};

    // When one string is less than the other
    if (a < b)
    {
        // Then the same ordering applies to the paths
        EXPECT_LT(path_a, path_b);
    }
    else if (b < a)
    {
        EXPECT_LT(path_b, path_a);
    }
    // else, if no string is less than the other
    else
    {
        // then the paths are equal
        EXPECT_EQ(path_a, path_b);
    }
}

INSTANTIATE_TEST_SUITE_P(SamplePaths,
                         PathLessThan,
                         ::testing::Values(std::make_tuple("", ""),
                                           std::make_tuple("a", "b"),
                                           std::make_tuple("/a", "a")));

class PathIteratorFixture : public ::testing::Test
{
  public:
    std::vector<Path> PathToArray(const Path& path)
    {
        std::vector<Path> array;
        for (const auto& p : path)
        {
            array.emplace_back(p);
        }
        return array;
    }
    std::vector<Path> PathToArray(const char* path)
    {
        return PathToArray(Path{path});
    }
};

TEST_F(PathIteratorFixture, SplitAbsolutePath)
{
    const auto& array = PathToArray("/foo/bar.txt");

    ASSERT_EQ(array.size(), 3);
    EXPECT_STREQ(array.at(0).CStr(), "/");
    EXPECT_STREQ(array.at(1).CStr(), "foo");
    EXPECT_STREQ(array.at(2).CStr(), "bar.txt");
}

TEST_F(PathIteratorFixture, SplitRelativePath)
{
    const auto& array = PathToArray("foo/bar.txt");

    ASSERT_EQ(array.size(), 2);
    EXPECT_STREQ(array.at(0).CStr(), "foo");
    EXPECT_STREQ(array.at(1).CStr(), "bar.txt");
}

TEST_F(PathIteratorFixture, SplitEmptyPath)
{
    const auto& array = PathToArray("");

    EXPECT_EQ(array.size(), 0);
}

TEST_F(PathIteratorFixture, SplitRootPath)
{
    const auto& array = PathToArray("/");

    EXPECT_EQ(array.size(), 1);
    EXPECT_STREQ(array.at(0).CStr(), "/");
}

TEST_F(PathIteratorFixture, SplitSeveralSeparators)
{
    const auto& array = PathToArray("/////");

    EXPECT_EQ(array.size(), 1);
    EXPECT_STREQ(array.at(0).CStr(), "/////");
}

TEST_F(PathIteratorFixture, SplitSeveralSeparatorsInAbsolutePath)
{
    const auto& array = PathToArray("///foo/////bar.txt");

    ASSERT_EQ(array.size(), 3);
    EXPECT_STREQ(array.at(0).CStr(), "/");
    EXPECT_STREQ(array.at(1).CStr(), "foo");
    EXPECT_STREQ(array.at(2).CStr(), "bar.txt");
}

TEST_F(PathIteratorFixture, PathToArrayWithouSeparator)
{
    const auto& array = PathToArray("foo");

    ASSERT_EQ(array.size(), 1);
    EXPECT_STREQ(array.at(0).CStr(), "foo");
}

TEST_F(PathIteratorFixture, SplitRelativePathDot)
{
    const auto& array = PathToArray("./foo/bar.txt");

    ASSERT_EQ(array.size(), 3);
    EXPECT_STREQ(array.at(0).CStr(), ".");
    EXPECT_STREQ(array.at(1).CStr(), "foo");
    EXPECT_STREQ(array.at(2).CStr(), "bar.txt");
}

TEST_F(PathIteratorFixture, SplitRelativePathDotDot)
{
    const auto& array = PathToArray("../foo/bar.txt");

    ASSERT_EQ(array.size(), 3);
    EXPECT_STREQ(array.at(0).CStr(), "..");
    EXPECT_STREQ(array.at(1).CStr(), "foo");
    EXPECT_STREQ(array.at(2).CStr(), "bar.txt");
}

TEST(PathIterator, LongPathIncrement)
{
    Path path{"/foo/bar.txt"};

    auto iterator = path.begin();
    EXPECT_EQ(*iterator, Path{"/"});

    ++iterator;
    EXPECT_EQ(*iterator, Path{"foo"});

    ++iterator;
    EXPECT_EQ(*iterator, Path{"bar.txt"});

    ++iterator;
    EXPECT_EQ(iterator, path.end());

    EXPECT_DEATH(++iterator, "");
}

TEST(PathIterator, ShortPathIncrement)
{
    Path path{"bar.txt"};

    auto iterator = path.begin();
    EXPECT_EQ(*iterator, Path{"bar.txt"});

    ++iterator;
    EXPECT_EQ(iterator, path.end());

    EXPECT_DEATH(++iterator, "");
}

TEST(PathIterator, EmptyPathIncrement)
{
    Path path{""};

    auto iterator = path.begin();
    EXPECT_EQ(iterator, path.end());

    EXPECT_DEATH(++iterator, "");
}

TEST(PathIterator, LongPathDecrement)
{
    Path path{"/foo/bar.txt"};

    auto iterator = path.begin();

    ++iterator;
    ++iterator;
    ++iterator;
    EXPECT_EQ(iterator, path.end());

    --iterator;
    EXPECT_EQ(*iterator, Path{"bar.txt"});

    --iterator;
    EXPECT_EQ(*iterator, Path{"foo"});

    --iterator;
    EXPECT_EQ(*iterator, Path{"/"});

    EXPECT_DEATH(--iterator, "");
}

TEST(PathIterator, ShortPathDecrement)
{
    Path path{"bar.txt"};

    auto iterator = path.begin();

    ++iterator;
    EXPECT_EQ(iterator, path.end());

    --iterator;
    EXPECT_EQ(*iterator, Path{"bar.txt"});

    EXPECT_DEATH(--iterator, "");
}

TEST(PathIterator, EmptyPathDecrement)
{
    Path path{""};

    auto iterator = path.begin();
    EXPECT_EQ(iterator, path.end());

    EXPECT_DEATH(--iterator, "");
}

TEST_F(PathIteratorFixture, ConcatPath)
{
    Path foo{"foo"};
    Path bar{"bar"};
    Path path = foo / bar;
    const auto& array = PathToArray(path);

    ASSERT_EQ(array.size(), 2);
    EXPECT_STREQ(array.at(0).CStr(), "foo");
    EXPECT_STREQ(array.at(1).CStr(), "bar");
}

TEST_F(PathIteratorFixture, AppendPath)
{
    Path foo{"foo"};
    Path bar{"bar"};
    Path path = foo;
    path /= bar;
    const auto& array = PathToArray(path);

    ASSERT_EQ(array.size(), 2);
    EXPECT_STREQ(array.at(0).CStr(), "foo");
    EXPECT_STREQ(array.at(1).CStr(), "bar");
}

TEST(PathIterator, PostIncrement)
{
    Path path{"foo/bar.txt"};

    auto iterator = path.begin();
    EXPECT_EQ(*iterator, Path{"foo"});

    auto prevoius_iterator = iterator++;
    EXPECT_EQ(*iterator, Path{"bar.txt"});
    EXPECT_EQ(prevoius_iterator, path.begin());
}

TEST(PathIterator, PostDecrement)
{
    Path path{"foo/bar.txt"};

    auto iterator = path.begin();

    ++iterator;
    ++iterator;
    EXPECT_EQ(iterator, path.end());

    --iterator;
    EXPECT_EQ(*iterator, Path{"bar.txt"});

    auto prevoius_iterator = iterator--;
    EXPECT_EQ(*iterator, Path{"foo"});
    EXPECT_EQ(*prevoius_iterator, Path{"bar.txt"});
}

TEST(PathIterator, Equality)
{
    Path path{"foo/bar.txt"};

    auto iterator_foo_1 = ++path.begin();
    auto iterator_foo_2 = ++path.begin();
    auto iterator_bar = ++ ++path.begin();

    // check ==
    EXPECT_TRUE(iterator_foo_1 == iterator_foo_2);
    EXPECT_TRUE(iterator_foo_2 == iterator_foo_1);
    EXPECT_FALSE(iterator_bar == iterator_foo_1);
    EXPECT_FALSE(iterator_foo_1 == iterator_bar);

    // check !=
    EXPECT_TRUE(iterator_foo_1 != iterator_bar);
    EXPECT_TRUE(iterator_bar != iterator_foo_1);
    EXPECT_FALSE(iterator_foo_1 != iterator_foo_2);
    EXPECT_FALSE(iterator_foo_2 != iterator_foo_1);
    EXPECT_FALSE(iterator_foo_2 != iterator_foo_1);
}

TEST(PathIterator, CompareIteratorsFromDifferentVariable)
{
    Path path1{"foo/bar.txt"};
    Path path2{"foo/bar.txt"};
    auto iterator1 = path1.begin();
    auto iterator2 = path2.begin();

    EXPECT_NE(iterator1, iterator2);
}

TEST(PathIterator, CompareWithEmptyIterator)
{
    Path path{"foo/bar.txt"};

    EXPECT_NE(path.begin(), Path::iterator{});
}

TEST(Path, ReplaceExtension)
{
    EXPECT_STREQ(Path{""}.ReplaceExtension(".empty").CStr(), ".empty");
    EXPECT_STREQ(Path{"/foo/bar.jpg"}.ReplaceExtension(".png").CStr(), "/foo/bar.png");
    EXPECT_STREQ(Path{"/foo/bar.jpg"}.ReplaceExtension("png").CStr(), "/foo/bar.png");
    EXPECT_STREQ(Path{"/foo/bar.jpg"}.ReplaceExtension(".").CStr(), "/foo/bar.");
    EXPECT_STREQ(Path{"/foo/bar.jpg"}.ReplaceExtension("").CStr(), "/foo/bar");
    EXPECT_STREQ(Path{"/foo/bar."}.ReplaceExtension("png").CStr(), "/foo/bar.png");
    EXPECT_STREQ(Path{"/foo/bar"}.ReplaceExtension(".png").CStr(), "/foo/bar.png");
    EXPECT_STREQ(Path{"/foo/bar"}.ReplaceExtension("png").CStr(), "/foo/bar.png");
    EXPECT_STREQ(Path{"/foo/bar"}.ReplaceExtension(".").CStr(), "/foo/bar.");
    EXPECT_STREQ(Path{"/foo/bar"}.ReplaceExtension("").CStr(), "/foo/bar");
    EXPECT_STREQ(Path{"/foo/."}.ReplaceExtension(".png").CStr(), "/foo/..png");
    EXPECT_STREQ(Path{"/foo/."}.ReplaceExtension("png").CStr(), "/foo/..png");
    EXPECT_STREQ(Path{"/foo/."}.ReplaceExtension(".").CStr(), "/foo/..");
    EXPECT_STREQ(Path{"/foo/."}.ReplaceExtension("").CStr(), "/foo/.");
    EXPECT_STREQ(Path{"/foo/"}.ReplaceExtension(".png").CStr(), "/foo/.png");
    EXPECT_STREQ(Path{"/foo/"}.ReplaceExtension("png").CStr(), "/foo/.png");
}

TEST(Path, RemoveFilename)
{
    EXPECT_STREQ(Path{"foo/bar"}.RemoveFilename().CStr(), "foo/");
    EXPECT_STREQ(Path{"foo/"}.RemoveFilename().CStr(), "foo/");
    EXPECT_STREQ(Path{"/foo"}.RemoveFilename().CStr(), "/");
    EXPECT_STREQ(Path{"/"}.RemoveFilename().CStr(), "/");
    EXPECT_STREQ(Path{""}.RemoveFilename().CStr(), "");
}

TEST(PathIterator, CopyOperator)
{
    Path path{"foo/bar.txt"};
    Path::iterator iterator = path.begin();
    Path::iterator iterator_copy{};

    iterator_copy = iterator;

    ASSERT_EQ(iterator, iterator_copy);
}

TEST(Path, CopyOperator_SelfAssignment)
{
    Path path{"foo/bar.txt"};

    path = *std::addressof(path);

    EXPECT_STREQ(path.CStr(), "foo/bar.txt");
}

TEST(Path, MoveOperator_SelfAssignment)
{
    Path path{"foo/bar.txt"};
    Path& path_ref{path};  // we use ref to avoid clang build error

    path = std::move(path_ref);

    EXPECT_STREQ(path.CStr(), "foo/bar.txt");
}

TEST(PathIterator, CopyOperator_SelfAssignment)
{
    Path path{"foo/bar.txt"};
    Path::iterator iterator = path.begin();

    iterator = *std::addressof(iterator);

    ASSERT_EQ(iterator, path.begin());
}

TEST(PathIterator, MoveOperator_SelfAssignment)
{
    Path path{"foo/bar.txt"};
    Path::iterator iterator = path.begin();
    Path::iterator& iterator_ref{iterator};  // we use ref to avoid clang build error

    iterator = std::move(iterator_ref);

    ASSERT_EQ(iterator, path.begin());
}

TEST(PathIterator, MoveOperator_Assignment)
{
    // Given two iterators on paths
    Path path{"foo/bar.txt"};
    Path path_2{"baz/fooz.txt"};
    Path::iterator iterator = path.begin();
    Path::iterator iterator_2 = path_2.begin();

    // When assigning one to the other by moving
    Path::iterator& iterator_2_ref{iterator_2};  // we use ref to avoid clang build error
    iterator = std::move(iterator_2_ref);

    // Then the target iterator is updated with the source iterator
    ASSERT_EQ(iterator, path_2.begin());
}

}  // namespace
}  // namespace filesystem
}  // namespace score
