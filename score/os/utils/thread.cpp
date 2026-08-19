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
#include "score/os/utils/thread.h"

#include "score/mw/log/logging.h"
#include "score/os/pthread.h"

#include <cstring>
#include <sstream>
#include <thread>

void score::os::set_thread_name(const pthread_t& thread, const std::string& name) noexcept
{
    const auto ret = score::os::Pthread::instance().setname_np(thread, name.c_str());
    if (!ret.has_value())
    {
        score::mw::log::LogWarn() << "pthread_setname_np: " << ret.error().ToString();
    }
}

void score::os::set_thread_name(std::thread& thread, const std::string& name) noexcept
{
    set_thread_name(thread.native_handle(), name);
}

void score::os::set_thread_name(score::cpp::jthread& thread, const std::string& name) noexcept
{
    set_thread_name(thread.native_handle(), name);
}

std::string score::os::get_thread_name(std::thread& thread)
{
    // Length is restricted by POSIX to 16 characters, including the terminating null byte ('\0').
    constexpr std::size_t length = 16U;

    /* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Required by POSIX API */
    /* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Required by POSIX API */
    // POSIX API requires C-style array
    // NOLINTNEXTLINE(modernize-avoid-c-arrays) see comment above
    char name[length];
    /* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE:Required by POSIX API */
    /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Required by POSIX API */

    // NOLINTBEGIN(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay) see rationale below
    // Suppress “AUTOSAR_Cpp14_M5_2_12” rule finding: “An identifier with array type passed as a function argument
    // shall not decay to a pointer.
    // Rationale: Array to pointer decay is intended here. POSIX API requires C-style array
    // coverity[autosar_cpp14_m5_2_12_violation]
    const auto ret = score::os::Pthread::instance().getname_np(thread.native_handle(), name, length);
    // NOLINTEND(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay)

    /* LCOV_EXCL_BR_START: It is not possible to call getname_np with wrong parameters using unit test.*/
    if (!ret.has_value())
    {
        /* LCOV_EXCL_START: It is not possible to call getname_np with wrong parameters using unit test.*/
        std::stringstream errStr;
        errStr << ret.error();
        return {};
        /* LCOV_EXCL_STOP */
    }
    // LCOV_EXCL_BR_STOP

    // POSIX API requires C-style array
    // NOLINTNEXTLINE(modernize-avoid-c-arrays, hicpp-no-array-decay) see comment above
    return std::string(static_cast<char*>(name)); /* KW_SUPPRESS:LOCRET.RET:False positive */
}
