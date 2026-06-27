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
#include "score/concurrency/thread_pool.h"
#include "score/os/pthread.h"
#if defined(__QNX__)
#include "score/os/qnx_pthread.h"
#elif defined(__linux__)
#include "score/os/linux_pthread.h"
#endif  // __QNX__

#include "score/utility.hpp"

score::concurrency::ThreadPool::ThreadPool(const std::size_t number_of_threads, const std::string& name)
    : ThreadPool(number_of_threads, score::cpp::pmr::get_default_resource(), name)
{
}

score::concurrency::ThreadPool::ThreadPool(const std::size_t number_of_threads,
                                           score::cpp::pmr::memory_resource* memory_resource,
                                           const std::string& name)
    : Executor(memory_resource)
{
    InitializeThreads(number_of_threads, name);
}

void score::concurrency::ThreadPool::InitializeThreads(const std::size_t number_of_threads, const std::string& name)
{
    pool_.reserve(number_of_threads);
    active_.resize(number_of_threads);

    std::unique_ptr<score::os::Pthread> pthread;
    // coverity[autosar_cpp14_a16_0_1_violation] must have implementation selection
#if defined(__QNX__)
    pthread = std::make_unique<score::os::QnxPthread>();
    // coverity[autosar_cpp14_a16_0_1_violation] must have implementation selection
#elif defined(__linux__)
    pthread = std::make_unique<score::os::LinuxPthread>();
    // coverity[autosar_cpp14_a16_0_1_violation] must have implementation selection
#endif  // __QNX__

    for (std::size_t thread_number = 0u; thread_number < number_of_threads; thread_number++)
    {
        score::cpp::jthread worker_thread{[this, thread_number](score::cpp::stop_token stop_token) {
            Work(thread_number, std::move(stop_token));
        }};

        // TODO: In ASIL-B context there are concerns that we may not directly use this API. We shall only use one
        //       abstraction level to work with threads. (Ticket-99121)
        score::cpp::ignore =
            pthread->setname_np(worker_thread.native_handle(), (name + "_" + std::to_string(thread_number)).c_str());
        pool_.push_back(std::move(worker_thread));
    }
}

void score::concurrency::ThreadPool::Work(const std::size_t thread_number, const score::cpp::stop_token stop_token)
{
    std::unique_lock<std::mutex> lock{queue_mutex_};
    // GCOV_EXCL_START : Decision coudn't be analyzed. Covered by ThreadPool
    while ((!stop_token.stop_requested()) || (!queue_.empty()))
    // GCOV_EXCL_STOP
    {
        score::cpp::ignore = condition_.wait(lock, stop_token, [this]() noexcept -> bool {
            return !queue_.empty();
        });

        score::cpp::pmr::unique_ptr<Task> task{};
        if (!queue_.empty())
        {
            task = std::move(queue_.front());
            queue_.pop_front();
        }

        if (task != nullptr)
        {
            // Be aware of the ABA-Problem :)
            active_.at(thread_number) = task->GetStopSource();
            lock.unlock();
            Execute(std::move(task));
            lock.lock();
            active_.at(thread_number) = score::cpp::stop_source{};
        }
    }
}

std::size_t score::concurrency::ThreadPool::MaxConcurrencyLevel() const noexcept
{
    return pool_.size();
}

void score::concurrency::ThreadPool::Shutdown() noexcept
{
    InternalShutdown();
}

void score::concurrency::ThreadPool::InternalShutdown() noexcept
{
    // we set this flag as first step so that no new tasks will get added to our queue
    shutdown_reqested_.store(true);

    std::lock_guard<std::mutex> lock{queue_mutex_};
    for (auto& task_stop_source : active_)
    {
        score::cpp::ignore = task_stop_source.request_stop();
    }
    for (auto& worker_thread : pool_)
    {
        score::cpp::ignore = worker_thread.request_stop();
    }
}

bool score::concurrency::ThreadPool::ShutdownRequested() const noexcept
{
    return shutdown_reqested_.load();
}

void score::concurrency::ThreadPool::Enqueue(score::cpp::pmr::unique_ptr<Task> task)
{
    // NOTE: Even though ShutdownRequested() is an atomic operation, the lock must
    //       be acquired already here since otherwise, tasks might get added to the
    //       queue while InternalShutdown() is currently running. Then, in an unlucky
    //       situation, it could happen that all workers already finished and the newly
    //       added task gets never executed. Hence, we acquire the lock already here.
    std::unique_lock<std::mutex> lock{queue_mutex_};
    if (ShutdownRequested())
    {
        lock.unlock();
        Execute(std::move(task));
    }
    else
    {
        score::cpp::ignore = queue_.emplace_back(std::move(task));
        condition_.notify_one();
    }
}

void score::concurrency::ThreadPool::Execute(score::cpp::pmr::unique_ptr<Task> task)
{
    if (ShutdownRequested())
    {
        score::cpp::ignore = task->GetStopSource().request_stop();
    }
    std::invoke(*task, task->GetStopSource().get_token());
}

score::concurrency::ThreadPool::~ThreadPool() noexcept
{
    InternalShutdown();
}
