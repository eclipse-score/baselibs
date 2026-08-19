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
#include "score/os/mqueue.h"

#include <fcntl.h>
#include <sys/stat.h>

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{
namespace impl
{

score::cpp::expected<std::int32_t, Error> MqueueImpl::mq_open(const char* const name,
                                                              const OpenFlag flags,
                                                              const ModeFlag perm,
                                                              mq_attr* const attr) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function, cppcoreguidelines-pro-type-vararg) see comment above
    const std::int32_t ret = ::mq_open(name, openflag_to_nativeflag(flags), modeflag_to_nativeflag(perm), attr);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret; /* KW_SUPPRESS:RH.LEAK: acquired resource not lost, no need to release it if ret == -1 */
}

score::cpp::expected<std::int32_t, Error> MqueueImpl::mq_open(const char* const name,
                                                              const OpenFlag flags) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function, cppcoreguidelines-pro-type-vararg) see comment above
    const std::int32_t ret = ::mq_open(name, openflag_to_nativeflag(flags));
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret; /* KW_SUPPRESS:RH.LEAK: acquired resource not lost, no need to release it if ret == -1 */
}

score::cpp::expected_blank<Error> MqueueImpl::mq_unlink(const char* const name) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::mq_unlink(name) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<Error> MqueueImpl::mq_send(const mqd_t mqdes,
                                                      const char* const msg_ptr,
                                                      const size_t msg_len,
                                                      const std::uint32_t msg_prio) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::mq_send(mqdes, msg_ptr, msg_len, msg_prio) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<Error> MqueueImpl::mq_timedsend(const mqd_t mqdes,
                                                           const char* const msg_ptr,
                                                           const size_t msg_len,
                                                           const std::uint32_t msg_prio,
                                                           const struct timespec* const timeout) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, timeout) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<ssize_t, Error> MqueueImpl::mq_receive(const mqd_t mqdes,
                                                            char* const msg_ptr,
                                                            const size_t msg_len,
                                                            std::uint32_t* const msg_prio) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const ssize_t ret = ::mq_receive(mqdes, msg_ptr, msg_len, msg_prio);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<ssize_t, Error> MqueueImpl::mq_timedreceive(const mqd_t mqdes,
                                                                 char* const msg_ptr,
                                                                 const size_t msg_len,
                                                                 std::uint32_t* const msg_prio,
                                                                 const struct timespec* const timeout) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const ssize_t ret = ::mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, timeout);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected_blank<Error> MqueueImpl::mq_close(const mqd_t mqdes) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::mq_close(mqdes) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<Error> MqueueImpl::mq_getattr(const mqd_t mqdes, mq_attr& mqstat) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::mq_getattr(mqdes, &mqstat) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

std::int32_t MqueueImpl::openflag_to_nativeflag(const OpenFlag flags) const noexcept
{
    const auto fn_test_flag = [flags](const OpenFlag flag) -> bool {
        return (static_cast<std::uint32_t>(flags) & static_cast<std::uint32_t>(flag)) != 0U;
    };

    /*KW_SUPPRESS_START:MISRA.USE.EXPANSION: macros used as open flags */
    std::uint32_t native_flags{};
    if (fn_test_flag(OpenFlag::kReadOnly))
    {
        native_flags |= static_cast<std::uint32_t>(O_RDONLY);
    }
    if (fn_test_flag(OpenFlag::kWriteOnly))
    {
        native_flags |= static_cast<std::uint32_t>(O_WRONLY);
    }
    if (fn_test_flag(OpenFlag::kReadWrite))
    {
        native_flags |= static_cast<std::uint32_t>(O_RDWR);
    }
    if (fn_test_flag(OpenFlag::kCreate))
    {
        native_flags |= static_cast<std::uint32_t>(O_CREAT);
    }
    if (fn_test_flag(OpenFlag::kCloseOnExec))
    {
        native_flags |= static_cast<std::uint32_t>(O_CLOEXEC);
    }
    if (fn_test_flag(OpenFlag::kNonBlocking))
    {
        native_flags |= static_cast<std::uint32_t>(O_NONBLOCK);
    }
    if (fn_test_flag(OpenFlag::kExclusive))
    {
        native_flags |= static_cast<std::uint32_t>(O_EXCL);
    }
    /*KW_SUPPRESS_END:MISRA.USE.EXPANSION */
    return static_cast<std::int32_t>(native_flags);
}

