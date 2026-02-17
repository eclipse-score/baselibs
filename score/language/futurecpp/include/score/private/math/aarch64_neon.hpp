/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_MATH_AARCH64_NEON_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_MATH_AARCH64_NEON_HPP

#include <score/private/bit/bit_cast.hpp>
#include <score/private/math/equals_bitexact.hpp>
#include <cstdint>
#include <limits>
// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4987269
#include <arm_neon.h> // only include NEON

// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4049789
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE __attribute__((__always_inline__))

// https://developer.arm.com/architectures/instruction-sets/intrinsics/#f:@navigationhierarchiessimdisa=[Neon]&f:@navigationhierarchiesarchitectures=[A64]

namespace score::cpp
{
namespace detail
{

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE double fmax_neon(double x, double y)
{
    return vdupd_lane_f64(vmaxnm_f64(vdup_n_f64(x), vdup_n_f64(y)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE float fmax_neon(float x, float y)
{
    return vdups_lane_f32(vmaxnm_f32(vdup_n_f32(x), vdup_n_f32(y)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE double fmin_neon(double x, double y)
{
    return vdupd_lane_f64(vminnm_f64(vdup_n_f64(x), vdup_n_f64(y)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE float fmin_neon(float x, float y)
{
    return vdups_lane_f32(vminnm_f32(vdup_n_f32(x), vdup_n_f32(y)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE double ceil_neon(double num)
{
    return vdupd_lane_f64(vrndp_f64(vdup_n_f64(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE float ceil_neon(float num)
{
    return vdups_lane_f32(vrndp_f32(vdup_n_f32(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE double floor_neon(double num)
{
    return vdupd_lane_f64(vrndm_f64(vdup_n_f64(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE float floor_neon(float num)
{
    return vdups_lane_f32(vrndm_f32(vdup_n_f32(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE double rint_neon(double num)
{
    return vdupd_lane_f64(vrndx_f64(vdup_n_f64(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE float rint_neon(float num)
{
    return vdups_lane_f32(vrndx_f32(vdup_n_f32(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE double round_neon(double num)
{
    return vdupd_lane_f64(vrnda_f64(vdup_n_f64(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE float round_neon(float num)
{
    return vdups_lane_f32(vrnda_f32(vdup_n_f32(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE double trunc_neon(double num)
{
    return vdupd_lane_f64(vrnd_f64(vdup_n_f64(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE float trunc_neon(float num)
{
    return vdups_lane_f32(vrnd_f32(vdup_n_f32(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE double abs_neon(double num)
{
    return vdupd_lane_f64(vabs_f64(vdup_n_f64(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE float abs_neon(float num)
{
    return vdups_lane_f32(vabs_f32(vdup_n_f32(num)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE double copysign_neon(double mag, double sgn)
{
    const auto sign_mask = vdup_n_u64(0x8000000000000000U);
    return vdupd_lane_f64(vbsl_f64(sign_mask, vdup_n_f64(sgn), vdup_n_f64(mag)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE float copysign_neon(float mag, float sgn)
{
    const auto sign_mask = vdup_n_u32(0x80000000U);
    return vdups_lane_f32(vbsl_f32(sign_mask, vdup_n_f32(sgn), vdup_n_f32(mag)), 0);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE bool isfinite_neon(double num)
{
    const auto inf = score::cpp::bit_cast<std::uint64_t>(std::numeric_limits<double>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint64_t>(num) & 0x7FFFFFFFFFFFFFFFU;
    return abs_num < inf;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE bool isfinite_neon(float num)
{
    const auto inf = score::cpp::bit_cast<std::uint32_t>(std::numeric_limits<float>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint32_t>(num) & 0x7FFFFFFFU;
    return abs_num < inf;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE bool isinf_neon(double num)
{
    const auto inf = score::cpp::bit_cast<std::uint64_t>(std::numeric_limits<double>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint64_t>(num) & 0x7FFFFFFFFFFFFFFFU;
    return abs_num == inf;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE bool isinf_neon(float num)
{
    const auto inf = score::cpp::bit_cast<std::uint32_t>(std::numeric_limits<float>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint32_t>(num) & 0x7FFFFFFFU;
    return abs_num == inf;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE bool isnan_neon(double num)
{
    const auto inf = score::cpp::bit_cast<std::uint64_t>(std::numeric_limits<double>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint64_t>(num) & 0x7FFFFFFFFFFFFFFFU;
    return inf < abs_num;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE bool isnan_neon(float num)
{
    const auto inf = score::cpp::bit_cast<std::uint32_t>(std::numeric_limits<float>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint32_t>(num) & 0x7FFFFFFFU;
    return inf < abs_num;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE bool isnormal_neon(double num)
{
    const auto min = score::cpp::bit_cast<std::uint64_t>(std::numeric_limits<double>::min());
    const auto inf = score::cpp::bit_cast<std::uint64_t>(std::numeric_limits<double>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint64_t>(num) & 0x7FFFFFFFFFFFFFFFU;
    return (abs_num - min) < (inf - min);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE bool isnormal_neon(float num)
{
    const auto min = score::cpp::bit_cast<std::uint32_t>(std::numeric_limits<float>::min());
    const auto inf = score::cpp::bit_cast<std::uint32_t>(std::numeric_limits<float>::infinity());
    const auto abs_num = score::cpp::bit_cast<std::uint32_t>(num) & 0x7FFFFFFFU;
    return (abs_num - min) < (inf - min);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE bool signbit_neon(double num)
{
    return (score::cpp::bit_cast<std::uint64_t>(num) >> 63U) != 0U;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE bool signbit_neon(float num)
{
    return (score::cpp::bit_cast<std::uint32_t>(num) >> 31U) != 0U;
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE double sqrt_neon(double num)
{
    static_assert(score::cpp::equals_bitexact(-0.0, 0.0), "usage of 0.0 in if-statement assumes equality");
    if (!score::cpp::detail::isnan_neon(num) && (num >= 0.0))
    {
        return vdupd_lane_f64(vsqrt_f64(vdup_n_f64(num)), 0);
    }
    return ::score_future_cpp_sqrt(num);
}

inline SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE float sqrt_neon(float num)
{
    static_assert(score::cpp::equals_bitexact(-0.0F, 0.0F), "usage of 0.0 in if-statement assumes equality");
    if (!score::cpp::detail::isnan_neon(num) && (num >= 0.0F))
    {
        return vdups_lane_f32(vsqrt_f32(vdup_n_f32(num)), 0);
    }
    return ::score_future_cpp_sqrtf(num);
}

} // namespace detail
} // namespace score::cpp

#undef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_AARCH64_NEON_ALWAYS_INLINE

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_MATH_AARCH64_NEON_HPP
