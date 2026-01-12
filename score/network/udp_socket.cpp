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
#include "score/network/udp_socket.h"

#include "score/os/fcntl.h"
#include "score/os/socket.h"
#include "score/os/unistd.h"

#include <score/assert.hpp>
#include <score/vector.hpp>

#include <tuple>

// Note 1
// we need a reinterpret_cast to convert from sockaddr_in to sockaddr.
// sockaddr is what is expected by the bind call.

namespace
{
constexpr std::int32_t INVALID_SOCKET_ID = -1;

void CloseFileDescriptorIfValidAndIgnoreError(const score::cpp::expected<std::int32_t, score::os::Error>& file_descriptor)
{
    if (file_descriptor.has_value() && (file_descriptor.value() != INVALID_SOCKET_ID))
    {
        // We need to decide how to proceed in case of error on close. At least we should display a log but then
        // we need to decide if we do it on std::cerr/cout or to ara::log. We could also template the class with an
        // ostream or any kind of injection of the ostream to be used.
        // Additionally, we might want to consider some action for some of the errors like EINTR.
        // Suppressed here because usage of banned OSAL method in OSAL modules is allowed."
        // NOLINTNEXTLINE(score-banned-function) see comment above
        std::ignore = score::os::Unistd::instance().close(file_descriptor.value());
    }
}
}  // namespace

score::cpp::expected<sockaddr_in, score::os::Error> score::os::GetSockAddrInFromIpAndPort(const score::os::Ipv4Address& address,
                                                                               const std::uint16_t port)
{
    sockaddr_in sockaddr_in{};
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(port);

    // JUSTIFICATION_BEGIN
    // \ID                score-qnx-banned-builtin
    // \RELATED_RULES     spp-quality-clang-tidy-qnx8-banned-builtins, posix-usage-check
    // \DESCRIPTION       The POSIX API inet_aton() is used to convert an IPv4 address string into its binary
    //                    representation (in_addr). The QNX 7/8 implementation internally maps this function
    //                    to __inet_aton, which triggers the BMW-QNX banned-builtin warning. This is a false
    //                    positive because inet_aton() does not depend on low-level builtins such as MMX/SSE
    //                    or compiler intrinsics; it is a deterministic and bounded library call.
    // \COUNTERMEASURE    The input is prevalidated by higher-level address utilities before conversion.
    //                    The function is reentrant, side-effect-free, and compliant with POSIX.
    //                    The usage has been reviewed and approved for both QM and ASIL network modules.
    // JUSTIFICATION_END

    // coverity[score-qnx-banned-builtin] see above justification
    // NOLINTNEXTLINE(score-qnx-banned-builtin): see above justification
    if (::inet_aton(address.ToString().c_str(), &sockaddr_in.sin_addr) == 0)  // LCOV_EXCL_BR_LINE: justification below
    {
        // Manual analysis: According to the QNX documentation, ::inet_aton fails when given an invalid string.
        // However, this is impossible to test through unit tests because, in the case of an invalid string,
        // the score::os::Ipv4Address constructor sets the default address to `0.0.0.0`, preventing ::inet_aton from
        // failing.
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));  // LCOV_EXCL_LINE: justification above
    }

    return sockaddr_in;
}

score::os::UdpSocket::UdpSocket(std::int32_t file_descriptor) noexcept : file_descriptor_{file_descriptor} {}

score::os::UdpSocket::~UdpSocket() noexcept
{
    CloseFileDescriptorIfValidAndIgnoreError(file_descriptor_);
}

score::cpp::expected<score::os::UdpSocket, score::os::Error> score::os::UdpSocket::Make() noexcept
{
    auto file_descriptor_expected = score::os::Socket::instance().socket(score::os::Socket::Domain::kIPv4, SOCK_DGRAM, 0);
    if (!file_descriptor_expected.has_value())
    {
        return score::cpp::make_unexpected(file_descriptor_expected.error());
    }

    auto flags_exp = score::os::Fcntl::instance().fcntl(file_descriptor_expected.value(),
                                                      score::os::Fcntl::Command::kFileGetStatusFlags);
    if (!flags_exp.has_value())
    {
        return score::cpp::make_unexpected(flags_exp.error());
    }

    auto ret = score::os::Fcntl::instance().fcntl(file_descriptor_expected.value(),
                                                score::os::Fcntl::Command::kFileSetStatusFlags,
                                                flags_exp.value() | score::os::Fcntl::Open::kNonBlocking);
    if (!ret.has_value())
    {
        return score::cpp::make_unexpected(ret.error());
    }

    UdpSocket socket{file_descriptor_expected.value()};
    return socket;
}

