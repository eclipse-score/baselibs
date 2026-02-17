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
#ifndef SCORE_LIB_SAFE_MATH_SAFE_ATOMICS_H
#define SCORE_LIB_SAFE_MATH_SAFE_ATOMICS_H

#include "score/language/safecpp/safe_atomics/error.h"

#include "score/language/safecpp/safe_math/details/addition_subtraction/addition_subtraction.h"
#include "score/memory/shared/atomic_indirector.h"
#include "score/result/result.h"

#include <atomic>
#include <type_traits>

namespace score::safe_atomics
{
namespace details
{

/// \brief Similar to std::atomic fetch_add except protects against integer overflow.
///
/// \tparam T Type of underlying atomic
/// \tparam AtomicIndirectorType when set to AtomicIndirectorReal, will dispatch to regular atomic operations. When
///         set to AtomicIndirectorMock, allows mocking of atomic behaviour for testing.
///
/// \Return Previous value of atomic before addition if addition would not lead to integer overflow. Otherwise, returns
/// an error.
template <class T,
          template <class> class AtomicIndirectorType = memory::shared::AtomicIndirectorReal,
          typename std::enable_if_t<std::is_integral<T>::value, bool> = true>
score::Result<T> TryAtomicAddImpl(std::atomic<T>& atomic, const T addition_value, const std::size_t max_retries)
{
    for (std::size_t i = 0; i < max_retries; ++i)
    {
        auto current_value = atomic.load();
        const auto addition_result = safe_math::Add(current_value, addition_value);
        if (!(addition_result.has_value()))
        {
            if (addition_result.error() == safe_math::ErrorCode::kExceedsNumericLimits)
            {
                return MakeUnexpected(ErrorCode::kExceedsNumericLimits);
            }

            // Defensive Programming. This path can only be triggered if the implementation of this function or the
            // safe_math::Add function changes
            // LCOV_EXCL_LINE (Defensive programming.)
            return MakeUnexpected(ErrorCode::kUnexpectedError, addition_result.error().Message());
        }

        const auto exchange_result = AtomicIndirectorType<T>::compare_exchange_strong(
            atomic, current_value, addition_result.value(), std::memory_order_seq_cst);
        if (exchange_result)
        {
            return current_value;
        }
    }

    return MakeUnexpected(ErrorCode::kMaxRetriesReached);
}

}  // namespace details

template <class T>
score::Result<T> TryAtomicAdd(std::atomic<T>& atomic, const T addition_value, const std::size_t max_retries = 10U)
{
    return details::TryAtomicAddImpl<T>(atomic, addition_value, max_retries);
}

}  // namespace score::safe_atomics

#endif  // SCORE_LIB_SAFE_MATH_SAFE_ATOMICS_H
