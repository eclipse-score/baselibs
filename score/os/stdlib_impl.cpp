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
#include "score/os/stdlib_impl.h"

#include <cstdlib>

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

namespace score
{
namespace os
{

score::cpp::expected_blank<Error> StdlibImpl::system_call(const std::string& cmd) const noexcept
{
    /* KW_SUPPRESS_START:MISRA.STDLIB.ABORT,MISRA.STDLIB.ABORT.2012_AMD1: This is wrapper function for ::system() */
    // Suppressed here because usage of this OSAL method is on banned list
    // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
    const auto key_ret = ::system(cmd.c_str());  // NOLINT(score-banned-function) see comment above
    /* KW_SUPPRESS_END:MISRA.STDLIB.ABORT,MISRA.STDLIB.ABORT.2012_AMD1: This is wrapper function for ::system() */
    /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: Required for capturing error number from ::system call */
    /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macros */
    // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
    if (WIFEXITED(key_ret) && (0 != WEXITSTATUS(key_ret))) /* LCOV_EXCL_BR_LINE
        It cannot be signalled to child process that is spawned to execute the cmd since we don't know the pid
        of the process spawned, and cannot be detected if the shell is started.
        So it is not possible to make WIFEXITED return false through unit test
        The command status WEXITSTATUS(key_ret) is valid only when the spawned process exits normally, so
        the check WIFEXITED(key_ret) && WEXITSTATUS(key_ret) is valid as intended */

    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macros */
    /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: Required for capturing error number from ::system call */
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}

// LCOV_EXCL_START Coverage not correctly shown because coverage is not flushed before exit.
/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: Function is wrapped */
// Suppressed here because usage of this OSAL method is on banned list
// NOLINTNEXTLINE(score-banned-function) see comment above
void StdlibImpl::exit(const int status) const noexcept
/* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME: Function is wrapped */
{
    /* KW_SUPPRESS_START:MISRA.STDLIB.ABORT,MISRA.STDLIB.ABORT.2012_AMD1: This is wrapper function for ::exit() */
    // Suppressed here because usage of this OSAL method is on banned list
    // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
    ::exit(status);  // NOLINT(score-banned-function) see comment above
    /* KW_SUPPRESS_END:MISRA.STDLIB.ABORT,MISRA.STDLIB.ABORT.2012_AMD1: This is wrapper function for ::exit() */
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START Coverage not correctly shown because coverage is not flushed before exit.
void StdlibImpl::quick_exit(const int status) const noexcept
{
    //  The std:: version of quick_exit was used for the feature to be OS agnostic and produce better portabilty of the
    //  code.
    std::quick_exit(status);
}
// LCOV_EXCL_STOP

/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: Function is wrapped */
// Suppressed here because usage of this OSAL method is on banned list
// NOLINTNEXTLINE(score-banned-function) see comment above
char* StdlibImpl::getenv(const char* const name) const noexcept
/* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME: Function is wrapped */
{
    /* KW_SUPPRESS_START:MISRA.STDLIB.ABORT,MISRA.STDLIB.ABORT.2012_AMD1: This is wrapper function for ::getenv() */
    // Suppressed here because usage of this OSAL method is on banned list
    // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
    return ::getenv(name);  // NOLINT(score-banned-function) see comment above
    /* KW_SUPPRESS_END:MISRA.STDLIB.ABORT,MISRA.STDLIB.ABORT.2012_AMD1: This is wrapper function for ::getenv() */
}

/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: Function is wrapped */
// Suppressed here because usage of this OSAL method is on banned list
// NOLINTNEXTLINE(score-banned-function) see comment above
int StdlibImpl::setenv(const char* const name, const char* const value, int overwrite) const noexcept
/* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME: Function is wrapped */
{
    /* KW_SUPPRESS_START:MISRA.STDLIB.ABORT,MISRA.STDLIB.ABORT.2012_AMD1: This is wrapper function for ::setenv() */
    // Suppressed here because usage of this OSAL method is on banned list
    // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
    return ::setenv(name, value, overwrite);  // NOLINT(score-banned-function) see comment above
    /* KW_SUPPRESS_END:MISRA.STDLIB.ABORT,MISRA.STDLIB.ABORT.2012_AMD1: This is wrapper function for ::setenv() */
}

/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: Function is wrapped */
// Suppressed here because usage of this OSAL method is on banned list
// NOLINTNEXTLINE(score-banned-function) see comment above
int StdlibImpl::unsetenv(const char* const name) const noexcept
/* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME: Function is wrapped */
{
    /* KW_SUPPRESS_START:MISRA.STDLIB.ABORT,MISRA.STDLIB.ABORT.2012_AMD1: This is wrapper function for ::unsetenv() */
    // Suppressed here because usage of this OSAL method is on banned list
    // coverity[autosar_cpp14_m18_0_3_violation] No harm to our code
    return ::unsetenv(name);  // NOLINT(score-banned-function) see comment above
    /* KW_SUPPRESS_END:MISRA.STDLIB.ABORT,MISRA.STDLIB.ABORT.2012_AMD1: This is wrapper function for ::unsetenv() */
}

Result<char*> StdlibImpl::realpath(const char* const path, char* const resolved_path) const noexcept
{
    char* const res = ::realpath(path, resolved_path);
    if (res == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return res;
}

Result<void*> StdlibImpl::calloc(size_t num_of_elements, size_t size) const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc): calloc needed for custom allocator
    const auto result = ::calloc(num_of_elements, size);
    if (result == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

void StdlibImpl::free(void* ptr) const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc): free needed for custom allocator
    ::free(ptr);
}

Result<int> StdlibImpl::mkstemp(char* const path) const noexcept
{
    const int res = ::mkstemp(path);
    if (res == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return res;
}

Result<int> StdlibImpl::mkstemps(char* const path, const int len) const noexcept
{
    const int res = ::mkstemps(path, len);
    if (res == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return res;
}

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
