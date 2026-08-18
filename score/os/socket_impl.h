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
#ifndef SCORE_LIB_OS_SOCKET_IMPL_H
#define SCORE_LIB_OS_SOCKET_IMPL_H

#include "score/os/socket.h"

namespace score
{
namespace os
{
class SocketImpl final : public Socket
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, Error> socket(const Domain domain,
                                                     const std::int32_t type,
                                                     const std::int32_t protocol) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> bind(const std::int32_t sockfd,
                                           const struct sockaddr* const addr,
                                           const socklen_t addrlen) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> listen(const std::int32_t sockfd,
                                             const std::int32_t backlog) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, Error> accept(const std::int32_t sockfd,
                                                     struct sockaddr* const addr,
                                                     socklen_t* const addrlen) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> connect(const std::int32_t sockfd,
                                              const struct sockaddr* const addr,
                                              const socklen_t addrlen) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> setsockopt(const std::int32_t sockfd,
                                                 const std::int32_t level,
                                                 const std::int32_t optname,
                                                 const void* const optval,
                                                 const socklen_t optlen) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> getsockopt(const std::int32_t sockfd,
                                                 const std::int32_t level,
                                                 const std::int32_t optname,
                                                 void* const optval,
                                                 socklen_t* const optlen) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<ssize_t, Error> recv(const std::int32_t sockfd,
                                              void* const buf,
                                              const size_t len,
                                              const MessageFlag flags) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<ssize_t, Error> recvfrom(const std::int32_t sockfd,
                                                  void* const buf,
                                                  const size_t len,
                                                  const MessageFlag flags,
                                                  sockaddr* const from_address,
                                                  socklen_t* const address_length) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<ssize_t, Error> sendto(const std::int32_t sockfd,
                                                const void* const buf,
                                                const size_t len,
                                                const MessageFlag flags,
                                                const struct sockaddr* const addr,
                                                const socklen_t addrlen) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<ssize_t, Error> recvmsg(const std::int32_t sockfd,
                                                 msghdr* const message,
                                                 const MessageFlag flags) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<ssize_t, Error> recvmmsg(const std::int32_t sockfd,
                                                  mmsghdr* msgvec,
                                                  const unsigned int vlen,
                                                  const MessageFlag flags,
                                                  struct timespec* timeout) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<ssize_t, Error> send(const std::int32_t sockfd,
                                              void* const buf,
                                              const size_t len,
                                              const MessageFlag flags) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<ssize_t, Error> sendmsg(const std::int32_t sockfd,
                                                 const msghdr* const message,
                                                 const MessageFlag flags) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    score::cpp::expected<std::int32_t, Error> sendmmsg(const std::int32_t sockfd,
                                                       const mmsghdr* messages_array,
                                                       const std::uint32_t message_array_length,
                                                       const MessageFlag flags) const noexcept override;

  private:
    std::int32_t messageflag_to_nativeflag(const MessageFlag flags) const noexcept;
    std::int32_t domain_to_native(const Domain domain) const noexcept;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SOCKET_IMPL_H
