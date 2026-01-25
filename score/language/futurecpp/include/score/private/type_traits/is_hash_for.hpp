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
/// \file
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_HASH_FOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_HASH_FOR_HPP

#include <type_traits>

#include <score/private/type_traits/invoke_traits.hpp>

namespace score::cpp
{

/// \brief Check whether the type Hash satisfies the named requirement Hash for the type Key
/// \tparam Hash: hash type to be checked
/// \tparam Key: key type to be checked
template <typename Hash, typename Key>
struct is_hash_for : std::integral_constant<bool,
                                            std::is_copy_constructible<Hash>::value &&
                                                std::is_destructible<Hash>::value && std::is_object<Hash>::value &&
                                                score::cpp::is_invocable_r<std::size_t, const Hash&, const Key&>::value &&
                                                score::cpp::is_invocable_r<std::size_t, const Hash&, Key&>::value &&
                                                score::cpp::is_invocable_r<std::size_t, Hash&, const Key&>::value &&
                                                score::cpp::is_invocable_r<std::size_t, Hash&, Key&>::value>
{
};

/// \brief Check whether the type Hash satisfies the named requirement Hash for the type Key
/// \tparam Hash: hash type to be checked
/// \tparam Key: key type to be checked
template <typename Hash, typename Key>
constexpr auto is_hash_for_v = is_hash_for<Hash, Key>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_HASH_FOR_HPP
