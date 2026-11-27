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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CreateRegularFile_Simple' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp");

    auto result = unit_.CreateRegularFile("/tmp/file", Perms::kReadWriteExecUser);

    EXPECT_TRUE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateRegularFile_FailedToCreateAlreadyExistedFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CreateRegularFile_FailedToCreateAlreadyExistedFile' behavior for the "
                   "filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    auto result = unit_.CreateRegularFile("/tmp/file", Perms::kReadWriteExecUser);

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, GetFileList)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FilesystemFakeFixture.GetFileList' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    auto result = unit_.GetFileList("/tmp");

    EXPECT_TRUE(result.has_value());
    const auto& filelist = result.value();
    ASSERT_EQ(filelist.size(), 1);
    EXPECT_STREQ(filelist.begin()->c_str(), "file");
}

TEST_F(FilesystemFakeFixture, GetFileListForNonExisted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.GetFileListForNonExisted' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    auto result = unit_.GetFileList("/non-existed");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, GetFileListForNonDirectory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.GetFileListForNonDirectory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    auto result = unit_.GetFileList("/tmp/file");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, StatusFindsRegularFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.StatusFindsRegularFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    auto status = unit_.Status("/tmp/file");

    ASSERT_TRUE(status.has_value());
    EXPECT_EQ(status.value().Type(), FileType::kRegular);
}

TEST_F(FilesystemFakeFixture, StatusFindsDirectory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.StatusFindsDirectory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp");

    auto status = unit_.Status("/tmp");

    ASSERT_TRUE(status.has_value());
    EXPECT_EQ(status.value().Type(), FileType::kDirectory);
}

TEST_F(FilesystemFakeFixture, FileNotFound)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FilesystemFakeFixture.FileNotFound' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto status = unit_.Status("/foo/bar");

    ASSERT_TRUE(status.has_value());
    FileStatus file_status{FileType::kNotFound, Perms::kUnknown};
    EXPECT_EQ(status.value(), file_status);
}

TEST_F(FilesystemFakeFixture, ExistsWhileFileExists)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.ExistsWhileFileExists' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.Exists("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, ExistsWhileFileDoesNotExists)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.ExistsWhileFileDoesNotExists' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto result = unit_.Exists("/foo/bar");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, RetrieveLastTime)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.RetrieveLastTime' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto time = unit_.LastWriteTime("/tmp/file");

    ASSERT_TRUE(time.has_value());
    EXPECT_LE(time.value(), std::chrono::system_clock::now());
    EXPECT_GT(time.value() + std::chrono::minutes(1), std::chrono::system_clock::now());
}

TEST_F(FilesystemFakeFixture, RetrieveLastTimeFailure)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.RetrieveLastTimeFailure' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto time = unit_.LastWriteTime("/foo/bar");

    ASSERT_FALSE(time.has_value());
}

TEST_F(FilesystemFakeFixture, CreateDirectory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CreateDirectory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp");

    const auto result = unit_.CreateDirectory("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(unit_.Exists("/tmp/dir").value());
}

TEST_F(FilesystemFakeFixture, CreateDirectory_EmptyName)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CreateDirectory_EmptyName' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp");

    const auto result = unit_.CreateDirectory("/tmp/");

    ASSERT_TRUE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateDirectory_AlreadyExisted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateDirectory_AlreadyExisted' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateDirectory_AlreadyExisted' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp");

    const auto result = unit_.CreateDirectory("/tmp");

    ASSERT_TRUE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateDirectory_ParentFolderDoesntExist)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateDirectory_ParentFolderDoesntExist' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateDirectory_ParentFolderDoesntExist' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto result = unit_.CreateDirectory("/foo/bar");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateDirectories)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CreateDirectories' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp");

    const auto result = unit_.CreateDirectories("/tmp/foo/bar");

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(unit_.Exists("/tmp/foo/bar").value());
}

TEST_F(FilesystemFakeFixture, CreateDirectoriesThatAlreadyExist)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateDirectoriesThatAlreadyExist' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateDirectoriesThatAlreadyExist' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp/foo/bar");

    const auto result = unit_.CreateDirectories("/tmp/foo/bar");

    ASSERT_TRUE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateDirectoriesInvalidPath)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CreateDirectoriesInvalidPath' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.CreateDirectories("/tmp/file/foo");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, SetPermissionsReplace)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.SetPermissionsReplace' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.SetPermissionsAdd' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.SetPermissionsRemove' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.SetPermissionsNonExistedPath' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto result = unit_.Permissions("/non_existed", Perms::kReadWriteExecUser, PermOptions::kReplace);

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, SetPermissionsEmptyPath)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.SetPermissionsEmptyPath' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto result = unit_.Permissions("", Perms::kReadWriteExecUser, PermOptions::kReplace);

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, SetPermissionsInvalidOptions)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.SetPermissionsInvalidOptions' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file", Perms::kReadWriteExecUser);

    const auto result = unit_.Permissions("/tmp/file", Perms::kReadWriteExecUser, static_cast<PermOptions>(42));

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, Remove_File)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FilesystemFakeFixture.Remove_File' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.Remove("/tmp/file");

    ASSERT_TRUE(result.has_value());
    ASSERT_FALSE(unit_.Exists("/tmp/file").value());
}

