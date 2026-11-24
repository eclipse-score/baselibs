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
#include "score/filesystem/iterator/recursive_directory_iterator.h"

#include "score/filesystem/details/test_helper.h"
#include "score/filesystem/standard_filesystem_mock.h"
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

using ::testing::_;

class RecursiveDirectoryIteratorFixture : public ::testing::Test
{
  public:
    void CreateFolder(const Path& path)
    {
        auto full_path = temp_folder_ / path;
        folders_.emplace_back(full_path);
        ASSERT_EQ(::mkdir(full_path.CStr(), 0755), 0);
    }

    void CreateFile(const Path& path)
    {
        auto full_path = temp_folder_ / path;
        files_.emplace_back(full_path);
        const auto file_descriptor = ::open(full_path.CStr(), O_RDWR | O_CREAT, 0755);
        ASSERT_GE(file_descriptor, 0);
        ::close(file_descriptor);
    }

    void CreateSymlink(const Path& target, const Path& link)
    {
        const Path full_path = temp_folder_ / link;
        files_.emplace_back(full_path);
        ASSERT_EQ(::symlink(target.CStr(), full_path.CStr()), 0);
    }

    void InitTempFolder()
    {
        temp_folder_ = test::InitTempDirectoryFor("recursive_directory_iterator_test");
        folders_.emplace_back(temp_folder_);
    }

    void SetUp() override
    {
        InitTempFolder();
    }

    void TearDown() override
    {
        for (const auto& file : files_)
        {
            ASSERT_EQ(::unlink(file.CStr()), 0);
        }

        std::for_each(folders_.crbegin(), folders_.crend(), [](const Path& folder) {
            ASSERT_EQ(::rmdir(folder.CStr()), 0);
        });
    }

    const Path& TempFolder()
    {
        return temp_folder_;
    }

    bool Contains(const std::vector<Path>& filelist, const char* path)
    {
        return std::find(filelist.begin(), filelist.end(), TempFolder() / Path{path}) != filelist.end();
    }

  private:
    std::vector<Path> files_{};
    std::vector<Path> folders_{};
    Path temp_folder_;
};

TEST_F(RecursiveDirectoryIteratorFixture, DefaultConstructedEqualEnd)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.DefaultConstructedEqualEnd' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.DefaultConstructedEqualEnd' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecursiveDirectoryIterator unit{"/"};

    EXPECT_EQ(RecursiveDirectoryIterator{}, end(unit));
}

TEST_F(RecursiveDirectoryIteratorFixture, IterateRangeBased)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.IterateRangeBased' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.IterateRangeBased' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFolder("folder_0");
    CreateFolder("folder_1");
    CreateFile("file_1.txt");
    CreateFolder("folder_1/folder_2");
    CreateFile("folder_1/file_2.txt");
    CreateFile("folder_1/folder_2/file_3.txt");
    CreateFolder("folder_1/folder_2/folder_3");
    CreateFile("folder_1/folder_2/file_4.txt");

    RecursiveDirectoryIterator unit{TempFolder()};
    std::vector<Path> found_paths{};
    for (const auto& entry : unit)
    {
        found_paths.push_back(entry.GetPath());
    }

    EXPECT_TRUE(unit.HasValue());
    EXPECT_EQ(found_paths.size(), 8);
    EXPECT_TRUE(Contains(found_paths, "folder_0"));
    EXPECT_TRUE(Contains(found_paths, "folder_1"));
    EXPECT_TRUE(Contains(found_paths, "file_1.txt"));
    EXPECT_TRUE(Contains(found_paths, "folder_1/folder_2"));
    EXPECT_TRUE(Contains(found_paths, "folder_1/file_2.txt"));
    EXPECT_TRUE(Contains(found_paths, "folder_1/folder_2/file_3.txt"));
    EXPECT_TRUE(Contains(found_paths, "folder_1/folder_2/folder_3"));
    EXPECT_TRUE(Contains(found_paths, "folder_1/folder_2/file_4.txt"));
}

TEST_F(RecursiveDirectoryIteratorFixture, IterateNonExistingDirectory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.IterateNonExistingDirectory' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.IterateNonExistingDirectory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecursiveDirectoryIterator unit{Path{"/tmp/recursive_directory_iterator_test_not_existing"}};

    EXPECT_EQ(unit, end(RecursiveDirectoryIterator{}));
    EXPECT_FALSE(unit.HasValue());
    EXPECT_EQ(unit.Error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

TEST_F(RecursiveDirectoryIteratorFixture, CanDereferenceIterator)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.CanDereferenceIterator' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.CanDereferenceIterator' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("file_1.txt");
    RecursiveDirectoryIterator unit{TempFolder()};

    const auto& path = (*unit).GetPath();

    EXPECT_STREQ(path.CStr(), (TempFolder() / "file_1.txt").CStr());
    EXPECT_TRUE(unit.HasValue());
}

TEST_F(RecursiveDirectoryIteratorFixture, ArrowOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description",
                   "Check 'RecursiveDirectoryIteratorFixture.ArrowOperator' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("file_1.txt");
    RecursiveDirectoryIterator unit{TempFolder()};

    const auto& path = unit->GetPath();

    EXPECT_STREQ(path.CStr(), (TempFolder() / "file_1.txt").CStr());
    EXPECT_TRUE(unit.HasValue());
}

TEST_F(RecursiveDirectoryIteratorFixture, IterateEmptyDirectory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.IterateEmptyDirectory' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.IterateEmptyDirectory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFolder("folder_1");

    RecursiveDirectoryIterator unit{Path{TempFolder() / "folder_1"}};

    EXPECT_EQ(unit, end(RecursiveDirectoryIterator{}));
    EXPECT_TRUE(unit.HasValue());
}

TEST_F(RecursiveDirectoryIteratorFixture, FailedReaddir)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description",
                   "Check 'RecursiveDirectoryIteratorFixture.FailedReaddir' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    test::DirentWithCorrectSize dir_entry{};
    // trick qcc to skip dirent::d_name size check, whcih is 1 on QNX:
    memcpy(&dir_entry.entry.d_name[0], "filename", sizeof("filename"));
    os::MockGuard<os::MockDirent> dirent_mock;
    ::testing::InSequence seq;
    EXPECT_CALL(*dirent_mock, opendir(_));
    EXPECT_CALL(*dirent_mock, readdir(_))
        .WillOnce(::testing::Return(score::cpp::expected<struct dirent*, score::os::Error>{&dir_entry.entry}));
    EXPECT_CALL(*dirent_mock, readdir(_))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EBADF))));
    EXPECT_CALL(*dirent_mock, closedir);

    RecursiveDirectoryIterator unit{Path{TempFolder()}};
    while (++unit != end(unit))
        ;

    EXPECT_FALSE(unit.HasValue());
}

