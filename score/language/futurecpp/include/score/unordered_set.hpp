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
/// \brief Score.Futurecpp.UnorderedSet component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_UNORDERED_SET_HPP
#define SCORE_LANGUAGE_FUTURECPP_UNORDERED_SET_HPP

#include <score/private/iterator/at.hpp>   // IWYU pragma: export
#include <score/private/iterator/data.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <score/memory_resource.hpp>

#include <unordered_set> // IWYU pragma: export

namespace score::cpp
{
namespace pmr
{

template <class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
using unordered_set = std::unordered_set<Key, Hash, Pred, score::cpp::pmr::polymorphic_allocator<Key>>;

template <class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
using unordered_multiset = std::unordered_multiset<Key, Hash, Pred, score::cpp::pmr::polymorphic_allocator<Key>>;

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_UNORDERED_SET_HPP
