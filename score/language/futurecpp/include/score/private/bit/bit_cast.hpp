/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_BIT_BIT_CAST_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_BIT_BIT_CAST_HPP

#include <score/private/utility/ignore.hpp>

#include <cstring>
#include <type_traits>

namespace score::cpp
{

///
/// \brief Implements the equivalent of `*reinterpret_cast<To*>(&from)`.
///
/// For a thorough description, see the following links:
///  * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0476r2.html
///  * https://en.cppreference.com/w/cpp/numeric/bit_cast
///  * https://chromium.googlesource.com/chromium/src/base/+/master/bit_cast.h.
///
template <typename To, typename From>
inline To bit_cast(const From& from) noexcept
{
    static_assert(sizeof(To) == sizeof(From), "bit_cast requires source and destination types to be of same size");
    static_assert(std::is_trivially_copyable<To>::value, "bit_cast requires destination type to be trivially copyable");
    static_assert(std::is_trivially_copyable<From>::value, "bit_cast requires source type to be trivially copyable");

    To to;
    score::cpp::ignore = std::memcpy(&to, &from, sizeof(to));
    return to;
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_BIT_BIT_CAST_HPP
