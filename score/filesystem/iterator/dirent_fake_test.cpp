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
#include "score/filesystem/iterator/dirent_fake.h"

#include "score/filesystem/iterator/directory_iterator.h"
#include "score/filesystem/iterator/recursive_directory_iterator.h"
#include "score/filesystem/standard_filesystem_fake.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

using ::testing::_;

class DirentFakeFixture : public ::testing::Test
{
  public:
    DirentFakeFixture() : filesystem_mock_{}
    {
        EXPECT_CALL(*filesystem_mock_, WeaklyCanonical(_)).Times(::testing::AnyNumber());
        AddFile("/tmp/foo/file1");
        AddFile("/tmp/foo/file2");
        AddFile("/tmp/foo/file3");
        AddFile("/tmp/bar/baz/file1");
        AddDir("/tmp/baz");
        AddDir("/xyz");
    }
    void AddDir(const Path& path)
    {
        const Path weakly_canonical = filesystem_mock_->WeaklyCanonical(path).value();
        ASSERT_TRUE(filesystem_mock_->CreateDirectories(weakly_canonical).has_value());
    }
    void AddFile(const Path& path)
    {
        const Path weakly_canonical = filesystem_mock_->WeaklyCanonical(path).value();
        AddDir(weakly_canonical.ParentPath());
        ASSERT_TRUE(filesystem_mock_->CreateRegularFile(path, Perms::kReadWriteExecUser).has_value());
    }
    static inline bool Contains(const std::list<std::string>& filelist, const char* filepath)
    {
        return std::find(filelist.begin(), filelist.end(), std::string(filepath)) != filelist.end();
    }
    os::MockGuard<StandardFilesystemFake> filesystem_mock_;
};

TEST_F(DirentFakeFixture, DirectoryIterator)
{
    std::list<std::string> filelist;
    for (const auto& entry : DirectoryIterator{"/tmp"})
    {
        filelist.emplace_back(entry.GetPath().CStr());
    }
    EXPECT_EQ(filelist.size(), 3);
    EXPECT_TRUE(Contains(filelist, "/tmp/foo"));
    EXPECT_TRUE(Contains(filelist, "/tmp/bar"));
    EXPECT_TRUE(Contains(filelist, "/tmp/baz"));
}

TEST_F(DirentFakeFixture, RecursiveDirectoryIterator)
{
    std::list<std::string> filelist;
    RecursiveDirectoryIterator recurIterator{"/tmp"};
    for (const auto& entry : recurIterator)
    {
        filelist.emplace_back(entry.GetPath().CStr());
    }
    EXPECT_EQ(filelist.size(), 8);
    EXPECT_TRUE(Contains(filelist, "/tmp/foo"));
    EXPECT_TRUE(Contains(filelist, "/tmp/foo/file1"));
    EXPECT_TRUE(Contains(filelist, "/tmp/foo/file2"));
    EXPECT_TRUE(Contains(filelist, "/tmp/foo/file3"));
    EXPECT_TRUE(Contains(filelist, "/tmp/bar"));
    EXPECT_TRUE(Contains(filelist, "/tmp/bar/baz"));
    EXPECT_TRUE(Contains(filelist, "/tmp/bar/baz/file1"));
    EXPECT_TRUE(Contains(filelist, "/tmp/baz"));

    EXPECT_TRUE(recurIterator.HasValue());
}
TEST_F(DirentFakeFixture, OpendirForNonExisted)
{
    auto& unit = filesystem_mock_->GetDirentFake();

    const auto& result = unit.opendir("/non_existed");
    EXPECT_FALSE(result.has_value());
}

TEST_F(DirentFakeFixture, OpendirForNonDirectory)
{
    auto& unit = filesystem_mock_->GetDirentFake();

    const auto& result = unit.opendir("/tmp/foo/file1");

    EXPECT_FALSE(result.has_value());
}

TEST_F(DirentFakeFixture, ReaddirForInvalidDescriptor)
{
    auto& unit = filesystem_mock_->GetDirentFake();

    const auto& result = unit.readdir(nullptr);

    EXPECT_FALSE(result.has_value());
}

TEST_F(DirentFakeFixture, ClosedirForInvalidDescriptor)
{
    auto& unit = filesystem_mock_->GetDirentFake();

    const auto& result = unit.closedir(nullptr);

    EXPECT_FALSE(result.has_value());
}

}  // namespace
}  // namespace filesystem
}  // namespace score
