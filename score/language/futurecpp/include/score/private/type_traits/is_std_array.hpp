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

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_STD_ARRAY_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_STD_ARRAY_HPP

#include <array>
#include <type_traits>

namespace score::cpp
{
namespace detail
{

/// \brief Check whether the type is a std::array type
///
/// note: `std::is_array<std::array>` returns `false`
///
/// \tparam T type to be checked
template <typename T>
struct is_std_array : std::false_type
{
};

/// \brief Check whether the type is a std::array type
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type
{
};

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_STD_ARRAY_HPP
