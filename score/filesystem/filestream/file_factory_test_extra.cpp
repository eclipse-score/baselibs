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
#include "score/filesystem/file_utils/file_test_utils.h"
#include "score/filesystem/filestream/file_buf.h"
#include "score/filesystem/filestream/file_factory.h"
#include "score/os/mocklib/stat_mock.h"
#include "score/os/mocklib/unistdmock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ftw.h>

#include <cstdio>
#include <iostream>

namespace score
{
namespace filesystem
{
namespace
{

using namespace ::testing;
using testing::Return;

class FileFactoryTestExtra : public ::testing::Test
{
  public:
    FileFactoryTestExtra() : unit_{}, test_tmpdir_{}, remove_temp_{false}
    {
        InitializeTempDir();
    }

    ~FileFactoryTestExtra() override
    {
        RemoveTempContent();
    }

  protected:
    void InitializeTempDir()
    {
        test_tmpdir_ = "/tmp";
        const auto temp_directory = FileTestUtils::GetTempDirectory();
        if (temp_directory.has_value())
        {
            test_tmpdir_ = temp_directory.value();
        }
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

TEST_F(FileFactoryTestExtra, CreateFileStreamUnopenedFile)
{
    class MockStdioFileBuf : public details::StdioFileBuf
    {
      public:
        MockStdioFileBuf(int fd, std::ios::openmode mode) : StdioFileBuf(fd, mode)
        {
            EXPECT_CALL(*this, is_open()).WillOnce(Return(false));
        }
    };

    auto result = details::CreateFileStream<MockStdioFileBuf>(0, std::ios::in);
    ASSERT_FALSE(result.has_value());
}

TEST_F(FileFactoryTestExtra, AtomicUpdateCleansUpWhenCreateFileStreamFails)
{
    // In the STDIO_FILEBUF_BASE_TESTING environment, is_open() is a mock that
    // returns 0 (false) by default, causing CreateFileStream to fail.

    os::MockGuard<os::StatMock> stat;
    os::MockGuard<os::UnistdMock> unistd;

    auto set_mode = [](auto, auto& buffer, auto) {
        buffer.st_mode = mode_t{S_IFREG | S_IWUSR};
        buffer.st_uid = ::getuid();
        buffer.st_gid = ::getgid();
        return score::cpp::expected_blank<os::Error>{};
    };
    EXPECT_CALL(*stat, stat(_, _, _)).WillOnce(Invoke(set_mode));

    // Expect cleanup: close the fd and remove the temp file
    EXPECT_CALL(*unistd, close(_)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
    EXPECT_CALL(*unistd, unlink(_)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));

    static constexpr auto kTestFileName{"test_file"};
    Path test_filename = test_tmpdir_ / kTestFileName;

    auto result = unit_.AtomicUpdate(test_filename, std::ios_base::out);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

}  // namespace
}  // namespace filesystem
}  // namespace score
