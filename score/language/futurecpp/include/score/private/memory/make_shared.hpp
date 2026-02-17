/********************************************************************************
 * Copyright (c) 2018 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2018 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Memory component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_MAKE_SHARED_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_MAKE_SHARED_HPP

#include <score/memory_resource.hpp>

#include <memory>
#include <utility>

namespace score::cpp
{
namespace pmr
{

/// Creates a shared pointer that manages a new object allocated using a memory resource.
template <typename T, typename... Args>
std::shared_ptr<T> make_shared(score::cpp::pmr::memory_resource* resource, Args&&... args)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(resource != nullptr);
    score::cpp::pmr::polymorphic_allocator<std::remove_cv_t<T>> allocator{resource};
    return std::allocate_shared<std::remove_cv_t<T>>(allocator, std::forward<Args>(args)...);
}

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_MAKE_SHARED_HPP
