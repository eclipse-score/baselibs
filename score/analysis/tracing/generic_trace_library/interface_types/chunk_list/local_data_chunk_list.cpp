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

}  // namespace tracing
}  // namespace analysis
}  // namespace score
