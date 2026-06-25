/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

#include "score/flatbuffers/load_buffer.hpp"
#include "score/flatbuffers/details/load_buffer_internal.hpp"

#include "score/os/mocklib/fcntl_mock.h"
#include "score/os/mocklib/stat_mock.h"
#include "score/os/mocklib/unistdmock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cerrno>
#include <cstring>
#include <iterator>
#include <memory_resource>
#include <vector>

namespace score
{

namespace flatbuffers
{

namespace unit_test
{

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

constexpr std::int32_t kTestFd = 42;
const score::filesystem::Path kTestPath{"/tmp/test.bin"};
constexpr std::int64_t kTestFileSize = 10;
constexpr std::int64_t kInvalidNegativeSize = -100;

class ThrowingBadAllocResource : public std::pmr::memory_resource
{
    void* do_allocate(std::size_t /*bytes*/, std::size_t /*align*/) override
    {
        throw std::bad_alloc{};
    }
    void do_deallocate(void* /*p*/, std::size_t /*bytes*/, std::size_t /*align*/) override {}
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        return this == &other;
    }
};

struct CustomResizeException : public std::exception
{
};

class ThrowingCustomExceptionResource : public std::pmr::memory_resource
{
    void* do_allocate(std::size_t /*bytes*/, std::size_t /*align*/) override
    {
        throw CustomResizeException{};
    }
    void do_deallocate(void* /*p*/, std::size_t /*bytes*/, std::size_t /*align*/) override {}
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        return this == &other;
    }
};

class LoadFlatbufferTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        RecordProperty("Description",
                       "covers the defensive error handling branches for code coverage as well as all others");
        RecordProperty("TestType", "structural-branch-coverage");
        RecordProperty("TestType", "fault-injection");
    }

    void SetUpSuccessfulOpen()
    {
        ON_CALL(os_.fcntl, open(_, _))
            .WillByDefault(Return(score::cpp::expected<std::int32_t, score::os::Error>{kTestFd}));
    }

    void SetUpSuccessfulFstat(std::int64_t file_size)
    {
        ON_CALL(os_.stat, fstat(kTestFd, _))
            .WillByDefault(DoAll(Invoke([file_size](std::int32_t, score::os::StatBuffer& buf) {
                                     buf.st_size = file_size;
                                 }),
                                 Return(score::cpp::expected_blank<score::os::Error>{})));
    }

    void SetUpSuccessfulClose()
    {
        ON_CALL(os_.unistd, close(kTestFd)).WillByDefault(Return(score::cpp::expected_blank<score::os::Error>{}));
    }

    void SetUpSuccessfulReadAll(const std::vector<uint8_t>& content)
    {
        ON_CALL(os_.unistd, read(kTestFd, _, _))
            .WillByDefault(Invoke([content](std::int32_t,
                                            void* buf,
                                            std::size_t count) -> score::cpp::expected<ssize_t, score::os::Error> {
                const auto to_copy = std::min(count, content.size());
                std::memcpy(buf, content.data(), to_copy);
                return static_cast<ssize_t>(to_copy);
            }));
    }

    template <class Container>
    score::cpp::expected_blank<score::os::Error> call_impl(const score::filesystem::Path& path, Container& data)
    {
        return detail::LoadBufferImpl(os_, path, data);
    }

    struct OSMock
    {
        ::testing::NiceMock<score::os::FcntlMock> fcntl{};
        ::testing::NiceMock<score::os::StatMock> stat{};
        ::testing::NiceMock<score::os::UnistdMock> unistd{};
    };

    OSMock os_{};
};

