///
/// \file
/// \copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IN_PLACE_TYPE_T_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IN_PLACE_TYPE_T_HPP

namespace score::cpp
{

/// Dispatch type used to construct a variant or any using direct-initialization.
template <typename T>
struct in_place_type_t
{
    explicit in_place_type_t() = default;
};

/// Instance of \a in_place_type_t for use with \a variant or any.
template <typename T>
constexpr in_place_type_t<T> in_place_type{};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IN_PLACE_TYPE_T_HPP
