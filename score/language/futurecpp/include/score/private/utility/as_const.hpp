///
/// \file
/// \copyright Copyright (C) 2017-2018, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Utility Component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_AS_CONST_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_AS_CONST_HPP

#include <type_traits>

namespace score::cpp
{

/// \brief Forms a lvalue reference to const type of t
template <typename T>
constexpr typename std::add_const<T>::type& as_const(T& t) noexcept
{
    return t;
}

template <typename T>
constexpr void as_const(const T&&)
{
    static_assert(!std::is_same<T, T>::value, "Deleted to disallow rvalue arguments.");
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_AS_CONST_HPP
