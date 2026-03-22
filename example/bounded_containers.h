/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_SHM_EXAMPLE_BOUNDED_CONTAINERS_H
#define SCORE_SHM_EXAMPLE_BOUNDED_CONTAINERS_H

#include "score/shm/map.h"
#include "score/shm/monotonic_buffer_resource.h"
#include "score/shm/vector.h"

#include <cstddef>
#include <functional>
#include <utility>

namespace score::shm::example
{

/// @brief A self-contained Vector+Map bundle suitable for shared-memory placement.
///
/// The vector uses CreateWithBuffer() with inline storage.
/// The map uses a MonotonicBufferResource whose storage is also inline.
///
/// This keeps all container state and backing storage within the same shared-memory object.
///
/// @tparam T Vector element type.
/// @tparam VectorCapacity Maximum vector elements.
/// @tparam MapBufferBytes Number of bytes reserved for map node allocations.
template <typename T, std::size_t VectorCapacity, std::size_t MapBufferBytes>
struct BoundedContainers
{
    using MapAllocator = PolymorphicAllocator<std::pair<const int, int>>;
    using MapType = Map<int, int, std::less<int>, MapAllocator>;

    BoundedContainers() noexcept
        : vector{Vector<T>::CreateWithBuffer(vector_buffer, sizeof(vector_buffer))},
          map_resource{map_buffer, sizeof(map_buffer), GetNullMemoryResource()},
          map{MapType::CreateOrAbort(std::less<int>{}, MapAllocator{&map_resource})}
    {
    }

    BoundedContainers(const BoundedContainers&) = delete;
    BoundedContainers& operator=(const BoundedContainers&) = delete;
    BoundedContainers(BoundedContainers&&) = delete;
    BoundedContainers& operator=(BoundedContainers&&) = delete;
    ~BoundedContainers() = default;

    alignas(T) std::byte vector_buffer[VectorCapacity * sizeof(T)];
    alignas(std::max_align_t) std::byte map_buffer[MapBufferBytes];
    Vector<T> vector;
    MonotonicBufferResource map_resource;
    MapType map;
};

}  // namespace score::shm::example

#endif  // SCORE_SHM_EXAMPLE_BOUNDED_CONTAINERS_H
