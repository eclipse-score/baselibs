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
#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator.h"
#include <numeric>
namespace score
{
namespace analysis
{
namespace tracing
{
namespace
{
// Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
// not lead to data loss.".
// Rationale: False positive method in nameless namespace.
// coverity[autosar_cpp14_a2_10_1_violation : FALSE]
inline std::size_t GetSizeAligned(const std::size_t non_aligned_address, const std::size_t alignment)
{
    if (alignment == 0u)
    {
        return non_aligned_address;
    }

    const std::size_t remainder = non_aligned_address % alignment;
    if (remainder == 0u)
    {
        return non_aligned_address;
    }
    // No possibility here for data loss
    // coverity[autosar_cpp14_a4_7_1_violation]
    return non_aligned_address + alignment - remainder;
}

}  // namespace

FlexibleCircularAllocator::FlexibleCircularAllocator(void* base_address, std::size_t size)
    : IFlexibleCircularAllocator(),
      base_address_(base_address),
      total_size_(size),
      latest_allocated_address_{base_address}
{
    free_blocks_.push_back({base_address_, size});
}
// clang-format off
void FlexibleCircularAllocator::SplitMemoryBlockIfNeeded(
    std::list<FlexibleCircularAllocator::MemoryBlock>::iterator next_address_to_use,
    std::size_t aligned_size)
{
    if (next_address_to_use->size > aligned_size)
    {

        next_address_to_use->address =
            // Needed in order to perform offset calculations (offset in the offset_ptr) for rule m5-2-8,m5-0-15
            // coverity[autosar_cpp14_m5_2_8_violation]
            // coverity[autosar_cpp14_m5_0_15_violation]
            static_cast<std::uint8_t*>(next_address_to_use->address) + aligned_size;// NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic): See above.
        next_address_to_use->size -= aligned_size;
    }
    else
    {
        // No harm from ignoring the returned iterator (a0-1-1)
        //  coverity[autosar_cpp14_a0_1_1_violation]
        next_address_to_use = free_blocks_.erase(next_address_to_use);
    }
}
// clang-format on
score::Result<std::list<FlexibleCircularAllocator::MemoryBlock>::iterator> FlexibleCircularAllocator::GetNextAddressToUse(
    std::size_t aligned_size)
{
    auto it = std::lower_bound(free_blocks_.begin(), free_blocks_.end(), latest_allocated_address_);
    std::list<MemoryBlock>::iterator next_address_to_use;
    // we went through all element in the list. and no block satisfy the needed size
    for (std::uint32_t count = 0u; count < free_blocks_.size(); count++)
    {
        // wrap around
        if (it == free_blocks_.end())
        {
            it = free_blocks_.begin();
        }

        if (it->size >= aligned_size)
        {
            next_address_to_use = it;
            return next_address_to_use;
        }
        it++;
    }
    return score::MakeUnexpected(FlexibleAllocatorErrorCode::kNotEnoughMemory);
}
// clang-format off
// This is intented, we don't enforce users to specify align unless needed
// NOLINTNEXTLINE(google-default-arguments) see comment above
score::Result<void*> FlexibleCircularAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
    std::lock_guard<std::mutex> guard(mutex_);
    // No harm from declaring void pointer
    //  coverity[autosar_cpp14_a0_1_1_violation]
    void* allocated_ptr = nullptr;
    const std::size_t aligned_size = GetSizeAligned(size, alignment);

    // if no free blocks avilable return nullptr
    if ((free_blocks_.empty()) || (aligned_size >= total_size_))
    {
        return score::MakeUnexpected(FlexibleAllocatorErrorCode::kNotEnoughMemory);
    }
    // Find the first free block with sufficient size.
    std::list<FlexibleCircularAllocator::MemoryBlock>::iterator next_address_to_use;
    // get next free element in ascending order
    auto result = GetNextAddressToUse(aligned_size);
    if (!result.has_value())
    {
        return MakeUnexpected<void*>(result.error());
    }
    next_address_to_use = result.value();
    allocated_ptr = next_address_to_use->address;
    // No harm when do arithmetic operations (m5_0_15_), No harm from using static_cast here(m5-2-8).
    // coverity[autosar_cpp14_m5_2_8_violation]
    // coverity[autosar_cpp14_m5_0_15_violation]
    latest_allocated_address_ = static_cast<std::uint8_t*>(allocated_ptr) + aligned_size;// NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic): See above
    // If the free block has more space than needed, split it.
    SplitMemoryBlockIfNeeded(next_address_to_use, aligned_size);
    std::ignore = allocated_blocks_.emplace(allocated_ptr, MemoryBlock{allocated_ptr, aligned_size});

