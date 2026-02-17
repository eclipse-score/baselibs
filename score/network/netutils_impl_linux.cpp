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

#include <ifaddrs.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
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
static constexpr char kLogContext[]{"osnu"};
}

score::cpp::expected<score::os::MacAddress, score::os::Error> NetutilsImpl::get_mac_address(
    const std::string& ifc_name) const noexcept
{
    const auto ret = score::os::Socket::instance().socket(score::os::Socket::Domain::kIPv4, SOCK_STREAM, 0);
    if (!ret.has_value())
    {
        score::mw::log::LogError(kLogContext) << __func__ << "failed to create socket";
        return score::cpp::make_unexpected(ret.error());
    }
    const std::int32_t fd = ret.value();

    ifreq ifr = {};
    ifc_name.copy(ifr.ifr_name, IFNAMSIZ);

    const auto ioctl_ret = score::os::Ioctl::instance().ioctl(fd, SIOCGIFHWADDR, &ifr);
    const auto close_ret = score::os::Unistd::instance().close(fd);
    if (!close_ret.has_value())
    {
        score::mw::log::LogError(kLogContext) << __func__ << "failed to close the socket";
        return score::cpp::make_unexpected(close_ret.error());
    }
    if (!ioctl_ret.has_value())
    {
        score::mw::log::LogError(kLogContext) << __func__ << "failed to ioctl SIOCGIFADDR";
        return score::cpp::make_unexpected(ioctl_ret.error());
    }
    return score::os::MacAddress{static_cast<std::uint8_t>(ifr.ifr_addr.sa_data[0]),
                               static_cast<std::uint8_t>(ifr.ifr_addr.sa_data[1]),
                               static_cast<std::uint8_t>(ifr.ifr_addr.sa_data[2]),
                               static_cast<std::uint8_t>(ifr.ifr_addr.sa_data[3]),
                               static_cast<std::uint8_t>(ifr.ifr_addr.sa_data[4]),
                               static_cast<std::uint8_t>(ifr.ifr_addr.sa_data[5])};
}

score::cpp::optional<std::uint32_t> NetutilsImpl::get_default_gateway_ip4() const noexcept
{
    constexpr std::uint16_t NETLINK_BUFFER_SIZE = 8192U;

    score::cpp::optional<std::uint32_t> gatewayAddr;
    std::uint8_t buf[NETLINK_BUFFER_SIZE]{};  // should be sufficient, but if this is not enough, function will fail
                                              // gracefully below.

    nlmsghdr netlink_message = {};
    netlink_message.nlmsg_len = NLMSG_LENGTH(sizeof(netlink_message));
    netlink_message.nlmsg_type = RTM_GETROUTE;
    netlink_message.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    netlink_message.nlmsg_pid = score::os::Unistd::instance().getpid();

    const auto ret = score::os::Socket::instance().socket(score::os::Socket::Domain::kNetlink, SOCK_RAW, NETLINK_ROUTE);
    const std::int32_t fd = ret.has_value() ? ret.value() : -1;
    if (fd < 0)
    {
        score::mw::log::LogError(kLogContext)
            << __func__ << "failed to create netlink socket Err: " << ret.error().ToString();
        return gatewayAddr;
    }

    const score::os::Socket::MessageFlag flags{};
    std::memcpy(buf, &netlink_message, sizeof(netlink_message));

    const auto sendRet = score::os::Socket::instance().send(fd, buf, netlink_message.nlmsg_len, flags);
    if (!sendRet.has_value())
    {
        score::mw::log::LogError(kLogContext)
            << __func__ << "failed to send on netlink socket Err: " << sendRet.error().ToString();
        score::os::Unistd::instance().close(fd);
        return gatewayAddr;
    }

    auto received_bytes = score::os::Socket::instance().recv(fd, buf, sizeof(buf), flags);

    if (!received_bytes.has_value())
    {
        score::mw::log::LogError(kLogContext)
            << __func__ << "failed to receive on netlink socket Err: " << received_bytes.error().ToString();
        score::os::Unistd::instance().close(fd);
        return gatewayAddr;
    }

    const auto closeRet = score::os::Unistd::instance().close(fd);
    if (!closeRet.has_value())
    {
        score::mw::log::LogError(kLogContext)
            << __func__ << "failed to close netlink socket Err: " << closeRet.error().ToString();
        return gatewayAddr;
    }

    std::uint8_t* pNlByte = buf;
    std::memset(&netlink_message, 0, sizeof(netlink_message));
    std::memcpy(&netlink_message, pNlByte, sizeof(netlink_message));

    if ((NLMSG_OK(reinterpret_cast<nlmsghdr*>(pNlByte), received_bytes.value()) == 0) ||
        netlink_message.nlmsg_type == NLMSG_ERROR)
    {
        score::mw::log::LogError(kLogContext) << __func__ << "received malformed netlink message";
        return gatewayAddr;
    }

    std::int32_t ipAddr = 0;
    // Scan for gateway attribute is performed here
    for (; NLMSG_OK(reinterpret_cast<nlmsghdr*>(pNlByte), received_bytes.value());
         pNlByte = reinterpret_cast<uint8_t*>(NLMSG_NEXT(reinterpret_cast<nlmsghdr*>(pNlByte), received_bytes.value())))
    {
        std::uint8_t* pRtAttrByte =
            reinterpret_cast<std::uint8_t*>(RTM_RTA(NLMSG_DATA(reinterpret_cast<nlmsghdr*>(pNlByte))));
        int rt_attribute_len = RTM_PAYLOAD(reinterpret_cast<nlmsghdr*>(pNlByte));

        for (; RTA_OK(reinterpret_cast<rtattr*>(pRtAttrByte), rt_attribute_len);
             pRtAttrByte =
                 reinterpret_cast<std::uint8_t*>(RTA_NEXT(reinterpret_cast<rtattr*>(pRtAttrByte), rt_attribute_len)))
        {
            rtattr rtAttr = {};
            std::memcpy(&rtAttr, pRtAttrByte, sizeof(rtAttr));

            if (rtAttr.rta_type == RTA_GATEWAY)
            {
                std::memcpy(&ipAddr, RTA_DATA(pRtAttrByte), sizeof(ipAddr));
                gatewayAddr = ipAddr;
                return gatewayAddr = ipAddr;
            }
        }
    }
    // No Error and no gateway found
    gatewayAddr = ipAddr;
    return gatewayAddr;
}

score::cpp::expected_blank<score::os::Error> NetutilsImpl::set_alias_ip_address(const std::string&,
                                                                       const Ipv4Address&) const noexcept
{
    return score::cpp::make_unexpected(Error::createFromErrno(0));
}

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET:Cast is used in respect to ifaddrs structure */
/* KW_SUPPRESS_END:MISRA.CAST.PTR.UNRELATED:Cast is used in respect to ifaddrs structure */
