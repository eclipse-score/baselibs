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
#include "score/mw/log/detail/text_recorder/non_blocking_writer.h"

#include "score/os/mocklib/unistdmock.h"
#include "score/os/mocklib/fcntlmock.h"

#include "gtest/gtest.h"
#include <fcntl.h>
#include <array>

using ::testing::_;
using ::testing::Return;

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

constexpr std::size_t max_chunk_size = 2048;

struct NonBlockingWriterTestFixture : ::testing::Test
{
    void SetUp() override
    {
        auto unistd = score::cpp::pmr::make_unique<score::os::UnistdMock>(score::cpp::pmr::get_default_resource());
        unistd_ = unistd.get();
        auto fcntl = score::cpp::pmr::make_unique<score::os::FcntlMock>(score::cpp::pmr::get_default_resource());
        fcntl_ = fcntl.get();
        writer_ = std::make_unique<NonBlockingWriter>("", max_chunk_size, std::move(unistd), std::move(fcntl), false, false, 0, 1, false);
    }

    void TearDown() override
    {
        writer_.reset();
        unistd_ = nullptr;
        fcntl_ = nullptr;
    }

  protected:
    std::unique_ptr<NonBlockingWriter> writer_;
    score::os::UnistdMock* unistd_{};
    score::os::FcntlMock* fcntl_{};

    std::int32_t kFileDescriptor{};
};

TEST_F(NonBlockingWriterTestFixture, NonBlockingWriterWhenCircularLoggingExceedsMaxSize)
{
    RecordProperty("ParentRequirement", "SCR-861578");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "NonBlockingWrite wraps around when circular logging is enabled and max size is reached.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto unistd = score::cpp::pmr::make_unique<score::os::UnistdMock>(score::cpp::pmr::get_default_resource());
    unistd_ = unistd.get();
    auto fcntl = score::cpp::pmr::make_unique<score::os::FcntlMock>(score::cpp::pmr::get_default_resource());
    fcntl_ = fcntl.get();
    writer_ = std::make_unique<NonBlockingWriter>("", max_chunk_size, std::move(unistd), std::move(fcntl), true, true, max_chunk_size * 2, 1, false);

    std::array<uint8_t, 3 * max_chunk_size> payload{};
    const score::cpp::span<const std::uint8_t> buffer{payload.data(),
                                               static_cast<score::cpp::span<const std::uint8_t>::size_type>(payload.size())};

    writer_->SetSpan(buffer);

    EXPECT_CALL(*fcntl_, open(_,_,_)).WillOnce(Return(kFileDescriptor));
    EXPECT_CALL(*unistd_, lseek(kFileDescriptor, 0, SEEK_END)).WillOnce(Return(0));
    EXPECT_CALL(*unistd_, write(kFileDescriptor, buffer.data(), max_chunk_size)).WillOnce(Return(max_chunk_size));
    ASSERT_EQ(NonBlockingWriter::Result::kWouldBlock, writer_->FlushIntoFile().value());

    EXPECT_CALL(*unistd_, write(kFileDescriptor, &(buffer.data()[max_chunk_size]), max_chunk_size))
        .WillOnce(Return(max_chunk_size));
    ASSERT_EQ(NonBlockingWriter::Result::kWouldBlock, writer_->FlushIntoFile().value());

    EXPECT_CALL(*unistd_, lseek(kFileDescriptor, 0, SEEK_SET)).WillOnce(Return(0));
    EXPECT_CALL(*unistd_, write(kFileDescriptor, &(buffer.data()[2*max_chunk_size]), max_chunk_size))
        .WillOnce(Return(max_chunk_size));
    ASSERT_EQ(NonBlockingWriter::Result::kDone, writer_->FlushIntoFile().value());
}

