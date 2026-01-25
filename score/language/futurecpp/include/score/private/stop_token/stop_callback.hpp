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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_CALLBACK_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_CALLBACK_HPP

#include <score/private/stop_token/stop_state.hpp>
#include <score/private/stop_token/stop_token.hpp>
#include <score/private/thread/thread.hpp>
#include <score/private/type_traits/invoke_traits.hpp>
#include <score/move_only_function.hpp>

#include <memory>
#include <type_traits>
#include <utility>

namespace score::cpp
{

/// \brief The stop_callback class template provides an RAII object type that registers a callback function for an
/// associated score::cpp::stop_token object, such that the callback function will be invoked when the score::cpp::stop_token's
/// associated score::cpp::stop_source is requested to stop.
///
/// Callback functions registered via stop_callback's constructor are invoked either in the same thread that
/// successfully invokes request_stop() for a score::cpp::stop_source of the stop_callback's associated score::cpp::stop_token; or if
/// stop has already been requested prior to the constructor's registration, then the callback is invoked in the thread
/// constructing the stop_callback.
///
/// More than one stop_callback can be created for the same score::cpp::stop_token, from the same or different threads
/// concurrently. No guarantee is provided for the order in which they will be executed, but they will be invoked
/// synchronously; except for stop_callback(s) constructed after stop has already been requested for the
/// score::cpp::stop_token, as described previously.
///
/// If an invocation of a callback exits via an exception then std::terminate is called.
///
/// score::cpp::stop_callback is not CopyConstructible, CopyAssignable, MoveConstructible, nor MoveAssignable.
///
/// The template param Callback type must be both invocable and destructible. Any return value is ignored.
///
/// \details Deviation from the C++20 Standard
///
/// As per standard the class stop_callback shall be templated in order to support the member `callback_type` referring
/// to the respective type. At the same time the standard foresees a custom "Class template argument deduction (CTAD)"
/// in a way that the user of this library would not need to specify the class template parameter and thus could just
/// construct `score::cpp::stop_callback cb{my_token, my_callback};` The problem is that "Class template argument deduction
/// (CTAD)" is only supported from C++17 on. With us only supporting C++14, there seems no way to implement this
/// correctly. The trade-off needs to be made between either sticking to the _nice_ user API and removing the template
/// parameter and `callback_type` or keeping the template parameter and forcing the user to specify the type. We decided
/// for former, since it seems to be the more clean way.
///
/// This comes with the drawback that right now only callables with the signature `void()` can be used. The standard for
/// sees anyhow only callbacks with parameters, but allows theoretically a different return value other than void (which
/// will be thrown away). Thus, the drawback does not seem to be high.
class stop_callback
{
public:
    /// \brief Constructs a new stop_callback object, saving and registering the cb callback function into the given
    /// score::cpp::stop_token's associated stop-state, for later invocation if stop is requested on the associated
    /// score::cpp::stop_source.
    ///
    /// Constructs a stop_callback for the given st score::cpp::stop_token (copied), with the given invocable callback function
    /// cb.
    ///
    /// \details If st.stop_requested() == true for the passed-in score::cpp::stop_token, then the callback function is invoked
    /// in the current thread before the constructor returns.
    ///
    /// \tparam C the callable type
    /// \param st a score::cpp::stop_token object to register this stop_callback object with
    /// \param cb the type to invoke if stop is requested
    template <typename C>
    explicit stop_callback(const stop_token& st, C&& cb) noexcept(std::is_nothrow_constructible<C>::value)
    {
        static_assert(score::cpp::is_invocable<C>::value, "Callback needs to be invocable");
        static_assert(std::is_nothrow_destructible<C>::value, "Callback needs to be nothrow destructable");
        cb_ = std::forward<C>(cb);
        state_ = st.state_;
        if (state_ != nullptr)
        {
            state_->register_callback(this);
        }
    }

    /// \brief Constructs a stop_callback for the given st score::cpp::stop_token (moved), with the given invocable callback
    /// function cb. For more information see stop_callback(const stop_token& st, C&& cb)
    ///
    /// \tparam C the callable type
    /// \param st a score::cpp::stop_token object to register this stop_callback object with
    /// \param cb the type to invoke if stop is requested
    template <typename C>
    explicit stop_callback(stop_token&& st, C&& cb) noexcept(std::is_nothrow_constructible<C>::value)
    {
        static_assert(score::cpp::is_invocable<C>::value, "Callback needs to be invocable");
        static_assert(std::is_nothrow_destructible<C>::value, "Callback needs to be nothrow destructable");
        cb_ = std::forward<C>(cb);
        state_ = st.state_;
        if (state_ != nullptr)
        {
            state_->register_callback(this);
        }
    }

    /// \brief Destroys the stop_callback object.
    ///
    /// If *this has a stop_token with associated stop-state, deregisters the callback from it.
    ///
    /// If the callback function is being invoked concurrently on another thread, the destructor does not complete
    /// until the callback function invocation is complete. If the callback function is being invoked on the same
    /// thread the destructor is being invoked on, then the destructor returns without waiting for callback invocation
    /// to complete (see Details).
    ///
    /// \details The stop_callback destructor is designed to prevent race conditions and deadlocks.
    /// If another thread is currently invoking the callback, then the destructor cannot return until that completes,
    /// or else the function object could potentially be destroyed while it is being executed. The callback function
    /// is not required to be neither copyable nor movable - it lives in the stop_callback object itself even after
    /// registration.
    ///
    /// On the other hand, if the current thread invoking the destructor is the same thread that is invoking the
    /// callback, then the destructor cannot wait or else a deadlock would occur. It is possible and valid for the
    /// same thread to be destroying the stop_callback while it is invoking its callback function, because the callback
    /// function might itself destroy the stop_callback, directly or indirectly.
    ~stop_callback()
    {
        if (state_ != nullptr)
        {
            state_->deregister_callback(this);
        }
    }

    stop_callback(const stop_callback&) = delete;
    stop_callback(stop_callback&&) = delete;
    stop_callback& operator=(const stop_callback&) = delete;
    stop_callback& operator=(stop_callback&&) = delete;

private:
    friend detail::stop_state;

    std::shared_ptr<detail::stop_state> state_{nullptr};
    score::cpp::move_only_function<void()> cb_{};
    detail::stop_state::stop_callback_id callback_id_{detail::stop_state::default_stop_callback_id};
    bool already_started_executing_{false};
    score::cpp::thread::id executed_by_{};
    stop_callback* next_{nullptr};
    stop_callback* prev_{nullptr};
};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_CALLBACK_HPP
