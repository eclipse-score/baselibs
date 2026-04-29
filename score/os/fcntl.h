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
#ifndef SCORE_LIB_OS_FCNTL_H
#define SCORE_LIB_OS_FCNTL_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"
#include "score/os/stat.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <fcntl.h>  // This will be removed with the old API
#include <cstdint>
#include <memory>

namespace score
{
namespace os
{

class Fcntl : public ObjectSeam<Fcntl>
{
  public:
    /// \brief Creates a new instance of the production implementation.
    /// \details This is to enable the usage of OSAL without the Singleton instance(). Especially library code
    /// should avoid the Singleton instance() method as this would interfere the unit tests of user code that also uses
    /// the instance().
    static std::unique_ptr<Fcntl> Default() noexcept;

    static score::cpp::pmr::unique_ptr<Fcntl> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    static Fcntl& instance() noexcept;

    enum class Command : std::uint32_t
    {
        kInvalid = 0UL,
        kFileGetStatusFlags = 1UL,
        kFileSetStatusFlags = 2UL
    };

    enum class Open : std::uint32_t
    {
        kReadOnly = 1UL,
        kWriteOnly = 2UL,
        kReadWrite = 4UL,
        kCreate = 8UL,
        kCloseOnExec = 16UL,
        kNonBlocking = 32UL,
        kExclusive = 64UL,
        kTruncate = 128UL,
        kDirectory = 256UL,
        kAppend = 512UL,
        kSynchronized = 1052672UL
    };

    enum class Operation : std::uint32_t
    {
        kLockExclusive = 1UL,
        kLockShared = 2UL,
        kLockNB = 4UL,
        kUnLock = 8UL
    };

    virtual score::cpp::expected_blank<Error> fcntl(const std::int32_t fd,
                                             const Fcntl::Command command,
                                             const Fcntl::Open flags) const noexcept = 0;

    virtual score::cpp::expected<Fcntl::Open, Error> fcntl(const std::int32_t fd,
                                                    const Fcntl::Command command) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> open(const char* const pathname, const Open flags) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, Error> open(const char* const pathname,
                                                    const Open flags,
                                                    const Stat::Mode mode) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> posix_fallocate(const std::int32_t fd,
                                                       const off_t offset,
                                                       const off_t len) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> flock(const std::int32_t filedes, const Operation op) const noexcept = 0;

    virtual ~Fcntl() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Fcntl(const Fcntl&) = delete;
    Fcntl& operator=(const Fcntl&) = delete;
    Fcntl(Fcntl&& other) = delete;
    Fcntl& operator=(Fcntl&& other) = delete;

  protected:
    Fcntl() = default;
};

namespace internal
{

namespace fcntl_helper
{
score::cpp::expected<std::int32_t, Error> CommandToInteger(const Fcntl::Command command) noexcept;

Fcntl::Open IntegerToOpenFlag(const std::int32_t flags) noexcept;
std::int32_t OpenFlagToInteger(const Fcntl::Open flags) noexcept;
std::int32_t OperationFlagToInteger(const Fcntl::Operation op) noexcept;
}  // namespace fcntl_helper

}  // namespace internal

}  // namespace os
   //
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Fcntl::Open> : public std::true_type
{
};
template <>
struct enable_bitmask_operators<::score::os::Fcntl::Operation> : public std::true_type
{
};
}  // namespace score

#endif  // SCORE_LIB_OS_FCNTL_H
