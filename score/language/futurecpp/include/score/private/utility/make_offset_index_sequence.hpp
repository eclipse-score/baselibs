/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2025 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_MAKE_OFFSET_INDEX_SEQUENCE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_MAKE_OFFSET_INDEX_SEQUENCE_HPP

#include <utility>

namespace score::cpp
{

namespace detail
{

template <std::size_t Offset, std::size_t... Is>
constexpr auto make_offset_index_sequence(std::index_sequence<Is...>)
{
    return std::index_sequence<(Offset + Is)...>{};
}

} // namespace detail

template <std::size_t O, std::size_t N>
using make_offset_index_sequence = decltype(score::cpp::detail::make_offset_index_sequence<O>(std::make_index_sequence<N>{}));

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_MAKE_OFFSET_INDEX_SEQUENCE_HPP
