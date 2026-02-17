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

#ifdef __cplusplus
#error "File must be compiled as C source file"
#endif // __cplusplus

#include <score/private/math/cmath.h>

#include <math.h>

double score_future_cpp_acos(double num) { return acos(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_acosf(float num) { return acosf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_asin(double num) { return asin(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_asinf(float num) { return asinf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_atan2(double y, double x) { return atan2(y, x); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_atan2f(float y, float x) { return atan2f(y, x); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_atan(double num) { return atan(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_atanf(float num) { return atanf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_cos(double num) { return cos(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_cosf(float num) { return cosf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_sin(double num) { return sin(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_sinf(float num) { return sinf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_tan(double num) { return tan(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_tanf(float num) { return tanf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_acosh(double num) { return acosh(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_acoshf(float num) { return acoshf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_asinh(double num) { return asinh(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_asinhf(float num) { return asinhf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_atanh(double num) { return atanh(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_atanhf(float num) { return atanhf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_cosh(double num) { return cosh(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_coshf(float num) { return coshf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_sinh(double num) { return sinh(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_sinhf(float num) { return sinhf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_tanh(double num) { return tanh(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_tanhf(float num) { return tanhf(num); } // NOLINT(bmw-no-standard-math)

int score_future_cpp_ilogb(double num) { return ilogb(num); }  // NOLINT(bmw-no-standard-math)
int score_future_cpp_ilogbf(float num) { return ilogbf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_log10(double num) { return log10(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_log10f(float num) { return log10f(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_log1p(double num) { return log1p(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_log1pf(float num) { return log1pf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_log2(double num) { return log2(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_log2f(float num) { return log2f(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_logb(double num) { return logb(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_logbf(float num) { return logbf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_log(double num) { return log(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_logf(float num) { return logf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_fmax(double x, double y) { return fmax(x, y); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_fmaxf(float x, float y) { return fmaxf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_fmin(double x, double y) { return fmin(x, y); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_fminf(float x, float y) { return fminf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_ceil(double num) { return ceil(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_ceilf(float num) { return ceilf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_floor(double num) { return floor(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_floorf(float num) { return floorf(num); } // NOLINT(bmw-no-standard-math)

long score_future_cpp_lrint(double num) { return lrint(num); }  // NOLINT(bmw-no-standard-math)
long score_future_cpp_lrintf(float num) { return lrintf(num); } // NOLINT(bmw-no-standard-math)

long score_future_cpp_lround(double num) { return lround(num); }  // NOLINT(bmw-no-standard-math)
long score_future_cpp_lroundf(float num) { return lroundf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_nearbyint(double num) { return nearbyint(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_nearbyintf(float num) { return nearbyintf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_nextafter(double from, double to) { return nextafter(from, to); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_nextafterf(float from, float to) { return nextafterf(from, to); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_nexttoward(double from, double to) { return nexttoward(from, to); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_nexttowardf(float from, float to) { return nexttowardf(from, to); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_rint(double num) { return rint(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_rintf(float num) { return rintf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_round(double num) { return round(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_roundf(float num) { return roundf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_trunc(double num) { return trunc(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_truncf(float num) { return truncf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_erfc(double num) { return erfc(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_erfcf(float num) { return erfcf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_erf(double num) { return erf(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_erff(float num) { return erff(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_cbrt(double num) { return cbrt(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_cbrtf(float num) { return cbrtf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_exp2(double num) { return exp2(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_exp2f(float num) { return exp2f(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_expm1(double num) { return expm1(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_expm1f(float num) { return expm1f(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_exp(double num) { return exp(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_expf(float num) { return expf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_pow(double base, double exp) { return pow(base, exp); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_powf(float base, float exp) { return powf(base, exp); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_sqrt(double num) { return sqrt(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_sqrtf(float num) { return sqrtf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_lgamma(double num) { return lgamma(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_lgammaf(float num) { return lgammaf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_tgamma(double num) { return tgamma(num); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_tgammaf(float num) { return tgammaf(num); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_fmod(double x, double y) { return fmod(x, y); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_fmodf(float x, float y) { return fmodf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_modf(double num, double* iptr) { return modf(num, iptr); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_modff(float num, float* iptr) { return modff(num, iptr); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_remainder(double x, double y) { return remainder(x, y); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_remainderf(float x, float y) { return remainderf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_remquo(double x, double y, int* quo) { return remquo(x, y, quo); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_remquof(float x, float y, int* quo) { return remquof(x, y, quo); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_copysign(double mag, double sgn) { return copysign(mag, sgn); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_copysignf(float mag, float sgn) { return copysignf(mag, sgn); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_fabs(double arg) { return fabs(arg); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_fabsf(float arg) { return fabsf(arg); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_fdim(double x, double y) { return fdim(x, y); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_fdimf(float x, float y) { return fdimf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_fma(double x, double y, double z) { return fma(x, y, z); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_fmaf(float x, float y, float z) { return fmaf(x, y, z); }   // NOLINT(bmw-no-standard-math)

double score_future_cpp_frexp(double num, int* exp) { return frexp(num, exp); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_frexpf(float num, int* exp) { return frexpf(num, exp); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_hypot(double x, double y) { return hypot(x, y); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_hypotf(float x, float y) { return hypotf(x, y); }  // NOLINT(bmw-no-standard-math)

double score_future_cpp_ldexp(double num, int exp) { return ldexp(num, exp); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_ldexpf(float num, int exp) { return ldexpf(num, exp); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_nan(const char* arg) { return nan(arg); }  // NOLINT(bmw-no-standard-math)
float score_future_cpp_nanf(const char* arg) { return nanf(arg); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_scalbln(double num, int exp) { return scalbln(num, exp); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_scalblnf(float num, int exp) { return scalblnf(num, exp); } // NOLINT(bmw-no-standard-math)

double score_future_cpp_scalbn(double num, int exp) { return scalbn(num, exp); } // NOLINT(bmw-no-standard-math)
float score_future_cpp_scalbnf(float num, int exp) { return scalbnf(num, exp); } // NOLINT(bmw-no-standard-math)

int score_future_cpp_fpclassify(double num) { return fpclassify(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_fpclassifyf(float num) { return fpclassify(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_future_cpp_isfinite(double num) { return isfinite(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_isfinitef(float num) { return isfinite(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_future_cpp_isinf(double num) { return isinf(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_isinff(float num) { return isinf(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_future_cpp_isnan(double num) { return isnan(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_isnanf(float num) { return isnan(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_future_cpp_isnormal(double num) { return isnormal(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_isnormalf(float num) { return isnormal(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_future_cpp_signbit(double num) { return signbit(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_signbitf(float num) { return signbit(num); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_future_cpp_isgreater(double x, double y) { return isgreater(x, y); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_isgreaterf(float x, float y) { return isgreater(x, y); }  // NOLINT(bmw-no-standard-math,bmw-banned-macro)

// NOLINTNEXTLINE(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_isgreaterequal(double x, double y) { return isgreaterequal(x, y); }
// NOLINTNEXTLINE(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_isgreaterequalf(float x, float y) { return isgreaterequal(x, y); }

int score_future_cpp_isless(double x, double y) { return isless(x, y); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_islessf(float x, float y) { return isless(x, y); }  // NOLINT(bmw-no-standard-math,bmw-banned-macro)

int score_future_cpp_islessequal(double x, double y) { return islessequal(x, y); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_islessequalf(float x, float y) { return islessequal(x, y); }  // NOLINT(bmw-no-standard-math,bmw-banned-macro)

// NOLINTNEXTLINE(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_islessgreater(double x, double y) { return islessgreater(x, y); }
// NOLINTNEXTLINE(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_islessgreaterf(float x, float y) { return islessgreater(x, y); }

int score_future_cpp_isunordered(double x, double y) { return isunordered(x, y); } // NOLINT(bmw-no-standard-math,bmw-banned-macro)
int score_future_cpp_isunorderedf(float x, float y) { return isunordered(x, y); }  // NOLINT(bmw-no-standard-math,bmw-banned-macro)
