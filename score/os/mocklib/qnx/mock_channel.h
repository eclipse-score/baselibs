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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_CHANNEL_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_CHANNEL_H

#include "score/os/qnx/channel.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */

class MockChannel : public Channel
{
  public:
    MOCK_METHOD((score::cpp::expected<rcvid_t, score::os::Error>),
                MsgReceive,
                (std::int32_t chid, void* msg, std::size_t bytes, _msg_info* info),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<rcvid_t, score::os::Error>),
                MsgReceivev,
                (std::int32_t chid, const iov_t* riov, std::size_t rparts, struct _msg_info* info),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                MsgReceivePulse,
                (std::int32_t chid, void* pulse, std::size_t bytes, _msg_info* info),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                MsgReply,
                (rcvid_t rcvid, std::int64_t status, const void* msg, std::size_t bytes),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                MsgReplyv,
                (rcvid_t rcvid, std::int64_t status, const iov_t* riov, std::size_t rparts),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                MsgError,
                (rcvid_t rcvid, std::int32_t err),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int64_t, score::os::Error>),
                MsgSend,
                (std::int32_t coid, const void* smsg, std::size_t sbytes, void* rmsg, std::size_t rbytes),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int64_t, score::os::Error>),
                MsgSendv,
                (std::int32_t coid, const iov_t* siov, std::size_t sparts, iov_t* riov, std::size_t rparts),
                (const, noexcept, override));

    MOCK_METHOD(void, SetIov, (iov_t * msg, void* addr, size_t len), (const, noexcept, override));

    MOCK_METHOD(void, SetIovConst, (iov_t * msg, const void* addr, size_t len), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                MsgSendPulse,
                (std::int32_t coid, std::int32_t priority, std::int32_t code, std::int32_t value),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                MsgSendPulsePtr,
                (std::int32_t coid, std::int32_t priority, std::int32_t code, void* value),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                MsgDeliverEvent,
                (rcvid_t rcvid, const struct sigevent* event),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                ConnectClientInfo,
                (std::int32_t scoid, _client_info* info, std::int32_t ngroups),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                ConnectAttach,
                (const std::uint32_t reserved,
                 const pid_t pid,
                 const std::int32_t chid,
                 const std::uint32_t index,
                 const std::int32_t flags),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>), ConnectDetach, (std::int32_t coid), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                MsgRegisterEvent,
                (sigevent * ev, std::int32_t coid),
                (noexcept, override));
};

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_CHANNEL_H
