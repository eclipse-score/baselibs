/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_CONSTRUCT_AT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_CONSTRUCT_AT_HPP

#include <memory>
#include <utility>

namespace score::cpp
{
namespace detail
{

template <typename T>
constexpr void* voidify(T& obj) noexcept
{
    return std::addressof(obj);
}

/// \brief Creates a `T` object initialized with arguments `args...` at given address `p`.
///
/// Implements https://isocpp.org/files/papers/N4860.pdf#subsection.25.11.7
/// Non-conforming: Not constexpr and not constrained.
template <typename T, typename... Args, typename = decltype(::new (std::declval<void*>()) T(std::declval<Args>()...))>
T* construct_at(T* const p, Args&&... args)
{
    return ::new (score::cpp::detail::voidify(*p)) T(std::forward<Args>(args)...);
}

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_CONSTRUCT_AT_HPP
