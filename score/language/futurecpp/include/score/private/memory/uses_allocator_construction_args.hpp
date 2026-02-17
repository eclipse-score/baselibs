/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_USES_ALLOCATOR_CONSTRUCTION_ARGS_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_USES_ALLOCATOR_CONSTRUCTION_ARGS_HPP

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include <score/apply.hpp>
#include <score/type_traits.hpp>

namespace score::cpp
{
namespace detail
{

// https://isocpp.org/files/papers/N4860.pdf#subsubsection.20.10.8.2
// Uses-allocator construction
//
// (1) Uses-allocator construction with allocator `alloc` and constructor
// arguments `args...` refers to the construction of an object of type `T` such
// that `alloc` is passed to the constructor of `T` if `T` uses an allocator
// type compatible with `alloc`. When applied to the construction of an object
// of type `T`, it is equivalent to initializing it with the value of the
// expression `make_obj_using_allocator<T>(alloc, args...)`, described in `score/memory.hpp`.
//
// (2) The following utility functions support three conventions for passing `alloc` to a
// constructor:

// (2.1) If `T` does not use an allocator compatible with `alloc`, then `alloc` is ignored.
template <typename T, typename... Args>
auto uses_allocator_construction_args_without_allocator(Args&&... args)
{
    return std::forward_as_tuple(std::forward<Args>(args)...);
}

// Otherwise, if `T` has a constructor invocable as `T(allocator_arg, alloc, args...)` (leading-allocator
// convention), then uses-allocator construction chooses this constructor form.
template <typename T, typename Alloc, typename... Args>
auto uses_allocator_construction_args_leading_allocator_convention(const Alloc& alloc, Args&&... args)
{
    return std::tuple<std::allocator_arg_t, const Alloc&, Args&&...>(
        std::allocator_arg, alloc, std::forward<Args>(args)...);
}

// (2.3) Otherwise, if `T` has a constructor invocable as `T(args..., alloc)` (trailing-allocator convention),
// then uses-allocator construction chooses this constructor form.
template <typename T, typename Alloc, typename... Args>
auto uses_allocator_construction_args_trailing_allocator_convention(const Alloc& alloc, Args&&... args)
{
    return std::forward_as_tuple(std::forward<Args>(args)..., alloc);
}

// (3) The `uses_allocator_construction_args` function template takes an allocator and argument list and
// produces (as a tuple) a new argument list matching one of the above conventions.
//
// The `make_obj_using_allocator` and `uninitialized_construct_using_allocator` function templates apply the modified
// constructor arguments to construct an object of type `T` as a return value or in-place, respectively

/// @brief Prepares the argument list needed to create an object of the given type T by means of uses-allocator
/// construction.
///
/// See: https://en.cppreference.com/w/cpp/memory/uses_allocator_construction_args
///
/// Do not construct a trait to detect whether `uses_allocator_construction_args<T>(alloc, args)` is a valid expression
/// because it will always provide a true type. The reason for this is the chose of tag-dispatch over SFINAE in the
/// implementation. With SFINAE `uses_allocator_construction_args<std::pair<U,V>>(alloc, a, b, c)` would be detected as
/// invalid because there is no pair-overload of uses_allocator_construction_args that takes more than two arguments.
/// With the tag-dispatching variant however, this is not detected, because there is only
/// uses_allocator_construction_args(const Alloc& alloc, Args&&... args), which matches any argument list.
template <typename T, typename Alloc, typename... Args>
constexpr auto uses_allocator_construction_args(const Alloc& alloc, Args&&... args) noexcept;

// \{
// Additionally, overloads are provided that treat specializations of pair such that uses-allocator construction is
// applied individually to the `first` and `second` data members.

template <typename T, typename Alloc, typename U, typename V>
constexpr auto
uses_allocator_construction_args_pair(const Alloc& alloc, std::piecewise_construct_t, U&& u, V&& v) noexcept
{
    using First = typename T::first_type;
    using Second = typename T::second_type;
    return std::make_tuple(std::piecewise_construct,
                           score::cpp::apply(
                               [&alloc](auto&&... args) {
                                   return score::cpp::detail::uses_allocator_construction_args<First>(
                                       alloc, std::forward<decltype(args)>(args)...);
                               },
                               std::forward<U>(u)),
                           score::cpp::apply(
                               [&alloc](auto&&... args) {
                                   return score::cpp::detail::uses_allocator_construction_args<Second>(
                                       alloc, std::forward<decltype(args)>(args)...);
                               },
                               std::forward<V>(v)));
}

template <typename T, typename Alloc>
constexpr auto uses_allocator_construction_args_pair(const Alloc& alloc) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_pair<T>(
        alloc, std::piecewise_construct, std::tuple<>{}, std::tuple<>{});
}

template <typename T, typename Alloc, typename U, typename V>
constexpr auto uses_allocator_construction_args_pair(const Alloc& alloc, U&& u, V&& v) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_pair<T>(alloc,
                                                                 std::piecewise_construct,
                                                                 std::forward_as_tuple(std::forward<U>(u)),
                                                                 std::forward_as_tuple(std::forward<V>(v)));
}

