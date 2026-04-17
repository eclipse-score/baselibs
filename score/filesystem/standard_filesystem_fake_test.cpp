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
#include "score/filesystem/standard_filesystem_fake.h"

#include "score/filesystem/filestream/file_factory_fake.h"

#include <score/assert.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

namespace score
{
namespace filesystem
{
namespace
{

using ::testing::_;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::Unused;

class FilesystemFakeFixture : public ::testing::Test
{
  public:
    void CreateFile(const Path& path, Perms perms = Perms::kReadWriteExecUser)
    {
        ASSERT_TRUE(unit_.CreateDirectories(path.ParentPath()).has_value());
        ASSERT_TRUE(unit_.CreateRegularFile(path, perms).has_value());
    }

    void CreateDir(const Path& path)
    {
        ASSERT_TRUE(unit_.CreateDirectories(path).has_value());
    }

    FileStatus GetStatus(const Path& path)
    {
        auto status = unit_.Status(path);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(status.has_value(), "Failed to get the file status.");
        return status.value();
    }

    FileType GetType(const Path& path)
    {
        return GetStatus(path).Type();
    }

    void WriteFile(const Path& path, const char* content)
    {
        CreateDir(path.ParentPath());
        *IFileFactory::instance().Open(path, std::ios_base::in | std::ios_base::out).value() << content;
    }

    std::string GetContent(const Path& path)
    {
        return string_factory_mock_->Get(path).str();
    }

    void ExpectStatusWith(const FileType file_type, Path path)
    {
        EXPECT_CALL(unit_, Status(path)).WillRepeatedly(Return(FileStatus{file_type, Perms{}}));
    }

    void ExpectStatusWith(const filesystem::ErrorCode error_code, Path path)
    {
        EXPECT_CALL(unit_, Status(path)).WillRepeatedly(Invoke([error_code](auto) {
            return MakeUnexpected(error_code);
        }));
    }

    void ExpectLastWriteTime(const time_t mtime, Path path)
    {
        EXPECT_CALL(unit_, LastWriteTime(path)).WillRepeatedly(Invoke([mtime](auto) {
            return std::chrono::system_clock::from_time_t(mtime);
        }));
    }

