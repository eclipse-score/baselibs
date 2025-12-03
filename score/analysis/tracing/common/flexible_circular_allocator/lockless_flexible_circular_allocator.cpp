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
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/error_code.h"

#include <score/utility.hpp>
#include <memory>

namespace score
{
namespace analysis
{
namespace tracing
{
namespace
{
// Cast from uint8_t* to BufferBlock* for accessing block metadata
// Validates alignment before casting to ensure safe type-punning
static inline BufferBlock* cast_to_buffer_block(uint8_t* from) noexcept
{
    // Validate alignment before casting
    void* aligned_ptr = from;
    std::size_t space = sizeof(BufferBlock);

    // Check if pointer is properly aligned for BufferBlock
    if ((std::align(alignof(BufferBlock), sizeof(BufferBlock), aligned_ptr, space) == nullptr) ||
        (aligned_ptr != static_cast<void*>(from)))
    {
        return nullptr;  // Alignment validation failed
    }

    // Suppress "AUTOSAR C++14 A5-2-4" rule finding. This rule states:
    // "reinterpret_cast shall not be used."
    // Rationale: BufferBlock* pointer is used to access block metadata stored at calculated offsets.
    // Alignment has been validated above using std::align before performing the cast.
    // coverity[autosar_cpp14_a5_2_4_violation]
    return reinterpret_cast<BufferBlock*>(from);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast): Required for
                                                  // type-punning byte buffer to structured metadata
}
}  // anonymous namespace
constexpr std::uint8_t kMaxRetries = 200u;
constexpr std::uint32_t kInvalidAddressValue = 0xFFFFFFFFUL;

template <template <class> class AtomicIndirectorType>
LocklessFlexibleCircularAllocator<AtomicIndirectorType>::LocklessFlexibleCircularAllocator(void* base_address,
                                                                                           std::size_t size)
    : IFlexibleCircularAllocator(),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): tolerated for algorithm
      base_address_(base_address),
      // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
      // not lead to data loss.".
      // Rationale: casting from std::size_t to std::uint32_t won't cause data loss.
      // coverity[autosar_cpp14_a4_7_1_violation]
      total_size_(static_cast<std::uint32_t>(size)),
      gap_address_(kInvalidAddressValue),
      buffer_queue_head_{0U},
      buffer_queue_tail_{0U},
      list_array_{},
      list_queue_head_{0U},
      list_queue_tail_{0U},
      // Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
      // being given values that are not subsequently used"
      // False positive, variable is used.
      // coverity[autosar_cpp14_a0_1_1_violation : FALSE]
      available_size_(total_size_),
      wrap_around_(false),
      cumulative_usage_(0U),
      lowest_size_(total_size_),
      alloc_cntr_(0U),
      dealloc_cntr_(0U),
      tmd_stats_enabled_(false),
      last_error_code_(0)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(list_queue_head_.is_always_lock_free == true, "ListQueue head is not lock free");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(list_queue_head_.is_always_lock_free == true, "ListQueue tail is not lock free");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(buffer_queue_head_.is_always_lock_free == true, "BufferQueue head is not lock free");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(buffer_queue_tail_.is_always_lock_free == true, "BufferQueue tail is not lock free");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(std::atomic<ListEntry>{}.is_always_lock_free == true,
                                 "ListEntry structure is not lock free");
}

template <template <class> class AtomicIndirectorType>
std::size_t LocklessFlexibleCircularAllocator<AtomicIndirectorType>::GetAvailableMemory() noexcept
{
    return static_cast<std::size_t>(available_size_);
}

template <template <class> class AtomicIndirectorType>
std::uint32_t LocklessFlexibleCircularAllocator<AtomicIndirectorType>::GetListQueueNextHead()
{
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
    // not lead to data loss.".
    // Rationale: All operands and operations are performed using std::uint32_t. Since both
    // the values and the resulting expression are within the range of a 32-bit unsigned integer,
    // no data loss can occur.
    // coverity[autosar_cpp14_a4_7_1_violation]
    std::uint32_t head = (list_queue_head_.load() + 1U) % (kListEntryArraySize - 1U);
    return head;
}

