/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Math component:
///  The component contains math constants and functions.
///

#ifndef SCORE_LANGUAGE_FUTURECPP_MATH_HPP
#define SCORE_LANGUAGE_FUTURECPP_MATH_HPP

#include <score/private/math/div_round_closest.hpp> // IWYU pragma: export
#include <score/private/math/equals_bitexact.hpp>   // IWYU pragma: export

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>

#include <score/private/math/cmath.h>
#include <score/private/numeric/lerp.hpp>
#include <score/assert.hpp>
#include <score/bit.hpp>
#include <score/math_constants.hpp>

#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
#include <score/private/math/x86_64_sse42.hpp>
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
#include <score/private/math/aarch64_neon.hpp>
#endif

namespace score::cpp
{

inline double acos(double num) { return ::score_future_cpp_acos(num); }
inline float acos(float num) { return ::score_future_cpp_acosf(num); }

inline double asin(double num) { return ::score_future_cpp_asin(num); }
inline float asin(float num) { return ::score_future_cpp_asinf(num); }

inline double atan2(double y, double x) { return ::score_future_cpp_atan2(y, x); }
inline float atan2(float y, float x) { return ::score_future_cpp_atan2f(y, x); }

inline double atan(double num) { return ::score_future_cpp_atan(num); }
inline float atan(float num) { return ::score_future_cpp_atanf(num); }

inline double cos(double num) { return ::score_future_cpp_cos(num); }
inline float cos(float num) { return ::score_future_cpp_cosf(num); }

inline double sin(double num) { return ::score_future_cpp_sin(num); }
inline float sin(float num) { return ::score_future_cpp_sinf(num); }

inline double tan(double num) { return ::score_future_cpp_tan(num); }
inline float tan(float num) { return ::score_future_cpp_tanf(num); }

inline double acosh(double num) { return ::score_future_cpp_acosh(num); }
inline float acosh(float num) { return ::score_future_cpp_acoshf(num); }

inline double asinh(double num) { return ::score_future_cpp_asinh(num); }
inline float asinh(float num) { return ::score_future_cpp_asinhf(num); }

inline double atanh(double num) { return ::score_future_cpp_atanh(num); }
inline float atanh(float num) { return ::score_future_cpp_atanhf(num); }

inline double cosh(double num) { return ::score_future_cpp_cosh(num); }
inline float cosh(float num) { return ::score_future_cpp_coshf(num); }

inline double sinh(double num) { return ::score_future_cpp_sinh(num); }
inline float sinh(float num) { return ::score_future_cpp_sinhf(num); }

inline double tanh(double num) { return ::score_future_cpp_tanh(num); }
inline float tanh(float num) { return ::score_future_cpp_tanhf(num); }

inline int ilogb(double num) { return ::score_future_cpp_ilogb(num); }
inline int ilogb(float num) { return ::score_future_cpp_ilogbf(num); }

inline double log10(double num) { return ::score_future_cpp_log10(num); }
inline float log10(float num) { return ::score_future_cpp_log10f(num); }

inline double log1p(double num) { return ::score_future_cpp_log1p(num); }
inline float log1p(float num) { return ::score_future_cpp_log1pf(num); }

inline double log2(double num) { return ::score_future_cpp_log2(num); }
inline float log2(float num) { return ::score_future_cpp_log2f(num); }

inline double logb(double num) { return ::score_future_cpp_logb(num); }
inline float logb(float num) { return ::score_future_cpp_logbf(num); }

inline double log(double num) { return ::score_future_cpp_log(num); }
inline float log(float num) { return ::score_future_cpp_logf(num); }

inline double fmax(double x, double y)
{
#if defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::fmax_neon(x, y);
#else
    return ::score_future_cpp_fmax(x, y);
#endif
}
inline float fmax(float x, float y)
{
#if defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::fmax_neon(x, y);
#else
    return ::score_future_cpp_fmaxf(x, y);
#endif
}

inline double fmin(double x, double y)
{
#if defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::fmin_neon(x, y);
#else
    return ::score_future_cpp_fmin(x, y);
#endif
}
inline float fmin(float x, float y)
{
#if defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::fmin_neon(x, y);
#else
    return ::score_future_cpp_fminf(x, y);
#endif
}

inline double ceil(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::ceil_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::ceil_neon(num);
#else
    return ::score_future_cpp_ceil(num);
#endif
}
inline float ceil(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::ceil_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::ceil_neon(num);
#else
    return ::score_future_cpp_ceilf(num);
#endif
}

inline double floor(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::floor_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::floor_neon(num);
#else
    return ::score_future_cpp_floor(num);
#endif
}
inline float floor(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::floor_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::floor_neon(num);
#else
    return ::score_future_cpp_floorf(num);
#endif
}

inline long lrint(double num) { return ::score_future_cpp_lrint(num); }
inline long lrint(float num) { return ::score_future_cpp_lrintf(num); }

inline long lround(double num) { return ::score_future_cpp_lround(num); }
inline long lround(float num) { return ::score_future_cpp_lroundf(num); }

inline double nearbyint(double num) { return ::score_future_cpp_nearbyint(num); }
inline float nearbyint(float num) { return ::score_future_cpp_nearbyintf(num); }

inline double nextafter(double from, double to) { return ::score_future_cpp_nextafter(from, to); }
inline float nextafter(float from, float to) { return ::score_future_cpp_nextafterf(from, to); }

inline double nexttoward(double from, double to) { return ::score_future_cpp_nexttoward(from, to); }
inline float nexttoward(float from, float to) { return ::score_future_cpp_nexttowardf(from, to); }

inline double rint(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::rint_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::rint_neon(num);
#else
    return ::score_future_cpp_rint(num);
#endif
}
inline float rint(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::rint_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::rint_neon(num);
#else
    return ::score_future_cpp_rintf(num);
#endif
}

inline double round(double num)
{
#if defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::round_neon(num);
#else
    return ::score_future_cpp_round(num);
#endif
}
inline float round(float num)
{
#if defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::round_neon(num);
#else
    return ::score_future_cpp_roundf(num);
#endif
}

inline double trunc(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::trunc_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::trunc_neon(num);
#else
    return ::score_future_cpp_trunc(num);
#endif
}
inline float trunc(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::trunc_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::trunc_neon(num);
#else
    return ::score_future_cpp_truncf(num);
#endif
}

inline double erfc(double num) { return ::score_future_cpp_erfc(num); }
inline float erfc(float num) { return ::score_future_cpp_erfcf(num); }

inline double erf(double num) { return ::score_future_cpp_erf(num); }
inline float erf(float num) { return ::score_future_cpp_erff(num); }

inline double cbrt(double num) { return ::score_future_cpp_cbrt(num); }
inline float cbrt(float num) { return ::score_future_cpp_cbrtf(num); }

inline double exp2(double num) { return ::score_future_cpp_exp2(num); }
inline float exp2(float num) { return ::score_future_cpp_exp2f(num); }

inline double expm1(double num) { return ::score_future_cpp_expm1(num); }
inline float expm1(float num) { return ::score_future_cpp_expm1f(num); }

inline double exp(double num) { return ::score_future_cpp_exp(num); }
inline float exp(float num) { return ::score_future_cpp_expf(num); }
template <typename Integer, typename std::enable_if_t<std::is_integral<Integer>::value, bool> = false>
inline double exp(Integer num)
{
    return score::cpp::exp(static_cast<double>(num));
}

/// \brief Computes the power of base to constant expression exponent N

/// \pre N <= 6

/// \tparam N is the exponent of type unsigned integer.
/// \tparam T Floating point type.
/// \param base The base for the power operation.
/// \return Power of base to N. Might return nan/inf/-inf.
template <const std::uint32_t N, typename T>
inline T int_pow(T base)
{
    static_assert(N <= 6, "N must be less than or equal to 6");
    static_assert(std::is_floating_point<T>::value, "T must be a floating point type");

    if (N == 0)
    {
        return T{1};
    }
    else if ((N % 2) == 1)
    {
        return base * int_pow<N / 2>(base * base);
    }
    else
    {
        return int_pow<N / 2>(base * base);
    }
}

inline double pow(double base, double exp) { return ::score_future_cpp_pow(base, exp); }
inline float pow(float base, float exp) { return ::score_future_cpp_powf(base, exp); }

inline double sqrt(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::sqrt_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::sqrt_neon(num);
#else
    return ::score_future_cpp_sqrt(num);
#endif
}
inline float sqrt(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::sqrt_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::sqrt_neon(num);
#else
    return ::score_future_cpp_sqrtf(num);
#endif
}
template <typename Integer, typename std::enable_if_t<std::is_integral<Integer>::value, bool> = false>
inline double sqrt(Integer num)
{
    return score::cpp::sqrt(static_cast<double>(num));
}

inline double lgamma(double num) { return ::score_future_cpp_lgamma(num); }
inline float lgamma(float num) { return ::score_future_cpp_lgammaf(num); }

inline double tgamma(double num) { return ::score_future_cpp_tgamma(num); }
inline float tgamma(float num) { return ::score_future_cpp_tgammaf(num); }

inline double fmod(double x, double y) { return ::score_future_cpp_fmod(x, y); }
inline float fmod(float x, float y) { return ::score_future_cpp_fmodf(x, y); }

inline double modf(double num, double* iptr) { return ::score_future_cpp_modf(num, iptr); }
inline float modf(float num, float* iptr) { return ::score_future_cpp_modff(num, iptr); }

inline double remainder(double x, double y) { return ::score_future_cpp_remainder(x, y); }
inline float remainder(float x, float y) { return ::score_future_cpp_remainderf(x, y); }

inline double remquo(double x, double y, int* quo) { return ::score_future_cpp_remquo(x, y, quo); }
inline float remquo(float x, float y, int* quo) { return ::score_future_cpp_remquof(x, y, quo); }

inline double copysign(double mag, double sgn)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::copysign_sse42(mag, sgn);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::copysign_neon(mag, sgn);
#else
    return ::score_future_cpp_copysign(mag, sgn);
#endif
}
inline float copysign(float mag, float sgn)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::copysign_sse42(mag, sgn);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::copysign_neon(mag, sgn);
#else
    return ::score_future_cpp_copysignf(mag, sgn);
