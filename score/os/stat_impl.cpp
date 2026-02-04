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
#include "score/os/stat_impl.h"

#include <fcntl.h>
#include <type_traits>

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
namespace score
{
namespace os
{

// This is a wrapper function and it simulates the behaviour of OS function
// NOLINTNEXTLINE(google-default-arguments) see comment above
score::cpp::expected_blank<Error> StatImpl::stat(const char* const file,
                                          struct StatBuffer& buf,
                                          const bool resolve_symlinks) const noexcept
{
    struct stat native_buffer{};
    int result = 0;

    if (resolve_symlinks)
    {
        result = ::stat(file, &native_buffer);
    }
    else
    {
        result = ::lstat(file, &native_buffer);
    }

    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }

    stat_to_statbuffer(native_buffer, buf);
    return {};
}

score::cpp::expected_blank<Error> StatImpl::fstat(const std::int32_t fd, struct StatBuffer& buf) const noexcept
{
    struct stat native_buffer{};
    if (::fstat(fd, &native_buffer) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    stat_to_statbuffer(native_buffer, buf);
    return {};
}

score::cpp::expected_blank<Error> StatImpl::mkdir(const char* const path, const Stat::Mode mode) const noexcept
{
    const std::uint32_t native_mode{ModeToInteger(mode)};
    if (::mkdir(path, native_mode) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<Error> StatImpl::chmod(const char* const path, const Stat::Mode mode) const noexcept
{
    const mode_t native_mode{ModeToInteger(mode)};

    if (::chmod(path, native_mode) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<Error> StatImpl::fchmod(const std::int32_t fd, const Stat::Mode mode) const noexcept
{
    const mode_t native_mode{ModeToInteger(mode)};
    if (::fchmod(fd, native_mode) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<Stat::Mode, Error> StatImpl::umask(const Stat::Mode umask) const noexcept
{
    const mode_t native_umask{ModeToInteger(umask)};
    return IntegerToMode(::umask(native_umask));
}

void StatImpl::stat_to_statbuffer(const struct stat& native_stat, StatBuffer& stat_buffer) const noexcept
{
    stat_buffer.st_mode = native_stat.st_mode;
    stat_buffer.st_ino = native_stat.st_ino;
    stat_buffer.st_dev = native_stat.st_dev;
    stat_buffer.st_nlink = native_stat.st_nlink;
    stat_buffer.st_uid = static_cast<std::int64_t>(native_stat.st_uid);
    stat_buffer.st_gid = static_cast<std::int64_t>(native_stat.st_gid);
    stat_buffer.st_rdev = native_stat.st_rdev;
    stat_buffer.st_size = native_stat.st_size;
    static_assert(std::is_same<time_t, std::int64_t>::value, "Types don't match");
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
    stat_buffer.atime = native_stat.st_atime;  // NOLINT(cppcoreguidelines-pro-type-union-access) see comment above
    stat_buffer.mtime = native_stat.st_mtime;  // NOLINT(cppcoreguidelines-pro-type-union-access) see comment above
    stat_buffer.ctime = native_stat.st_ctime;  // NOLINT(cppcoreguidelines-pro-type-union-access) see comment above
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
// blkcnt_t is int64 in Linux and uint64 in QNX
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__QNX__)
    stat_buffer.st_blocks = native_stat.st_blocks;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
    stat_buffer.st_blocks = static_cast<uint64_t>(native_stat.st_blocks);
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    stat_buffer.st_blksize = static_cast<std::int64_t>(native_stat.st_blksize);
}

score::cpp::expected_blank<Error> StatImpl::fchmodat(const std::int32_t fd,
                                              const char* const path,
                                              const Mode mode,
                                              const bool resolve_symlinks) const noexcept
{
    const mode_t native_mode{ModeToInteger(mode)};
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
    const std::int32_t flags = resolve_symlinks ? 0 : AT_SYMLINK_NOFOLLOW;
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
    if (::fchmodat(fd, path, native_mode, flags) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
