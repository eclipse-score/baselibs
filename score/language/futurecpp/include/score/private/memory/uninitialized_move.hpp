/********************************************************************************
 * Copyright (c) 2018 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2018 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_MOVE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_MOVE_HPP

#include <score/private/memory/construct_at.hpp>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace score::cpp
{

namespace detail
{

template <bool TrivialTypes>
struct uninitialized_move_impl
{
    template <typename InputIter, typename ForwardIter>
    static ForwardIter move(InputIter first, InputIter last, ForwardIter first_destination)
    {
        auto current = first_destination;
        try
        {
            for (; first != last; ++first)
            {
                static_cast<void>(score::cpp::detail::construct_at(std::addressof(*current), std::move(*first)));
                static_cast<void>(++current);
            }
            return current;
        }
        catch (...)
        {
            using value_type = typename std::iterator_traits<ForwardIter>::value_type;
            for (; first_destination != current; ++first_destination)
            {
                first_destination->~value_type();
            }
            throw;
        }
    }
};

template <>
struct uninitialized_move_impl<true>
{
    template <typename InputIter, typename ForwardIter>
    static ForwardIter move(InputIter first, InputIter last, ForwardIter first_destination)
    {
        return std::uninitialized_copy(first, last, first_destination);
    }
};

} // namespace detail

/// \brief Backport of C++17 std::uninitialized_move().
template <typename InputIter, typename ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter first_destination)
{
    using input_value_type = typename std::iterator_traits<InputIter>::value_type;
    using output_value_type = typename std::iterator_traits<ForwardIter>::value_type;
    using input_reference = typename std::iterator_traits<InputIter>::reference;

    return score::cpp::detail::uninitialized_move_impl < std::is_trivial<input_value_type>::value &&
           std::is_trivial<output_value_type>::value &&
           std::is_constructible<output_value_type, input_reference>::value > ::move(first, last, first_destination);
}

namespace detail
{

template <bool TrivialTypes>
struct uninitialized_move_n_impl
{
    template <typename InputIter, typename Size, typename ForwardIter>
    static std::pair<InputIter, ForwardIter> move(InputIter first, Size n, ForwardIter first_destination)
    {
        auto current = first_destination;
        try
        {
            for (; n > 0; --n)
            {
                static_cast<void>(score::cpp::detail::construct_at(std::addressof(*current), std::move(*first)));
                static_cast<void>(++current);
                static_cast<void>(++first);
            }
            return {first, current};
        }
        catch (...)
        {
            using value_type = typename std::iterator_traits<ForwardIter>::value_type;
            for (; first_destination != current; ++first_destination)
            {
                first_destination->~value_type();
            }
            throw;
        }
    }
};

template <>
struct uninitialized_move_n_impl<true>
{
    template <typename InputIter, typename Size, typename ForwardIter>
    static std::pair<InputIter, ForwardIter> move(InputIter first, Size n, ForwardIter first_destination)
    {
        return {first + static_cast<std::ptrdiff_t>(n), std::uninitialized_copy_n(first, n, first_destination)};
    }
};

} // namespace detail

/// \brief Backport of C++17 std::uninitialized_move().
template <typename InputIter, typename Size, typename ForwardIter>
std::pair<InputIter, ForwardIter> uninitialized_move_n(InputIter first, Size n, ForwardIter first_destination)
{
    using input_value_type = typename std::iterator_traits<InputIter>::value_type;
    using output_value_type = typename std::iterator_traits<ForwardIter>::value_type;
    using input_reference = typename std::iterator_traits<InputIter>::reference;

    return score::cpp::detail::uninitialized_move_n_impl < std::is_trivial<input_value_type>::value &&
           std::is_trivial<output_value_type>::value &&
           std::is_constructible<output_value_type, input_reference>::value > ::move(first, n, first_destination);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNINITIALIZED_MOVE_HPP