// SCORE_CCM_NO_LINT It is not possible to split the functionality in a meaniful way.
mode_t MqueueImpl::modeflag_to_nativeflag(const ModeFlag flags) const noexcept
{
    const auto fn_test_flag = [flags](const ModeFlag flag) -> bool {
        /* KW_SUPPRESS_START:MISRA.CVALUE.IMPL.CAST.CPP: cast is explicit */
        return (static_cast<std::uint32_t>(flags) & static_cast<std::uint32_t>(flag)) != 0U;
        /* KW_SUPPRESS_END:MISRA.CVALUE.IMPL.CAST.CPP */
    };

    /*KW_SUPPRESS_START:MISRA.USE.EXPANSION: macros used as mode flags */
    /*KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: bit operations are performed on an unsigned integers */
    std::uint32_t native_flags{};
    if (fn_test_flag(ModeFlag::kReadUser))
    {
        native_flags |= static_cast<std::uint32_t>(S_IRUSR);
    }
    if (fn_test_flag(ModeFlag::kWriteUser))
    {
        native_flags |= static_cast<std::uint32_t>(S_IWUSR);
    }
    if (fn_test_flag(ModeFlag::kExecUser))
    {
        native_flags |= static_cast<std::uint32_t>(S_IXUSR);
    }
    if (fn_test_flag(ModeFlag::kReadGroup))
    {
        native_flags |= static_cast<std::uint32_t>(S_IRGRP);
    }
    if (fn_test_flag(ModeFlag::kWriteGroup))
    {
        native_flags |= static_cast<std::uint32_t>(S_IWGRP);
    }
    if (fn_test_flag(ModeFlag::kExecGroup))
    {
        native_flags |= static_cast<std::uint32_t>(S_IXGRP);
    }
    if (fn_test_flag(ModeFlag::kReadOthers))
    {
        native_flags |= static_cast<std::uint32_t>(S_IROTH);
    }
    if (fn_test_flag(ModeFlag::kWriteOthers))
    {
        native_flags |= static_cast<std::uint32_t>(S_IWOTH);
    }
    if (fn_test_flag(ModeFlag::kExecOthers))
    {
        native_flags |= static_cast<std::uint32_t>(S_IXOTH);
    }
    /*KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED */
    /*KW_SUPPRESS_END:MISRA.USE.EXPANSION */
    return native_flags;
}

}  // namespace impl
}  // namespace os
}  // namespace score

score::os::Mqueue& score::os::Mqueue::instance() noexcept
{
    return select_instance(

        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
        // Suppress “AUTOSAR_Cpp14_A5_2_4” rule finding: “Reinterpret_cast shall not be used.”
        // Rationale: Reinterpret_cast is used here to ensure proper type handling of the underlying storage
        // (StaticDestructionGuard<impl::MqueueImpl>::GetStorage()), allowing correct object destruction. This usage is
        // considered safe in this context, as it involves casting an object from static storage with a well-defined
        // type relationship. Despite AUTOSAR A5-2-4 discouraging reinterpret_cast, it is necessary in this specific
        // scenario.
        // coverity[autosar_cpp14_a5_2_4_violation]
        reinterpret_cast<impl::MqueueImpl&>(utils::StaticDestructionGuard<impl::MqueueImpl>::GetStorage()));
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
}

/* KW_SUPPRESS_START:MISRA.PPARAM.NEEDS.CONST,MISRA.VAR.NEEDS.CONST: */
/* score::cpp::pmr::make_unique takes non-const memory_resource */
score::cpp::pmr::unique_ptr<score::os::Mqueue> score::os::Mqueue::Default(
    score::cpp::pmr::memory_resource* memory_resource) noexcept
/* KW_SUPPRESS_END:MISRA.PPARAM.NEEDS.CONST,MISRA.VAR.NEEDS.CONST */
{
    return score::cpp::pmr::make_unique<impl::MqueueImpl>(memory_resource);
}

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
