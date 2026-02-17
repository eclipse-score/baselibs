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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_SCOPE_STATE_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_SCOPE_STATE_H

#include <score/optional.hpp>
#include <score/stop_token.hpp>

#include <functional>
#include <optional>
#include <shared_mutex>
#include <type_traits>

namespace score::safecpp::details
{

class ScopeState final
{
  public:
    // Suppress "AUTOSAR C++14 A12-1-5" rule finding. This rule states:"Common class initialization for non-constant
    // members shall be done by a delegating constructor.".
    // Rationale: Delegation to a separate constructor complexifies the code significantly.
    // This adds more risk than doing the initialization of the members in each constructor.
    // coverity[autosar_cpp14_a12_1_5_violation : FALSE]
    ScopeState() noexcept : expiration_mutex_{}, expired_{false}, expiration_callback_{std::nullopt} {}
    ~ScopeState() noexcept = default;

    // Suppress "AUTOSAR C++14 A12-1-5" rule finding. This rule states:"Common class initialization for non-constant
    // members shall be done by a delegating constructor.".
    // Rationale: Delegation to a separate constructor complexifies the code significantly.
    // This adds more risk than doing the initialization of the members in each constructor.
    // coverity[autosar_cpp14_a12_1_5_violation : FALSE]
    explicit ScopeState(const score::cpp::stop_token& stop_token) noexcept
        : expiration_mutex_{}, expired_{false}, expiration_callback_{std::in_place, stop_token, [this]() noexcept {
                                                                         Expire();
                                                                     }}
    {
    }

    ScopeState(const ScopeState&) = delete;
    ScopeState& operator=(const ScopeState& other) = delete;

    ScopeState(ScopeState&& other) noexcept = delete;
    ScopeState& operator=(ScopeState&& other) noexcept = delete;

    void Expire() noexcept;

    template <class Callable, class ReturnType = std::invoke_result_t<Callable>>
    [[nodiscard]] score::cpp::optional<ReturnType> InvokeIfNotExpired(Callable& callable)
    {
        // Suppress "AUTOSAR C++14 M0-1-9" rule finding. This rule states: "There shall be no dead code.".
        // Rationale: The code is not dead, but the lock is used to protect the shared state.
        // Suppress "AUTOSAR C++14 M0-1-3" rule finding: "A project shall not contain unused variables."
        // Rationale: The lock is used to protect the shared state and it releases the lock when it goes out of scope.
        // Explicitly allow multiple simultaneous invocations. If a user wants to prohibit simultaneous calls, he must
        // synchronize the function itself.
        // coverity[autosar_cpp14_m0_1_9_violation : FALSE]
        // coverity[autosar_cpp14_m0_1_3_violation : FALSE]
        std::shared_lock<std::shared_mutex> lock{expiration_mutex_};

        if (!expired_)
        {
            return callable();
        }
        return {score::cpp::nullopt};
    }

  private:
    std::shared_mutex expiration_mutex_;
    bool expired_;
    std::optional<score::cpp::stop_callback> expiration_callback_;
};

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_SCOPE_STATE_H
