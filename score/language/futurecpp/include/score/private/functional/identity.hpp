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
/// \brief Score.Futurecpp.Functional Component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_IDENTITY_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_IDENTITY_HPP

#include <utility>

namespace score::cpp
{

///
/// \brief A function object type whose operator() returns its argument unchanged.
///
struct identity
{
    template <typename T>
    constexpr T&& operator()(T&& t) const noexcept
    {
        return std::forward<T>(t);
    }
};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_IDENTITY_HPP
