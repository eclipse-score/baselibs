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
static inline score::Result<BufferBlock*> cast_to_buffer_block(uint8_t* from) noexcept
{
    // Validate alignment before casting
    void* aligned_ptr = from;
    std::size_t space = sizeof(BufferBlock);

    // Check if pointer is properly aligned for BufferBlock
    if ((std::align(alignof(BufferBlock), sizeof(BufferBlock), aligned_ptr, space) == nullptr) ||
        (aligned_ptr != static_cast<void*>(from)))
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kAddressNotAligned);  // Alignment validation failed
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
      tmd_stats_enabled_(false)
{
    // Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
    // being given values that are not subsequently used"
    // False positive, no declarations here.
    // coverity[autosar_cpp14_a0_1_1_violation : FALSE]
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(list_queue_head_.is_always_lock_free == true, "ListQueue head is not lock free");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(list_queue_tail_.is_always_lock_free == true, "ListQueue tail is not lock free");
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
std::uint32_t LocklessFlexibleCircularAllocator<AtomicIndirectorType>::GetListQueueNextHead(
    const std::uint32_t current_head) const
{
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
    // not lead to data loss.".
    // Rationale: All operands and operations are performed using std::uint32_t. Since both
    // the values and the resulting expression are within the range of a 32-bit unsigned integer,
    // no data loss can occur.
    // coverity[autosar_cpp14_a4_7_1_violation]
    std::uint32_t head = (current_head + 1U) % (kListEntryArraySize - 1U);
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

    tmd_stats.tmd_allocate_retry_cntr_res = allocate_retry_cntr_.ExchangeWithZero();

    tmd_stats.tmd_allocate_call_cntr_res = allocate_call_cntr_.ExchangeWithZero();
}

template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
score::Result<std::uint32_t> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::ValidateAndReserveMemory(
    std::size_t size,
    std::size_t alignment_size) noexcept
{
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
    // not lead to data loss.".
    // Rationale: The addition of 'size' and 'sizeof(BufferBlock)' is performed using std::size_t arithmetic.
    // A runtime check ensures that the sum does not overflow, preventing any loss of data. The result is then
    // validated to fit within std::uint32_t range before casting, ensuring no data loss occurs.
    // coverity[autosar_cpp14_a4_7_1_violation]
    if (size > std::numeric_limits<std::size_t>::max() - sizeof(BufferBlock))
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kOverFlowOccurred);
    }
    auto aligned_size_result = GetAlignedSize(size + sizeof(BufferBlock), alignment_size);
    if (!aligned_size_result.has_value())
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kOverFlowOccurred);
    }
    const std::size_t aligned_size = aligned_size_result.value();

    // Check for uint32_t overflow before checking available memory
    if (aligned_size > std::numeric_limits<std::uint32_t>::max())
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kOverFlowOccurred);
    }

    const auto aligned_size_u32 = static_cast<std::uint32_t>(aligned_size);
    bool subtraction_successful = false;
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        std::uint32_t available = available_size_.load(std::memory_order_seq_cst);
        if (aligned_size_u32 >= available)
        {
            return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kNotEnoughMemory);
        }
        if (AtomicIndirectorType<decltype(available_size_.load())>::compare_exchange_weak(available_size_,
                                                                                          available,
                                                                                          available - aligned_size_u32,
                                                                                          std::memory_order_seq_cst,
                                                                                          std::memory_order_seq_cst))
        {
            subtraction_successful = true;
            break;
        }
    }
    if (!subtraction_successful)
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kViolatedMaximumRetries);
    }
    return aligned_size_u32;
}

template <template <class> class AtomicIndirectorType>
score::Result<std::uint32_t> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::AcquireListQueueEntry() noexcept
{
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto old_list_queue_head = list_queue_head_.load();
        auto new_list_queue_head = GetListQueueNextHead(old_list_queue_head);

        // Check if the queue is full (head would catch up to tail)
        if (new_list_queue_head == list_queue_tail_.load(std::memory_order_seq_cst))
        {
            return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kListQueueFull);
        }

        if (AtomicIndirectorType<decltype(list_queue_head_.load())>::compare_exchange_strong(
                list_queue_head_, old_list_queue_head, new_list_queue_head, std::memory_order_seq_cst) == true)
        {
            return new_list_queue_head;
        }
        [[maybe_unused]] auto retry_cntr_incr_res = allocate_retry_cntr_.Increment();
    }
    return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kViolatedMaximumRetries);
}