TEST_F(FilesystemFakeFixture, Remove_NonExisted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.Remove_NonExisted' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto result = unit_.Remove("/non_existed");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, Remove_NonExistedParent)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.Remove_NonExistedParent' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp");

    const auto result = unit_.Remove("/tmp/non_existed/foo");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, Remove_InvalidPath)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.Remove_InvalidPath' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.Remove("/tmp/file/foo");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, Remove_EmptyFolder)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.Remove_EmptyFolder' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp/dir");

    const auto result = unit_.Remove("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(unit_.Exists("/tmp/dir").value());
}

TEST_F(FilesystemFakeFixture, Remove_NonEmptyFolder)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.Remove_NonEmptyFolder' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp/dir/bar");

    const auto result = unit_.Remove("/tmp/dir");

    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(unit_.Exists("/tmp/dir/bar").value());
}

TEST_F(FilesystemFakeFixture, RemoveAll_Directory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.RemoveAll_Directory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.RemoveAll("/tmp");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(unit_.Exists("/tmp").value());
}

TEST_F(FilesystemFakeFixture, RemoveAll_File)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.RemoveAll_File' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.RemoveAll("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(unit_.Exists("/tmp/file").value());
    EXPECT_TRUE(unit_.Exists("/tmp").value());
}

TEST_F(FilesystemFakeFixture, RemoveAll_NonExisted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.RemoveAll_NonExisted' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.RemoveAll("/tmp/non_existed");

    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(unit_.Exists("/tmp").value());
}

TEST_F(FilesystemFakeFixture, RemoveAll_NonExistedParentPath)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.RemoveAll_NonExistedParentPath' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.RemoveAll_NonExistedParentPath' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.RemoveAll("/tmp/non_existed/foo");

    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(unit_.Exists("/tmp").value());
}

TEST_F(FilesystemFakeFixture, RemoveAll_InvalidPath)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.RemoveAll_InvalidPath' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    // Tries to remove
    const auto result = unit_.RemoveAll("/tmp/file/foo");

    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(unit_.Exists("/tmp/file").value());
}

TEST_F(FilesystemFakeFixture, SetCurrentPath_Simple)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.SetCurrentPath_Simple' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.CurrentPath("/tmp");

    ASSERT_TRUE(result.has_value());
    const auto current_path_result = unit_.CurrentPath();
    ASSERT_TRUE(current_path_result.has_value());
    EXPECT_STREQ(current_path_result.value().CStr(), "/tmp");
}

TEST_F(FilesystemFakeFixture, SetCurrentPath_NonExisted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.SetCurrentPath_NonExisted' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.CurrentPath("/non_existed");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CopyFile_ErrorWhileAccessingFrom)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_ErrorWhileAccessingFrom' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_ErrorWhileAccessingFrom' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_TryingToCopyNotAFile' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_TryingToCopyNotAFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_ErrorWhileAccessingTo' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_ErrorWhileAccessingTo' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CopyFile_ModesOfSourceFileIsCopiedtoDestination' behavior for the "
                   "filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_FileContentsAreEqual' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_FileContentsAreEqual' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CopyFile_CopyEmptyFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndNotFile' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndNotFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndNoOptions' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndNoOptions' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndSkipsExisting' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndSkipsExisting' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndOverwrite' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndOverwrite' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.ErrorForInappropriateOption' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndUpdateExisting' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndUpdateExisting' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CopyFile_DestExistsAndUpdateExistingNotNecessary' behavior for the "
                   "filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CopyFile_ErrorWhileRemove' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two files
    WriteFile("/tmp/from", "Hello World.");
    WriteFile("/tmp/to", "42");
    // Return error while Remove
    const Result<void> error_remove = MakeUnexpected(ErrorCode::kCouldNotRemoveFileOrDirectory);
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.FileFactory_CreateNewFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.FileFactory_WriteToFileTwice' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.FileFactory_CreateFileInCurrentFolder' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.FileFactory_CreateFileInCurrentFolder' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.FileFactory_CreateAndCopyFile' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.FileFactory_CreateAndCopyFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.FileFactory_FailedToOpenEmptyFilename' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.FileFactory_FailedToOpenEmptyFilename' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    auto file = IFileFactory::instance().Open("/tmp/", std::ios_base::in | std::ios_base::out);

    ASSERT_FALSE(file.has_value());
}

