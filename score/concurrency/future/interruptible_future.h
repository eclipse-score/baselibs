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
#ifndef SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_FUTURE_H
#define SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_FUTURE_H

#include "score/concurrency/future/base_interruptible_future.h"
#include "score/concurrency/future/interruptible_shared_future.h"
#include "score/concurrency/future/interruptible_state.h"
#include "score/result/result.h"

#include "score/expected.hpp"

namespace score
{
namespace concurrency
{
namespace detail
{
// Suppress "AUTOSAR C++14 M3-2-3" rule finding. This rule states: "A type, object or function that is used in multiple
// translation units shall be declared in one and only one file.".
// This is a forward declaration
// coverity[autosar_cpp14_m3_2_3_violation]
template <typename Value>
class BaseInterruptiblePromise;
}  // namespace detail
// This is a forward declaration
// coverity[autosar_cpp14_m3_2_3_violation]
template <typename Value>
class InterruptibleSharedFuture;

/**
 * Interruptible future used to wait for an InterruptiblePromise. Close reimplementation of std::future but with the
 * capability to interrupt any call to a function that waits for the shared state to be ready.
 *
 * \tparam Value The type of the values stored in the shared state
 */
template <typename Value>
// Suppress "AUTOSAR C++14 M3-2-3" rule finding.
// This rule states: "A type, object or function that is used in multiple translation units shall
// be declared in one and only one file.".
// Justification: Tolerated: the coverity tool raises this finding because this class has different
// template specializations.
// coverity[autosar_cpp14_m3_2_3_violation]
// coverity[autosar_cpp14_a12_1_6_violation] keeping constructors for documentation
class InterruptibleFuture : protected score::concurrency::detail::BaseInterruptibleFuture<Value>
{
    // intentional usage; may generate more instantiations, but is harmless
    // coverity[autosar_cpp14_a5_1_7_violation]
    using BaseInterruptibleFuture = score::concurrency::detail::BaseInterruptibleFuture<Value>;

    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Required to enable InterruptiblePromise::GetFuture() to share its state with InterruptibleFuture. We use friend
    // classes here to stick closely to the standard, that does not list the necessary constructor to do this without
    // friendship.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class detail::BaseInterruptiblePromise<Value>;

    // Required for InterruptibleSharedFuture to be able to access our protected base class.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class InterruptibleSharedFuture<Value>;

    // Required for BaseInterruptibleFuture to be able to access our protected base class.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend BaseInterruptibleFuture;

  public:
    /**
     * Constructs an InterruptibleFuture without shared state.
     */
    constexpr InterruptibleFuture() noexcept = default;

    ~InterruptibleFuture() noexcept override = default;

    InterruptibleFuture(const InterruptibleFuture<Value>&) noexcept = delete;

    InterruptibleFuture& operator=(const InterruptibleFuture<Value>&) noexcept = delete;

    /**
     * Move constructor. Constructs an InterruptibleFuture with the shared state of another InterruptibleFuture using
     * move semantics. The other InterruptibleFuture will be without shared state afterwards.
     *
     * \param other The InterruptibleFuture to take the shared state from
     */
    // using explicit std::move to test functionality while avoiding slicing
    // coverity[autosar_cpp14_a12_8_6_violation]
    InterruptibleFuture(InterruptibleFuture<Value>&& other) noexcept = default;

    /**
     * Move assignment. Assigns the shared state of another InterruptibleFuture to *this using move semantics.
     * The other InterruptibleFuture will be without shared state afterwards.
     *
     * \param other The InterruptibleFuture to take the shared state from
     */
    // using explicit std::move to test functionality while avoiding slicing
    // coverity[autosar_cpp14_a12_8_6_violation]
    InterruptibleFuture& operator=(InterruptibleFuture<Value>&&) noexcept = default;

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
    score::Result<Value> Get(const score::cpp::stop_token& stop_token) noexcept
    {
        score::cpp::expected_blank<Error> return_code = this->Wait(stop_token);
        if (!return_code.has_value())
        {
            return MakeUnexpected(return_code.error());
        }

        score::Result<Value> value = std::move(this->StealState()->GetValue());
        return value;
    }

