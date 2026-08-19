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
#include "score/os/fcntl_impl.h"
#include <sys/file.h>

namespace score::os
{

score::cpp::expected_blank<Error> FcntlImpl::fcntl(const std::int32_t fd,
                                                   const Command command,
                                                   const Open flags) const noexcept
{
    // Because the signature is very specific, we must also restrict the commands we support.
    // Based on the signature this is solely Command::kFileSetStatusFlags
    if (command != Command::kFileSetStatusFlags)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(EINVAL));
    }

    // Manual code analysis:
    // This is only defensive programming. The above check for the specific command ensures that this case never
    // happens.
    const auto native_command_exp = internal::fcntl_helper::CommandToInteger(command);
    if (!native_command_exp.has_value())  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(native_command_exp.error());  // LCOV_EXCL_LINE
    }

    const std::int32_t native_flags{internal::fcntl_helper::OpenFlagToInteger(flags)};
    // Suppressed here because POSIX method accepts c-style vararg.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) see comment above
    const std::int32_t ret{::fcntl(fd, native_command_exp.value(), native_flags)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<Fcntl::Open, Error> FcntlImpl::fcntl(const std::int32_t fd,
                                                          const Fcntl::Command command) const noexcept
{
    // Because the signature is very specific, we must also restrict the commands we support.
    // This signature supports more commands, but we restrict to the required commands for the current use cases.
    if (command != Command::kFileGetStatusFlags)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(EINVAL));
    }

    // Manual code analysis:
    // This is only defensive programming. The above check for the specific command ensures that this case never
    // happens.
    const auto native_command_exp = internal::fcntl_helper::CommandToInteger(command);
    if (!native_command_exp.has_value())  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(native_command_exp.error());  // LCOV_EXCL_LINE
    }

    // Suppressed here because POSIX method accepts c-style vararg.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) see comment above
    const std::int32_t ret{::fcntl(fd, native_command_exp.value())};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return internal::fcntl_helper::IntegerToOpenFlag(ret);
}

score::cpp::expected<std::int32_t, Error> FcntlImpl::open(const char* const pathname, const Open flags) const noexcept
{
    const std::int32_t native_flags{internal::fcntl_helper::OpenFlagToInteger(flags)};
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function, cppcoreguidelines-pro-type-vararg): POSIX method accepts c-style vararg
    const std::int32_t ret{::open(pathname, native_flags)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> FcntlImpl::open(const char* const pathname,
                                                          const Open flags,
                                                          const Stat::Mode mode) const noexcept
{
    const std::int32_t native_flags{internal::fcntl_helper::OpenFlagToInteger(flags)};
    const std::uint32_t native_mode{ModeToInteger(mode)};
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function, cppcoreguidelines-pro-type-vararg): POSIX method accepts c-style vararg
    const std::int32_t ret{::open(pathname, native_flags, native_mode)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected_blank<Error> FcntlImpl::posix_fallocate(const std::int32_t fd,
                                                             const off_t offset,
                                                             const off_t len) const noexcept
{
    const std::int32_t ret{::posix_fallocate(fd, offset, len)};
    if (ret != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(ret));
    }
    return {};
}

score::cpp::expected_blank<Error> FcntlImpl::flock(const std::int32_t filedes, const Operation op) const noexcept
{
    const std::int32_t ret{::flock(filedes, internal::fcntl_helper::OperationFlagToInteger(op))};
    if (ret != 0)
    {
        // Suppress "AUTOSAR C++14 M19-3-1", The rule states: "The error indicator errno shall not be used."
        // Using library-defined macro to ensure correct operation.
        // coverity[autosar_cpp14_m19_3_1_violation]
        return score::cpp::make_unexpected(Error::createFromErrnoFlockSpecific(errno));
    }
    return {};
}

}  // namespace score::os
