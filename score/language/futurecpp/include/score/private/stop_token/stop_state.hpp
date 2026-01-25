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
/// \brief Score.Futurecpp.StopToken component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_STATE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_STATE_HPP

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>

namespace score::cpp
{

class stop_callback;

namespace detail
{

/// \brief Represents the shared state between stop_source and stop_token.
///
/// Its two main responsibilities are to hold the atomic state if a stop has been requested and to execute any
/// registered stop_callback if the former happened. In order to fulfill further requirements, also a source reference
/// count is necessary, to determine if a stop for stop_token is still possible.
class stop_state
{
private:
    friend stop_callback;

    /// \brief Provides a type alias for the id of a stop_callback.
    using stop_callback_id = std::uint64_t;

    /// \brief Provides the stop_callback_id value representing a stop_callback that did not get any such id assigned.
    static constexpr stop_callback_id default_stop_callback_id{0U};

    /// \brief Associates a callback that will be invoked once request_stop() is invoked.
    ///
    /// \param cb The callback to associate with the state
    void register_callback(stop_callback* const cb);

    /// \brief Removes the association of a callback with this state. Afterwards, an invocation of request_stop()
    /// will no longer invoke the callback.
    ///
    /// \param cb The callback to remove the association with this state
    void deregister_callback(stop_callback* const cb);

public:
    /// \brief Invokes associated callbacks and changes internal state to stop requested, such that
    /// stop_requested() will return true
    ///
    /// \return This call returns true if not invoked before
    bool request_stop();

    /// \brief Accessor to the internal state
    ///
    /// \return This call returns true if request_stop() has been invoked before
    bool stop_requested() { return stop_requested_.load(std::memory_order_acquire); }

    /// \brief Checks if stop_requested() can ever change its state.
    ///
    /// \return This call returns true, as long as at least one stop_source is associated with the state.
    bool stop_possible() { return associated_sources_.load(std::memory_order_acquire) != 0; }

    /// \brief Gets invoked once a new stop_source is associated with this state
    ///
    /// \details Necessary to implement stop_possible() functionality.
    void increment_associated_sources() { ++associated_sources_; }

    /// \brief Get invoked once a stop_source removes its association with this state
    ///
    /// \details Necessary to implement stop_possible() functionality.
    void decrement_associated_sources() { --associated_sources_; }

private:
    /// \brief Invokes all registered callbacks
    void notify_registered_callbacks();

    /// \brief Executes the callback while unlocking the provided guard. As a side effect, respective states within cb
    /// will be updated.
    ///
    /// \detail We have to unlock the guard to enable the use case of a callback that destroys itself.
    ///
    /// \param guard The guard that will be unlocked while executing cb
    /// \param cb The callback that will be executed
    void execute(std::unique_lock<std::mutex>& guard, stop_callback* const cb);

    std::atomic<bool> stop_requested_{false};
    std::mutex callback_mutex_{};
    stop_callback* first_callback_{nullptr};
    std::atomic<std::size_t> associated_sources_{0};

    stop_callback_id currently_executing_callback_{default_stop_callback_id}; // default id: no callback is executing
                                                                              // other id: the executed callback's id
    stop_callback_id next_callback_id_{1};
    std::condition_variable wakeup_{};
};

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_STATE_HPP