TEST_F(LoadFlatbufferTest, OpenFailureReturnsError)
{
    const auto open_error = score::os::Error::createFromErrno(ENOENT);
    EXPECT_CALL(os_.fcntl, open(_, _)).WillOnce(Return(score::cpp::make_unexpected(open_error)));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

TEST_F(LoadFlatbufferTest, FstatFailureReturnsErrorAndClosesFile)
{
    SetUpSuccessfulOpen();

    const auto stat_error = score::os::Error::createFromErrno(EIO);
    EXPECT_CALL(os_.stat, fstat(kTestFd, _)).WillOnce(Return(score::cpp::make_unexpected(stat_error)));
    EXPECT_CALL(os_.unistd, close(kTestFd)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInputOutput);
}

TEST_F(LoadFlatbufferTest, NegativeFileSizeReturnsErrorAndClosesFile)
{
    SetUpSuccessfulOpen();
    SetUpSuccessfulClose();

    EXPECT_CALL(os_.stat, fstat(kTestFd, _))
        .WillOnce(DoAll(Invoke([](std::int32_t, score::os::StatBuffer& buf) {
                            buf.st_size = kInvalidNegativeSize;
                        }),
                        Return(score::cpp::expected_blank<score::os::Error>{})));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(LoadFlatbufferTest, ReadFailureReturnsErrorAndClosesFile)
{
    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(kTestFileSize);
    SetUpSuccessfulClose();

    const auto read_error = score::os::Error::createFromErrno(EIO);
    EXPECT_CALL(os_.unistd, read(kTestFd, _, _)).WillOnce(Return(score::cpp::make_unexpected(read_error)));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInputOutput);
}

TEST_F(LoadFlatbufferTest, ReadReturnsZeroBytesReportsUnexpectedEof)
{
    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(kTestFileSize);
    SetUpSuccessfulClose();

    EXPECT_CALL(os_.unistd, read(kTestFd, _, _))
        .WillOnce(Return(score::cpp::expected<ssize_t, score::os::Error>{static_cast<ssize_t>(0)}));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInputOutput);
}

TEST_F(LoadFlatbufferTest, PartialReadsAreHandledCorrectly)
{
    const std::vector<uint8_t> expected_data{'A', 'B', 'C', 'D', 'E'};

    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(static_cast<std::int64_t>(expected_data.size()));
    SetUpSuccessfulClose();

    std::size_t offset = 0U;
    EXPECT_CALL(os_.unistd, read(kTestFd, _, _))
        .WillOnce(Invoke([&](std::int32_t, void* buf, std::size_t) -> score::cpp::expected<ssize_t, score::os::Error> {
            std::memcpy(buf, std::next(expected_data.data(), static_cast<std::ptrdiff_t>(offset)), 2U);
            offset += 2U;
            return static_cast<ssize_t>(2);
        }))
        .WillOnce(Invoke([&](std::int32_t, void* buf, std::size_t) -> score::cpp::expected<ssize_t, score::os::Error> {
            std::memcpy(buf, std::next(expected_data.data(), static_cast<std::ptrdiff_t>(offset)), 2U);
            offset += 2U;
            return static_cast<ssize_t>(2);
        }))
        .WillOnce(Invoke([&](std::int32_t, void* buf, std::size_t) -> score::cpp::expected<ssize_t, score::os::Error> {
            std::memcpy(buf, std::next(expected_data.data(), static_cast<std::ptrdiff_t>(offset)), 1U);
            offset += 1U;
            return static_cast<ssize_t>(1);
        }));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(data, expected_data);
}

TEST_F(LoadFlatbufferTest, InterruptedReadIsRetried)
{
    const std::vector<uint8_t> expected_data{'X', 'Y'};
    const auto eintr_error = score::os::Error::createFromErrno(EINTR);

    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(static_cast<std::int64_t>(expected_data.size()));
    SetUpSuccessfulClose();

    EXPECT_CALL(os_.unistd, read(kTestFd, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(eintr_error)))
        .WillOnce(
            Invoke([&](std::int32_t, void* buf, std::size_t count) -> score::cpp::expected<ssize_t, score::os::Error> {
                const auto to_copy = std::min(count, expected_data.size());
                std::memcpy(buf, expected_data.data(), to_copy);
                return static_cast<ssize_t>(to_copy);
            }));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(data, expected_data);
}

