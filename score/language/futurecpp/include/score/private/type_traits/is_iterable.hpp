///
/// \file
/// \copyright Copyright (C) 2017-2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_ITERABLE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_ITERABLE_HPP

#include <iterator>
#include <type_traits>
#include <utility>

#include <score/private/type_traits/has_operator_indirection.hpp>
#include <score/private/type_traits/has_operator_pre_increment.hpp>
#include <score/private/type_traits/is_detected.hpp>
#include <score/private/type_traits/is_not_equal_comparable.hpp>

namespace score::cpp
{

namespace detail
{

///
/// \brief A helper check for whether the type(iterator) is a copyable or not
///
/// \tparam T: type to be checked
///
/// \see https://en.cppreference.com/w/cpp/named_req/Iterator.html
///
template <typename T>
using copyable_iterator_concept_check =
    std::conjunction<std::is_copy_assignable<T>, std::is_copy_constructible<T>, std::is_destructible<T>>;

///
/// \brief A helper check for whether the type is a conceptually legacy(input)-like iterator or not, please \note the
/// difference between the 'conceptual' check and other std::iterator_traits, this is a check/guarantee for the
/// concept of iterability only.
///
/// \tparam T: type to be checked
///
/// \see https://en.cppreference.com/w/cpp/named_req/Iterator.html
/// \see https://eel.is/c++draft/iterator.traits
///
/// \note the 'is_not_equal_comparable' check is added to guarantee the 'iterable-ness' for some range-based
/// algorithms.
///
template <typename T>
constexpr bool legacy_input_iterator_concept_check_v = std::conjunction_v<score::cpp::is_not_equal_comparable<T>,
                                                                          score::cpp::has_operator_pre_increment<T>,
                                                                          score::cpp::has_operator_indirection<T>,
                                                                          copyable_iterator_concept_check<T>,
                                                                          std::is_swappable<T>>;

///
/// \brief Check whether the type is a legacy(input) iterator
/// the negative case
///
/// \tparam T: type to be checked
///
template <typename T, typename = void>
struct is_legacy_input_iterator : std::false_type
{
};

///
/// \brief Check whether the type is legacy(input) iterator
/// the positive case
///
/// \tparam T: type to be checked
///
template <typename T>
struct is_legacy_input_iterator<T, std::enable_if_t<legacy_input_iterator_concept_check_v<T>>> : std::true_type
{
};

///
/// \brief Check whether the type is a legacy(input) iterator
///
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool is_legacy_input_iterator_v = is_legacy_input_iterator<T>::value;

} // namespace detail

/// \brief Check whether the type is iterable
/// the negative case
/// \tparam T: type to be checked
///
template <typename T, typename = void>
struct is_iterable : std::false_type
{
};

/// \brief Check whether the type is iterable
/// the positive case
/// \tparam T: type to be checked
///
template <typename T>
struct is_iterable<
    T,
    std::enable_if_t<std::conjunction_v<detail::is_legacy_input_iterator<decltype(std::begin(std::declval<T&>()))>,
                                        detail::is_legacy_input_iterator<decltype(std::end(std::declval<T&>()))>>,
                     void>> : std::true_type
{
};

/// \brief Check whether the type is a iterable
/// \tparam T: type to be checked
///
template <typename T>
constexpr auto is_iterable_v = is_iterable<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_ITERABLE_HPP
