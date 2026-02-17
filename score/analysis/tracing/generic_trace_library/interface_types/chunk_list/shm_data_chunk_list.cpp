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
#include "score/analysis/tracing/generic_trace_library/interface_types/chunk_list/shm_data_chunk_list.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location_helpers.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/error_code/error_code.h"
#include <memory>

namespace score
{
namespace analysis
{
namespace tracing
{

ShmDataChunkList::ShmDataChunkList() : ShmDataChunkList(SharedMemoryChunk{}, false) {}

ShmDataChunkList::ShmDataChunkList(const SharedMemoryChunk& root) : ShmDataChunkList(root, true) {}

ShmDataChunkList::ShmDataChunkList(const SharedMemoryChunk& root, bool has_root) : list_{}, number_of_chunks_{0U}
{
    if (has_root)
    {
        list_[0U] = root;
        number_of_chunks_ = 1U;
    }
}

void ShmDataChunkList::AppendFront(const SharedMemoryChunk& chunk)
{
    if (number_of_chunks_ < kMaxChunksPerOneTraceRequest)
    {
        number_of_chunks_++;
    }

    for (std::uint8_t i = kMaxChunksPerOneTraceRequest - 1U; i > 0U; i--)
    {
        // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
        // not be implicitly converted to a different underlying type"
        // False positive, right hand value is the same type.
        // coverity[autosar_cpp14_m5_0_3_violation]
        // coverity[autosar_cpp14_a4_7_1_violation]: It's checked in the for loop condition.
        list_.at(i) = list_.at(i - 1U);
    }
    // coverity[autosar_cpp14_m5_0_3_violation]
    list_.at(0U) = chunk;
}

void ShmDataChunkList::Append(const SharedMemoryChunk& next)
{
    if (number_of_chunks_ < kMaxChunksPerOneTraceRequest)
    {
        // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
        // not be implicitly converted to a different underlying type"
        // False positive, right hand value is the same type.
        // coverity[autosar_cpp14_m5_0_3_violation]
        list_.at(number_of_chunks_) = next;
        number_of_chunks_++;
    }
}

std::size_t ShmDataChunkList::Size() const
{
    return static_cast<std::size_t>(number_of_chunks_);
}

void ShmDataChunkList::Clear()
{
    list_.fill(SharedMemoryChunk{{0, 0U}, 0U});
    number_of_chunks_ = 0U;
}

const std::array<SharedMemoryChunk, kMaxChunksPerOneTraceRequest>& ShmDataChunkList::GetList() const
{
    return list_;
}

std::array<SharedMemoryChunk, kMaxChunksPerOneTraceRequest>& ShmDataChunkList::GetList()
{
    // Suppress "AUTOSAR C++14 A9-3-1" rule finding: "Member functions shall not return non-const “raw” pointers or
    // references to private or protected data owned by the class."
    // Justification: Intended by implementation, caller e.i. ShmDataChunkList requires a reference.
    // coverity[autosar_cpp14_a9_3_1_violation]
    return list_;
}

// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function
// shall not be called implicitly."
// Using `at()` inside, still protected with boundaries check
// coverity[autosar_cpp14_a15_5_3_violation]
bool operator==(const ShmDataChunkList& lhs, const ShmDataChunkList& rhs) noexcept
{
    if (lhs.number_of_chunks_ != rhs.number_of_chunks_)
    {
        return false;
    }

    for (std::uint8_t i = 0U; i < kMaxChunksPerOneTraceRequest; i++)
    {
        // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
        // not be implicitly converted to a different underlying type"
        // False positive, right hand value is the same type.
        // coverity[autosar_cpp14_m5_0_3_violation]
        if (!(lhs.list_.at(i) == rhs.list_.at(i)))
        {
            return false;
        }
    }
    return true;
}
score::Result<SharedMemoryLocation> ShmDataChunkList::SaveToSharedMemory(
    ResourcePointer memory_resource,
    ShmObjectHandle handle,
    std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator)
{
    if (((nullptr == memory_resource) || (nullptr == memory_resource->getBaseAddress())) || (-1 == handle))
    {
        return score::MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }

    if (flexible_allocator->GetAvailableMemory() <
        // No possiblity here for data loss
        // coverity[autosar_cpp14_a4_7_1_violation]
        (CalculateNeededAllignmentSpaceForElementType<ShmChunkVector>() + EstimateChunkListAllocationSize(list_)))
    {
        return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
    }

    auto vector_shm_raw_pointer_result =
        flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t));
    if ((!vector_shm_raw_pointer_result.has_value()) || (nullptr == vector_shm_raw_pointer_result.value()))
    {
        return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
    }

    // Usage of placement new is intended here and safe as we explicitly construct the shared list
    // in the pre-allocated memory provided by the flexible allocator
    // NOLINTBEGIN(score-no-dynamic-raw-memory) Usage of placement new is intended here to allocate the shared list
    //  coverity[autosar_cpp14_a18_5_10_violation]
    auto* const vector = new (vector_shm_raw_pointer_result.value()) ShmChunkVector(flexible_allocator);
    // NOLINTEND(score-no-dynamic-raw-memory) Usage of placement new is intended here to allocate the shared list

    const std::size_t offset = GetOffsetFromPointer(vector, memory_resource).value();
    SharedMemoryLocation result{handle, offset};
    // Tooling issue: as reported from quality team that cases where branch coverage is 100% but decision couldn't be
    // analyzed are accepted as deviations
    for (std::uint8_t i = 0U; i < this->Size(); i++)  // LCOV_EXCL_BR_LINE not testable see comment above.
    {
        // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
        // not be implicitly converted to a different underlying type"
        // False positive, right hand value is the same type.
        // coverity[autosar_cpp14_m5_0_3_violation]
        auto emplace_result = vector->emplace_back(list_.at(i));
        if (!emplace_result.has_value())
        {
            vector->clear();
            score::cpp::ignore = flexible_allocator->Deallocate(vector_shm_raw_pointer_result.value(), sizeof(ShmChunkVector));
            return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
        }
    }
    return result;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
