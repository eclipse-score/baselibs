///
/// \file
/// \copyright Copyright (C) 2017-2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_NOT_EQUAL_COMPARABLE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_NOT_EQUAL_COMPARABLE_HPP

#include <type_traits>

#include <score/private/type_traits/is_detected.hpp>

namespace score::cpp
{

namespace detail
{

///
/// \brief Check whether T is not-equal comparable "!="
///
/// \tparam T: type to be checked
///
template <typename T>
using is_not_equal_comparable_check = decltype(std::declval<T&>() != std::declval<T&>());

} // namespace detail

///
/// \brief Check whether the not-equal comparison "!=" o'tor be applied to T
///
/// \tparam T: type to be checked
///
template <typename T>
using is_not_equal_comparable = score::cpp::is_detected<detail::is_not_equal_comparable_check, T>;

///
/// \brief Check whether the not-equal comparison "!=" op'tor be applied to T
///
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool is_not_equal_comparable_v = is_not_equal_comparable<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_NOT_EQUAL_COMPARABLE_HPP
