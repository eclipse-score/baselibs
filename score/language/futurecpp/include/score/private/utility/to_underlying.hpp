/********************************************************************************
 * Copyright (c) 2017 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2017 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Utility Component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_TO_UNDERLYING_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_TO_UNDERLYING_HPP

#include <type_traits>

namespace score::cpp
{

/// \brief Converts an enumeration to its underlying type.
///
/// \tparam Enum The enumeration type.
/// \param e Enumeration value to convert.
/// \return The integer value of the underlying type of Enum, converted from e.
template <typename Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_TO_UNDERLYING_HPP
