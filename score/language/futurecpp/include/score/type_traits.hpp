///
/// \file
/// \copyright Copyright (C) 2017-2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.TypeTraits
///

#ifndef SCORE_LANGUAGE_FUTURECPP_TYPE_TRAITS_HPP
#define SCORE_LANGUAGE_FUTURECPP_TYPE_TRAITS_HPP

#include <score/private/type_traits/conjunction.hpp>                // IWYU pragma: export
#include <score/private/type_traits/has_operator_indirection.hpp>   // IWYU pragma: export
#include <score/private/type_traits/has_operator_pre_increment.hpp> // IWYU pragma: export
#include <score/private/type_traits/invoke_traits.hpp>              // IWYU pragma: export
#include <score/private/type_traits/is_detected.hpp>                // IWYU pragma: export
#include <score/private/type_traits/is_expected.hpp>                // IWYU pragma: export
#include <score/private/type_traits/is_hash_for.hpp>                // IWYU pragma: export
#include <score/private/type_traits/is_inplace_vector.hpp>          // IWYU pragma: export
#include <score/private/type_traits/is_iterable.hpp>                // IWYU pragma: export
#include <score/private/type_traits/is_not_equal_comparable.hpp>    // IWYU pragma: export
#include <score/private/type_traits/is_optional.hpp>                // IWYU pragma: export
#include <score/private/type_traits/is_pair.hpp>                    // IWYU pragma: export
#include <score/private/type_traits/is_span.hpp>                    // IWYU pragma: export
#include <score/private/type_traits/is_static_vector.hpp>           // IWYU pragma: export
#include <score/private/type_traits/is_tuple.hpp>                   // IWYU pragma: export
#include <score/private/type_traits/iterator_category.hpp>          // IWYU pragma: export
#include <score/private/type_traits/remove_cvref.hpp>               // IWYU pragma: export
#include <score/private/type_traits/type_identity.hpp>              // IWYU pragma: export
#include <score/private/type_traits/void_t.hpp>                     // IWYU pragma: export

#include <iterator>
#include <type_traits>

namespace score::cpp
{

namespace detail
{
/// \cond PRIVATE
///
/// \brief Check for presence of method size
///
template <typename T>
using has_size_check = decltype(void(&std::decay_t<T>::size));

///
/// \brief Check for presence of operator+
///
template <typename T>
using has_operator_addition_check = decltype(void(&std::decay_t<T>::operator+));

///
/// \brief Check for presence of operator-
///
template <typename T>
using has_operator_substraction_check = decltype(void(&std::decay_t<T>::operator-));

///
/// \brief Check for presence of operator*
///
template <typename T>
using has_operator_multiplication_check = decltype(void(&std::decay_t<T>::operator*));

///
/// \brief Check for presence of operator/
///
template <typename T>
using has_operator_division_check = decltype(void(&std::decay_t<T>::operator/));

///
/// \brief Check for presence of operator<
///
template <typename T>
using has_operator_less_check = decltype(void(&std::decay_t<T>::operator<));

///
/// \brief Check whether it is less-then-comparable a<b
///
template <typename T>
using is_less_then_comparable_check = decltype(std::declval<T>() < std::declval<T>());

///
/// \brief Check whether it is less-then-comparable a<=b
///
template <typename T>
using is_less_than_or_equal_to_comparable_check = decltype(void(std::declval<T>() <= std::declval<T>()));

///
/// \brief Check whether the provided iterator is mutable
///
template <typename, typename = void>
struct is_mutable_iterator : std::false_type
{
};

template <typename I>
struct is_mutable_iterator<
    I,
    void_t<typename std::iterator_traits<I>::iterator_category,
           decltype(*std::declval<I>() = std::declval<typename std::iterator_traits<I>::value_type>())>>
    : std::true_type
{
};

/// \endcond
} // namespace detail

///
/// \brief Check for presence of method size
/// \tparam T: type to be checked
///
template <typename T>
using has_size = is_detected<detail::has_size_check, T>;

///
/// \brief Check for presence of method size
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool has_size_v = has_size<T>::value;

///
/// \brief Check for presence of operator+
/// \tparam T: type to be checked
///
template <typename T>
using has_operator_addition = is_detected<detail::has_operator_addition_check, T>;

///
/// \brief Check for presence of operator+
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool has_operator_addition_v = has_operator_addition<T>::value;

///
/// \brief Check for presence of operator+
/// \tparam T: type to be checked
///
template <typename T>
using has_operator_subtraction = is_detected<detail::has_operator_substraction_check, T>;

///
/// \brief Check for presence of operator-
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool has_operator_subtraction_v = has_operator_subtraction<T>::value;

///
/// \brief Check for presence of operator*
/// \tparam T: type to be checked
///
template <typename T>
using has_operator_multiplication = is_detected<detail::has_operator_multiplication_check, T>;

///
/// \brief Check for presence of operator*
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool has_operator_multiplication_v = has_operator_multiplication<T>::value;

///
/// \brief Check for presence of operator/
/// \tparam T: type to be checked
///
template <typename T>
using has_operator_division = is_detected<detail::has_operator_division_check, T>;

///
/// \brief Check for presence of operator/
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool has_operator_division_v = has_operator_division<T>::value;

///
/// \brief Check for presence of operator<
/// \tparam T: type to be checked
///
template <typename T>
using has_operator_less = is_detected<detail::has_operator_less_check, T>;

///
/// \brief Check for presence of operator<
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool has_operator_less_v = has_operator_less<T>::value;

///
/// \brief Check whether the expression a < b compiles if a and b are both objects of type T
///
/// \tparam T: type to be checked
///
template <typename T>
using is_less_than_comparable = is_detected<detail::is_less_then_comparable_check, T>;

///
/// \brief Check whether the expression a < b compiles if a and b are both objects of type T
///
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool is_less_than_comparable_v = is_less_than_comparable<T>::value;

///
/// \brief Check whether the expression a <= b compiles if a and b are both objects of type T
///
/// \tparam T: type to be checked
///
template <typename T>
using is_less_than_or_equal_to_comparable = is_detected<detail::is_less_than_or_equal_to_comparable_check, T>;

///
/// \brief Check whether the expression a <= b compiles if a and b are both objects of type T
///
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool is_less_than_or_equal_to_comparable_v = is_less_than_or_equal_to_comparable<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_TYPE_TRAITS_HPP
