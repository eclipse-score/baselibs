/********************************************************************************
 * Copyright (c) 2023 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2023 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_MATH_X86_64_SSE42_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_MATH_X86_64_SSE42_HPP

#include <score/private/bit/bit_cast.hpp>
#include <score/private/math/cmath.h>
#include <score/private/math/equals_bitexact.hpp>
#include <cstdint>
#include <limits>
// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4036540
#include <nmmintrin.h> // only include SSE4.2.

// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4049789
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE __attribute__((__always_inline__))

namespace score::cpp
{
namespace detail
{

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE double ceil_sse42(double num)
{
    const __m128d v = _mm_round_sd(_mm_set_sd(num), _mm_set_sd(num), (_MM_FROUND_NO_EXC | _MM_FROUND_TO_POS_INF));
    double r;
    _mm_store_sd(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE float ceil_sse42(float num)
{
    const __m128 v = _mm_round_ss(_mm_set_ss(num), _mm_set_ss(num), (_MM_FROUND_NO_EXC | _MM_FROUND_TO_POS_INF));
    float r;
    _mm_store_ss(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE double floor_sse42(double num)
{
    const __m128d v = _mm_round_sd(_mm_set_sd(num), _mm_set_sd(num), (_MM_FROUND_NO_EXC | _MM_FROUND_TO_NEG_INF));
    double r;
    _mm_store_sd(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE float floor_sse42(float num)
{
    const __m128 v = _mm_round_ss(_mm_set_ss(num), _mm_set_ss(num), (_MM_FROUND_NO_EXC | _MM_FROUND_TO_NEG_INF));
    float r;
    _mm_store_ss(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE double rint_sse42(double num)
{
    const __m128d v = _mm_round_sd(_mm_set_sd(num), _mm_set_sd(num), (_MM_FROUND_RAISE_EXC | _MM_FROUND_CUR_DIRECTION));
    double r;
    _mm_store_sd(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE float rint_sse42(float num)
{
    const __m128 v = _mm_round_ss(_mm_set_ss(num), _mm_set_ss(num), (_MM_FROUND_RAISE_EXC | _MM_FROUND_CUR_DIRECTION));
    float r;
    _mm_store_ss(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE double trunc_sse42(double num)
{
    const __m128d v = _mm_round_sd(_mm_set_sd(num), _mm_set_sd(num), (_MM_FROUND_NO_EXC | _MM_FROUND_TO_ZERO));
    double r;
    _mm_store_sd(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE float trunc_sse42(float num)
{
    const __m128 v = _mm_round_ss(_mm_set_ss(num), _mm_set_ss(num), (_MM_FROUND_NO_EXC | _MM_FROUND_TO_ZERO));
    float r;
    _mm_store_ss(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE double abs_sse42(double num)
{
    const auto mask = score::cpp::bit_cast<double>(0x7FFFFFFFFFFFFFFFU);
    const __m128d v = _mm_and_pd(_mm_set_sd(num), _mm_set_sd(mask));
    double r;
    _mm_store_sd(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE float abs_sse42(float num)
{
    const auto mask = score::cpp::bit_cast<float>(0x7FFFFFFFU);
    const __m128 v = _mm_and_ps(_mm_set_ss(num), _mm_set_ss(mask));
    float r;
    _mm_store_ss(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE double copysign_sse42(double mag, double sgn)
{
    const auto sign_mask = score::cpp::bit_cast<double>(0x8000000000000000U);
    const __m128d sign_of_sgn = _mm_and_pd(_mm_set_sd(sgn), _mm_set_sd(sign_mask));
    const auto magnitude_mask = score::cpp::bit_cast<double>(0x7FFFFFFFFFFFFFFFU);
    const __m128d magnitude_of_mag = _mm_and_pd(_mm_set_sd(mag), _mm_set_sd(magnitude_mask));
    const __m128d v = _mm_or_pd(magnitude_of_mag, sign_of_sgn);
    double r;
    _mm_store_sd(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE float copysign_sse42(float mag, float sgn)
{
    const auto sign_mask = score::cpp::bit_cast<float>(0x80000000U);
    const __m128 sign_of_sgn = _mm_and_ps(_mm_set_ss(sgn), _mm_set_ss(sign_mask));
    const auto magnitude_mask = score::cpp::bit_cast<float>(0x7FFFFFFFU);
    const __m128 magnitude_of_mag = _mm_and_ps(_mm_set_ss(mag), _mm_set_ss(magnitude_mask));
    const __m128 v = _mm_or_ps(magnitude_of_mag, sign_of_sgn);
    float r;
    _mm_store_ss(&r, v);
    return r;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE bool isfinite_sse42(double num)
{
    const auto inf = score::cpp::bit_cast<std::uint64_t>(std::numeric_limits<double>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint64_t>(num) & 0x7FFFFFFFFFFFFFFFU;
    return abs_num < inf;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE bool isfinite_sse42(float num)
{
    const auto inf = score::cpp::bit_cast<std::uint32_t>(std::numeric_limits<float>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint32_t>(num) & 0x7FFFFFFFU;
    return abs_num < inf;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE bool isinf_sse42(double num)
{
    const auto inf = score::cpp::bit_cast<std::uint64_t>(std::numeric_limits<double>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint64_t>(num) & 0x7FFFFFFFFFFFFFFFU;
    return abs_num == inf;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE bool isinf_sse42(float num)
{
    const auto inf = score::cpp::bit_cast<std::uint32_t>(std::numeric_limits<float>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint32_t>(num) & 0x7FFFFFFFU;
    return abs_num == inf;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE bool isnan_sse42(double num)
{
    const auto inf = score::cpp::bit_cast<std::uint64_t>(std::numeric_limits<double>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint64_t>(num) & 0x7FFFFFFFFFFFFFFFU;
    return inf < abs_num;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE bool isnan_sse42(float num)
{
    const auto inf = score::cpp::bit_cast<std::uint32_t>(std::numeric_limits<float>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint32_t>(num) & 0x7FFFFFFFU;
    return inf < abs_num;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE bool isnormal_sse42(double num)
{
    const auto min = score::cpp::bit_cast<std::uint64_t>(std::numeric_limits<double>::min());
    const auto inf = score::cpp::bit_cast<std::uint64_t>(std::numeric_limits<double>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint64_t>(num) & 0x7FFFFFFFFFFFFFFFU;
    return (abs_num - min) < (inf - min);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE bool isnormal_sse42(float num)
{
    const auto min = score::cpp::bit_cast<std::uint32_t>(std::numeric_limits<float>::min());
    const auto inf = score::cpp::bit_cast<std::uint32_t>(std::numeric_limits<float>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint32_t>(num) & 0x7FFFFFFFU;
    return (abs_num - min) < (inf - min);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE bool signbit_sse42(double num)
{
    return (score::cpp::bit_cast<std::uint64_t>(num) >> 63U) != 0U;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE bool signbit_sse42(float num)
{
    return (score::cpp::bit_cast<std::uint32_t>(num) >> 31U) != 0U;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE double sqrt_sse42(double num)
{
    static_assert(score::cpp::equals_bitexact(-0.0, 0.0), "usage of 0.0 in if-statement assumes equality");
    if (!score::cpp::detail::isnan_sse42(num) && (num >= 0.0))
    {
        double r;
        const __m128d v = _mm_sqrt_sd(_mm_set_sd(num), _mm_set_sd(num));
        _mm_store_sd(&r, v);
        return r;
    }
    return ::score_future_cpp_sqrt(num);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE float sqrt_sse42(float num)
{
    static_assert(score::cpp::equals_bitexact(-0.0F, 0.0F), "usage of 0.0 in if-statement assumes equality");
    if (!score::cpp::detail::isnan_sse42(num) && (num >= 0.0F))
    {
        float r;
        const __m128 v = _mm_sqrt_ss(_mm_set_ss(num));
        _mm_store_ss(&r, v);
        return r;
    }
    return ::score_future_cpp_sqrtf(num);
}

} // namespace detail
} // namespace score::cpp

#undef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_X86_64_SSE42_ALWAYS_INLINE

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_MATH_X86_64_SSE42_HPP
