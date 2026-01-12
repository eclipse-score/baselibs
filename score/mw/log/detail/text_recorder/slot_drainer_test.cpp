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
#include "score/mw/log/detail/text_recorder/slot_drainer.h"

#include "score/os/mocklib/unistdmock.h"
#include "score/mw/log/detail/error.h"
#include "score/mw/log/detail/text_recorder/mock/message_builder_mock.h"

#include "gtest/gtest.h"

#include <score/expected.hpp>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{

using ::testing::_;
using ::testing::Exactly;
using ::testing::Return;

using SpanData = score::cpp::span<const std::uint8_t>;
using OptionalSpan = score::cpp::optional<SpanData>;

constexpr std::uint32_t kLimitSlotsInOneCycle{32};

class SlotDrainerFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        unistd_ptr_ = score::cpp::pmr::make_unique<score::os::UnistdMock>(score::cpp::pmr::get_default_resource());
        unistd_mock_ = unistd_ptr_.get();

        allocator_ = std::make_unique<CircularAllocator<LogRecord>>(pool_size_);

        message_builder_mock_ = std::make_unique<mock::MessageBuilderMock>();

        raw_message_builder_mock_ = message_builder_mock_.get();
        message_builder = std::move(message_builder_mock_);
    }
    void TearDown() override {}

  private:
    const std::uint8_t pool_size_ = 8;  //  arbitrary size of circular allocator

  protected:
    const std::uint8_t data_table_[64] = {};  //  some memory used in test

    score::cpp::pmr::unique_ptr<::score::os::UnistdMock> unistd_ptr_{};
    ::score::os::UnistdMock* unistd_mock_{};
    std::unique_ptr<IMessageBuilder> message_builder = nullptr;
    std::shared_ptr<CircularAllocator<LogRecord>> allocator_ = nullptr;
    std::int32_t file_descriptor_ = 23;  //  random number file descriptor
    std::unique_ptr<mock::MessageBuilderMock> message_builder_mock_ = nullptr;
    mock::MessageBuilderMock* raw_message_builder_mock_ = nullptr;
};

TEST_F(SlotDrainerFixture, TestOneWriteFileFailurePath)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Writes will fail with IO error in case of failure path.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    //  Given write file error
    SlotDrainer unit(std::move(message_builder),
                 allocator_,
                 file_descriptor_,
                 std::move(unistd_ptr_),
                 false,
                 0,
                 kLimitSlotsInOneCycle);

    EXPECT_CALL(*raw_message_builder_mock_, GetNextSpan)
        .WillOnce(Return(OptionalSpan{}))  //  first unitialized
        .WillOnce(
            Return(score::cpp::span<const std::uint8_t>(data_table_, sizeof(data_table_))));  //  actual data to be written

    EXPECT_CALL(*raw_message_builder_mock_, SetNextMessage(_)).Times(Exactly(1));

    EXPECT_CALL(*unistd_mock_, write(file_descriptor_, _, _))
        .WillRepeatedly(Return(score::cpp::unexpected<score::os::Error>(score::os::Error::createFromErrno(EIO))));

    const auto slot = allocator_->AcquireSlotToWrite();
    EXPECT_TRUE(slot.has_value());

    //  Cast is not harmful because CircullarAllocator object size is within range of uint8_t
    unit.PushBack(SlotHandle{static_cast<SlotIndex>(slot.value())});
    unit.Flush();
}