TEST_F(RecursiveDirectoryIteratorFixture, FailedStatus)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description",
                   "Check 'RecursiveDirectoryIteratorFixture.FailedStatus' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFolder("folder_1");
    CreateFolder("folder_2");

    os::MockGuard<StandardFilesystemMock> filesystem_mock;
    EXPECT_CALL(*filesystem_mock, Status(_))
        .WillOnce(::testing::Return(MakeUnexpected(ErrorCode::kCouldNotRetrieveStatus)));

    RecursiveDirectoryIterator unit{Path{TempFolder()}};
    while (++unit != end(unit))
        ;

    EXPECT_FALSE(unit.HasValue());
}

TEST_F(RecursiveDirectoryIteratorFixture, IterateOnDefaultConstructed)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.IterateOnDefaultConstructed' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.IterateOnDefaultConstructed' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecursiveDirectoryIterator unit{};

    ++unit;

    EXPECT_EQ(RecursiveDirectoryIterator{}, end(unit));
    EXPECT_TRUE(unit.HasValue());
}

TEST_F(RecursiveDirectoryIteratorFixture, FollowSymlink)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description",
                   "Check 'RecursiveDirectoryIteratorFixture.FollowSymlink' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFolder("a");
    CreateFolder("a/b");
    CreateFolder("z");
    CreateFile("z/file");
    CreateSymlink("../../z", "a/b/symlink");

    RecursiveDirectoryIterator unit{TempFolder() / "a", DirectoryOptions::kFollowDirectorySymlink};
    std::vector<Path> found_paths{};
    for (const auto& entry : unit)
    {
        found_paths.push_back(entry.GetPath());
    }

    EXPECT_EQ(found_paths.size(), 3);
    EXPECT_TRUE(Contains(found_paths, "a/b"));
    EXPECT_TRUE(Contains(found_paths, "a/b/symlink"));
    EXPECT_TRUE(Contains(found_paths, "a/b/symlink/file"));
    EXPECT_TRUE(unit.HasValue());
}

TEST_F(RecursiveDirectoryIteratorFixture, DoNotFollowSymlink)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.DoNotFollowSymlink' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.DoNotFollowSymlink' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFolder("a");
    CreateFolder("a/b");
    CreateFolder("z");
    CreateFile("z/file");
    CreateSymlink("../../z", "a/b/symlink");

    RecursiveDirectoryIterator unit{TempFolder() / "a"};  // do not follow symlink
    std::vector<Path> found_paths{};
    for (const auto& entry : unit)
    {
        found_paths.push_back(entry.GetPath());
    }

    EXPECT_EQ(found_paths.size(), 2);
    EXPECT_TRUE(Contains(found_paths, "a/b"));
    EXPECT_TRUE(Contains(found_paths, "a/b/symlink"));
    EXPECT_TRUE(unit.HasValue());
}

TEST_F(RecursiveDirectoryIteratorFixture, ErrorInCaseCircularSymlinks)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.ErrorInCaseCircularSymlinks' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.ErrorInCaseCircularSymlinks' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFolder("a");
    CreateSymlink("../a", "a/symlink");

    RecursiveDirectoryIterator unit{TempFolder() / "a", DirectoryOptions::kFollowDirectorySymlink};
    std::vector<Path> found_paths{};
    for (const auto& entry : unit)
    {
        found_paths.push_back(entry.GetPath());
    }

    EXPECT_FALSE(unit.HasValue());
}

TEST_F(RecursiveDirectoryIteratorFixture, FailedSymlinkStatus)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.FailedSymlinkStatus' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'RecursiveDirectoryIteratorFixture.FailedSymlinkStatus' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFolder("folder_1");
    CreateFolder("folder_2");

    os::MockGuard<StandardFilesystemMock> filesystem_mock;
    EXPECT_CALL(*filesystem_mock, Status(_)).WillOnce(::testing::Invoke([](const Path&) {
        return FileStatus{FileType::kDirectory};
    }));
    EXPECT_CALL(*filesystem_mock, SymlinkStatus(_))
        .WillOnce(::testing::Return(MakeUnexpected(ErrorCode::kCouldNotRetrieveStatus)));

    RecursiveDirectoryIterator unit{Path{TempFolder()}};
    while (++unit != end(unit))
        ;

    EXPECT_FALSE(unit.HasValue());
}

}  // namespace
}  // namespace filesystem
}  // namespace score
