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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_SHARED_MEMORY_CHUNK_H
#define SCORE_ANALYSIS_TRACING_COMMON_SHARED_MEMORY_CHUNK_H

#include "score/analysis/tracing/common/canary_wrapper/canary_wrapper.h"
#include "shared_memory_location.h"
#include "types.h"

namespace score
{
namespace analysis
{
namespace tracing
{
/// @brief Internal data structure for SharedMemoryChunk
///
/// Stores the actual data without canary protection.
/// This is wrapped by CanaryWrapper to provide corruption detection.
class SharedMemoryChunkData
{
  public:
    SharedMemoryLocation start_;  ///< Object that stores locators to the data that needs to be traced
    std::size_t size_;            ///< Size of the data that needs to be traced
    SharedMemoryChunkData() noexcept : SharedMemoryChunkData(SharedMemoryLocation{}, 0U) {}

    SharedMemoryChunkData(const SharedMemoryLocation& start, std::size_t size) noexcept : start_(start), size_(size) {}
};

/// @brief SharedMemoryChunk type with canary protection
///
/// Type alias for CanaryWrapper protecting SharedMemoryChunkData.
/// Used to store chunks of data residing in shared-memory with corruption detection.
using SharedMemoryChunk = CanaryWrapper<SharedMemoryChunkData, std::uint32_t>;

/// @brief Equality operator for SharedMemoryChunk
bool operator==(const SharedMemoryChunk& lhs, const SharedMemoryChunk& rhs) noexcept;

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif
