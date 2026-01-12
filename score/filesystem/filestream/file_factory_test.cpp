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
#include "score/filesystem/filestream/file_factory.h"
#include "score/filesystem/factory/filesystem_factory.h"
#include "score/filesystem/file_utils/file_test_utils.h"
#include "score/os/mocklib/fcntl_mock.h"
#include "score/os/mocklib/stat_mock.h"
#include "score/os/mocklib/unistdmock.h"
#include "score/os/stat.h"

#include <fcntl.h>
#include <gtest/gtest.h>

#include <ftw.h>

#include <cstdio>
#include <iostream>

namespace score::filesystem
{
namespace
{

using namespace ::testing;

class FileFactoryTest : public ::testing::Test
{
  public:
    FileFactoryTest() : unit_{}, test_tmpdir_{}, remove_temp_{false}
    {
        InitializeTempDir();
    }

    ~FileFactoryTest() override
    {
        RemoveTempContent();
    }

  protected:
    void InitializeTempDir()
    {
        SCOPED_TRACE("Setup of temporary directory");
        auto temp_dir_result = FileTestUtils::GetTempDirectory();
        ASSERT_TRUE(temp_dir_result.has_value());
        test_tmpdir_ = temp_dir_result.value();
    }

    static int RemoveDirentry(const char* fname, const struct stat*, int, struct FTW*)
    {
        std::remove(fname);
        return 0;
    }

    void RemoveTempContent() const
    {
        if (remove_temp_)
        {
            ::nftw(test_tmpdir_.Native().c_str(), RemoveDirentry, 64, FTW_DEPTH);
        }
    }