TEST_F(NonBlockingWriterTestFixture, NonBlockingWriterWhenNonCircularLoggingExceedsMaxSize)
{
    RecordProperty("ParentRequirement", "SCR-86157TBD");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "NonBlockingWrite does not wrap around when circular logging is disabled.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto unistd = score::cpp::pmr::make_unique<score::os::UnistdMock>(score::cpp::pmr::get_default_resource());
    unistd_ = unistd.get();
    auto fcntl = score::cpp::pmr::make_unique<score::os::FcntlMock>(score::cpp::pmr::get_default_resource());
    fcntl_ = fcntl.get();
    writer_ = std::make_unique<NonBlockingWriter>("", max_chunk_size, std::move(unistd), std::move(fcntl), false, false, max_chunk_size * 2, 1, false);

    std::array<uint8_t, 3 * max_chunk_size> payload{};
    const score::cpp::span<const std::uint8_t> buffer{payload.data(),
                                               static_cast<score::cpp::span<const std::uint8_t>::size_type>(payload.size())};

    writer_->SetSpan(buffer);

    EXPECT_CALL(*fcntl_, open(_,_,_)).WillOnce(Return(kFileDescriptor));
    EXPECT_CALL(*unistd_, write(kFileDescriptor, buffer.data(), max_chunk_size)).WillOnce(Return(max_chunk_size));
    ASSERT_EQ(NonBlockingWriter::Result::kWouldBlock, writer_->FlushIntoFile().value());

    EXPECT_CALL(*unistd_, write(kFileDescriptor, &(buffer.data()[max_chunk_size]), max_chunk_size))
        .WillOnce(Return(max_chunk_size));
    ASSERT_EQ(NonBlockingWriter::Result::kWouldBlock, writer_->FlushIntoFile().value());

    EXPECT_CALL(*unistd_, lseek(_, _, _)).Times(0);
    EXPECT_CALL(*unistd_, write(kFileDescriptor, &(buffer.data()[2*max_chunk_size]), max_chunk_size))
        .WillOnce(Return(max_chunk_size));
    ASSERT_EQ(NonBlockingWriter::Result::kDone, writer_->FlushIntoFile().value());
}

TEST_F(NonBlockingWriterTestFixture, NonBlockingWriterWhenFlushingTwiceMaxChunkSizeShallReturnTrue)
{
    RecordProperty("ParentRequirement", "SCR-861578");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "NonBlockingWrite can flush 2 max chunks size.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::array<uint8_t, 2 * max_chunk_size> payload{};

    const score::cpp::span<const std::uint8_t> buffer{payload.data(),
                                               static_cast<score::cpp::span<const std::uint8_t>::size_type>(payload.size())};

    writer_->SetSpan(buffer);

    EXPECT_CALL(*unistd_, write(kFileDescriptor, buffer.data(), max_chunk_size)).WillOnce(Return(max_chunk_size));

    ASSERT_EQ(NonBlockingWriter::Result::kWouldBlock, writer_->FlushIntoFile().value());

    EXPECT_CALL(*unistd_, write(kFileDescriptor, &(buffer.data()[max_chunk_size]), max_chunk_size))
        .WillOnce(Return(max_chunk_size));

    ASSERT_EQ(NonBlockingWriter::Result::kDone, writer_->FlushIntoFile().value());
}

TEST_F(NonBlockingWriterTestFixture,
       NonBlockingWriterWhenFlushing2DifferentSpansWithDifferentSizesShallReturnOkInLastFlushForEachSpan)
{
    RecordProperty("ParentRequirement", "SCR-861578");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "NonBlockingWrite can flush 2 different spans with different sizes.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::array<uint8_t, (2 * max_chunk_size) + 3> first_payload{};

    score::cpp::span<const std::uint8_t> first_span{
        first_payload.data(), static_cast<score::cpp::span<const std::uint8_t>::size_type>(first_payload.size())};

    writer_->SetSpan(first_span);

    EXPECT_CALL(*unistd_, write(kFileDescriptor, first_span.data(), max_chunk_size)).WillOnce(Return(max_chunk_size));

    ASSERT_EQ(NonBlockingWriter::Result::kWouldBlock, writer_->FlushIntoFile().value());

    EXPECT_CALL(*unistd_, write(kFileDescriptor, &(first_span.data()[max_chunk_size]), max_chunk_size))
        .WillOnce(Return(max_chunk_size));

    ASSERT_EQ(NonBlockingWriter::Result::kWouldBlock, writer_->FlushIntoFile().value());

    EXPECT_CALL(*unistd_, write(kFileDescriptor, &(first_span.data()[2 * max_chunk_size]), 3)).WillOnce(Return(3));

    ASSERT_EQ(NonBlockingWriter::Result::kDone, writer_->FlushIntoFile().value());

    std::vector<uint8_t> second_payload(max_chunk_size, 0);

    score::cpp::span<const std::uint8_t> second_span{
        second_payload.data(), static_cast<score::cpp::span<const std::uint8_t>::size_type>(second_payload.size())};

    writer_->SetSpan(second_span);

    EXPECT_CALL(*unistd_, write(kFileDescriptor, second_span.data(), max_chunk_size)).WillOnce(Return(max_chunk_size));

    ASSERT_EQ(NonBlockingWriter::Result::kDone, writer_->FlushIntoFile().value());
}

