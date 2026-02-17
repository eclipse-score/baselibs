/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2025 Contributors to the Eclipse Foundation
///
/// References to http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/n4808.pdf are of the form
/// [parallel] chapter paragraph
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_HPP

#include <score/private/utility/ignore.hpp>
#include <score/private/utility/make_offset_index_sequence.hpp>

#include <array>
#include <cstddef>
#include <utility>

// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4049789
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE __attribute__((__always_inline__))

namespace score::cpp
{
namespace simd
{
namespace detail
{

template <typename T, typename Abi, std::size_t... Is>
struct array_mask
{
    using type = std::array<typename Abi::type, sizeof...(Is)>;

    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE broadcast(const bool v) noexcept
    {
        return {(static_cast<void>(Is), Abi::broadcast(v))...};
    }

    template <typename G, std::size_t... I>
    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE init(G&& gen, const std::index_sequence<I...>) noexcept
    {
        return {Abi::init(gen, make_offset_index_sequence<Is * Abi::width, Abi::width>{})...};
    }

    static bool SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        return {Abi::extract(v[i / Abi::width], i % Abi::width)};
    }

    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE logical_not(const type v) noexcept
    {
        return {Abi::logical_not(v[Is])...};
    }
    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE logical_and(const type a, const type b) noexcept
    {
        return {Abi::logical_and(a[Is], b[Is])...};
    }
    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE logical_or(const type a, const type b) noexcept
    {
        return {Abi::logical_or(a[Is], b[Is])...};
    }

    static bool SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE all_of(const type v) noexcept
    {
        return (... && Abi::all_of(v[Is]));
    }
    static bool SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE any_of(const type v) noexcept
    {
        return (... || Abi::any_of(v[Is]));
    }
    static bool SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE none_of(const type v) noexcept
    {
        return (... && Abi::none_of(v[Is]));
    }
};

template <typename T, typename Abi, typename MaskAbi, std::size_t... Is>
struct array
{
    using type = std::array<typename Abi::type, sizeof...(Is)>;
    using mask_type = typename array_mask<T, MaskAbi, Is...>::type;
    static constexpr std::size_t width{Abi::width * sizeof...(Is)};

    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE broadcast(const T v) noexcept
    {
        return {(static_cast<void>(Is), Abi::broadcast(v))...};
    }

    template <typename G, std::size_t... I>
    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE init(G&& gen, const std::index_sequence<I...>) noexcept
    {
        return {Abi::init(gen, make_offset_index_sequence<Is * Abi::width, Abi::width>{})...};
    }

    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE load(const T* const v) noexcept
    {
        return {Abi::load(v + (Is * Abi::width))...};
    }
    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE load_aligned(const T* const v) noexcept
    {
        return {Abi::load(v + (Is * Abi::width))...};
    }
    static void SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE store(T* const v, const type a) noexcept
    {
        (..., Abi::store(v + (Is * Abi::width), a[Is]));
    }
    static void SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE store_aligned(T* const v, const type a) noexcept
    {
        (..., Abi::store(v + (Is * Abi::width), a[Is]));
    }

    static T SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE extract(const type v, const std::size_t i) noexcept
    {
        return {Abi::extract(v[i / Abi::width], i % Abi::width)};
    }

    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE add(const type a, const type b) noexcept
    {
        return {Abi::add(a[Is], b[Is])...};
    }
    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE subtract(const type a, const type b) noexcept
    {
        return {Abi::subtract(a[Is], b[Is])...};
    }
    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE multiply(const type a, const type b) noexcept
    {
        return {Abi::multiply(a[Is], b[Is])...};
    }
    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE divide(const type a, const type b) noexcept
    {
        return {Abi::divide(a[Is], b[Is])...};
    }
    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE negate(const type v) noexcept { return {Abi::negate(v[Is])...}; }

    static mask_type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE equal(const type a, const type b) noexcept
    {
        return {Abi::equal(a[Is], b[Is])...};
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE not_equal(const type a, const type b) noexcept
    {
        return {Abi::not_equal(a[Is], b[Is])...};
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE less_than(const type a, const type b) noexcept
    {
        return {Abi::less_than(a[Is], b[Is])...};
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE less_equal(const type a, const type b) noexcept
    {
        return {Abi::less_equal(a[Is], b[Is])...};
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE greater_than(const type a, const type b) noexcept
    {
        return {Abi::greater_than(a[Is], b[Is])...};
    }
    static mask_type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE greater_equal(const type a, const type b) noexcept
    {
        return {Abi::greater_equal(a[Is], b[Is])...};
    }

    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE min(const type a, const type b) noexcept
    {
        return {Abi::min(a[Is], b[Is])...};
    }
    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE max(const type a, const type b) noexcept
    {
        return {Abi::max(a[Is], b[Is])...};
    }

    template <typename To>
    static std::array<To, sizeof...(Is)> SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE convert(const type v, const To) noexcept
    {
        static_assert(Abi::width == 1U, "for now only support width 1. rearranging widths not yet needed");
        return {Abi::convert(v[Is], To{})...};
    }

    static mask_type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE is_nan(const type v) noexcept
    {
        return {Abi::is_nan(v[Is])...};
    }

    static type SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE blend(const type a, const type b, const mask_type c) noexcept
    {
        return {Abi::blend(a[Is], b[Is], c[Is])...};
    }
};

} // namespace detail
} // namespace simd
} // namespace score::cpp

#undef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_ALWAYS_INLINE

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_ARRAY_HPP
