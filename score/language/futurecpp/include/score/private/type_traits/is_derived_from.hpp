/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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
/// @file
/// @copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_DERIVED_FROM_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_DERIVED_FROM_HPP

#include <score/private/type_traits/conjunction.hpp>
#include <type_traits>

namespace score::cpp
{
namespace detail
{

template <typename Derived, typename Base>
using is_derived_from = score::cpp::conjunction<std::is_base_of<Base, Derived>,
                                         std::is_convertible<const volatile Derived*, const volatile Base*>>;

}
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_DERIVED_FROM_HPP
