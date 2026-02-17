/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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
/// @file
/// @copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_OPTIONAL_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_OPTIONAL_HPP

#include <type_traits>

namespace score::cpp
{

template <typename T>
class optional;

/// \brief Check whether the type is an optional type
/// \tparam T: type to be checked
template <typename T>
struct is_optional : std::false_type
{
};

/// \brief Check whether the type is an optional type
/// \tparam T: type to be checked
template <typename T>
struct is_optional<score::cpp::optional<T>> : std::true_type
{
};

/// \brief Check whether the type is an optional type
/// \tparam T: type to be checked
template <typename T>
constexpr auto is_optional_v = is_optional<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_OPTIONAL_HPP
