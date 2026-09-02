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
#ifndef SCORE_LIB_OS_ERRNO_H
#define SCORE_LIB_OS_ERRNO_H

#include <array>
#include <cerrno>  // shall be removed once transition is completed
#include <cstdint>
#include <ostream>
#include <string>

#include <score/expected.hpp>

namespace score
{
namespace os
{

/// \brief This class is an operating system independent version of error codes.
///        It shall be used as part of any functions return code within the os abstraction which consists of an
///        `score::cpp::expected`.
class Error final
{
  public:
    /// \brief A set of error codes that can be checked by applications interacting with the operating system.
    ///
    /// \detail It shall be noted that not all operating system support all these error codes. This is fine, since
    ///         this represents the list of errors we want to perform an error reaction, if they appear.
    ///         On some errors we will not define an error reaction, in that case it might not be in this list
    ///         and the error will just be logged using our `ToString()` method.
    // coverity[autosar_cpp14_a7_2_4_violation] no harm to our code
    // kReadOnlyFileSystem and kNoSuchFileOrDirectory intentionally reuse the real errno values
    // (see below) during the ongoing OS-agnostic error code transition. Remove this suppression
    // once that transition is completed.
    // NOLINTNEXTLINE(readability-enum-initial-value)
    enum class Code : std::int32_t
    {
        kOperationNotPermitted,
        kInputOutput,
        kNoSuchDeviceOrAddress,
        kBadFileDescriptor,
        kResourceTemporarilyUnavailable,
        kNotEnoughSpace,
        kPermissionDenied,
        kDeviceOrResourceBusy,
        kNotADirectory,
        kIsADirectory,
        kInvalidArgument,
        kTooManyOpenFilesInSystem,
        kTooManyOpenFiles,
        kNoSpaceLeftOnDevice,
        kFilenameTooLong,
        kToManyLevelsOfSymbolicLinks,
        kValueTooLargeForDataType,
        kOperationWasInterruptedBySignal,
        kOperationNotSupported,
        kObjectExists,
        kGlobAborted,
        kGlobNoMatch,
        kGlobNoSpace,
        kUnexpected,                      // use when an error appears that is not represented as OS agnostic
        kReadOnlyFileSystem = EROFS,      // shall be removed once the transition is completed
        kNoSuchFileOrDirectory = ENOENT,  // shall be removed once the transition is completed
        kNoSuchProcess,
        kFdRefersToAnObject,
        kAlreadyLocked,
        kFileSystemDoesNotSupportTheOperation,
        kKernelOutOfMemoryForAllocatingLocks,
        kKernelTimeout,
    };

    /// \brief Creates a new instance from the error indicated by errno.
    static Error createFromErrno(const std::int32_t error_number) noexcept;

    /// \brief Creates a new instance from the error indicated by errno.
    static Error createFromErrno() noexcept;

    /// \brief Creates a new instance from the error for Unspecified errors to support non OS agnostic sys calls
    static Error createUnspecifiedError() noexcept;

    /// \brief Creates an Error instance from a glob error code.
    static Error createFromGlobError(const std::int32_t glob_error) noexcept;

    /// \brief Creates an flock specific Error instance .
    static Error createFromErrnoFlockSpecific(const std::int32_t error_number) noexcept;

    /// \brief Default construction is not supported. Use createFromErrno() instead.
    Error() = delete;

    /// \brief Helper function to easily identify what error this object is representing
    /// \param rhs the error code to compare this object with
    /// \return true if the error that is represented by this object equals expected one, false otherwise
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Method is identifiable through namespace usage */
    // Suppress "AUTOSAR C++14 A13-5-5", The rule states: "Comparison operators shall be non-member functions with
    // identical parameter types and noexcept.
    // Justification: These functions are needed to compare with Code, identical parameter comparators are also defined.
    // These are defined friend as an exception to A11-3-1 as it uses internal member for comparison
    // coverity[autosar_cpp14_a13_5_5_violation]
    friend bool operator==(const Error& lhs, const Code rhs) noexcept;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Method is identifiable through namespace usage */

    /// \brief Helper function to easily identify what error this object is representing
    /// \param rhs the error code to compare this object with
    /// \return true if the error that is represented by this object equals expected one, false otherwise
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Method is identifiable through namespace usage */
    // coverity[autosar_cpp14_a13_5_5_violation] See justification above
    friend bool operator!=(const Error& lhs, const Code rhs) noexcept;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Method is identifiable through namespace usage */

    /// \brief Turn this error into a string
    std::string ToString() const noexcept;

    /// \brief wrap error into an array of 32 characters.
    std::array<char, 32> ToStringContainer(const score::os::Error& error) const noexcept;

    /// \brief Getter method for OS-dependent error code.
    std::int32_t GetOsDependentErrorCode() const noexcept;

  private:
    /// \brief This class shall only be instantiated by createFromErrno().
    /// \param os_independent_error The OS-independent error code.
    /// \param os_dependent_error_code errno
    Error(const Code os_independent_error, const std::int32_t os_dependent_error_code) noexcept;

    Code os_independent_error_;
    std::int32_t os_dependent_error_code_;

    // Necessary to allow access to os_dependent_error_code_ variable.
    // Suppress "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Friend function is used for overloading operator << to access private members of both operands in the operator
    // function. This is intended for functionality.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend std::ostream& operator<<(std::ostream& ostream, const score::os::Error& error);
};

/// \brief Equality comparison operator
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Method is identifiable through namespace usage */
bool operator==(const Error& lhs, const Error& rhs) noexcept;
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Method is identifiable through namespace usage */

/// \brief Inqueality comparison operator
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Method is identifiable through namespace usage */
bool operator!=(const Error& lhs, const Error& rhs) noexcept;
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Method is identifiable through namespace usage */

static_assert(std::is_move_constructible_v<Error>);
static_assert(std::is_move_assignable_v<Error>);

template <typename R>
using Result = score::cpp::expected<R, Error>;

std::ostream& operator<<(std::ostream& ostream, const Error& error);

std::int32_t geterrno();
void seterrno(const std::int32_t new_errno);

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_ERRNO_H