    /**
     * Creates an InterruptibleSharedFuture associated with the shared state of this future.
     * Invalidates the shared state of *this.
     *
     * \returns Shared future associated with the same state as *this
     */
    // intentional usage; may generate more instantiations, but is harmless
    // coverity[autosar_cpp14_a5_1_7_violation]
    InterruptibleSharedFuture<Value> Share() noexcept
    {
        const InterruptibleSharedFuture<Value> shared_future{std::move(*this)};
        return shared_future;
    }

    // coverity[autosar_cpp14_a13_3_1_violation] addressed at BaseInterruptibleFuture::Then definition
    using BaseInterruptibleFuture::Then;
    using BaseInterruptibleFuture::Valid;
    using BaseInterruptibleFuture::Wait;
    using BaseInterruptibleFuture::WaitFor;
    using BaseInterruptibleFuture::WaitUntil;

  private:
    explicit InterruptibleFuture(std::shared_ptr<InterruptibleState<Value>> state_ptr) noexcept
        : score::concurrency::detail::BaseInterruptibleFuture<Value>{std::move(state_ptr)}
    {
    }

    // declared in private section to hide these methods from classes potentially inheriting from us
    using BaseInterruptibleFuture::AccessState;
    using BaseInterruptibleFuture::StealState;
};

/**
 * Interruptible future used to wait for an InterruptiblePromise. Close reimplementation of std::future but with the
 * capability to interrupt any call to a function that waits for the shared state to be ready.
 *
 * Specialization for reference-types.
 *
 * \tparam Value The type of the values stored in the shared state
 */
template <typename Value>
class InterruptibleFuture<Value&> : protected score::concurrency::detail::BaseInterruptibleFuture<Value&>
{
    using BaseInterruptibleFuture = score::concurrency::detail::BaseInterruptibleFuture<Value&>;
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Required to enable InterruptiblePromise::GetFuture() to share its state with InterruptibleFuture. We use friend
    // classes here to stick closely to the standard, that does not list the necessary constructor to do this without
    // friendship.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class detail::BaseInterruptiblePromise<Value&>;

    // Required for InterruptibleSharedFuture to be able to access our protected base class.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class InterruptibleSharedFuture<Value&>;

    // Required for BaseInterruptibleFuture to be able to access our protected base class.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend BaseInterruptibleFuture;

  public:
    /**
     * Constructs an InterruptibleFuture without shared state.
     */
    constexpr InterruptibleFuture() noexcept = default;

    ~InterruptibleFuture() noexcept override = default;

    InterruptibleFuture(const InterruptibleFuture<Value&>&) noexcept = delete;

    InterruptibleFuture& operator=(const InterruptibleFuture<Value&>&) noexcept = delete;

    /**
     * Move constructor. Constructs an InterruptibleFuture with the shared state of another InterruptibleFuture using
     * move semantics. The other InterruptibleFuture will be without shared state afterwards.
     *
     * \param other The InterruptibleFuture to take the shared state from
     */
    InterruptibleFuture(InterruptibleFuture<Value&>&& other) noexcept = default;

    /**
     * Move assignment. Assigns the shared state of another InterruptibleFuture to *this using move semantics.
     * The other InterruptibleFuture will be without shared state afterwards.
     *
     * \param other The InterruptibleFuture to take the shared state from
     */
    InterruptibleFuture& operator=(InterruptibleFuture<Value&>&&) noexcept = default;

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
    score::Result<std::reference_wrapper<Value>> Get(const score::cpp::stop_token& stop_token) noexcept
    {
        score::cpp::expected_blank<Error> return_code = this->Wait(stop_token);
        if (!return_code.has_value())
        {
            return MakeUnexpected(return_code.error());
        }

        score::Result<std::reference_wrapper<Value>> result = this->StealState()->GetValue();
        return result;
    }

    /**
     * Creates an InterruptibleSharedFuture associated with the shared state of this future.
     * Invalidates the shared state of *this.
     *
     * \returns Shared future associated with the same state as *this
     */
    InterruptibleSharedFuture<Value&> Share() noexcept
    {
        const InterruptibleSharedFuture<Value&> shared_future{std::move(*this)};
        return shared_future;
    }

