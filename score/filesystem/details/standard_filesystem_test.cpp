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
#include "score/filesystem/details/standard_filesystem.h"

#include "score/filesystem/details/test_helper.h"
#include "score/filesystem/error.h"
#include "score/filesystem/filestream/file_factory_mock.h"
#include "score/os/mocklib/mock_dirent.h"
#include "score/os/mocklib/stat_mock.h"
#include "score/os/mocklib/stdiomock.h"
#include "score/os/mocklib/stdlib_mock.h"
#include "score/os/mocklib/unistdmock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <dirent.h>
#include <chrono>
#include <cstring>
#include <fstream>
#include <stack>

namespace score
{
namespace filesystem
{
namespace
{

using namespace ::testing;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::StrEq;

class FilesystemFixture : public ::testing::Test
{
  public:
    void ExpectStatWith(const time_t mtime,
                        std::string path = {},
                        const mode_t mode = mode_t{S_IFREG},
                        bool resolve_symlinks = true)
    {
        auto set_time = [mode, mtime](auto, auto& buffer, auto) {
            buffer.st_mode = mode;
            buffer.mtime = mtime;
            return score::cpp::expected_blank<os::Error>{};
        };
        if (path.empty())
        {
            EXPECT_CALL(*stat_mock_, stat(_, _, resolve_symlinks)).WillRepeatedly(Invoke(set_time));
        }
        else
        {
            EXPECT_CALL(*stat_mock_, stat(StrEq(path), _, resolve_symlinks)).WillRepeatedly(Invoke(set_time));
        }
    }

    void ExpectStatWith(const mode_t mode, std::string path = {}, bool resolve_symlinks = true)
    {
        auto set_mode = [mode](auto, auto& buffer, auto) {
            buffer.st_mode = mode;
            return score::cpp::expected_blank<os::Error>{};
        };
        if (path.empty())
        {
            EXPECT_CALL(*stat_mock_, stat(_, _, resolve_symlinks)).WillRepeatedly(Invoke(set_mode));
        }
        else
        {
            EXPECT_CALL(*stat_mock_, stat(StrEq(path), _, resolve_symlinks)).WillRepeatedly(Invoke(set_mode));
        }
    }

    void ExpectStatWith(std::int32_t code, std::string path = {}, bool resolve_symlinks = true)
    {
        if (path.empty())
        {
            EXPECT_CALL(*stat_mock_, stat(_, _, resolve_symlinks))
                .WillRepeatedly(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(code))));
        }
        else
        {
            EXPECT_CALL(*stat_mock_, stat(StrEq(path), _, resolve_symlinks))
                .WillRepeatedly(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(code))));
        }
    }

    void ExpectFcmodat(Perms mode)
    {
        EXPECT_CALL(*stat_mock_, fchmodat(_, _, mode, _)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
    }

    void ExpectNoFcmodat()
    {
        EXPECT_CALL(*stat_mock_, fchmodat(_, _, _, _)).Times(0);
    }

    void ExpectStatusEqual(const mode_t mode, std::pair<FileType, Perms> expected_mode)
    {
        ExpectStatWith(mode);

        const auto status = unit_.Status("/foo/bar");
        FileStatus file_status{expected_mode.first, expected_mode.second};

        ASSERT_TRUE(status.has_value());
        EXPECT_EQ(status.value(), file_status);
    }

    void ExpectRealpathWith(std::string path, std::string resolved_path)
    {
        EXPECT_CALL(*stdlib_mock_, realpath(StrEq(path.c_str()), _))
            .WillRepeatedly(Invoke([resolved_path](const char*, char* out_path) {
                return score::os::Result<char*>{strcpy(out_path, resolved_path.c_str())};
            }));
    }

    void ExpectRealpathWith(std::string path, std::int32_t code)
    {
        EXPECT_CALL(*stdlib_mock_, realpath(StrEq(path.c_str()), _))
            .WillRepeatedly(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(code))));
    }

    void ExpectGetcwdWith(std::string path)
    {
        EXPECT_CALL(*unistd_mock_, getcwd(_, _)).WillRepeatedly(Invoke([path](char* buf, size_t size) {
            return score::os::Result<char*>{strncpy(buf, path.c_str(), size)};
        }));
    }

    void PrepareDummyFile(const std::string& content)
    {
        std::ofstream dummy{};
        dummy.open("/tmp/from");
        dummy << content;
        dummy.close();

        chmod("/tmp/from", S_IRWXU | S_IRWXO);
    }

    mode_t GetMode(const std::string& path)
    {
        struct stat buffer{};
        ::stat(path.c_str(), &buffer);

        return buffer.st_mode;
    }

    std::string ReadFile(const std::string& path)
    {
        std::ifstream destination_file{path};
        std::stringstream destination_content{};
        destination_content << destination_file.rdbuf();

        return destination_content.str();
    }

    void EnvVariableExist(const char* var_name)
    {
        static const char* path = "/tmp";
        EXPECT_CALL(*stdlib_mock_, getenv(StrEq(var_name))).WillOnce(Return(const_cast<char*>(path)));
    }

    void EnvVariableDoesNotExist(const char* var_name)
    {
        EXPECT_CALL(*stdlib_mock_, getenv(StrEq(var_name))).WillOnce(Return(nullptr));
    }

    void TearDown() override
    {
        unlink("/tmp/from");
        unlink("/tmp/to");
    }

    os::MockGuard<os::StatMock> stat_mock_{};
    os::MockGuard<os::StdlibMock> stdlib_mock_{};
    os::MockGuard<os::StdioMock> stdio_mock_{};
    os::MockGuard<os::MockDirent> dirent_mock_{};
    os::MockGuard<os::UnistdMock> unistd_mock_{};
    StandardFilesystem unit_{};
};

class FilesystemFixtureWithoutMocks : public ::testing::Test
{
  public:
    void CreateFolder(const Path& path)
    {
        auto full_path = temp_folder_ / path;
        ASSERT_EQ(::mkdir(full_path.CStr(), 0755), 0);
    }

    void WriteFile(const Path& path, const char* content)
    {
        auto full_path = temp_folder_ / path;
        std::ofstream file(full_path.CStr());
        file << content;
    }

    std::string ReadFile(const Path& path)
    {
        std::ifstream file{temp_folder_ / path};
        std::stringstream content{};
        content << file.rdbuf();
        return std::move(content).str();
    }

    void CreateSymboliclink(const Path& target, const Path& link)
    {
        const Path full_path = temp_folder_ / link;
        ASSERT_EQ(::symlink(target.CStr(), full_path.CStr()), 0);
    }

    void InitTempFolder()
    {
        temp_folder_ = test::InitTempDirectoryFor("FilesystemFixtureWithoutMocks");
    }

    void SetCurrentPath(const Path& path)
    {
        ASSERT_TRUE(previous_current_path_.Empty());
        constexpr std::size_t kBufSize = static_cast<std::size_t>(PATH_MAX);
        char buf[kBufSize]{}; /* KW_SUPPRESS:AUTOSAR.BUILTIN_NUMERIC:char array for getcwd*/
        const char* getcwd_res = ::getcwd(&buf[0], kBufSize);
        ASSERT_TRUE(getcwd_res != NULL);
        previous_current_path_ = Path{getcwd_res};
        int chdir_res = ::chdir(path.CStr());
        ASSERT_EQ(chdir_res, 0);
    }

    void SetUp() override
    {
        InitTempFolder();
    }

    void TearDown() override
    {
        if (!previous_current_path_.Empty())
        {
            int chdir_res = ::chdir(previous_current_path_.CStr());
            ASSERT_EQ(chdir_res, 0);
        }
        ASSERT_TRUE(unit_.RemoveAll(TempFolder()));
    }

    const Path& TempFolder()
    {
        return temp_folder_;
    }

    StandardFilesystem unit_{};

  private:
    Path temp_folder_{};
    Path previous_current_path_;
};

TEST_F(FilesystemFixture, StatusFindsRegularFile)
{
    ExpectStatusEqual(S_IFREG | S_IRWXU, {FileType::kRegular, Perms::kReadWriteExecUser});
}

TEST_F(FilesystemFixture, StatusFindsDirectory)
{
    ExpectStatusEqual(S_IFDIR | S_IROTH, {FileType::kDirectory, Perms::kReadOthers});
}

TEST_F(FilesystemFixture, StatusFindsBlockDevice)
{
    ExpectStatusEqual(S_IFBLK | S_IRGRP | S_IXGRP, {FileType::kBlock, Perms::kReadGroup | Perms::kExecGroup});
}

