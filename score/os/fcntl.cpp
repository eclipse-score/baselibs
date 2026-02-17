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

#include <type_traits>

// Note 1
// Suppress "AUTOSAR C++14 M6-4-5" and "AUTOSAR C++14 M6-4-3", The rule states: An unconditional throw or break
// statement shall terminate every nonempty switch-clause." and "A switch statement shall be a well-formed
// switch statement.", respectively. This is false positive. The `return` statement in this case clause
// unconditionally exits the function, making an additional `break` statement redundant.

namespace score::os
{

score::cpp::expected<std::int32_t, Error> internal::fcntl_helper::CommandToInteger(const Fcntl::Command command) noexcept
{
    // coverity[autosar_cpp14_m6_4_3_violation] see Note 1
    switch (command)
    {
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Fcntl::Command::kFileGetStatusFlags:
            return F_GETFL;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Fcntl::Command::kFileSetStatusFlags:
            return F_SETFL;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Fcntl::Command::kInvalid:
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        default:
            return score::cpp::make_unexpected(Error::createFromErrno(EINVAL));
    }
}

Fcntl::Open internal::fcntl_helper::IntegerToOpenFlag(const std::int32_t flags) noexcept
{
    //  We expect and will react only on positive value of 'flags'
    const auto bitwise_flags = static_cast<std::uint32_t>(flags);
    Fcntl::Open fcntl_flags{};

    // POSIX specifies that at least one of the access modes must be specified
    if ((bitwise_flags & static_cast<std::uint32_t>(O_ACCMODE)) == static_cast<std::uint32_t>(O_WRONLY))
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kWriteOnly;
    }
    else if ((bitwise_flags & static_cast<std::uint32_t>(O_ACCMODE)) == static_cast<std::uint32_t>(O_RDWR))
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kReadWrite;
    }
    else
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kReadOnly;
    }

    if ((bitwise_flags & static_cast<std::uint32_t>(O_CREAT)) == static_cast<std::uint32_t>(O_CREAT))
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kCreate;
    }
    if ((bitwise_flags & static_cast<std::uint32_t>(O_CLOEXEC)) == static_cast<std::uint32_t>(O_CLOEXEC))
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kCloseOnExec;
    }
    if ((bitwise_flags & static_cast<std::uint32_t>(O_NONBLOCK)) == static_cast<std::uint32_t>(O_NONBLOCK))
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kNonBlocking;
    }
    if ((bitwise_flags & static_cast<std::uint32_t>(O_EXCL)) == static_cast<std::uint32_t>(O_EXCL))
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kExclusive;
    }
    if ((bitwise_flags & static_cast<std::uint32_t>(O_TRUNC)) == static_cast<std::uint32_t>(O_TRUNC))
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kTruncate;
    }
    if ((bitwise_flags & static_cast<std::uint32_t>(O_DIRECTORY)) == static_cast<std::uint32_t>(O_DIRECTORY))
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kDirectory;
    }
    if ((bitwise_flags & static_cast<std::uint32_t>(O_APPEND)) == static_cast<std::uint32_t>(O_APPEND))
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kAppend;
    }
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef __linux__
    // LCOV_EXCL_START: Linux specific code, scope of codecoverage is only for qnx code
    if ((bitwise_flags & static_cast<std::uint32_t>(O_SYNC)) == static_cast<std::uint32_t>(O_SYNC))
    {
        fcntl_flags = fcntl_flags | Fcntl::Open::kSynchronized;
    }
    // LCOV_EXCL_STOP
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  // __linux__
    return fcntl_flags;
}

// SCORE_CCM_NO_LINT It is not possible to split the functionality in a meaniful way.
std::int32_t internal::fcntl_helper::OpenFlagToInteger(const Fcntl::Open flags) noexcept
{
    using utype_openflag = std::underlying_type<score::os::Fcntl::Open>::type;

    std::uint32_t native_flags{0x0U};
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kReadOnly) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_RDONLY);
    }
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kWriteOnly) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_WRONLY);
    }
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kReadWrite) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_RDWR);
    }
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kCreate) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_CREAT);
    }
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kCloseOnExec) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_CLOEXEC);
    }
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kNonBlocking) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_NONBLOCK);
    }
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kExclusive) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_EXCL);
    }
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kTruncate) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_TRUNC);
    }
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kDirectory) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_DIRECTORY);
    }
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kAppend) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_APPEND);
    }
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#ifdef __linux__
    // LCOV_EXCL_START: Linux specific code, scope of codecoverage is only for qnx code
    if (static_cast<utype_openflag>(flags & Fcntl::Open::kSynchronized) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(O_SYNC);
    }
    // LCOV_EXCL_STOP
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  // __linux__
    //  We can cast it because the largest possible value to be used is lower than the highest positive 32-bit signed
    //  integer
    return static_cast<std::int32_t>(native_flags);
}

std::int32_t internal::fcntl_helper::OperationFlagToInteger(const Fcntl::Operation op) noexcept
{
    using utype_operation = std::underlying_type<score::os::Fcntl::Operation>::type;

    std::uint32_t native_flags{0x0U};
    if (static_cast<utype_operation>(op & Fcntl::Operation::kLockShared) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(LOCK_SH);
    }
    if (static_cast<utype_operation>(op & Fcntl::Operation::kLockExclusive) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(LOCK_EX);
    }
    if (static_cast<utype_operation>(op & Fcntl::Operation::kLockNB) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(LOCK_NB);
    }
    if (static_cast<utype_operation>(op & Fcntl::Operation::kUnLock) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(LOCK_UN);
    }

    return static_cast<std::int32_t>(native_flags);
}

std::unique_ptr<score::os::Fcntl> score::os::Fcntl::Default() noexcept
{
    return std::make_unique<score::os::FcntlImpl>();
}

score::os::Fcntl& score::os::Fcntl::instance() noexcept
{
    static score::os::FcntlImpl instance; /* LCOV_EXCL_BR_LINE */
    /* All branches are generated by certified compiler, no additional check necessary. */
    return select_instance(instance);
}

// Justification: The identifier name of a non-member object with static storage duration or
// static function shall not be reused within a namespace.
// static function here is overloaded and used as a wrapper.
// coverity[autosar_cpp14_a2_10_4_violation]
score::cpp::pmr::unique_ptr<score::os::Fcntl> score::os::Fcntl::Default(score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    return score::cpp::pmr::make_unique<score::os::FcntlImpl>(memory_resource);
}

}  // namespace score::os
