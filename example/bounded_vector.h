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
#ifndef SCORE_SHM_EXAMPLE_BOUNDED_VECTOR_H
#define SCORE_SHM_EXAMPLE_BOUNDED_VECTOR_H

#include "score/shm/vector.h"

#include <cstddef>

namespace score::shm::example
{

/// @brief A self-contained Vector+buffer bundle suitable for shared-memory placement.
///
/// All state (Vector metadata, element storage) lives within the struct itself at fixed
/// offsets, so OffsetPtr resolves correctly when the struct is mapped at different base
/// addresses in different processes. The allocator member is unused; the Vector operates
/// in fixed-capacity buffer mode.
///
/// @tparam T Element type.
/// @tparam Capacity Maximum number of elements.
template <typename T, std::size_t Capacity>
struct BoundedVector
{
    BoundedVector() noexcept : vector{Vector<T>::CreateWithBuffer(buffer, sizeof(buffer))} {}

    BoundedVector(const BoundedVector&) = delete;
    BoundedVector& operator=(const BoundedVector&) = delete;
    BoundedVector(BoundedVector&&) = delete;
    BoundedVector& operator=(BoundedVector&&) = delete;
    ~BoundedVector() = default;

    Vector<T> vector;
    alignas(T) std::byte buffer[Capacity * sizeof(T)];
};

}  // namespace score::shm::example

#endif  // SCORE_SHM_EXAMPLE_BOUNDED_VECTOR_H
