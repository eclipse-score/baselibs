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
#ifndef SCORE_LIB_OS_SOCKET_H
#define SCORE_LIB_OS_SOCKET_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <sys/socket.h>
#include <unistd.h>

#if defined(__QNX__)
#include <sys/nto_version.h>
#endif  //__QNX__

namespace score
{
namespace os
{

class Socket : public ObjectSeam<Socket>
{
  public:
    static Socket& instance() noexcept;

    static score::cpp::pmr::unique_ptr<Socket> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    enum class MessageFlag : std::int32_t
    {
        kNone = 0x00,
        kPeek = 0x01,
        kWaitAll = 0x02,
        kOutOfBand = 0x04,
        kDONTUseGateway = 0x08,
        kNoSignal = 0x10,
        kTerminateRecord = 0x20,
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__linux__)
        kErrorQueue = 0x40,
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
        kWaitForOne = 0x80,
    };

    enum class Domain : std::int32_t
    {
        kInvalid = 0,
        kUnix,
        kIPv4,
        kIPv6,
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__linux__)
        kPacket,
        kNetlink,  // In some system has the same effect like kRoute
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif           // defined(__linux__)
        kRoute,  // In some system has the same effect like kNetlink
    };

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> socket(const Domain domain,
                                                      const std::int32_t type,
                                                      const std::int32_t protocol) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> bind(const std::int32_t sockfd,
                                            const struct sockaddr* const addr,
                                            const socklen_t addrlen) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> listen(const std::int32_t sockfd, const std::int32_t backlog) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> accept(const std::int32_t sockfd,
                                                      struct sockaddr* const addr,
                                                      socklen_t* const addrlen) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> connect(const std::int32_t sockfd,
                                               const struct sockaddr* const addr,
                                               const socklen_t addrlen) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> setsockopt(const std::int32_t sockfd,
                                                  const std::int32_t level,
                                                  const std::int32_t optname,
                                                  const void* const optval,
                                                  const socklen_t optlen) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> getsockopt(const std::int32_t sockfd,
                                                  const std::int32_t level,
                                                  const std::int32_t optname,
                                                  void* const optval,
                                                  socklen_t* const optlen) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<ssize_t, Error> recv(const std::int32_t sockfd,
                                               void* const buf,
                                               const size_t len,
                                               const MessageFlag flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<ssize_t, Error> recvfrom(const std::int32_t sockfd,
                                                   void* const buf,
                                                   const size_t len,
                                                   const MessageFlag flags,
                                                   sockaddr* const addr,
                                                   socklen_t* const addrlen) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<ssize_t, Error> sendto(const std::int32_t sockfd,
                                                 const void* const buf,
                                                 const size_t len,
                                                 const MessageFlag flags,
                                                 const struct sockaddr* const addr,
                                                 const socklen_t addrlen) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<ssize_t, Error> recvmsg(const std::int32_t sockfd,
                                                  msghdr* const message,
                                                  const MessageFlag flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<ssize_t, Error> recvmmsg(const std::int32_t sockfd,
                                                   mmsghdr* msgvec,
                                                   const unsigned int vlen,
                                                   const MessageFlag flags,
                                                   struct timespec* timeout) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<ssize_t, Error> send(const std::int32_t sockfd,
                                               void* const buf,
                                               const size_t len,
                                               const MessageFlag flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<ssize_t, Error> sendmsg(const std::int32_t sockfd,
                                                  const msghdr* const message,
                                                  const MessageFlag flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    //  Returns the number of bytes sent
    virtual score::cpp::expected<std::int32_t, Error> sendmmsg(const std::int32_t sockfd,
                                                        const mmsghdr* messages_array,
                                                        const std::uint32_t message_array_length,
                                                        const MessageFlag flags) const noexcept = 0;

    virtual ~Socket() = default;

  protected:
    Socket() = default;
    Socket(const Socket&) = default;
    Socket(Socket&&) = default;
    Socket& operator=(const Socket&) = default;
    Socket& operator=(Socket&&) = default;
};

}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Socket::MessageFlag> : public std::true_type
{
};
}  // namespace score

#endif  // SCORE_LIB_OS_SOCKET_H