template <template <class> class AtomicIndirectorType>
void LocklessFlexibleCircularAllocator<AtomicIndirectorType>::UpdateAllocationStatistics() noexcept
{
    if (!tmd_stats_enabled_.load(std::memory_order_acquire))
    {
        return;
    }

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

template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
score::Result<void*> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::Allocate(
    const std::size_t size,
    const std::size_t alignment_size) noexcept
{
    [[maybe_unused]] auto call_cntr_incr_res = allocate_call_cntr_.Increment();
    // ValidateAndReserveMemory atomically checks availability and reserves memory using CAS
    auto aligned_size_result = ValidateAndReserveMemory(size, alignment_size);
    if (!aligned_size_result.has_value())
    {
        return MakeUnexpected<void*>(aligned_size_result.error());
    }
    const std::uint32_t aligned_size = aligned_size_result.value();
    auto list_entry_result = AcquireListQueueEntry();
    if (!list_entry_result.has_value())
    {
        // Rollback available size on failure
        IncrementAvailableSize(aligned_size);
        return MakeUnexpected<void*>(list_entry_result.error());
    }
    const std::uint32_t list_entry_element_index = list_entry_result.value();
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
    // not lead to data loss.".
    // Rationale: The subtraction (total_size_ - buffer_queue_head_.load()) is performed using std::uint32_t,
    // ensuring that the result is within the 32-bit range. aligned_size is validated to be within uint32_t
    // range by ValidateAllocationRequest(), so the comparison is safe.
    // coverity[autosar_cpp14_a4_7_1_violation]
    if (total_size_ - buffer_queue_head_.load() <= aligned_size)
    {
        if (buffer_queue_tail_.load(std::memory_order_seq_cst) < static_cast<std::uint32_t>(aligned_size))
        {
            // Rollback available size on failure
            IncrementAvailableSize(aligned_size);
            return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kNotEnoughMemory);
        }
        wrap_around_.store(true);
        gap_address_.store(buffer_queue_head_.load());
    }

    score::Result<void*> allocated_address = nullptr;

    // This prevents race condition where multiple threads think they should wrap around
    bool expected_wrap_around = true;
    if (AtomicIndirectorType<bool>::compare_exchange_strong(
            wrap_around_, expected_wrap_around, false, std::memory_order_seq_cst))
    {
        // Only one thread will successfully enter this branch
        allocated_address = AllocateWithWrapAround(aligned_size, list_entry_element_index);
    }
    else
    {
        // Either wrap_around_ was false, or another thread already claimed the wrap-around
        allocated_address = AllocateWithNoWrapAround(aligned_size, list_entry_element_index);
    }
    if (!allocated_address.has_value())
    {
        // Rollback available size on failure
        IncrementAvailableSize(aligned_size);
        return MakeUnexpected<void*>(allocated_address.error());
    }
    if (nullptr != allocated_address.value())
    {
        UpdateAllocationStatistics();
    }

    return allocated_address;
}
template <template <class> class AtomicIndirectorType>
bool LocklessFlexibleCircularAllocator<AtomicIndirectorType>::ResetBufferQueuTail()
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
            return true;
        }
    }
    return false;  // LCOV_EXCL_LINE not testable
}
template <template <class> class AtomicIndirectorType>
void LocklessFlexibleCircularAllocator<AtomicIndirectorType>::ResetBufferTailAndClearGapAddress()
{
    // Only clear gap_address_ if the CAS to reset buffer_queue_tail_ succeeds,
    // preserving the original atomic semantics where both operations are linked.
    if (ResetBufferQueuTail())
    {
        gap_address_.store(kInvalidAddressValue);
    }
}
template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A8-4-10", The rule states: "A parameter shall be passed by reference if it can’t be NULL".
// The whole algorithm by design here relied on address manipulation
// coverity[autosar_cpp14_a8_4_10_violation]
ResultBlank LocklessFlexibleCircularAllocator<AtomicIndirectorType>::MarkListEntryAsFree(const BufferBlock* meta)
{
    // Validate list_entry_offset before using it to prevent out-of-bounds access
    if (!ValidateListEntryIndex(meta->list_entry_offset))
    {
        return MakeUnexpected(
            LocklessFlexibleAllocatorErrorCode::kCorruptedBufferBlock);  // Early return to prevent crash
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

    return {};
}
template <template <class> class AtomicIndirectorType>
score::Result<bool> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::IsRequestedBlockAtBufferQueueTail(
    // Suppress "AUTOSAR C++14 A8-4-10", The rule states: "A parameter shall be passed by reference if it can’t be
    // NULL". Function scope is pointer address manipulation passing by reference only delays obtaining
    // pointer into function body.
    // coverity[autosar_cpp14_a8_4_10_violation]
    const BufferBlock* meta) const
{
    // Validate list_entry_offset before using it to prevent out-of-bounds access

    if (!ValidateListEntryIndex(meta->list_entry_offset))
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kCorruptedBufferBlock);
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
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
score::Result<BufferBlock*> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::GetValidatedBlock(
    std::uint32_t offset) noexcept
{
    auto block_ptr = GetBufferPositionAt(offset);
    if ((!block_ptr.has_value()) || (block_ptr.value() == nullptr))
    {
        return MakeUnexpected<BufferBlock*>(block_ptr.error());  // Out-of-bounds offset detected
    }
    auto current_block_result = cast_to_buffer_block(block_ptr.value());
    if ((!current_block_result.has_value()) || (current_block_result.value() == nullptr))
    {
        return MakeUnexpected<BufferBlock*>(
            current_block_result.error());  // Alignment validation failed, stop iteration
    }
    return current_block_result;
}

template <template <class> class AtomicIndirectorType>
bool LocklessFlexibleCircularAllocator<AtomicIndirectorType>::ShouldResetBufferTail(
    std::uint32_t current_tail) const noexcept
{
    return ((current_tail == gap_address_.load()) && (current_tail != buffer_queue_head_.load())) ||
           (current_tail >= total_size_);
}

template <template <class> class AtomicIndirectorType>
ResultBlank LocklessFlexibleCircularAllocator<AtomicIndirectorType>::IterateBlocksToDeallocate()
{
    auto init_tail = buffer_queue_tail_.load();
    while (init_tail != buffer_queue_head_.load())
    {
        auto current_block_result = GetValidatedBlock(init_tail);
        if (!current_block_result.has_value())
        {
            return MakeUnexpected<score::Blank>(current_block_result.error());
        }
        BufferBlock* current_block = current_block_result.value();
        if (init_tail == 0U)
        {
            auto mark_list_entry_as_free_result = MarkListEntryAsFree(current_block);
            // Check if MarkListEntryAsFree encountered an error

            if (!mark_list_entry_as_free_result.has_value())
            {
                return MakeUnexpected<score::Blank>(mark_list_entry_as_free_result.error());
            }
        }
        // list_entry_offset is always set internally via controlled allocation paths, making invalid
        // indices impossible during normal operation.
        auto index = current_block->list_entry_offset;
        if (ValidateListEntryIndex(index))  // LCOV_EXCL_BR_LINE not testable
        // see comment above.
        {
            if (static_cast<std::uint8_t>(AtomicIndirectorType<ListEntry>::load(
                                              list_array_.at(static_cast<size_t>(current_block->list_entry_offset)),
                                              std::memory_order_seq_cst)
                                              .flags) == static_cast<std::uint8_t>(ListEntryFlag::kFree))
            {
                auto free_block_result = FreeBlock(*current_block);
                // Check if FreeBlock encountered an error
                if (!free_block_result.has_value())
                {
                    return MakeUnexpected<score::Blank>(free_block_result.error());
                }
                init_tail += current_block->block_length;

                if (ShouldResetBufferTail(init_tail))
                {
                    ResetBufferTailAndClearGapAddress();
                    init_tail = 0U;
                }
            }
            else
            {
                break;
            }
        }
    }
    return {};
}
template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
ResultBlank LocklessFlexibleCircularAllocator<AtomicIndirectorType>::Deallocate(void* const addr,
                                                                                const std::size_t) noexcept
{
    // Validate bounds
    if (!IsInBounds(addr, 0U))
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kInvalidDeallocationAddress);
    }

    // Validate alignment using std::align to avoid pointer-to-integer casts
    void* aligned_ptr = addr;
    std::size_t space = sizeof(std::max_align_t);
    if ((std::align(alignof(std::max_align_t), sizeof(std::max_align_t), aligned_ptr, space) == nullptr) ||
        (aligned_ptr != addr))
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kAddressNotAligned);  // Alignment validation failed
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

    auto mark_list_entry_as_free_result = MarkListEntryAsFree(meta);
    if (!mark_list_entry_as_free_result.has_value())
    {
        return MakeUnexpected<score::Blank>(mark_list_entry_as_free_result.error());
    }

    auto is_requested_block_at_buffer_queue_tail_result = IsRequestedBlockAtBufferQueueTail(meta);
    // list_entry_offset has been validated inside MarkListEntryAsFree() before, so validation failure here
    // indicates internal data corruption. This error path is not testable under normal operation as it
    // represents an impossible state when the allocator functions correctly.

    if (!is_requested_block_at_buffer_queue_tail_result.has_value())  // LCOV_EXCL_BR_LINE not testable
    {
        return MakeUnexpected<score::Blank>(is_requested_block_at_buffer_queue_tail_result.error());
    }
    if (is_requested_block_at_buffer_queue_tail_result.value())
    {
        auto iterate_blocks_to_deallocate_result = IterateBlocksToDeallocate();

        if (!iterate_blocks_to_deallocate_result.has_value())
        {
            return MakeUnexpected<score::Blank>(iterate_blocks_to_deallocate_result.error());
        }
    }
    // Note: If IsRequestedBlockAtBufferQueueTail() fails due to corruption, it returns false
    // and sets an error code. Since we don't call IterateBlocksToDeallocate() in that case,
    // the primary deallocation still succeeds, which is the desired behavior.
    dealloc_cntr_++;
    return {};
}

