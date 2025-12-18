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
#include "score/mw/log/detail/circular_allocator.h"

#include "gtest/gtest.h"

#include "score/utility.hpp"

#include <array>
#include <cstdint>
#include <numeric>
#include <thread>

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

class CircularAllocatorFixture : public ::testing::Test
{
  public:
    void WriteInto(CircularAllocator<std::int32_t>& unit, std::int32_t value) const noexcept
    {
        const auto slot = unit.AcquireSlotToWrite();
        ASSERT_TRUE(slot.has_value());
        unit.GetUnderlyingBufferFor(slot.value()) = value;
        unit.ReleaseSlot(slot.value());
    }
};

using CircularAllocatorFixtureDeathTest = CircularAllocatorFixture;

TEST_F(CircularAllocatorFixture, WriteSingleEntryWithoutThreads)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Writing into the circular allocator shall succeed if there are multiple free slots.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a Ring-Buffer with enough space
    CircularAllocator<std::int32_t> unit{5};

    // When writing into it a single value
    WriteInto(unit, 42);

    // Then no exceptions, wrong allocations or other errors happen
}

TEST_F(CircularAllocatorFixture, WriteSingleEntryWithoutThreadsWithSingleSlotCapacity)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Writing into the circular allocator shall succeed if a single slot is free.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a Ring-Buffer with enough space
    CircularAllocator<std::int32_t> unit{1};

    // When writing into it a single value
    WriteInto(unit, 42);

    // Then no exceptions, wrong allocations or other errors happen
}

TEST_F(CircularAllocatorFixture, WriteSingleThreadedOverBufferSize)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "When writing more slots in the circular allocator than capacity, the write to the next slot shall "
                   "wrap around and succeed.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Overview of the expected storage layout:
    // | Slot 0 | Slot 1 | Slot 2 |
    // |    0   |    1   |   2    | <- Before overrun
    // |    3   |    1   |   2    | <- After overrun

    // Given a Ring-Buffer with to few space
    CircularAllocator<std::int32_t> unit{3};

    // When adding more into the buffer than its capacity
    for (std::uint8_t counter{}; counter < 4; counter++)
    {
        WriteInto(unit, counter);
    }

    // Then old unused values are overwritten
    ASSERT_EQ(unit.GetUnderlyingBufferFor(std::size_t{0}), 3);
    ASSERT_EQ(unit.GetUnderlyingBufferFor(std::size_t{1}), 1);
    ASSERT_EQ(unit.GetUnderlyingBufferFor(std::size_t{2}), 2);
}

TEST_F(CircularAllocatorFixture, WritingFromMultipleThreadsIsSafe)
{
    RecordProperty("Requirement", "SCR-861578, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "When writing to the CircularAllocator from multiple threads and each thread shall occupy one slot at a time "
        "every slot allocation from every thread shall succeed if the number of threads is equal the "
        "number of slots.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a Ring-Buffer
    constexpr size_t number_of_slots = 10U;
    CircularAllocator<std::int32_t> unit{number_of_slots};

    // When writing into it from multiple threads
    std::array<std::thread, number_of_slots - 1> threads{};
    for (std::uint8_t counter{}; counter < threads.size(); counter++)
    {
        threads.at(counter) = std::thread([&unit, counter, this]() noexcept {
            //  TODO: Re-enable multithreaded test for N threads allocating from N-element pool. See Ticket-110148
            //  i.e. the loop: 'for (std::int32_t number{}; number < 100; number++) { WriteInto(unit, number); }'
            WriteInto(unit, counter);
        });
    }

    // Then no memory corruption or race conditions happen (checked by TSAN, ASAN, valgrind)
    for (auto& thread : threads)
    {
        thread.join();
    }
}

TEST_F(CircularAllocatorFixture, WritingFromMultipleThreadsIsSafeWithInsufficientCapacity)
{
    RecordProperty("Requirement", "SCR-861578, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "When writing to CircularAllocator with multiple threads in parallel and trying to allocate more "
                   "slots than capacity, the number of reserved slots shall be equal to the capacity.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a Ring-Buffer
    constexpr std::size_t number_of_slots{100};
    CircularAllocator<std::int32_t> unit{number_of_slots};

    // When trying to write into it from multiple threads such that the number of slots is insufficient.
    std::array<std::thread, 10> threads{};
    std::array<std::size_t, 10> number_of_reserved_slots_per_thread{};
    for (std::size_t counter{}; counter < threads.size(); counter++)
    {
        threads.at(counter) = std::thread([&unit, counter, &number_of_reserved_slots_per_thread]() noexcept {
            for (std::size_t number{}; number < 50; number++)
            {
                if (unit.AcquireSlotToWrite().has_value())
                {
                    number_of_reserved_slots_per_thread[counter]++;
                }
            }
        });
    }

    // Then no memory corruption or race conditions happen (checked by TSAN, ASAN, valgrind)
    for (auto& thread : threads)
    {
        thread.join();
    }

    // And the number of reserved slots shall be equal to the capacity.
    const std::size_t number_of_reserved_slots =
        std::accumulate(number_of_reserved_slots_per_thread.begin(), number_of_reserved_slots_per_thread.end(), 0u);
    EXPECT_EQ(number_of_reserved_slots, number_of_slots);
}

TEST_F(CircularAllocatorFixture, TryAcquireWhenAllSlotsAcquired)
{
    RecordProperty("Requirement", "SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "When every slot is occupied acquiring another slot shall return an empty result.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a Ring-Buffer where all slots are acquired
    CircularAllocator<std::int32_t> unit{1};
    EXPECT_TRUE(unit.AcquireSlotToWrite().has_value());

    // When acquiring another one
    const auto slot = unit.AcquireSlotToWrite();

    // Then no slot can be acquired and empty optional is returned
    EXPECT_FALSE(slot.has_value());
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
