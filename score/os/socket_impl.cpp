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
#include "score/os/socket_impl.h"
#include <type_traits>

namespace score
{
namespace os
{

score::cpp::expected<std::int32_t, Error> SocketImpl::socket(const Domain domain,
                                                             const std::int32_t type,
                                                             const std::int32_t protocol) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret = ::socket(domain_to_native(domain), type, protocol);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected_blank<Error> SocketImpl::listen(const std::int32_t sockfd,
                                                     const std::int32_t backlog) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::listen(sockfd, backlog) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<std::int32_t, Error> SocketImpl::accept(const std::int32_t sockfd,
                                                             struct sockaddr* const addr,
                                                             socklen_t* const addrlen) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret = ::accept(sockfd, addr, addrlen);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected_blank<Error> SocketImpl::bind(const std::int32_t sockfd,
                                                   const struct sockaddr* const addr,
                                                   const socklen_t addrlen) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::bind(sockfd, addr, addrlen) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<Error> SocketImpl::connect(const std::int32_t sockfd,
                                                      const struct sockaddr* const addr,
                                                      const socklen_t addrlen) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::connect(sockfd, addr, addrlen) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<Error> SocketImpl::setsockopt(const std::int32_t sockfd,
                                                         const std::int32_t level,
                                                         const std::int32_t optname,
                                                         const void* const optval,
                                                         const socklen_t optlen) const noexcept
{
    if (::setsockopt(sockfd, level, optname, optval, optlen) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<Error> SocketImpl::getsockopt(const std::int32_t sockfd,
                                                         const std::int32_t level,
                                                         const std::int32_t optname,
                                                         void* const optval,
                                                         socklen_t* const optlen) const noexcept
{
    if (::getsockopt(sockfd, level, optname, optval, optlen) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<ssize_t, Error> SocketImpl::recv(const std::int32_t sockfd,
                                                      void* const buf,
                                                      const size_t len,
                                                      const MessageFlag flags) const noexcept
{
    const ssize_t ret = ::recv(sockfd, buf, len, messageflag_to_nativeflag(flags));
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<ssize_t, Error> SocketImpl::recvfrom(const std::int32_t sockfd,
                                                          void* const buf,
                                                          const size_t len,
                                                          const MessageFlag flags,
                                                          sockaddr* const from_address,
                                                          socklen_t* const address_length) const noexcept
{
    const ssize_t ret = ::recvfrom(sockfd, buf, len, messageflag_to_nativeflag(flags), from_address, address_length);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<ssize_t, Error> SocketImpl::sendto(const std::int32_t sockfd,
                                                        const void* const buf,
                                                        const size_t len,
                                                        const MessageFlag flags,
                                                        const struct sockaddr* const addr,
                                                        const socklen_t addrlen) const noexcept
{
    const ssize_t ret = ::sendto(sockfd, buf, len, messageflag_to_nativeflag(flags), addr, addrlen);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<ssize_t, Error> SocketImpl::recvmsg(const std::int32_t sockfd,
                                                         msghdr* const message,
                                                         const MessageFlag flags) const noexcept
{
    const ssize_t ret = ::recvmsg(sockfd, message, messageflag_to_nativeflag(flags));
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

// coverity[misra_cpp_2023_rule_6_9_2_violation]
score::cpp::expected<ssize_t, Error> SocketImpl::recvmmsg(const std::int32_t sockfd,
                                                          mmsghdr* msgvec,
                                                          const unsigned int vlen,
                                                          const MessageFlag flags,
                                                          struct timespec* timeout) const noexcept
{
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__QNX__)
// coverity[autosar_cpp14_a16_0_1_violation]
#if __QNX__ >= 800
    const std::int32_t message_flags{messageflag_to_nativeflag(flags)};
// coverity[autosar_cpp14_a16_0_1_violation]
#else
    const std::uint32_t message_flags{static_cast<std::uint32_t>(messageflag_to_nativeflag(flags))};
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
    // LCOV_EXCL_START we're collecting coverage for QNX only
    const std::int32_t message_flags{messageflag_to_nativeflag(flags)};
    // LCOV_EXCL_STOP
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    // Rationale: Conversions are intentional to match syscall interface; safe and required.
    // coverity[autosar_cpp14_m5_0_3_violation]
    // coverity[autosar_cpp14_m5_0_4_violation]
    // coverity[misra_cpp_2023_rule_7_0_6_violation]
    const ssize_t ret = ::recvmmsg(sockfd, msgvec, vlen, message_flags, timeout);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<ssize_t, Error> SocketImpl::send(const std::int32_t sockfd,
                                                      void* const buf,
                                                      const size_t len,
                                                      const MessageFlag flags) const noexcept
{
    const ssize_t ret = ::send(sockfd, buf, len, messageflag_to_nativeflag(flags));
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<ssize_t, Error> SocketImpl::sendmsg(const std::int32_t sockfd,
                                                         const msghdr* const message,
                                                         const MessageFlag flags) const noexcept
{
    const ssize_t ret = ::sendmsg(sockfd, message, messageflag_to_nativeflag(flags));
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> SocketImpl::sendmmsg(const std::int32_t sockfd,
                                                               const mmsghdr* messages_array,
                                                               const std::uint32_t message_array_length,
                                                               const MessageFlag flags) const noexcept
{
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__QNX__)
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if __QNX__ >= 800
    const std::int32_t message_flags{messageflag_to_nativeflag(flags)};
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
    const std::uint32_t message_flags{static_cast<std::uint32_t>(messageflag_to_nativeflag(flags))};
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
    // LCOV_EXCL_START we're collecting coverage for QNX only
    const std::int32_t message_flags{messageflag_to_nativeflag(flags)};
    // LCOV_EXCL_STOP
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    // Suppress "AUTOSAR C++14 A5-2-3" rule finding: A cast shall not remove any const or volatile
    // qualification from the type of a pointer or reference.
    // Rationale : const_cast is necessary to remove const qualifier in order to adjust constant
    // message to standard API-sendmmsg
    // Suppress MISRA/AUTOSAR/CERT warnings related to const_cast and numeric conversions
    // Rationale: Conversions are intentional to match syscall interface; safe and required.
    const auto ret =
        // coverity[autosar_cpp14_a5_2_3_violation]
        // coverity[misra_cpp_2023_rule_8_2_3_violation]
        // coverity[cert_exp55_cpp_violation]
        // coverity[autosar_cpp14_m5_0_3_violation]
        // coverity[autosar_cpp14_m5_0_4_violation]
        // coverity[misra_cpp_2023_rule_7_0_6_violation]
        ::sendmmsg(sockfd, const_cast<mmsghdr*>(messages_array), message_array_length, message_flags);

    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return static_cast<std::int32_t>(ret);
}

std::int32_t SocketImpl::messageflag_to_nativeflag(const MessageFlag flags) const noexcept
{
    using utype_messageflag = std::underlying_type<score::os::Socket::MessageFlag>::type;
    std::uint32_t native_flags{};
    if (static_cast<utype_messageflag>(flags & MessageFlag::kPeek) != 0)
    {
        native_flags |= static_cast<std::uint32_t>(MSG_PEEK);
    }
    if (static_cast<utype_messageflag>(flags & MessageFlag::kWaitAll) != 0)
    {
        native_flags |= static_cast<std::uint32_t>(MSG_WAITALL);
    }
    if (static_cast<utype_messageflag>(flags & MessageFlag::kOutOfBand) != 0)
    {
        native_flags |= static_cast<std::uint32_t>(MSG_OOB);
    }
    if (static_cast<utype_messageflag>(flags & MessageFlag::kDONTUseGateway) != 0)
    {
        native_flags |= static_cast<std::uint32_t>(MSG_DONTROUTE);
    }
    if (static_cast<utype_messageflag>(flags & MessageFlag::kNoSignal) != 0)
    {
        native_flags |= static_cast<std::uint32_t>(MSG_NOSIGNAL);
    }
    if (static_cast<utype_messageflag>(flags & MessageFlag::kTerminateRecord) != 0)
    {
        native_flags |= static_cast<std::uint32_t>(MSG_EOR);
    }
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
// LCOV_EXCL_START: we collect coverage for QNX only
#if defined(__linux__)
    if (static_cast<utype_messageflag>(flags & MessageFlag::kErrorQueue) != 0)
    {
        native_flags |= static_cast<std::uint32_t>(MSG_ERRQUEUE);
    }
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    // LCOV_EXCL_STOP
    if (static_cast<utype_messageflag>(flags & MessageFlag::kWaitForOne) != 0)
    {
        native_flags |= static_cast<std::uint32_t>(MSG_WAITFORONE);
    }

    return static_cast<std::int32_t>(native_flags);
}

std::int32_t SocketImpl::domain_to_native(const Domain domain) const noexcept
{
    switch (domain)
    {
        case Domain::kIPv4:
            return AF_INET;
        case Domain::kIPv6:
            return AF_INET6;
        case Domain::kUnix:
            return AF_UNIX;
// Supress lcov coverage as we're collecting coverage for QNX only
// LCOV_EXCL_START
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__linux__)
        case Domain::kPacket:
            return AF_PACKET;
        case Domain::kNetlink:
            // In systems where AF_ROUTE is the same like AF_NETLINK, both enum values will map to AF_ROUTE
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Suppression is necessary for selecting the appropriate socket address family at compile time,
// ensuring platform compatibility and preventing undefined behavior.
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if AF_ROUTE != AF_NETLINK
            return AF_NETLINK;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  // AF_ROUTE != AF_NETLINK
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  // defined(__linux__)
        // LCOV_EXCL_STOP

        case Domain::kRoute:
            return AF_ROUTE;
        case Domain::kInvalid:
        default:
            // We do not have exceptions, we do not have logging, we do not want to support this
            // as a valid use case. The best we can do is to terminate
            std::terminate();
    }
}

}  // namespace os
}  // namespace score
