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
#include "interprocess_mutex.h"

#include "score/utility.hpp"

#include <pthread.h>
#include <iostream>

// We need to ensure that this is the case, since otherwise we cannot store it in shared memory.
static_assert(std::is_standard_layout<score::os::InterprocessMutex>::value,
              "InterprocessMutex is not of Standard layout");

score::os::InterprocessMutex::InterprocessMutex() noexcept
{
    ::pthread_mutexattr_t mutexConfig;
    const auto error = ::pthread_mutexattr_init(&mutexConfig); /** \requirement swdda.pthread_mutexattr_init.attr **/
    if (error != 0) /* LCOV_EXCL_START: Unable to reliably create an error case in the scope of a unit test. */
    {
        /* Failure happens if insufficient memory exists to initialise the mutex attributes object or specified invalid
         * attribute */
        // Here we are printing error logs and it doesn't harm
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay) see comment above
        // Suppress “AUTOSAR_Cpp14_M5_2_12” rule finding: “An identifier with array type passed as a function argument
        // shall not decay to a pointer.
        // Rationale: Here the pointer decay is happening because the __func_ is treated as a const
        // char*. When an array decays to a pointer, the size of the array is lost. Hence the above rule is added. Here
        // there is no harm to functionality.
        // coverity[autosar_cpp14_m5_2_12_violation]
        std::cerr << __func__ << __LINE__ << "Could not initialize pthread_mutex_attr:" << error << "Terminating.";
        // NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay)
        std::terminate();
        /* LCOV_EXCL_STOP */
    }

    score::cpp::ignore = ::pthread_mutexattr_setpshared(
        &mutexConfig,
        PTHREAD_PROCESS_SHARED); /** \requirement swdda.pthread_mutexattr_setpshared.pshared.PTHREAD_PROCESS_SHARED
                                  **/
    score::cpp::ignore = ::pthread_mutex_init(&mutex, &mutexConfig); /** \requirement swdda.pthread_mutex_init.mutex **/
}

score::os::InterprocessMutex::~InterprocessMutex() noexcept
{
    score::cpp::ignore = ::pthread_mutex_destroy(&mutex);
}

void score::os::InterprocessMutex::lock() noexcept
{
    const auto error = ::pthread_mutex_lock(&mutex); /** \requirement swdda.pthread_mutex_lock.mutex */
    if (error != 0) /* LCOV_EXCL_START: Unable to reliably create an error case in the scope of a unit test. */
    {
        /* Calling again locked mutex with default attribute type blocks thread, unless mutex type is ERRORCHECK */
        // Here we are printing error logs and it doesn't harm
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay) see comment above
        // Suppress “AUTOSAR_Cpp14_M5_2_12” rule finding: “An identifier with array type passed as a function argument
        // shall not decay to a pointer.
        // Rationale: Here the pointer decay is happening because the __func_ is treated as a const
        // char*. When an array decays to a pointer, the size of the array is lost. Hence the above rule is added. Here
        // there is no harm to functionality.
        // coverity[autosar_cpp14_m5_2_12_violation]
        std::cerr << __func__ << __LINE__ << "Could not lock mutex:" << error << "Terminating.";
        // NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay)
        std::terminate();
        /* LCOV_EXCL_STOP */
    }
}

void score::os::InterprocessMutex::unlock() noexcept
{
    const auto error = pthread_mutex_unlock(&mutex); /** \requirement swdda.pthread_mutex_unlock.mutex */
    if (error != 0) /* LCOV_EXCL_START: Unable to reliably create an error case in the scope of a unit test. */
    {
        /* Mutex is not corresponds to the type when calling unlock again returns the error. */
        // Here we are printing error logs and it doesn't harm
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay) see comment above
        // Suppress “AUTOSAR_Cpp14_M5_2_12” rule finding: “An identifier with array type passed as a function argument
        // shall not decay to a pointer.
        // Rationale: Here the pointer decay is happening because the __func_ is treated as a const
        // char*. When an array decays to a pointer, the size of the array is lost. Hence the above rule is added. Here
        // there is no harm to functionality.
        // coverity[autosar_cpp14_m5_2_12_violation]
        std::cerr << __func__ << __LINE__ << "Could not unlock mutex:" << error << "Terminating.";
        // NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay)
        std::terminate();
        /* LCOV_EXCL_STOP */
    }
}

bool score::os::InterprocessMutex::try_lock() noexcept
{
    return ::pthread_mutex_trylock(&mutex) == 0; /** \requirement swdda.pthread_mutex_trylock.mutex */
}
