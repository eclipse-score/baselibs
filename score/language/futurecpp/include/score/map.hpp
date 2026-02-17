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
/// \brief Score.Futurecpp.Map component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_MAP_HPP
#define SCORE_LANGUAGE_FUTURECPP_MAP_HPP

#include <score/private/iterator/at.hpp>   // IWYU pragma: export
#include <score/private/iterator/data.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <score/memory_resource.hpp>

#include <map> // IWYU pragma: export

namespace score::cpp
{
namespace pmr
{

template <class Key, class T, class Compare = std::less<Key>>
using map = std::map<Key, T, Compare, score::cpp::pmr::polymorphic_allocator<std::pair<const Key, T>>>;

template <class Key, class T, class Compare = std::less<Key>>
using multimap = std::multimap<Key, T, Compare, score::cpp::pmr::polymorphic_allocator<std::pair<const Key, T>>>;

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_MAP_HPP