    StandardFilesystemFake unit_{};
    score::os::MockGuard<FileFactoryFake> string_factory_mock_{unit_};
};

TEST_F(FilesystemFakeFixture, CreateRegularFile_Simple)
{
    CreateDir("/tmp");

    auto result = unit_.CreateRegularFile("/tmp/file", Perms::kReadWriteExecUser);

    EXPECT_TRUE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateRegularFile_FailedToCreateAlreadyExistedFile)
{
    CreateFile("/tmp/file");

    auto result = unit_.CreateRegularFile("/tmp/file", Perms::kReadWriteExecUser);

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, GetFileList)
{
    CreateFile("/tmp/file");

    auto result = unit_.GetFileList("/tmp");

    EXPECT_TRUE(result.has_value());
    const auto& filelist = result.value();
    ASSERT_EQ(filelist.size(), 1);
    EXPECT_STREQ(filelist.begin()->c_str(), "file");
}

TEST_F(FilesystemFakeFixture, GetFileListForNonExisted)
{
    auto result = unit_.GetFileList("/non-existed");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, GetFileListForNonDirectory)
{
    CreateFile("/tmp/file");

    auto result = unit_.GetFileList("/tmp/file");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, StatusFindsRegularFile)
{
    CreateFile("/tmp/file");

    auto status = unit_.Status("/tmp/file");

    ASSERT_TRUE(status.has_value());
    EXPECT_EQ(status.value().Type(), FileType::kRegular);
}

TEST_F(FilesystemFakeFixture, StatusFindsDirectory)
{
    CreateDir("/tmp");

    auto status = unit_.Status("/tmp");

    ASSERT_TRUE(status.has_value());
    EXPECT_EQ(status.value().Type(), FileType::kDirectory);
}

TEST_F(FilesystemFakeFixture, FileNotFound)
{
    const auto status = unit_.Status("/foo/bar");

    ASSERT_TRUE(status.has_value());
    FileStatus file_status{FileType::kNotFound, Perms::kUnknown};
    EXPECT_EQ(status.value(), file_status);
}

TEST_F(FilesystemFakeFixture, ExistsWhileFileExists)
{
    CreateFile("/tmp/file");

    const auto result = unit_.Exists("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, ExistsWhileFileDoesNotExists)
{
    const auto result = unit_.Exists("/foo/bar");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, RetrieveLastTime)
{
    CreateFile("/tmp/file");

    const auto time = unit_.LastWriteTime("/tmp/file");

    ASSERT_TRUE(time.has_value());
    EXPECT_LE(time.value(), std::chrono::system_clock::now());
    EXPECT_GT(time.value() + std::chrono::minutes(1), std::chrono::system_clock::now());
}

TEST_F(FilesystemFakeFixture, RetrieveLastTimeFailure)
{
    const auto time = unit_.LastWriteTime("/foo/bar");

    ASSERT_FALSE(time.has_value());
}

TEST_F(FilesystemFakeFixture, CreateDirectory)
{
    CreateDir("/tmp");

    const auto result = unit_.CreateDirectory("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(unit_.Exists("/tmp/dir").value());
}

TEST_F(FilesystemFakeFixture, CreateDirectory_EmptyName)
{
    CreateDir("/tmp");

    const auto result = unit_.CreateDirectory("/tmp/");

    ASSERT_TRUE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateDirectory_AlreadyExisted)
{
    CreateDir("/tmp");

    const auto result = unit_.CreateDirectory("/tmp");

    ASSERT_TRUE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateDirectory_ParentFolderDoesntExist)
{
    const auto result = unit_.CreateDirectory("/foo/bar");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateDirectories)
{
    CreateDir("/tmp");

    const auto result = unit_.CreateDirectories("/tmp/foo/bar");

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(unit_.Exists("/tmp/foo/bar").value());
}

TEST_F(FilesystemFakeFixture, CreateDirectoriesThatAlreadyExist)
{
    CreateDir("/tmp/foo/bar");

    const auto result = unit_.CreateDirectories("/tmp/foo/bar");

    ASSERT_TRUE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateDirectoriesInvalidPath)
{
    CreateFile("/tmp/file");

    const auto result = unit_.CreateDirectories("/tmp/file/foo");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, SetPermissionsReplace)
{
    CreateFile("/tmp/file", Perms::kReadWriteExecUser);

    const auto result = unit_.Permissions("/tmp/file", Perms::kNone, PermOptions::kReplace);

    ASSERT_TRUE(result.has_value());
    const auto status_result = unit_.Status("/tmp/file");
    ASSERT_TRUE(status_result.has_value());
    const auto status = status_result.value();
    EXPECT_EQ(status.Permissions(), Perms::kNone);
}

TEST_F(FilesystemFakeFixture, SetPermissionsAdd)
{
    CreateFile("/tmp/file", Perms::kWriteUser);

    const auto result = unit_.Permissions("/tmp/file", Perms::kReadUser, PermOptions::kAdd);

    ASSERT_TRUE(result.has_value());
    const auto status_result = unit_.Status("/tmp/file");
    ASSERT_TRUE(status_result.has_value());
    const auto status = status_result.value();
    EXPECT_EQ(status.Permissions(), Perms::kReadUser | Perms::kWriteUser);
}

TEST_F(FilesystemFakeFixture, SetPermissionsRemove)
{
    CreateFile("/tmp/file", Perms::kReadWriteExecUser);

    const auto result = unit_.Permissions("/tmp/file", Perms::kWriteUser, PermOptions::kRemove);

    ASSERT_TRUE(result.has_value());
    const auto status_result = unit_.Status("/tmp/file");
    ASSERT_TRUE(status_result.has_value());
    const auto status = status_result.value();
    EXPECT_EQ(status.Permissions(), Perms::kReadUser | Perms::kExecUser);
}

TEST_F(FilesystemFakeFixture, SetPermissionsNonExistedPath)
{
    const auto result = unit_.Permissions("/non_existed", Perms::kReadWriteExecUser, PermOptions::kReplace);

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, SetPermissionsEmptyPath)
{
    const auto result = unit_.Permissions("", Perms::kReadWriteExecUser, PermOptions::kReplace);

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, SetPermissionsInvalidOptions)
{
    CreateFile("/tmp/file", Perms::kReadWriteExecUser);

    const auto result = unit_.Permissions("/tmp/file", Perms::kReadWriteExecUser, static_cast<PermOptions>(42));

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, Remove_File)
{
    CreateFile("/tmp/file");

    const auto result = unit_.Remove("/tmp/file");

    ASSERT_TRUE(result.has_value());
    ASSERT_FALSE(unit_.Exists("/tmp/file").value());
}

TEST_F(FilesystemFakeFixture, Remove_NonExisted)
{
    const auto result = unit_.Remove("/non_existed");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, Remove_NonExistedParent)
{
    CreateDir("/tmp");

    const auto result = unit_.Remove("/tmp/non_existed/foo");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, Remove_InvalidPath)
{
    CreateFile("/tmp/file");

    const auto result = unit_.Remove("/tmp/file/foo");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, Remove_EmptyFolder)
{
    CreateDir("/tmp/dir");

    const auto result = unit_.Remove("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(unit_.Exists("/tmp/dir").value());
}

TEST_F(FilesystemFakeFixture, Remove_NonEmptyFolder)
{
    CreateDir("/tmp/dir/bar");

    const auto result = unit_.Remove("/tmp/dir");

    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(unit_.Exists("/tmp/dir/bar").value());
}

TEST_F(FilesystemFakeFixture, RemoveAll_Directory)
{
    CreateFile("/tmp/file");

    const auto result = unit_.RemoveAll("/tmp");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(unit_.Exists("/tmp").value());
}

TEST_F(FilesystemFakeFixture, RemoveAll_File)
{
    CreateFile("/tmp/file");

    const auto result = unit_.RemoveAll("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(unit_.Exists("/tmp/file").value());
    EXPECT_TRUE(unit_.Exists("/tmp").value());
}

TEST_F(FilesystemFakeFixture, RemoveAll_NonExisted)
{
    CreateFile("/tmp/file");

    const auto result = unit_.RemoveAll("/tmp/non_existed");

    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(unit_.Exists("/tmp").value());
}

TEST_F(FilesystemFakeFixture, RemoveAll_NonExistedParentPath)
{
    CreateFile("/tmp/file");

    const auto result = unit_.RemoveAll("/tmp/non_existed/foo");

    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(unit_.Exists("/tmp").value());
}

TEST_F(FilesystemFakeFixture, RemoveAll_InvalidPath)
{
    CreateFile("/tmp/file");

    // Tries to remove
    const auto result = unit_.RemoveAll("/tmp/file/foo");

    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(unit_.Exists("/tmp/file").value());
}

TEST_F(FilesystemFakeFixture, SetCurrentPath_Simple)
{
    CreateFile("/tmp/file");

    const auto result = unit_.CurrentPath("/tmp");

    ASSERT_TRUE(result.has_value());
    const auto current_path_result = unit_.CurrentPath();
    ASSERT_TRUE(current_path_result.has_value());
    EXPECT_STREQ(current_path_result.value().CStr(), "/tmp");
}

TEST_F(FilesystemFakeFixture, SetCurrentPath_NonExisted)
{
    CreateFile("/tmp/file");

    const auto result = unit_.CurrentPath("/non_existed");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CopyFile_ErrorWhileAccessingFrom)
{
    // Expecting that an error occurs while accessing the FROM file
    ExpectStatusWith(ErrorCode::kCouldNotRetrieveStatus, "/tmp/from");
    ExpectStatusWith(FileType::kNotFound, "/tmp/to");

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kNone);

    // Then copying was not successful
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kFromFileDoesNotExist);
}

TEST_F(FilesystemFakeFixture, CopyFile_TryingToCopyNotAFile)
{
    CreateDir("/tmp/from");

    // Expecting that we try to copy not a file (Directory in this case)
    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kNone);

    // Then copying was not successful
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kFromFileDoesNotExist);
}

TEST_F(FilesystemFakeFixture, CopyFile_ErrorWhileAccessingTo)
{
    ExpectStatusWith(FileType::kRegular, "/tmp/from");
    ExpectStatusWith(ErrorCode::kCouldNotRetrieveStatus, "/tmp/to");

    // Expecting that an error occurs while accessing the to file
    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kSkipExisting);

    // Then copying was not successful
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotAccessFileDuringCopy);
}

TEST_F(FilesystemFakeFixture, CopyFile_ModesOfSourceFileIsCopiedtoDestination)
{
    CreateFile("/tmp/from");
    EXPECT_TRUE(unit_.Permissions("/tmp/from", Perms::kReadUser | Perms::kWriteUser, PermOptions::kReplace));

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kNone);

