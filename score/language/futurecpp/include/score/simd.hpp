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
/// \brief Score.Futurecpp.Simd component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_SIMD_HPP
#define SCORE_LANGUAGE_FUTURECPP_SIMD_HPP

#if defined(__SSE4_2__) && (defined(__linux__) || defined(__QNX__))
#include <score/private/simd/sse42_backend.hpp>
#elif defined(__ARM_NEON) && (defined(__linux__) || defined(__QNX__))
#include <score/private/simd/aarch64_neon_backend.hpp>
#else
#include <score/private/simd/default_backend.hpp>
#endif
#include <score/private/simd/data_types.hpp> // IWYU pragma: export
#include <score/private/simd/math.hpp>       // IWYU pragma: export

#endif // SCORE_LANGUAGE_FUTURECPP_SIMD_HPP
