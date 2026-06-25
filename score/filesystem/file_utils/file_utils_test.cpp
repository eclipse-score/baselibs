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
#include "score/filesystem/file_utils/file_utils.h"

#include "score/filesystem/error.h"
#include "score/filesystem/filestream/file_factory_mock.h"
#include "score/filesystem/standard_filesystem_mock.h"
#include "score/os/mocklib/fcntl_mock.h"
#include "score/os/mocklib/grpmock.h"
#include "score/os/mocklib/stat_mock.h"
#include "score/os/mocklib/stdlib_mock.h"
#include "score/os/mocklib/unistdmock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

using ::testing::_;
using ::testing::ByMove;
using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::Return;
using ::testing::Sequence;
using ::testing::StartsWith;
using ::testing::StrEq;
using ::testing::StrNe;
using ::testing::Unused;

class FileUtilsTest : public ::testing::Test
{
  public:
    // error constants
    const Result<void> no_error{};
    const Result<void> error_create_directory = MakeUnexpected(ErrorCode::kCouldNotCreateDirectory);
    const Result<FileStatus> error_is_directory = MakeUnexpected(filesystem::ErrorCode::kCouldNotRetrieveStatus);
    const Result<void> error_permissions = MakeUnexpected(filesystem::ErrorCode::kCouldNotChangePermissions);
    const score::cpp::expected_blank<score::os::Error> os_no_error{};
    const score::cpp::expected_blank<score::os::Error> os_enoent = score::cpp::make_unexpected(score::os::Error::createFromErrno(ENOENT));
    const score::cpp::expected<os::GroupBuffer, score::os::Error> os_getgrnam_enoent =
        score::cpp::make_unexpected(score::os::Error::createFromErrno(ENOENT));
    const score::cpp::expected_blank<score::os::Error> os_eio = score::cpp::make_unexpected(score::os::Error::createFromErrno(EIO));

    // mocks
    score::os::MockGuard<score::filesystem::StandardFilesystemMock> filesystemMock_;
    score::os::MockGuard<score::filesystem::FileFactoryMock> filefactory_mock_;
    score::os::MockGuard<score::os::StatMock> stat_mock_;
    score::os::MockGuard<score::os::StdlibMock> stdlib_mock_;
    score::os::MockGuard<score::os::GrpMock> grp_mock_;
    score::os::MockGuard<score::os::UnistdMock> unistd_mock_;
    score::os::MockGuard<score::os::FcntlMock> fcntl_mock_;

    // others
    const score::os::Stat::Mode permissions = score::os::Stat::Mode::kReadWriteExecUser |
                                            score::os::Stat::Mode::kReadWriteExecGroup |
                                            score::os::Stat::Mode::kReadOthers | score::os::Stat::Mode::kExecOthers;

    // object for test
    score::filesystem::FileUtils unit_{*filesystemMock_, *filefactory_mock_};
};