template <template <class> class AtomicIndirectorType>
void LocklessFlexibleCircularAllocator<AtomicIndirectorType>::GetTmdMemUsage(TmdStatistics& tmd_stats) noexcept
{
    tmd_stats_enabled_.store(true, std::memory_order_release);
    static_assert(sizeof(std::size_t) >= sizeof(std::uint32_t),
                  "std::size_t must be able to represent all values of std::uint32_t");
    tmd_stats.tmd_max =
        static_cast<std::size_t>(total_size_) - static_cast<std::size_t>(lowest_size_.exchange(total_size_));
    const std::uint32_t number_of_allocations = std::max(1U, alloc_cntr_.exchange(0U));
    tmd_stats.tmd_average = cumulative_usage_.exchange(0U) / number_of_allocations;
    tmd_stats.tmd_alloc_rate =
        static_cast<float>(dealloc_cntr_.exchange(0U)) / static_cast<float>(number_of_allocations);
}

template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
void* LocklessFlexibleCircularAllocator<AtomicIndirectorType>::Allocate(const std::size_t size,
                                                                        const std::size_t alignment_size) noexcept
{
    ClearError();
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
    // not lead to data loss.".
    // Rationale: The addition of 'size' and 'sizeof(BufferBlock)' is performed using std::size_t arithmetic.
    // A runtime check ensures that the sum does not overflow, preventing any loss of data. Although the result
    // (aligned_size) is computed as a std::size_t, this calculation is safe because the check guarantees that no
    // overflow occurs.
    // coverity[autosar_cpp14_a4_7_1_violation]
    if (size > std::numeric_limits<std::size_t>::max() - sizeof(BufferBlock))
    {
        return nullptr;
    }
    const std::size_t aligned_size = GetAlignedSize(size + sizeof(BufferBlock), alignment_size);

    if (aligned_size >= GetAvailableMemory())
    {

        return nullptr;
    }
    if (aligned_size > std::numeric_limits<std::uint32_t>::max())
    {
        return nullptr;
    }
    std::ignore = available_size_.fetch_sub(static_cast<std::uint32_t>(aligned_size), std::memory_order_seq_cst);

    std::uint32_t list_entry_element_index = 0U;
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto old_list_queue_head = list_queue_head_.load();
        auto new_list_queue_head = GetListQueueNextHead();
        if (AtomicIndirectorType<decltype(list_queue_head_.load())>::compare_exchange_strong(
                list_queue_head_, old_list_queue_head, new_list_queue_head, std::memory_order_seq_cst) == true)
        {
            list_entry_element_index = new_list_queue_head;
            break;
        }

        if (retries == kMaxRetries - 1U)
        {
            return nullptr;
        }
    }
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
    // not lead to data loss.".
    // Rationale: The subtraction (total_size_ - buffer_queue_head_.load()) is performed using std::uint32_t,
    // ensuring that the result is within the 32-bit range. Although 'aligned_size' is of type std::size_t,
    // in this context its value is expected to be within the 32-bit range, so the comparison is safe.
    // coverity[autosar_cpp14_a4_7_1_violation]
    if (total_size_ - buffer_queue_head_.load() <= static_cast<uint32_t>(aligned_size))
    {
        wrap_around_.store(true);
        gap_address_.store(buffer_queue_head_.load());
    }
    void* allocated_address = nullptr;

    // This prevents race condition where multiple threads think they should wrap around
    bool expected_wrap_around = true;
    if (AtomicIndirectorType<bool>::compare_exchange_strong(
            wrap_around_, expected_wrap_around, false, std::memory_order_seq_cst))
    {
        // Only one thread will successfully enter this branch
        static_assert(sizeof(std::size_t) >= sizeof(std::uint32_t),
                      "std::size_t must be able to represent all values of std::uint32_t");
        // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
        // not lead to data loss.".
        // Rationale: casting from std::size_t to std::uint32_t won't cause data loss.
        // coverity[autosar_cpp14_a4_7_1_violation]
        allocated_address = AllocateWithWrapAround(static_cast<std::uint32_t>(aligned_size), list_entry_element_index);
    }
    else
    {
        // Either wrap_around_ was false, or another thread already claimed the wrap-around
        static_assert(sizeof(std::size_t) >= sizeof(std::uint32_t),
                      "std::size_t must be able to represent all values of std::uint32_t");
        // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
        // not lead to data loss.".
        // Rationale: casting from std::size_t to std::uint32_t won't cause data loss.
        // coverity[autosar_cpp14_a4_7_1_violation]
        allocated_address =
            AllocateWithNoWrapAround(static_cast<std::uint32_t>(aligned_size), list_entry_element_index);
    }

    if ((nullptr != allocated_address) && tmd_stats_enabled_.load(std::memory_order_acquire))
    {
        const std::uint32_t available_tmd_size = available_size_.load(std::memory_order_seq_cst);
        lowest_size_ = std::min(available_tmd_size, lowest_size_.load(std::memory_order_seq_cst));

        static_assert(sizeof(std::size_t) >= sizeof(std::uint32_t),
                      "std::size_t must be able to represent all values of std::uint32_t");
        // Both total_size_ and available_tmd_size are std::uint32_t. Casted to std::size_t for safe subtraction.
        // The subtraction result is guaranteed to be within [0, total_size_] since available_tmd_size <=
        // total_size_ by design. cumulative_usage_ is std::uint64_t with ample headroom for accumulation,
        // and is periodically reset via GetTmdMemUsage().
        cumulative_usage_ += (static_cast<std::size_t>(total_size_) - static_cast<std::size_t>(available_tmd_size));
        alloc_cntr_++;
    }

    return allocated_address;
}
template <template <class> class AtomicIndirectorType>
void LocklessFlexibleCircularAllocator<AtomicIndirectorType>::ResetBufferQueuTail()
{
    // The retries loop is designed to secure successful completion well within the set limit. kMaxRetries
    // is intentionally set high to ensure operations reliably complete without reaching it, aligning with
    // the system's robustness goals. Scenarios hitting the max retries conflict with this design, focusing on
    // success within fewer attempts.
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)  // LCOV_EXCL_BR_LINE not testable see comment above.
    {
        auto old_buffer_queue_tail = buffer_queue_tail_.load();
        if (AtomicIndirectorType<decltype(buffer_queue_tail_.load())>::compare_exchange_strong(
                buffer_queue_tail_, old_buffer_queue_tail, 0U, std::memory_order_seq_cst) == true)
        {
            break;
        }
    }
}
template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A8-4-10", The rule states: "A parameter shall be passed by reference if it can’t be NULL".
// The whole algorithm by design here relied on address manipulation
// coverity[autosar_cpp14_a8_4_10_violation]
void LocklessFlexibleCircularAllocator<AtomicIndirectorType>::MarkListEntryAsFree(const BufferBlock* meta)
{
    // Validate list_entry_offset before using it to prevent out-of-bounds access
    if (!ValidateListEntryIndex(meta->list_entry_offset))
    {
        SetError(FlexibleAllocatorErrorCode::kCorruptedBufferBlock);
        return;  // Early return to prevent crash
    }
    // The retries loop is designed to secure successful completion well within the set limit. kMaxRetries
    // is intentionally set high to ensure operations reliably complete without reaching it, aligning with
    // the system's robustness goals. Scenarios hitting the max retries conflict with this design, focusing on
    // success within fewer attempts.
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)  // LCOV_EXCL_BR_LINE not testable see comment above.
    {
        auto list_entry_old = list_array_.at(static_cast<size_t>(meta->list_entry_offset)).load();
        auto list_entry_new = list_entry_old;
        list_entry_new.flags = static_cast<std::uint8_t>(ListEntryFlag::kFree);
        if (AtomicIndirectorType<ListEntry>::compare_exchange_strong(
                list_array_.at(static_cast<size_t>(meta->list_entry_offset)),
                list_entry_old,
                list_entry_new,
                std::memory_order_seq_cst) == true)
        {
            break;
        }
    }
}
template <template <class> class AtomicIndirectorType>
bool LocklessFlexibleCircularAllocator<AtomicIndirectorType>::IsRequestedBlockAtBufferQueueTail(
    // Suppress "AUTOSAR C++14 A8-4-10", The rule states: "A parameter shall be passed by reference if it can’t be
    // NULL". Function scope is pointer address manipulation passing by reference only delays obtaining
    // pointer into function body.
    // coverity[autosar_cpp14_a8_4_10_violation]
    const BufferBlock* meta) const
{
    // Validate list_entry_offset before using it to prevent out-of-bounds access
    if (!ValidateListEntryIndex(meta->list_entry_offset))
    {
        SetError(FlexibleAllocatorErrorCode::kCorruptedBufferBlock);
        return false;  // For invalid offset
    }
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
    // not lead to data loss.".
    // Rationale: All operands and operations are performed using std::uint32_t. Since both
    // the values and the resulting expression are within the range of a 32-bit unsigned integer,
    // no data loss can occur.
    // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
    // not be implicitly converted to a different underlying type"
    // False positive, right hand value is the same type.
    // coverity[autosar_cpp14_m5_0_3_violation]
    // coverity[autosar_cpp14_a4_7_1_violation]
    return ((list_array_.at(meta->list_entry_offset).load().offset -
             // coverity[autosar_cpp14_m5_0_3_violation]
             list_array_.at(meta->list_entry_offset).load().length) == buffer_queue_tail_.load()) ||
           (buffer_queue_tail_.load() == 0U);
}

