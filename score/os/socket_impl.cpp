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

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SocketImpl::socket(const Domain domain,
                                                      const std::int32_t type,
                                                      const std::int32_t protocol) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
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

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<Error> SocketImpl::listen(const std::int32_t sockfd, const std::int32_t backlog) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::listen(sockfd, backlog) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SocketImpl::accept(const std::int32_t sockfd,
                                                      struct sockaddr* const addr,
                                                      socklen_t* const addrlen) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
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

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<Error> SocketImpl::bind(const std::int32_t sockfd,
                                            const struct sockaddr* const addr,
                                            const socklen_t addrlen) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::bind(sockfd, addr, addrlen) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<Error> SocketImpl::connect(const std::int32_t sockfd,
                                               const struct sockaddr* const addr,
                                               const socklen_t addrlen) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::connect(sockfd, addr, addrlen) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<Error> SocketImpl::setsockopt(const std::int32_t sockfd,
                                                  const std::int32_t level,
                                                  const std::int32_t optname,
                                                  const void* const optval,
                                                  const socklen_t optlen) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::setsockopt(sockfd, level, optname, optval, optlen) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<Error> SocketImpl::getsockopt(const std::int32_t sockfd,
                                                  const std::int32_t level,
                                                  const std::int32_t optname,
                                                  void* const optval,
                                                  socklen_t* const optlen) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::getsockopt(sockfd, level, optname, optval, optlen) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<ssize_t, Error> SocketImpl::recv(const std::int32_t sockfd,
                                               void* const buf,
                                               const size_t len,
                                               const MessageFlag flags) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const ssize_t ret = ::recv(sockfd, buf, len, messageflag_to_nativeflag(flags));
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<ssize_t, Error> SocketImpl::recvfrom(const std::int32_t sockfd,
                                                   void* const buf,
                                                   const size_t len,
                                                   const MessageFlag flags,
                                                   sockaddr* const from_address,
                                                   socklen_t* const address_length) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const ssize_t ret = ::recvfrom(sockfd, buf, len, messageflag_to_nativeflag(flags), from_address, address_length);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<ssize_t, Error> SocketImpl::sendto(const std::int32_t sockfd,
                                                 const void* const buf,
                                                 const size_t len,
                                                 const MessageFlag flags,
                                                 const struct sockaddr* const addr,
                                                 const socklen_t addrlen) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const ssize_t ret = ::sendto(sockfd, buf, len, messageflag_to_nativeflag(flags), addr, addrlen);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<ssize_t, Error> SocketImpl::recvmsg(const std::int32_t sockfd,
                                                  msghdr* const message,
                                                  const MessageFlag flags) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const ssize_t ret = ::recvmsg(sockfd, message, messageflag_to_nativeflag(flags));
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
// coverity[misra_cpp_2023_rule_6_9_2_violation]
score::cpp::expected<ssize_t, Error> SocketImpl::recvmmsg(const std::int32_t sockfd,
                                                   mmsghdr* msgvec,
                                                   const unsigned int vlen,
                                                   const MessageFlag flags,
                                                   struct timespec* timeout) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
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

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<ssize_t, Error> SocketImpl::send(const std::int32_t sockfd,
                                               void* const buf,
                                               const size_t len,
                                               const MessageFlag flags) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const ssize_t ret = ::send(sockfd, buf, len, messageflag_to_nativeflag(flags));
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<ssize_t, Error> SocketImpl::sendmsg(const std::int32_t sockfd,
                                                  const msghdr* const message,
                                                  const MessageFlag flags) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
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
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_flags |= static_cast<std::uint32_t>(MSG_PEEK);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    if (static_cast<utype_messageflag>(flags & MessageFlag::kWaitAll) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_flags |= static_cast<std::uint32_t>(MSG_WAITALL);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    if (static_cast<utype_messageflag>(flags & MessageFlag::kOutOfBand) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_flags |= static_cast<std::uint32_t>(MSG_OOB);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    if (static_cast<utype_messageflag>(flags & MessageFlag::kDONTUseGateway) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_flags |= static_cast<std::uint32_t>(MSG_DONTROUTE);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    if (static_cast<utype_messageflag>(flags & MessageFlag::kNoSignal) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_flags |= static_cast<std::uint32_t>(MSG_NOSIGNAL);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    if (static_cast<utype_messageflag>(flags & MessageFlag::kTerminateRecord) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_flags |= static_cast<std::uint32_t>(MSG_EOR);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
// LCOV_EXCL_START: we collect coverage for QNX only
#if defined(__linux__)
    if (static_cast<utype_messageflag>(flags & MessageFlag::kErrorQueue) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_flags |= static_cast<std::uint32_t>(MSG_ERRQUEUE);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    // LCOV_EXCL_STOP
    if (static_cast<utype_messageflag>(flags & MessageFlag::kWaitForOne) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_flags |= static_cast<std::uint32_t>(MSG_WAITFORONE);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }

    return static_cast<std::int32_t>(native_flags);
}

std::int32_t SocketImpl::domain_to_native(const Domain domain) const noexcept
{
    switch (domain)
    {
        case Domain::kIPv4:
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
            return AF_INET;
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        case Domain::kIPv6:
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
            return AF_INET6;
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        case Domain::kUnix:
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
            return AF_UNIX;
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
// Supress lcov coverage as we're collecting coverage for QNX only
// LCOV_EXCL_START
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__linux__)
        case Domain::kPacket:
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
            return AF_PACKET;
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        case Domain::kNetlink:
            // In systems where AF_ROUTE is the same like AF_NETLINK, both enum values will map to AF_ROUTE
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Suppression is necessary for selecting the appropriate socket address family at compile time,
// ensuring platform compatibility and preventing undefined behavior.
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if AF_ROUTE != AF_NETLINK
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
            return AF_NETLINK;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  // AF_ROUTE != AF_NETLINK
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  // defined(__linux__)
        // LCOV_EXCL_STOP

        case Domain::kRoute:
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
            return AF_ROUTE;
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        /* KW_SUPPRESS_START:MISRA.SWITCH.NO_BREAK:Intentionally the default operation */
        case Domain::kInvalid:
        /* KW_SUPPRESS_END:MISRA.SWITCH.NO_BREAK:Intentionally the default operation */
        default:
            // We do not have exceptions, we do not have logging, we do not want to support this
            // as a valid use case. The best we can do is to terminate
            std::terminate();
    }
}

}  // namespace os
}  // namespace score