    // Then copying was successful and their modes are equal
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(GetStatus("/tmp/to"), GetStatus("/tmp/from"));
}

TEST_F(FilesystemFakeFixture, CopyFile_FileContentsAreEqual)
{
    // Given a dummy file
    WriteFile("/tmp/from", "Hello World!");

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kNone);

    // Then copying was successful and the content is equal
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(GetContent("/tmp/to"), "Hello World!");
}

TEST_F(FilesystemFakeFixture, CopyFile_CopyEmptyFile)
{
    // Given a dummy file
    WriteFile("/tmp/from", "");

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kNone);

    // Then copying was successful and the content is equal
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(GetContent("/tmp/to"), "");
}

TEST_F(FilesystemFakeFixture, CopyFile_DestExistsAndNotFile)
{
    // Given a dummy file and directory
    CreateFile("/tmp/from");
    CreateDir("/tmp/dir");

    // When copying FROM to not a file
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/dir", CopyOptions::kOverwriteExisting);

    // Then copying aborts with an error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCopyFailed);
}

TEST_F(FilesystemFakeFixture, CopyFile_DestExistsAndNoOptions)
{
    // Given a file that was already copied
    CreateFile("/tmp/from");
    EXPECT_TRUE(unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kNone));

    // When copying it again
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kNone);

    // Then it fails
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCopyFailed);
}

