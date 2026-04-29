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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_CHUNK_LIST_SHM_DATA_CHUNK_LIST_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_CHUNK_LIST_SHM_DATA_CHUNK_LIST_H

#include "score/analysis/tracing/common/canary_wrapper/canary_wrapper.h"
#include "score/analysis/tracing/common/shared_list/shared_list.h"
#include <array>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief Type alias for the inner list of SharedMemoryChunk elements
using ShmChunkList = shared::List<SharedMemoryChunk>;

/// @brief Type alias for the canary-protected list of SharedMemoryChunk elements
using ShmChunkVector = CanaryWrapper<ShmChunkList>;

/// @brief ShmDataChunkList class
///
/// Class used to pass lists of shared-memory data chunks.
class ShmDataChunkList
{
  public:
    /// @brief Constructor. Create a new ShmDataChunkList object.
    ///
    /// @param root Reference to the root element in the list.
    explicit ShmDataChunkList(const SharedMemoryChunk& root);

    /// @brief Constructor. Create a new empty ShmDataChunkList object.
    ShmDataChunkList();

    ShmDataChunkList(const ShmDataChunkList&) = delete;
    ShmDataChunkList& operator=(const ShmDataChunkList&) = delete;
    ShmDataChunkList(ShmDataChunkList&&) = delete;
    ShmDataChunkList& operator=(ShmDataChunkList&&) = delete;

    /// @brief Destructor. Destroys a ShmDataChunkList object.
    ~ShmDataChunkList() = default;

    /// @brief Append SharedMemoryChunk to the list in the front.
    ///
    /// @param chunk reference to the new element that should be appended in front of the list.
    void AppendFront(const SharedMemoryChunk& chunk);

    /// @brief Append SharedMemoryChunk to the list.
    ///
    /// @param next reference to the new element that should be appended to list at the end.
    void Append(const SharedMemoryChunk& next);

    /// @brief Get count of chunks currently on the list
    ///
    /// @return Count of chunks in the list
    std::size_t Size() const;

    /// @brief Clear the contents of LocalDataChunkList
    void Clear();

    /// @brief Get the underlying container
    const std::array<SharedMemoryChunk, kMaxChunksPerOneTraceRequest>& GetList() const;

    /// @brief Get the underlying container
    std::array<SharedMemoryChunk, kMaxChunksPerOneTraceRequest>& GetList();

    /// @brief == operator overloading to check if two chunk list are equal or not
    ///
    /// @param other the other instance of the chunk list which is the subject of the comparison.
    ///
    /// @return True if the two instances are typical and False otherwise.
    friend bool operator==(const ShmDataChunkList& lhs, const ShmDataChunkList& rhs) noexcept;

  private:
    /// @brief Private Delegator Constructor to centralize the members initialization
    /// @param root reference to the root element in the list
    /// @param has_root check if the root chunk should be considered
    ShmDataChunkList(const SharedMemoryChunk& root, bool has_root);

    std::array<SharedMemoryChunk, kMaxChunksPerOneTraceRequest>
        list_;  ///< Fixed-size array of shared memory data chunks
    std::uint8_t number_of_chunks_;
};
bool operator==(const ShmDataChunkList& lhs, const ShmDataChunkList& rhs) noexcept;
}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_CHUNK_LIST_SHM_DATA_CHUNK_LIST_H
