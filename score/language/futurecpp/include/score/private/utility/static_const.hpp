/********************************************************************************
 * Copyright (c) 2022 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2022 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_STATIC_CONST_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_STATIC_CONST_HPP

namespace score::cpp
{

/// @brief Define global constexpr variables in header files without ODR-violations using score::cpp::static_const.
/// @see http://ericniebler.github.io/std/wg21/D4381.html#no-violations-of-the-one-definition-rule
///
/// Example:
/// namespace score::cpp
/// {
/// struct in_place_t
/// {
///     explicit in_place_t() = default;
/// };
/// namespace
/// {
/// constexpr auto& in_place = static_const<in_place_t>::value;
/// }
/// } // namespace score::cpp
template <typename T>
struct static_const
{
    static constexpr T value{};
};

template <typename T>
constexpr T static_const<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_STATIC_CONST_HPP
