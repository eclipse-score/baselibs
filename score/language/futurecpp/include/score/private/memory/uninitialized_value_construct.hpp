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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP

#include <score/private/memory/construct_at.hpp>
#include <iterator>
#include <memory>

namespace score::cpp
{

/// \brief Backport of C++17 std::uninitialized_value_construct_n().
template <typename ForwardIter, typename Size>
ForwardIter uninitialized_value_construct_n(ForwardIter first, Size n)
{
    auto current = first;
    try
    {
        for (; n > 0; --n)
        {
            static_cast<void>(score::cpp::detail::construct_at(std::addressof(*current)));
            static_cast<void>(++current);
        }
        return current;
    }
    catch (...)
    {
        using value_type = typename std::iterator_traits<ForwardIter>::value_type;
        for (; first != current; ++first)
        {
            first->~value_type();
        }
        throw;
    }
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP
