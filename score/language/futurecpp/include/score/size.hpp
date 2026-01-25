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
/// \brief Score.Futurecpp.Size Component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_SIZE_HPP
#define SCORE_LANGUAGE_FUTURECPP_SIZE_HPP

#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <cstddef>
#include <limits> // to be removed but removing them may break downstream

namespace score::cpp
{

inline namespace literals
{

///
/// \brief Type literal for std::size_t, e.g. 7_UZ is equivalent to std::size_t{7}
///
/// Implements: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0330r8.html
///
constexpr std::size_t operator""_UZ(const unsigned long long int x) { return static_cast<std::size_t>(x); }

///
/// \brief Type literal for std::ptrdiff_t
///
/// Implements: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0330r8.html
///
constexpr std::ptrdiff_t operator""_Z(const unsigned long long int x) { return static_cast<std::ptrdiff_t>(x); }

} // namespace literals

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_SIZE_HPP