TEST_F(FilesystemFakeFixture, FileFactory_FailedToOpenDirectoryAsAFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.FileFactory_FailedToOpenDirectoryAsAFile' behavior for the filesystem "
                   "component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    auto file = IFileFactory::instance().Open("/tmp", std::ios_base::in | std::ios_base::out);

    ASSERT_FALSE(file.has_value());
}

TEST_F(FilesystemFakeFixture, FileFactory_FailedToOpenInvalidPath)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.FileFactory_FailedToOpenInvalidPath' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.FileFactory_FailedToOpenInvalidPath' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    auto file = IFileFactory::instance().Open("/tmp/file/foo", std::ios_base::in | std::ios_base::out);

    ASSERT_FALSE(file.has_value());
}

TEST_F(FilesystemFakeFixture, FileFactory_FailedToOpenEmptyPath)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.FileFactory_FailedToOpenEmptyPath' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.FileFactory_FailedToOpenEmptyPath' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    auto file = IFileFactory::instance().Open("", std::ios_base::in | std::ios_base::out);

    ASSERT_FALSE(file.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_Simple)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CreateHardLink_Simple' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    WriteFile("/tmp/from", "42");

    auto result = unit_.CreateHardLink("/tmp/from", "/tmp/to");

    EXPECT_TRUE(result.has_value());
    EXPECT_STREQ(GetContent("/tmp/to").c_str(), "42");
}

