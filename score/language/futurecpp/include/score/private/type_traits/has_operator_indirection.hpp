///
/// \file
/// \copyright Copyright (C) 2017-2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_HAS_OPERATOR_INDIRECTION_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_HAS_OPERATOR_INDIRECTION_HPP

#include <type_traits>

#include <score/private/type_traits/is_detected.hpp>

namespace score::cpp
{

namespace detail
{

///
/// \brief Check whether T is dereferenceable
///
/// \tparam T: type to be checked
///
template <typename T>
using has_operator_indirection_check = decltype(*std::declval<T&>());

} // namespace detail

///
/// \brief Check whether the dereference operator can be applied to type T
///
/// \tparam T: type to be checked
///
template <typename T>
using has_operator_indirection = score::cpp::is_detected<detail::has_operator_indirection_check, T>;

///
/// \brief Check whether the dereference operator can be applied to type T
///
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool has_operator_indirection_v = has_operator_indirection<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_HAS_OPERATOR_INDIRECTION_HPP