template <template <class> class AtomicIndirectorType>
ResultBlank LocklessFlexibleCircularAllocator<AtomicIndirectorType>::FreeBlock(BufferBlock& current_block)
{
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto old_buffer_queue_tail = buffer_queue_tail_.load();
        auto new_buffer_queue_tail = old_buffer_queue_tail + static_cast<uint32_t>(current_block.block_length);
        if (AtomicIndirectorType<decltype(buffer_queue_tail_.load())>::compare_exchange_strong(
                buffer_queue_tail_, old_buffer_queue_tail, new_buffer_queue_tail, std::memory_order_seq_cst) == true)
        {
            IncrementAvailableSize(static_cast<std::uint32_t>(current_block.block_length));
            break;
        }
    }

    // Validate list_entry_offset before using it to prevent out-of-bounds access

    if (!ValidateListEntryIndex(current_block.list_entry_offset))
    {
        return MakeUnexpected(
            LocklessFlexibleAllocatorErrorCode::kInvalidListEntryOffset);  // Early return to prevent crash
    }
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto list_entry_old = list_array_.at(static_cast<size_t>(current_block.list_entry_offset)).load();
        auto list_entry_new = list_entry_old;
        list_entry_new.flags = static_cast<std::uint8_t>(ListEntryFlag::kFree);
        list_entry_new.length = 0U;
        list_entry_new.offset = 0U;
        if (AtomicIndirectorType<ListEntry>::compare_exchange_strong(
                list_array_.at(static_cast<size_t>(current_block.list_entry_offset)),
                list_entry_old,
                list_entry_new,
                std::memory_order_seq_cst) == true)
        {
            break;
        }
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

    return {};
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
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// This function is intentionally declared noexcept; failures are reported via the Result return value instead of
// exceptions. If an unexpected exception still escapes, calling std::terminate() is the intended fail-fast behavior
// per the safety concept.
// coverity[autosar_cpp14_a15_5_3_violation]
bool LocklessFlexibleCircularAllocator<AtomicIndirectorType>::IsInBounds(const void* const address,
                                                                         const std::size_t size) const noexcept
{
    if (GetSize() < size)
    {
        return false;
    }
    // NOLINTBEGIN(score-no-pointer-comparison): Needed pointer comparisons for validating memory ranges safely
    // For size==0, the largest valid address is the last byte in the buffer.
    // Using offset==GetSize() would produce a one-past-end pointer, which must not be considered in-bounds.
    auto end_ptr = (size == 0U) ? GetBufferPositionAt(GetSize() - 1U) : GetBufferPositionAt(GetSize() - size);

    if ((!end_ptr.has_value()) || (end_ptr == nullptr))
    {
        return false;  // Invalid offset, out of bounds
    }
    if ((address >= GetBaseAddress()) && (address <= static_cast<void*>(end_ptr.value())))
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
score::Result<std::uint32_t> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::AcquireWrapAroundBufferHead(
    std::uint32_t aligned_size) noexcept
{
    auto new_buffer_queue_head = 0U;
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto old_buffer_queue_head = buffer_queue_head_.load();
        new_buffer_queue_head = static_cast<uint32_t>(aligned_size);

        if (AtomicIndirectorType<decltype(buffer_queue_head_.load())>::compare_exchange_strong(
                buffer_queue_head_, old_buffer_queue_head, new_buffer_queue_head, std::memory_order_seq_cst) == true)
        {
            return new_buffer_queue_head;
        }
    }
    return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kViolatedMaximumRetries);
}
template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
score::Result<uint8_t*> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::SetupBlockMetadata(
    std::uint32_t offset,
    std::uint32_t aligned_size,
    std::uint32_t list_entry_element_index) noexcept
{
    auto block_ptr = GetBufferPositionAt(offset);
    if ((!block_ptr.has_value()) || (block_ptr.value() == nullptr))
    {
        return MakeUnexpected<uint8_t*>(block_ptr.error());  // Out-of-bounds offset detected
    }
    auto block_meta_data_result = cast_to_buffer_block(block_ptr.value());

    if ((!block_meta_data_result.has_value()) || (block_meta_data_result.value() == nullptr))
    {
        return MakeUnexpected<uint8_t*>(
            block_meta_data_result.error());  // Alignment validation failed in cast_to_buffer_block
    }
    auto block_meta_data = block_meta_data_result.value();
    block_meta_data->list_entry_offset = list_entry_element_index;
    block_meta_data->block_length = static_cast<std::uint32_t>(aligned_size);
    score::Result<uint8_t*> allocated_address =
        GetBufferPositionAt(static_cast<std::size_t>(offset) + sizeof(BufferBlock));
    if ((!allocated_address.has_value()) || (allocated_address.value() == nullptr))
    {
        return MakeUnexpected<uint8_t*>(allocated_address.error());  // Out-of-bounds offset detected
    }

    if (aligned_size > kMaxPossibleAllocationSize)
    {
        // Return early if aligned_size exceeds the maximum value representable by 31 bits.
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kRequestedSizeIsTooLarge);
    }

    // Validate list_entry_element_index before using it to prevent out-of-bounds access
    if (!ValidateListEntryIndex(list_entry_element_index))
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kInvalidListEntryOffset);
    }
    return allocated_address;
}

