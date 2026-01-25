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
#include "score/os/errno.h"

#include <score/utility.hpp>

#include <glob.h>
#include <array>
#include <cerrno>
#include <cstring>
#include <sstream>

// Note 1
// Suppress "AUTOSAR C++14 M6-4-5" and "AUTOSAR C++14 M6-4-3", The rule states: An unconditional throw or break
// statement shall terminate every nonempty switch-clause." and "A switch statement shall be a well-formed
// switch statement.", respectively. This is false positive. The `return` statement in this case clause
// unconditionally exits the function, making an additional `break` statement redundant.

namespace
{

/// If we have OS specific error codes, we define them in this function via ifdefs.
/// this way we do not double define the POSIX values which are common for both OS.
///
/// All the error names specified by POSIX.1 must have distinct values, with the exception of EAGAIN and
/// EWOULDBLOCK, which may be the same. On Linux and QNX, these two have the same value.
/// And with the exception of ENOTSUP and EOPNOTSUPP, which may be the same. On Linux these two have the same
/// value.
///
/// @see https://www.man7.org/linux/man-pages/man3/errno.3.html#DESCRIPTION
/// @see https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/e/errno.html
///
// SCORE_CCM_NO_LINT It is not possible to split the functionality in a meaniful way.
score::os::Error::Code convertErrnoToCode(const std::int32_t error_number) noexcept
{
    // coverity[autosar_cpp14_m6_4_3_violation] see Note 1
    switch (error_number)
    {
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EPERM: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kOperationNotPermitted;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ENOENT: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kNoSuchFileOrDirectory;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EINTR: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kOperationWasInterruptedBySignal;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EIO: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kInputOutput;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ENXIO: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kNoSuchFileOrDirectory;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EBADF: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kBadFileDescriptor;
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EAGAIN:  // same as EWOULDBLOCK
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kResourceTemporarilyUnavailable;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ENOMEM: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kNotEnoughSpace;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EACCES: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kPermissionDenied;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EBUSY: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kDeviceOrResourceBusy;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ENOTDIR: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kNotADirectory;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EISDIR: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kIsADirectory;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EINVAL: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kInvalidArgument;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ENFILE: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kTooManyOpenFilesInSystem;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EMFILE: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kTooManyOpenFiles;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ENOSPC: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kNoSpaceLeftOnDevice;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EROFS: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kReadOnlyFileSystem;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ENAMETOOLONG: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kFilenameTooLong;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ELOOP: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kToManyLevelsOfSymbolicLinks;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EOVERFLOW: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kValueTooLargeForDataType;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ENOTSUP: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kOperationNotSupported;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EEXIST: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kObjectExists;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ESRCH: /* KW_SUPPRESS:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
            return score::os::Error::Code::kNoSuchProcess;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ENOSYS:
            return score::os::Error::Code::kFileSystemDoesNotSupportTheOperation;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ENOLCK:
            return score::os::Error::Code::kKernelOutOfMemoryForAllocatingLocks;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ETIMEDOUT:
            return score::os::Error::Code::kKernelTimeout;
        default:
            break;
    }
    return score::os::Error::Code::kUnexpected;
}

score::os::Error::Code convertGlobErrorToCode(const std::int32_t glob_error) noexcept
{
    // coverity[autosar_cpp14_m6_4_3_violation] see Note 1
    switch (glob_error)
    {
        /* KW_SUPPRESS:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case GLOB_NOSPACE:
            return score::os::Error::Code::kGlobNoSpace;
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux/ __x86_64__ and QNX to exist
// in the same file. It also prevents compiler errors in linux/__x86_64__ code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef __linux__
        // LCOV_EXCL_START: Linux specific code, scope of codecoverage is only for qnx code
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case GLOB_ABORTED:
            return score::os::Error::Code::kGlobAborted;
            // LCOV_EXCL_STOP
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case GLOB_ABEND:
            return score::os::Error::Code::kGlobAborted;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case GLOB_NOMATCH:
            return score::os::Error::Code::kGlobNoMatch;
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        default:
            break;
    }
    return score::os::Error::Code::kUnexpected;
}

score::os::Error::Code convertFlockErrorToCode(const std::int32_t error) noexcept
{
    // coverity[autosar_cpp14_m6_4_3_violation] see Note 1
    switch (error)
    {
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EWOULDBLOCK:
            return score::os::Error::Code::kAlreadyLocked;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case EOPNOTSUPP:
            return score::os::Error::Code::kFdRefersToAnObject;
        default:
            break;
    }
    return convertErrnoToCode(error);
}

}  // namespace

score::os::Error score::os::Error::createFromErrno(const std::int32_t error_number) noexcept
{
    return Error{convertErrnoToCode(error_number), error_number};
}

score::os::Error score::os::Error::createFromErrnoFlockSpecific(const std::int32_t error_number) noexcept
{
    return Error{convertFlockErrorToCode(error_number), error_number};
}

score::os::Error score::os::Error::createFromErrno() noexcept
{
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    // Suppress "AUTOSAR C++14 M19-3-1", The rule states: "The error indicator errno shall not be used."
    // Using library-defined macro to ensure correct operation.
    // coverity[autosar_cpp14_m19_3_1_violation]
    return Error{convertErrnoToCode(errno), errno};
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
}

score::os::Error score::os::Error::createUnspecifiedError() noexcept
{
    return Error{score::os::Error::Code::kUnexpected, -1};
}

score::os::Error score::os::Error::createFromGlobError(const std::int32_t glob_error) noexcept
{
    const auto error_code = convertGlobErrorToCode(glob_error);

    return Error{error_code, glob_error};
}

score::os::Error::Error(const Code os_independent_error, const std::int32_t os_dependent_error_code) noexcept
    : os_independent_error_{os_independent_error}, os_dependent_error_code_{os_dependent_error_code}
{
}

std::int32_t score::os::Error::GetOsDependentErrorCode() const noexcept
{
    return os_dependent_error_code_;
}

// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// Rationale: some of the std functions involving string, maps may throw bad_alloc, but it is rare scenario
// Exception thrown from noexcept lib will result in terminate.
// Such exception can not be handled by any other means, hence suppressed here to avoid try..catch
// coverity[autosar_cpp14_a15_5_3_violation]
std::string score::os::Error::ToString() const noexcept
{
    std::stringstream ss;  // LCOV_EXCL_BR_LINE: Hidden exception handling.
    ss << *this;           // LCOV_EXCL_BR_LINE: Hidden exception handling.
    return ss.str();
}

std::array<char, 32> score::os::Error::ToStringContainer(const score::os::Error& error) const noexcept
{
    std::array<char, 32> result{};

// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__linux__) && defined(__x86_64__)
    /*As per *man 3 strerror_r* since _GNU_SOURCE is defined: the GNU-specific strerror_r() returns a pointer to a
    string  containing the  error  message.  This may be either a pointer to a string that the function stores in buf,
    or a pointer to some (immutable) static  string (in which case buf is unused). */
    const auto msg = strerror_r(error.os_dependent_error_code_, result.data(), result.size());
    score::cpp::ignore = memcpy(result.data(), msg, result.size());
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
    std::ignore = strerror_r(error.os_dependent_error_code_, result.data(), result.size());
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif

    // adding null terminator to last element.
    const size_t size = result.size();
    result[size - 1U] = '\0';
    return result;
}

std::ostream& score::os::operator<<(std::ostream& ostream, const score::os::Error& error)
{
    ostream << "An OS error has occurred with error code: ";  // LCOV_EXCL_BR_LINE: Hidden exception handling.

    auto error_string = error.ToStringContainer(error);
    ostream << error_string.data();  // LCOV_EXCL_BR_LINE: Hidden exception handling.

    return ostream;
}

std::int32_t score::os::geterrno()
{
    // Suppress "AUTOSAR C++14 M19-3-1", The rule states: "The error indicator errno shall not be used."
    // Using library-defined macro to ensure correct operation.
    // coverity[autosar_cpp14_m19_3_1_violation]
    return errno;
}

void score::os::seterrno(const std::int32_t new_errno)
{
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    // Suppress "AUTOSAR C++14 M19-3-1", The rule states: "The error indicator errno shall not be used."
    // Using library-defined macro to ensure correct operation.
    // coverity[autosar_cpp14_m19_3_1_violation]
    errno = new_errno;
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
}

namespace score
{
namespace os
{

bool operator==(const score::os::Error& lhs, const score::os::Error& rhs) noexcept
{
    return lhs.GetOsDependentErrorCode() == rhs.GetOsDependentErrorCode();
}

bool operator!=(const score::os::Error& lhs, const Error& rhs) noexcept
{
    return !(lhs == rhs);
}

// Suppress "AUTOSAR C++14 A13-5-5", The rule states: "Comparison operators shall be non-member functions with
// identical parameter types and noexcept.
// Justification: These functions are needed to compare with Code, identical parameter comparators are also defined.
// These are defined friend as an exception to A11-3-1 as it uses internal member for comparison
// coverity[autosar_cpp14_a13_5_5_violation]
bool operator==(const score::os::Error& lhs, const score::os::Error::Code rhs) noexcept
{
    return lhs.os_independent_error_ == rhs;
}

// coverity[autosar_cpp14_a13_5_5_violation] See justification above
bool operator!=(const score::os::Error& lhs, const score::os::Error::Code rhs) noexcept
{
    return lhs.os_independent_error_ != rhs;
}

}  // namespace os
}  // namespace score