TEST_F(LoadFlatbufferTest, EmptyFileReturnsEmptyVector)
{
    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(0);
    SetUpSuccessfulClose();

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(data.empty());
}

TEST_F(LoadFlatbufferTest, CloseFailureOnEmptyFilePropagatesCloseError)
{
    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(0);

    const auto close_error = score::os::Error::createFromErrno(EIO);
    EXPECT_CALL(os_.unistd, close(kTestFd)).WillOnce(Return(score::cpp::make_unexpected(close_error)));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInputOutput);
}

TEST_F(LoadFlatbufferTest, CloseFailureAfterSuccessfulReadPropagatesCloseError)
{
    const std::vector<uint8_t> expected_data{'Z'};

    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(static_cast<std::int64_t>(expected_data.size()));
    SetUpSuccessfulReadAll(expected_data);

    const auto close_error = score::os::Error::createFromErrno(EIO);
    EXPECT_CALL(os_.unistd, close(kTestFd)).WillOnce(Return(score::cpp::make_unexpected(close_error)));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInputOutput);
}

TEST_F(LoadFlatbufferTest, CloseFailureAfterReadErrorPropagatesOriginalError)
{
    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(kTestFileSize);

    const auto read_error = score::os::Error::createFromErrno(ENOENT);
    EXPECT_CALL(os_.unistd, read(kTestFd, _, _)).WillOnce(Return(score::cpp::make_unexpected(read_error)));

    const auto close_error = score::os::Error::createFromErrno(EIO);
    EXPECT_CALL(os_.unistd, close(kTestFd)).WillOnce(Return(score::cpp::make_unexpected(close_error)));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

TEST_F(LoadFlatbufferTest, ReadErrorAfterPartialReadReportsError)
{
    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(4);
    SetUpSuccessfulClose();

    const auto read_error = score::os::Error::createFromErrno(EIO);
    EXPECT_CALL(os_.unistd, read(kTestFd, _, _))
        .WillOnce(Invoke([](std::int32_t, void* buf, std::size_t) -> score::cpp::expected<ssize_t, score::os::Error> {
            const char byte = 'A';
            std::memcpy(buf, &byte, 1U);
            return static_cast<ssize_t>(1);
        }))
        .WillOnce(Return(score::cpp::make_unexpected(read_error)));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInputOutput);
}

TEST_F(LoadFlatbufferTest, UnexpectedEofAfterPartialReadReportsError)
{
    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(4);
    SetUpSuccessfulClose();

    EXPECT_CALL(os_.unistd, read(kTestFd, _, _))
        .WillOnce(Invoke([](std::int32_t, void* buf, std::size_t) -> score::cpp::expected<ssize_t, score::os::Error> {
            const char byte = 'B';
            std::memcpy(buf, &byte, 1U);
            return static_cast<ssize_t>(1);
        }))
        .WillOnce(Return(score::cpp::expected<ssize_t, score::os::Error>{static_cast<ssize_t>(0)}));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInputOutput);
}

TEST_F(LoadFlatbufferTest, MultipleInterruptsFollowedBySuccessfulRead)
{
    const std::vector<uint8_t> expected_data{'I'};
    const auto eintr_error = score::os::Error::createFromErrno(EINTR);

    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(static_cast<std::int64_t>(expected_data.size()));
    SetUpSuccessfulClose();

    EXPECT_CALL(os_.unistd, read(kTestFd, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(eintr_error)))
        .WillOnce(Return(score::cpp::make_unexpected(eintr_error)))
        .WillOnce(
            Invoke([&](std::int32_t, void* buf, std::size_t count) -> score::cpp::expected<ssize_t, score::os::Error> {
                const auto to_copy = std::min(count, expected_data.size());
                std::memcpy(buf, expected_data.data(), to_copy);
                return static_cast<ssize_t>(to_copy);
            }));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(data, expected_data);
}

