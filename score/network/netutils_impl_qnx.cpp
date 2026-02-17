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
#include "score/network/netutils_impl.h"

#include "score/os/ifaddrs.h"
#include "score/os/ioctl.h"
#include "score/os/socket.h"
#include "score/os/unistd.h"
#include "score/mw/log/logging.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <sys/ioctl.h>

/* KW_SUPPRESS_START:AUTOSAR.CAST.REINTERPRET:Cast is used in respect to ifaddrs structure */
/* KW_SUPPRESS_START:MISRA.CAST.PTR.UNRELATED:Cast is used in respect to ifaddrs structure */

namespace score
{
namespace os
{

namespace
{
static constexpr auto kLogContext = "osnu";
}

score::cpp::expected<score::os::MacAddress, score::os::Error> NetutilsImpl::get_mac_address(
    const std::string& ifc_name) const noexcept
{
    auto ifaphead = score::os::Ifaddrs::instance().getifaddrs();
    if (!ifaphead.has_value())
    {
        return score::cpp::make_unexpected(ifaphead.error());
    }

    const auto ifa_list = ifaphead.value();

    for (auto ifap = ifa_list; ifap != nullptr; ifap = ifap->ifa_next)
    {
        if ((nullptr != ifap->ifa_addr) && (AF_LINK == ifap->ifa_addr->sa_family) &&
            (ifc_name == std::string(ifap->ifa_name)))
        {
            // Despite AUTOSAR A5-2-4 advising against reinterpret_cast, in this specific case, there is a clear
            // relationship between the types involved, and no memory safety violations occur.
            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
            // coverity[autosar_cpp14_a5_2_4_violation] : see justification above
            const auto sdl = reinterpret_cast<struct sockaddr_dl*>(ifap->ifa_addr);
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
            score::os::MacAddress if_mac{};
            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic): safe use of pointer arithmetic
            // coverity[autosar_cpp14_m5_0_15_violation] : safe use of pointer arithmetic
            std::copy(LLADDR(sdl), LLADDR(sdl) + sdl->sdl_alen, std::begin(if_mac));
            // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            score::os::Ifaddrs::instance().freeifaddrs(ifa_list);
            return score::os::MacAddress{static_cast<std::uint8_t>(if_mac[0]),
                                       static_cast<std::uint8_t>(if_mac[1]),
                                       static_cast<std::uint8_t>(if_mac[2]),
                                       static_cast<std::uint8_t>(if_mac[3]),
                                       static_cast<std::uint8_t>(if_mac[4]),
                                       static_cast<std::uint8_t>(if_mac[5])};
        }
    }

    if (nullptr != ifa_list)
    {
        score::os::Ifaddrs::instance().freeifaddrs(ifa_list);
    }

