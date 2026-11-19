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

namespace score
{
namespace filesystem
{

namespace
{
class FilesystemErrorDomain final : public score::result::ErrorDomain
{
  public:
    // This method contains a large switch statement to map error enumeration values to corresponding error messages.
    // Currently, no alternative approach provides equivalent clarity or maintainability. This strategy has been
    // reviewed and adopted as a project-wide convention for error mapping hence, suppressing the warning.
    // SCORE_CCM_NO_LINT
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        // coverity[autosar_cpp14_m6_4_3_violation]
        switch (code)
        {
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotRetrieveStatus):
                return "Could not execute stat()";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kFromFileDoesNotExist):
                return "Input file does not exists";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotAccessFileDuringCopy):
                return "Could not access file during copy";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCopyFailed):
                return "Copy failed";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotSetPermissions):
                return "Could not set permissions";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kFileNotCopied):
                return "File was not copied";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotCreateDirectory):
                return "Could not create directory";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotChangePermissions):
                return "Could not change permissions";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kFileOrDirectoryDoesNotExist):
                return "File or Directory does not exist";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotRemoveFileOrDirectory):
                return "Could not remove file or directory";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotOpenFileStream):
                return "Could not open filestream";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kFileOrDirectoryAlreadyExists):
                return "File or directory already exists";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotCreateFile):
                return "Could not create file";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kParentDirectoryDoesNotExist):
                return "Parent directory does not exist";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kInvalidPath):
                return "The path is invalid";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotGetCurrentPath):
                return "Could not get current path";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotSetCurrentPath):
                return "Could not set current path";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotCreateHardLink):
                return "Could not create hard link";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotOpenDirectory):
                return "Could not open directory";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotGetRealPath):
                return "Could not get real path";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotCreateSymlink):
                return "Could not create symbolic link";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotChangeGroup):
                return "Could not change group";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotValidateGroup):
                return "Could not validate group";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotGenerateTmpName):
                return "Could not generate a unique temporary filename";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotSyncDirectory):
                return "Could not sync directory";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotCompareFiles):
                return "Could not compare files";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCorruptedFileSystem):
                return "Corrupted filesystem";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotFormatPartition):
                return "Could not format partition";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotReadSymlink):
                return "Could not read symlink";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kFsyncFailed):
                return "Could not issue file sync";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCouldNotRenameFile):
                return "Could not rename file";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kNotImplemented):
                return "Not implemented";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kCloseFailed):
                return "Close failed";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case static_cast<score::result::ErrorCode>(ErrorCode::kNotARegularFile):
                return "Not a regular file";
            // coverity[autosar_cpp14_m6_4_5_violation]
            default:
                return "Unknown Error!";
        }
    }
};

constexpr FilesystemErrorDomain filesystem_error_domain;
}  // namespace

score::result::Error MakeError(const ErrorCode code, const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), filesystem_error_domain, user_message};
}

}  // namespace filesystem
}  // namespace score