score::os::UdpSocket::UdpSocket(UdpSocket&& other) noexcept : file_descriptor_{other.file_descriptor_}
{
    other.file_descriptor_ = INVALID_SOCKET_ID;
}

score::os::UdpSocket& score::os::UdpSocket::operator=(UdpSocket&& other) noexcept
{
    CloseFileDescriptorIfValidAndIgnoreError(this->file_descriptor_);

    this->file_descriptor_ = other.file_descriptor_;
    other.file_descriptor_ = INVALID_SOCKET_ID;
    return *this;
}

std::int32_t score::os::UdpSocket::GetFileDescriptor() const noexcept
{
    return this->file_descriptor_;
}

score::cpp::expected_blank<score::os::Error> score::os::UdpSocket::Bind(const Ipv4Address& address, std::uint16_t port) noexcept
{
    if (!address.IsValid())
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
    }

    auto src_expected = GetSockAddrInFromIpAndPort(address, port);
    if (!src_expected.has_value())  // LCOV_EXCL_BR_LINE: Justification below
    {
        // Manual analysis: According to the QNX documentation, ::inet_aton fails when given an invalid string.
        // However, this is impossible to test through unit tests because, in the case of an invalid string,
        // the score::os::Ipv4Address constructor sets the default address to `0.0.0.0`, preventing ::inet_aton from
        // failing.
        return score::cpp::make_unexpected(src_expected.error());  // LCOV_EXCL_LINE: Justification above
    }
    const sockaddr_in src = src_expected.value();

    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) see Note 1
    // In the following line we need a reinterpret_cast to convert from sockaddr_in to sockaddr.
    // sockaddr is what is expected by the bind call.
    // Even though reinterpret_cast should be avoided, in this case is how you are supposed to do it
    // with the posix API.
    // See https://pubs.opengroup.org/onlinepubs/009695399/basedefs/netinet/in.h.html
    // coverity[autosar_cpp14_a5_2_4_violation] : safe reinterpret_cast, see justification above
    const auto addr = reinterpret_cast<const struct sockaddr*>(&src);
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast) see Note 1
    return score::os::Socket::instance().bind(file_descriptor_, addr, sizeof(*addr));
}

score::cpp::expected<ssize_t, score::os::Error> score::os::UdpSocket::TryReceive(unsigned char* const buffer,
                                                                      const std::size_t length) noexcept
{
    return Socket::instance().recvfrom(file_descriptor_, buffer, length, Socket::MessageFlag::kNone, nullptr, nullptr);
}

score::cpp::expected<std::tuple<ssize_t, score::os::Ipv4Address>, score::os::Error> score::os::UdpSocket::TryReceiveWithAddress(
    unsigned char* const buffer,
    const std::size_t length) noexcept
{
    sockaddr_in source_address{};
    socklen_t address_length = sizeof(source_address);

    auto result = Socket::instance().recvfrom(file_descriptor_,
                                              buffer,
                                              length,
                                              Socket::MessageFlag::kNone,
                                              // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) see Note 1
                                              // coverity[autosar_cpp14_a5_2_4_violation] : safe reinterpret_cast
                                              reinterpret_cast<struct sockaddr*>(&source_address),
                                              // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast) see Note 1
                                              &address_length);

    if (result.has_value() == true)
    {
        const ssize_t numBytes = *result;
        const Ipv4Address senderAddress = Ipv4Address::CreateFromUint32NetOrder(source_address.sin_addr.s_addr);
        return std::make_tuple(numBytes, senderAddress);
    }
    else
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
}

