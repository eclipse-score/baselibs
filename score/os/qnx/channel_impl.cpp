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
#include "score/os/qnx/channel_impl.h"

namespace score
{
namespace os
{

static_assert(sizeof(std::size_t) == sizeof(_Sizet), "unexpected _Sizet");

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
score::cpp::expected<rcvid_t, score::os::Error> ChannelImpl::MsgReceive(const std::int32_t chid,
                                                               void* const msg,
                                                               const std::size_t bytes,
                                                               _msg_info* const info) const noexcept
/* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const rcvid_t result = ::MsgReceive(chid, msg, bytes, info);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
score::cpp::expected<rcvid_t, score::os::Error> ChannelImpl::MsgReceivev(const std::int32_t chid,
                                                                const iov_t* const riov,
                                                                const std::size_t rparts,
                                                                struct _msg_info* const info) const noexcept
/* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const rcvid_t result = ::MsgReceivev(chid, riov, rparts, info);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
score::cpp::expected<std::int32_t, score::os::Error> ChannelImpl::MsgReceivePulse(const std::int32_t chid,
                                                                         void* const pulse,
                                                                         const std::size_t bytes,
                                                                         _msg_info* const info) const noexcept
/* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t result = ::MsgReceivePulse(chid, pulse, bytes, info);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> ChannelImpl::MsgReply(const rcvid_t rcvid,
                                                          const std::int64_t status,
                                                          const void* const msg,
                                                          const std::size_t bytes) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::MsgReply(rcvid, status, msg, bytes) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> ChannelImpl::MsgReplyv(const rcvid_t rcvid,
                                                           const std::int64_t status,
                                                           const iov_t* const riov,
                                                           const std::size_t rparts) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::MsgReplyv(rcvid, status, riov, rparts) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> ChannelImpl::MsgError(const rcvid_t rcvid, const std::int32_t err) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::MsgError(rcvid, err) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int64_t, score::os::Error> ChannelImpl::MsgSend(const std::int32_t coid,
                                                                 const void* const smsg,
                                                                 const std::size_t sbytes,
                                                                 void* const rmsg,
                                                                 const std::size_t rbytes) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int64_t result = ::MsgSend(coid, smsg, sbytes, rmsg, rbytes);
    if (result == -1L)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int64_t, score::os::Error> ChannelImpl::MsgSendv(const std::int32_t coid,
                                                                  const iov_t* const siov,
                                                                  const std::size_t sparts,
                                                                  iov_t* const riov,
                                                                  const std::size_t rparts) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int64_t result = ::MsgSendv(coid, siov, sparts, riov, rparts);
    if (result == -1L)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

// NOLINTBEGIN(score-banned-preprocessor-directives) see below
// This rule stated: "The #pragma directive shall not be used"
// rationale: pre-processor directives are required for diagnostic warning "-Wc99-extensions"

// this is a wrapper only, pointers will be used below anyway
// coverity[autosar_cpp14_a8_4_10_violation]
void ChannelImpl::SetIov(iov_t* const msg, void* const addr, const size_t len) const noexcept
{
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__clang__)
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_a16_7_1_violation]
#pragma clang diagnostic push
// Rationale: C-style cast is used in SETIOV, this is defined in a qnx internal code so we cannot change it
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_a16_7_1_violation]
#pragma clang diagnostic warning "-Wc99-extensions"
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
    // Cast is happening outside our code domain
    // coverity[autosar_cpp14_a5_2_2_violation]
    // coverity[autosar_cpp14_m7_3_1_violation]
    SETIOV(msg, addr, len);
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__clang__)
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_a16_7_1_violation]
#pragma clang diagnostic pop
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
}

// this is a wrapper only, pointers will be used below anyway
// coverity[autosar_cpp14_a8_4_10_violation]
void ChannelImpl::SetIovConst(iov_t* const msg, const void* const addr, const size_t len) const noexcept
{
/* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__clang__)
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_a16_7_1_violation]
#pragma clang diagnostic push
// Rationale: C-style cast is used in SETIOV, this is defined in a qnx internal code so we cannot change it
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_a16_7_1_violation]
#pragma clang diagnostic warning "-Wc99-extensions"
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
    SETIOV_CONST(msg, addr, len);
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__clang__)
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_a16_7_1_violation]
#pragma clang diagnostic pop
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
}
// NOLINTEND(score-banned-preprocessor-directives)

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> ChannelImpl::MsgSendPulse(const std::int32_t coid,
                                                              const std::int32_t priority,
                                                              const std::int32_t code,
                                                              const std::int32_t value) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTBEGIN(score-banned-function) see comment above
    // Suppress AUTOSAR C++14 M5-0-3 rule findings: "A cvalue expression shall not be implicitly converted to a
    // different underlying type." Rationale: MsgSendPulse is a C system API that signals failure by returning -1 The
    // comparison follows the API definition and is safe.
    // coverity[autosar_cpp14_m5_0_3_violation]
    if (::MsgSendPulse(coid, priority, code, value) == -1)
    // NOLINTEND(score-banned-function) see comment above
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> ChannelImpl::MsgSendPulsePtr(const std::int32_t coid,
                                                                 const std::int32_t priority,
                                                                 const std::int32_t code,
                                                                 void* const value) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::MsgSendPulsePtr(coid, priority, code, value) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> ChannelImpl::MsgDeliverEvent(const rcvid_t rcvid,
                                                                 const struct sigevent* const event) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::MsgDeliverEvent(rcvid, event) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> ChannelImpl::ConnectClientInfo(const std::int32_t scoid,
                                                                   _client_info* const info,
                                                                   const std::int32_t ngroups) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::ConnectClientInfo(scoid, info, ngroups) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> ChannelImpl::ConnectAttach(const std::uint32_t reserved,
                                                                       const pid_t pid,
                                                                       const std::int32_t chid,
                                                                       const std::uint32_t index,
                                                                       const std::int32_t flags) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t result = ::ConnectAttach(reserved, pid, chid, index, flags);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> ChannelImpl::ConnectDetach(const std::int32_t coid) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::ConnectDetach(coid) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<std::int32_t, score::os::Error> ChannelImpl::MsgRegisterEvent(sigevent* ev, std::int32_t coid) noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t result = ::MsgRegisterEvent(ev, coid);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}
}  // namespace os
}  // namespace score