TEST_F(FilesystemFixture, StatusFindsCharDevice)
{
    ExpectStatusEqual(S_IFCHR, {FileType::kCharacter, Perms::kNone});
}

TEST_F(FilesystemFixture, StatusFindsFifo)
{
    ExpectStatusEqual(S_IFIFO, {FileType::kFifo, Perms::kNone});
}

TEST_F(FilesystemFixture, StatusFindsSocket)
{
    ExpectStatusEqual(S_IFSOCK, {FileType::kSocket, Perms::kNone});
}

TEST_F(FilesystemFixture, StatusFindsUnkownType)
{
    ExpectStatusEqual(S_IROTH, {FileType::kUnknown, Perms::kReadOthers});
}

TEST_F(FilesystemFixture, NoPermissionsForStat)
{
    ExpectStatWith(EACCES);

    const auto status = unit_.Status("/foo/bar");

    ASSERT_FALSE(status.has_value());
}

TEST_F(FilesystemFixture, FileNotFound)
{
    ExpectStatWith(ENOENT);

    const auto status = unit_.Status("/foo/bar");
    FileStatus file_status{FileType::kNotFound, Perms::kUnknown};

    ASSERT_TRUE(status.has_value());
    EXPECT_EQ(status.value(), file_status);
}

TEST_F(FilesystemFixture, AnyOtherError)
{
    ExpectStatWith(ELOOP);

    const auto status = unit_.Status("/foo/bar");

    ASSERT_FALSE(status.has_value());
}

TEST_F(FilesystemFixture, ExistsWhileFileExists)
{
    ExpectStatWith(mode_t{S_IFREG});

    const auto result = unit_.Exists("/foo/bar");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFixture, ExistsWithUnkownFileType)
{
    ExpectStatWith(mode_t{});

    const auto result = unit_.Exists("/foo/bar");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFixture, ExistsWhileFileDoesNotExists)
{
    ExpectStatWith(ENOENT);

    const auto result = unit_.Exists("/foo/bar");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFixture, ExistsWithOtherError)
{
    ExpectStatWith(ELOOP);

    const auto result = unit_.Exists("/foo/bar");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFixture, RetrieveLastTime)
{
    ExpectStatWith(time_t{42});

    const auto time = unit_.LastWriteTime("/foo/bar");

    ASSERT_TRUE(time.has_value());
    EXPECT_EQ(time.value(), std::chrono::system_clock::from_time_t(42));
}

TEST_F(FilesystemFixture, RetrieveLastTimeFailure)
{
    ExpectStatWith(ELOOP);

    const auto time = unit_.LastWriteTime("/foo/bar");

    ASSERT_FALSE(time.has_value());
}

TEST_F(FilesystemFixture, WeaklyCanonicalForExistedAbsolutePath)
{
    ExpectRealpathWith("/tmp/from", "/tmp/from");

    const auto path = unit_.WeaklyCanonical("/tmp/from");

    ASSERT_TRUE(path.has_value());
    EXPECT_STREQ(path.value().CStr(), "/tmp/from");
}

TEST_F(FilesystemFixture, WeaklyCanonicalForNonExistedAbsolutePath)
{
    ExpectRealpathWith("/foo/bar", ENOENT);
    ExpectRealpathWith("/", ENOENT);

    const auto path = unit_.WeaklyCanonical("/foo/bar");

    ASSERT_TRUE(path.has_value());
    EXPECT_STREQ(path.value().CStr(), "/foo/bar");
}

TEST_F(FilesystemFixture, WeaklyCanonicalForPartlyNonExistedAbsolutePath)
{
    ExpectRealpathWith("/foo/./bar", ENOENT);
    ExpectRealpathWith("/", "/");
    ExpectRealpathWith("/foo", ENOENT);

    const auto path = unit_.WeaklyCanonical("/foo/./bar");

    ASSERT_TRUE(path.has_value());
    EXPECT_STREQ(path.value().CStr(), "/foo/bar");
}

TEST_F(FilesystemFixture, WeaklyCanonicalForRelativePath)
{
    ExpectGetcwdWith("/tmp");
    ExpectRealpathWith("/tmp/./foo/bar", "/tmp/foo/bar");

    const auto path = unit_.WeaklyCanonical("./foo/bar");

    ASSERT_TRUE(path.has_value());
    EXPECT_STREQ(path.value().CStr(), "/tmp/foo/bar");
}

TEST_F(FilesystemFixture, WeaklyCanonicalForNonExistedRelativePath)
{
    ExpectGetcwdWith("/tmp");
    ExpectRealpathWith("/tmp/foo/./bar", ENOENT);
    ExpectRealpathWith("/tmp/foo", ENOENT);

    const auto path = unit_.WeaklyCanonical("foo/./bar");

    ASSERT_TRUE(path.has_value());
    EXPECT_STREQ(path.value().CStr(), "/tmp/foo/bar");
}

TEST_F(FilesystemFixture, WeaklyCanonicalForFailedCurrentPath)
{
    EXPECT_CALL(*unistd_mock_, getcwd(_, _))
        .WillRepeatedly(Return(score::cpp::unexpected<score::os::Error>{score::os::Error::createFromErrno(ENAMETOOLONG)}));

    const auto path = unit_.WeaklyCanonical("./foo/bar");

    EXPECT_FALSE(path.has_value());
}

TEST_F(FilesystemFixture, WeaklyCanonicalForEmptyPath)
{
    const auto path = unit_.WeaklyCanonical("");

    ASSERT_TRUE(path.has_value());
    EXPECT_STREQ(path.value().CStr(), "");
}

TEST_F(FilesystemFixture, GetCurrentPath)
{
    EXPECT_CALL(*unistd_mock_, getcwd(_, _))
        .WillOnce(Return(score::cpp::expected<char*, os::Error>{const_cast<char*>("/foo/bar")}));

    const auto path = unit_.CurrentPath();

    EXPECT_TRUE(path.has_value());
    EXPECT_STREQ(path.value().CStr(), "/foo/bar");
}

TEST_F(FilesystemFixture, SetCurrentPath)
{
    const Path path{"/foo/bar"};
    EXPECT_CALL(*unistd_mock_, chdir(path.CStr())).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));

    const auto result = unit_.CurrentPath(path);

    EXPECT_TRUE(result.has_value());
}

TEST_F(FilesystemFixture, SetCurrentPathFailed)
{
    const Path path{"/foo/bar"};
    EXPECT_CALL(*unistd_mock_, chdir(path.CStr()))
        .WillOnce(Return(score::cpp::unexpected<os::Error>{score::os::Error::createFromErrno(ENOENT)}));

    const auto result = unit_.CurrentPath(path);

    EXPECT_FALSE(result.has_value());
}

using FilesystemCopyFile = FilesystemFixture;

TEST_F(FilesystemCopyFile, ErrorWhileAccessingFrom)
{
    // Expecting that an error occurs while accessing the FROM file
    ExpectStatWith(ELOOP, "/tmp/from");

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to");

    // Then copying was not successful
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kFromFileDoesNotExist);
}

TEST_F(FilesystemCopyFile, TryingToCopyNotAFile)
{
    // Expecting that we try to copy not a file (Bulk-Device in this case)
    ExpectStatWith(mode_t{S_IFBLK});

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to");

    // Then copying was not successful
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kFromFileDoesNotExist);
}

TEST_F(FilesystemCopyFile, ErrorWhileAccessingTo)
{
    // Expecting that an error occurs while accessing the to file
    ExpectStatWith(mode_t{S_IFREG}, "/tmp/from");
    ExpectStatWith(ELOOP, "/tmp/to");

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kOverwriteExisting);

    // Then copying was not successful
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotAccessFileDuringCopy);
}

TEST_F(FilesystemCopyFile, ModesOfSourceFileIsCopiedtoDestination)
{
    // Given a dummy file
    PrepareDummyFile(".");
    stat_mock_->restore_instance();

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to");

    // Then copying was successful and their modes are equal
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(GetMode("/tmp/to"), GetMode("/tmp/from"));
}

TEST_F(FilesystemCopyFile, FileContentsAreEqual)
{
    // Given a dummy file
    PrepareDummyFile("Hello World!");
    stat_mock_->restore_instance();

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to");

    // Then copying was successful and the content is equal
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(ReadFile("/tmp/to"), "Hello World!");
}

