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
#include "score/filesystem/iterator/directory_iterator.h"

#include "score/os/mocklib/mock_dirent.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <vector>

namespace score
{
namespace filesystem
{
namespace
{

class DirectoryIteratorFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        ::mkdir("/tmp/directory_iterator_test", 0755);
        files_.push_back("/tmp/directory_iterator_test/file_1.txt");
        files_.push_back("/tmp/directory_iterator_test/file_2.txt");
        files_.push_back("/tmp/directory_iterator_test/file_3.txt");

        for (const auto& file : files_)
        {
            ::close(::open(file.CStr(), O_RDWR | O_CREAT, 0755));
        }
    }

    void TearDown() override
    {
        for (const auto& file : files_)
        {
            ::unlink(file.CStr());
        }

        ::unlink("/tmp/directory_iterator_test");
    }

    bool IsExistingFile(const Path& path) const
    {
        return std::find(files_.cbegin(), files_.cend(), path) != files_.cend();
    }

  private:
    std::vector<Path> files_{};
};

TEST_F(DirectoryIteratorFixture, DefaultConstructedEqualEnd)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'DirectoryIteratorFixture.DefaultConstructedEqualEnd' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'DirectoryIteratorFixture.DefaultConstructedEqualEnd' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    DirectoryIterator unit{"/"};

    EXPECT_EQ(DirectoryIterator{}, end(unit));
}

TEST_F(DirectoryIteratorFixture, IterateRangeBased)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description",
                   "Check 'DirectoryIteratorFixture.IterateRangeBased' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    DirectoryIterator unit{Path{"/tmp/directory_iterator_test"}};

    std::vector<Path> found_paths{};
    for (const auto& entry : unit)
    {
        found_paths.push_back(entry.GetPath());
    }

    ASSERT_EQ(found_paths.size(), 3);

    EXPECT_TRUE(IsExistingFile(found_paths[1]));
}

TEST_F(DirectoryIteratorFixture, IterateNonExistingDirectory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'DirectoryIteratorFixture.IterateNonExistingDirectory' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'DirectoryIteratorFixture.IterateNonExistingDirectory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    DirectoryIterator unit{Path{"/tmp/directory_iterator_test_not_existing"}};

    EXPECT_EQ(unit, end(DirectoryIterator{}));
    EXPECT_EQ(unit->GetPath(), Path{});
    EXPECT_FALSE(unit.HasValue());
    EXPECT_EQ(unit.Error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

TEST_F(DirectoryIteratorFixture, CanDereferenceIterator)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description",
                   "Check 'DirectoryIteratorFixture.CanDereferenceIterator' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    DirectoryIterator unit{Path{"/tmp/directory_iterator_test"}};

    const auto& entry = *unit;

    EXPECT_TRUE(IsExistingFile(entry.GetPath()));
}

TEST_F(DirectoryIteratorFixture, DefaultConstructedIsEmpty)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description",
                   "Check 'DirectoryIteratorFixture.DefaultConstructedIsEmpty' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    DirectoryIterator unit{};

    EXPECT_EQ(unit->GetPath(), Path{});
    EXPECT_TRUE(unit.HasValue());
    constexpr auto NO_ERROR = 0;
    EXPECT_EQ(unit.Error(), score::os::Error::createFromErrno(NO_ERROR));
}

TEST_F(DirectoryIteratorFixture, DefaultConstructedIsSafeToIncrement)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'DirectoryIteratorFixture.DefaultConstructedIsSafeToIncrement' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'DirectoryIteratorFixture.DefaultConstructedIsSafeToIncrement' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    DirectoryIterator unit{};

    score::cpp::ignore = ++unit;

    EXPECT_EQ(unit, DirectoryIterator{});
}

TEST_F(DirectoryIteratorFixture, DefaultConstructedIsSafeToDereference)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description",
                   "Check 'DirectoryIteratorFixture.DefaultConstructedIsSafeToDereference' behavior for the filesystem "
                   "component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    DirectoryIterator unit{};

    EXPECT_NO_THROW(*unit);
}

TEST_F(DirectoryIteratorFixture, ReaddirError)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description",
                   "Check 'DirectoryIteratorFixture.ReaddirError' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    DirectoryIterator unit{Path{"/tmp/directory_iterator_test"}};

    {
        os::MockGuard<os::MockDirent> dirent_mock;
        ON_CALL(*dirent_mock, readdir(::testing::_))
            .WillByDefault(::testing::Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EBADF))));
        ++unit;
    }

    EXPECT_FALSE(unit.HasValue());
    EXPECT_EQ(unit.Error(), score::os::Error::createFromErrno(EBADF));
}

}  // namespace
}  // namespace filesystem
}  // namespace score
