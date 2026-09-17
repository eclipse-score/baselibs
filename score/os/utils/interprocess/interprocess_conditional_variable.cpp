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
#include "score/os/utils/interprocess/interprocess_conditional_variable.h"

#include <pthread.h>
#include <cstdint>
#include <iostream>
#include <tuple>

score::os::InterprocessConditionalVariable::InterprocessConditionalVariable() noexcept
{
    std::int32_t error{0};
    pthread_condattr_t attr;
    error = ::pthread_condattr_init(&attr);
    /* There is no way to reliably create an error case in the scope of a unit test. */
    /* Failure happens if insufficient memory exists to initialize the condition variable attributes object.*/
    if (error != 0)  // LCOV_EXCL_BR_LINE
    {
        /* LCOV_EXCL_START */
        std::cerr << "Could not initialize conditional variable attributes with error" << error;
        // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
        std::abort();
        /* LCOV_EXCL_STOP */
    }
    error = ::pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    /* There is no way to reliably create an error case in the scope of a unit test. */
    /* Failure happens if condition variable attributes object is invalid. */
    if (error != 0)  // LCOV_EXCL_BR_LINE
    {
        /* LCOV_EXCL_START */
        std::cerr << "Could not set PTHREAD_PROCESS_SHARED on conditional variable with error" << error;
        // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
        std::abort();
        /* LCOV_EXCL_STOP */
    }

    error = ::pthread_cond_init(&this->conditionalVariable, &attr);

    /* There is no way to reliably create an error case in the scope of a unit test. */
    /* Failure happens if insufficient memory exists or necessary resources to init conditional variable */
    if (error != 0)  // LCOV_EXCL_BR_LINE
    {
        /* LCOV_EXCL_START */
        std::cerr << "Could not initialize conditional variable with error" << error;
        // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
        std::abort();
        /* LCOV_EXCL_STOP */
    }

    error = ::pthread_condattr_destroy(&attr);

    /* Do not overreact if it fails. Just log it and proceed. Attr is a local variable and will be destroyed anyway */
    if (error != 0)  // LCOV_EXCL_BR_LINE
    {
        /* LCOV_EXCL_START */
        std::cerr << "pthread_condattr_destroy failed to destroy attribute object: error: " << error;
        /* LCOV_EXCL_STOP */
    }
}

score::os::InterprocessConditionalVariable::~InterprocessConditionalVariable() noexcept
{
    std::ignore = ::pthread_cond_destroy(&this->conditionalVariable);
}

auto score::os::InterprocessConditionalVariable::notify_one() noexcept -> void
{
    const auto error = ::pthread_cond_signal(&this->conditionalVariable);
    /* There is no way to reliably create an error case in the scope of a unit test. */
    /* Failure happens if the value cond does not refer to an initialised condition variable. */
    if (error != 0)  // LCOV_EXCL_BR_LINE
    {
        /* LCOV_EXCL_START */
        std::cerr << "Error while notifying waiting thread" << error;
        // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
        std::abort();
        /* LCOV_EXCL_STOP */
    }
}

auto score::os::InterprocessConditionalVariable::notify_all() noexcept -> void
{
    const auto error = ::pthread_cond_broadcast(&this->conditionalVariable);
    /* There is no way to reliably create an error case in the scope of a unit test. */
    /* Failure happens if the value cond does not refer to an initialised condition variable.*/
    if (error != 0)  // LCOV_EXCL_BR_LINE
    {
        /* LCOV_EXCL_START */
        std::cerr << "Error while notifying all waiting threads" << error;
        // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
        std::abort();
        /* LCOV_EXCL_STOP */
    }
}

auto score::os::InterprocessConditionalVariable::wait(std::unique_lock<InterprocessMutex>& lock) noexcept -> void
{
    /*  Not possible to test as systen aborts if mutex lock fails. */
    if (!lock.owns_lock())  // LCOV_EXCL_BR_LINE
    {
        std::cerr << "Violated precondition. mutex needs to be locked before passing to conditional variable!";
        // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
        std::abort();
    }

    std::ignore = ::pthread_cond_wait(&this->conditionalVariable, &(lock.mutex()->mutex));
}