TEST_F(FilesystemFakeFixture, CopyFile_DestExistsAndSkipsExisting)
{
    // Given two files
    WriteFile("/tmp/from", "42");
    WriteFile("/tmp/to", "Hello");

    // When copying with skipping if it already exists
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kSkipExisting);

    // Then no error is returned
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(GetContent("/tmp/to"), "Hello");
}

TEST_F(FilesystemFakeFixture, CopyFile_DestExistsAndOverwrite)
{
    // Given a file where the destination exists
    WriteFile("/tmp/from", "42");
    EXPECT_TRUE(unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kNone));
    WriteFile("/tmp/from", "Hello World.");

    // When copying with overwriting enabled
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kOverwriteExisting);

    // Then the destination is overwritten
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(GetContent("/tmp/to"), "Hello World.");
}

TEST_F(FilesystemFakeFixture, ErrorForInappropriateOption)
{
    // Given a dummy file
    WriteFile("/tmp/from", "42");
    WriteFile("/tmp/to", "111");

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kDirectoriesOnly);

    // Then nothing is copied
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(GetContent("/tmp/to"), "111");
}

TEST_F(FilesystemFakeFixture, CopyFile_DestExistsAndUpdateExisting)
{
    // Given a file that is newer then an already existing destination
    WriteFile("/tmp/from", "Hello World.");
    WriteFile("/tmp/to", "42");
    ExpectLastWriteTime(time_t{555 + 1}, "/tmp/from");
    ExpectLastWriteTime(time_t{555}, "/tmp/to");

    // When copying with updating existing enabled
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kUpdateExisting);

    // Then copying works fine and the file is updated
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(GetContent("/tmp/to"), "Hello World.");
}

TEST_F(FilesystemFakeFixture, CopyFile_DestExistsAndUpdateExistingNotNecessary)
{
    // Given a file that is older than an already existing destination
    WriteFile("/tmp/from", "Hello World.");
    WriteFile("/tmp/to", "42");
    ExpectLastWriteTime(time_t{555}, "/tmp/from");
    ExpectLastWriteTime(time_t{555 + 1}, "/tmp/to");

    // When copying with updating existing enabled
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kUpdateExisting);

    // Then nothing is copied or overwritten
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kFileNotCopied);
    EXPECT_EQ(GetContent("/tmp/to"), "42");
}