template <template <class> class AtomicIndirectorType>
void LocklessFlexibleCircularAllocator<AtomicIndirectorType>::IterateBlocksToDeallocate()
{
    auto init_tail = buffer_queue_tail_.load();
    while (init_tail != buffer_queue_head_.load())
    {
        uint8_t* block_ptr = GetBufferPositionAt(init_tail);
        if (block_ptr == nullptr)
        {
            SetError(FlexibleAllocatorErrorCode::kCorruptedBufferBlock);
            break;  // Out-of-bounds offset detected
        }
        BufferBlock* current_block = cast_to_buffer_block(block_ptr);
        if (current_block == nullptr)
        {
            SetError(FlexibleAllocatorErrorCode::kCorruptedBufferBlock);
            break;  // Alignment validation failed, stop iteration
        }
        if (init_tail == 0U)
        {
            MarkListEntryAsFree(current_block);
            // Check if MarkListEntryAsFree encountered an error
            auto error = GetLastError();
            if (*error != 0)
            {
                break;  // break the loop to avoid further corruption
            }
        }
        // list_entry_offset is always set internally via controlled allocation paths, making invalid
        // indices impossible during normal operation.
        auto index = current_block->list_entry_offset;
        if (ValidateListEntryIndex(index))  // LCOV_EXCL_BR_LINE not testable
        // see comment above.
        {
            if ((AtomicIndirectorType<ListEntry>::load(
                     list_array_.at(static_cast<size_t>(current_block->list_entry_offset)), std::memory_order_seq_cst)
                     .flags) == static_cast<std::uint8_t>(ListEntryFlag::kFree))
            {
                FreeBlock(*current_block);
                // Check if FreeBlock encountered an error
                auto error = GetLastError();
                if (*error != 0)
                {
                    break;  // break the loop to avoid further corruption
                }

                init_tail += current_block->block_length;

                if (((init_tail == gap_address_.load()) && (init_tail != buffer_queue_head_.load())) ||
                    (init_tail >= total_size_))
                {
                    // The retries loop is designed to secure successful completion well within the set limit.
                    // kMaxRetries is intentionally set high to ensure operations reliably complete without reaching it,
                    // aligning with the system's robustness goals. Scenarios hitting the max retries conflict with this
                    // design, focusing on success within fewer attempts.
                    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)  // LCOV_EXCL_BR_LINE not
                    // testable see comment above.
                    {
                        auto old_buffer_queue_tail = buffer_queue_tail_.load();
                        if (AtomicIndirectorType<decltype(buffer_queue_tail_.load())>::compare_exchange_strong(
                                buffer_queue_tail_, old_buffer_queue_tail, 0U, std::memory_order_seq_cst) == true)
                        {
                            gap_address_.store(kInvalidAddressValue);
                            break;
                        }
                    }
                    init_tail = 0U;
                }
            }
            else
            {
                break;
            }
        }
    }
}
template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
bool LocklessFlexibleCircularAllocator<AtomicIndirectorType>::Deallocate(void* const addr, const std::size_t) noexcept
{
    ClearError();

    // Validate bounds
    if (!IsInBounds(addr, 0U))
    {
        return false;
    }

    // Validate alignment using std::align to avoid pointer-to-integer casts
    void* aligned_ptr = addr;
    std::size_t space = sizeof(std::max_align_t);
    if ((std::align(alignof(std::max_align_t), sizeof(std::max_align_t), aligned_ptr, space) == nullptr) ||
        (aligned_ptr != addr))
    {
        return false;  // Alignment validation failed
    }

    auto user_data_ptr =
        // Suppress "AUTOSAR C++14 A5-2-4" rule finding. This rule states:
        // "reinterpret_cast shall not be used."
        // Suppress "AUTOSAR C++14 M5-2-8" rule finding. This rule states:
        // "An object with integer type or pointer to void type shall not be
        // converted to an object with pointer type"
        // Rationale: BufferBlock* pointer is used to access block metadata stored at calculated offsets.
        // Alignment has been validated above using std::align before performing the cast.
        // coverity[autosar_cpp14_a5_2_4_violation]
        // coverity[autosar_cpp14_m5_2_8_violation]
        reinterpret_cast<BufferBlock*>(addr);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast): Cast user data
                                               // address to BufferBlock* for pointer arithmetic

    // Suppress "AUTOSAR C++14 M5-0-15" rule finding. This rule states:
    // "Array indexing shall be the only form of pointer arithmetic."
    // Rationale: Every block's metadata is located immediately before its user data.
    // To access the metadata, we subtract one BufferBlock from the user data pointer.
    // Memory layout: [BufferBlock metadata][User data at 'addr']
    // coverity[autosar_cpp14_m5_0_15_violation]
    BufferBlock* meta = user_data_ptr - 1;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic): Required for
                                            // accessing metadata block before user data
    if (buffer_queue_tail_.load() == gap_address_)
    {
        ResetBufferQueuTail();
    }

    MarkListEntryAsFree(meta);
    // Check if MarkListEntryAsFree encountered an error
    auto error = GetLastError();
    if (*error != 0)
    {
        return false;
    }

    if (IsRequestedBlockAtBufferQueueTail(meta))
    {
        IterateBlocksToDeallocate();
    }
    // Note: If IsRequestedBlockAtBufferQueueTail() fails due to corruption, it returns false
    // and sets an error code. Since we don't call IterateBlocksToDeallocate() in that case,
    // the primary deallocation still succeeds, which is the desired behavior.
    dealloc_cntr_++;
    return true;
}