    FileFactory unit_;
    Path test_tmpdir_;
    bool remove_temp_;
};

class FileFactoryTestWithFcntlMock : public FileFactoryTest
{
  protected:
    os::MockGuard<os::FcntlMock> fcntl_;
};

class FileFactoryTestWithStatMock : public FileFactoryTest
{
  protected:
    os::MockGuard<os::StatMock> stat_;
    os::MockGuard<os::UnistdMock> unistd_;
};

TEST_F(FileFactoryTest, OpensFile)
{
    auto file = unit_.Open("/tmp/some_file", std::ios_base::out);

    ASSERT_TRUE(file.has_value());

    *file.value() << "Foo";

    ::unlink("/tmp/some_file");
}

TEST_F(FileFactoryTest, OpensFileForAppending)
{
    auto file = unit_.Open("/tmp/some_file", std::ios_base::out | std::ios_base::app);

    ASSERT_TRUE(file.has_value());

    *file.value() << "Foo";

    ::unlink("/tmp/some_file");
}

TEST_F(FileFactoryTest, OpensNonExistingFile)
{
    auto file = unit_.Open("/tmp/some_file_not_existing", std::ios_base::in);

    ASSERT_FALSE(file.has_value());
    EXPECT_EQ(file.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTest, OpenForAtomicUpdate)
{
    static constexpr auto kTestFileName = "not_existing_yet";
    Path test_filename = test_tmpdir_ / kTestFileName;

    // Given a file with some random content
    auto file_result = unit_.Open(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(file_result.has_value());
    std::unique_ptr<std::iostream> file = *std::move(file_result);
    (*file) << "Test";
    file.reset();

    // When updating the file with the atomic update
    file_result = unit_.AtomicUpdate(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    (*file) << "Narf";
    file.reset();

    // Then the file contains the content that just had been written
    file_result = unit_.Open(test_filename, std::ios::in);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    std::string text;
    (*file) >> text;
    EXPECT_EQ(text, "Narf");
}

TEST_F(FileFactoryTest, AtomicUpdateInvalidMode)
{
    static constexpr auto kTestFileName = "not_existing_yet";
    Path test_filename = test_tmpdir_ / kTestFileName;
    auto file_result = unit_.AtomicUpdate(test_filename, std::ios::in);

    ASSERT_FALSE(file_result.has_value());
    EXPECT_EQ(file_result.error(), filesystem::ErrorCode::kNotImplemented);
}

TEST_F(FileFactoryTest, OldContentVisibleBeforeAtomicObjectGetsDeleted)
{
    static constexpr auto kTestFileName = "keep_content_before_drop";
    Path test_filename = test_tmpdir_ / kTestFileName;

    // Given a file with some random content
    auto file_result = unit_.Open(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(file_result.has_value());
    std::unique_ptr<std::iostream> file = *std::move(file_result);
    (*file) << "Test";
    file.reset();

    // When updating the file without dropping the object
    auto update_result = unit_.AtomicUpdate(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(update_result.has_value());
    auto update_file = *std::move(update_result);
    (*update_file) << "Narf";

    // Then the file should still contain the previous content

    // Now read from the "old" file by regularly opening it
    file_result = unit_.Open(test_filename, std::ios::in);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    std::string text;
    (*file) >> text;
    EXPECT_EQ(text, "Test");
    text.clear();
    file.reset();

    // When finally dropping the atomic update file
    update_file.reset();

    // Then the file content shall change
    file_result = unit_.Open(test_filename, std::ios::in);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    (*file) >> text;
    EXPECT_EQ(text, "Narf");
}

TEST_F(FileFactoryTest, ErrorOnFailingAtomicUpdate)
{
    auto fs = FilesystemFactory{}.CreateInstance();

    static constexpr auto kFaultyTargetFile{"file_in_disguise"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;
    ASSERT_TRUE(fs.standard->CreateDirectory(test_dir).has_value());

    auto stream = unit_.AtomicUpdate(test_dir, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(stream.has_value());
    **stream << "Hallo";
    auto close_result = (*stream)->Close();
    EXPECT_FALSE(close_result.has_value());

    EXPECT_TRUE(fs.standard->RemoveAll(test_dir).has_value());
}

TEST_F(FileFactoryTest, AtomicUpdateWithEmptyPath)
{
    auto fs = FilesystemFactory{}.CreateInstance();

    static constexpr auto kFaultyTargetFile{""};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;
    ASSERT_TRUE(fs.standard->CreateDirectory(test_dir).has_value());

    auto stream = unit_.AtomicUpdate(test_dir, std::ios::out | std::ios::trunc);
    ASSERT_FALSE(stream.has_value());
    EXPECT_EQ(stream.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithFcntlMock, OpenFileHandleFailedOnOpen)
{
    constexpr os::Stat::Mode kDefaultMode = os::Stat::Mode::kReadUser | os::Stat::Mode::kWriteUser |
                                            os::Stat::Mode::kReadGroup | os::Stat::Mode::kWriteGroup |
                                            os::Stat::Mode::kReadOthers | os::Stat::Mode::kWriteOthers;

    EXPECT_CALL(*fcntl_, open(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = details::OpenFileHandle(test_tmpdir_, std::ios_base::out, kDefaultMode);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithFcntlMock, OpenFileHandleFailedOnOpen_AppendMode)
{
    constexpr os::Stat::Mode kDefaultMode = os::Stat::Mode::kReadUser | os::Stat::Mode::kWriteUser |
                                            os::Stat::Mode::kReadGroup | os::Stat::Mode::kWriteGroup |
                                            os::Stat::Mode::kReadOthers | os::Stat::Mode::kWriteOthers;

    EXPECT_CALL(*fcntl_, open(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = details::OpenFileHandle(test_tmpdir_, std::ios_base::app, kDefaultMode);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithFcntlMock, AtomicUpdateFileHandleFailedOnOpen)
{
    EXPECT_CALL(*fcntl_, open(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = unit_.AtomicUpdate(test_tmpdir_, std::ios_base::out);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithStatMock, AtomicUpdateFileHandleFailedOnStat)
{
    EXPECT_CALL(*stat_, stat(_, _, _)).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = unit_.AtomicUpdate(test_tmpdir_, std::ios_base::out);
    ASSERT_TRUE(result.has_value());
}

TEST_F(FileFactoryTestWithStatMock, OpenForAtomicUpdateWithOwnershipChange)
{
    static constexpr auto kTestFileName = "not_existing_yet";
    Path test_filename = test_tmpdir_ / kTestFileName;

    auto set_mode = [](auto, auto& buffer, auto) {
        buffer.st_mode = mode_t{S_IFREG};
        buffer.st_uid = 1234;
        buffer.st_gid = 5678;
        return score::cpp::expected_blank<os::Error>{};
    };
    ON_CALL(*stat_, stat(_, _, _)).WillByDefault(Invoke(set_mode));

    // Expecting a call to chown
    EXPECT_CALL(*unistd_, chown(_, _, _)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));

    // When updating the file with the atomic update with ownership change
    const auto file_result = unit_.AtomicUpdate(test_filename, std::ios::out);
    ASSERT_TRUE(file_result.has_value());
}

TEST_F(FileFactoryTestWithStatMock, AtomicUpdateFileHandleFailedOnChown)
{
    auto set_mode = [](auto, auto& buffer, auto) {
        buffer.st_mode = mode_t{S_IFREG};
        buffer.st_uid = 1234;
        buffer.st_gid = 5678;
        return score::cpp::expected_blank<os::Error>{};
    };
    EXPECT_CALL(*stat_, stat(_, _, _)).WillOnce(Invoke(set_mode));
    EXPECT_CALL(*unistd_, chown(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = unit_.AtomicUpdate(test_tmpdir_, std::ios_base::out);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotSetPermissions);
}

TEST(FileFactoryHelpersTest, ComposeTempFilenameUsesAllParameters)
{
    const auto result = details::ComposeTempFilename("foo", 42, 32);
    EXPECT_EQ(result, ".foo-000042-00000032");
}

}  // namespace
}  // namespace score::filesystem