TEST_F(FilesystemCopyFile, CopyEmptyFile)
{
    // Given a dummy file
    PrepareDummyFile("");
    stat_mock_->restore_instance();

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to");

    // Then copying was successful and the content is equal
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(ReadFile("/tmp/to"), "");
}

TEST_F(FilesystemCopyFile, FullDisk)
{
    // Given a dummy file
    PrepareDummyFile("");
    stat_mock_->restore_instance();

    // Expecting that the target file is on a partition with a full disk (invalid stream)
    os::MockGuard<FileFactoryMock> file_factory_mock_;
    auto bad_ss = [](Unused, Unused) -> std::unique_ptr<std::iostream> {
        auto bad_ss_uptr = std::make_unique<std::stringstream>();
        bad_ss_uptr->setstate(std::ios_base::badbit);
        return bad_ss_uptr;
    };
    auto ok_ss = [](Unused, Unused) -> std::unique_ptr<std::iostream> {
        return std::make_unique<std::stringstream>();
    };
    EXPECT_CALL(*file_factory_mock_, Open(Eq(Path("/tmp/from")), _)).WillRepeatedly(Invoke(ok_ss));
    EXPECT_CALL(*file_factory_mock_, Open(Eq(Path("/dev/full")), _)).WillRepeatedly(Invoke(bad_ss));

    // When copying FROM to a full disk
    const auto result = unit_.CopyFile("/tmp/from", "/dev/full");

    // Then copying aborts with an error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCopyFailed);
}

TEST_F(FilesystemCopyFile, SettingPermissionsFails)
{
    // Given a dummy file
    PrepareDummyFile("");

    // Expecting that from exist and dest does not exist
    ExpectStatWith(mode_t{S_IFREG}, "/tmp/from");
    ExpectStatWith(ENOENT, "/tmp/to");

    // Expecting that chmod will fail
    EXPECT_CALL(*stat_mock_, chmod(StrEq("/tmp/to"), _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EBADF))));

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to");

    // Then copying aborts with a respective failure
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotSetPermissions);
}

TEST_F(FilesystemCopyFile, OpeningInputStreamFails)
{
    // Given that no real file exists

    // Expecting that at first that from exists and to does not
    ExpectStatWith(mode_t{S_IFREG}, "/tmp/from");
    ExpectStatWith(ENOENT, "/tmp/to");

    // When copying a file
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to");

    // Then copying aborts with an respective error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotAccessFileDuringCopy);
}

TEST_F(FilesystemCopyFile, OpeningOutputStreamFails)
{
    // Given a dummy file
    PrepareDummyFile("");

    // Expecting that FROM exists, and the path to TO does not exist
    ExpectStatWith(mode_t{S_IFREG}, "/tmp/from");
    ExpectStatWith(ENOENT, "/DOES_NOT_EXIST/to");

    // When copying a file
    const auto result = unit_.CopyFile("/tmp/from", "/DOES_NOT_EXIST/to");

    // Then copying aborts with a respective error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotAccessFileDuringCopy);
}

TEST_F(FilesystemCopyFile, DestExistsAndNotFile)
{
    // Given a dummy file and directory
    PrepareDummyFile("");
    ExpectStatWith(mode_t{S_IFREG}, "/tmp/from");
    ExpectStatWith(mode_t{S_IFDIR}, "/tmp/dir");

    // When copying FROM to not a file
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/dir", CopyOptions::kOverwriteExisting);

    // Then copying aborts with an error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCopyFailed);
}

TEST_F(FilesystemCopyFile, DestExistsAndNoOptions)
{
    // Given a file that was already copied
    PrepareDummyFile("");
    stat_mock_->restore_instance();
    EXPECT_TRUE(unit_.CopyFile("/tmp/from", "/tmp/to"));

    // When copying it again
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kNone);

    // Then it fails
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCopyFailed);
}

TEST_F(FilesystemCopyFile, DestExistsAndSkipsExisting)
{
    // Given a file that was already copied
    PrepareDummyFile("");
    stat_mock_->restore_instance();
    EXPECT_TRUE(unit_.CopyFile("/tmp/from", "/tmp/to"));

    // When copying with skipping if it already exists
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kSkipExisting);

    // Then no error is returned
    ASSERT_TRUE(result.has_value());
}

TEST_F(FilesystemCopyFile, DestExistsAndOverwrite)
{
    // Given a file where the destination exists
    PrepareDummyFile("42");
    stat_mock_->restore_instance();
    EXPECT_TRUE(unit_.CopyFile("/tmp/from", "/tmp/to"));
    PrepareDummyFile("Hello World.");

    // When copying with overwriting enabled
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kOverwriteExisting);

    // Then the destination is overwritten
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(ReadFile("/tmp/to"), "Hello World.");
}

TEST_F(FilesystemCopyFile, DestExistsAndUpdateExisting)
{
    // Given a file that is newer then an already existing destination
    PrepareDummyFile("");
    std::ignore = unit_.CopyFile("/tmp/from", "/tmp/to");
    PrepareDummyFile("Hello World.");
    ExpectStatWith(time_t{42 + 1}, "/tmp/from");
    ExpectStatWith(time_t{42}, "/tmp/to");

    // When copying with updating existing enabled
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kUpdateExisting);

    // Then copying works fine and the file is updated
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(ReadFile("/tmp/to"), "Hello World.");
}

TEST_F(FilesystemCopyFile, UpdateExistingFailedOnFirstLastWriteTime)
{
    // Given
    ::testing::InSequence seq{};
    auto set_mode = [](auto, auto& buffer, auto) {
        buffer.st_mode = mode_t{S_IFREG};
        return score::cpp::expected_blank<os::Error>{};
    };
    // Status() for from+dest
    EXPECT_CALL(*stat_mock_, stat(_, _, _)).WillOnce(Invoke(set_mode));
    EXPECT_CALL(*stat_mock_, stat(_, _, _)).WillOnce(Invoke(set_mode));
    // LastWriteTime() for from+dest
    EXPECT_CALL(*stat_mock_, stat(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));
    EXPECT_CALL(*stat_mock_, stat(_, _, _)).WillOnce(Invoke(set_mode));

    // When copying with updating existing enabled
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kUpdateExisting);

    // Then copying works fine and the file is updated
    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemCopyFile, UpdateExistingFailedOnSecondLastWriteTime)
{
    // Given
    ::testing::InSequence seq{};
    auto set_mode = [](auto, auto& buffer, auto) {
        buffer.st_mode = mode_t{S_IFREG};
        return score::cpp::expected_blank<os::Error>{};
    };
    EXPECT_CALL(*stat_mock_, stat(_, _, _)).WillOnce(Invoke(set_mode));
    EXPECT_CALL(*stat_mock_, stat(_, _, _)).WillOnce(Invoke(set_mode));
    EXPECT_CALL(*stat_mock_, stat(_, _, _)).WillOnce(Invoke(set_mode));
    EXPECT_CALL(*stat_mock_, stat(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));

    // When copying with updating existing enabled
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kUpdateExisting);

    // Then copying works fine and the file is updated
    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemCopyFile, DestExistsAndUpdateExistingNotNecessary)
{
    // Given a file that is older than an already existing destination
    PrepareDummyFile("");
    stat_mock_->restore_instance();
    EXPECT_TRUE(unit_.CopyFile("/tmp/from", "/tmp/to"));

    // When copying with updating existing enabled
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kUpdateExisting);

    // Then nothing is copied or overwritten
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kFileNotCopied);
}

TEST_F(FilesystemCopyFile, SimulateRaceConditionOnSourceFile)
{
    ::testing::InSequence seq{};

    // Given a dummy file
    PrepareDummyFile("");

    mode_t mode = mode_t{S_IFREG};
    EXPECT_CALL(*stat_mock_, stat(StrEq("/tmp/from"), _, true)).WillOnce(Invoke([mode](auto, auto& buffer, auto) {
        buffer.st_mode = mode;
        return score::cpp::expected_blank<os::Error>{};
    }));

    // Expecting that the dest does not exist
    ExpectStatWith(ENOENT, "/tmp/to");

    // Expecting that the source file gets removed after we copied, before we set the premissions
    ExpectStatWith(ELOOP, "/tmp/from");

    // When copying a file
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to");

    // Then copying aborts with a respective error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotAccessFileDuringCopy);
}

