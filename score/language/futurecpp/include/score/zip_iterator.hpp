/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// @copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.ZipIterator component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_ZIP_ITERATOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_ZIP_ITERATOR_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>

#include <score/private/type_traits/conjunction.hpp>
#include <score/private/type_traits/iterator_category.hpp>
#include <score/apply.hpp>
#include <score/for_each_tuple_element.hpp>
#include <score/functional.hpp>
#include <score/range_pair.hpp>
#include <score/type_traits.hpp>
#include <score/utility.hpp>

namespace score::cpp
{

namespace detail
{

/// Wrapper class to allow ADL overload of `swap` for the reference type
template <typename... Ts>
class zipped_tuple_like : public std::tuple<Ts...>
{
    using Base = std::tuple<Ts...>;

    template <typename U>
    using is_forwarding_ref_overload = std::is_same<zipped_tuple_like, score::cpp::remove_cvref_t<U>>;

public:
    using Base::Base;

    template <typename... Us, typename = std::enable_if_t<!is_forwarding_ref_overload<zipped_tuple_like<Us...>>::value>>
    zipped_tuple_like(const zipped_tuple_like<Us...>& other) : Base{static_cast<const std::tuple<Us...>&>(other)}
    {
    }
    template <typename... Us, typename = std::enable_if_t<!is_forwarding_ref_overload<zipped_tuple_like<Us...>>::value>>
    zipped_tuple_like(zipped_tuple_like<Us...>&& other) : Base{static_cast<std::tuple<Us...>&&>(std::move(other))}
    {
    }

    template <typename Alloc,
              typename... Us,
              typename = std::enable_if_t<!is_forwarding_ref_overload<zipped_tuple_like<Us...>>::value>>
    zipped_tuple_like(std::allocator_arg_t, const Alloc& a, const zipped_tuple_like<Us...>& other)
        : Base{std::allocator_arg, a, static_cast<const std::tuple<Us...>&>(other)}
    {
    }
    template <typename Alloc,
              typename... Us,
              typename = std::enable_if_t<!is_forwarding_ref_overload<zipped_tuple_like<Us...>>::value>>
    zipped_tuple_like(std::allocator_arg_t, const Alloc& a, zipped_tuple_like<Us...>&& other)
        : Base{std::allocator_arg, a, static_cast<std::tuple<Us...>&&>(std::move(other))}
    {
    }

    using Base::operator=;

    template <typename... Us, typename = std::enable_if_t<!is_forwarding_ref_overload<zipped_tuple_like<Us...>>::value>>
    zipped_tuple_like& operator=(const zipped_tuple_like<Us...>& other)
    {
        score::cpp::ignore = Base::operator=(static_cast<const std::tuple<Us...>&>(other));
        return *this;
    }
    template <typename... Us, typename = std::enable_if_t<!is_forwarding_ref_overload<zipped_tuple_like<Us...>>::value>>
    zipped_tuple_like& operator=(zipped_tuple_like<Us...>&& other)
    {
        score::cpp::ignore = Base::operator=(static_cast<std::tuple<Us...>&&>(std::move(other)));
        return *this;
    }

    Base& base() { return *this; }
    const Base& base() const { return *this; }
};

template <typename... Ts>
void swap(zipped_tuple_like<Ts...> lhs, zipped_tuple_like<Ts...> rhs)
{
    std::swap(lhs.base(), rhs.base());
}

template <typename Iterators, std::size_t... Idxs>
class zip_iterator_impl;

template <typename... Iterators, std::size_t... Idxs>
class zip_iterator_impl<std::tuple<Iterators...>, Idxs...>
{
    static_assert(sizeof...(Iterators) > 0U, "Empty zip iterator not supported");

    using iterator_tuple = std::tuple<Iterators...>;

    using are_random_access_iterator = conjunction<is_random_access_iterator<Iterators>...>;

    using are_bidirectional_iterator = conjunction<is_bidirectional_iterator<Iterators>...>;

    using are_forward_iterator = conjunction<is_forward_iterator<Iterators>...>;

public:
    using value_type = zipped_tuple_like<typename std::iterator_traits<Iterators>::value_type...>;
    using reference = zipped_tuple_like<typename std::iterator_traits<Iterators>::reference...>;
    using pointer = zipped_tuple_like<typename std::iterator_traits<Iterators>::pointer...>;

    using difference_type = std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;

    using iterator_category = std::conditional_t<
        are_random_access_iterator::value,
        std::random_access_iterator_tag,
        std::conditional_t<
            are_bidirectional_iterator::value,
            std::bidirectional_iterator_tag,
            std::conditional_t<are_forward_iterator::value, std::forward_iterator_tag, std::input_iterator_tag>>>;

