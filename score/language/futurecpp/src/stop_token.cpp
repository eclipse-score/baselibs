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

#include <score/stop_token.hpp>

#include <score/private/thread/this_thread.hpp>
#include <mutex>
#include <score/assert.hpp>

namespace score::cpp
{
namespace detail
{

void stop_state::register_callback(stop_callback* const cb)
{
    std::unique_lock<std::mutex> lock{callback_mutex_};
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(!cb->already_started_executing_);
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(cb->prev_ == nullptr);
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(cb->next_ == nullptr);
    if (stop_requested_)
    {
        // Since we are in a state here where stop_requested() got already called, the stop_callback
        // requested to be registered here must be executed immediately. This must NOT be done by
        // utilizing our execute() method since that one would overwrite the value currently
        // assigned to currently_executing_callback_ by the handler currently invoking the
        // callbacks which are contained in our callback list (i.e. first_callback_).
        cb->callback_id_ = default_stop_callback_id;
        cb->executed_by_ = score::cpp::this_thread::get_id();
        cb->already_started_executing_ = true;
        lock.unlock();
        cb->cb_();
    }
    else
    {
        // NOTE: Below logic could be simplified and be made more efficient by using push_front logic instead of append.
        //       Even though this simplification would reverse the later invocation order of stop_callbacks, this would
        //       be permitted since the invocation order of stop_callbacks is unspecified according to the C++ standard.
        //       See https://en.cppreference.com/w/cpp/thread/stop_callback for further details about this topic.
        //       However, we skip this for now since that would be a severe change of the current behavior!
        cb->callback_id_ = next_callback_id_++;
        if (first_callback_ == nullptr)
        {
            first_callback_ = cb;
        }
        else
        {
            stop_callback* next_callback = first_callback_;
            while (next_callback->next_ != nullptr)
            {
                next_callback = next_callback->next_;
            }
            next_callback->next_ = cb;
            cb->prev_ = next_callback;
        }
    }
}

void stop_state::execute(std::unique_lock<std::mutex>& guard, stop_callback* const cb)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(cb->callback_id_ != 0U);
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(currently_executing_callback_ == 0U);
    currently_executing_callback_ = cb->callback_id_;
    cb->already_started_executing_ = true;
    cb->executed_by_ = score::cpp::this_thread::get_id();
    guard.unlock();
    cb->cb_();
    guard.lock();
    currently_executing_callback_ = default_stop_callback_id;
    this->wakeup_.notify_all();
}

void stop_state::deregister_callback(stop_callback* const cb)
{
    if (cb->callback_id_ == default_stop_callback_id)
    {
        // nothing to be done here since 'cb' never got added to our list of stop_callbacks
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(cb->prev_ == nullptr);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(cb->next_ == nullptr);
        return;
    }

    std::unique_lock<std::mutex> lock{callback_mutex_};
    if ((cb->executed_by_ != score::cpp::this_thread::get_id()) && (cb->executed_by_ != score::cpp::thread::id{}))
    {
        // callback executed by other thread, wait
        wakeup_.wait(lock, [this, cb]() { return cb->callback_id_ != currently_executing_callback_; });
    }

    // remove stop_callback 'cb' from our double-linked list now
    if (cb->prev_ == nullptr)
    {
        first_callback_ = cb->next_;
    }
    else
    {
        // ASSERT is done below since we would otherwise face an unchecked unlink from our double-linked list which is a
        // security issue (a.k.a unlink vulnerability, cf. https://github.com/chocolate-doom/chocolate-doom/issues/1453)
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(cb->prev_->next_ == cb);
        cb->prev_->next_ = cb->next_;
    }

    if (cb->next_ != nullptr)
    {
        // ASSERT is done below since we would otherwise face an unchecked unlink from our double-linked list which is a
        // security issue (a.k.a unlink vulnerability, cf. https://github.com/chocolate-doom/chocolate-doom/issues/1453)
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(cb->next_->prev_ == cb);
        cb->next_->prev_ = cb->prev_;
    }

    // clear cb's list pointers
    cb->next_ = nullptr;
    cb->prev_ = nullptr;
}

bool stop_state::request_stop()
{
    bool was_already_executed = false;
    if (stop_requested_.compare_exchange_strong(was_already_executed, true))
    {
        notify_registered_callbacks();
        return true;
    }
    return false;
}

void stop_state::notify_registered_callbacks()
{
    std::unique_lock<std::mutex> guard{callback_mutex_};
    auto* cb = first_callback_;
    while (cb != nullptr)
    {
        // Allow manipulation of callback list, during execution
        // of single callback (enable the manipulation within the callback)
        if (!cb->already_started_executing_)
        {
            execute(guard, cb);
            // We don't know if during the execution of the callback,
            // somebody altered the list. So we have to start at beginning
            cb = first_callback_;
        }
        else
        {
            cb = cb->next_;
        }
    }
}

} // namespace detail

const nostopstate_t nostopstate{};

} // namespace score::cpp
