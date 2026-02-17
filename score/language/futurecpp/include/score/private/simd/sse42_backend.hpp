/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SSE42_BACKEND_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SSE42_BACKEND_HPP

#include <score/private/bit/bit_cast.hpp>
#include <score/private/simd/abi.hpp>
#include <score/private/simd/array.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4036540
#include <nmmintrin.h> // only include SSE4.2.

// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4049789
#define SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE __attribute__((__always_inline__))

// Documentation of Intel SSE4.2 Intrinsics
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#techs=SSE_ALL

namespace score::cpp
{
namespace simd
{

namespace detail
{
namespace sse42
{

// wrap the SSE types to avoid "ignored attributes" warning: https://stackoverflow.com/a/79539286
// see also the GCC bug ticket https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97222

struct uint8x16_t
{
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE uint8x16_t() = default;
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE uint8x16_t(const __m128i value) noexcept : v(value) {}
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator __m128i() const noexcept { return v; }
    __m128i v;
};

struct int32x4_t
{
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE int32x4_t() = default;
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE int32x4_t(const __m128i value) noexcept : v(value) {}
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator __m128i() const noexcept { return v; }
    __m128i v;
};

struct float32x4_t
{
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE float32x4_t() = default;
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE float32x4_t(const __m128 value) noexcept : v(value) {}
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator __m128() const noexcept { return v; }
    __m128 v;
};

struct float64x2_t
{
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE float64x2_t() = default;
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE float64x2_t(const __m128d value) noexcept : v(value) {}
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator __m128d() const noexcept { return v; }
    __m128d v;
};

template <typename T>
struct mask_backend;

template <>
struct mask_backend<std::int32_t>
{
    using type = int32x4_t;
    static constexpr std::size_t width{4};

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const bool v) noexcept
    {
        return _mm_set1_epi32(-static_cast<std::int32_t>(v));
    }

    template <typename G, std::size_t I0, std::size_t I1, std::size_t I2, std::size_t I3>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<I0, I1, I2, I3>) noexcept
    {
        const auto r = _mm_set_epi32(static_cast<std::int32_t>(gen(std::integral_constant<std::size_t, I3>{})),
                                     static_cast<std::int32_t>(gen(std::integral_constant<std::size_t, I2>{})),
                                     static_cast<std::int32_t>(gen(std::integral_constant<std::size_t, I1>{})),
                                     static_cast<std::int32_t>(gen(std::integral_constant<std::size_t, I0>{})));
        return _mm_sub_epi32(_mm_setzero_si128(), r);
    }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        return (_mm_movemask_epi8(v) & (1 << (4U * i))) != 0;
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_not(const type v) noexcept
    {
        return _mm_cmpeq_epi32(v, _mm_setzero_si128());
    }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_and(const type a, const type b) noexcept { return _mm_and_si128(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_or(const type a, const type b) noexcept { return _mm_or_si128(a, b); }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE all_of(const type v) noexcept { return _mm_movemask_epi8(v) == 0xFFFF; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE any_of(const type v) noexcept { return _mm_movemask_epi8(v) != 0; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE none_of(const type v) noexcept { return _mm_movemask_epi8(v) == 0; }
};

template <>
struct mask_backend<float>
{
    using type = float32x4_t;
    static constexpr std::size_t width{4};

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const bool v) noexcept
    {
        return _mm_castsi128_ps(_mm_set1_epi32(-static_cast<std::int32_t>(v)));
    }

    template <typename G, std::size_t I0, std::size_t I1, std::size_t I2, std::size_t I3>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<I0, I1, I2, I3>) noexcept
    {
        const auto r = _mm_set_epi32(static_cast<std::int32_t>(gen(std::integral_constant<std::size_t, I3>{})),
                                     static_cast<std::int32_t>(gen(std::integral_constant<std::size_t, I2>{})),
                                     static_cast<std::int32_t>(gen(std::integral_constant<std::size_t, I1>{})),
                                     static_cast<std::int32_t>(gen(std::integral_constant<std::size_t, I0>{})));
        return _mm_castsi128_ps(_mm_sub_epi32(_mm_setzero_si128(), r));
    }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        return (_mm_movemask_ps(v) & (1 << i)) != 0;
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_not(const type v) noexcept { return _mm_cmpeq_ps(v, _mm_setzero_ps()); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_and(const type a, const type b) noexcept { return _mm_and_ps(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_or(const type a, const type b) noexcept { return _mm_or_ps(a, b); }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE all_of(const type v) noexcept { return _mm_movemask_ps(v) == 0b1111; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE any_of(const type v) noexcept { return _mm_movemask_ps(v) != 0; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE none_of(const type v) noexcept { return _mm_movemask_ps(v) == 0; }
};

template <>
struct mask_backend<double>
{
    using type = float64x2_t;
    static constexpr std::size_t width{2};

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const bool v) noexcept
    {
        return _mm_castsi128_pd(_mm_set1_epi64x(-static_cast<std::int64_t>(v)));
    }

    template <typename G, std::size_t I0, std::size_t I1>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<I0, I1>) noexcept
    {
        const auto r = _mm_set_epi64x(static_cast<std::int64_t>(gen(std::integral_constant<std::size_t, I1>{})),
                                      static_cast<std::int64_t>(gen(std::integral_constant<std::size_t, I0>{})));
        return _mm_castsi128_pd(_mm_sub_epi64(_mm_setzero_si128(), r));
    }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        return (_mm_movemask_pd(v) & (1 << i)) != 0;
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_not(const type v) noexcept { return _mm_cmpeq_pd(v, _mm_setzero_pd()); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_and(const type a, const type b) noexcept { return _mm_and_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE logical_or(const type a, const type b) noexcept { return _mm_or_pd(a, b); }

    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE all_of(const type v) noexcept { return _mm_movemask_pd(v) == 0b11; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE any_of(const type v) noexcept { return _mm_movemask_pd(v) != 0; }
    static bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE none_of(const type v) noexcept { return _mm_movemask_pd(v) == 0; }
};

template <typename T>
struct backend;

template <>
struct backend<std::uint8_t>
{
    using type = uint8x16_t;
    static constexpr std::size_t width{16};

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load(const std::uint8_t* const v) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(v));
    }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load_aligned(const std::uint8_t* const v) noexcept
    {
        return _mm_load_si128(reinterpret_cast<const __m128i*>(v));
    }

    static std::array<float32x4_t, 4> SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE convert(const type v, float) noexcept
    {
        return {
            _mm_cvtepi32_ps(_mm_cvtepu8_epi32(v)),
            _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(v, 4))),
            _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(v, 8))),
            _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(v, 12))),
        };
    }
};

template <>
struct backend<std::int32_t>
{
    using type = int32x4_t;
    using mask_type = mask_backend<std::int32_t>::type;
    static constexpr std::size_t width{4};

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const std::int32_t v) noexcept { return _mm_set1_epi32(v); }

