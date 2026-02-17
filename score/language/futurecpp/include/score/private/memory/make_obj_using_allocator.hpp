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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_MAKE_OBJ_USING_ALLOCATOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_MAKE_OBJ_USING_ALLOCATOR_HPP

#include <score/private/memory/uses_allocator_construction_args.hpp>
#include <score/tuple.hpp>

#include <utility>

namespace score::cpp
{

/// \brief Creates an object of the given type `T` by means of uses-allocator construction
template <typename T, typename Alloc, typename... Args>
constexpr T make_obj_using_allocator(const Alloc& alloc, Args&&... args)
{
    return score::cpp::make_from_tuple<T>(
        score::cpp::detail::uses_allocator_construction_args<T>(alloc, std::forward<Args>(args)...));
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_MAKE_OBJ_USING_ALLOCATOR_HPP
