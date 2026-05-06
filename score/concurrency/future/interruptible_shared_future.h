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
#ifndef SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_SHARED_FUTURE_H
#define SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_SHARED_FUTURE_H

#include "score/concurrency/future/base_interruptible_future.h"
#include "score/concurrency/future/error.h"
#include "score/result/result.h"

#include "score/expected.hpp"

namespace score
{
namespace concurrency
{
// Suppress "AUTOSAR C++14 M3-2-3" rule finding. This rule states: "A type, object or function that is used in multiple
// translation units shall be declared in one and only one file.".
// This is a forward declaration
// coverity[autosar_cpp14_m3_2_3_violation]
template <typename Value>
class InterruptibleFuture;

/**
 * Interruptible shared future used to wait for an InterruptiblePromise. Close reimplementation of std::shared_future
 * but with the capability to interrupt any call to a function that waits for the shared state to be ready.
 *
 * \tparam Value The type of the values stored in the shared state
 */
template <typename Value>
class InterruptibleSharedFuture : protected score::concurrency::detail::BaseInterruptibleFuture<Value>
{
    using BaseInterruptibleFuture = score::concurrency::detail::BaseInterruptibleFuture<Value>;

  public:
    /**
     * Constructs an InterruptibleSharedFuture without shared state.
     */
    constexpr InterruptibleSharedFuture() noexcept = default;

    /**
     * Constructs an InterruptibleSharedFuture by taking the shared state of an InterruptibleFuture using move
     * semantics. The InterruptibleFuture will be without shared state afterwards.
     *
     * \param other InterruptibleFuture to take the shared state from
     */
    explicit InterruptibleSharedFuture(InterruptibleFuture<Value>&& other)
        : score::concurrency::detail::BaseInterruptibleFuture<Value>{std::move(other)} {};

    /**
     * Waits for the shared state to become ready and then returns the value/error of the shared state.
     * Calls to this function block indefinitely until the state becomes ready. To abort prematurely, the stop token may
     * be set.
     *
     * \param stop_token Token to abort the wait prematurely.
     * \returns The value/error on success or an Error if:
     * - no shared state is associated
     * - the token was set
     * - the promise was broken
     */
    // .error() is marked noexcept due to SCORE_LANGUAGE_FUTURECPP_PRECONDITION call in it; the error() call won't throw because we
    // check for value presence
    // coverity[autosar_cpp14_a15_5_3_violation]
    score::Result<std::reference_wrapper<const Value>> Get(const score::cpp::stop_token& stop_token) noexcept
    {
        score::cpp::expected_blank<Error> return_code = this->Wait(stop_token);
        if (!return_code.has_value() == true)
        {
            return MakeUnexpected(return_code.error());
        }

        auto state_ptr = this->AccessState();
        if (state_ptr->GetValue().has_value() == true)
        {
            return state_ptr->GetValue().value();
        }
        else
        {
            return MakeUnexpected<std::reference_wrapper<const Value>>(state_ptr->GetValue().error());
        }
    }

    // coverity[autosar_cpp14_a13_3_1_violation] addressed at BaseInterruptibleFuture::Then definition
    using BaseInterruptibleFuture::Then;
    using BaseInterruptibleFuture::Valid;
    using BaseInterruptibleFuture::Wait;
    using BaseInterruptibleFuture::WaitFor;
    using BaseInterruptibleFuture::WaitUntil;

  private:
    // declared in private section to hide these methods from classes potentially inheriting from us
    using BaseInterruptibleFuture::AccessState;
    using BaseInterruptibleFuture::StealState;
};

/**
 * Interruptible shared future used to wait for an InterruptiblePromise. Close reimplementation of std::shared_future
 * but with the capability to interrupt any call to a function that waits for the shared state to be ready.
 *
 * Specialization for reference-types.
 *
 * \tparam Value The type of the values stored in the shared state
 */
template <typename Value>
class InterruptibleSharedFuture<Value&> : protected score::concurrency::detail::BaseInterruptibleFuture<Value&>
{
    using BaseInterruptibleFuture = score::concurrency::detail::BaseInterruptibleFuture<Value&>;

