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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_MATH_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_MATH_HPP

#include <score/private/simd/data_types.hpp>

// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4049789
#define SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE __attribute__((__always_inline__))

namespace score::cpp
{
namespace simd
{

/// @brief Returns true if v is a NaN, false otherwise
///
/// [parallel] 9.7.7 ff
template <typename Abi>
inline basic_mask<float, Abi> SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE is_nan(const basic_vec<float, Abi>& v) noexcept
{
    return basic_mask<float, Abi>{Abi::impl::is_nan(static_cast<typename Abi::impl::type>(v))};
}

/// @brief Returns true if v is a NaN, false otherwise
///
/// [parallel] 9.7.7 ff
template <typename Abi>
inline basic_mask<double, Abi> SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE is_nan(const basic_vec<double, Abi>& v) noexcept
{
    return basic_mask<double, Abi>{Abi::impl::is_nan(static_cast<typename Abi::impl::type>(v))};
}

} // namespace simd
} // namespace score::cpp

#undef SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_MATH_HPP
