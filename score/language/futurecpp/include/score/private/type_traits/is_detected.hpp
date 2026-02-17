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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_DETECTED_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_DETECTED_HPP

#include <type_traits>

#include <score/private/type_traits/void_t.hpp>

namespace score::cpp
{

namespace detail
{
template <class Default, class AlwaysVoid, template <class...> class Op, class... Args>
struct detector
{
    using value_t = std::false_type;
    using type = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...>
{
    using value_t = std::true_type;
    using type = Op<Args...>;
};

///
/// \brief class type used by detected_t to indicate detection failure. It is the C++14 version of library fundamentals
/// TS v2 \see https://en.cppreference.com/w/cpp/experimental/nonesuch
///
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions) Follows literaly the C++ standard
struct nonesuch
{
    ~nonesuch() = delete;
    nonesuch(const nonesuch&) = delete;
    void operator=(const nonesuch&) = delete;
};
} // namespace detail

///
/// \brief check whether a certain trait is available in a class e.g the presence of a member or a function. It is the
/// C++14 version of library fundamentals TS v2 \see https://en.cppreference.com/w/cpp/experimental/is_detected
template <template <class...> class Op, class... Args>
using is_detected = typename detail::detector<detail::nonesuch, void, Op, Args...>::value_t;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_DETECTED_HPP
