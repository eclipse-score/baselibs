///
/// \file
/// \copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IN_PLACE_T_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IN_PLACE_T_HPP

namespace score::cpp
{

/// Dispatch type used to construct an optional using direct-initialization.
struct in_place_t
{
    explicit in_place_t() = default;
};

/// Instance of \a in_place_t for use with \a optional.
inline constexpr in_place_t in_place{};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IN_PLACE_T_HPP