    return score::cpp::make_unexpected(score::os::Error::createFromErrno());
}

score::cpp::optional<std::uint32_t> NetutilsImpl::get_default_gateway_ip4() const noexcept
{
    score::cpp::optional<std::uint32_t> gateway_addr;
    constexpr std::uint16_t SEQ = 1234U;

    struct routing_table
    {
        struct rt_msghdr header;
        struct sockaddr_in dst;
        struct sockaddr_in gw;
        struct sockaddr_in mask;
    };

    const pid_t pid = score::os::Unistd::instance().getpid();
    routing_table cur_rt = {{
                                sizeof(struct rt_msghdr) + sizeof(struct sockaddr_in),  // rtm_msglen
                                RTM_VERSION,                                            // rtm_version
                                RTM_GET,                                                // rtm_type
                                0,                                                      // rtm_index
                                0,                                                      // zero
                                RTF_UP | RTF_GATEWAY,                                   // rtm_flags
                                RTA_DST | RTA_NETMASK,                                  // rtm_addrs
                                pid,                                                    // rtm_pid
                                SEQ,                                                    // rtm_seq
                                0,                                                      // rtm_errno
                                0,                                                      // rtm_use
                                0,                                                      // rtm_inits
                                {}                                                      // rmt_rmx
                            },
                            {
                                0,        // sin_len
                                AF_INET,  // sin_family
                                0,        // sin_port
                                {},       // sin_addr
                                {0}       // sin_zero[8]
                            },
                            {
                                0,        // sin_len
                                AF_INET,  // sin_family
                                0,        // sin_port
                                {},       // sin_addr
                                {0}       // sin_zero[8]
                            },
                            {
                                0,        // sin_len
                                AF_INET,  // sin_family
                                0,        // sin_port
                                {},       // sin_addr
                                {0}       // sin_zero[8]
                            }};

    const auto ret = score::os::Socket::instance().socket(score::os::Socket::Domain::kRoute, SOCK_RAW, 0);
    const std::int32_t socketfd = ret.has_value() ? ret.value() : -1;
    if (socketfd == -1)
    {
        score::mw::log::LogError(kLogContext) << static_cast<const char*>(__func__) << "error on socket";
        return gateway_addr;
    }

    score::mw::log::LogDebug(kLogContext) << static_cast<const char*>(__func__)
                                        << "rtm->rtm_pid : " << cur_rt.header.rtm_pid;
    // Suppressed here because usage of banned OSAL method in OSAL modules is allowed."
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const auto write_ret = score::os::Unistd::instance().write(socketfd, &cur_rt, cur_rt.header.rtm_msglen);
    if (!write_ret.has_value())
    {
        score::mw::log::LogError(kLogContext)
            << static_cast<const char*>(__func__) << "error on write : " << write_ret.error().ToString();
        return gateway_addr;
    }

    do
    {
        // Suppressed here because usage of banned OSAL method in OSAL modules is allowed."
        // NOLINTNEXTLINE(score-banned-function) see comment above
        const auto read_ret = score::os::Unistd::instance().read(socketfd, &cur_rt, sizeof(struct routing_table));
        if (!read_ret.has_value())
        {
            score::mw::log::LogError(kLogContext)
                << static_cast<const char*>(__func__) << "error on read : " << read_ret.error().ToString();
            return gateway_addr;
        }
    } while ((cur_rt.header.rtm_type != RTM_GET) || (cur_rt.header.rtm_seq != SEQ) || (cur_rt.header.rtm_pid != pid));

    if (0 != cur_rt.header.rtm_errno)
    {
        score::mw::log::LogError(kLogContext) << static_cast<const char*>(__func__) << "RTM_GET command failed.";
        return gateway_addr;
    }
    else
    {
        // Suppressed here because usage of banned OSAL method in OSAL modules is allowed."
        score::mw::log::LogError(kLogContext)
            // NOLINTNEXTLINE(score-banned-function) see comment above
            << static_cast<const char*>(__func__) << "gateway : " << inet_ntoa(cur_rt.gw.sin_addr);
        gateway_addr = static_cast<std::uint32_t>(cur_rt.gw.sin_addr.s_addr);
        return gateway_addr;
    }
}

score::cpp::expected_blank<score::os::Error> NetutilsImpl::set_alias_ip_address(const std::string& ifc_name,
                                                                       const Ipv4Address& ip_addr) const noexcept
{
    const auto sockfd = Socket::instance().socket(Socket::Domain::kIPv4, SOCK_DGRAM, 0);
    if (!sockfd.has_value())
    {
        score::mw::log::LogError(kLogContext) << __func__ << "socket() err:" << sockfd.error().ToString();
        return score::cpp::make_unexpected(sockfd.error());
    }

    ifaliasreq ifar{};
    // Safely copy interface name using string::copy - handles truncation automatically
    // Note: ifar is zero-initialized, so no explicit null termination needed
    ifc_name.copy(static_cast<char*>(ifar.ifra_name), IFNAMSIZ - 1);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    inet_aton(ip_addr.ToString().c_str(), &addr.sin_addr);
    // Suppress "AUTOSAR C++14 A12-0-2" The rule states: "Bitwise operations and operations that assume data
    // copying raw memory using std::memcpy. The object being deserialized TriviallyCopyable, and thus, safe to copy
    // with memcpy.
    // NOLINTBEGIN(score-banned-function): std::memcpy() needed for serialization, TriviallyCopyable-ness checked.
    // coverity[autosar_cpp14_a12_0_2_violation]
    std::memcpy(&ifar.ifra_addr, &addr, sizeof(struct sockaddr));
    // NOLINTEND(score-banned-function)

    const auto res = score::os::Ioctl::instance().ioctl(sockfd.value(), SIOCAIFADDR, &ifar);
    // Suppressed here because usage of banned OSAL method in OSAL modules is allowed."
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const auto close_ret = score::os::Unistd::instance().close(sockfd.value());
    if (!res.has_value())
    {
        score::mw::log::LogError(kLogContext) << __func__ << "ioctl(SIOCAIFADDR) err:" << res.error().ToString();
        return score::cpp::make_unexpected(res.error());
    }
    if (!close_ret.has_value())
    {
        score::mw::log::LogError(kLogContext) << __func__ << "close() err:" << close_ret.error().ToString();
    }

    return score::cpp::blank();
}

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET:Cast is used in respect to ifaddrs structure */
/* KW_SUPPRESS_END:MISRA.CAST.PTR.UNRELATED:Cast is used in respect to ifaddrs structure */