TEST_F(FilesystemFakeFixture, CreateHardLink_SameData)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CreateHardLink_SameData' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    WriteFile("/tmp/from", "42");

    auto result = unit_.CreateHardLink("/tmp/from", "/tmp/to");
    EXPECT_TRUE(result.has_value());
    // let's change the original file and check that the content has changed in other one
    WriteFile("/tmp/from", "changes");

    EXPECT_STRNE(GetContent("/tmp/to").c_str(), "42");
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorFromDoesntExist)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateHardLink_ErrorFromDoesntExist' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateHardLink_ErrorFromDoesntExist' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    auto result = unit_.CreateHardLink("/tmp/nonexisted", "/tmp/to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorFromIsDirectory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateHardLink_ErrorFromIsDirectory' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateHardLink_ErrorFromIsDirectory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    auto result = unit_.CreateHardLink("/tmp", "/tmp/to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorToParrentPathIsNotDirectory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CreateHardLink_ErrorToParrentPathIsNotDirectory' behavior for the "
                   "filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    WriteFile("/tmp/from", "42");
    WriteFile("/tmp/not_dir", "+");

    auto result = unit_.CreateHardLink("/tmp/from", "/tmp/not_dir/to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorToIsAlreadyExist)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateHardLink_ErrorToIsAlreadyExist' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateHardLink_ErrorToIsAlreadyExist' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    WriteFile("/tmp/from", "42");
    WriteFile("/tmp/to", "+");

    auto result = unit_.CreateHardLink("/tmp/from", "/tmp/to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorToDirectoryNonExisted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.CreateHardLink_ErrorToDirectoryNonExisted' behavior for the "
                   "filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    WriteFile("/tmp/from", "42");

    auto result = unit_.CreateHardLink("/tmp/from", "/non_existed/to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, CreateHardLink_ErrorEmptyFilename)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateHardLink_ErrorEmptyFilename' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.CreateHardLink_ErrorEmptyFilename' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    WriteFile("/tmp/from", "42");

    auto result = unit_.CreateHardLink("/tmp/from", "/tmp/to/");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, HardLinkCount_File1)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.HardLinkCount_File1' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    WriteFile("/tmp/file", "42");

    auto result = unit_.HardLinkCount("/tmp/file");

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1U);
}

TEST_F(FilesystemFakeFixture, HardLinkCount_File2)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.HardLinkCount_File2' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.HardLinkCount_Directory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.HardLinkCount_ErrorNonExisted' behavior for the filesystem component.");
    RecordProperty(
        "Description",
        "Check 'FilesystemFakeFixture.HardLinkCount_ErrorNonExisted' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    auto result = unit_.HardLinkCount("/tmp/non_existed");

    EXPECT_FALSE(result.has_value());
}

TEST_F(FilesystemFakeFixture, IsDirectory_Directory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsDirectory_Directory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp/dir");

    const auto result = unit_.IsDirectory("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, IsDirectory_RegularFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsDirectory_RegularFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.IsDirectory("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsDirectory_NonExisted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsDirectory_NonExisted' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto result = unit_.IsDirectory("/non-existed");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsRegularFile_RegularFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsRegularFile_RegularFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.IsRegularFile("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, IsRegularFile_Directory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsRegularFile_Directory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp/dir");

    const auto result = unit_.IsRegularFile("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsRegularFile_NonExisted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsRegularFile_NonExisted' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto result = unit_.IsRegularFile("/non-existed");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsSymlink_RegularFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsSymlink_RegularFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.IsSymlink("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsSymlink_Symlink)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsSymlink_Symlink' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    EXPECT_CALL(unit_, Status(Path{"/tmp/link"})).WillOnce(Return(FileStatus{FileType::kSymlink, Perms{}}));

    const auto result = unit_.IsSymlink("/tmp/link");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, IsEmpty_EmptyFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsEmpty_EmptyFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/file");

    const auto result = unit_.IsEmpty("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, IsEmpty_EmptyDirectory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsEmpty_EmptyDirectory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateDir("/tmp/dir");

    const auto result = unit_.IsEmpty("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFakeFixture, IsEmpty_NonExisted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsEmpty_NonExisted' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto result = unit_.IsEmpty("/non_existed");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsEmpty_NonEmptyFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsEmpty_NonEmptyFile' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    WriteFile("/tmp/file", "42");

    const auto result = unit_.IsEmpty("tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, IsEmpty_NonEmptyDirectory)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.IsEmpty_NonEmptyDirectory' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CreateFile("/tmp/dir/file");

    const auto result = unit_.IsEmpty("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFakeFixture, AbsoluteForEmptyPath)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.AbsoluteForEmptyPath' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    EXPECT_STREQ(unit_.Absolute("").value().CStr(), "");
}

TEST_F(FilesystemFakeFixture, AbsoluteForAbsolutePaths)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.AbsoluteForAbsolutePaths' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    EXPECT_STREQ(unit_.Absolute("/").value().CStr(), "/");
    EXPECT_STREQ(unit_.Absolute("/foo").value().CStr(), "/foo");
    EXPECT_STREQ(unit_.Absolute("/foo/bar").value().CStr(), "/foo/bar");
    EXPECT_STREQ(unit_.Absolute("/foo/./bar").value().CStr(), "/foo/./bar");
    EXPECT_STREQ(unit_.Absolute("/foo/../").value().CStr(), "/foo/../");
}

TEST_F(FilesystemFakeFixture, AbsoluteForRelativePaths)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.AbsoluteForRelativePaths' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    EXPECT_CALL(unit_, CurrentPath()).WillRepeatedly(Return(Path{"/current/path"}));

    EXPECT_STREQ(unit_.Absolute(".").value().CStr(), "/current/path/.");
    EXPECT_STREQ(unit_.Absolute("foo").value().CStr(), "/current/path/foo");
    EXPECT_STREQ(unit_.Absolute("foo/bar").value().CStr(), "/current/path/foo/bar");
    EXPECT_STREQ(unit_.Absolute("./foo/./bar").value().CStr(), "/current/path/./foo/./bar");
    EXPECT_STREQ(unit_.Absolute("../foo/../").value().CStr(), "/current/path/../foo/../");
}

TEST_F(FilesystemFakeFixture, AbsoluteForError)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FilesystemFakeFixture.AbsoluteForError' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    EXPECT_CALL(unit_, CurrentPath()).WillOnce(Return(MakeUnexpected(ErrorCode::kCouldNotGetCurrentPath)));

    const auto res = unit_.Absolute("foo");

    EXPECT_FALSE(res.has_value());
}

TEST(IsValid, kReplace)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'IsValid.kReplace' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    ASSERT_TRUE(IsValid(PermOptions::kReplace));
}

TEST(IsValid, kAdd)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'IsValid.kAdd' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    ASSERT_TRUE(IsValid(PermOptions::kAdd));
}

TEST(IsValid, kRemove)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'IsValid.kRemove' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    ASSERT_TRUE(IsValid(PermOptions::kRemove));
}

TEST(IsValid, Invalid)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'IsValid.Invalid' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    ASSERT_FALSE(IsValid(static_cast<PermOptions>(0xFFFFFFFF)));
}

TEST(IsValid, NoOperation)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'IsValid.NoOperation' behavior for the filesystem component.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    ASSERT_FALSE(IsValid(PermOptions::kNoFollow));
}

}  // namespace
}  // namespace filesystem
}  // namespace score
