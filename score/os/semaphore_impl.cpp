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
#include "score/os/semaphore_impl.h"

#include <fcntl.h>
#include <sys/stat.h>

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{

score::cpp::expected_blank<Error> SemaphoreImpl::sem_init(sem_t* const sem,
                                                   const std::int32_t pshared,
                                                   const std::uint32_t value) const noexcept
{
    // Manual code analysis:
    // As per QNX documentation function returns error when the given semaphore was previously initialized,
    // and has not been destroyed, but implementation returns 0. When the value argument exceeds SEM_VALUE_MAX
    // memcheck fails with integer overflow. And there is no way to reliably simulate the following error cases
    // in the scope of a unit test. A resource required to initialize the semaphore has been exhausted, The
    // process lacks the appropriate privileges to initialize the semaphore and the sem_init() function isn't
    // supported. In case of an error ::sem_init return a value of -1 and set errno to indicate the error.
    if (::sem_init(sem, pshared, value) != 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return {};
}
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected<sem_t*, Error> SemaphoreImpl::sem_open(const char* const pathname,
                                                     const Semaphore::OpenFlag oflag,
                                                     const Semaphore::ModeFlag mode,
                                                     const std::uint32_t value) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override  */
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg): POSIX method accepts c-style vararg.
    sem_t* const ret = ::sem_open(pathname, openflag_to_nativeflag(oflag), modeflag_to_nativeflag(mode), value);
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:  OS library macro */
    // Suppress "AUTOSAR C++14 M5-2-8" rule finding: "An object with integer type or pointer to void type shall not be
    // converted to an object with pointer type."
    // Suppress "AUTOSAR C++14 M5-2-9" rule finding: "A cast shall not
    // convert a pointer type to an integral type."
    // Rationale: Cast is happening outside our code domain
    // coverity[autosar_cpp14_m5_2_8_violation]
    // coverity[autosar_cpp14_m5_2_9_violation]
    // coverity[autosar_cpp14_a5_2_2_violation] SEM_FAILED is system macro
    if (ret == SEM_FAILED)
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:  OS library macro  */
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected<sem_t*, Error> SemaphoreImpl::sem_open(const char* const pathname,
                                                     const Semaphore::OpenFlag oflag) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override  */
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg): POSIX method accepts c-style vararg.
    sem_t* const ret = ::sem_open(pathname, openflag_to_nativeflag(oflag));
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:  OS library macro */
    // Suppress "AUTOSAR C++14 M5-2-8" rule finding: "An object with integer type or pointer to void type shall not be
    // converted to an object with pointer type."
    // Suppress "AUTOSAR C++14 M5-2-9" rule finding: "A cast shall not
    // convert a pointer type to an integral type."
    // Rationale: Cast is happening outside our code domain
    // coverity[autosar_cpp14_m5_2_8_violation]
    // coverity[autosar_cpp14_m5_2_9_violation]
    // coverity[autosar_cpp14_a5_2_2_violation] SEM_FAILED is system macro
    if (ret == SEM_FAILED)
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:  OS library macro  */
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<Error> SemaphoreImpl::sem_wait(sem_t* const sem) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override  */
{
    // Manual code analysis:
    // ::sem_wait() only fails if the sem_t pointer is invalid or a signal interrupted the call. Neither
    // is easily simulated in a unit test. On failure, ::pclose will return -1 and set errno (POSIX 1003.1).
    if (::sem_wait(sem) != 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return {};
}
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<Error> SemaphoreImpl::sem_post(sem_t* const sem) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override  */
{
    if (::sem_post(sem) != 0) /* LCOV_EXCL_BR_LINE */
    /* Not possible to cover through unit test. Not possible to make ::sem_post return non zero value. */
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
        /* Not possible to cover through unit test. Not possible to make ::sem_post return non zero value.
           Negative Test: CI fails on it, thread sanitizer reports sem_t pointer could not be nullptr. */
    }
    return {};
}
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<Error> SemaphoreImpl::sem_close(sem_t* const sem) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override  */
{
    if (::sem_close(sem) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<Error> SemaphoreImpl::sem_unlink(const char* const pathname) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override  */
{
    if (::sem_unlink(pathname) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<Error> SemaphoreImpl::sem_timedwait(sem_t* const sem,
                                                        const struct timespec* const abs_time) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override  */
{

    if (::sem_timedwait(sem, abs_time) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}
// SemaphoreImpl::sem_timedwait_monotonic() is platform-specific and lives in
// semaphore_impl_linux.cpp / semaphore_impl_qnx.cpp (selected in BUILD).
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<Error> SemaphoreImpl::sem_getvalue(sem_t* const sem, std::int32_t* const sval) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override  */
{
    // Manual code analysis:
    // Failure only happens when the sem argument does not refer to a valid semaphore. In case of an error
    // ::sem_getvalue return a value of -1 and set errno to indicate the error (POSIX 1003.1).
    // Negative Test: CI fails on it, thread sanitizer reports sem_t pointer could not be nullptr.
    // There is no other way to reliably create an invalid sem in the scope of a unit test.
    if (::sem_getvalue(sem, sval) != 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return {};
}

std::int32_t SemaphoreImpl::openflag_to_nativeflag(const Semaphore::OpenFlag flags) const noexcept
{
    std::int32_t native_flags{};
    if (static_cast<std::int32_t>(flags & Semaphore::OpenFlag::kCreate) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= O_CREAT;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    if (static_cast<std::int32_t>(flags & Semaphore::OpenFlag::kExclusive) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= O_EXCL;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    return native_flags;
}

// SCORE_CCM_NO_LINT It is not possible to split the functionality in a meaniful way.
std::int32_t SemaphoreImpl::modeflag_to_nativeflag(const Semaphore::ModeFlag flags) const noexcept
{
    std::int32_t native_flags{};
    if (static_cast<std::int32_t>(flags & Semaphore::ModeFlag::kReadUser) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= S_IRUSR;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    if (static_cast<std::int32_t>(flags & Semaphore::ModeFlag::kWriteUser) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= S_IWUSR;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    if (static_cast<std::int32_t>(flags & Semaphore::ModeFlag::kExecUser) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= S_IXUSR;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    if (static_cast<std::int32_t>(flags & Semaphore::ModeFlag::kReadGroup) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= S_IRGRP;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    if (static_cast<std::int32_t>(flags & Semaphore::ModeFlag::kWriteGroup) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= S_IWGRP;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    if (static_cast<std::int32_t>(flags & Semaphore::ModeFlag::kExecGroup) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= S_IXGRP;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    if (static_cast<std::int32_t>(flags & Semaphore::ModeFlag::kReadOthers) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= S_IROTH;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    if (static_cast<std::int32_t>(flags & Semaphore::ModeFlag::kWriteOthers) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= S_IWOTH;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    if (static_cast<std::int32_t>(flags & Semaphore::ModeFlag::kExecOthers) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: OS library macro */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        native_flags |= S_IXOTH;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: OS library macro */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the signedness of the resultant */
    }
    return native_flags;
}

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

}  // namespace os
}  // namespace score
