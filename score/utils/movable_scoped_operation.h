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
#ifndef SCORE_UTILS_MOVABLE_SCOPED_OPERATION_H
#define SCORE_UTILS_MOVABLE_SCOPED_OPERATION_H

#include "score/utils/src/scoped_operation.h"

#include <score/callback.hpp>

#include <memory>
#include <type_traits>

namespace score::utils
{

/// @brief A movable wrapper around ScopedFunction. Ensures that the callback is only called on destruction of a
/// MovableScopedOperation once by transferring ownership of the callable when moving the MovableScopedOperation.
template <typename CallbackType = score::cpp::callback<void()>>
class MovableScopedOperation final
{
    static_assert(std::is_invocable_v<CallbackType>);

  public:
    explicit MovableScopedOperation(CallbackType fn)
        : scoped_operation_{std::make_unique<ScopedOperation<CallbackType>>(std::move(fn))}
    {
    }

  private:
    std::unique_ptr<ScopedOperation<CallbackType>> scoped_operation_;
};

}  // namespace score::utils

#endif  // SCORE_UTILS_MOVABLE_SCOPED_OPERATION_H