template <template <class> class AtomicIndirectorType>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
bool LocklessFlexibleCircularAllocator<AtomicIndirectorType>::UpdateListEntryForAllocation(
    std::size_t list_entry_index,
    std::uint32_t aligned_size,
    std::uint32_t offset) noexcept
{
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto list_entry_old = list_array_.at(list_entry_index).load();
        auto list_entry_new = list_entry_old;
        list_entry_new.flags = static_cast<std::uint8_t>(ListEntryFlag::kInUse);
        list_entry_new.length = (aligned_size & kMaxPossibleAllocationSize);
        list_entry_new.offset = offset;
        if (AtomicIndirectorType<ListEntry>::compare_exchange_strong(
                list_array_.at(list_entry_index), list_entry_old, list_entry_new, std::memory_order_seq_cst) == true)
        {
            return true;
        }
    }
    return false;  // LCOV_EXCL_LINE : hard to reach
}

template <template <class> class AtomicIndirectorType>
score::Result<std::uint8_t*> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::AllocateWithWrapAround(
    std::uint32_t aligned_size,
    std::uint32_t list_entry_element_index)
{
    auto head_result = AcquireWrapAroundBufferHead(aligned_size);
    if (!head_result.has_value())
    {
        return MakeUnexpected<std::uint8_t*>(head_result.error());
    }
    const std::uint32_t new_buffer_queue_head = head_result.value();
    // Underflow check: ensures new_buffer_queue_head >= aligned_size before subtraction.
    // This condition is always false in practice since AcquireWrapAroundBufferHead sets
    // new_buffer_queue_head = aligned_size, but is required to justify the coverity suppression below.
    if (new_buffer_queue_head < aligned_size)  // LCOV_EXCL_BR_LINE: Defensive check for coverity
    {
        return MakeUnexpected(                                       // LCOV_EXCL_LINE
            LocklessFlexibleAllocatorErrorCode::kOverFlowOccurred);  // LCOV_EXCL_LINE
    }
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
    // not lead to data loss.".
    // Rationale: Underflow is prevented by the check above ensuring new_buffer_queue_head >= aligned_size.
    // coverity[autosar_cpp14_a4_7_1_violation]
    const std::uint32_t block_offset = new_buffer_queue_head - aligned_size;
    auto allocated_address = SetupBlockMetadata(block_offset, aligned_size, list_entry_element_index);
    if (!allocated_address.has_value())
    {
        return MakeUnexpected<uint8_t*>(allocated_address.error());
    }

    if (!UpdateListEntryForAllocation(static_cast<std::size_t>(list_entry_element_index),
                                      // coverity[autosar_cpp14_a4_7_1_violation]
                                      aligned_size,
                                      new_buffer_queue_head))
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kViolatedMaximumRetries);
    }

    return allocated_address;
}

