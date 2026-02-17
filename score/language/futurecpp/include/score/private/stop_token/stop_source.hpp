/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_SOURCE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_SOURCE_HPP

#include <score/private/stop_token/nostopstate_t.hpp>
#include <score/private/stop_token/stop_state.hpp>
#include <score/private/stop_token/stop_token.hpp>

#include <memory>
#include <utility>

namespace score::cpp
{

/// \brief The stop_source class provides the means to issue a stop request, such as for score::cpp::jthread cancellation. A
/// stop request made for one stop_source object is visible to all stop_sources and score::cpp::stop_tokens of the same
/// associated stop-state; any score::cpp::stop_callback(s) registered for associated score::cpp::stop_token(s) will be invoked,
/// and any std::condition_variable_any objects waiting on associated score::cpp::stop_token(s) will be awoken.
///
/// Once a stop is requested, it cannot be withdrawn. Additional stop requests have no effect.
///
/// \details For the purposes of score::cpp::jthread cancellation the stop_source object should be retrieved from the
/// score::cpp::jthread object using get_stop_source(); or stop should be requested directly from the score::cpp::jthread object using
/// request_stop(). This will then use the same associated stop-state as that passed into the score::cpp::jthread's invoked
/// function argument (i.e., the function being executed on its thread).
///
/// For other uses, however, a stop_source can be constructed separately using the default constructor, which creates
/// new stop-state.
class stop_source
{
public:
    /// \brief Constructs a stop_source with new stop-state.
    ///
    /// \post stop_possible() is true and stop_requested() is false
    stop_source() : state_{std::make_shared<detail::stop_state>()} { state_->increment_associated_sources(); }

    /// \brief Constructs an empty stop_source with no associated stop-state.
    ///
    /// \post stop_possible() and stop_requested() are both false
    explicit stop_source(nostopstate_t) noexcept {}

    /// \brief Copy constructor. Constructs a stop_source whose associated stop-state is the same as that of other.
    ///
    /// \param other another stop_source object to construct this stop_source object with
    /// \post *this and other share the same associated stop-state and compare equal
    stop_source(const stop_source& other) noexcept
    {
        state_ = other.state_;
        if (state_ != nullptr)
        {
            state_->increment_associated_sources();
        }
    }

    /// \brief Move constructor. Constructs a stop_source whose associated stop-state is the same as that of other;
    /// other is left empty.
    ///
    /// \param other another stop_source object to construct this stop_source object with
    /// \post *this has other's previously associated stop-state, and other.stop_possible() is false
    stop_source(stop_source&& other) noexcept = default;

    /// \brief Copy-assigns the stop-state of other to that of *this. Equivalent to stop_source(other).swap(*this).
    ///
    /// \param other another stop_source object to share the stop-state with to or acquire the stop-state from
    /// \return Constructed stop_source
    // NOLINTNEXTLINE(bugprone-unhandled-self-assignment) see comment in function body
    stop_source& operator=(const stop_source& other) noexcept
    {
        // Handle self-assignment by first incrementing `other.state_`, decrementing `this->state_` and then copy. This
        // avoids an observable side effect via `stop_token::stop_possible()` of decrementing first. "Equivalent to
        // stop_source(other).swap(*this)" also first creates a copy (`stop_source(other)`) which is equivalent to
        // incrementing.
        if (other.state_ != nullptr)
        {
            other.state_->increment_associated_sources();
        }
        if (state_ != nullptr)
        {
            state_->decrement_associated_sources();
        }
        state_ = other.state_;
        return *this;
    }

    /// \brief Move-assigns the stop-state of other to that of *this. After the assignment, *this contains the previous
    /// stop-state of other, and other has no stop-state. Equivalent to stop_source(std::move(other)).swap(*this)
    ///
    /// \param other another stop_source object to share the stop-state with to or acquire the stop-state from
    /// \return Constructed stop_source
    stop_source& operator=(stop_source&& other) noexcept
    {
        if (state_ != nullptr)
        {
            state_->decrement_associated_sources();
        }
        state_ = std::move(other.state_);
        return *this;
    }

    /// \brief Destroys the stop_source object.
    ///
    /// If *this has associated stop-state, releases ownership of it.
    ~stop_source()
    {
        if (state_ != nullptr)
        {
            state_->decrement_associated_sources();
        }
    }