    template <typename G, std::size_t I0, std::size_t I1, std::size_t I2, std::size_t I3>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<I0, I1, I2, I3>) noexcept
    {
        return _mm_set_epi32(gen(std::integral_constant<std::size_t, I3>{}),
                             gen(std::integral_constant<std::size_t, I2>{}),
                             gen(std::integral_constant<std::size_t, I1>{}),
                             gen(std::integral_constant<std::size_t, I0>{}));
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load(const std::int32_t* const v) noexcept
    {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(v));
    }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load_aligned(const std::int32_t* const v) noexcept
    {
        return _mm_load_si128(reinterpret_cast<const __m128i*>(v));
    }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store(std::int32_t* const v, const type a) noexcept
    {
        _mm_storeu_si128(reinterpret_cast<__m128i*>(v), a);
    }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store_aligned(std::int32_t* const v, const type a) noexcept
    {
        _mm_store_si128(reinterpret_cast<__m128i*>(v), a);
    }

    static std::int32_t SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        alignas(16) std::int32_t tmp[width];
        _mm_store_si128(reinterpret_cast<__m128i*>(tmp), v);
        return tmp[i];
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE add(const type a, const type b) noexcept { return _mm_add_epi32(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE subtract(const type a, const type b) noexcept { return _mm_sub_epi32(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE multiply(const type a, const type b) noexcept { return _mm_mullo_epi32(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE divide(const type a, const type b) noexcept
    {
        type r0{};
        r0 = _mm_insert_epi32(r0, _mm_extract_epi32(a, 0) / _mm_extract_epi32(b, 0), 0);
        r0 = _mm_insert_epi32(r0, _mm_extract_epi32(a, 1) / _mm_extract_epi32(b, 1), 1);

        type r1{};
        r1 = _mm_insert_epi32(r1, _mm_extract_epi32(a, 2) / _mm_extract_epi32(b, 2), 0);
        r1 = _mm_insert_epi32(r1, _mm_extract_epi32(a, 3) / _mm_extract_epi32(b, 3), 1);

        return _mm_unpacklo_epi64(r0, r1);
    }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE negate(const type v) noexcept { return _mm_sub_epi32(_mm_setzero_si128(), v); }

    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE equal(const type a, const type b) noexcept { return _mm_cmpeq_epi32(a, b); }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE not_equal(const type a, const type b) noexcept
    {
        return _mm_cmpeq_epi32(_mm_cmpeq_epi32(a, b), _mm_setzero_si128());
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_than(const type a, const type b) noexcept
    {
        return _mm_cmplt_epi32(a, b);
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_equal(const type a, const type b) noexcept
    {
        return _mm_cmpeq_epi32(_mm_cmpgt_epi32(a, b), _mm_setzero_si128());
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_than(const type a, const type b) noexcept
    {
        return _mm_cmpgt_epi32(a, b);
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_equal(const type a, const type b) noexcept
    {
        return _mm_cmpeq_epi32(_mm_cmplt_epi32(a, b), _mm_setzero_si128());
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE min(const type a, const type b) noexcept { return _mm_min_epi32(b, a); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE max(const type a, const type b) noexcept { return _mm_max_epi32(b, a); }

    static float32x4_t SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE convert(const type v, float) noexcept { return _mm_cvtepi32_ps(v); }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE blend(const type a, const type b, const mask_type c) noexcept
    {
        return _mm_blendv_epi8(a, b, c);
    }
};

template <>
struct backend<float>
{
    using type = float32x4_t;
    using mask_type = mask_backend<float>::type;
    static constexpr std::size_t width{4};

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const float v) noexcept { return _mm_set1_ps(v); }

    template <typename G, std::size_t I0, std::size_t I1, std::size_t I2, std::size_t I3>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<I0, I1, I2, I3>) noexcept
    {
        return _mm_set_ps(gen(std::integral_constant<std::size_t, I3>{}),
                          gen(std::integral_constant<std::size_t, I2>{}),
                          gen(std::integral_constant<std::size_t, I1>{}),
                          gen(std::integral_constant<std::size_t, I0>{}));
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load(const float* const v) noexcept { return _mm_loadu_ps(v); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load_aligned(const float* const v) noexcept { return _mm_load_ps(v); }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store(float* const v, const type a) noexcept { _mm_storeu_ps(v, a); }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store_aligned(float* const v, const type a) noexcept { _mm_store_ps(v, a); }

    static float SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        alignas(16) float tmp[width];
        _mm_store_ps(&tmp[0], v);
        return tmp[i];
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE add(const type a, const type b) noexcept { return _mm_add_ps(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE subtract(const type a, const type b) noexcept { return _mm_sub_ps(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE multiply(const type a, const type b) noexcept { return _mm_mul_ps(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE divide(const type a, const type b) noexcept { return _mm_div_ps(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE negate(const type v) noexcept { return _mm_xor_ps(v, _mm_set1_ps(-0.0F)); }

    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE equal(const type a, const type b) noexcept { return _mm_cmpeq_ps(a, b); }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE not_equal(const type a, const type b) noexcept
    {
        return _mm_cmpneq_ps(a, b);
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_than(const type a, const type b) noexcept
    {
        return _mm_cmplt_ps(a, b);
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_equal(const type a, const type b) noexcept
    {
        return _mm_cmple_ps(a, b);
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_than(const type a, const type b) noexcept
    {
        return _mm_cmpgt_ps(a, b);
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_equal(const type a, const type b) noexcept
    {
        return _mm_cmpge_ps(a, b);
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE min(const type a, const type b) noexcept { return _mm_min_ps(b, a); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE max(const type a, const type b) noexcept { return _mm_max_ps(b, a); }

    static int32x4_t SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE convert(const type v, std::int32_t) noexcept { return _mm_cvttps_epi32(v); }

    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE is_nan(const type v) noexcept
    {
        const auto inf = score::cpp::bit_cast<std::int32_t>(std::numeric_limits<float>::infinity());
        const auto abs_v = _mm_and_si128(_mm_castps_si128(v), _mm_set1_epi32(0x7FFF'FFFF));
        return _mm_castsi128_ps(_mm_cmplt_epi32(_mm_set1_epi32(inf), abs_v));
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE blend(const type a, const type b, const mask_type c) noexcept
    {
        return _mm_blendv_ps(a, b, c);
    }
};

template <>
struct backend<double>
{
    using type = float64x2_t;
    using mask_type = mask_backend<double>::type;
    static constexpr std::size_t width{2};

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE broadcast(const double v) noexcept { return _mm_set1_pd(v); }

    template <typename G, std::size_t I0, std::size_t I1>
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE init(G&& gen, const std::index_sequence<I0, I1>) noexcept
    {
        return _mm_set_pd(gen(std::integral_constant<std::size_t, I1>{}),
                          gen(std::integral_constant<std::size_t, I0>{}));
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load(const double* const v) noexcept { return _mm_loadu_pd(v); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE load_aligned(const double* const v) noexcept { return _mm_load_pd(v); }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store(double* const v, const type a) noexcept { _mm_storeu_pd(v, a); }
    static void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE store_aligned(double* const v, const type a) noexcept { _mm_store_pd(v, a); }

    static double SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        alignas(16) double tmp[width];
        _mm_store_pd(&tmp[0], v);
        return tmp[i];
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE add(const type a, const type b) noexcept { return _mm_add_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE subtract(const type a, const type b) noexcept { return _mm_sub_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE multiply(const type a, const type b) noexcept { return _mm_mul_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE divide(const type a, const type b) noexcept { return _mm_div_pd(a, b); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE negate(const type v) noexcept { return _mm_xor_pd(v, _mm_set1_pd(-0.0)); }

    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE equal(const type a, const type b) noexcept { return _mm_cmpeq_pd(a, b); }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE not_equal(const type a, const type b) noexcept
    {
        return _mm_cmpneq_pd(a, b);
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_than(const type a, const type b) noexcept
    {
        return _mm_cmplt_pd(a, b);
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE less_equal(const type a, const type b) noexcept
    {
        return _mm_cmple_pd(a, b);
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_than(const type a, const type b) noexcept
    {
        return _mm_cmpgt_pd(a, b);
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE greater_equal(const type a, const type b) noexcept
    {
        return _mm_cmpge_pd(a, b);
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE min(const type a, const type b) noexcept { return _mm_min_pd(b, a); }
    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE max(const type a, const type b) noexcept { return _mm_max_pd(b, a); }

    static mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE is_nan(const type v) noexcept
    {
        const auto inf = score::cpp::bit_cast<std::int64_t>(std::numeric_limits<double>::infinity());
        const auto abs_v = _mm_and_si128(_mm_castpd_si128(v), _mm_set1_epi64x(0x7FFF'FFFF'FFFF'FFFF));
        return _mm_castsi128_pd(_mm_cmpgt_epi64(abs_v, _mm_set1_epi64x(inf)));
    }

    static type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE blend(const type a, const type b, const mask_type c) noexcept
    {
        return _mm_blendv_pd(a, b, c);
    }
};

template <typename T>
struct abi;

template <>
struct abi<std::uint8_t>
{
    using impl = backend<std::uint8_t>;
    using mask_impl = mask_backend<std::uint8_t>;
};
template <>
struct abi<std::int32_t>
{
    using impl = backend<std::int32_t>;
    using mask_impl = mask_backend<std::int32_t>;
};
template <>
struct abi<float>
{
    using impl = backend<float>;
    using mask_impl = mask_backend<float>;
};
template <>
struct abi<double>
{
    using impl = backend<double>;
    using mask_impl = mask_backend<double>;
};

template <typename T, std::size_t N>
struct array_abi;

template <>
struct array_abi<float, 16>
{
    using impl = array<float, backend<float>, mask_backend<float>, 0, 1, 2, 3>;
    using mask_impl = array_mask<float, mask_backend<float>, 0, 1, 2, 3>;
};

} // namespace sse42

template <>
struct native_abi<std::uint8_t>
{
    using type = sse42::abi<std::uint8_t>;
};
template <>
struct native_abi<std::int32_t>
{
    using type = sse42::abi<std::int32_t>;
};
template <>
struct native_abi<float>
{
    using type = sse42::abi<float>;
};
template <>
struct native_abi<double>
{
    using type = sse42::abi<double>;
};

template <>
struct deduce_abi<std::int32_t, 4>
{
    using type = native_abi<std::int32_t>::type;
};
template <>
struct deduce_abi<float, 4>
{
    using type = native_abi<float>::type;
};
template <>
struct deduce_abi<float, 16>
{
    using type = sse42::array_abi<float, 16>;
};
template <>
struct deduce_abi<double, 2>
{
    using type = native_abi<double>::type;
};

template <>
struct is_abi_tag<sse42::abi<std::int32_t>> : std::true_type
{
};
template <>
struct is_abi_tag<sse42::abi<float>> : std::true_type
{
};
template <>
struct is_abi_tag<sse42::array_abi<float, 16>> : std::true_type
{
};
template <>
struct is_abi_tag<sse42::abi<double>> : std::true_type
{
};

} // namespace detail
} // namespace simd
} // namespace score::cpp

#undef SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SSE42_BACKEND_HPP
