/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Narrow component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_NARROW_HPP
#define SCORE_LANGUAGE_FUTURECPP_NARROW_HPP

#include <score/optional.hpp>
#include <score/utility.hpp>

namespace score::cpp
{

/// \brief Cast the given value to a different data type if representable in that new data type.
///
/// \tparam Out The desired new data type.
/// \tparam In The current data type of value.
/// \param value The value in question.
/// \return optional<Out> An optional holding no or converted value depending on whether value is representable as Out.
template <typename Out, typename In>
constexpr optional<Out> narrow(const In& value) noexcept
{
    optional<Out> result{};
    if (score::cpp::is_convertible<Out>(value))
    {
        result = static_cast<Out>(value);
    }

    return result;
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_NARROW_HPP