TEST_F(NonBlockingWriterTestFixture, NonBlockingWriterShallReturnFalseWhenWriteSysCallFailsWithError_EBADF)
{
    RecordProperty("ParentRequirement", "SCR-861578");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "NonBlockingWrite cannot flush if the system call write fails with error EBADF.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::array<uint8_t, max_chunk_size> payload{};
    score::cpp::span<const std::uint8_t> buffer{payload.data(),
                                         static_cast<score::cpp::span<const std::uint8_t>::size_type>(payload.size())};

    writer_->SetSpan(buffer);

    auto error = score::cpp::make_unexpected(score::os::Error::createFromErrno(EBADF));

    EXPECT_CALL(*unistd_, write(kFileDescriptor, buffer.data(), max_chunk_size)).WillOnce(Return(error));

    ASSERT_EQ(writer_->FlushIntoFile().error(), score::mw::log::detail::Error::kUnknownError);
}

TEST_F(NonBlockingWriterTestFixture,
       NonBlockingWriterWhenFlushing2kMaxChunkSizeShallReturnTrueWhenFlushingSpanWithCorrectIndexesToWriteSystemCall)
{
    RecordProperty("ParentRequirement", "SCR-861578");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "NonBlockingWrite can flush 2 max chunk sizes when flushing span with correct indexes to system call write");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::array<uint8_t, 2 * max_chunk_size> payload{};
    score::cpp::span<const std::uint8_t> buffer{payload.data(),
                                         static_cast<score::cpp::span<const std::uint8_t>::size_type>(payload.size())};

    writer_->SetSpan(buffer);

    EXPECT_CALL(*unistd_, write(kFileDescriptor, buffer.data(), max_chunk_size)).WillOnce(Return(max_chunk_size));

    ASSERT_EQ(NonBlockingWriter::Result::kWouldBlock, writer_->FlushIntoFile().value());

    EXPECT_CALL(*unistd_, write(kFileDescriptor, &(buffer.data()[max_chunk_size]), max_chunk_size))
        .WillOnce(Return(max_chunk_size));

    ASSERT_EQ(NonBlockingWriter::Result::kDone, writer_->FlushIntoFile().value());
}

TEST_F(NonBlockingWriterTestFixture,
       NonBlockingWriterWhenFlushing1kMaxChunkSizeOnTwoTimesSinceWriteWillReturnHalfOfMaxChunkSizeShallReturnTrue)
{
    RecordProperty("ParentRequirement", "SCR-861578");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "NonBlockingWrite can flush 1 k max chunk size on two different times even if the write returns "
                   "half of max chunk size");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::array<uint8_t, max_chunk_size> payload{};

    const score::cpp::span<const std::uint8_t> buffer{payload.data(),
                                               static_cast<score::cpp::span<const std::uint8_t>::size_type>(payload.size())};

    writer_->SetSpan(buffer);

    EXPECT_CALL(*unistd_, write(kFileDescriptor, buffer.data(), max_chunk_size)).WillOnce(Return(max_chunk_size / 2));

    ASSERT_EQ(NonBlockingWriter::Result::kWouldBlock, writer_->FlushIntoFile().value());

    EXPECT_CALL(*unistd_, write(kFileDescriptor, &(buffer.data()[max_chunk_size / 2]), max_chunk_size / 2))
        .WillOnce(Return(max_chunk_size / 2));

    ASSERT_EQ(NonBlockingWriter::Result::kDone, writer_->FlushIntoFile().value());
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