TEST_F(FilesystemCopyFile, BadSrcFile)
{
    // Given a dummy file
    PrepareDummyFile("");
    stat_mock_->restore_instance();

    os::MockGuard<FileFactoryMock> file_factory_mock_;
    auto bad_ss = [](Unused, Unused) -> std::unique_ptr<std::iostream> {
        auto bad_ss_uptr = std::make_unique<std::stringstream>();
        bad_ss_uptr->setstate(std::ios_base::badbit);
        return bad_ss_uptr;
    };
    auto ok_ss = [](Unused, Unused) -> std::unique_ptr<std::iostream> {
        return std::make_unique<std::stringstream>();
    };
    EXPECT_CALL(*file_factory_mock_, Open(Eq(Path("/tmp/from")), _)).WillOnce(Invoke(bad_ss));
    EXPECT_CALL(*file_factory_mock_, Open(Eq(Path("/tmp/to")), _)).WillOnce(Invoke(ok_ss));

    // When copying with updating existing enabled
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kOverwriteExisting);

    // Then nothing is copied or overwritten
    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemCopyFile, BadDestFile)
{
    // Given a dummy file
    PrepareDummyFile("");
    stat_mock_->restore_instance();

    os::MockGuard<FileFactoryMock> file_factory_mock_;
    auto bad_ss = [](Unused, Unused) -> std::unique_ptr<std::iostream> {
        auto bad_ss_uptr = std::make_unique<std::stringstream>();
        bad_ss_uptr->setstate(std::ios_base::badbit);
        return bad_ss_uptr;
    };
    auto ok_ss = [](Unused, Unused) -> std::unique_ptr<std::iostream> {
        return std::make_unique<std::stringstream>();
    };
    EXPECT_CALL(*file_factory_mock_, Open(Eq(Path("/tmp/from")), _)).WillOnce(Invoke(ok_ss));
    EXPECT_CALL(*file_factory_mock_, Open(Eq(Path("/tmp/to")), _)).WillOnce(Invoke(bad_ss));

    // When copying with updating existing enabled
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kOverwriteExisting);

    // Then nothing is copied or overwritten
    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemCopyFile, ErrorForInappropriateOption)
{
    // Given a dummy file
    PrepareDummyFile("Hello World!");
    stat_mock_->restore_instance();
    EXPECT_TRUE(unit_.CopyFile("/tmp/from", "/tmp/to"));

    // When copying FROM to TO
    const auto result = unit_.CopyFile("/tmp/from", "/tmp/to", CopyOptions::kDirectoriesOnly);

    // Then nothing is copied
    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFixture, CreateDirectory)
{
    // Expecting no error on mkdir
    ExpectStatWith(ENOENT);
    EXPECT_CALL(*stat_mock_, mkdir(_, _)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));

    // When creating a directory
    const auto success = unit_.CreateDirectory("/foo/bar");

    // Then the result is ok
    ASSERT_TRUE(success.has_value());
}

TEST_F(FilesystemFixture, CreateDirectoryThatAlreadyExists)
{
    // the directory is already exists
    ExpectStatWith(mode_t{S_IFDIR});

    // When creating a directory
    const auto success = unit_.CreateDirectory("/foo/bar");

    // Then the result is ok
    ASSERT_TRUE(success.has_value());
}

TEST_F(FilesystemFixture, CreateDirectoryReturnsSuccessWhenPathAlreadyExistsAndIsADirectory)
{
    // Expecting mkdir returns an error that the path already exists
    EXPECT_CALL(*stat_mock_, mkdir(_, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EEXIST))));

    // and the path is a directory
    ExpectStatWith(mode_t{S_IFDIR});

    // When creating a directory
    const auto success = unit_.CreateDirectory("/foo/bar");

    // Then the result is ok
    ASSERT_TRUE(success.has_value());
}

TEST_F(FilesystemFixture, CreateDirectoryReturnsErrorWhenPathAlreadyExistsAndIsNotADirectory)
{
    // Expecting mkdir returns an error that the path already exists
    EXPECT_CALL(*stat_mock_, mkdir(_, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EEXIST))));

    // and the path is not a directory
    ExpectStatWith(mode_t{S_IFREG});

    // When creating a directory
    const auto success = unit_.CreateDirectory("/foo/bar");

    // Then the result contains an error
    ASSERT_FALSE(success.has_value());
    EXPECT_EQ(success.error(), filesystem::ErrorCode::kCouldNotCreateDirectory);
}

TEST_F(FilesystemFixture, CreateDirectoryReturnsErrorWhenMkdirAndStatFails)
{

    EXPECT_CALL(*stat_mock_, stat(_, _, true))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EIO))));
    EXPECT_CALL(*stat_mock_, mkdir(_, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EEXIST))));

    // When creating a directory
    const auto success = unit_.CreateDirectory("/foo/bar");

    // Then the result contains an error
    ASSERT_FALSE(success.has_value());
    EXPECT_EQ(success.error(), filesystem::ErrorCode::kCouldNotCreateDirectory);
}

TEST_F(FilesystemFixture, CreateDirectoryReturnsErrorWhenPathAlreadyExistsAndIsDirectoryFails)
{
    // Expecting mkdir returns an error that the path already exists
    EXPECT_CALL(*stat_mock_, mkdir(_, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EEXIST))));

    // and checking if the path is a directory returns an error
    ExpectStatWith(ENOENT);

    // When creating a directory
    const auto success = unit_.CreateDirectory("/foo/bar");

    // Then the result contains an error
    ASSERT_FALSE(success.has_value());
    EXPECT_EQ(success.error(), filesystem::ErrorCode::kCouldNotCreateDirectory);
}

TEST_F(FilesystemFixture, CreateDirectoryReturnsErrorWhenPathDoesNotExistAndUnderlyingCreateDirectoryFails)
{
    // Expecting an error on mkdir
    EXPECT_CALL(*stat_mock_, mkdir(_, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(ELOOP))));

    // When creating a directory
    const auto success = unit_.CreateDirectory("/foo/bar");

    // Then the result contains an error
    ASSERT_FALSE(success.has_value());
    EXPECT_EQ(success.error(), filesystem::ErrorCode::kCouldNotCreateDirectory);
}

TEST_F(FilesystemFixture, CreateDirectoriesWithAllPathsExistent)
{
    // Expecting that all paths exist
    ExpectStatWith(mode_t{S_IFDIR}, "/foo");
    ExpectStatWith(mode_t{S_IFDIR}, "/foo/bar");

    // When creating a directory
    const auto success = unit_.CreateDirectories("/foo/bar");

    // Then the result is ok
    ASSERT_TRUE(success.has_value());
}

TEST_F(FilesystemFixture, CreateDirectoriesWithEmptyPath)
{
    // When creating a directory
    const auto result = unit_.CreateDirectories("");

    // Then the result contains an error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotCreateDirectory);
}

