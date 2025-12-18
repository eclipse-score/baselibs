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
#include "score/mw/log/detail/verbose_payload.h"

#include "gtest/gtest.h"

#include <vector>

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

class VerbosePayloadFixture : public ::testing::Test
{
  public:
    VerbosePayload constructUnitWithSize(const std::size_t size)
    {
        return VerbosePayload{size, buffer_};
    }

    ByteVector buffer_{};
};

using VerbosePayloadFixtureDeathTest = VerbosePayloadFixture;

TEST_F(VerbosePayloadFixture, SinglePutStoresMemoryCorrect)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Put function stores data in correct order in the payload (buffer) if called once.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given an empty verbose payload with enough space
    VerbosePayload unit = constructUnitWithSize(10);

    // When adding some data into that buffer
    constexpr auto some_text = "Hello";
    unit.Put(some_text, 6);

    // Then the data is stored in the right order
    ASSERT_EQ(buffer_.at(0), 'H');
    ASSERT_EQ(buffer_.at(1), 'e');
    ASSERT_EQ(buffer_.at(2), 'l');
    ASSERT_EQ(buffer_.at(3), 'l');
    ASSERT_EQ(buffer_.at(4), 'o');
    ASSERT_EQ(buffer_.at(5), '\0');
}

TEST_F(VerbosePayloadFixture, MultiplePutStoresMemoryCorrectly)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Put function stores data in correct order in the payload (buffer) if called multiple times.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given an already filled buffer with enough space
    VerbosePayload unit = constructUnitWithSize(20);
    unit.Put("Hello", 6);

    // When adding some more data into that buffer
    constexpr auto some_text = "Next";
    unit.Put(some_text, 5);

    // Then the previous data is not corrupted and the new one is appended
    // Please be advised that on purpose we want raw memory handling, meaning
    // the duplicated null-termination is correct.
    ASSERT_EQ(buffer_.at(5), '\0');
    ASSERT_EQ(buffer_.at(6), 'N');
    ASSERT_EQ(buffer_.at(7), 'e');
    ASSERT_EQ(buffer_.at(8), 'x');
    ASSERT_EQ(buffer_.at(9), 't');
    ASSERT_EQ(buffer_.at(10), '\0');
}

TEST_F(VerbosePayloadFixture, PutZeroSize)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Put function handles zero size data requests gracefully");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given an already filled buffer with enough space
    VerbosePayload unit = constructUnitWithSize(20);
    unit.Put("Hello", 0);

    ASSERT_EQ(buffer_.size(), 0);
}

TEST_F(VerbosePayloadFixture, PutStopsAtMaximumSize)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Put function continues to fill data to maximum size and any additional data will not be filled.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given an empty verbose payload with to few space
    VerbosePayload unit = constructUnitWithSize(3);

    // When adding some data into that buffer
    constexpr auto some_text = "Hello";
    unit.Put(some_text, 6);

    // Then the data is stored in the right order, with maximum size of the buffer
    ASSERT_EQ(buffer_.at(0), 'H');
    ASSERT_EQ(buffer_.at(1), 'e');
    ASSERT_EQ(buffer_.at(2), 'l');
    ASSERT_EQ(buffer_.size(), 3);
}

TEST_F(VerbosePayloadFixtureDeathTest, AssertForInvalidPointerKicksIn)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Put function will exit with failure in case of invalid data pointer.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given an empty buffer
    VerbosePayload unit = constructUnitWithSize(0);

    // When trying to write data from a nullptr
    // Then an assertion protects from wrong behavior
    ASSERT_NO_FATAL_FAILURE(unit.Put(nullptr, 5));
}

TEST_F(VerbosePayloadFixture, EmptyBufferHasNoWrongBehavior)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Put function will not fill any data in case of empty buffer and will return successfully.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given an empty verbose payload with no space
    VerbosePayload unit = constructUnitWithSize(0);

    // When adding some data into that buffer
    constexpr auto some_text = "Hello";
    unit.Put(some_text, 6);

    // Then no data is stored
    ASSERT_EQ(buffer_.size(), 0);
}

TEST_F(VerbosePayloadFixture, SizeFitsInPayload)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies size fits in payload.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given an empty verbose payload with enough space
    VerbosePayload unit = constructUnitWithSize(5);

    // When checking if a fitting capacity is requested
    // Then WillOverflow returns false
    ASSERT_FALSE(unit.WillOverflow(5));
}

TEST_F(VerbosePayloadFixture, SizeFitsNotInPayload)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies size does not fit in payload.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given an empty verbose payload with enough space
    VerbosePayload unit = constructUnitWithSize(5);

    // When checking if a fitting capacity is requested
    // Then WillOverflow returns false
    ASSERT_TRUE(unit.WillOverflow(6));
}

TEST_F(VerbosePayloadFixture, SetBufferShallRebindReference)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies setting new buffer for VerbosePayload will update the pointer to the new buffer and discards "
        "the old one.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given VerbosePayload is constructed with a different buffer.
    constexpr std::size_t kOldBufferSize = 5U;
    VerbosePayload unit = constructUnitWithSize(kOldBufferSize);

    // When assigning another buffer...
    constexpr std::size_t kNewBufferSize = 7U;
    ByteVector new_buffer{};
    new_buffer.reserve(kNewBufferSize);
    unit.SetBuffer(new_buffer);

    // ...check that the new buffer is used:
    ASSERT_EQ(unit.RemainingCapacity(), kNewBufferSize);

    // ...and that the old buffer is unchanged:
    ASSERT_EQ(buffer_.capacity(), kOldBufferSize);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
