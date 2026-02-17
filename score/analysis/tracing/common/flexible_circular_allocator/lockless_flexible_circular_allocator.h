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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_LOCKLESS_FLEXIBLE_CIRCULAR_ALLOCATOR_H
#define SCORE_ANALYSIS_TRACING_COMMON_LOCKLESS_FLEXIBLE_CIRCULAR_ALLOCATOR_H
#include "score/analysis/tracing/common/flexible_circular_allocator/error_codes/lockless_flexible_circular_allocator/error_code.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_interface.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator_types.h"
#include "score/memory/shared/atomic_indirector.h"
#include "score/memory/shared/managed_memory_resource.h"
#include <array>
#include <atomic>
#include <memory>

namespace score
{
namespace analysis
{
namespace tracing
{

template <template <class> class AtomicIndirectorType = score::memory::shared::AtomicIndirectorReal>
class LocklessFlexibleCircularAllocator : public IFlexibleCircularAllocator
{
  public:
    LocklessFlexibleCircularAllocator(void* base_address, std::size_t size);
    score::Result<void*> Allocate(const std::size_t size, const std::size_t alignment_size) noexcept override;
    ResultBlank Deallocate(void* const addr, const std::size_t) noexcept override;
    std::size_t GetAvailableMemory() noexcept override;
    void GetTmdMemUsage(TmdStatistics& tmd_stats) noexcept override;
    void* GetBaseAddress() const noexcept override;
    std::size_t GetSize() const noexcept override;
    bool IsInBounds(const void* const address, const std::size_t size) const noexcept override;

  private:
// Suppress "autosar_cpp14_a16_0_1_violation" rule finding. This rule states: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef, (2)
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include."
// No harm in using this preprocessor.
// Test-only access: internal helper branches are unreachable via the public API because
// bounds/alignment checks prevent corrupted pointers and metadata tampering.
// and this friend keyword will be removed after working on this ticket broken_link_j/Ticket-228578.
// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef UNIT_TEST_BUILD
    friend class LocklessFlexibleCircularAllocatorTestAccessor;
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
    std::uint32_t BufferQueueSize();
    ResultBlank FreeBlock(BufferBlock& current_block);
    std::uint32_t GetListQueueNextHead();
    score::Result<uint8_t*> AllocateWithWrapAround(std::uint32_t aligned_size, std::uint32_t list_entry_element_index);
    score::Result<uint8_t*> AllocateWithNoWrapAround(std::uint32_t aligned_size, std::uint32_t list_entry_element_index);
    bool ValidateListEntryIndex(const std::uint32_t& index) const;
    void ResetBufferQueuTail();
    ResultBlank MarkListEntryAsFree(const BufferBlock* meta);
    score::Result<bool> IsRequestedBlockAtBufferQueueTail(const BufferBlock* meta) const;
    ResultBlank IterateBlocksToDeallocate();
    template <typename OffsetT>
    score::Result<uint8_t*> GetBufferPositionAt(OffsetT offset) const noexcept;
    void IncrementAvailableSize(std::uint32_t delta) noexcept;

    void* base_address_;
    std::uint32_t total_size_;
    std::atomic<std::uint32_t> gap_address_;
    std::atomic<std::uint32_t> buffer_queue_head_;
    std::atomic<std::uint32_t> buffer_queue_tail_;
    std::array<std::atomic<ListEntry>, kListEntryArraySize> list_array_;
    std::atomic<std::uint32_t> list_queue_head_;
    std::atomic<std::uint32_t> list_queue_tail_;
    std::atomic<std::uint32_t> available_size_;
    std::atomic<bool> wrap_around_;
    std::atomic<std::uint64_t> cumulative_usage_;
    std::atomic<std::uint32_t> lowest_size_;
    std::atomic<std::uint32_t> alloc_cntr_;
    std::atomic<std::uint32_t> dealloc_cntr_;
    std::atomic<std::uint64_t> allocate_retry_cntr_;
    std::atomic<std::uint64_t> allocate_call_cntr_;
    std::atomic<bool> tmd_stats_enabled_;
    mutable std::atomic<score::result::ErrorCode> last_error_code_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_LOCKLESS_FLEXIBLE_CIRCULAR_ALLOCATOR_H
