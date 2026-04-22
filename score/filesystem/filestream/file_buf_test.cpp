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
#include "score/filesystem/filestream/file_buf.h"
#include "score/filesystem/error.h"
#include "score/filesystem/filestream/file_stream.h"
#include "score/os/mocklib/stdiomock.h"
#include "score/os/mocklib/unistdmock.h"

#include <gtest/gtest.h>

using testing::_;
using testing::Return;
using testing::StrEq;

namespace score::filesystem::details
{
class FileBufTest : public ::testing::Test
{
  protected:
    StdioFileBuf filebuf{0, std::ios::in};
};

class AtomicFileBufTest : public ::testing::Test
{
  protected:
    AtomicFileBuf atomic_filebuf{0, std::ios::in, "from_path", "to_path"};
    os::MockGuard<os::UnistdMock> unistd_;
    os::MockGuard<os::StdioMock> stdio_;
};

TEST_F(FileBufTest, TestFailureOnClose)
{
    EXPECT_CALL(filebuf, is_open()).WillOnce(Return(true));
    EXPECT_CALL(filebuf, close()).WillOnce(Return(nullptr));

    auto result = filebuf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCloseFailed);
}

TEST_F(FileBufTest, TestClosingAlreadyClosedFile)
{
    EXPECT_CALL(filebuf, is_open()).WillOnce(Return(false));
    EXPECT_CALL(filebuf, close()).Times(0);

    auto result = filebuf.Close();
    EXPECT_TRUE(result.has_value());
}

TEST_F(AtomicFileBufTest, TestFailureOnSync)
{
    EXPECT_CALL(atomic_filebuf, is_open()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(atomic_filebuf, sync()).WillOnce(testing::Return(-1));
    EXPECT_CALL(*unistd_, unlink(StrEq("from_path"))).WillRepeatedly(Return(score::cpp::expected_blank<score::os::Error>{}));

    auto result = atomic_filebuf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kFsyncFailed);
}

TEST_F(AtomicFileBufTest, TestFailureOnFsync)
{
    EXPECT_CALL(atomic_filebuf, is_open()).WillRepeatedly(Return(true));
    EXPECT_CALL(atomic_filebuf, sync()).WillOnce(Return(0));
    EXPECT_CALL(*unistd_, fsync(_)).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    EXPECT_CALL(atomic_filebuf, close()).Times(0);
    EXPECT_CALL(*unistd_, unlink(StrEq("from_path"))).WillRepeatedly(Return(score::cpp::expected_blank<score::os::Error>{}));

    auto result = atomic_filebuf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kFsyncFailed);
}

TEST_F(AtomicFileBufTest, TestFailureOnClose)
{
    EXPECT_CALL(atomic_filebuf, is_open()).WillRepeatedly(Return(true));
    EXPECT_CALL(atomic_filebuf, sync()).WillOnce(Return(0));
    EXPECT_CALL(*unistd_, fsync(_)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    EXPECT_CALL(atomic_filebuf, close()).WillOnce(Return(nullptr));
    EXPECT_CALL(*unistd_, unlink(StrEq("from_path"))).WillRepeatedly(Return(score::cpp::expected_blank<score::os::Error>{}));

    auto result = atomic_filebuf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCloseFailed);
}
TEST_F(AtomicFileBufTest, TestFailureOnRename)
{
    EXPECT_CALL(atomic_filebuf, is_open()).WillRepeatedly(Return(true));
    EXPECT_CALL(atomic_filebuf, sync()).WillOnce(Return(0));
    EXPECT_CALL(*unistd_, fsync(_)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    EXPECT_CALL(atomic_filebuf, close()).WillOnce(Return(&atomic_filebuf));
    EXPECT_CALL(*stdio_, rename(_, _)).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    EXPECT_CALL(*unistd_, unlink(StrEq("from_path"))).WillRepeatedly(Return(score::cpp::expected_blank<score::os::Error>{}));

    auto result = atomic_filebuf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kCouldNotRenameFile);
}

TEST(FileStreamTest, TestNullBuffer)
{
    FileStream unit{nullptr};
    EXPECT_TRUE(unit.Close().has_value());
}

class AtomicFileBufTempFileCleanupTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        temp_file_ = "/tmp/atomic_filebuf_test_temp_file";
        {
            std::ofstream file(temp_file_);
            file << "temporary data";
        }
        ASSERT_EQ(::access(temp_file_.c_str(), F_OK), 0) << "Temp file should exist before test";
    }

    void TearDown() override
    {
        // Clean up in case the test fails and the file is still there
        ::unlink(temp_file_.c_str());
    }

    std::string temp_file_;
};

TEST_F(AtomicFileBufTempFileCleanupTest, CloseFailureRemovesTempFileFromDisk)
{
    AtomicFileBuf buf{0, std::ios::in, temp_file_, "to_path"};

    EXPECT_CALL(buf, is_open()).WillRepeatedly(Return(true));
    EXPECT_CALL(buf, sync()).WillOnce(Return(-1));

    auto result = buf.Close();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kFsyncFailed);
    EXPECT_NE(::access(temp_file_.c_str(), F_OK), 0) << "Temp file should be removed after close failure";
}

TEST_F(AtomicFileBufTempFileCleanupTest, DestructorRemovesTempFileWhenCloseNeverCalled)
{
    {
        AtomicFileBuf buf{0, std::ios::in, temp_file_, "to_path"};
        EXPECT_CALL(buf, is_open()).WillRepeatedly(Return(true));
        // buf goes out of scope without Close() being called
    }
    EXPECT_NE(::access(temp_file_.c_str(), F_OK), 0) << "Temp file should be removed by destructor";
}

TEST_F(AtomicFileBufTempFileCleanupTest, DestructorDoesNothingAfterSuccessfulClose)
{
    {
        AtomicFileBuf buf{0, std::ios::in, temp_file_, "to_path"};
        EXPECT_CALL(buf, is_open()).WillRepeatedly(Return(false));
        // is_open() returns false, simulating a successfully closed stream
    }
    EXPECT_EQ(::access(temp_file_.c_str(), F_OK), 0) << "Temp file should still exist after no-op destructor";
}

}  // namespace score::filesystem::details
