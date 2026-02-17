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

#ifndef SCORE_LANGUAGE_FUTURECPP_UTILITY_HPP
#define SCORE_LANGUAGE_FUTURECPP_UTILITY_HPP

#include <score/private/iterator/at.hpp>             // IWYU pragma: export
#include <score/private/utility/as_const.hpp>        // IWYU pragma: export
#include <score/private/utility/ignore.hpp>          // IWYU pragma: export
#include <score/private/utility/in_place_t.hpp>      // IWYU pragma: export
#include <score/private/utility/in_place_type_t.hpp> // IWYU pragma: export
#include <score/private/utility/in_range.hpp>        // IWYU pragma: export
#include <score/private/utility/int_cmp.hpp>         // IWYU pragma: export
#include <score/private/utility/static_const.hpp>    // IWYU pragma: export
#include <score/private/utility/to_underlying.hpp>   // IWYU pragma: export

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>

#include <score/assert.hpp>
#include <score/float.hpp>
#include <score/size.hpp>

namespace score::cpp
{

/// \brief Checks if a given value can be represented in another data type.
///
/// \tparam Out The desired new data type.
/// \tparam In The current data type of value.
/// \param value The value in question.
/// \return true if value can be represented in Out, false otherwise.
template <typename Out, typename In>
constexpr bool is_convertible(const In& value) noexcept
{
    static_assert(!(std::is_floating_point<In>::value && std::is_floating_point<Out>::value),
                  "Not both type are allowed to be floating point.");

    const Out casted_value{static_cast<Out>(value)};
    const In twice_casted_value{static_cast<In>(casted_value)};

    if (!score::cpp::equals_bitexact(value, twice_casted_value))
    {
        return false;
    }

    const bool have_types_same_signedness{std::is_signed<In>::value == std::is_signed<Out>::value};
    return have_types_same_signedness || ((casted_value < Out{}) == (value < In{}));
}

/// \brief Cast the given value to a different data type.
///
/// \tparam Out The desired new data type.
/// \tparam In The current data type of value.
/// \param value The value in question.
/// \pre score::cpp::is_convertible<Out>(value).
/// \return The converted value.
template <typename Out, typename In>
constexpr Out narrow_cast(const In& value)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(score::cpp::is_convertible<Out>(value));
    return static_cast<Out>(value);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_UTILITY_HPP
