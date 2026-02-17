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

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IGNORE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IGNORE_HPP

namespace score::cpp
{
namespace detail
{

struct swallow
{
    template <typename T>
    // NOLINTNEXTLINE(misc-unconventional-assign-operator) not allow chaining so return value is `const swallow&`
    constexpr const swallow& operator=(T&&) const
    {
        return *this;
    }
};

} // namespace detail

///
/// \brief Swallows the value assigned to it.
///
/// Shall be used to explicitly state that a return value shall be ignored. The value assigned to ignore is ignored and
/// side-effect free.
///
/// Note that the implementation of std::ignore is "unspecified". So who knows what operations are done for the
/// assignment in std::ignore and even than it can change with newer C++ versions.
///
constexpr score::cpp::detail::swallow ignore{};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IGNORE_HPP