    // coverity[autosar_cpp14_a13_3_1_violation] addressed at BaseInterruptibleFuture::Then definition
    using BaseInterruptibleFuture::Then;
    using BaseInterruptibleFuture::Valid;
    using BaseInterruptibleFuture::Wait;
    using BaseInterruptibleFuture::WaitFor;
    using BaseInterruptibleFuture::WaitUntil;

  private:
    explicit InterruptibleFuture(std::shared_ptr<InterruptibleState<Value&>> state_ptr) noexcept
        : score::concurrency::detail::BaseInterruptibleFuture<Value&>{std::move(state_ptr)}
    {
    }

    // declared in private section to hide these methods from classes potentially inheriting from us
    using BaseInterruptibleFuture::AccessState;
    using BaseInterruptibleFuture::StealState;
};

/**
 * Interruptible future used to wait for an InterruptiblePromise. Close reimplementation of std::future but with the
 * capability to interrupt any call to a function that waits for the shared state to be ready.
 *
 * Specialization for void.
 */
template <>
// constructor inheritance is pointless here since we delete copy ctr + use ctr declarations for documentation
// coverity[autosar_cpp14_a12_1_6_violation]
class InterruptibleFuture<void> : protected score::concurrency::detail::BaseInterruptibleFuture<void>
{
    using BaseInterruptibleFuture = score::concurrency::detail::BaseInterruptibleFuture<void>;

    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Required to enable InterruptiblePromise::GetFuture() to share its state with InterruptibleFuture. We use friend
    // classes here to stick closely to the standard, that does not list the necessary constructor to do this without
    // friendship.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class detail::BaseInterruptiblePromise<void>;

    // Required for InterruptibleSharedFuture to be able to access our protected base class.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class InterruptibleSharedFuture<void>;

    // Required for BaseInterruptibleFuture to be able to access our protected base class.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend BaseInterruptibleFuture;

  public:
    /**
     * Constructs an InterruptibleFuture without shared state.
     */
    constexpr InterruptibleFuture() noexcept = default;

    ~InterruptibleFuture() noexcept override = default;

    InterruptibleFuture(const InterruptibleFuture<void>&) noexcept = delete;

    InterruptibleFuture& operator=(const InterruptibleFuture<void>&) noexcept = delete;

    /**
     * Move constructor. Constructs an InterruptibleFuture with the shared state of another InterruptibleFuture using
     * move semantics. The other InterruptibleFuture will be without shared state afterwards.
     *
     * \param other The InterruptibleFuture to take the shared state from
     */
    InterruptibleFuture(InterruptibleFuture<void>&& other) noexcept = default;

    /**
     * Move assignment. Assigns the shared state of another InterruptibleFuture to *this using move semantics.
     * The other InterruptibleFuture will be without shared state afterwards.
     *
     * \param other The InterruptibleFuture to take the shared state from
     */
    InterruptibleFuture& operator=(InterruptibleFuture<void>&&) noexcept = default;

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
    score::Result<void> Get(const score::cpp::stop_token& stop_token) noexcept;

    /**
     * Creates an InterruptibleSharedFuture associated with the shared state of this future.
     * Invalidates the shared state of *this.
     *
     * \returns Shared future associated with the same state as *this
     */
    InterruptibleSharedFuture<void> Share() noexcept;

    // coverity[autosar_cpp14_a13_3_1_violation] addressed at BaseInterruptibleFuture::Then definition
    using BaseInterruptibleFuture::Then;
    using BaseInterruptibleFuture::Valid;
    using BaseInterruptibleFuture::Wait;
    using BaseInterruptibleFuture::WaitFor;
    using BaseInterruptibleFuture::WaitUntil;

  private:
    explicit InterruptibleFuture(std::shared_ptr<InterruptibleState<void>> state_ptr) noexcept;

    // declared in private section to hide these methods from classes potentially inheriting from us
    using BaseInterruptibleFuture::AccessState;
    using BaseInterruptibleFuture::StealState;
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_FUTURE_H