TEST_F(SlotDrainerFixture, IncompleteWriteFileShouldMakeFlushSpansReturnWouldBlock)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "If write not completed, the Flush API would wait till flushing is complete.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    //  Given write file error
    SlotDrainer unit(std::move(message_builder),
                 allocator_,
                 file_descriptor_,
                 std::move(unistd_ptr_),
                 false,
                 0,
                 kLimitSlotsInOneCycle);

    EXPECT_CALL(*raw_message_builder_mock_, GetNextSpan)
        .WillOnce(Return(OptionalSpan{}))  //  first unitialized
        .WillOnce(
            Return(score::cpp::span<const std::uint8_t>(data_table_, sizeof(data_table_))));  //  actual data to be written

    EXPECT_CALL(*raw_message_builder_mock_, SetNextMessage(_)).Times(Exactly(1));

    EXPECT_CALL(*unistd_mock_, write(file_descriptor_, _, _))
        .WillOnce([](int, const void*, size_t) {
            return 1;
        })
        .WillOnce([](int, const void*, size_t) {
            return 1;
        });

    const auto slot = allocator_->AcquireSlotToWrite();
    EXPECT_TRUE(slot.has_value());

    //  Cast is not harmful because CircullarAllocator object size is within range of uint8_t
    unit.PushBack(SlotHandle{static_cast<SlotIndex>(slot.value())});
    unit.Flush();
}

TEST_F(SlotDrainerFixture, TestOneSlotOneSpan)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Writes shall succeed in case of proper arguments.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    //  Given one slot flushed
    SlotDrainer unit(std::move(message_builder),
                 allocator_,
                 file_descriptor_,
                 std::move(unistd_ptr_),
                 false,
                 0,
                 kLimitSlotsInOneCycle);

    //  Expect sequence of calls to check if any spans from possible previous slots are remaining:
    EXPECT_CALL(*raw_message_builder_mock_, GetNextSpan)
        .WillOnce(Return(OptionalSpan{}))  //  first unitialized
        .WillOnce(
            Return(score::cpp::span<const std::uint8_t>(data_table_, sizeof(data_table_))))  //  actual data to be written
        .WillOnce(Return(OptionalSpan{}))                                             //  spans depleted in a slot
        .WillOnce(Return(OptionalSpan{}));                                            //  spans depleted in a slot

    EXPECT_CALL(*raw_message_builder_mock_, SetNextMessage(_)).Times(Exactly(1));

    EXPECT_CALL(*unistd_mock_, write(file_descriptor_, data_table_, sizeof(data_table_)))
        .WillOnce([](int, const void*, size_t) {
            return sizeof(data_table_);
        });

    const auto slot = allocator_->AcquireSlotToWrite();
    EXPECT_TRUE(slot.has_value());

    //  Cast is not harmful because CircullarAllocator object size is within range of uint8_t
    unit.PushBack(SlotHandle{static_cast<SlotIndex>(slot.value())});
    unit.Flush();
}

TEST_F(SlotDrainerFixture, TestTooManySlotsForSingleCallShallNotBeAbleToFlushAllSlots)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Fails to flush all slots in case of exceeding the limit of slots to be processed per one call.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::size_t kLimiNumberOfSlotsProcesssedInOneCall = 2UL;
    const std::size_t kNumberOfSlotsQueued = 3UL;
    const std::size_t kNumberOfUnflushedSlots = 1UL;

    //  Given SlotDrainer set to limit number of slots processed in one call to:
    SlotDrainer unit(std::move(message_builder),
                 allocator_,
                 file_descriptor_,
                 std::move(unistd_ptr_),
                 false,
                 0,
                 kLimiNumberOfSlotsProcesssedInOneCall);

    //  Given 3 slots queued due to 'would block' returns:
    EXPECT_CALL(*raw_message_builder_mock_, GetNextSpan)
        .WillOnce(Return(OptionalSpan{}))  //  first unitialized
        .WillRepeatedly(Return(OptionalSpan{}));

    for (std::size_t i = 0; i < kNumberOfSlotsQueued; i++)
    {
        const auto slot = allocator_->AcquireSlotToWrite();
        EXPECT_TRUE(slot.has_value());
        unit.PushBack(SlotHandle{static_cast<SlotIndex>(slot.value())});
    }

    const auto slot = allocator_->AcquireSlotToWrite();
    unit.PushBack(SlotHandle{static_cast<SlotIndex>(slot.value())});
    unit.Flush();

    //  Expectation is that one slot is left unflushed:
    EXPECT_EQ(allocator_->GetUsedCount(), kNumberOfUnflushedSlots);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
