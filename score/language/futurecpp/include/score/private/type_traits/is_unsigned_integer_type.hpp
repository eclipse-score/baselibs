/********************************************************************************
 * Copyright (c) 2022 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2022 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_UNSIGNED_INTEGER_TYPE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_UNSIGNED_INTEGER_TYPE_HPP

#include <type_traits>

namespace score::cpp
{

namespace detail
{

template <typename T>
struct is_unsigned_integer_type
{
    static constexpr bool value{std::is_same<T, unsigned char>::value || std::is_same<T, unsigned short int>::value ||
                                std::is_same<T, unsigned int>::value || std::is_same<T, unsigned long int>::value ||
                                std::is_same<T, unsigned long long int>::value};
};

} // namespace detail

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_UNSIGNED_INTEGER_TYPE_HPP
