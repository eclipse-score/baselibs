/********************************************************************************
 * Copyright (c) 2023 Contributors to the Eclipse Foundation
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
/// @copyright Copyright (c) 2023 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_EXPECTED_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_EXPECTED_HPP

#include <score/expected.hpp>

#include <type_traits>

namespace score::cpp
{

/// \brief Check whether the type is an expected type
/// \tparam T: type to be checked
template <typename T>
struct is_expected : std::false_type
{
};

/// \brief Check whether the type is an expected type
/// \tparam T: type to be checked
template <typename T, typename E>
struct is_expected<score::cpp::expected<T, E>> : std::true_type
{
};

/// \brief Check whether the type is an expected type
/// \tparam T: type to be checked
template <typename T>
constexpr auto is_expected_v = is_expected<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_EXPECTED_HPP