#endif
}

inline int abs(int num) { return std::abs(num); }
inline long int abs(long int num) { return std::abs(num); }
inline long long int abs(long long int num) { return std::abs(num); }
inline double abs(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::abs_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::abs_neon(num);
#else
    return ::score_future_cpp_fabs(num);
#endif
}
inline float abs(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::abs_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::abs_neon(num);
#else
    return ::score_future_cpp_fabsf(num);
#endif
}

inline double fabs(double arg)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::abs_sse42(arg);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::abs_neon(arg);
#else
    return ::score_future_cpp_fabs(arg);
#endif
}
inline float fabs(float arg)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::abs_sse42(arg);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::abs_neon(arg);
#else
    return ::score_future_cpp_fabsf(arg);
#endif
}

inline double fdim(double x, double y) { return ::score_future_cpp_fdim(x, y); }
inline float fdim(float x, float y) { return ::score_future_cpp_fdimf(x, y); }

inline double fma(double x, double y, double z) { return ::score_future_cpp_fma(x, y, z); }
inline float fma(float x, float y, float z) { return ::score_future_cpp_fmaf(x, y, z); }

inline double frexp(double num, int* exp) { return ::score_future_cpp_frexp(num, exp); }
inline float frexp(float num, int* exp) { return ::score_future_cpp_frexpf(num, exp); }

