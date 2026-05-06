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
#ifndef SCORE_LIB_OS_STAT_H
#define SCORE_LIB_OS_STAT_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <sys/stat.h>  // This will be removed with the old API
#include <cstdint>
#include <memory>

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
// coverity[autosar_cpp14_m7_3_1_violation] see comment below
struct stat; /* KW_SUPPRESS:MISRA.NS.GLOBAL:Struct declaration needed to avoid incomplete type error in stat() */

namespace score
{
namespace os
{

/// @brief Buffer struct used with stat()
struct StatBuffer
{
    std::uint32_t st_mode;
    std::uint64_t st_ino;
    std::uint64_t st_dev;
    std::uint64_t st_nlink;
    std::int64_t st_uid;
    std::int64_t st_gid;
    std::uint64_t st_rdev;
    std::int64_t st_size;
    std::int64_t atime;
    std::int64_t mtime;
    std::int64_t ctime; /* KW_SUPPRESS:MISRA.STDLIB.WRONGNAME:ctime is defined in a separate namespace than stdlib */
    std::uint64_t st_blocks;
    std::int64_t st_blksize;
};

class Stat : public ObjectSeam<Stat>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static Stat& instance() noexcept;

    static score::cpp::pmr::unique_ptr<Stat> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    /// \brief Creates a new instance of the production implementation.
    /// \details This is to enable the usage of OSAL without the Singleton instance(). Especially library code
    /// should avoid the Singleton instance() method as this would interfere the unit tests of user code that also uses
    /// the instance().
    static std::unique_ptr<Stat> Default() noexcept;

    enum class Mode : std::uint32_t
    {
        kNone = 0b0000'0000'0000,
        kReadUser = 0b0000'0000'0001,
        kWriteUser = 0b0000'0000'0010,
        kExecUser = 0b0000'0000'0100,
        kReadWriteExecUser = 0b0000'0000'0111,
        kReadGroup = 0b0000'0000'1000,
        kWriteGroup = 0b0000'0001'0000,
        kExecGroup = 0b0000'0010'0000,
        kReadWriteExecGroup = 0b0000'0011'1000,
        kReadOthers = 0b0000'0100'0000,
        kWriteOthers = 0b0000'1000'0000,
        kExecOthers = 0b0001'0000'0000,
        kReadWriteExecOthers = 0b0001'1100'0000,
        kSticky = 0b0010'0000'0000,
        kSetGroupId = 0b0100'0000'0000,
        kSetUserId = 0b1000'0000'0000,
        kUnknown = 0xFFFF,
    };

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    // Wrapper must be backward compatible for the applications that use linux specific stat() (without resolve_symlinks
    // argument)
    // NOLINTNEXTLINE(google-default-arguments) see comment above
    virtual score::cpp::expected_blank<Error> stat(const char* const file,
                                            StatBuffer& buf,
                                            const bool resolve_symlinks = true) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> fstat(const std::int32_t fd, StatBuffer& buf) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> mkdir(const char* const path, const Mode mode) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> chmod(const char* const path, const Mode mode) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> fchmod(const std::int32_t fd, const Mode mode) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<Mode, Error> umask(const Mode umask) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    // Wrapper must be backward compatible for the applications that use linux specific fchmodat() (without
    // resolve_symlinks arg)
    // NOLINTNEXTLINE(google-default-arguments) see comment above
    virtual score::cpp::expected_blank<Error> fchmodat(const std::int32_t fd,
                                                const char* const path,
                                                const Mode mode,
                                                const bool resolve_symlinks = true) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual ~Stat() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Stat(const Stat&) = delete;
    Stat& operator=(const Stat&) = delete;
    Stat(Stat&& other) = delete;
    Stat& operator=(Stat&& other) = delete;

  protected:
    Stat() = default;
};

Stat::Mode IntegerToMode(const mode_t mode) noexcept;
mode_t ModeToInteger(const Stat::Mode mode) noexcept;

/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Stat::Mode> : public std::true_type
{
};
}  // namespace score

#endif  // SCORE_LIB_OS_STAT_H