template <template <class> class AtomicIndirectorType>
void LocklessFlexibleCircularAllocator<AtomicIndirectorType>::FreeBlock(BufferBlock& current_block)
{
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto old_buffer_queue_tail = buffer_queue_tail_.load();
        auto new_buffer_queue_tail = old_buffer_queue_tail + static_cast<uint32_t>(current_block.block_length);
        if (AtomicIndirectorType<decltype(buffer_queue_tail_.load())>::compare_exchange_strong(
                buffer_queue_tail_, old_buffer_queue_tail, new_buffer_queue_tail, std::memory_order_seq_cst) == true)
        {
            std::ignore = available_size_.fetch_add(static_cast<std::uint32_t>(current_block.block_length),
                                                    std::memory_order_seq_cst);
            break;
        }
    }

    // Validate list_entry_offset before using it to prevent out-of-bounds access
    if (!ValidateListEntryIndex(current_block.list_entry_offset))
    {
        SetError(FlexibleAllocatorErrorCode::kInvalidListEntryOffset);
        return;  // Early return to prevent crash
    }

    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto list_entry_old = list_array_.at(static_cast<size_t>(current_block.list_entry_offset)).load();
        auto list_entry_new = list_entry_old;
        list_entry_new.flags = static_cast<std::uint8_t>(ListEntryFlag::kFree);
        list_entry_new.length = 0U;
        list_entry_new.offset = 0U;
        // LCOV_EXCL_START The line in the decision report is counted as 0/2 decision taken. While the tests are
        // covering the both cases in multiple tests. Therefore, it had to be skipped.
        if (AtomicIndirectorType<ListEntry>::compare_exchange_strong(
                list_array_.at(static_cast<size_t>(current_block.list_entry_offset)),
                list_entry_old,
                list_entry_new,
                std::memory_order_seq_cst) == true)
        {
            break;
        }
        // LCOV_EXCL_STOP
    }
    // The retries loop is designed to secure successful completion well within the set limit. kMaxRetries
    // is intentionally set high to ensure operations reliably complete without reaching it, aligning with
    // the system's robustness goals. Scenarios hitting the max retries conflict with this design, focusing on
    // success within fewer attempts.
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)  // LCOV_EXCL_BR_LINE not testable see comment above.
    {
        auto old_list_queue_tail = list_queue_tail_.load();
        auto new_list_queue_tail = current_block.list_entry_offset;
        if (AtomicIndirectorType<decltype(list_queue_tail_.load())>::compare_exchange_strong(
                list_queue_tail_, old_list_queue_tail, new_list_queue_tail, std::memory_order_seq_cst) == true)
        {
            break;
        }
    }
}

