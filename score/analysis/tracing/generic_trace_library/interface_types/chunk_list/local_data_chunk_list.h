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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_CHUNK_LIST_LOCAL_DATA_CHUNK_LIST_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_CHUNK_LIST_LOCAL_DATA_CHUNK_LIST_H

#include "score/analysis/tracing/common/flexible_circular_allocator/custom_polymorphic_offset_ptr_allocator.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location.h"
#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/chunk_list/shm_data_chunk_list.h"
#include "score/memory/shared/managed_memory_resource.h"
#include "score/result/result.h"
#include <array>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief LocalDataChunk class
///
/// Class used to store in the list single element of the local memory data chunks.
/// It stores pointer to the data and it's size.
class LocalDataChunk
{
  public:
    //  Tolerated here as this is a generic pointer and will be casted later
    // coverity[autosar_cpp14_a9_6_1_violation]
    const void* start;  ///< Pointer to the data that needs to be traced
    std::size_t size;   ///< Size of the data that needs to be traced
};
bool operator==(const LocalDataChunk& lhs, const LocalDataChunk& rhs) noexcept;

/// @brief LocalDataChunkList class
///
/// Class used to pass lists of local memory data chunks.
class LocalDataChunkList
{
  public:
    /// @brief Constructor. Create a new LocalDataChunk object.
    ///
    /// @param root reference to the root element in the list.
    explicit LocalDataChunkList(const LocalDataChunk& root);

    /// @brief Constructor. Create a new empty LocalDataChunk object.
    LocalDataChunkList();

    LocalDataChunkList(const LocalDataChunkList&) = delete;
    LocalDataChunkList& operator=(const LocalDataChunkList&) = delete;
    LocalDataChunkList(LocalDataChunkList&&) = delete;
    LocalDataChunkList& operator=(LocalDataChunkList&&) = delete;

    /// @brief Destructor. Destroys a LocalDataChunkList object.
    ~LocalDataChunkList() = default;

    /// @brief Append LocalDataChunk to the list in the front.
    ///
    /// @param chunk reference to the new element that should be appended in front of the list.
    void AppendFront(const LocalDataChunk& chunk);

    /// @brief Append LocalDataChunk to the list.
    ///
    /// @param next reference to the new element that should be appended to list at the end.
    void Append(const LocalDataChunk& next);

    /// @brief Get count of chunks currently on the list
    ///
    /// @return Count of chunks in the list
    std::size_t Size() const;

    /// @brief Clear the contents of LocalDataChunkList
    void Clear();

    /// @brief Get the underlying container
    const std::array<LocalDataChunk, kMaxChunksPerOneTraceRequest>& GetList() const;

    /// @brief Get the underlying container
    std::array<LocalDataChunk, kMaxChunksPerOneTraceRequest>& GetList();

    /// @brief Copies LocalDataChunkList into shared-memory and copy all data from local to shared
    /// memory.
    ///
    /// @param memory_resource shared pointer holding a shared-memory resource in which the ChunkList will be placed.
    /// @param handle shared-memory handle assigned by the daemon during region registration
    ///
    /// @return SharedMemoryLocation where vector with data is stored or error code if operation fails.
    score::Result<SharedMemoryLocation> SaveToSharedMemory(
        ResourcePointer memory_resource,
        ShmObjectHandle handle,
        std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator);
    /// @param lhs the  instance of the chunk list which is the subject of the comparison.
    /// @param rhs the other instance of the chunk list which is the subject of the comparison.
    /// @brief == operator overloading to check if two chunk list are similar or not
    /// @return True if the two instances are typical and False otherwise.
    friend bool operator==(const LocalDataChunkList& lhs, const LocalDataChunkList& rhs) noexcept;
    // check on availabilty of the mem resource
    bool IsValidResource(ResourcePointer memory_resource, ShmObjectHandle handle) const;
    // check on size of the memory
    bool HasEnoughMemory(std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator,
                         std::size_t required_memory_size) const;
    // allocate vector
    score::Result<void*> AllocateVector(std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator) const;
    // construct vector in shared memory
    ShmChunkVector* ConstructShmChunkVector(void* vector_shm_raw_pointer,
                                            std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator) const;
    // check on availabilty of the element
    bool IsValidElement(const LocalDataChunk& element) const;
    // copy data to the shared memory
    void CopyDataToSharedMemory(const LocalDataChunk& element, void* shm_pointer) const;
    // do some cleaning in the memory
    void CleanupAllocatedData(std::array<std::pair<void*, std::size_t>, kMaxChunksPerOneTraceRequest>& allocated_data,
                              const std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator,
                              ShmChunkVector* vector,
                              void* vector_shm_raw_pointer) const;
    // create an object to refer to the location in shared memory where the vector is stored.
    score::Result<SharedMemoryLocation> CreateSharedMemoryLocation(ShmChunkVector* vector,
                                                                 ResourcePointer memory_resource,
                                                                 ShmObjectHandle handle) const;
    score::Result<SharedMemoryLocation> FillVectorInSharedMemory(
        ShmChunkVector* vector,
        ResourcePointer memory_resource,
        ShmObjectHandle handle,
        const std::shared_ptr<IFlexibleCircularAllocator>& flexible_allocator,
        void* vector_shm_raw_pointer);

  private:
    /// @brief Private Delegator Constructor to centralize the members initialization
    /// @param root reference to the root element in the list
    /// @param has_root check if the root chunk should be considered
    LocalDataChunkList(const LocalDataChunk& root, bool has_root);

    std::array<LocalDataChunk, kMaxChunksPerOneTraceRequest> list_;  ///< Fixed-size array of local memory data chunks
    std::uint8_t number_of_chunks_;
};
bool operator==(const LocalDataChunkList& lhs, const LocalDataChunkList& rhs) noexcept;
}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_CHUNK_LIST_LOCAL_DATA_CHUNK_LIST_H