template <template <class> class AtomicIndirectorType>
score::Result<std::uint32_t> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::AcquireNoWrapAroundBufferHead(
    std::uint32_t aligned_size) noexcept
{
    auto offset = 0U;
    for (uint8_t retries = 0U; retries < kMaxRetries; retries++)
    {
        auto old_buffer_queue_head = buffer_queue_head_.load();
        // Safety check: never advance head beyond the buffer end.
        // Under allocation concurrency, the earlier wrap-around decision can become stale.
        // Prevent unsigned underflow by validating the relationship before subtraction.
        // All arithmetic stays within std::uint32_t.
        if (old_buffer_queue_head > total_size_)
        {
            return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kInvalidOffsetValue);
        }
        const std::uint32_t remaining = total_size_ - old_buffer_queue_head;
        if (aligned_size > remaining)
        {
            return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kNotEnoughMemory);
        }
        auto new_buffer_queue_head = old_buffer_queue_head + static_cast<std::uint32_t>(aligned_size);
        if (AtomicIndirectorType<decltype(buffer_queue_head_.load())>::compare_exchange_strong(
                buffer_queue_head_, old_buffer_queue_head, new_buffer_queue_head, std::memory_order_seq_cst) == true)
        {
            if (new_buffer_queue_head <
                static_cast<unsigned int>(
                    aligned_size))  // LCOV_EXCL_BR_LINE: Defensive check, prior bounds validation prevents overflow
            {
                return MakeUnexpected(                                       // LCOV_EXCL_LINE
                    LocklessFlexibleAllocatorErrorCode::kOverFlowOccurred);  // LCOV_EXCL_LINE
            }
            offset = new_buffer_queue_head - aligned_size;
            break;
        }
    }
    return offset;
}