template <template <class> class AtomicIndirectorType>
void* LocklessFlexibleCircularAllocator<AtomicIndirectorType>::GetBaseAddress() const noexcept
{
    return base_address_;
}

template <template <class> class AtomicIndirectorType>
std::size_t LocklessFlexibleCircularAllocator<AtomicIndirectorType>::GetSize() const noexcept
{
    return static_cast<std::size_t>(total_size_);
}

template <template <class> class AtomicIndirectorType>
bool LocklessFlexibleCircularAllocator<AtomicIndirectorType>::IsInBounds(const void* const address,
                                                                         const std::size_t size) const noexcept
{
    if (GetSize() < size)
    {
        return false;
    }
    // NOLINTBEGIN(score-no-pointer-comparison): Needed pointer comparisons for validating memory ranges safely
    uint8_t* end_ptr = GetBufferPositionAt(GetSize() - size);
    if (end_ptr == nullptr)
    {
        return false;  // Invalid offset, out of bounds
    }
    if ((address >= GetBaseAddress()) && (address <= static_cast<void*>(end_ptr)))
    {
        return true;
    }
    // NOLINTEND(score-no-pointer-comparison)
    else
    {
        return false;
    }
}

template <template <class> class AtomicIndirectorType>
uint8_t* LocklessFlexibleCircularAllocator<AtomicIndirectorType>::AllocateWithWrapAround(
    std::uint32_t aligned_size,
    std::uint32_t list_entry_element_index)
{
    uint8_t* allocated_address = nullptr;
    auto new_buffer_queue_head = 0U;
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto old_buffer_queue_head = buffer_queue_head_.load();
        new_buffer_queue_head = static_cast<uint32_t>(aligned_size);

        if (AtomicIndirectorType<decltype(buffer_queue_head_.load())>::compare_exchange_strong(
                buffer_queue_head_, old_buffer_queue_head, new_buffer_queue_head, std::memory_order_seq_cst) == true)
        {
            break;
        }
    }
    if (new_buffer_queue_head < aligned_size)
    {
        return nullptr;
    }
    uint8_t* block_ptr = GetBufferPositionAt(static_cast<std::uint32_t>(new_buffer_queue_head) - aligned_size);
    if (block_ptr == nullptr)
    {
        return nullptr;  // Out-of-bounds offset detected
    }
    auto block_meta_data = cast_to_buffer_block(block_ptr);
    if (block_meta_data == nullptr)
    {
        return nullptr;  // Alignment validation failed in cast_to_buffer_block
    }
    block_meta_data->list_entry_offset = list_entry_element_index;
    block_meta_data->block_length = static_cast<uint32_t>(aligned_size);
    allocated_address = GetBufferPositionAt(static_cast<std::size_t>(new_buffer_queue_head) -
                                            static_cast<std::size_t>(aligned_size) + sizeof(BufferBlock));
    if (allocated_address == nullptr)
    {
        return nullptr;  // Out-of-bounds offset detected
    }
    if (aligned_size > std::numeric_limits<std::uint16_t>::max())
    {
        // Return early if aligned_size exceeds the maximum value representable by std::uint32_t,
        // since list_entry_new.length is a std::uint16_t and cannot hold a larger value.
        return nullptr;
    }

    // Validate list_entry_element_index before using it to prevent out-of-bounds access
    if (!ValidateListEntryIndex(list_entry_element_index))
    {
        SetError(FlexibleAllocatorErrorCode::kInvalidListEntryOffset);
        return nullptr;
    }

    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto list_entry_old = list_array_.at(static_cast<size_t>(list_entry_element_index)).load();
        auto list_entry_new = list_entry_old;
        list_entry_new.flags = static_cast<std::uint8_t>(ListEntryFlag::kInUse);
        list_entry_new.length = static_cast<std::uint16_t>(aligned_size);
        list_entry_new.offset = (static_cast<std::uint32_t>(aligned_size) + new_buffer_queue_head - aligned_size);
        if (AtomicIndirectorType<ListEntry>::compare_exchange_strong(
                list_array_.at(static_cast<size_t>(list_entry_element_index)),
                list_entry_old,
                list_entry_new,
                std::memory_order_seq_cst) == true)
        {
            break;
        }
    }

    return allocated_address;
}
template <template <class> class AtomicIndirectorType>
uint8_t* LocklessFlexibleCircularAllocator<AtomicIndirectorType>::AllocateWithNoWrapAround(
    std::uint32_t aligned_size,
    std::uint32_t list_entry_element_index)
{
    uint8_t* allocated_address = nullptr;
    auto offset = 0U;

    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto old_buffer_queue_head = buffer_queue_head_.load();
        auto new_buffer_queue_head = old_buffer_queue_head + static_cast<std::uint32_t>(aligned_size);
        if (AtomicIndirectorType<decltype(buffer_queue_head_.load())>::compare_exchange_strong(
                buffer_queue_head_, old_buffer_queue_head, new_buffer_queue_head, std::memory_order_seq_cst) == true)
        {
            if (new_buffer_queue_head < static_cast<unsigned int>(aligned_size))
            {
                return nullptr;
            }
            offset = new_buffer_queue_head - aligned_size;
            break;
        }
    }
    uint8_t* block_ptr = GetBufferPositionAt(offset);
    if (block_ptr == nullptr)
    {
        return nullptr;  // Out-of-bounds offset detected
    }
    auto block_meta_data = cast_to_buffer_block(block_ptr);
    if (block_meta_data == nullptr)
    {
        return nullptr;  // Alignment validation failed in cast_to_buffer_block
    }
    block_meta_data->list_entry_offset = list_entry_element_index;
    block_meta_data->block_length = static_cast<std::uint32_t>(aligned_size);
    allocated_address = GetBufferPositionAt(static_cast<std::size_t>(offset) + sizeof(BufferBlock));
    if (allocated_address == nullptr)
    {
        return nullptr;  // Out-of-bounds offset detected
    }
    if (aligned_size > std::numeric_limits<std::uint16_t>::max())
    {
        // Return early if aligned_size exceeds the maximum value representable by std::uint32_t,
        // since list_entry_new.length is a std::uint16_t and cannot hold a larger value.
        return nullptr;
    }

    // Validate list_entry_element_index before using it to prevent out-of-bounds access
    if (!ValidateListEntryIndex(list_entry_element_index))
    {
        SetError(FlexibleAllocatorErrorCode::kInvalidListEntryOffset);
        return nullptr;
    }

    // The retries loop is designed to secure successful completion well within the set limit. kMaxRetries
    // is intentionally set high to ensure operations reliably complete without reaching it, aligning with
    // the system's robustness goals. Scenarios hitting the max retries conflict with this design, focusing on
    // success within fewer attempts.
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)  // LCOV_EXCL_BR_LINE not testable see comment above.
    {
        auto list_entry_old = list_array_.at(static_cast<size_t>(list_entry_element_index)).load();
        auto list_entry_new = list_entry_old;
        list_entry_new.flags = static_cast<std::uint8_t>(ListEntryFlag::kInUse);
        list_entry_new.length = static_cast<std::uint16_t>(aligned_size);
        list_entry_new.offset = (static_cast<std::uint32_t>(aligned_size) + offset);
        if (AtomicIndirectorType<ListEntry>::compare_exchange_strong(
                list_array_.at(static_cast<size_t>(list_entry_element_index)),
                list_entry_old,
                list_entry_new,
                std::memory_order_seq_cst) == true)
        {
            break;
        }
    }

    return allocated_address;
}

