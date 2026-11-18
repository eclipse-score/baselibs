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
#include "score/filesystem/error.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

class FilesystemErrorTest : public ::testing::Test
{
  public:
    bool ErrorMessageContains(ErrorCode error_code, std::string_view error_message)
    {
        score::result::Error error = MakeError(error_code);
        return error.Message().find(error_message) != error.Message().npos;
    }
};

TEST_F(FilesystemErrorTest, AllMessages)
{
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotRetrieveStatus, "Could not execute stat()"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kFromFileDoesNotExist, "Input file does not exists"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotAccessFileDuringCopy, "Could not access file during copy"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCopyFailed, "Copy failed"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotSetPermissions, "Could not set permissions"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kFileNotCopied, "File was not copied"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotCreateDirectory, "Could not create directory"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotChangePermissions, "Could not change permissions"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kFileOrDirectoryDoesNotExist, "File or Directory does not exist"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotRemoveFileOrDirectory, "Could not remove file or directory"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotOpenFileStream, "Could not open filestream"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kFileOrDirectoryAlreadyExists, "File or directory already exists"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotCreateFile, "Could not create file"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kParentDirectoryDoesNotExist, "Parent directory does not exist"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kInvalidPath, "The path is invalid"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotGetCurrentPath, "Could not get current path"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotSetCurrentPath, "Could not set current path"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotCreateHardLink, "Could not create hard link"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotOpenDirectory, "Could not open directory"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotGetRealPath, "Could not get real path"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotCreateSymlink, "Could not create symbolic link"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotChangeGroup, "Could not change group"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotValidateGroup, "Could not validate group"));
    ASSERT_TRUE(
        ErrorMessageContains(ErrorCode::kCouldNotGenerateTmpName, "Could not generate a unique temporary filename"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotSyncDirectory, "Could not sync directory"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotCompareFiles, "Could not compare files"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCorruptedFileSystem, "Corrupted filesystem"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotFormatPartition, "Could not format partition"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotReadSymlink, "Could not read symlink"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kFsyncFailed, "Could not issue file sync"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCouldNotRenameFile, "Could not rename file"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kCloseFailed, "Close failed"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kNotImplemented, "Not implemented"));
    ASSERT_TRUE(ErrorMessageContains(ErrorCode::kNotARegularFile, "Not a regular file"));
}

TEST_F(FilesystemErrorTest, UserMessage)
{
    std::string_view user_message = "ABC_123";
    score::result::Error error = MakeError(ErrorCode::kCouldNotRetrieveStatus, user_message.data());
    ASSERT_EQ(error.UserMessage(), user_message);
}

TEST_F(FilesystemErrorTest, UnknownErrorCode)
{
    score::result::ErrorCode unknown_error_code = -42;
    score::result::Error error = MakeError(static_cast<ErrorCode>(unknown_error_code));
    ASSERT_EQ(*error, unknown_error_code);
    ASSERT_TRUE(error.Message().find("Unknown Error!") != error.Message().npos);
}

}  // namespace
}  // namespace filesystem
}  // namespace score
