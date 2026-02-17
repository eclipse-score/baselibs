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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_SCOPED_FUNCTION_INVOKER_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_SCOPED_FUNCTION_INVOKER_H

#include "score/language/safecpp/scoped_function/details/call_and_return_modified.h"

#include <score/assert.hpp>

namespace score::safecpp::details
{

template <bool NoExcept, class FunctionType>
class ScopedFunctionInvoker;

template <bool NoExcept, class ReturnType, class... Args>
class ScopedFunctionInvoker<NoExcept, ReturnType(Args...)>
{
  public:
    template <class ScopedFunction>
    using CallableType = ReturnType (*)(ScopedFunction&, Args...) noexcept(NoExcept);

    template <class ScopedFunction>
    [[nodiscard]] static CallableType<ScopedFunction> ActualInvoker() noexcept
    {
        return [](ScopedFunction& base_scoped_function, Args... args) noexcept(NoExcept) -> ReturnType {
            // LCOV_EXCL_BR_START False Positive:
            // 1. The return statement inside the following if statement is covered as the report shows - this implies
            // that the "true" path was taken
            // 2. Test cases exist which test all possible combinations:
            //      Scope State and Callable ok
            //      Scope State Empty, Callable Ok
            //      Scope State ok, Callable Empty
            //      Scope State Empty, Callable Empty
            if ((base_scoped_function.scope_state_ == nullptr) || (base_scoped_function.callable_ == nullptr))
            {
                return {};
            }
            // LCOV_EXCL_BR_STOP

            auto wrapped_callable = [&base_scoped_function, &args...]() {
                SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(base_scoped_function.callable_ != nullptr);
                return details::CallAndReturnModified<Args...>(*(base_scoped_function.callable_),
                                                               std::forward<Args>(args)...);
            };

            return base_scoped_function.scope_state_->InvokeIfNotExpired(wrapped_callable);
        };
    }
};

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_SCOPED_FUNCTION_INVOKER_H
