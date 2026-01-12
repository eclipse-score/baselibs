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

#if defined(__QNXNTO__)
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_dl.h>
#else
#include <linux/if.h>
#endif
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

/* KW_SUPPRESS_START:AUTOSAR.CAST.REINTERPRET:Cast is used in respect to ifaddrs structure */
/* KW_SUPPRESS_START:MISRA.CAST.PTR.UNRELATED:Cast is used in respect to ifaddrs structure */

namespace score
{
namespace os
{

namespace
{
constexpr auto kLogContext = "osnu";
constexpr sa_family_t kAfInet = AF_INET;
}  // namespace

using InterfaceAddressList = std::unique_ptr<ifaddrs, void (*)(ifaddrs*)>;
using InterfaceAddressResult = score::cpp::expected<std::pair<ifaddrs*, InterfaceAddressList>, Error>;

static InterfaceAddressResult find_interface(const std::string& name, const sa_family_t family);

score::cpp::expected<std::uint32_t, score::os::Error> NetutilsImpl::get_ifcip_address(const std::string& ifc_name) const noexcept
{
    const auto ret = find_interface(ifc_name, kAfInet);
    if (!ret.has_value())
    {
        return score::cpp::make_unexpected(ret.error());
    }
    // Despite AUTOSAR A5-2-4 advising against reinterpret_cast, in this specific case, there is a clear
    // relationship between the types involved, and no memory safety violations occur.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
    // coverity[autosar_cpp14_a5_2_4_violation] : see justification above
    const std::uint32_t address = reinterpret_cast<sockaddr_in*>(ret.value().first->ifa_addr)->sin_addr.s_addr;
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    return address;
}

score::cpp::expected<std::uint32_t, score::os::Error> NetutilsImpl::get_net_mask(const std::string& ifc_name) const noexcept
{
    const auto ret = find_interface(ifc_name, kAfInet);
    if (!ret.has_value())
    {
        return score::cpp::make_unexpected(ret.error());
    }
    const auto ifa = ret.value().first;
    if (ifa->ifa_netmask == nullptr)
    {
        score::mw::log::LogError(kLogContext) << "Netutils - failed to get netmask";
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(0));
    }
    // Despite AUTOSAR A5-2-4 advising against reinterpret_cast, in this specific case, there is a clear
    // relationship between the types involved, and no memory safety violations occur.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
    // coverity[autosar_cpp14_a5_2_4_violation] : see justification above
    const std::uint32_t netmask = reinterpret_cast<sockaddr_in*>(ifa->ifa_netmask)->sin_addr.s_addr;
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    return netmask;
}

score::cpp::expected<Ipv4Address, score::os::Error> NetutilsImpl::get_ifc_ip_address_net_mask(
    const std::string& ifc_name) const noexcept
{
    const auto ret = find_interface(ifc_name, kAfInet);
    if (!ret.has_value())
    {
        return score::cpp::make_unexpected(ret.error());
    }
    const auto ifa = ret.value().first;
    // Despite AUTOSAR A5-2-4 advising against reinterpret_cast, in this specific case, there is a clear
    // relationship between the types involved, and no memory safety violations occur.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
    // coverity[autosar_cpp14_a5_2_4_violation] : see justification above
    const std::uint32_t address = reinterpret_cast<sockaddr_in*>(ifa->ifa_addr)->sin_addr.s_addr;
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    if (ifa->ifa_netmask == nullptr)
    {
        score::mw::log::LogError(kLogContext) << "Netutils - failed to get netmask";
        return Ipv4Address::CreateFromUint32NetOrder(address);
    }
    // Despite AUTOSAR A5-2-4 advising against reinterpret_cast, in this specific case, there is a clear
    // relationship between the types involved, and no memory safety violations occur.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
    // coverity[autosar_cpp14_a5_2_4_violation] : see justification above
    const std::uint32_t netmask = reinterpret_cast<sockaddr_in*>(ifa->ifa_netmask)->sin_addr.s_addr;
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    return Ipv4Address::CreateFromUint32NetOrder(address, Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(netmask));
}

score::cpp::expected_blank<score::os::Error> NetutilsImpl::set_ip_address(const std::string& ifc_name,
                                                                 const Ipv4Address& ip_addr) const noexcept
{
    const auto sockfd = Socket::instance().socket(Socket::Domain::kIPv4, SOCK_DGRAM, 0);
    if (!sockfd.has_value())
    {
        score::mw::log::LogError(kLogContext) << __func__ << "socket() err:" << sockfd.error().ToString();
        return score::cpp::make_unexpected(sockfd.error());
    }

    ifreq ifr{};
    // Safely copy interface name using string::copy - handles truncation automatically
    // Note: ifr is zero-initialized, so no explicit null termination needed
    ifc_name.copy(static_cast<char*>(ifr.ifr_name), IFNAMSIZ - 1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    inet_aton(ip_addr.ToString().c_str(), &addr.sin_addr);
    // Suppress "AUTOSAR C++14 A12-0-2" The rule states: "Bitwise operations and operations that assume data
    // representation in memory shall not be performed on objects." Serialization and deserialization operations involve
    // copying raw memory using std::memcpy. The object being deserialized TriviallyCopyable, and thus, safe to copy
    // with memcpy. This low-level operation is necessary for performance optimization.
    // NOLINTBEGIN(score-banned-function): std::memcpy() needed for serialization, TriviallyCopyable-ness checked.
    // coverity[autosar_cpp14_a12_0_2_violation]
    std::memcpy(&ifr.ifr_addr, &addr, sizeof(struct sockaddr));
    // NOLINTEND(score-banned-function)

    auto res = score::os::Ioctl::instance().ioctl(sockfd.value(), SIOCSIFADDR, &ifr);
    // Suppressed here because usage of banned OSAL method in OSAL modules is allowed."
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const auto close_ret = score::os::Unistd::instance().close(sockfd.value());
    if (!res.has_value())
    {
        score::mw::log::LogError(kLogContext) << __func__ << "ioctl(SIOCSIFADDR) err:" << res.error().ToString();
        return score::cpp::make_unexpected(res.error());
    }
    if (!close_ret.has_value())
    {
        score::mw::log::LogError(kLogContext) << __func__ << "close() err:" << close_ret.error().ToString();
    }

    return score::cpp::blank();
}

InterfaceAddressResult find_interface(const std::string& name, const sa_family_t family)
{
    const auto ret = Ifaddrs::instance().getifaddrs();
    if (!ret.has_value())
    {
        score::mw::log::LogError(kLogContext) << "Netutils - getifaddrs failed";
        return score::cpp::make_unexpected(ret.error());
    }
    const auto ifa_list = ret.value();
    /* KW_SUPPRESS_START:MISRA.FOR.COND.EQ:ifaddrs list iteration */
    /* KW_SUPPRESS_START:MISRA.FOR.INCR:ifaddrs list iteration */
    for (auto ifa = ifa_list; ifa != nullptr; ifa = ifa->ifa_next)
    /* KW_SUPPRESS_END:MISRA.FOR.COND.EQ:ifaddrs list iteration */
    /* KW_SUPPRESS_END:MISRA.FOR.INCR:ifaddrs list iteration */
    {
        if ((ifa->ifa_addr != nullptr) && (ifa->ifa_addr->sa_family == family) && (std::string(ifa->ifa_name) == name))
        {
            return std::make_pair(ifa, InterfaceAddressList{ifa_list, [](ifaddrs* const ifa_list_) {
                                                                Ifaddrs::instance().freeifaddrs(ifa_list_);
                                                            }});
        }
    }
    if (ifa_list != nullptr)
    {
        Ifaddrs::instance().freeifaddrs(ifa_list);
    }
    score::mw::log::LogError(kLogContext) << "Netutils - interface not found";
    return score::cpp::make_unexpected(Error::createFromErrno(0));
}

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET:Cast is used in respect to ifaddrs structure */
/* KW_SUPPRESS_END:MISRA.CAST.PTR.UNRELATED:Cast is used in respect to ifaddrs structure */