    /// \brief Issues a stop request to the stop-state, if the stop_source object has stop-state and it has not yet
    /// already had stop requested.
    ///
    /// The determination is made atomically, and if stop was requested, the stop-state is atomically updated to avoid
    /// race conditions, such that:
    ///
    /// * stop_requested() and stop_possible() can be concurrently invoked on other stop_tokens and stop_sources of the
    /// same stop-state
    ///
    /// * request_stop() can be concurrently invoked on other stop_source objects, and only one will
    /// actually perform the stop request
    /// However, see the Details section.
    ///
    /// \details If the request_stop() does issue a stop request (i.e., returns true), then any stop_callbacks
    /// registered for the same associated stop-state will be invoked synchronously, on the same thread request_stop()
    /// is issued on. If an invocation of a callback exits via an exception, std::terminate is called.
    ///
    /// If the stop_source object has stop-state but a stop request has already been made, this function returns false.
    /// However there is no guarantee that another stop_source object which has just (successfully) requested stop is
    /// not still in the middle of invoking a stop_callback function.
    ///
    /// If the request_stop() does issue a stop request (i.e., returns true), then all condition variables of base
    /// type std::condition_variable_any registered with an interruptible wait for stop_tokens associated with
    /// the stop_source's stop-state will be notified.
    ///
    /// \post stop_possible() is false or stop_requested() is true
    /// \return true if the stop_source object has stop-state and this invocation made a stop request, otherwise false
    bool request_stop() noexcept
    {
        if (state_ != nullptr)
        {
            return state_->request_stop();
        }
        return false;
    }

    /// \brief Exchanges the stop-state of *this and other.
    ///
    /// \param other stop_source to exchange the contents with
    void swap(stop_source& other) noexcept { std::swap(state_, other.state_); }

    /// \brief Returns a stop_token object associated with the stop_source's stop-state, if the stop_source has
    /// stop-state; otherwise returns a default-constructed (empty) stop_token.
    ///
    /// \return A stop_token object, which will be empty if stop_possible() == false
    stop_token get_token() const noexcept { return stop_token(state_); }

    /// \brief Checks if the stop_source object has stop-state and that state has received a stop request.
    ///
    /// \return true if the stop_token object has stop-state and it received a stop request, false otherwise
    bool stop_requested() const noexcept { return state_ != nullptr && state_->stop_requested(); }

    /// \brief Checks if the stop_source object has stop-state.
    ///
    /// \details If the stop_source object has stop-state and a stop request has already been made,
    /// this function still returns true.
    ///
    /// \return true if the stop_source object has stop-state, otherwise false
    bool stop_possible() const noexcept { return state_ != nullptr; }

    /// \brief Compares two stop_source objects.
    ///
    /// This function is not visible to ordinary unqualified or qualified lookup, and can only be found by
    /// argument-dependent lookup when score::cpp::stop_source is an associated class of the arguments.
    ///
    /// \param lhs stop_sources to compare
    /// \param rhs stop_sources to compare
    /// \return true if lhs and rhs have the same stop-state, or both have no stop-state, otherwise false
    friend bool operator==(const stop_source& lhs, const stop_source& rhs) noexcept { return lhs.state_ == rhs.state_; }

    /// \brief Compares two stop_source objects.
    ///
    /// This function is not visible to ordinary unqualified or qualified lookup, and can only be found by
    /// argument-dependent lookup when score::cpp::stop_source is an associated class of the arguments.
    ///
    /// \param lhs stop_sources to compare
    /// \param rhs stop_sources to compare
    /// \return true if lhs and rhs have a different stop-state, otherwise false
    friend bool operator!=(const stop_source& lhs, const stop_source& rhs) noexcept { return !(lhs == rhs); }

    /// \brief Overloads the std::swap algorithm for score::cpp::stop_source. Exchanges the stop-state of lhs with that of rhs.
    /// Effectively calls lhs.swap(rhs).
    ///
    /// This function is not visible to ordinary unqualified or qualified lookup, and can only be found
    /// by argument-dependent lookup when score::cpp::stop_source is an associated class of the arguments.
    ///
    /// \param lhs stop_sources to swap
    /// \param rhs stop_sources to swap
    friend void swap(stop_source& lhs, stop_source& rhs) noexcept { std::swap(lhs.state_, rhs.state_); }

private:
    std::shared_ptr<detail::stop_state> state_{nullptr};
};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_SOURCE_HPP