inline double hypot(double x, double y) { return ::score_future_cpp_hypot(x, y); }
inline float hypot(float x, float y) { return ::score_future_cpp_hypotf(x, y); }

inline double ldexp(double num, int exp) { return ::score_future_cpp_ldexp(num, exp); }
inline float ldexp(float num, int exp) { return ::score_future_cpp_ldexpf(num, exp); }

inline double nan(const char* arg) { return ::score_future_cpp_nan(arg); }
inline float nanf(const char* arg) { return ::score_future_cpp_nanf(arg); }

inline double scalbln(double num, int exp) { return ::score_future_cpp_scalbln(num, exp); }
inline float scalbln(float num, int exp) { return ::score_future_cpp_scalblnf(num, exp); }

inline double scalbn(double num, int exp) { return ::score_future_cpp_scalbn(num, exp); }
inline float scalbn(float num, int exp) { return ::score_future_cpp_scalbnf(num, exp); }

inline int fpclassify(double num) { return ::score_future_cpp_fpclassify(num); }
inline int fpclassify(float num) { return ::score_future_cpp_fpclassifyf(num); }

template <typename T, typename std::enable_if_t<std::is_integral<T>::value, bool> = false>
inline bool isfinite(T /*num*/)
{
    return true;
}
inline bool isfinite(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isfinite_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isfinite_neon(num);
#else
    return ::score_future_cpp_isfinite(num) != 0;
#endif
}
inline bool isfinite(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isfinite_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isfinite_neon(num);
#else
    return ::score_future_cpp_isfinitef(num) != 0;
#endif
}