    zip_iterator_impl(const iterator_tuple& its) : current_iterators_{its} {}

    zip_iterator_impl()
    {
        static_assert(conjunction_v<std::is_default_constructible<Iterators>...>,
                      "default constructor is only available starting with forward iterators!");
    }

    zip_iterator_impl& operator++()
    {
        score::cpp::for_each_tuple_element(current_iterators_, [](auto& iter) { ++iter; });
        return *this;
    }
    zip_iterator_impl operator++(int)
    {
        const zip_iterator_impl copy{*this};
        ++(*this);
        return copy;
    }

    reference operator*() const { return {*std::get<Idxs>(current_iterators_)...}; }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_input_iterator<I>::value>>
    friend bool operator==(const zip_iterator_impl& lhs, const zip_iterator_impl& rhs)
    {
        return lhs.current_iterators_ == rhs.current_iterators_;
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_input_iterator<I>::value>>
    friend bool operator!=(const zip_iterator_impl& lhs, const zip_iterator_impl& rhs)
    {
        return lhs.current_iterators_ != rhs.current_iterators_;
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_bidirectional_iterator<I>::value>>
    zip_iterator_impl& operator--()
    {
        for_each_tuple_element(current_iterators_, [](auto& iter) { --iter; });
        return *this;
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_bidirectional_iterator<I>::value>>
    zip_iterator_impl operator--(int)
    {
        const zip_iterator_impl copy{*this};
        --(*this);
        return copy;
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend bool operator<(const zip_iterator_impl& lhs, const zip_iterator_impl& rhs)
    {
        const std::array<bool, sizeof...(Iterators)> condition{
            (std::get<Idxs>(lhs.current_iterators_) < std::get<Idxs>(rhs.current_iterators_))...};
        return std::all_of(condition.begin(), condition.end(), identity{});
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend bool operator<=(const zip_iterator_impl& lhs, const zip_iterator_impl& rhs)

    {
        const std::array<bool, sizeof...(Iterators)> condition{
            (std::get<Idxs>(lhs.current_iterators_) <= std::get<Idxs>(rhs.current_iterators_))...};
        return std::all_of(condition.begin(), condition.end(), identity{});
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend bool operator>(const zip_iterator_impl& lhs, const zip_iterator_impl& rhs)

    {
        const std::array<bool, sizeof...(Iterators)> condition{
            (std::get<Idxs>(lhs.current_iterators_) > std::get<Idxs>(rhs.current_iterators_))...};
        return std::all_of(condition.begin(), condition.end(), identity{});
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend bool operator>=(const zip_iterator_impl& lhs, const zip_iterator_impl& rhs)
    {
        const std::array<bool, sizeof...(Iterators)> condition{
            (std::get<Idxs>(lhs.current_iterators_) >= std::get<Idxs>(rhs.current_iterators_))...};
        return std::all_of(condition.begin(), condition.end(), identity{});
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend zip_iterator_impl operator+(const zip_iterator_impl& self, const difference_type n)
    {
        return zip_iterator_impl{self.current_iterators_} += n;
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend zip_iterator_impl operator+(const difference_type n, const zip_iterator_impl& self)
    {
        return self + n;
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend zip_iterator_impl operator-(const zip_iterator_impl& self, const difference_type n)
    {
        return zip_iterator_impl{self.current_iterators_} -= n;
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend difference_type operator-(const zip_iterator_impl& lhs, const zip_iterator_impl& rhs)
    {
        const std::array<difference_type, sizeof...(Iterators)> distances{static_cast<difference_type>(
            std::get<Idxs>(lhs.current_iterators_) - std::get<Idxs>(rhs.current_iterators_))...};
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(std::adjacent_find(distances.begin(), distances.end(), std::not_equal_to<>{}) ==
                                     distances.end(),
                                 "inconsistent distances is not supported");
        return distances.front();
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    zip_iterator_impl& operator+=(const difference_type n)
    {
        score::cpp::for_each_tuple_element(current_iterators_, [n](auto& iter) {
            using actual_difference_type = typename std::iterator_traits<std::decay_t<decltype(iter)>>::difference_type;
            iter += narrow_cast<actual_difference_type>(n);
        });
        return *this;
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    zip_iterator_impl& operator-=(const difference_type n)
    {
        score::cpp::for_each_tuple_element(current_iterators_, [n](auto& iter) {
            using actual_difference_type = typename std::iterator_traits<std::decay_t<decltype(iter)>>::difference_type;
            iter -= narrow_cast<actual_difference_type>(n);
        });
        return *this;
    }

    template <typename I = zip_iterator_impl, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    reference operator[](const difference_type n) const
    {
        return {std::get<Idxs>(current_iterators_)[narrow_cast<
            typename std::iterator_traits<std::tuple_element_t<Idxs, iterator_tuple>>::difference_type>(n)]...};
    }

    const iterator_tuple& get_iterator_tuple() const { return current_iterators_; }

private:
    iterator_tuple current_iterators_;
};

template <typename... Iterators, std::size_t... Idxs>
zip_iterator_impl<std::tuple<Iterators...>, Idxs...> zip_iterator_helper(const std::tuple<Iterators...>& iterators,
                                                                         std::index_sequence<Idxs...> /*unused*/)
{
    return {iterators};
}

template <typename... ContainerSizes>
std::ptrdiff_t minimum_container_size(const ContainerSizes... container_sizes)
{
    static_assert(sizeof...(ContainerSizes) > 0, "must at least contain one container");
    const std::array<std::ptrdiff_t, sizeof...(container_sizes)> sizes{static_cast<ptrdiff_t>(container_sizes)...};
    return *std::min_element(sizes.begin(), sizes.end());
}

} // namespace detail

/// \brief Implementation of an iterator wrapper which wraps arbitrary number of iterators and provides consistent
/// operations on them
///
/// \warning When specifying a range of iterators make sure they have the same range, otherwise a `begin != end` logic
/// might never terminate
///
/// \see Helper functions make_sized_zip_range(...) and make_zip_range(...)
///
/// \tparam Iterators Types which should be wrapped in the zip_iterator
template <typename... Iterators>
using zip_iterator = decltype(score::cpp::detail::zip_iterator_helper(std::declval<std::tuple<Iterators...>>(),
                                                               std::index_sequence_for<Iterators...>()));

/// \brief Helper to setup a consistent zipped range using the desired size
///
/// As the zip_iterator does not allow iterators with different ranges this helper function provides a safe
/// way to generate begin/end pairs for the given containers with different sizes, using the provided size.
///
/// \pre range_size >= 0
/// \pre range_size <= min(size(containers)...)
///
/// \tparam Containers The container types that shall be used to generate the zip_iterator based range
/// \param range_size The size of the range which shall be extracted from the container
/// \param containers The container instances that shall be used to generate the zip_iterator based range
/// \return A range pair of "begin" and "end" zip_iterators generated from the provided containers
template <typename... Containers,
          typename ZipIteratorType = zip_iterator<decltype(std::begin(std::declval<Containers>()))...>>
auto make_sized_zip_range(const std::ptrdiff_t range_size, Containers&&... containers) -> range_pair<ZipIteratorType>
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(range_size >= 0, "size must be positive");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(range_size <= score::cpp::detail::minimum_container_size(score::cpp::ssize(containers)...));

    const auto first = score::cpp::detail::zip_iterator_helper(std::make_tuple(std::begin(containers)...),
                                                        std::index_sequence_for<Containers...>());
    return score::cpp::make_range_pair(first, std::next(first, range_size));
}

/// \brief Helper to setup a consistent zipped range using the range of the smallest container
///
/// As the zip_iterator does not allow iterators with different ranges this helper function provides a safe
/// way to generate begin/end pairs for the given containers with different sizes
///
/// \tparam Containers The container types that shall be used to generate the zip_iterator based range
/// \param containers The container instances that shall be used to generate the zip_iterator based range
/// \return A range pair of "begin" and "end" zip_iterators generated from the provided containers
template <typename... Containers,
          typename ZipIteratorType = zip_iterator<decltype(std::begin(std::declval<Containers>()))...>>
auto make_zip_range(Containers&&... containers) -> range_pair<ZipIteratorType>
{
    const auto size = score::cpp::detail::minimum_container_size(score::cpp::ssize(containers)...);
    return score::cpp::make_sized_zip_range(size, std::forward<Containers>(containers)...);
}

} // namespace score::cpp

namespace std
{
template <std::size_t I, typename... Ts>
struct tuple_element<I, score::cpp::detail::zipped_tuple_like<Ts...>>
{
    using type = std::tuple_element_t<I, std::tuple<Ts...>>;
};

template <typename... Ts>
struct tuple_size<score::cpp::detail::zipped_tuple_like<Ts...>>
    : public std::integral_constant<std::size_t, std::tuple_size<std::tuple<Ts...>>::value>
{
};

} // namespace std

#endif // SCORE_LANGUAGE_FUTURECPP_ZIP_ITERATOR_HPP
