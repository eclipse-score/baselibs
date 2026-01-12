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
#include "score/analysis/tracing/generic_trace_library/interface_types/chunk_list/local_data_chunk_list.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location_helpers.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/chunk_list/shm_data_chunk_list.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/error_code/error_code.h"
#include <cstring>

namespace score
{
namespace analysis
{
namespace tracing
{

LocalDataChunkList::LocalDataChunkList() : LocalDataChunkList(LocalDataChunk{}, false) {}

LocalDataChunkList::LocalDataChunkList(const LocalDataChunk& root) : LocalDataChunkList(root, true) {}

LocalDataChunkList::LocalDataChunkList(const LocalDataChunk& root, bool has_root) : list_{}, number_of_chunks_{0U}
{
    if (has_root)
    {
        list_[0U] = root;
        number_of_chunks_ = 1U;
    }
}

void LocalDataChunkList::AppendFront(const LocalDataChunk& chunk)
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

void LocalDataChunkList::Append(const LocalDataChunk& next)
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

std::size_t LocalDataChunkList::Size() const
{
    return static_cast<std::size_t>(number_of_chunks_);
}

void LocalDataChunkList::Clear()
{
    list_.fill(LocalDataChunk{nullptr, 0U});
    number_of_chunks_ = 0U;
}

const std::array<LocalDataChunk, kMaxChunksPerOneTraceRequest>& LocalDataChunkList::GetList() const
{
    return list_;
}

std::array<LocalDataChunk, kMaxChunksPerOneTraceRequest>& LocalDataChunkList::GetList()
{
    // Suppress "AUTOSAR C++14 A9-3-1" rule finding: "Member functions shall not return non-const “raw” pointers or
    // references to private or protected data owned by the class."
    // Justification: Intended by implementation, caller e.i. LocalDataChunkList requires a reference.
    // coverity[autosar_cpp14_a9_3_1_violation]
    return list_;
}

bool operator==(const LocalDataChunk& lhs, const LocalDataChunk& rhs) noexcept
{
    return (lhs.start == rhs.start) && (lhs.size == rhs.size);
}

// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function
// shall not be called implicitly."
// Using `at()` inside, still protected with boundaries check
// coverity[autosar_cpp14_a15_5_3_violation]
bool operator==(const LocalDataChunkList& lhs, const LocalDataChunkList& rhs) noexcept
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

score::Result<SharedMemoryLocation> LocalDataChunkList::SaveToSharedMemory(
    ResourcePointer memory_resource,
    ShmObjectHandle handle,
    std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator)
{
    std::size_t alignment_space = CalculateNeededAllignmentSpaceForElementType<ShmChunkVector>();
    std::size_t chunk_list_size = EstimateChunkListAllocationSize(list_);
    // No possibility here for data loss
    // coverity[autosar_cpp14_a4_7_1_violation]
    std::size_t required_memory_size = alignment_space + chunk_list_size;
    if (!IsValidResource(memory_resource, handle))
    {
        return score::MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }
    if (!HasEnoughMemory(flexible_allocator, required_memory_size))
    {
        return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
    }
    void* const vector_shm_raw_pointer = AllocateVector(flexible_allocator);

    if (nullptr == vector_shm_raw_pointer)
    {
        return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
    }
    auto* vector = ConstructShmChunkVector(vector_shm_raw_pointer, flexible_allocator);
    return FillVectorInSharedMemory(vector, memory_resource, handle, flexible_allocator, vector_shm_raw_pointer);
}

score::Result<SharedMemoryLocation> LocalDataChunkList::FillVectorInSharedMemory(
    // No harm to pass vector by addresss here
    // coverity[autosar_cpp14_a8_4_10_violation]
    ShmChunkVector* vector,
    ResourcePointer memory_resource,
    ShmObjectHandle handle,
    const std::shared_ptr<IFlexibleCircularAllocator>& flexible_allocator,
    void* vector_shm_raw_pointer)
{
    // coverity[autosar_cpp14_m8_5_2_violation] kept for the sake of zero-initialization
    std::array<std::pair<void*, std::size_t>, kMaxChunksPerOneTraceRequest> allocated_data{};
    std::uint8_t index = 0U;
    // Tooling issue: as reported from quality team that cases where branch coverage is 100% but decision couldn't be
    // analyzed are accepted as deviations
    for (std::uint8_t i = 0U; i < this->Size(); i++)  // LCOV_EXCL_BR_LINE not testable see comment above.
    {
        // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
        // not be implicitly converted to a different underlying type"
        // False positive, right hand value is the same type.
        // coverity[autosar_cpp14_m5_0_3_violation]
        auto& element = list_.at(i);

        if (!IsValidElement(element))
        {
            continue;
        }
        void* shm_pointer = flexible_allocator->Allocate(element.size);
        if (shm_pointer == nullptr)
        {
            CleanupAllocatedData(allocated_data, flexible_allocator, vector, vector_shm_raw_pointer);
            return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
        }
        // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
        // not be implicitly converted to a different underlying type"
        // False positive, right hand value is the same type.
        // coverity[autosar_cpp14_m5_0_3_violation]
        allocated_data.at(index) = std::pair<void*, std::size_t>{shm_pointer, element.size};

        if (index < kMaxChunksPerOneTraceRequest - 1U)
        {
            index++;
        }
        else
        {
            index = 0U;
        }

        CopyDataToSharedMemory(element, shm_pointer);
        auto result = vector->push_back(
            {SharedMemoryLocation{handle, GetOffsetFromPointer(shm_pointer, memory_resource).value()}, element.size});
        if (!result.has_value())
        {
            CleanupAllocatedData(allocated_data, flexible_allocator, vector, vector_shm_raw_pointer);
            return score::MakeUnexpected(ErrorCode::kNotEnoughMemoryRecoverable);
        }
    }
    return CreateSharedMemoryLocation(vector, memory_resource, handle);
}

bool LocalDataChunkList::IsValidResource(ResourcePointer memory_resource, ShmObjectHandle handle) const
{
    return ((memory_resource != nullptr) && (memory_resource->getBaseAddress() != nullptr)) && (handle != -1);
}

bool LocalDataChunkList::HasEnoughMemory(std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator,
                                         std::size_t required_memory_size) const
{
    if (!flexible_allocator)
    {
        return false;
    }
    return flexible_allocator->GetAvailableMemory() >= required_memory_size;
}
void* LocalDataChunkList::AllocateVector(std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator) const
{
    return flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t));
}
ShmChunkVector* LocalDataChunkList::ConstructShmChunkVector(
    void* vector_shm_raw_pointer,
    std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator) const
{
    // Usage of placement new is intended here and safe as we explicitly construct the shared list
    // in the pre-allocated memory provided by the flexible allocator
    // NOLINTBEGIN(score-no-dynamic-raw-memory) Usage of placement new is intended here to allocate the shared list
    //  coverity[autosar_cpp14_a18_5_10_violation]
    return new (vector_shm_raw_pointer) ShmChunkVector(flexible_allocator);
    // NOLINTEND(score-no-dynamic-raw-memory) Usage of placement new is intended here to allocate the shared list
}
bool LocalDataChunkList::IsValidElement(const LocalDataChunk& element) const
{
    return (element.size != 0U) && (element.start != nullptr);
}

// clang-format off
void LocalDataChunkList::CopyDataToSharedMemory(const LocalDataChunk& element, void* shm_pointer) const
{
    // Needed in order to perform offset calculations (offset in the offset_ptr) for rule m5-2-8,m5-0-15
    // coverity[autosar_cpp14_m5_2_8_violation]
    // coverity[autosar_cpp14_m5_0_15_violation]
    const auto *const element_start_end = static_cast<const std::uint8_t*>(element.start) + element.size;// NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) see comment above
    // Needed in order to perform offset calculations (offset in the offset_ptr)
    // coverity[autosar_cpp14_m5_2_8_violation]
    std::ignore = std::copy(static_cast<const std::uint8_t*>(element.start), element_start_end, static_cast<std::uint8_t*>(shm_pointer));
}
// clang-format on
// Const Reference passing for shared pointer.
// coverity[autosar_cpp14_a8_4_13_violation] passing shared pointer is intended.
void LocalDataChunkList::CleanupAllocatedData(
    std::array<std::pair<void*, std::size_t>, kMaxChunksPerOneTraceRequest>& allocated_data,
    const std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator,
    // No harm to pass vector by addresss here
    // coverity[autosar_cpp14_a8_4_10_violation]
    ShmChunkVector* vector,
    void* vector_shm_raw_pointer) const
{
    for (const auto& data : allocated_data)
    {
        std::ignore = flexible_allocator->Deallocate(data.first, sizeof(data.first));
    }
    vector->clear();
    std::ignore = flexible_allocator->Deallocate(vector_shm_raw_pointer, sizeof(vector_shm_raw_pointer));
}

score::Result<SharedMemoryLocation> LocalDataChunkList::CreateSharedMemoryLocation(ShmChunkVector* vector,
                                                                                 ResourcePointer memory_resource,
                                                                                 ShmObjectHandle handle) const
{
    std::size_t vector_offset = GetOffsetFromPointer(vector, memory_resource).value();
    return SharedMemoryLocation{handle, vector_offset};
}
}  // namespace tracing
}  // namespace analysis
}  // namespace score