TEST_F(FilesystemFixture, CreateMissingParentDirectoriesAbsolutePath)
{
    // Expecting that the parent path does not exist, that the parent path is also created
    EXPECT_CALL(*stat_mock_, mkdir(StrEq("/foo"), _)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
    EXPECT_CALL(*stat_mock_, mkdir(StrEq("/foo/bar"), _)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
    ExpectStatWith(ENOENT, "/foo");
    ExpectStatWith(ENOENT, "/foo/bar");

    // When creating a directory
    const auto success = unit_.CreateDirectories("/foo/bar");

    // Then the result is ok
    ASSERT_TRUE(success.has_value());
}

TEST_F(FilesystemFixture, CreateMissingParentDirectoriesRelativePath)
{
    // Expecting that the parent path does not exist, that the parent path is also created
    EXPECT_CALL(*stat_mock_, mkdir(StrEq("foo"), _)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
    EXPECT_CALL(*stat_mock_, mkdir(StrEq("foo/bar"), _)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
    ExpectStatWith(ENOENT, "foo");
    ExpectStatWith(ENOENT, "foo/bar");

    // When creating a directory
    const auto success = unit_.CreateDirectories("foo/bar");

    // Then the result is ok
    ASSERT_TRUE(success.has_value());
}

TEST_F(FilesystemFixture, CreateParentDirectoriesWhileMkdirFailed)
{
    // Expecting that mkdir failed
    ExpectStatWith(ENOENT, "foo");
    EXPECT_CALL(*stat_mock_, mkdir(StrEq("foo"), _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(ELOOP))));

    // When creating a directory
    const auto success = unit_.CreateDirectories("foo/bar");

    // Then the result is ok
    ASSERT_FALSE(success.has_value());
}

TEST_F(FilesystemFixture, CreateDirectoriesWithSeparatorAtPathEnd)
{
    // Expecting that the parent path does not exist, that the parent path is also created
    EXPECT_CALL(*stat_mock_, mkdir(StrEq("foo"), _)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
    EXPECT_CALL(*stat_mock_, mkdir(StrEq("foo/bar"), _)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
    ExpectStatWith(ENOENT, "foo");
    ExpectStatWith(ENOENT, "foo/bar");

    // When creating a directory
    const auto success = unit_.CreateDirectories("foo/bar/");

    // Then the result is ok
    ASSERT_TRUE(success.has_value());
}

using TempDirectory = FilesystemFixture;

TEST_F(TempDirectory, CanGetTmpDirDirectory)
{
    EnvVariableExist("TMPDIR");
    const auto tmp_path = unit_.TempDirectoryPath();

    EXPECT_STREQ(tmp_path.value().CStr(), "/tmp");
}

TEST_F(TempDirectory, CanGetTmpDirectory)
{
    EnvVariableDoesNotExist("TMPDIR");
    EnvVariableExist("TMP");
    const auto tmp_path = unit_.TempDirectoryPath();

    EXPECT_STREQ(tmp_path.value().CStr(), "/tmp");
}

TEST_F(TempDirectory, CanGetTempDirectory)
{
    EnvVariableDoesNotExist("TMPDIR");
    EnvVariableDoesNotExist("TMP");
    EnvVariableExist("TEMP");
    const auto tmp_path = unit_.TempDirectoryPath();

    EXPECT_STREQ(tmp_path.value().CStr(), "/tmp");
}

TEST_F(TempDirectory, CanGetTempDirDirectory)
{
    EnvVariableDoesNotExist("TMPDIR");
    EnvVariableDoesNotExist("TMP");
    EnvVariableDoesNotExist("TEMP");
    EnvVariableExist("TEMPDIR");
    const auto tmp_path = unit_.TempDirectoryPath();

    EXPECT_STREQ(tmp_path.value().CStr(), "/tmp");
}

TEST_F(TempDirectory, NoEnvExists)
{
    EnvVariableDoesNotExist("TMPDIR");
    EnvVariableDoesNotExist("TMP");
    EnvVariableDoesNotExist("TEMP");
    EnvVariableDoesNotExist("TEMPDIR");
    const auto tmp_path = unit_.TempDirectoryPath();

    EXPECT_STREQ(tmp_path.value().CStr(), "/tmp");
}

using Permissions = FilesystemFixture;

TEST_F(Permissions, DoesntTryToChangeSamePermissions)
{
    ExpectStatWith(mode_t{S_IRUSR | S_IWUSR});
    ExpectNoFcmodat();
    const auto result = unit_.Permissions("/foo/bar", Perms::kReadUser | Perms::kWriteUser, PermOptions::kReplace);

    EXPECT_TRUE(result.has_value());
}

TEST_F(Permissions, OverWritesAnyPreviousPermissions)
{
    ExpectStatWith(mode_t{S_IRUSR | S_IWUSR});
    ExpectFcmodat(Perms::kReadOthers);
    const auto result = unit_.Permissions("/foo/bar", Perms::kReadOthers, PermOptions::kReplace);

    EXPECT_TRUE(result.has_value());
}

TEST_F(Permissions, AddsPermissions)
{
    ExpectStatWith(mode_t{S_IRUSR | S_IWUSR});
    ExpectFcmodat(Perms::kReadOthers | Perms::kReadUser | Perms::kWriteUser);

    const auto result = unit_.Permissions("/foo/bar", Perms::kReadOthers | Perms::kWriteUser, PermOptions::kAdd);

    EXPECT_TRUE(result.has_value());
}

TEST_F(Permissions, RemovePermissions)
{
    ExpectStatWith(mode_t{S_IRUSR | S_IROTH});
    ExpectFcmodat(Perms::kReadUser);

    const auto result = unit_.Permissions("/foo/bar", Perms::kReadOthers, PermOptions::kRemove);

    EXPECT_TRUE(result.has_value());
}

TEST_F(Permissions, ReceiveStatNotPossible)
{
    ExpectStatWith(ELOOP);

    const auto result = unit_.Permissions("/foo/bar", Perms::kReadOthers, PermOptions::kAdd);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotChangePermissions);
}

TEST_F(Permissions, UnkownOption)
{
    ExpectStatWith(mode_t{S_IRUSR});
    const auto result = unit_.Permissions("/foo/bar", Perms::kReadOthers, static_cast<PermOptions>(42));

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotChangePermissions);
}

TEST_F(Permissions, FchmodatReturnsError)
{
    ExpectStatWith(mode_t{S_IRUSR});
    EXPECT_CALL(*stat_mock_, fchmodat(_, _, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(ELOOP))));
    const auto result = unit_.Permissions("/foo/bar", Perms::kReadOthers);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotChangePermissions);
}

TEST_F(Permissions, FollowSymlink)
{
    // expect calls which follow symlink
    EXPECT_CALL(*stat_mock_, stat(_, _, true)).Times(1);
    EXPECT_CALL(*stat_mock_, stat(_, _, false)).Times(0);
    EXPECT_CALL(*stat_mock_, fchmodat(_, _, _, true)).Times(1);
    EXPECT_CALL(*stat_mock_, fchmodat(_, _, _, false)).Times(0);

    const auto result = unit_.Permissions("/foo/bar", Perms::kReadUser, PermOptions::kAdd);

    EXPECT_TRUE(result.has_value());
}

TEST_F(Permissions, NoFollowSymlink)
{
    // expect calls which NO follow symlink
    EXPECT_CALL(*stat_mock_, stat(_, _, true)).Times(0);
    EXPECT_CALL(*stat_mock_, stat(_, _, false)).Times(1);
    EXPECT_CALL(*stat_mock_, fchmodat(_, _, _, true)).Times(0);
    EXPECT_CALL(*stat_mock_, fchmodat(_, _, _, false)).Times(1);

    const auto result = unit_.Permissions("/foo/bar", Perms::kReadUser, PermOptions::kAdd | PermOptions::kNoFollow);

    EXPECT_TRUE(result.has_value());
}

class Remove : public FilesystemFixture
{
  public:
    void ExpectDirectoryReads()
    {
        EXPECT_CALL(*dirent_mock_, opendir(_))
            .WillRepeatedly(Return(score::cpp::expected<DIR*, score::os::Error>{reinterpret_cast<DIR*>(1)}));
        EXPECT_CALL(*dirent_mock_, closedir(_)).WillRepeatedly(Return(score::cpp::expected_blank<os::Error>{}));

        auto return_entries = [this](auto) -> score::cpp::expected<struct dirent*, score::os::Error> {
            if (!filesystem_.empty())
            {
                entries_.push_back({});
                strcpy(entries_.back().entry.d_name, filesystem_.top().CStr());
                filesystem_.pop();
                return &entries_.back().entry;
            }
            else
            {
                return static_cast<struct dirent*>(nullptr);
            }
        };
        EXPECT_CALL(*dirent_mock_, readdir(_)).WillRepeatedly(Invoke(return_entries));
    }

    std::vector<test::DirentWithCorrectSize> entries_{};
    std::stack<Path> filesystem_{};
};

TEST_F(Remove, CanRemoveSingleFile)
{
    EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo/bar"))).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));

    const auto result = unit_.Remove("/foo/bar");

    EXPECT_TRUE(result.has_value());
}

TEST_F(Remove, CanNotRemoveSingleFile)
{
    EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo/bar")))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(ELOOP))));

    const auto result = unit_.Remove("/foo/bar");

    EXPECT_FALSE(result.has_value());
}

using RemoveAll = Remove;

TEST_F(RemoveAll, CanRemoveSingleFile)
{
    ExpectStatWith(mode_t{S_IFREG}, "/foo/bar/file.txt", false);
    EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo/bar/file.txt"))).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));

    const auto result = unit_.RemoveAll("/foo/bar/file.txt");

    EXPECT_TRUE(result.has_value());
}

TEST_F(RemoveAll, ErrorDuringRemovingSingleFile)
{
    ExpectStatWith(mode_t{S_IFREG}, "/foo/bar/file.txt", false);
    EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo/bar/file.txt")))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(ELOOP))));

    const auto result = unit_.RemoveAll("/foo/bar/file.txt");

    EXPECT_FALSE(result.has_value());
}

TEST_F(RemoveAll, CanRemoveFolderRecursive)
{
    // Given a sub-directory and a file
    filesystem_.emplace("/foo/bar/file.txt");
    ExpectStatWith(mode_t{S_IFREG}, "/foo/bar/file.txt", true);
    ExpectStatWith(mode_t{S_IFREG}, "/foo/bar/file.txt", false);

    filesystem_.emplace("/foo/bar");
    ExpectStatWith(mode_t{S_IFDIR}, "/foo/bar", true);
    ExpectStatWith(mode_t{S_IFDIR}, "/foo/bar", false);

    ExpectDirectoryReads();
    ExpectStatWith(mode_t{S_IFDIR}, "/foo/bar", false);
    ExpectStatWith(mode_t{S_IFDIR}, "/foo", false);

    // Expecting that they get deleted
    {
        ::testing::InSequence s;
        EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo/bar/file.txt")))
            .WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
        EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo/bar"))).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
        EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo"))).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
    }

    // When removing the parent directory
    const auto result = unit_.RemoveAll("/foo");

    // Then the removal was successful
    EXPECT_TRUE(result.has_value());
}

TEST_F(RemoveAll, FailsOnDirectoryRemoval)
{
    // Given a sub-directory and a file
    filesystem_.emplace("/foo/bar/file.txt");
    ExpectStatWith(mode_t{S_IFREG}, "/foo/bar/file.txt", true);
    ExpectStatWith(mode_t{S_IFREG}, "/foo/bar/file.txt", false);

    filesystem_.emplace("/foo/bar");
    ExpectStatWith(mode_t{S_IFDIR}, "/foo/bar", true);
    ExpectStatWith(mode_t{S_IFDIR}, "/foo/bar", false);

    ExpectDirectoryReads();
    ExpectStatWith(mode_t{S_IFDIR}, "/foo", true);
    ExpectStatWith(mode_t{S_IFDIR}, "/foo", false);

    // Expecting that the OS call for directory removal fails
    EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo")))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));
    EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo/bar")))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));
    EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo/bar/file.txt"))).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));

    // When removing the parent directory
    const auto result = unit_.RemoveAll("/foo");

    // Then the removal was fails
    EXPECT_FALSE(result.has_value());
}

TEST_F(RemoveAll, FailsOnFileRemoval)
{
    // Given a sub-directory and a file
    filesystem_.emplace("/foo/bar/file.txt");
    ExpectStatWith(mode_t{S_IFREG}, "/foo/bar/file.txt", true);
    ExpectStatWith(mode_t{S_IFREG}, "/foo/bar/file.txt", false);

    filesystem_.emplace("/foo/bar");
    ExpectStatWith(mode_t{S_IFDIR}, "/foo/bar", true);
    ExpectStatWith(mode_t{S_IFDIR}, "/foo/bar", false);

    ExpectDirectoryReads();
    ExpectStatWith(mode_t{S_IFDIR}, "/foo", true);
    ExpectStatWith(mode_t{S_IFDIR}, "/foo", false);

    // Expecting that the OS call for file removal fails
    EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo")))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));
    EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo/bar")))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));
    EXPECT_CALL(*stdio_mock_, remove(StrEq("/foo/bar/file.txt")))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));

    // When removing the parent directory
    const auto result = unit_.RemoveAll("/foo");

    // Then the removal was fails
    EXPECT_FALSE(result.has_value());
}

