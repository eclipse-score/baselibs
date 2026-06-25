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

#include "score/nothrow/map.h"
#include "score/nothrow/vector.h"

#include <cstddef>
#include <functional>

namespace score::nothrow::example
{

/// @brief A self-contained Vector+Map bundle suitable for shared-memory placement.
///
/// Both containers are provisioned from inline buffers via CreateWithBuffer():
/// the vector treats its buffer as fixed capacity, and the map carves nodes from
/// its buffer with an internal free-list pool (bounded, and reused on erase).
///
/// No allocator or memory resource is involved, so there is no vtable, no raw
/// upstream pointer, and no process-local singleton to relocate: with the
/// default offset pointer policy the whole bundle is self-contained and
/// position-independent across mappings of the same shared-memory object.
///
/// @tparam T Vector element type.
/// @tparam VectorCapacity Maximum vector elements.
/// @tparam MapNodeCount Maximum live map entries.
template <typename T, std::size_t VectorCapacity, std::size_t MapNodeCount>
struct BoundedContainers
{
    using MapType = Map<int, int, std::less<int>>;

    BoundedContainers() noexcept
        : vector{Vector<T>::CreateWithBuffer(vector_buffer, sizeof(vector_buffer))},
          map{MapType::CreateWithBuffer(map_buffer, sizeof(map_buffer))}
    {
    }

    BoundedContainers(const BoundedContainers&) = delete;
    BoundedContainers& operator=(const BoundedContainers&) = delete;
    BoundedContainers(BoundedContainers&&) = delete;
    BoundedContainers& operator=(BoundedContainers&&) = delete;
    ~BoundedContainers() = default;

    alignas(T) std::byte vector_buffer[VectorCapacity * sizeof(T)];
    alignas(MapType::cell_alignment()) std::byte map_buffer[MapType::required_bytes(MapNodeCount)];
    Vector<T> vector;
    MapType map;
};

}  // namespace score::nothrow::example

#endif  // SCORE_SHM_EXAMPLE_BOUNDED_CONTAINERS_H
