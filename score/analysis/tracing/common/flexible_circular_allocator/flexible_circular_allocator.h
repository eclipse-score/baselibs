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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_H
#define SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_H

#include "score/analysis/tracing/common/flexible_circular_allocator/error_codes/flexible_circular_allocator/error_code.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_interface.h"
#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/result/result.h"

#include <cstddef>
#include <cstdint>
#include <list>
#include <mutex>
#include <unordered_map>

namespace score
{
namespace analysis
{
namespace tracing
{

class FlexibleCircularAllocator : public IFlexibleCircularAllocator
{
  public:
    FlexibleCircularAllocator(void* base_address, std::size_t size);
    // This is intented, we don't enforce users to specify align unless needed
    // NOLINTNEXTLINE(google-default-arguments) see comment above
    score::Result<void*> Allocate(const std::size_t size, const std::size_t alignment = alignment::kBlockSize) override;
    ResultBlank Deallocate(void* const pointer, const std::size_t) override;

    std::size_t GetAvailableMemory() noexcept override;

    void GetTmdMemUsage(TmdStatistics&) noexcept override;

    void* GetBaseAddress() const noexcept override;

    std::size_t GetSize() const noexcept override;
    bool IsInBounds(const void* const address, const std::size_t size) const noexcept override;

  private:
    // No harm to define struct in that format
    // coverity[autosar_cpp14_a11_0_2_violation]
    struct MemoryBlock  // NOLINT(score-struct-usage-compliance): Intended struct semantic
    {
        // It is ok to declare the adddress to be void pointer and cast it later
        //  coverity[autosar_cpp14_a9_6_1_violation]
        void* address;
        std::size_t size;
        // clang-format off
        // No harm to define comparison operator here as member function
        //  coverity[autosar_cpp14_a13_5_5_violation]
        bool operator<(const MemoryBlock& rhs) const
        {
            return address < rhs.address;  // NOLINT(score-no-pointer-comparison): Both pointers points to the same type of pointer (so no UB).
        }
        // No harm to define comparison operator here as member function
        //  coverity[autosar_cpp14_a13_5_5_violation]
        bool operator<(const void* rhs) const
        {
            return address < rhs;  // NOLINT(score-no-pointer-comparison): Both pointers points to the same type of pointer (so no UB).
        }
        // clang-format on
    };
    // to sort free blocks and merge adjacent blocks;
    void CollectGarbage(void* pointer);
    bool MergeTwoBlocksIfApplicable(MemoryBlock& a, MemoryBlock& b);
    score::Result<std::list<MemoryBlock>::iterator> GetNextAddressToUse(std::size_t aligned_size);
    void SplitMemoryBlockIfNeeded(std::list<FlexibleCircularAllocator::MemoryBlock>::iterator next_address_to_use,
                                  std::size_t aligned_size);
    void* base_address_;
    std::size_t total_size_;
    void* latest_allocated_address_;

    std::list<MemoryBlock> free_blocks_;
    std::unordered_map<void*, MemoryBlock> allocated_blocks_;
    std::mutex mutex_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_H
