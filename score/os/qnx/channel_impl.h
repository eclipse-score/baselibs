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
#ifndef SCORE_LIB_OS_QNX_CHANNEL_IMPL_H
#define SCORE_LIB_OS_QNX_CHANNEL_IMPL_H

#include "score/os/qnx/channel.h"

namespace score
{
namespace os
{

class ChannelImpl final : public Channel
{
  public:
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<rcvid_t, score::os::Error> MsgReceive(const std::int32_t chid,
                                                      void* const msg,
                                                      const std::size_t bytes,
                                                      _msg_info* const info) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */

    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<rcvid_t, score::os::Error> MsgReceivev(const std::int32_t chid,
                                                       const iov_t* const riov,
                                                       const std::size_t rparts,
                                                       struct _msg_info* const info) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */

    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, score::os::Error> MsgReceivePulse(const std::int32_t chid,
                                                                void* const pulse,
                                                                const std::size_t bytes,
                                                                _msg_info* const info) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> MsgReply(const rcvid_t rcvid,
                                                 const std::int64_t status,
                                                 const void* const msg,
                                                 const std::size_t bytes) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> MsgReplyv(const rcvid_t rcvid,
                                                  const std::int64_t status,
                                                  const iov_t* const riov,
                                                  const std::size_t rparts) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> MsgError(const rcvid_t rcvid, const std::int32_t err) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int64_t, score::os::Error> MsgSend(const std::int32_t coid,
                                                        const void* const smsg,
                                                        const std::size_t sbytes,
                                                        void* const rmsg,
                                                        const std::size_t rbytes) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int64_t, score::os::Error> MsgSendv(const std::int32_t coid,
                                                         const iov_t* const siov,
                                                         const std::size_t sparts,
                                                         iov_t* const riov,
                                                         const std::size_t rparts) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    void SetIov(iov_t* const msg, void* const addr, const size_t len) const noexcept override;

    void SetIovConst(iov_t* const msg, const void* const addr, const size_t len) const noexcept override;

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> MsgSendPulse(const std::int32_t coid,
                                                     const std::int32_t priority,
                                                     const std::int32_t code,
                                                     const std::int32_t value) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> MsgSendPulsePtr(const std::int32_t coid,
                                                        const std::int32_t priority,
                                                        const std::int32_t code,
                                                        void* const value) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> MsgDeliverEvent(const rcvid_t rcvid,
                                                        const struct sigevent* const event) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> ConnectClientInfo(const std::int32_t scoid,
                                                          _client_info* const info,
                                                          const std::int32_t ngroups) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, score::os::Error> ConnectAttach(const std::uint32_t reserved,
                                                              const pid_t pid,
                                                              const std::int32_t chid,
                                                              const std::uint32_t index,
                                                              const std::int32_t flags) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> ConnectDetach(const std::int32_t coid) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    score::cpp::expected<std::int32_t, score::os::Error> MsgRegisterEvent(sigevent* ev, std::int32_t coid) noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_CHANNEL_IMPL_H