TEST_F(LoadFlatbufferTest, CloseFailureAfterFstatErrorPropagatesFstatError)
{
    SetUpSuccessfulOpen();

    const auto stat_error = score::os::Error::createFromErrno(EBADF);
    EXPECT_CALL(os_.stat, fstat(kTestFd, _)).WillOnce(Return(score::cpp::make_unexpected(stat_error)));

    const auto close_error = score::os::Error::createFromErrno(EIO);
    EXPECT_CALL(os_.unistd, close(kTestFd)).WillOnce(Return(score::cpp::make_unexpected(close_error)));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kBadFileDescriptor);
}

TEST_F(LoadFlatbufferTest, ResizeBadAllocReturnsEnomemAndClosesFile)
{
    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(kTestFileSize);
    SetUpSuccessfulClose();

    ThrowingBadAllocResource bad_alloc_resource;
    std::pmr::vector<uint8_t> data{&bad_alloc_resource};
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kNotEnoughSpace);
}

TEST_F(LoadFlatbufferTest, ResizeCustomExceptionReturnsErrorAndClosesFile)
{
    SetUpSuccessfulOpen();
    SetUpSuccessfulFstat(kTestFileSize);
    SetUpSuccessfulClose();

    ThrowingCustomExceptionResource custom_resource;
    std::pmr::vector<uint8_t> data{&custom_resource};
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kUnexpected);
}

TEST_F(LoadFlatbufferTest, CloseFailureAfterNegativeSizePropagatesInvalidArgument)
{
    SetUpSuccessfulOpen();

    EXPECT_CALL(os_.stat, fstat(kTestFd, _))
        .WillOnce(DoAll(Invoke([](std::int32_t, score::os::StatBuffer& buf) {
                            buf.st_size = kInvalidNegativeSize;
                        }),
                        Return(score::cpp::expected_blank<score::os::Error>{})));

    const auto close_error = score::os::Error::createFromErrno(EIO);
    EXPECT_CALL(os_.unistd, close(kTestFd)).WillOnce(Return(score::cpp::make_unexpected(close_error)));

    std::vector<uint8_t> data;
    const auto result = call_impl(kTestPath, data);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

// ====================================
// Tests for the public LoadBuffer API
// ====================================

// These smoke tests use real OS resources (/dev/null, non-existent path).
const score::filesystem::Path kDevNull{"/dev/null"};
const score::filesystem::Path kNonExistent{"/tmp/score_flatbuffers_nonexistent_xyz987.bin"};

TEST(LoadBufferPublicApiTest, VectorOverloadSuccessPathReturnsEmptyVector)
{
    RecordProperty("TestType", "structural-branch-coverage");
    RecordProperty("DerivationTechnique", "boundary-values");

    const auto result = score::flatbuffers::LoadBuffer(kDevNull);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().empty());
}

TEST(LoadBufferPublicApiTest, VectorOverloadErrorPathReturnsError)
{
    RecordProperty("TestType", "structural-branch-coverage");
    RecordProperty("TestType", "fault-injection");

    const auto result = score::flatbuffers::LoadBuffer(kNonExistent);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

TEST(LoadBufferPublicApiTest, PmrVectorOverloadSuccessPathReturnsEmpty)
{
    RecordProperty("TestType", "structural-branch-coverage");
    RecordProperty("DerivationTechnique", "boundary-values");

    std::pmr::vector<uint8_t> data;
    const auto result = score::flatbuffers::LoadBuffer(kDevNull, data);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(data.empty());
}

TEST(LoadBufferPublicApiTest, PmrVectorOverloadErrorPathReturnsError)
{
    RecordProperty("TestType", "structural-branch-coverage");
    RecordProperty("TestType", "fault-injection");

    std::pmr::vector<uint8_t> data;
    const auto result = score::flatbuffers::LoadBuffer(kNonExistent, data);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

}  // namespace unit_test
}  // namespace flatbuffers
}  // namespace score