TEST_F(FileUtilsTest, CreateDirectory)
{
    EXPECT_CALL(*filesystemMock_, Status(Path{"/foo/bar"})).WillOnce(Return(FileStatus{FileType::kNotFound}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/foo/bar"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"/foo/bar"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));

    const auto success = unit_.CreateDirectory("/foo/bar", permissions);

    ASSERT_TRUE(success.has_value());
}

TEST_F(FileUtilsTest, CreateDirectoryThatAlreadyExistsWithCorrectPermissions)
{
    EXPECT_CALL(*filesystemMock_, Status(Path{"/foo/bar"}))
        .WillOnce(Return(FileStatus{FileType::kDirectory, permissions}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(_)).Times(0);
    EXPECT_CALL(*filesystemMock_, Permissions(_, _, _)).Times(0);

    const auto success = unit_.CreateDirectory("/foo/bar", permissions);

    ASSERT_TRUE(success.has_value());
}

TEST_F(FileUtilsTest, CreateDirectoryThatAlreadyExistsWithWrongPermissions)
{
    EXPECT_CALL(*filesystemMock_, Status(Path{"/foo/bar"}))
        .WillOnce(Return(FileStatus{FileType::kDirectory, Perms::kNone}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(_)).Times(0);
    EXPECT_CALL(*filesystemMock_, Permissions(_, _, _)).Times(0);

    const auto success = unit_.CreateDirectory("/foo/bar", permissions);

    ASSERT_FALSE(success.has_value());
}

TEST_F(FileUtilsTest, CreateDirectoryThatAlreadyExistsAsFile)
{
    EXPECT_CALL(*filesystemMock_, Status(Path{"/foo/bar"}))
        .WillOnce(Return(FileStatus{FileType::kRegular, Perms::kWriteUser}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(_)).Times(0);
    EXPECT_CALL(*filesystemMock_, Permissions(_, _, _)).Times(0);

    const auto success = unit_.CreateDirectory("/foo/bar", permissions);

    ASSERT_FALSE(success.has_value());
}

TEST_F(FileUtilsTest, CreateDirectoryFailsOnCreate)
{
    EXPECT_CALL(*filesystemMock_, Status(Path{"/foo/bar"})).WillOnce(Return(FileStatus{FileType::kNotFound}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/foo/bar"})).WillOnce(Return(error_create_directory));
    EXPECT_CALL(*filesystemMock_, Permissions(_, _, _)).Times(0);

    const auto success = unit_.CreateDirectory("/foo/bar", permissions);

    ASSERT_FALSE(success.has_value());
    EXPECT_EQ(success.error(), ErrorCode::kCouldNotCreateDirectory);
}

TEST_F(FileUtilsTest, CreateDirectoryFailsOnIsDirectory)
{
    EXPECT_CALL(*filesystemMock_, Status(Path{"/foo/bar"})).WillOnce(Return(error_is_directory));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(_)).Times(0);
    EXPECT_CALL(*filesystemMock_, Permissions(_, _, _)).Times(0);

    const auto success = unit_.CreateDirectory("/foo/bar", permissions);

    ASSERT_FALSE(success.has_value());
    EXPECT_EQ(success.error(), ErrorCode::kCouldNotCreateDirectory);
}

TEST_F(FileUtilsTest, CreateDirectoryFailsOnPermissions)
{
    EXPECT_CALL(*filesystemMock_, Status(Path{"/foo/bar"})).WillOnce(Return(FileStatus{FileType::kNotFound}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/foo/bar"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(_, _, _)).WillOnce(Return(error_permissions));

    const auto success = unit_.CreateDirectory("/foo/bar", permissions);

    ASSERT_FALSE(success.has_value());
    EXPECT_EQ(success.error(), ErrorCode::kCouldNotCreateDirectory);
}

TEST_F(FileUtilsTest, CreateDirectories_AbsolutePathTest)
{
    EXPECT_CALL(*filesystemMock_, Status(_)).WillRepeatedly(Return(FileStatus{FileType::kNotFound}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1/dir2"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1/dir2/dir3"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"/dir1"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"/dir1/dir2"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"/dir1/dir2/dir3"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));

    ASSERT_TRUE(unit_.CreateDirectories("/dir1/dir2/dir3", permissions).has_value());
}

TEST_F(FileUtilsTest, CreateDirectories_AbsolutePathRetryTest)
{
    EXPECT_CALL(*filesystemMock_, Status(_)).WillRepeatedly(Return(FileStatus{FileType::kNotFound}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1"})).WillOnce(Return(no_error));
    EXPECT_CALL(*unistd_mock_, nanosleep(_, _)).Times(3).WillRepeatedly(Return(os_no_error));
    Sequence s1;
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1/dir2"}))
        .Times(3)
        .InSequence(s1)
        .WillRepeatedly(Return(error_create_directory));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1/dir2"})).InSequence(s1).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1/dir2/dir3"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"/dir1"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"/dir1/dir2"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"/dir1/dir2/dir3"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));

    ASSERT_TRUE(unit_.CreateDirectories("/dir1/dir2/dir3", permissions).has_value());
}

TEST_F(FileUtilsTest, CreateDirectories_AbsolutePathRetryFailTest)
{
    EXPECT_CALL(*filesystemMock_, Status(_)).WillRepeatedly(Return(FileStatus{FileType::kNotFound}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"/dir1"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));
    EXPECT_CALL(*unistd_mock_, nanosleep(_, _)).Times(3).WillRepeatedly(Return(os_no_error));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1/dir2"}))
        .Times(4)
        .WillRepeatedly(Return(error_create_directory));

    ASSERT_FALSE(unit_.CreateDirectories("/dir1/dir2/dir3", permissions).has_value());
}

TEST_F(FileUtilsTest, CreateDirectories_RelativePathTest)
{
    EXPECT_CALL(*filesystemMock_, IsDirectory(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*filesystemMock_, Status(_)).WillRepeatedly(Return(FileStatus{FileType::kNotFound}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"dir1"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"dir1/dir2"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"dir1/dir2/dir3"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"dir1"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"dir1/dir2"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"dir1/dir2/dir3"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));

    ASSERT_TRUE(unit_.CreateDirectories("dir1/dir2/dir3", permissions).has_value());
}

TEST_F(FileUtilsTest, CreateDirectories_PartialDirectoryExistsTest)
{
    EXPECT_CALL(*filesystemMock_, IsDirectory(Path{"/dir1"})).WillOnce(Return(true));
    EXPECT_CALL(*filesystemMock_, IsDirectory(Path{"/dir1/dir2"})).WillOnce(Return(false));
    EXPECT_CALL(*filesystemMock_, Status(Path{"/dir1/dir2"})).WillOnce(Return(FileStatus{FileType::kNotFound}));
    EXPECT_CALL(*filesystemMock_, Status(Path{"/dir1/dir2/dir3"})).WillOnce(Return(FileStatus{FileType::kNotFound}));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1/dir2"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, CreateDirectory(Path{"/dir1/dir2/dir3"})).WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"/dir1/dir2"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));
    EXPECT_CALL(*filesystemMock_, Permissions(Path{"/dir1/dir2/dir3"}, permissions, PermOptions::kReplace))
        .WillOnce(Return(no_error));

    ASSERT_TRUE(unit_.CreateDirectories("/dir1/dir2/dir3", permissions).has_value());
}

TEST_F(FileUtilsTest, CreateDirectories_PathEndsWithSlash)
{
    EXPECT_CALL(*filesystemMock_, IsDirectory(Path{"dir1"})).WillOnce(Return(true));

    ASSERT_TRUE(unit_.CreateDirectories("dir1/", permissions).has_value());
}

TEST_F(FileUtilsTest, CreateDirectories_FailsWithExistingPathWithWrongPermissions)
{
    testing::InSequence seq{};

    ON_CALL(*filesystemMock_, IsDirectory(Path{"/dir1"})).WillByDefault(Return(true));
    ON_CALL(*filesystemMock_, IsDirectory(Path{"/dir1/dir2"})).WillByDefault(Return(true));
    ON_CALL(*filesystemMock_, Status(Path{"/dir1/dir2/dir3"}))
        .WillByDefault(Return(FileStatus{FileType::kDirectory, Perms::kNone}));

    ASSERT_FALSE(unit_.CreateDirectories("/dir1/dir2/dir3", permissions).has_value());
}

TEST_F(FileUtilsTest, ValidateGroup_failedStat)
{
    const Path path{"/dir1/file1"};
    const std::string group_name{"group_name"};
    //  Given error returned when getting file stats
    EXPECT_CALL(*stat_mock_, stat(StrEq("/dir1/file1"), _, _)).WillOnce(Return(os_enoent));

    const auto result = unit_.ValidateGroup(path, group_name);

    //  Validation returns error
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(static_cast<ErrorCode>(*result.error()), ErrorCode::kCouldNotValidateGroup);
}

TEST_F(FileUtilsTest, ValidateGroup_failedGetgrnam)
{
    const Path path{"/dir1/file1"};
    const std::string group_name{"group_name"};
    //  Given error returned for getting group
    EXPECT_CALL(*grp_mock_, getgrnam(StrEq("group_name"))).WillOnce(Return(os_getgrnam_enoent));

    const auto result = unit_.ValidateGroup(path, group_name);

    //  Validation returns error
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(static_cast<ErrorCode>(*result.error()), ErrorCode::kCouldNotValidateGroup);
}

TEST_F(FileUtilsTest, ValidateGroup_equalGroups)
{
    const Path path{"/dir1/file1"};
    const std::string group_name{"group_name"};
    const auto gid = 78293;
    //  Given specific gid returned for a file
    auto getFileGid = [this](Unused, os::StatBuffer& buf, Unused) -> score::cpp::expected_blank<score::os::Error> {
        buf.st_gid = gid;
        return os_no_error;
    };
    EXPECT_CALL(*stat_mock_, stat(StrEq("/dir1/file1"), _, _)).WillOnce(getFileGid);
    //  and the same gid returned for a group
    auto getGroupGid = [](Unused) -> score::cpp::expected<os::GroupBuffer, score::os::Error> {
        os::GroupBuffer buf{};
        buf.gid = gid;
        return buf;
    };
    EXPECT_CALL(*grp_mock_, getgrnam(StrEq("group_name"))).WillOnce(getGroupGid);

    const auto result = unit_.ValidateGroup(path, group_name);

    //  Validation returns positive result
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value());
}

TEST_F(FileUtilsTest, ValidateGroup_diffGroups)
{
    const Path path{"/dir1/file1"};
    const std::string group_name{"group_name"};
    const auto gid1 = 78293;
    const auto gid2 = 39287;
    //  Given specific gid returned for a file
    auto getFileGid = [this](Unused, os::StatBuffer& buf, Unused) -> score::cpp::expected_blank<score::os::Error> {
        buf.st_gid = gid1;
        return os_no_error;
    };
    EXPECT_CALL(*stat_mock_, stat(StrEq("/dir1/file1"), _, _)).WillOnce(getFileGid);
    //  and the another gid returned for a group
    auto getGroupGid = [](Unused) -> score::cpp::expected<os::GroupBuffer, score::os::Error> {
        os::GroupBuffer buf{};
        buf.gid = gid2;
        return buf;
    };
    EXPECT_CALL(*grp_mock_, getgrnam(StrEq("group_name"))).WillOnce(getGroupGid);

    const auto result = unit_.ValidateGroup(path, group_name);

    //  Validation returns negative result
    ASSERT_TRUE(result.has_value());
    ASSERT_FALSE(result.value());
}

TEST_F(FileUtilsTest, ChangeGroup_checkCalls)
{
    const Path path{"/dir1/file1"};
    const std::string group_name{"group_name"};
    const auto uid = 135;
    const auto gid = 7226;
    //  Given a 'gid' value
    auto getGroupGid = [](Unused) -> score::cpp::expected<os::GroupBuffer, score::os::Error> {
        os::GroupBuffer buf{};
        buf.gid = gid;
        return buf;
    };
    EXPECT_CALL(*grp_mock_, getgrnam(StrEq("group_name"))).WillOnce(Invoke(getGroupGid));
    //  Given a 'uid' value read
    EXPECT_CALL(*unistd_mock_, getuid()).WillOnce(Return(uid));
    //  Expect call with read 'uid' and given 'gid'
    EXPECT_CALL(*unistd_mock_, chown(StrEq("/dir1/file1"), Eq(uid), Eq(gid))).WillOnce(Return(os_no_error));

    const auto result = unit_.ChangeGroup(path, group_name);

    ASSERT_TRUE(result.has_value());
}

TEST_F(FileUtilsTest, ChangeGroup_checkFailGetGroupName)
{
    const Path path{"/dir1/file1"};
    const std::string group_name{"group_name"};
    //  Given a 'gid' value retrival failiure:
    EXPECT_CALL(*grp_mock_, getgrnam(StrEq("group_name"))).WillOnce(Return(os_getgrnam_enoent));

    const auto result = unit_.ChangeGroup(path, group_name);

    //  Expect failure status:
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(static_cast<ErrorCode>(*result.error()), ErrorCode::kCouldNotChangeGroup);
}

TEST_F(FileUtilsTest, ChangeGroup_checkFailChmod)
{
    const Path path{"/dir1/file1"};
    const std::string group_name{"group_name"};
    //  Given positive status of getgrnam
    EXPECT_CALL(*grp_mock_, getgrnam(StrEq("group_name"))).WillOnce(Return(os::GroupBuffer{}));
    //  When chmod fails
    EXPECT_CALL(*unistd_mock_, chown(StrEq("/dir1/file1"), _, _)).WillOnce(Return(os_enoent));

    const auto result = unit_.ChangeGroup(path, group_name);

    //  Expect failure status:
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(static_cast<ErrorCode>(*result.error()), ErrorCode::kCouldNotChangeGroup);
}

TEST_F(FileUtilsTest, ChangeGroup_checkOkChmodByGID)
{
    const Path path{"/dir1/file1"};
    const auto uid = 123;
    const auto gid = 4321;

    //  Given a 'uid' value read
    EXPECT_CALL(*unistd_mock_, getuid()).WillOnce(Return(uid));
    //  Expect call with read 'uid' and given 'gid'
    EXPECT_CALL(*unistd_mock_, chown(StrEq("/dir1/file1"), Eq(uid), Eq(gid))).WillOnce(Return(os_no_error));

    const auto result = unit_.ChangeGroup(path, gid);

    ASSERT_TRUE(result.has_value());
}

TEST_F(FileUtilsTest, ChangeGroup_checkNokChmodByGID)
{
    const Path path{"/dir1/file1"};
    const auto uid = 123;
    const auto gid = 4321;

    //  Given a 'uid' value read
    EXPECT_CALL(*unistd_mock_, getuid()).WillOnce(Return(uid));
    //  When chmod fails
    EXPECT_CALL(*unistd_mock_, chown(StrEq("/dir1/file1"), _, _)).WillOnce(Return(os_enoent));

    const auto result = unit_.ChangeGroup(path, gid);

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(static_cast<ErrorCode>(*result.error()), ErrorCode::kCouldNotChangeGroup);
}

TEST_F(FileUtilsTest, SyncDirectory_ok)
{
    const std::int32_t directoryDescriptor = 42;
    EXPECT_CALL(*fcntl_mock_, open(StrEq("/dir1/dir2"), score::os::Fcntl::Open::kReadOnly))
        .WillOnce(Return(directoryDescriptor));
    EXPECT_CALL(*unistd_mock_, fsync(directoryDescriptor)).WillOnce(Return(os_no_error));
    EXPECT_CALL(*unistd_mock_, close(directoryDescriptor)).Times(1);

    const auto result = unit_.SyncDirectory("/dir1/dir2");

    ASSERT_TRUE(result.has_value());
}

TEST_F(FileUtilsTest, SyncDirectory_failed_open)
{
    EXPECT_CALL(*fcntl_mock_, open(StrEq("/dir1/dir2"), score::os::Fcntl::Open::kReadOnly))
        .WillOnce(Return(score::cpp::make_unexpected(os::Error::createFromErrno(EEXIST))));
    EXPECT_CALL(*unistd_mock_, fsync).Times(0);
    EXPECT_CALL(*unistd_mock_, close).Times(0);

    const auto result = unit_.SyncDirectory("/dir1/dir2");

    ASSERT_FALSE(result.has_value());
}

TEST_F(FileUtilsTest, SyncDirectory_failed_fsync)
{
    const std::int32_t directoryDescriptor = 42;
    EXPECT_CALL(*fcntl_mock_, open(StrEq("/dir1/dir2"), score::os::Fcntl::Open::kReadOnly))
        .WillOnce(Return(directoryDescriptor));
    EXPECT_CALL(*unistd_mock_, fsync(directoryDescriptor)).WillOnce(Return(os_eio));
    EXPECT_CALL(*unistd_mock_, close(directoryDescriptor)).Times(1);

    const auto result = unit_.SyncDirectory("/dir1/dir2");

    ASSERT_FALSE(result.has_value());
}

class FileUtilsTest_OpenUniqueFile : public ::testing::Test
{
  public:
    void SetUp() noexcept override
    {
        ON_CALL(*stdlib_mock_, mkstemp(StrEq(template_path_.Native().data())))
            .WillByDefault(Invoke([this](char* target) {
                memcpy(target, expected_path_.Native().c_str(), expected_path_.Native().size());
                return valid_file_descriptor_;
            }));
    }

    std::string base_path_{"/abc/def-"};
    Path template_path_{base_path_ + "XXXXXX"};
    Path expected_path_{base_path_ + "ghijkl"};
    static constexpr int valid_file_descriptor_{42};

    const score::cpp::expected_blank<score::os::Error> os_no_error_{};
    const score::cpp::expected_blank<score::os::Error> os_eio_ = score::cpp::make_unexpected(score::os::Error::createFromErrno(EIO));

    std::stringbuf buffer_{};
    std::unique_ptr<std::iostream> iostream_{std::make_unique<std::iostream>(&buffer_)};

    score::os::MockGuard<score::filesystem::StandardFilesystemMock> filesystem_mock_;
    score::os::MockGuard<score::filesystem::FileFactoryMock> filefactory_mock_;
    score::os::MockGuard<score::os::StdlibMock> stdlib_mock_;
    score::os::MockGuard<score::os::UnistdMock> unistd_mock_;

    score::filesystem::FileUtils unit_{*filesystem_mock_, *filefactory_mock_};
};

TEST_F(FileUtilsTest_OpenUniqueFile, OpenUniqueFile_ok)
{
    EXPECT_CALL(*stdlib_mock_, mkstemp(StrEq(template_path_.Native().c_str())));
    EXPECT_CALL(*unistd_mock_, close(valid_file_descriptor_)).WillOnce(Return(os_no_error_));
    EXPECT_CALL(*filefactory_mock_, Open(expected_path_, std::ios_base::out))
        .WillOnce(Return(ByMove(std::move(iostream_))));

    const auto result = unit_.OpenUniqueFile(template_path_, std::ios_base::out);

    ASSERT_TRUE(result.has_value());

    constexpr auto expected_content{'a'};
    result->first->put(expected_content);
    EXPECT_EQ(buffer_.str(), std::string{expected_content});
    EXPECT_EQ(result->second, expected_path_);
}

TEST_F(FileUtilsTest_OpenUniqueFile, OpenUniqueFile_fails_with_wrong_path)
{
    EXPECT_CALL(*stdlib_mock_, mkstemp(_)).Times(0);
    EXPECT_CALL(*unistd_mock_, close(_)).Times(0);
    EXPECT_CALL(*filefactory_mock_, Open(_, _)).Times(0);

    Path template_path{base_path_ + "XXXXX"};
    const auto result = unit_.OpenUniqueFile(template_path, std::ios_base::out);

    ASSERT_FALSE(result.has_value());
}

TEST_F(FileUtilsTest_OpenUniqueFile, OpenUniqueFile_mkstemp_fails)
{
    EXPECT_CALL(*stdlib_mock_, mkstemp(StrEq(template_path_.Native().data())))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL))));
    EXPECT_CALL(*unistd_mock_, close(_)).Times(0);
    EXPECT_CALL(*filefactory_mock_, Open(_, _)).Times(0);

    const auto result = unit_.OpenUniqueFile(template_path_, std::ios_base::out);

    ASSERT_FALSE(result.has_value());
}

TEST_F(FileUtilsTest_OpenUniqueFile, OpenUniqueFile_close_fails)
{
    EXPECT_CALL(*stdlib_mock_, mkstemp(StrEq(template_path_.Native().data())));
    EXPECT_CALL(*unistd_mock_, close(valid_file_descriptor_)).WillOnce(Return(os_eio_));
    EXPECT_CALL(*filefactory_mock_, Open(_, _)).Times(0);

    const auto result = unit_.OpenUniqueFile(template_path_, std::ios_base::out);

    ASSERT_FALSE(result.has_value());
}

TEST_F(FileUtilsTest_OpenUniqueFile, OpenUniqueFile_open_fails)
{
    EXPECT_CALL(*stdlib_mock_, mkstemp(StrEq(template_path_.Native().data())));
    EXPECT_CALL(*unistd_mock_, close(valid_file_descriptor_)).WillOnce(Return(os_no_error_));
    EXPECT_CALL(*filefactory_mock_, Open(expected_path_, std::ios_base::out))
        .WillOnce(Return(ByMove(MakeUnexpected(ErrorCode::kCouldNotOpenFileStream))));

    const auto result = unit_.OpenUniqueFile(template_path_, std::ios_base::out);

    ASSERT_FALSE(result.has_value());
}

class FileUtilsTest_FileContentsAreIdentical : public ::testing::Test
{
  public:
    // constants
    const Path path1{"file1"};
    const Path path2{"file2"};
    const Result<bool> error = MakeUnexpected(ErrorCode::kCouldNotRetrieveStatus);

    // mocks
    score::os::MockGuard<score::filesystem::StandardFilesystemMock> filesystem_mock_;
    score::os::MockGuard<score::filesystem::FileFactoryMock> filefactory_mock_;

    // object for test
    score::filesystem::FileUtils unit_{*filesystem_mock_, *filefactory_mock_};

    // helper functions

    void ExpectFileOpen(const Path& path, const std::string content)
    {
        auto ss = [content](Unused, Unused) -> std::unique_ptr<std::iostream> {
            auto file = std::make_unique<std::stringstream>();
            *file << content;
            return file;
        };
        EXPECT_CALL(*filefactory_mock_, Open(Eq(path), _)).WillOnce(Invoke(ss));
    }

    void ExpectBadFileOpen(const Path& path)
    {
        auto bad_ss = [](Unused, Unused) -> std::unique_ptr<std::iostream> {
            auto bad_ss_uptr = std::make_unique<std::stringstream>();
            bad_ss_uptr->setstate(std::ios_base::badbit);
            return bad_ss_uptr;
        };
        EXPECT_CALL(*filefactory_mock_, Open(Eq(path), _)).WillOnce(Invoke(bad_ss));
    }

    void ExpectFileOpenNullPtr(const Path& path)
    {
        std::unique_ptr<std::iostream> null_ptr{};
        EXPECT_CALL(*filefactory_mock_, Open(Eq(path), _)).WillOnce(Return(ByMove(std::move(null_ptr))));
    }

    void ExpectFileOpenError(const Path& path)
    {
        score::Result<std::unique_ptr<std::iostream>> file_error =
            MakeUnexpected(filesystem::ErrorCode::kCouldNotOpenFileStream);
        EXPECT_CALL(*filefactory_mock_, Open(Eq(path), _)).WillOnce(Return(ByMove(std::move(file_error))));
    }
};

TEST_F(FileUtilsTest_FileContentsAreIdentical, FilesAreEqual)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectFileOpen(path1, "data");
    ExpectFileOpen(path2, "data");

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, FilesAreEmpty)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectFileOpen(path1, "");
    ExpectFileOpen(path2, "");

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value());
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, FirstFileIsLarger)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectFileOpen(path1, "ab");
    ExpectFileOpen(path2, "a");

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, SecondFileIsLarger)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectFileOpen(path1, "a");
    ExpectFileOpen(path2, "ab");

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, ContentAreNotEqual)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectFileOpen(path1, "abc");
    ExpectFileOpen(path2, "abX");

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value());
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, FailedIsRegilarFileForPath1)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(error));

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotCompareFiles);
    EXPECT_EQ(result.error().UserMessage(), "Failed to check file1 existence");
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, FailedIsRegilarFileForPath2)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(error));

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotCompareFiles);
    EXPECT_EQ(result.error().UserMessage(), "Failed to check file2 existence");
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, Path1DoesntExist)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(false));

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotCompareFiles);
    EXPECT_EQ(result.error().UserMessage(), "File1 does not exist or is not a regular file");
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, Path2DoesntExist)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(false));

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotCompareFiles);
    EXPECT_EQ(result.error().UserMessage(), "File2 does not exist or is not a regular file");
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, File1_Bad)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectBadFileOpen(path1);

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotCompareFiles);
    EXPECT_EQ(result.error().UserMessage(), "Failed to open file1");
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, File1_NullPtr)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectFileOpenNullPtr(path1);

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotCompareFiles);
    EXPECT_EQ(result.error().UserMessage(), "Failed to open file1");
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, File1_Error)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectFileOpenError(path1);

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotCompareFiles);
    EXPECT_EQ(result.error().UserMessage(), "Failed to open file1");
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, File2_Bad)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectFileOpen(path1, "data");
    ExpectBadFileOpen(path2);

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotCompareFiles);
    EXPECT_EQ(result.error().UserMessage(), "Failed to open file2");
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, File2_NullPtr)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectFileOpen(path1, "data");
    ExpectFileOpenNullPtr(path2);

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotCompareFiles);
    EXPECT_EQ(result.error().UserMessage(), "Failed to open file2");
}

TEST_F(FileUtilsTest_FileContentsAreIdentical, File2_Error)
{
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path1))).WillOnce(Return(true));
    EXPECT_CALL(*filesystem_mock_, IsRegularFile(Eq(path2))).WillOnce(Return(true));
    ExpectFileOpen(path1, "data");
    ExpectFileOpenError(path2);

    const auto result = unit_.FileContentsAreIdentical(path1, path2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotCompareFiles);
    EXPECT_EQ(result.error().UserMessage(), "Failed to open file2");
}

}  // namespace
}  // namespace filesystem
}  // namespace score