template <template <class> class AtomicIndirectorType>
bool LocklessFlexibleCircularAllocator<AtomicIndirectorType>::ValidateListEntryIndex(const std::uint32_t& index) const
{
    bool result = false;
    if (index < kListEntryArraySize)
    {
        result = true;
    }
    return result;
}
template <template <class> class AtomicIndirectorType>
void LocklessFlexibleCircularAllocator<AtomicIndirectorType>::SetError(
    FlexibleAllocatorErrorCode error_code) const noexcept
{
    last_error_code_.store(static_cast<score::result::ErrorCode>(error_code), std::memory_order_seq_cst);
}

template <template <class> class AtomicIndirectorType>
score::result::Error LocklessFlexibleCircularAllocator<AtomicIndirectorType>::GetLastError() const noexcept
{
    auto error_code = last_error_code_.load(std::memory_order_seq_cst);
    // Suppress "AUTOSAR C++14 A7-2-1" rule finding. This rule states:
    // "An expression with enum underlying type shall only have values corresponding to the enumerators of the
    // enumeration." Rationale: The error_code value is always set via SetError() which stores valid
    // FlexibleAllocatorErrorCode enumerators. This cast is the inverse operation of SetError() and is guaranteed to
    // produce valid enum values by design.
    // coverity[autosar_cpp14_a7_2_1_violation]
    return MakeError(static_cast<FlexibleAllocatorErrorCode>(error_code));
}