score::cpp::expected<score::cpp::pmr::vector<std::tuple<ssize_t, score::os::Ipv4Address>>, score::os::Error>
score::os::UdpSocket::TryReceiveMultipleMessagesWithAddress(unsigned char* const recv_bufs,
                                                          const std::size_t recv_buffer_size,
                                                          const std::size_t vlen,
                                                          const std::size_t msg_length) noexcept
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(!((vlen * msg_length) > recv_buffer_size));
    score::cpp::pmr::vector<struct mmsghdr> msgs(vlen);
    score::cpp::pmr::vector<struct iovec> iovecs(vlen);
    score::cpp::pmr::vector<struct sockaddr_in> sock_from(vlen);
    constexpr struct timespec* const timeout = nullptr;

    // NOLINTBEGIN(score-banned-function) Suppressed here because usage of this OSAL method is on banned list
    // Suppress "AUTOSAR C++14 A12-0-2" The rule states: "Bitwise operations and operations that assume data
    // representation in memory shall not be performed on objects." The object being initialized using memset
    // (score::cpp::pmr::vector) uses sequential memory and can be initialized using memset. This low-level operation
    // is necessary for performance optimization.
    // coverity[autosar_cpp14_a12_0_2_violation]
    std::memset(msgs.data(), 0, msgs.size() * sizeof(struct mmsghdr));
    // NOLINTEND(score-banned-function) Suppressed here because usage of this OSAL method is on banned list
    constexpr const std::size_t iov_len = 1U;
    for (auto i = 0U; i < vlen; ++i)
    {
        // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access, cppcoreguidelines-pro-bounds-pointer-arithmetic) iovec
        // is unchangable, It's (POSIX standard)
        // coverity[autosar_cpp14_m5_0_15_violation] : safe use of pointer arithmetic
        iovecs[i].iov_base = static_cast<void*>(&recv_bufs[i * msg_length]);
        // NOLINTEND(cppcoreguidelines-pro-type-union-access, cppcoreguidelines-pro-bounds-pointer-arithmetic)
        iovecs[i].iov_len = msg_length;
        msgs[i].msg_hdr.msg_iov = &iovecs[i];
        msgs[i].msg_hdr.msg_iovlen = iov_len;
        msgs[i].msg_hdr.msg_name = &sock_from[i];
        msgs[i].msg_hdr.msg_namelen = sizeof(struct sockaddr_in);
    }

    const auto response_result = Socket::instance().recvmmsg(
        file_descriptor_, msgs.data(), static_cast<unsigned int>(vlen), Socket::MessageFlag::kNone, timeout);
    if (response_result.has_value() == true)
    {
        score::cpp::pmr::vector<std::tuple<ssize_t, score::os::Ipv4Address>> result;

        const auto n_msgs = response_result.value();
        for (auto i = 0U; i < n_msgs; ++i)
        {
            // Suppress "AUTOSAR C++14 M5-2-8" rule. The rule declares:
            // An object with integer type or pointer to void type shall not be converted to an object with pointer
            // type. Here, msgs contains Ipv4 addresses, msg_name is start address of sockaddr and safe to convert to
            // target.
            // coverity[autosar_cpp14_m5_2_8_violation]
            const auto* const sock_addr = static_cast<sockaddr_in*>(msgs[i].msg_hdr.msg_name);
            Ipv4Address sender_address = score::os::Ipv4Address::CreateFromUint32NetOrder(sock_addr->sin_addr.s_addr);
            result.emplace_back(msgs[i].msg_len, std::move(sender_address));
        }
        return result;
    }
    else
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
}

score::cpp::expected<ssize_t, score::os::Error> score::os::UdpSocket::TrySendTo(const Ipv4Address& recipient,
                                                                     const std::uint16_t port,
                                                                     const unsigned char* const buffer,
                                                                     const std::size_t length) noexcept
{
    auto recipient_sockaddr_in_expected = GetSockAddrInFromIpAndPort(recipient, port);
    if (!recipient_sockaddr_in_expected.has_value())  // LCOV_EXCL_BR_LINE: Justification below
    {
        // Manual analysis: According to the QNX documentation, ::inet_aton fails when given an invalid string.
        // However, this is impossible to test through unit tests because, in the case of an invalid string,
        // the score::os::Ipv4Address constructor sets the default address to `0.0.0.0`, preventing ::inet_aton from
        // failing.
        return score::cpp::make_unexpected(recipient_sockaddr_in_expected.error());  // LCOV_EXCL_LINE: Justification above
    }
    const sockaddr_in recipient_sockaddr_in = recipient_sockaddr_in_expected.value();

    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) see Note 1
    // In the following line we need a reinterpret_cast to convert from sockaddr_in to sockaddr.
    // sockaddr is what is expected by the bind call.
    // Even though reinterpret_cast should be avoided, in this case is how you are supposed to do it
    // with the posix API.
    // See https://pubs.opengroup.org/onlinepubs/009695399/basedefs/netinet/in.h.html
    // coverity[autosar_cpp14_a5_2_4_violation] : safe reinterpret_cast, see justification above
    const auto recipient_sockaddr = reinterpret_cast<const struct sockaddr*>(&recipient_sockaddr_in);
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast) see Note 1

    return Socket::instance().sendto(
        file_descriptor_, buffer, length, Socket::MessageFlag::kNone, recipient_sockaddr, sizeof(*recipient_sockaddr));
}

score::cpp::expected_blank<score::os::Error> score::os::UdpSocket::SetSocketOption(const std::int32_t level,
                                                                        const std::int32_t optname,
                                                                        const void* optval,
                                                                        const socklen_t optlen) noexcept
{
    return score::os::Socket::instance().setsockopt(this->file_descriptor_, level, optname, optval, optlen);
}