TEST_F(FilesystemFakeFixture, CopyFile_ErrorWhileRemove)
{
    // Given two files
    WriteFile("/tmp/from", "Hello World.");
    WriteFile("/tmp/to", "42");
    // Return error while Remove
    const ResultBlank error_remove = MakeUnexpected(ErrorCode::kCouldNotRemoveFileOrDirectory);
    EXPECT_CALL(unit_, Remove(Path{"/tmp/to"})).WillOnce(Return(error_remove));

    // When copying
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kOverwriteExisting);

    // Then nothing is copied or overwritten
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCopyFailed);
    EXPECT_EQ(GetContent("/tmp/to"), "42");
}

TEST_F(FilesystemFakeFixture, FileFactory_CreateNewFile)
{
    CreateDir("/tmp");

    // When creating a new file and writing some data
    {
        auto file = IFileFactory::instance().Open("/tmp/new_file", std::ios_base::in | std::ios_base::out);
        ASSERT_TRUE(file.has_value());
        *file.value() << "foo";
    }

    // Then the file exists and contains the written data
    EXPECT_TRUE(unit_.Exists("/tmp/new_file").value());
    EXPECT_STREQ(GetContent("/tmp/new_file").c_str(), "foo");
}

TEST_F(FilesystemFakeFixture, FileFactory_WriteToFileTwice)
{
    CreateDir("/tmp");
    // Given a new file with some data
    {
        auto file = IFileFactory::instance().Open("/tmp/new_file", std::ios_base::in | std::ios_base::out);
        ASSERT_TRUE(file.has_value());
        *file.value() << "foo";
    }

    // When reopening the file and writing some other data
    {
        auto file = IFileFactory::instance().Open("/tmp/new_file", std::ios_base::in | std::ios_base::out);
        ASSERT_TRUE(file.has_value());
        *file.value() << "bar";
    }

    // Then the file exist and contains all written data
    EXPECT_TRUE(unit_.Exists("/tmp/new_file").value());
    EXPECT_STREQ(GetContent("/tmp/new_file").c_str(), "foobar");
}

TEST_F(FilesystemFakeFixture, FileFactory_CreateFileInCurrentFolder)
{
    CreateDir("/tmp");

    // Given the current directory
    EXPECT_TRUE(unit_.CurrentPath("/tmp"));

    // When creating a file in current directory and writing some data
    {
        auto file = IFileFactory::instance().Open("new_file", std::ios_base::in | std::ios_base::out);
        ASSERT_TRUE(file.has_value());
        *file.value() << "foo";
    }

    // Then the file exist in current directory and contains the written data
    EXPECT_TRUE(unit_.Exists("new_file").value());
    EXPECT_STREQ(GetContent("new_file").c_str(), "foo");
    // and doesn't exist in other directory
    EXPECT_TRUE(unit_.CurrentPath("/"));
    EXPECT_FALSE(unit_.Exists("new_file").value());
}

TEST_F(FilesystemFakeFixture, FileFactory_CreateAndCopyFile)
{
    CreateDir("/tmp");

    // Given a new file with some data
    {
        auto file = IFileFactory::instance().Open("/tmp/from", std::ios_base::in | std::ios_base::out);
        ASSERT_TRUE(file.has_value());
        *file.value() << "foo";
    }

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kNone);

    // Then the TO-file exists and contains the same data
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(unit_.Exists("/tmp/to").value());
    EXPECT_STREQ(GetContent("/tmp/to").c_str(), "foo");
}

TEST_F(FilesystemFakeFixture, FileFactory_FailedToOpenEmptyFilename)
{
    CreateFile("/tmp/file");

    auto file = IFileFactory::instance().Open("/tmp/", std::ios_base::in | std::ios_base::out);

    ASSERT_FALSE(file.has_value());
}

TEST_F(FilesystemFakeFixture, FileFactory_FailedToOpenDirectoryAsAFile)
{
    CreateFile("/tmp/file");

    auto file = IFileFactory::instance().Open("/tmp", std::ios_base::in | std::ios_base::out);

    ASSERT_FALSE(file.has_value());
}