template <template <class> class AtomicIndirectorType>
void LocklessFlexibleCircularAllocator<AtomicIndirectorType>::ClearError() noexcept
{
    last_error_code_.store(0, std::memory_order_seq_cst);
}

template <template <class> class AtomicIndirectorType>
template <typename OffsetT>
uint8_t* LocklessFlexibleCircularAllocator<AtomicIndirectorType>::GetBufferPositionAt(OffsetT offset) const noexcept
{
    static_assert(std::is_integral_v<OffsetT>, "OffsetT must be an integral type");
    static_assert(sizeof(std::size_t) >= sizeof(std::uint32_t),
                  "std::size_t must be able to represent all values of std::uint32_t");

    // Runtime bounds validation - ensures offset is within buffer bounds
    // Returns nullptr if offset exceeds total_size_ to prevent out-of-bounds access
    // Suppress "AUTOSAR C++14 A7-1-8" rule finding. This rule states:
    // "A non-type specifier shall be placed before a type specifier in a declaration."
    // Rationale: Not a typical use case of this rule. Here, we are using constexpr before
    // if condition, not in declaration context. (false positive).
    // coverity[autosar_cpp14_a7_1_8_violation: False]
    if constexpr (std::is_signed_v<OffsetT>)
    {
        if (offset < 0)
        {
            return nullptr;  // Negative offset is out-of-bounds
        }
    }
    if (static_cast<std::size_t>(offset) > static_cast<std::size_t>(total_size_))
    {
        return nullptr;  // Out-of-bounds offset
    }
    // Suppress "AUTOSAR C++14 M5-2-8" rule finding. This rule states:
    // "An object with integer type or pointer to void type shall not be
    // converted to an object with pointer type"
    // Rationale: uint8_t is the smallest addressable unit. No UB arises as we're converting
    // to unsigned char type which can alias any object. Used for calculating positions within
    // the allocator's managed memory buffer.
    // coverity[autosar_cpp14_m5_2_8_violation]
    auto byte_ptr = static_cast<uint8_t*>(base_address_);
    // Suppress "AUTOSAR C++14 M5-0-15" rule finding. This rule states:
    // "Array indexing shall be the only form of pointer arithmetic."
    // Rationale: Array indexing is used for pointer arithmetic. Offset validity has been verified
    // above to ensure it does not exceed the buffer size.
    // coverity[autosar_cpp14_m5_0_15_violation]
    return &byte_ptr[offset];  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic): Array indexing is used for
                               // pointer arithmetic
}

template class LocklessFlexibleCircularAllocator<score::memory::shared::AtomicIndirectorReal>;
template class LocklessFlexibleCircularAllocator<score::memory::shared::AtomicIndirectorMock>;

}  // namespace tracing
}  // namespace analysis
}  // namespace score
