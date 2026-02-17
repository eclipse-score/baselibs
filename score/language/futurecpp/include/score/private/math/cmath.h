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
/// The safety manual (broken_link_c/issue/3871296) for QNX requires to compile math functions
/// with `-fno-builtin`. `amp` is taking care of this by providing for
/// Trigonometric Functions
///     - acos(), acosf()
///     - asin(), asinf()
///     - atan2(), atan2f()
///     - atan(), atanf()
///     - cos(), cosf()
///     - sin(), sinf()
///     - tan(), tanf()
/// Hyperbolic Functions
///     - acosh(), acoshf()
///     - asinh(), asinhf()
///     - atanh(), atanhf()
///     - cosh(), coshf()
///     - sinh(), sinhf()
///     - tanh(), tanhf()
/// Logarithmic Functions
///     - ilogb(), ilogbf()
///     - log10(), log10f()
///     - log1p(), log1pf()
///     - log2(), log2f()
///     - logb(), logbf()
///     - log(), logf()
/// Comparison and Classification Functions
///     - fmax(), fmaxf()
///     - fmin(), fminf()
/// Rounding Functions
///     - ceil(), ceilf()
///     - floor(), floorf()
///     - lrint(), lrintf()
///     - lround(), lroundf()
///     - nearbyint(), nearbyintf()
///     - nextafter(), nextafterf()
///     - nexttoward(), nexttowardf()
///     - rint(), rintf()
///     - round(), roundf()
///     - trunc(), truncf()
/// Statistical Functions
///     - erfc(), erfcf()
///     - erf(), erff()
/// Power Functions
///     - cbrt(), cbrtf()
///     - exp2(), exp2f()
///     - expm1(), expm1f()
///     - exp(), expf()
///     - pow(), powf()
///     - sqrt(), sqrtf()
/// Gamma Functions
///     - lgamma(), lgammaf()
///     - tgamma(), tgammaf()
/// Remainder Functions
///     - fmod(), fmodf()
///     - modf(), modff()
///     - remainder(), remainderf()
///     - remquo(), remquof()
/// Miscellaneous Functions
///     - copysign(), copysignf()
///     - fabs(), fabsf()
///     - fdim(), fdimf()
///     - fma(), fmaf()
///     - frexp(), frexpf()
///     - hypot(), hypotf()
///     - ldexp(), ldexpf()
///     - nan(), nanf()
///     - scalbln(), scalblnf()
///     - scalbn(), scalbnf()
/// Macros Comparison and Classification
///     - fpclassify()
///     - isfinite()
///     - isinf()
///     - isnan()
///     - isnormal()
///     - signbit()
///     - isgreater()
///     - isgreaterequal()
///     - isless()
///     - islessequal()
///     - islessgreater()
///     - isunordered()
/// a corresponding function in amp. The lib is compiled with `-fno-builtin`.
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_CMATH_H
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_CMATH_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    double score_future_cpp_acos(double num);
    float score_future_cpp_acosf(float num);

    double score_future_cpp_asin(double num);
    float score_future_cpp_asinf(float num);

    double score_future_cpp_atan2(double y, double x);
    float score_future_cpp_atan2f(float y, float x);

    double score_future_cpp_atan(double num);
    float score_future_cpp_atanf(float num);

    double score_future_cpp_cos(double num);
    float score_future_cpp_cosf(float num);

    double score_future_cpp_sin(double num);
    float score_future_cpp_sinf(float num);

    double score_future_cpp_tan(double num);
    float score_future_cpp_tanf(float num);

    double score_future_cpp_acosh(double num);
    float score_future_cpp_acoshf(float num);

    double score_future_cpp_asinh(double num);
    float score_future_cpp_asinhf(float num);

    double score_future_cpp_atanh(double num);
    float score_future_cpp_atanhf(float num);

    double score_future_cpp_cosh(double num);
    float score_future_cpp_coshf(float num);

    double score_future_cpp_sinh(double num);
    float score_future_cpp_sinhf(float num);

    double score_future_cpp_tanh(double num);
    float score_future_cpp_tanhf(float num);

    int score_future_cpp_ilogb(double num);
    int score_future_cpp_ilogbf(float num);

    double score_future_cpp_log10(double num);
    float score_future_cpp_log10f(float num);

    double score_future_cpp_log1p(double num);
    float score_future_cpp_log1pf(float num);

    double score_future_cpp_log2(double num);
    float score_future_cpp_log2f(float num);

    double score_future_cpp_logb(double num);
    float score_future_cpp_logbf(float num);

    double score_future_cpp_log(double num);
    float score_future_cpp_logf(float num);

    double score_future_cpp_fmax(double x, double y);
    float score_future_cpp_fmaxf(float x, float y);

    double score_future_cpp_fmin(double x, double y);
    float score_future_cpp_fminf(float x, float y);

    double score_future_cpp_ceil(double num);
    float score_future_cpp_ceilf(float num);

    double score_future_cpp_floor(double num);
    float score_future_cpp_floorf(float num);

    long score_future_cpp_lrint(double num);
    long score_future_cpp_lrintf(float num);

    long score_future_cpp_lround(double num);
    long score_future_cpp_lroundf(float num);

    double score_future_cpp_nearbyint(double num);
    float score_future_cpp_nearbyintf(float num);

    double score_future_cpp_nextafter(double from, double to);
    float score_future_cpp_nextafterf(float from, float to);

    double score_future_cpp_nexttoward(double from, double to);
    float score_future_cpp_nexttowardf(float from, float to);

    double score_future_cpp_rint(double num);
    float score_future_cpp_rintf(float num);

    double score_future_cpp_round(double num);
    float score_future_cpp_roundf(float num);

    double score_future_cpp_trunc(double num);
    float score_future_cpp_truncf(float num);

    double score_future_cpp_erfc(double num);
    float score_future_cpp_erfcf(float num);

    double score_future_cpp_erf(double num);
    float score_future_cpp_erff(float num);

    double score_future_cpp_cbrt(double num);
    float score_future_cpp_cbrtf(float num);

    double score_future_cpp_exp2(double num);
    float score_future_cpp_exp2f(float num);

    double score_future_cpp_expm1(double num);
    float score_future_cpp_expm1f(float num);

    double score_future_cpp_exp(double num);
    float score_future_cpp_expf(float num);

    double score_future_cpp_pow(double base, double exp);
    float score_future_cpp_powf(float base, float exp);

    double score_future_cpp_sqrt(double num);
    float score_future_cpp_sqrtf(float num);

    double score_future_cpp_lgamma(double num);
    float score_future_cpp_lgammaf(float num);

    double score_future_cpp_tgamma(double num);
    float score_future_cpp_tgammaf(float num);

    double score_future_cpp_fmod(double x, double y);
    float score_future_cpp_fmodf(float x, float y);

    double score_future_cpp_modf(double num, double* iptr);
    float score_future_cpp_modff(float num, float* iptr);

    double score_future_cpp_remainder(double x, double y);
    float score_future_cpp_remainderf(float x, float y);

    double score_future_cpp_remquo(double x, double y, int* quo);
    float score_future_cpp_remquof(float x, float y, int* quo);

    double score_future_cpp_copysign(double mag, double sgn);
    float score_future_cpp_copysignf(float mag, float sgn);

    double score_future_cpp_fabs(double arg);
    float score_future_cpp_fabsf(float arg);

    double score_future_cpp_fdim(double x, double y);
    float score_future_cpp_fdimf(float x, float y);

    double score_future_cpp_fma(double x, double y, double z);
    float score_future_cpp_fmaf(float x, float y, float z);

    double score_future_cpp_frexp(double num, int* exp);
    float score_future_cpp_frexpf(float num, int* exp);

    double score_future_cpp_hypot(double x, double y);
    float score_future_cpp_hypotf(float x, float y);

    double score_future_cpp_ldexp(double num, int exp);
    float score_future_cpp_ldexpf(float num, int exp);

    double score_future_cpp_nan(const char* arg);
    float score_future_cpp_nanf(const char* arg);

    double score_future_cpp_scalbln(double num, int exp);
    float score_future_cpp_scalblnf(float num, int exp);

    double score_future_cpp_scalbn(double num, int exp);
    float score_future_cpp_scalbnf(float num, int exp);

    int score_future_cpp_fpclassify(double num);
    int score_future_cpp_fpclassifyf(float num);

    int score_future_cpp_isfinite(double num);
    int score_future_cpp_isfinitef(float num);

    int score_future_cpp_isinf(double num);
    int score_future_cpp_isinff(float num);

    int score_future_cpp_isnan(double num);
    int score_future_cpp_isnanf(float num);

    int score_future_cpp_isnormal(double num);
    int score_future_cpp_isnormalf(float num);

    int score_future_cpp_signbit(double num);
    int score_future_cpp_signbitf(float num);

    int score_future_cpp_isgreater(double x, double y);
    int score_future_cpp_isgreaterf(float x, float y);

    int score_future_cpp_isgreaterequal(double x, double y);
    int score_future_cpp_isgreaterequalf(float x, float y);

    int score_future_cpp_isless(double x, double y);
    int score_future_cpp_islessf(float x, float y);

    int score_future_cpp_islessequal(double x, double y);
    int score_future_cpp_islessequalf(float x, float y);

    int score_future_cpp_islessgreater(double x, double y);
    int score_future_cpp_islessgreaterf(float x, float y);

    int score_future_cpp_isunordered(double x, double y);
    int score_future_cpp_isunorderedf(float x, float y);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_CMATH_H