TEST_F(RemoveAll, StatFailsOnFile)
{
    // Given a sub-directory and a file, where the stat of the file fails
    filesystem_.emplace("/foo/file.txt");
    ExpectStatWith(mode_t{S_IFREG}, "/foo/file.txt", false);  // failed in RecursiveDirectoryIterator
    ExpectStatWith(ELOOP, "/foo/file.txt", true);

    ExpectDirectoryReads();
    ExpectStatWith(mode_t{S_IFDIR}, "/foo", true);
    ExpectStatWith(mode_t{S_IFDIR}, "/foo", false);

    // When removing the parent directory
    const auto result = unit_.RemoveAll("/foo");

    // Then the removal was fails
    EXPECT_FALSE(result.has_value());
}

TEST_F(RemoveAll, LStatFailsOnFile)
{
    // Given a sub-directory and a file, where the stat of the file fails
    filesystem_.emplace("/foo/file.txt");
    ExpectStatWith(mode_t{S_IFREG}, "/foo/file.txt", true);
    ExpectStatWith(ELOOP, "/foo/file.txt", false);  // failed in RemoveAll

    ExpectDirectoryReads();
    ExpectStatWith(mode_t{S_IFDIR}, "/foo", true);
    ExpectStatWith(mode_t{S_IFDIR}, "/foo", false);

    // When removing the parent directory
    const auto result = unit_.RemoveAll("/foo");

    // Then the removal was fails
    EXPECT_FALSE(result.has_value());
}

TEST_F(RemoveAll, NonExisted)
{
    // The directory doesn't exist
    ExpectStatWith(ENOENT, "/nonexisted", false);

    // When removing the parent directory
    const auto result = unit_.RemoveAll("/nonexisted");

    // Then the removal was fails
    EXPECT_FALSE(result.has_value());
}

TEST_F(RemoveAll, FailedStatOnBaseFolder)
{
    ExpectStatWith(ELOOP, "/foo", false);

    const auto result = unit_.RemoveAll("/foo");

    EXPECT_FALSE(result.has_value());
}

using RemoveAllWithoutMocks = FilesystemFixtureWithoutMocks;

TEST_F(RemoveAllWithoutMocks, SymlinksAreNotFollowed)
{
    // Given "foo" folder with symlinks to "zzz" folder
    CreateFolder("foo");
    CreateFolder("foo/bar");
    CreateFolder("zzz");
    WriteFile("zzz/file", "Hi");
    CreateSymboliclink("../../zzz/file", "foo/bar/symlink_to_file");
    CreateSymboliclink("../../zzz", "foo/bar/symlink_to_folder");
    CreateSymboliclink("non-existed", "foo/bar/dangling_symlink");
    ASSERT_STREQ(ReadFile("foo/bar/symlink_to_file").c_str(), "Hi");
    ASSERT_STREQ(ReadFile("foo/bar/symlink_to_folder/file").c_str(), "Hi");

    const auto result = unit_.RemoveAll(TempFolder() / "foo");

    EXPECT_TRUE(result.has_value());
    // Checks that the folder "foo" has been deleted
    const auto foo_exists = unit_.Exists(TempFolder() / "foo");
    EXPECT_TRUE(foo_exists.has_value());
    EXPECT_FALSE(foo_exists.value());
    // Checks that the folder "zzz" has not been touched
    ASSERT_STREQ(ReadFile("zzz/file").c_str(), "Hi");
}

class HardLink : public FilesystemFixture
{
  public:
    void InitTempFolder()
    {
        temp_folder_ = test::InitTempDirectoryFor("hard_link_test");
    }

    void SetUp() override
    {
        InitTempFolder();
    }

    const Path& TempFolder()
    {
        return temp_folder_;
    }

  private:
    Path temp_folder_;
};

TEST_F(HardLink, Simple)
{
    {
        std::ofstream file(TempFolder() / "from");
        file << "42";
    }
    unistd_mock_->restore_instance();

    auto result = unit_.CreateHardLink(TempFolder() / "from", TempFolder() / "to");

    EXPECT_TRUE(result.has_value());
    EXPECT_STREQ(ReadFile(TempFolder() / "to").c_str(), "42");
}

TEST_F(HardLink, SameData)
{
    {
        std::ofstream file(TempFolder() / "from");
        file << "42";
    }
    unistd_mock_->restore_instance();

    auto result = unit_.CreateHardLink(TempFolder() / "from", TempFolder() / "to");

    EXPECT_TRUE(result.has_value());
    // let's change the original file and check that the content has changed in other one
    {
        std::ofstream file(TempFolder() / "from");
        file << "changes";
    }
    EXPECT_STRNE(ReadFile(TempFolder() / "to").c_str(), "42");
}