TEST_F(FilesystemFakeFixture, FileFactory_FailedToOpenInvalidPath)
{
    CreateFile("/tmp/file");

    auto file = IFileFactory::instance().Open("/tmp/file/foo", std::ios_base::in | std::ios_base::out);

    ASSERT_FALSE(file.has_value());
}

TEST_F(FilesystemFakeFixture, FileFactory_FailedToOpenEmptyPath)
{
    auto file = IFileFactory::instance().Open("", std::ios_base::in | std::ios_base::out);

    ASSERT_FALSE(file.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_Simple)
{
    WriteFile("/tmp/from", "42");

    auto result = unit_.CreateHardLink("/tmp/from", "/tmp/to");

    EXPECT_TRUE(result.has_value());
    EXPECT_STREQ(GetContent("/tmp/to").c_str(), "42");
}

TEST_F(FilesystemFakeFixture, CreateHardLink_SameData)
{
    WriteFile("/tmp/from", "42");

    auto result = unit_.CreateHardLink("/tmp/from", "/tmp/to");
    EXPECT_TRUE(result.has_value());
    // let's change the original file and check that the content has changed in other one
    WriteFile("/tmp/from", "changes");

    EXPECT_STRNE(GetContent("/tmp/to").c_str(), "42");
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorFromDoesntExist)
{
    CreateFile("/tmp/file");

    auto result = unit_.CreateHardLink("/tmp/nonexisted", "/tmp/to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorFromIsDirectory)
{
    CreateFile("/tmp/file");

    auto result = unit_.CreateHardLink("/tmp", "/tmp/to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorToParrentPathIsNotDirectory)
{
    WriteFile("/tmp/from", "42");
    WriteFile("/tmp/not_dir", "+");

    auto result = unit_.CreateHardLink("/tmp/from", "/tmp/not_dir/to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorToIsAlreadyExist)
{
    WriteFile("/tmp/from", "42");
    WriteFile("/tmp/to", "+");

    auto result = unit_.CreateHardLink("/tmp/from", "/tmp/to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorToDirectoryNonExisted)
{
    WriteFile("/tmp/from", "42");

    auto result = unit_.CreateHardLink("/tmp/from", "/non_existed/to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorEmptyFilename)
{
    WriteFile("/tmp/from", "42");

    auto result = unit_.CreateHardLink("/tmp/from", "/tmp/to/");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, HardLinkCount_File1)
{
    WriteFile("/tmp/file", "42");

    auto result = unit_.HardLinkCount("/tmp/file");

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1U);
}

TEST_F(FilesystemFakeFixture, HardLinkCount_File2)
{
    WriteFile("/tmp/from", "42");
    ASSERT_TRUE(unit_.CreateHardLink("/tmp/from", "/tmp/to").has_value());

    auto result_from = unit_.HardLinkCount("/tmp/from");
    auto result_to = unit_.HardLinkCount("/tmp/to");

    ASSERT_TRUE(result_from.has_value());
    EXPECT_EQ(result_from.value(), 2U);
    ASSERT_TRUE(result_to.has_value());
    EXPECT_EQ(result_to.value(), 2U);
}

TEST_F(FilesystemFakeFixture, HardLinkCount_Directory)
{
    CreateDir("/tmp/foo/folder1");
    CreateDir("/tmp/foo/folder2");
    CreateDir("/tmp/foo/folder3");
    CreateFile("/tmp/foo/file1");
    CreateFile("/tmp/foo/file2");
    CreateFile("/tmp/foo/file3");

    auto result = unit_.HardLinkCount("/tmp/foo");

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(),
              1U + 1U + 3U);  // parent directory + dot-entry for directory + dot-dot-entry for each subdirectory
}

TEST_F(FilesystemFakeFixture, HardLinkCount_ErrorNonExisted)
{
    CreateFile("/tmp/file");

    auto result = unit_.HardLinkCount("/tmp/non_existed");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, IsDirectory_Directory)
{
    CreateDir("/tmp/dir");

    const auto result = unit_.IsDirectory("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, IsDirectory_RegularFile)
{
    CreateFile("/tmp/file");

    const auto result = unit_.IsDirectory("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsDirectory_NonExisted)
{
    const auto result = unit_.IsDirectory("/non-existed");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsRegularFile_RegularFile)
{
    CreateFile("/tmp/file");

    const auto result = unit_.IsRegularFile("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, IsRegularFile_Directory)
{
    CreateFile("/tmp/file");

    const auto result = unit_.IsRegularFile("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsRegularFile_NonExisted)
{
    const auto result = unit_.IsRegularFile("/non-existed");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsSymlink_RegularFile)
{
    CreateFile("/tmp/file");

    const auto result = unit_.IsSymlink("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsSymlink_Symlink)
{
    EXPECT_CALL(unit_, Status(Path{"/tmp/link"})).WillOnce(Return(FileStatus{FileType::kSymlink, Perms{}}));

    const auto result = unit_.IsSymlink("/tmp/link");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, IsEmpty_EmptyFile)
{
    CreateFile("/tmp/file");

    const auto result = unit_.IsEmpty("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, IsEmpty_EmptyDirectory)
{
    CreateDir("/tmp/dir");

    const auto result = unit_.IsEmpty("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, IsEmpty_NonExisted)
{
    const auto result = unit_.IsEmpty("/non_existed");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsEmpty_NonEmptyFile)
{
    WriteFile("/tmp/file", "42");

    const auto result = unit_.IsEmpty("tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsEmpty_NonEmptyDirectory)
{
    CreateFile("/tmp/dir/file");

    const auto result = unit_.IsEmpty("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, AbsoluteForEmptyPath)
{
    EXPECT_STREQ(unit_.Absolute("").value().CStr(), "");
}

TEST_F(FilesystemFakeFixture, AbsoluteForAbsolutePaths)
{
    EXPECT_STREQ(unit_.Absolute("/").value().CStr(), "/");
    EXPECT_STREQ(unit_.Absolute("/foo").value().CStr(), "/foo");
    EXPECT_STREQ(unit_.Absolute("/foo/bar").value().CStr(), "/foo/bar");
    EXPECT_STREQ(unit_.Absolute("/foo/./bar").value().CStr(), "/foo/./bar");
    EXPECT_STREQ(unit_.Absolute("/foo/../").value().CStr(), "/foo/../");
}

TEST_F(FilesystemFakeFixture, AbsoluteForRelativePaths)
{
    EXPECT_CALL(unit_, CurrentPath()).WillRepeatedly(Return(Path{"/current/path"}));

    EXPECT_STREQ(unit_.Absolute(".").value().CStr(), "/current/path/.");
    EXPECT_STREQ(unit_.Absolute("foo").value().CStr(), "/current/path/foo");
    EXPECT_STREQ(unit_.Absolute("foo/bar").value().CStr(), "/current/path/foo/bar");
    EXPECT_STREQ(unit_.Absolute("./foo/./bar").value().CStr(), "/current/path/./foo/./bar");
    EXPECT_STREQ(unit_.Absolute("../foo/../").value().CStr(), "/current/path/../foo/../");
}

TEST_F(FilesystemFakeFixture, AbsoluteForError)
{
    EXPECT_CALL(unit_, CurrentPath()).WillOnce(Return(MakeUnexpected(ErrorCode::kCouldNotGetCurrentPath)));

    const auto res = unit_.Absolute("foo");

    EXPECT_FALSE(res.has_value());
}

TEST(IsValid, kReplace)
{
    ASSERT_TRUE(IsValid(PermOptions::kReplace));
}

TEST(IsValid, kAdd)
{
    ASSERT_TRUE(IsValid(PermOptions::kAdd));
}

TEST(IsValid, kRemove)
{
    ASSERT_TRUE(IsValid(PermOptions::kRemove));
}

TEST(IsValid, Invalid)
{
    ASSERT_FALSE(IsValid(static_cast<PermOptions>(0xFFFFFFFF)));
}

TEST(IsValid, NoOperation)
{
    ASSERT_FALSE(IsValid(PermOptions::kNoFollow));
}

}  // namespace
}  // namespace filesystem
}  // namespace score
