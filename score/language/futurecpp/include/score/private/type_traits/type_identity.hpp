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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_TYPE_IDENTITY_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_TYPE_IDENTITY_HPP

#include <type_traits>

namespace score::cpp
{

///
/// \brief type_identity can be used to block template argument deduction and is the C++14 version of C++20
/// \see https://en.cppreference.com/w/cpp/types/type_identity
///
/// \tparam T: type which is exposed
///
template <typename T>
struct type_identity
{
    using type = T;
};

template <typename T>
using type_identity_t = typename score::cpp::type_identity<T>::type;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_TYPE_IDENTITY_HPP