TEST_F(HardLink, ErrorFromDoesntExist)
{
    unistd_mock_->restore_instance();

    auto result = unit_.CreateHardLink(TempFolder() / "nonexisted", TempFolder() / "to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(HardLink, ErrorFromIsDirectory)
{
    unistd_mock_->restore_instance();

    auto result = unit_.CreateHardLink(TempFolder(), TempFolder() / "to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(HardLink, ErrorToIsAlreadyExist)
{
    {
        std::ofstream file(TempFolder() / "from");
        file << "42";
    }
    {
        std::ofstream file(TempFolder() / "to");
        file << "+";
    }
    unistd_mock_->restore_instance();

    auto result = unit_.CreateHardLink(TempFolder() / "from", TempFolder() / "to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(HardLink, Count)
{

    EXPECT_CALL(*stat_mock_, stat(StrEq("/tmp/from"), _, true)).WillOnce(Invoke([](auto, auto& buffer, auto) {
        buffer.st_nlink = 42U;
        return score::cpp::expected_blank<os::Error>{};
    }));

    auto result = unit_.HardLinkCount("/tmp/from");

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42U);
}

TEST_F(HardLink, ErrorCount)
{
    EXPECT_CALL(*stat_mock_, stat(StrEq("/tmp/from"), _, true))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));

    auto result = unit_.HardLinkCount("/tmp/from");

    EXPECT_FALSE(result.has_value());
}

using IsRegularFileFixture = FilesystemFixture;

TEST_F(IsRegularFileFixture, RegularFile)
{
    ExpectStatWith(mode_t{S_IFREG});

    const auto result = unit_.IsRegularFile("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(IsRegularFileFixture, Directory)
{
    ExpectStatWith(mode_t{S_IFDIR});

    const auto result = unit_.IsRegularFile("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(IsRegularFileFixture, NonExisted)
{
    ExpectStatWith(ENOENT);

    const auto result = unit_.IsRegularFile("/nonexisted");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(IsRegularFileFixture, StatusFailed)
{
    ExpectStatWith(ELOOP);

    const auto result = unit_.IsRegularFile("/record_with_failed_status");

    ASSERT_FALSE(result.has_value());
}

using IsDirectoryFixture = FilesystemFixture;

TEST_F(IsDirectoryFixture, Directory)
{
    ExpectStatWith(mode_t{S_IFDIR});

    const auto result = unit_.IsDirectory("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(IsDirectoryFixture, RegularFile)
{
    ExpectStatWith(mode_t{S_IFREG});

    const auto result = unit_.IsDirectory("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(IsDirectoryFixture, NonExisted)
{
    ExpectStatWith(ENOENT);

    const auto result = unit_.IsDirectory("/nonexisted");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(IsDirectoryFixture, StatusFailed)
{
    ExpectStatWith(ELOOP);

    const auto result = unit_.IsDirectory("/record_with_failed_status");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFixtureWithoutMocks, IsEmpty_EmptyDirectory)
{
    CreateFolder("dir");

    const auto result = unit_.IsEmpty(TempFolder() / "dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFixtureWithoutMocks, IsEmpty_EmptyRegularFile)
{
    WriteFile("from", "");

    const auto result = unit_.IsEmpty(TempFolder() / "from");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FilesystemFixtureWithoutMocks, IsEmpty_NonEmptyDirectory)
{
    CreateFolder("dir");
    WriteFile("dir/from", "");

    const auto result = unit_.IsEmpty(TempFolder() / "dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFixtureWithoutMocks, IsEmpty_NonEmptyRegularFile)
{
    WriteFile("from", "42");

    const auto result = unit_.IsEmpty(TempFolder() / "from");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFixtureWithoutMocks, IsEmpty_NonExisted)
{
    const auto result = unit_.IsEmpty("/nonexisted");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFixture, IsEmpty_StatusFailed)
{
    ExpectStatWith(ELOOP);

    const auto result = unit_.IsEmpty("/record_with_failed_status");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFixture, IsEmpty_OpendDirFailed)
{
    // Directory exists but cannot be opened
    ExpectStatWith(mode_t{S_IFDIR}, "/tmp/dir");
    EXPECT_CALL(*dirent_mock_, opendir(_))
        .WillRepeatedly(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(ELOOP))));

    const auto result = unit_.IsEmpty("/tmp/dir");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FilesystemFixture, IsEmpty_NoDirNofile)
{
    // Not a directory, not a regular file
    ExpectStatWith(mode_t{S_IFCHR}, "/tmp/dir");

    const auto result = unit_.IsEmpty("/tmp/dir");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

using IsSymlinkFixture = FilesystemFixture;

TEST_F(IsSymlinkFixture, Symlink)
{
    ExpectStatWith(mode_t{S_IFLNK}, "/tmp/symlink", false);

    const auto result = unit_.IsSymlink("/tmp/symlink");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(IsSymlinkFixture, RegularFile)
{
    ExpectStatWith(mode_t{S_IFREG}, "/tmp/file", false);

    const auto result = unit_.IsSymlink("/tmp/file");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(IsSymlinkFixture, StatusFailed)
{
    ExpectStatWith(ELOOP, "/record_with_failed_status", false);

    const auto result = unit_.IsSymlink("/record_with_failed_status");

    ASSERT_FALSE(result.has_value());
}

TEST_F(IsSymlinkFixture, DoesntExist)
{
    ExpectStatWith(ENOENT, "/non-existed", false);

    const auto result = unit_.IsSymlink("/non-existed");

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FilesystemFixtureWithoutMocks, Check_Upd_FileSystem_ClearDirContent_Steps)
{
    // This test added to check steps in Upd_FileSystem_ClearDirContent() function
    // in case the path ends with a separator

    // Let's create directory "a" with some content
    CreateFolder("a");
    CreateFolder("a/b");
    CreateFolder("a/b/c");
    WriteFile("a/file", "Hi");

    // the path ends with a separator
    const Path path = TempFolder() / "a/";

    // Upd_FileSystem_ClearDirContent steps:

    /* Check whether it is a directory */
    const auto is_directory_result = unit_.IsDirectory(path);
    ASSERT_TRUE(is_directory_result.has_value());
    EXPECT_TRUE(is_directory_result.value());

    /* Remove directory firstly */
    const auto remove_all_result = unit_.RemoveAll(path);
    EXPECT_TRUE(remove_all_result.has_value());
    EXPECT_FALSE(unit_.Exists(path).value());

    /* Create it again */
    const auto create_directories_result = unit_.CreateDirectories(path);
    EXPECT_TRUE(create_directories_result.has_value());
    EXPECT_TRUE(unit_.IsDirectory(path).value());
}

TEST_F(FilesystemFixture, AbsoluteForEmptyPath)
{
    EXPECT_STREQ(unit_.Absolute("").value().CStr(), "");
}

TEST_F(FilesystemFixture, AbsoluteForAbsolutePaths)
{
    EXPECT_STREQ(unit_.Absolute("/").value().CStr(), "/");
    EXPECT_STREQ(unit_.Absolute("/foo").value().CStr(), "/foo");
    EXPECT_STREQ(unit_.Absolute("/foo/bar").value().CStr(), "/foo/bar");
    EXPECT_STREQ(unit_.Absolute("/foo/./bar").value().CStr(), "/foo/./bar");
    EXPECT_STREQ(unit_.Absolute("/foo/../").value().CStr(), "/foo/../");
}

TEST_F(FilesystemFixture, AbsoluteForRelativePaths)
{
    EXPECT_CALL(*unistd_mock_, getcwd(_, _))
        .WillRepeatedly(Return(score::cpp::expected<char*, os::Error>{const_cast<char*>("/current/path")}));

    EXPECT_STREQ(unit_.Absolute(".").value().CStr(), "/current/path/.");
    EXPECT_STREQ(unit_.Absolute("foo").value().CStr(), "/current/path/foo");
    EXPECT_STREQ(unit_.Absolute("foo/bar").value().CStr(), "/current/path/foo/bar");
    EXPECT_STREQ(unit_.Absolute("./foo/./bar").value().CStr(), "/current/path/./foo/./bar");
    EXPECT_STREQ(unit_.Absolute("../foo/../").value().CStr(), "/current/path/../foo/../");
}

TEST_F(FilesystemFixture, AbsoluteForError)
{
    EXPECT_CALL(*unistd_mock_, getcwd(_, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));

    const auto res = unit_.Absolute("foo");

    EXPECT_FALSE(res.has_value());
}

TEST_F(FilesystemFixtureWithoutMocks, CanonicalForEmptyPath)
{
    EXPECT_FALSE(unit_.Canonical("").has_value());
}

TEST_F(FilesystemFixtureWithoutMocks, CanonicalForRelativePath)
{
    SetCurrentPath(TempFolder());
    WriteFile("file", "");
    CreateFolder("folder");

    EXPECT_STREQ(unit_.Canonical(".").value().CStr(), TempFolder().CStr());
    EXPECT_STREQ(unit_.Canonical("..").value().CStr(), TempFolder().ParentPath().CStr());
    EXPECT_STREQ(unit_.Canonical("file").value().CStr(), (TempFolder() / "file").CStr());
    EXPECT_STREQ(unit_.Canonical("folder/").value().CStr(), (TempFolder() / "folder").CStr());
}

TEST_F(FilesystemFixtureWithoutMocks, CanonicalForNonexistedPath)
{
    EXPECT_FALSE(unit_.Canonical("/non-existed").has_value());
}

TEST_F(FilesystemFixture, CanonicalForFailedGetCurrentPath)
{
    EXPECT_CALL(*unistd_mock_, getcwd(_, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));

    EXPECT_FALSE(unit_.Canonical(".").has_value());
}

TEST_F(FilesystemFixtureWithoutMocks, CanonicalForFileSymlink)
{
    SetCurrentPath(TempFolder());
    WriteFile("file", "");
    ASSERT_EQ(::symlink("./file", "./symlink"), 0);

    EXPECT_STREQ(unit_.Canonical("symlink").value().CStr(), (TempFolder() / "file").CStr());
}

TEST_F(FilesystemFixtureWithoutMocks, CanonicalForFolderSymlink)
{
    SetCurrentPath(TempFolder());

    CreateFolder("folder");
    ASSERT_EQ(::symlink("./folder/", "./symlink"), 0);

    EXPECT_STREQ(unit_.Canonical("symlink/.").value().CStr(), (TempFolder() / "folder").CStr());
}

using CreateSymlink = FilesystemFixtureWithoutMocks;

TEST_F(CreateSymlink, RelativePath)
{
    SetCurrentPath(TempFolder());
    WriteFile("target", "42");

    auto result = unit_.CreateSymlink("./target", TempFolder() / "symlink");

    EXPECT_TRUE(result.has_value());
    EXPECT_STREQ(ReadFile("symlink").c_str(), "42");
}

TEST_F(CreateSymlink, AbsolutePath)
{
    WriteFile("target", "42");

    auto result = unit_.CreateSymlink(TempFolder() / "target", TempFolder() / "symlink");

    EXPECT_TRUE(result.has_value());
    EXPECT_STREQ(ReadFile("symlink").c_str(), "42");
}

TEST_F(CreateSymlink, SameFilename)
{
    WriteFile("target", "42");

    auto result = unit_.CreateSymlink(TempFolder() / "target", TempFolder() / "symlink");

    EXPECT_TRUE(result.has_value());
    // let's remove the original file and create a new one with the same filename
    {
        ASSERT_TRUE(unit_.Remove(TempFolder() / "target").has_value());
        WriteFile("target", "abc");
    }
    EXPECT_STREQ(ReadFile("symlink").c_str(), "abc");
}

TEST_F(CreateSymlink, NoErrorFromNonExisted)
{
    SetCurrentPath(TempFolder());
    ASSERT_FALSE(unit_.Exists("./nonexisted").value());

    auto result = unit_.CreateSymlink("./nonexisted", TempFolder() / "symlink");

    EXPECT_TRUE(result.has_value());
}

TEST_F(CreateSymlink, SymlinkToDirectory)
{
    CreateFolder("folder");

    auto result = unit_.CreateSymlink(TempFolder() / "folder", TempFolder() / "symlink");

    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(unit_.IsDirectory(TempFolder() / "symlink"));
}

TEST_F(CreateSymlink, ErrorIfAlreadyExist)
{
    WriteFile("target", "");
    WriteFile("symlink", "");

    auto result = unit_.CreateSymlink(TempFolder() / "folder", TempFolder() / "symlink");

    EXPECT_FALSE(result.has_value());
}

using SymlinkStatusFixture = FilesystemFixtureWithoutMocks;

TEST_F(SymlinkStatusFixture, TargetExists)
{
    WriteFile("target", "");
    const Path symlink_path = TempFolder() / "symlink";
    EXPECT_TRUE(unit_.CreateSymlink(TempFolder() / "target", symlink_path));

    const auto result = unit_.SymlinkStatus(symlink_path);

    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().Type() == FileType::kSymlink);
}

TEST_F(SymlinkStatusFixture, TargetDoesntExist)
{
    const Path target_path = TempFolder() / "non-existed";
    ASSERT_FALSE(unit_.Exists(target_path).value());
    const Path symlink_path = TempFolder() / "symlink";
    EXPECT_TRUE(unit_.CreateSymlink(target_path, symlink_path));

    const auto result = unit_.SymlinkStatus(symlink_path);

    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().Type() == FileType::kSymlink);
}

using StatusFixture = FilesystemFixtureWithoutMocks;

TEST_F(StatusFixture, TargetDoesntExist)
{
    const Path target_path = TempFolder() / "non-existed";
    ASSERT_FALSE(unit_.Exists(target_path).value());
    const Path symlink_path = TempFolder() / "symlink";
    EXPECT_TRUE(unit_.CreateSymlink(target_path, symlink_path));

    const auto result = unit_.Status(symlink_path);

    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().Type() == FileType::kNotFound);
}

using ReadSymlinkFixture = FilesystemFixtureWithoutMocks;

TEST_F(SymlinkStatusFixture, SymlinkExists)
{
    const Path symlink_path = TempFolder() / "symlink";
    EXPECT_TRUE(unit_.CreateSymlink("foo/bar", symlink_path));

    const auto result = unit_.ReadSymlink(symlink_path);

    EXPECT_TRUE(result.has_value());
    EXPECT_STREQ(result.value().CStr(), "foo/bar");
}

TEST_F(SymlinkStatusFixture, SymlinkDoesNotExist)
{
    const Path symlink_path = TempFolder() / "symlink";

    const auto result = unit_.ReadSymlink(symlink_path);

    EXPECT_FALSE(result.has_value());
}

TEST_F(SymlinkStatusFixture, BufferTooSmall)
{
    os::MockGuard<os::UnistdMock> unistd_mock{};
    EXPECT_CALL(*unistd_mock, readlink(_, _, _)).WillOnce(Invoke([](const char*, char*, const std::size_t bufsize) {
        return bufsize;
    }));

    const auto result = unit_.ReadSymlink("foo/bar");

    EXPECT_FALSE(result.has_value());
    EXPECT_THAT(result.error().UserMessage(), HasSubstr("buffer is too small"));
}

using CreateDirectorySymlinkFixture = FilesystemFixtureWithoutMocks;

TEST_F(CreateDirectorySymlinkFixture, SymlinkToFolder)
{
    CreateFolder("foo");

    const auto result = unit_.CreateDirectorySymlink(TempFolder() / "foo", TempFolder() / "symlink");

    EXPECT_TRUE(result.has_value());
}

TEST_F(CreateDirectorySymlinkFixture, EmptySymlink)
{
    CreateFolder("foo");

    const auto result = unit_.CreateDirectorySymlink("", TempFolder() / "symlink");

    EXPECT_FALSE(result.has_value());
}

using CopySymlinkFixture = FilesystemFixtureWithoutMocks;

TEST_F(CopySymlinkFixture, NoError)
{
    const Path from = TempFolder() / "from";
    EXPECT_TRUE(unit_.CreateSymlink("foo/bar", from));
    const Path to = TempFolder() / "to";

    const auto result = unit_.CopySymlink(from, to);

    EXPECT_TRUE(result.has_value());
    const auto read_symlink_result = unit_.ReadSymlink(to);
    EXPECT_STREQ(read_symlink_result.value().CStr(), "foo/bar");
}

TEST_F(CopySymlinkFixture, ErrorDueToNonExisted)
{
    const auto result = unit_.CopySymlink(TempFolder() / "from-non-existed", TempFolder() / "to");

    EXPECT_FALSE(result.has_value());
}

TEST_F(CopySymlinkFixture, ErrorDueToAlreadyExisted)
{
    const Path from = TempFolder() / "from";
    EXPECT_TRUE(unit_.CreateSymlink("foo/bar", from));
    const Path to = TempFolder() / "to";
    WriteFile(to, "");

    const auto result = unit_.CopySymlink(from, to);

    EXPECT_FALSE(result.has_value());
}

using WeaklyCanonicalFixture = FilesystemFixtureWithoutMocks;

TEST_F(WeaklyCanonicalFixture, CanResolveSymbolicLink)
{
    SetCurrentPath(TempFolder());
    CreateFolder("foo");
    CreateSymboliclink("foo", "link-to-foo");

    const auto result = unit_.WeaklyCanonical("link-to-foo/non-existed");

    ASSERT_TRUE(result.has_value());
    const Path expected = TempFolder() / "foo/non-existed";
    EXPECT_STREQ(result.value().CStr(), expected.CStr());
}

}  // namespace
}  // namespace filesystem
}  // namespace score
