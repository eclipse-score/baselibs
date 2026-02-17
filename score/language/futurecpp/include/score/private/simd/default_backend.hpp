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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_DEFAULT_BACKEND_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_DEFAULT_BACKEND_HPP

#include <score/private/simd/abi.hpp>
#include <score/private/simd/array.hpp>

#include <score/assert.hpp>
#include <score/float.hpp>
#include <score/math.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace simd
{
namespace detail
{
namespace scalar
{

struct mask_backend
{
    using type = bool;
    static constexpr std::size_t width{1};

    static bool broadcast(const bool v) noexcept { return v; }

    template <typename G, std::size_t I>
    static bool init(G&& gen, const std::index_sequence<I>) noexcept
    {
        return gen(std::integral_constant<std::size_t, I>{});
    }

    static bool extract(const bool v, const size_t i) noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(i == 0U);
        return v;
    }

    static bool logical_not(const bool v) noexcept { return !v; }

    static bool logical_and(const bool a, const bool b) noexcept { return a && b; }

    static bool logical_or(const bool a, const bool b) noexcept { return a || b; }

    static bool all_of(const bool v) noexcept { return v; }

    static bool any_of(const bool v) noexcept { return v; }

    static bool none_of(const bool v) noexcept { return !v; }
};

template <typename T>
struct backend
{
    using type = T;
    using mask_type = typename mask_backend::type;
    static constexpr std::size_t width{1};

    static T broadcast(const T v) noexcept { return v; }

    template <typename G, std::size_t I>
    static T init(G&& gen, const std::index_sequence<I>) noexcept
    {
        return gen(std::integral_constant<std::size_t, I>{});
    }

    static T load(const T* const v) { return v[0]; }

    static T load_aligned(const T* const v) { return load(v); }

    static void store(T* const v, const T a) { v[0] = a; }

    static void store_aligned(T* const v, const T a) { store(v, a); }

    static T extract(const T v, const std::size_t i) noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(i == 0U);
        return v;
    }

    static T add(const T a, const T b) noexcept { return a + b; }

    static T subtract(const T a, const T b) noexcept { return a - b; }

    static T multiply(const T a, const T b) noexcept { return a * b; }

    static T divide(const T a, const T b) noexcept { return a / b; }

    static T negate(const T v) noexcept { return -v; }

    static bool equal(const T a, const T b) noexcept { return score::cpp::equals_bitexact(a, b); }

    static bool not_equal(const T a, const T b) noexcept { return !score::cpp::equals_bitexact(a, b); }

    static bool less_than(const T a, const T b) noexcept { return a < b; }

    static bool less_equal(const T a, const T b) noexcept { return a <= b; }

    static bool greater_than(const T a, const T b) noexcept { return a > b; }

    static bool greater_equal(const T a, const T b) noexcept { return a >= b; }

    static T min(const T a, const T b) noexcept { return std::min(a, b); }

    static T max(const T a, const T b) noexcept { return std::max(a, b); }

    template <typename To>
    static To convert(const T v, To) noexcept
    {
        static_assert(std::is_arithmetic<To>::value, "not an arithmetic type");
        return static_cast<To>(v);
    }

    static bool is_nan(const T v) noexcept
    {
        static_assert(std::is_floating_point<T>::value, "not a floating point type");
        return score::cpp::isnan(v);
    }

    static T blend(const T a, const T b, const bool c) noexcept { return c ? b : a; }
};

template <typename T, std::size_t... Is>
struct abi
{
    using impl = array<T, backend<T>, mask_backend, Is...>;
    using mask_impl = array_mask<T, mask_backend, Is...>;
};

template <typename T, std::size_t... Is>
constexpr auto make_native_abi_dispatch(std::index_sequence<Is...>)
{
    static_assert(std::is_arithmetic<T>::value, "not an arithmetic type");
    return abi<T, Is...>{};
}

template <typename T>
constexpr auto number_of_elements()
{
    static_assert(std::is_arithmetic<T>::value, "not an arithmetic type");
    // vector registers have a fixed length (for example 64 Bits). Define the biggest type to fit into one register.
    // The smaller types then fit number of times into the register:
    return std::make_index_sequence<sizeof(std::uint64_t) / sizeof(T)>{};
}

template <typename T>
using deduce_native_abi = decltype(score::cpp::simd::detail::scalar::make_native_abi_dispatch<T>(number_of_elements<T>()));

} // namespace scalar

template <>
struct native_abi<std::uint8_t>
{
    using type = scalar::deduce_native_abi<std::uint8_t>;
};
template <>
struct native_abi<std::int32_t>
{
    using type = scalar::deduce_native_abi<std::int32_t>;
};
template <>
struct native_abi<float>
{
    using type = scalar::deduce_native_abi<float>;
};
template <>
struct native_abi<double>
{
    using type = scalar::deduce_native_abi<double>;
};

template <>
struct deduce_abi<std::int32_t, 2>
{
    using type = native_abi<std::int32_t>::type;
};
template <>
struct deduce_abi<float, 2>
{
    using type = native_abi<float>::type;
};
template <>
struct deduce_abi<float, 8>
{
    using type = scalar::abi<float, 0, 1, 2, 3, 4, 5, 6, 7>;
};
template <>
struct deduce_abi<double, 1>
{
    using type = native_abi<double>::type;
};

template <>
struct is_abi_tag<scalar::abi<std::int32_t, 0, 1>> : std::true_type
{
};
template <>
struct is_abi_tag<scalar::abi<float, 0, 1>> : std::true_type
{
};
template <>
struct is_abi_tag<scalar::abi<float, 0, 1, 2, 3, 4, 5, 6, 7>> : std::true_type
{
};
template <>
struct is_abi_tag<scalar::abi<double, 0>> : std::true_type
{
};

} // namespace detail
} // namespace simd
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_DEFAULT_BACKEND_HPP
