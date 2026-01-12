///
/// \file
/// \copyright Copyright (C) 2017-2018, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Utility Component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_TO_UNDERLYING_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_TO_UNDERLYING_HPP

#include <type_traits>

namespace score::cpp
{

/// \brief Converts an enumeration to its underlying type.
///
/// \tparam Enum The enumeration type.
/// \param e Enumeration value to convert.
/// \return The integer value of the underlying type of Enum, converted from e.
template <typename Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_TO_UNDERLYING_HPP