template <template <class> class AtomicIndirectorType>
score::Result<uint8_t*> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::AllocateWithNoWrapAround(
    std::uint32_t aligned_size,
    std::uint32_t list_entry_element_index)
{
    auto offset_result = AcquireNoWrapAroundBufferHead(aligned_size);
    if (!offset_result.has_value())
    {
        return MakeUnexpected<uint8_t*>(offset_result.error());
    }
    const std::uint32_t offset = offset_result.value();

    auto allocated_address = SetupBlockMetadata(offset, aligned_size, list_entry_element_index);
    if (!allocated_address.has_value())
    {
        return MakeUnexpected<uint8_t*>(allocated_address.error());
    }

    const std::uint32_t list_entry_offset = aligned_size + offset;
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
    // not lead to data loss.".
    // Rationale: Narrowing conversion validity is ensured by SetupBlockMetadata()
    if (!UpdateListEntryForAllocation(static_cast<std::size_t>(list_entry_element_index),
                                      // coverity[autosar_cpp14_a4_7_1_violation]
                                      aligned_size,
                                      list_entry_offset))
    {
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kViolatedMaximumRetries);
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
template <typename OffsetT>
score::Result<uint8_t*> LocklessFlexibleCircularAllocator<AtomicIndirectorType>::GetBufferPositionAt(
    OffsetT offset) const noexcept
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
    // coverity[autosar_cpp14_a7_1_8_violation: FALSE]
    if constexpr (std::is_signed_v<OffsetT>)
    {
        if (offset < 0)
        {
            return MakeUnexpected(
                LocklessFlexibleAllocatorErrorCode::kInvalidOffsetValue);  // Negative offset is out-of-bounds
        }
    }
    if (static_cast<std::size_t>(offset) >= static_cast<std::size_t>(total_size_))
    {
        // Out-of-bounds offset

        // clang-format off
        return MakeUnexpected(LocklessFlexibleAllocatorErrorCode::kInvalidOffsetValue);
        // clang-format on
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

template <template <class> class AtomicIndirectorType>
void LocklessFlexibleCircularAllocator<AtomicIndirectorType>::IncrementAvailableSize(std::uint32_t delta) noexcept
{
    std::ignore = available_size_.fetch_add(delta, std::memory_order_seq_cst);
}

template class LocklessFlexibleCircularAllocator<score::memory::shared::AtomicIndirectorReal>;
template class LocklessFlexibleCircularAllocator<score::memory::shared::AtomicIndirectorMock>;

}  // namespace tracing
}  // namespace analysis
}  // namespace score