    return allocated_ptr;
}
// clang-format on
ResultBlank FlexibleCircularAllocator::Deallocate(void* const pointer, const std::size_t)
{
    std::lock_guard<std::mutex> guard(mutex_);
    // remove memory block from allocated_blocks and add it to free_blocks
    if (allocated_blocks_.find(pointer) != allocated_blocks_.cend())
    {
        CollectGarbage(pointer);
        std::ignore = allocated_blocks_.erase(pointer);
        return {};
    }
    return MakeUnexpected(FlexibleAllocatorErrorCode::kInvalidDeallocationAddress);
}
// clang-format off
std::size_t FlexibleCircularAllocator::GetAvailableMemory() noexcept
{
    std::lock_guard<std::mutex> guard(mutex_);
    //to avoid implicit data conversion
    constexpr std::size_t initial_value = 0u;
    return std::accumulate(
        free_blocks_.cbegin(),
        free_blocks_.cend(),
        initial_value,
        [](const std::size_t& running_sum, const auto& block) {
        // No data loss here as the return data type is unsigned long (a4-7-1)
        //  coverity[autosar_cpp14_a4_7_1_violation]
            return running_sum + block.size;
        }

    );
}
// clang-format on

void FlexibleCircularAllocator::GetTmdMemUsage(TmdStatistics&) noexcept {}

void* FlexibleCircularAllocator::GetBaseAddress() const noexcept
{
    return base_address_;
}

std::size_t FlexibleCircularAllocator::GetSize() const noexcept
{
    return total_size_;
}

bool FlexibleCircularAllocator::IsInBounds(const void* const address, const std::size_t size) const noexcept
{
    // NOLINTBEGIN(score-no-pointer-comparison): Both pointers points to the same type of pointer (so no UB).
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic): Bound check is needed.
    if (((address >= GetBaseAddress()) && (size <= total_size_)) &&
        // No harm when do arithmetic operations (m5_0_15_), No harm from using static_cast here(m5-2-8).
        // coverity[autosar_cpp14_m5_2_8_violation]
        // coverity[autosar_cpp14_m5_0_15_violation]
        (address <= static_cast<void*>(static_cast<uint8_t*>(GetBaseAddress()) + GetSize() - size)))
    {
        return true;
    }
    else
    {
        return false;
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic): Bound check is needed.
    // NOLINTEND(score-no-pointer-comparison): Both pointers points to the same type of pointer (so no UB).
}
// clang-format off
bool FlexibleCircularAllocator::MergeTwoBlocksIfApplicable(MemoryBlock& a, MemoryBlock& b)
{
    // set the left and the right block per the address order
    // NOLINTBEGIN(score-no-pointer-comparison): Both pointers points to the same type of pointer (so no UB).
    // No harm when using < operators, both pointers points to the same type of pointer (so no UB) for m5-0-18 and No
    // harm from  doing pointer arithematic here for m5-0-15.
    // coverity[autosar_cpp14_m5_0_18_violation]
    // coverity[autosar_cpp14_m5_0_15_violation]
    MemoryBlock left = a.address < b.address ? a : b;  //
    // No harm when using < operators, both pointers points to the same type of pointer (so no UB) or m5-0-18 and No
    // harm from  doing pointer arithematic here for m5-0-15.
    //  coverity[autosar_cpp14_m5_0_18_violation]
    //  coverity[autosar_cpp14_m5_0_15_violation]
    MemoryBlock right = a.address < b.address ? b : a;
    // No harm from using static_cast here
    // No harm from doing arithermatic operations on the addresses
    // NOLINTEND(score-no-pointer-comparison): Both pointers points to the same type of pointer (so no UB).
    // check if the right block is directly adjacent below/to the left one
    // No harm when do arithmetic operations (m5_0_15_), No harm from using static_cast here(m5-2-8).
    // coverity[autosar_cpp14_m5_2_8_violation]
    // coverity[autosar_cpp14_m5_0_15_violation]
    if (static_cast<std::uint8_t*>(left.address) + left.size  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic): See above==
        // No harm from using static_cast here(m5-2-8).
        // coverity[autosar_cpp14_m5_2_8_violation]
        == static_cast<std::uint8_t*>(
            right.address))  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic): See above
    {
        left.size += right.size;
        // Only the first parameter should be reusable after merge
        a = left;
        return true;
    }

    return false;
}
// clang-format on
// to sort free blocks and merge adjacent blocks;
void FlexibleCircularAllocator::CollectGarbage(void* pointer)
{
    auto is_merge_applicable = false;

    // free_blocks_ is sorted by design
    // We only arrive at this function call if allocated_blocks_[pointer] exists.
    auto next_it = std::lower_bound(free_blocks_.begin(), free_blocks_.end(), allocated_blocks_[pointer]);
    auto previous_it = std::prev(next_it);

    // merge the previous block if applicable
    if (next_it != free_blocks_.begin())
    {
        is_merge_applicable = MergeTwoBlocksIfApplicable(*previous_it, allocated_blocks_[pointer]);
    }

    // merge the next block if applicable
    if (is_merge_applicable)
    {
        if (MergeTwoBlocksIfApplicable(*previous_it, *next_it))
        {
            std::ignore = free_blocks_.erase(next_it);
        }
    }
    else
    {
        is_merge_applicable = MergeTwoBlocksIfApplicable(*next_it, allocated_blocks_[pointer]);
    }

    if (!is_merge_applicable)
    {
        std::ignore = free_blocks_.insert(next_it, {allocated_blocks_[pointer]});
    }
}
}  // namespace tracing
}  // namespace analysis
}  // namespace score