  public:
    /**
     * Constructs an InterruptibleSharedFuture without shared state.
     */
    constexpr InterruptibleSharedFuture() noexcept = default;

    /**
     * Constructs an InterruptibleSharedFuture by taking the shared state of an InterruptibleFuture using move
     * semantics. The InterruptibleFuture will be without shared state afterwards.
     *
     * \param other InterruptibleFuture to take the shared state from
     */
    explicit InterruptibleSharedFuture(InterruptibleFuture<Value&>&& other)
        : score::concurrency::detail::BaseInterruptibleFuture<Value&>{std::move(other)} {};

    /**
     * Waits for the shared state to become ready and then returns the value/error of the shared state.
     * Calls to this function block indefinitely until the state becomes ready. To abort prematurely, the stop token may
     * be set.
     *
     * \param stop_token Token to abort the wait prematurely.
     * \returns The value/error on success or an Error if:
     * - no shared state is associated
     * - the token was set
     * - the promise was broken
     */
    score::Result<std::reference_wrapper<Value>> Get(const score::cpp::stop_token& stop_token) const noexcept
    {
        score::cpp::expected_blank<Error> return_code = this->Wait(stop_token);
        // GCOV_EXCL_START: Decision 'true'/'false' are not taken.
        // This is a false-positive, covered by InterruptibleSharedFutureTest
        if (!return_code.has_value())
        // GCOV_EXCL_STOP
        {
            return MakeUnexpected<std::reference_wrapper<Value>>(return_code.error());
        }

        score::Result<std::reference_wrapper<Value>> result = this->AccessState()->GetValue();
        return result;
    }

    // coverity[autosar_cpp14_a13_3_1_violation] addressed at BaseInterruptibleFuture::Then definition
    using BaseInterruptibleFuture::Then;
    using BaseInterruptibleFuture::Valid;
    using BaseInterruptibleFuture::Wait;
    using BaseInterruptibleFuture::WaitFor;
    using BaseInterruptibleFuture::WaitUntil;

  private:
    // declared in private section to hide these methods from classes potentially inheriting from us
    using BaseInterruptibleFuture::AccessState;
    using BaseInterruptibleFuture::StealState;
};

/**
 * Interruptible shared future used to wait for an InterruptiblePromise. Close reimplementation of std::shared_future
 * but with the capability to interrupt any call to a function that waits for the shared state to be ready.
 *
 * Specialization for void.
 *
 * \tparam Value The type of the values stored in the shared state
 */
template <>
class InterruptibleSharedFuture<void> : protected score::concurrency::detail::BaseInterruptibleFuture<void>
{
    using BaseInterruptibleFuture = score::concurrency::detail::BaseInterruptibleFuture<void>;

  public:
    /**
     * Constructs an InterruptibleSharedFuture without shared state.
     */
    constexpr InterruptibleSharedFuture() noexcept = default;

    /**
     * Constructs an InterruptibleSharedFuture by taking the shared state of an InterruptibleFuture using move
     * semantics. The InterruptibleFuture will be without shared state afterwards.
     *
     * \param other InterruptibleFuture to take the shared state from
     */
    explicit InterruptibleSharedFuture(InterruptibleFuture<void>&& other);

    /**
     * Waits for the shared state to become ready and then returns the value/error of the shared state.
     * Calls to this function block indefinitely until the state becomes ready. To abort prematurely, the stop token may
     * be set.
     *
     * \param stop_token Token to abort the wait prematurely.
     * \returns The value/error on success or an Error if:
     * - no shared state is associated
     * - the token was set
     * - the promise was broken
     */
    score::Result<void> Get(const score::cpp::stop_token& stop_token) const noexcept;

    // coverity[autosar_cpp14_a13_3_1_violation] addressed at BaseInterruptibleFuture::Then definition
    using BaseInterruptibleFuture::Then;
    using BaseInterruptibleFuture::Valid;
    using BaseInterruptibleFuture::Wait;
    using BaseInterruptibleFuture::WaitFor;
    using BaseInterruptibleFuture::WaitUntil;

  private:
    // declared in private section to hide these methods from classes potentially inheriting from us
    using BaseInterruptibleFuture::AccessState;
    using BaseInterruptibleFuture::StealState;
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_SHARED_FUTURE_H