template <typename T, typename Alloc, typename U, typename V>
constexpr auto uses_allocator_construction_args_pair(const Alloc& alloc, const std::pair<U, V>& pr) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_pair<T>(
        alloc, std::piecewise_construct, std::forward_as_tuple(pr.first), std::forward_as_tuple(pr.second));
}

template <typename T, typename Alloc, typename U, typename V>
constexpr auto uses_allocator_construction_args_pair(const Alloc& alloc, std::pair<U, V>&& pr) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_pair<T>(alloc,
                                                                 std::piecewise_construct,
                                                                 std::forward_as_tuple(std::get<0>(std::move(pr))),
                                                                 std::forward_as_tuple(std::get<1>(std::move(pr))));
}
// \}

// 20.10.8.2 describes six overloads of uses_allocator_construction_args:
//
// 1.   One non-pair variant dealing with no-allocator, leading-allocator and trailing-allocator conventions.
//      Constraint: `T` is not a specialization of `pair`.
// 2-6. Pair overloads corresponding to the 5 overloads of `uses_allocator_construction_args_pair` above.
//      Constraint: `T` is a specialization of `pair`.

template <typename T, typename Alloc, typename... Args>
constexpr auto uses_allocator_construction_args_non_pair_leading_or_trailing_dispatch(
    std::true_type /*uses_leading_allocator_convention*/, const Alloc& alloc, Args&&... args) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_leading_allocator_convention<T>(alloc,
                                                                                         std::forward<Args>(args)...);
}

template <typename T, typename Alloc, typename... Args>
constexpr auto uses_allocator_construction_args_non_pair_leading_or_trailing_dispatch(
    std::false_type /*uses_leading_allocator_convention*/, const Alloc& alloc, Args&&... args) noexcept
{
    static_assert(std::is_constructible<T, Args..., const Alloc&>::value,
                  "Construction with an allocator must be possible if uses_allocator is true.");
    return score::cpp::detail::uses_allocator_construction_args_trailing_allocator_convention<T>(alloc,
                                                                                          std::forward<Args>(args)...);
}

template <typename T, typename Alloc, typename... Args>
constexpr auto uses_allocator_construction_args_non_pair_compatible_with_allocator_dispatch(
    std::true_type /*uses_allocator*/, const Alloc& alloc, Args&&... args) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_non_pair_leading_or_trailing_dispatch<T>(
        std::is_constructible<T, std::allocator_arg_t, const Alloc&, Args...>{}, alloc, std::forward<Args>(args)...);
}

template <typename T, typename Alloc, typename... Args>
constexpr auto uses_allocator_construction_args_non_pair_compatible_with_allocator_dispatch(
    std::false_type /*uses_allocator*/, const Alloc& /*alloc*/, Args&&... args) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_without_allocator<T>(std::forward<Args>(args)...);
}

template <typename T, typename Alloc, typename... Args>
constexpr auto uses_allocator_construction_args_non_pair(const Alloc& alloc, Args&&... args) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_non_pair_compatible_with_allocator_dispatch<T>(
        std::uses_allocator<T, Alloc>{}, alloc, std::forward<Args>(args)...);
}

template <typename T, typename Alloc, typename... Args>
constexpr auto
uses_allocator_construction_args_dispatch(std::true_type /*is_pair*/, const Alloc& alloc, Args&&... args) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_pair<T>(alloc, std::forward<Args>(args)...);
}

template <typename T, typename Alloc, typename... Args>
constexpr auto
uses_allocator_construction_args_dispatch(std::false_type /*is_pair*/, const Alloc& alloc, Args&&... args) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_non_pair<T>(alloc, std::forward<Args>(args)...);
}

template <typename T, typename Alloc, typename... Args>
constexpr auto uses_allocator_construction_args(const Alloc& alloc, Args&&... args) noexcept
{
    return score::cpp::detail::uses_allocator_construction_args_dispatch<T>(is_pair<T>{}, alloc, std::forward<Args>(args)...);
}

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_USES_ALLOCATOR_CONSTRUCTION_ARGS_HPP
