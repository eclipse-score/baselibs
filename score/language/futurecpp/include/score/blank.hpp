/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Blank component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_BLANK_HPP
#define SCORE_LANGUAGE_FUTURECPP_BLANK_HPP

namespace score::cpp
{

///
/// \brief This type represents a 'blank' object which can be used in Score.Futurecpp.Optional or Score.Futurecpp.Variant
///
/// Implements `std::monostate`
/// https://en.cppreference.com/w/cpp/utility/variant/monostate
///
struct blank
{
};

/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator<(blank, blank) noexcept { return false; }
/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator>(blank, blank) noexcept { return false; }
/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator<=(blank, blank) noexcept { return true; }
/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator>=(blank, blank) noexcept { return true; }
/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator==(blank, blank) noexcept { return true; }
/// \brief Specialization of comparison operators for \ref blank. All instances of \ref blank are equal by definition.
constexpr bool operator!=(blank, blank) noexcept { return false; }

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_BLANK_HPP
