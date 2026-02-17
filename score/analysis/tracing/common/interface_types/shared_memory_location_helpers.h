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
#ifndef AAS_ANALYSIS_TRACING_COMMON_SHARED_MEMORY_LOCATION_HELPERS_H
#define AAS_ANALYSIS_TRACING_COMMON_SHARED_MEMORY_LOCATION_HELPERS_H

#include "score/analysis/tracing/generic_trace_library/interface_types/error_code/error_code.h"
#include "shared_memory_chunk.h"
#include "shared_memory_location.h"
#include "types.h"

namespace score
{
namespace analysis
{
namespace tracing
{
// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used"
// False positive, variable is used.
// Suppress "AUTOSAR C++14 A2-10-4". The rule states: "The identifier name of a
// non-member object with static storage duration or static function shall not be
// reused within a namespace"
// This is a single declaration using constexpr.
// The identifier is not reused; it's declared once and referenced elsewhere.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
static constexpr std::size_t kStlContainerStorageNeeds = 1024U;
// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used"
// False positive, variable is used.
// Suppress "AUTOSAR C++14 A2-10-4". The rule states: "The identifier name of a
// non-member object with static storage duration or static function shall not be
// reused within a namespace"
// This is a single declaration using constexpr.
// The identifier is not reused; it's declared once and referenced elsewhere.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
static constexpr std::size_t kStlContainerElementStorageNeeds = sizeof(void*);

template <typename T>
auto GetPointerFromLocation(SharedMemoryLocation memory_location, ResourcePointer resource_ptr) -> T*
{
    if ((nullptr == resource_ptr) || (nullptr == (resource_ptr->getBaseAddress())))
    {
        return nullptr;
    }
    // clang-format off
    // reinterpret_cast needed to calculate address in desired type T.
    // No harm when do arithmetic operations, both pointers points to the same type of pointer (so no UB).
    // No harm from using doing arithematic operations(m5_0_15),No harm from using static_cast(a5_2_4),No harm from using reinterpret_cast(m5_2_8)
    // coverity[autosar_cpp14_m5_0_15_violation]
    // coverity[autosar_cpp14_m5_0_17_violation]
    // coverity[autosar_cpp14_a5_2_4_violation]
    // coverity[autosar_cpp14_m5_2_8_violation]
    return reinterpret_cast<T*>(static_cast<std::uint8_t*>(resource_ptr->getBaseAddress()) + memory_location.offset_);// NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, cppcoreguidelines-pro-bounds-pointer-arithmetic): Tolerated see above
    // clang-format on
}

template <typename T>
// Rationale: False positive method doesn't hide any identifiers
// coverity[autosar_cpp14_a2_10_1_violation] False
score::Result<std::size_t> GetOffsetFromPointer(T* pointer, ResourcePointer memory_resource)
{
    // The next line's decision coverage is being misevaluated and for that it will be skipped.
    if (((nullptr == pointer) || (nullptr == memory_resource)) ||
        (nullptr == (memory_resource->getBaseAddress())))  // GCOVR_EXCL_LINE
    {
        return score::MakeUnexpected(ErrorCode::kInvalidArgumentFatal);
    }
    // clang-format off
 // No harm from using doing arithematic operations(m5_0_15),No harm from using static_cast(a5_2_4),reinterpret_cast needed to calculate offset(m5_2_8),An explicit integral conversion is needed(m5_0_9)
    // coverity[autosar_cpp14_m5_0_15_violation]
    // coverity[autosar_cpp14_m5_0_17_violation]
    // coverity[autosar_cpp14_a5_2_4_violation]
    // coverity[autosar_cpp14_m5_2_8_violation]
    // coverity[autosar_cpp14_m5_0_9_violation]
    return static_cast<std::size_t>(reinterpret_cast<std::uint8_t*>(pointer) - static_cast<std::uint8_t*>(memory_resource->getBaseAddress()));// NOLINT(cppcoreguidelines-pro-type-reinterpret-cast): Tolerated
    // clang-format on
}

template <typename T>
std::size_t CalculateNeededAllignmentSpaceForElementType()
{
    if ((sizeof(T) % alignof(std::max_align_t)) == 0U)
    {
        return sizeof(T);
    }
    else if (sizeof(T) > alignof(std::max_align_t))
    {
        return sizeof(T) + (alignof(std::max_align_t) - (sizeof(T) % alignof(std::max_align_t)));
    }
    else
    {
        return alignof(std::max_align_t);
    }
}

inline std::size_t CalculateNeededAllignmentSpaceForElementSize(std::size_t size)
{
    if ((size % alignof(std::max_align_t)) == 0U)
    {
        return size;
    }
    else if (size > alignof(std::max_align_t))
    {
        return size + (alignof(std::max_align_t) - (size % alignof(std::max_align_t)));
    }
    else
    {
        return alignof(std::max_align_t);
    }
}

template <typename T>
std::size_t EstimateChunkListAllocationSize(const T& container)
{
    std::size_t chunk_list_size_estimate{0U};
    chunk_list_size_estimate += kStlContainerStorageNeeds;
    // No possibility for having data loss here
    // coverity[autosar_cpp14_a4_7_1_violation]
    chunk_list_size_estimate += container.size() * kStlContainerElementStorageNeeds;
    // No possibility for having data loss here
    // coverity[autosar_cpp14_a4_7_1_violation]
    chunk_list_size_estimate += container.size() * CalculateNeededAllignmentSpaceForElementType<SharedMemoryChunk>();
    return chunk_list_size_estimate;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // AAS_ANALYSIS_TRACING_COMMON_SHARED_MEMORY_LOCATION_HELPERS_H
