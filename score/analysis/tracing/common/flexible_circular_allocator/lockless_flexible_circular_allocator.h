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
#include "score/analysis/tracing/common/flexible_circular_allocator/error_code.h"
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
    void* Allocate(const std::size_t size, const std::size_t alignment_size) noexcept override;
    bool Deallocate(void* const addr, const std::size_t) noexcept override;
    std::size_t GetAvailableMemory() noexcept override;
    void GetTmdMemUsage(TmdStatistics& tmd_stats) noexcept override;
    void* GetBaseAddress() const noexcept override;
    std::size_t GetSize() const noexcept override;
    bool IsInBounds(const void* const address, const std::size_t size) const noexcept override;
    score::result::Error GetLastError() const noexcept;
    void ClearError() noexcept;

  private:
    std::uint32_t BufferQueueSize();
    void FreeBlock(BufferBlock& current_block);
    std::uint32_t GetListQueueNextHead();
    uint8_t* AllocateWithWrapAround(std::uint32_t aligned_size, std::uint32_t list_entry_element_index);
    uint8_t* AllocateWithNoWrapAround(std::uint32_t aligned_size, std::uint32_t list_entry_element_index);
    bool ValidateListEntryIndex(const std::uint32_t& index) const;
    void ResetBufferQueuTail();
    void MarkListEntryAsFree(const BufferBlock* meta);
    bool IsRequestedBlockAtBufferQueueTail(const BufferBlock* meta) const;
    void IterateBlocksToDeallocate();
    void SetError(FlexibleAllocatorErrorCode error_code) const noexcept;
    template <typename OffsetT>
    uint8_t* GetBufferPositionAt(OffsetT offset) const noexcept;

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
    std::atomic<bool> tmd_stats_enabled_;
    mutable std::atomic<score::result::ErrorCode> last_error_code_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_LOCKLESS_FLEXIBLE_CIRCULAR_ALLOCATOR_H
