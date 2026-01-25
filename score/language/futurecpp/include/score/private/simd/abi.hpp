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

///
/// \file
/// \copyright Copyright (c) 2025 Contributors to the Eclipse Foundation
///
/// References to http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/n4808.pdf are of the form
/// [parallel] chapter paragraph
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ABI_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ABI_HPP

#include <cstddef>
#include <type_traits>

namespace score::cpp
{
namespace simd
{

namespace detail
{
struct invalid_abi
{
};

template <typename T>
struct native_abi
{
    using type = invalid_abi;
};

template <typename T, std::size_t N>
struct deduce_abi
{
    using type = invalid_abi;
};

/// \brief If `T` is a standard or extended ABI tag returns `true_type`, and `false_type` otherwise.
///
/// [parallel] 9.4 1 and 2
/// \{
template <typename T>
struct is_abi_tag : std::false_type
{
};
template <typename T>
constexpr bool is_abi_tag_v{is_abi_tag<T>::value};
/// \}

} // namespace detail

/// \brief An ABI tag is a type that indicates a choice of size and binary representation for objects of data-parallel
/// type.
///
/// [parallel] 9.3 ff
/// \{
template <typename T>
using native_abi = typename detail::native_abi<T>::type;

template <typename T, std::size_t N>
using deduce_abi = typename detail::deduce_abi<T, N>::type;
/// \}

} // namespace simd
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ABI_HPP