inline bool isinf(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isinf_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isinf_neon(num);
#else
    return ::score_future_cpp_isinf(num) != 0;
#endif
}
inline bool isinf(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isinf_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isinf_neon(num);
#else
    return ::score_future_cpp_isinff(num) != 0;
#endif
}

inline bool isnan(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isnan_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isnan_neon(num);
#else
    return ::score_future_cpp_isnan(num) != 0;
#endif
}
inline bool isnan(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isnan_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isnan_neon(num);
#else
    return ::score_future_cpp_isnanf(num) != 0;
#endif
}

inline bool isnormal(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isnormal_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isnormal_neon(num);
#else
    return ::score_future_cpp_isnormal(num) != 0;
#endif
}
inline bool isnormal(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isnormal_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::isnormal_neon(num);
#else
    return ::score_future_cpp_isnormalf(num) != 0;
#endif
}

template <typename T, typename std::enable_if_t<std::is_integral<T>::value, bool> = false>
inline bool signbit(T num)
{
    return num < 0;
}
inline bool signbit(double num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::signbit_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::signbit_neon(num);
#else
    return ::score_future_cpp_signbit(num) != 0;
#endif
}
inline bool signbit(float num)
{
#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::signbit_sse42(num);
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
    return score::cpp::detail::signbit_neon(num);
#else
    return ::score_future_cpp_signbitf(num) != 0;
#endif
}

inline bool isgreater(double x, double y) { return ::score_future_cpp_isgreater(x, y) != 0; }
inline bool isgreater(float x, float y) { return ::score_future_cpp_isgreaterf(x, y) != 0; }

inline bool isgreaterequal(double x, double y) { return ::score_future_cpp_isgreaterequal(x, y) != 0; }
inline bool isgreaterequal(float x, float y) { return ::score_future_cpp_isgreaterequalf(x, y) != 0; }

inline bool isless(double x, double y) { return ::score_future_cpp_isless(x, y) != 0; }
inline bool isless(float x, float y) { return ::score_future_cpp_islessf(x, y) != 0; }

inline bool islessequal(double x, double y) { return ::score_future_cpp_islessequal(x, y) != 0; }
inline bool islessequal(float x, float y) { return ::score_future_cpp_islessequalf(x, y) != 0; }

inline bool islessgreater(double x, double y) { return ::score_future_cpp_islessgreater(x, y) != 0; }
inline bool islessgreater(float x, float y) { return ::score_future_cpp_islessgreaterf(x, y) != 0; }

inline bool isunordered(double x, double y) { return ::score_future_cpp_isunordered(x, y) != 0; }
inline bool isunordered(float x, float y) { return ::score_future_cpp_isunorderedf(x, y) != 0; }

/// \brief Checks floats for equality.
///
/// The function compares two floats for equality by considering the machine precision. The machine epsilon is scaled to
/// the same order of magnitude as the values, i.e., with the absolute maximum of both values.
///
/// Source: https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
/// Last accessed 25.07.2016
///
/// \tparam T Floating point type.
/// \param lhs First float value that is compared.
/// \param rhs Second float value that is compared.
/// \param max_rel_diff Maximum relative difference of the two floats. Relative means that max_rel_diff is multiplied
/// with the absolute maximum of the two numbers to be in the same order of magnitude as the numbers. Default is the
/// floating point epsilon, i.e., std::numeric_limits<float>::epsilon().
/// \return True, if the two floats are almost equal, false otherwise.
template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
constexpr bool equals(const T lhs, const T rhs, const T max_rel_diff = std::numeric_limits<T>::epsilon())
{
    if (!(score::cpp::isfinite(lhs) && score::cpp::isfinite(rhs)))
    {
        return score::cpp::equals_bitexact(lhs, rhs);
    }
    const T diff{score::cpp::abs(lhs - rhs)};
    const bool almost_zero{diff <= std::numeric_limits<T>::min()};
    const bool almost_equal{diff <= (std::max(score::cpp::abs(lhs), score::cpp::abs(rhs)) * max_rel_diff)};
    return almost_zero || almost_equal;
}

/// \brief Computes the floating-point remainder of the division operation \f$ \frac{value}{m} \f$.
///
/// This version is described by "Knuth, Donald. E. (1972). The Art of Computer Programming. Addison-Wesley.". It uses a
/// floored division where the quotient is defined by the floor function \f$ q =  \lfloor\frac{value}{m}\rfloor \f$.
///
/// float negative modulo formula: \f$ value - m \lfloor\frac{value}{m}\rfloor \f$
///
/// Note that std::fmod uses a truncated division: \f$ value - m \cdot trunc(\frac{value}{m}) \f$
///
/// Despite its widespread use, truncated division is shown to be inferior to the other definitions.
/// http://research.microsoft.com/pubs/151917/divmodnote.pdf
///
/// \pre score::cpp::abs(m) > std::numeric_limits<T>::min()
///
/// \tparam T Floating point type.
/// \param value Value which should be constrained with respect to given modulus
/// \param m Modulus. Not allowed to be zero!
/// \return The constrained given value within the interval [0, m). If m is zero, the function returns std::nan.
template <typename T>
constexpr T fmod_floored(const T value, const T m)
{
    static_assert(std::is_floating_point<T>::value, "Type need to be floating point.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(score::cpp::abs(m) > std::numeric_limits<T>::min());
    return value - (m * score::cpp::floor(value / m));
}

namespace math
{

///
/// \brief Convert an angle in \f$\text{deg}\f$ to \f$\text{rad}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost.
///
/// \param deg angle in \f$\text{deg}\f$
///
/// \tparam T value type
///
/// \asil QM
///
/// \return angle in \f$\text{rad}\f$
///
template <class T>
constexpr T deg_to_rad(const T& deg)
{
    return deg * constants::detail::constant_deg_to_rad<T>::get();
}

///
/// \brief Convert an angle in \f$\text{rad}\f$ to \f$\text{deg}\f$ for the value type \c T,
///
/// \note
///  If the value type \c T is not a floating point type, then the compiler should produce a warning and the accuracy is
///  lost.
///
/// \param rad angle in \f$\text{rad}\f$
///
/// \tparam T value type
///
/// \asil QM
///
/// \return angle in \f$\text{deg}\f$
///
template <class T>
constexpr T rad_to_deg(const T& rad)
{
    return rad * constants::detail::constant_rad_to_deg<T>::get();
}

/// \brief Checks if the passed integer is a power of two.
///
/// \param value The integer to check if it equals 2^i (i ranging from 0 to 30).
/// \return True if the passed integer equals 2^i (i ranging from 0 to 30).
constexpr bool is_power_of_two(const std::int32_t value)
{
    return (value > 0) && has_single_bit(static_cast<std::uint32_t>(value));
}

/// \brief Remaps angle to the interval [-pi, pi) using a modulus operation.
///
/// Formula: angle - 2 * pi * floor(angle / (2 * pi) + 0.5)
///
/// \tparam T Floating point type.
/// \param angle Value to wrap.
/// \return Radians in the interval [-pi, pi)
template <typename T>
constexpr T wrap_to_pi(const T angle)
{
    static_assert(std::is_floating_point<T>::value, "Type need to be floating point.");
    const T two_pi = score::cpp::math::constants::two_pi<T>();
    const T half = score::cpp::math::constants::half<T>();
    T multiple = angle / two_pi;
    multiple = score::cpp::floor(multiple + half);
    return angle - (two_pi * multiple);
}

/// \brief Remaps angle to the interval [0, 2*pi) using a modulus operation.
///
/// Formula: angle - 2 * pi * floor(angle / (2 * pi))
///
/// \tparam T Floating point type.
/// \param angle Value to wrap.
/// \return Radians in the interval [0, 2*pi)
template <typename T>
constexpr T wrap_to_two_pi(const T angle)
{
    static_assert(std::is_same<float, T>::value || std::is_same<double, T>::value, "Only float and double supported!");
    const T two_pi = score::cpp::math::constants::two_pi<T>();
    return score::cpp::fmod_floored(angle, two_pi);
}

/// \brief Computes the signum of a value.
///
///            | -1 if x<0,
/// signum(x) =|  0 if x=0,
///            |  1 if x>0,
///
/// \tparam T arithmetic type
/// \param value the signum should be computed of
/// \return the signum of the value with the same type.
template <typename T>
constexpr T signum(const T value)
{
    static_assert(std::is_signed<T>::value, "Signum makes only sense with signed values");
    return static_cast<T>((T{0} < value) - (value < T{0}));
}

/// \brief Squares the given value.
///
/// \tparam T Data type the function is applied to.
/// \param value Value to be squared.
/// \return value*value (or value**2)
template <typename T>
constexpr T square(const T value)
{
    return value * value;
}

} // namespace math
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_MATH_HPP
