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
#include "score/os/linux/ifaddrs.h"
#include "score/os/stat.h"

#include "network_interface.h"

#include "score/utility.hpp"
#include <algorithm>
#include <cerrno>
#include <iostream>
#include <set>
#include <tuple>

extern "C" {
#include <linux/if_arp.h>
#include <net/ethernet.h>
#include <sys/socket.h>
}

namespace score
{
namespace os
{

void pIfaddrsDeleter(ifaddrs* const addList)
{
    if (addList == nullptr)
    {
        return;
    }
    ::score::os::freeifaddrs(addList);
}

pIfaddrsList make_pIfAddrsList() noexcept
{
    ifaddrs* addr_list;
    if (::score::os::getifaddrs(&addr_list) == -1)
    {
        addr_list = nullptr;
    }
    return pIfaddrsList{addr_list, &pIfaddrsDeleter};
}

bool stat_sysfs(const std::string& sysfs_path)
{
    std::string path{"/sys/class/net/"};
    path += sysfs_path;
    struct ::score::os::StatBuffer ignored;
    return (::score::os::Stat::instance().stat(path.c_str(), ignored).has_value());
}

/*
 If the path does not exist or if_name is empty stat_sysfs still returns false
*/
bool bIsInterfaceBridge(const std::string& if_name) noexcept
{
    return stat_sysfs(if_name + "/bridge");
}

bool bIsInterfacePhyDevice(const std::string& if_name) noexcept
{
    return stat_sysfs(if_name + "/device");
}
/*
 * Even when both structs are at this point equal
 * copying member by member ensures that changes in the order and
 * type will be adjusted which is not true for memcpy
 * */

void vCopyInterfaceStatistics(LinkStats& stats, const ifaddrs* const ifa)
{
    if ((ifa == nullptr) || (ifa->ifa_data == nullptr))
    {
        return;
    }
    const rtnl_link_stats* const pStats = static_cast<rtnl_link_stats*>(ifa->ifa_data);
    stats.rx_packets = pStats->rx_packets; /* total packets received */
    stats.tx_packets = pStats->tx_packets; /* total packets transmitted */
    stats.rx_bytes = pStats->rx_bytes;     /* total bytes received */
    stats.tx_bytes = pStats->tx_bytes;     /* total bytes transmitted */
    stats.rx_errors = pStats->rx_errors;   /* bad packets received */
    stats.tx_errors = pStats->tx_errors;   /* packet transmit problems */
    stats.rx_dropped = pStats->rx_dropped; /* no space in linux buffers */
    stats.tx_dropped = pStats->tx_dropped; /* no space available in linux */
    stats.multicast = pStats->multicast;   /* multicast packets received */
    stats.collisions = pStats->collisions;
    /* detailed rx_errors: */
    stats.rx_length_errors = pStats->rx_length_errors;
    stats.rx_over_errors = pStats->rx_over_errors;     /* receiver ring buff overflow */
    stats.rx_crc_errors = pStats->rx_crc_errors;       /* recved pkt with crc error */
    stats.rx_frame_errors = pStats->rx_frame_errors;   /* recv'd frame alignment error */
    stats.rx_fifo_errors = pStats->rx_fifo_errors;     /* recv'r fifo overrun */
    stats.rx_missed_errors = pStats->rx_missed_errors; /* receiver missed packet */
    /* detailed tx_errors */
    stats.tx_aborted_errors = pStats->tx_aborted_errors;
    stats.tx_carrier_errors = pStats->tx_carrier_errors;
    stats.tx_fifo_errors = pStats->tx_fifo_errors;
    stats.tx_heartbeat_errors = pStats->tx_heartbeat_errors;
    stats.tx_window_errors = pStats->tx_window_errors;
    /* for cslip etc */
    stats.rx_compressed = pStats->rx_compressed;
    stats.tx_compressed = pStats->tx_compressed;
    stats.rx_nohandler = pStats->rx_nohandler; /* dropped, no handler found */
}

DeviceClass bFindIfaceClass(const sockaddr_ll* const sock_ll, const std::string& ifname)
{
    if (sock_ll == nullptr)
    {
        return DeviceClass::Unknown;
    }

    switch (sock_ll->sll_hatype)
    {
        case static_cast<uint16_t>(ARPHRD_LOOPBACK):
            return DeviceClass::Loopback;
        case static_cast<uint16_t>(ARPHRD_SIT):
            return DeviceClass::Sit;
        case static_cast<uint16_t>(ARPHRD_ETHER):
            // vlans and bridges still report as ethernet
            if (bIsInterfaceBridge(ifname.c_str()))
            {
                return DeviceClass::Bridge;
            }
            else if (bIsInterfacePhyDevice(ifname.c_str()))
            {
                return DeviceClass::Ethernet;
            }
            else
            {
                return DeviceClass::Vlan;
            }
        default:
            return DeviceClass::Unknown;
    }
}

bool bFillInterfaceInformation(network_settings& interfaces) noexcept
{
    bool status{false};
    const auto ifaddress = make_pIfAddrsList();

    if (ifaddress != nullptr)
    {
        const ifaddrs* ifa = ifaddress.get();
        while (ifa != nullptr)
        {
            if (ifa->ifa_addr == nullptr)
            {
                ifa = ifa->ifa_next;
                continue;
            }
            interfaces.emplace(std::piecewise_construct, std::forward_as_tuple(ifa->ifa_name), std::forward_as_tuple());

            auto& current_iface = interfaces[ifa->ifa_name];
            const auto family = ifa->ifa_addr->sa_family;
            // fetch information from the FLAGS
            current_iface.admin_state = static_cast<bool>(ifa->ifa_flags & IFF_UP);
            current_iface.plugged = static_cast<bool>(ifa->ifa_flags & IFF_RUNNING);

            switch (family)
            {
                case static_cast<uint16_t>(AF_INET):
                    // copy IPv4 address
                    score::cpp::ignore = std::copy(
                        ifa->ifa_addr->sa_data, ifa->ifa_addr->sa_data + Ipv4Size, current_iface.ipv4.begin());
                    break;
                case static_cast<uint16_t>(AF_PACKET):
                {
                    // fill statistics
                    vCopyInterfaceStatistics(current_iface.stats, ifa);
                    // get mac address
                    const struct sockaddr_ll* const link_layer = reinterpret_cast<struct sockaddr_ll*>(ifa->ifa_addr);
                    score::cpp::ignore =
                        std::copy(link_layer->sll_addr, link_layer->sll_addr + MacSize, current_iface.mac.begin());
                    // get interface index
                    current_iface.if_index = link_layer->sll_ifindex;
                    // find the interface class
                    current_iface.dev_class = bFindIfaceClass(link_layer, ifa->ifa_name);
                }
                break;
                default:
                    // Not using IPV6 data
                    break;
            }
            ifa = ifa->ifa_next;
        }
        status = true;
    }
    return status;
}

bool filter_adminup_interfaces(const network_interface& netif)
{
    return (netif.admin_state == 1);
}

bool filter_plugged_interfaces(const network_interface& netif)
{
    return (netif.plugged == 1);
}

bool filter_ethernet_interfaces(const network_interface& netif)
{
    return (netif.dev_class == DeviceClass::Ethernet);
}

bool filter_vlan_interfaces(const network_interface& netif)
{
    return (netif.dev_class == DeviceClass::Vlan);
}
bool filter_bridges_interfaces(const network_interface& netif)
{
    return (netif.dev_class == DeviceClass::Bridge);
}

/* Return a list of references to interfaces which are of interest */
bool bFilterInterfaces(network_settings& net_set,
                       std::set<std::string>& filtered,
                       const std::vector<std::function<bool(const network_interface&)>>& filters)
{
    if (filtered.empty())
    {
        /* Start with all the interfaces as part of the set */
        for (const auto& if_element : net_set)
        {
            score::cpp::ignore = filtered.emplace(if_element.first);
        }
    }

    for (const auto& filter : filters)
    {
        auto iter_set = filtered.begin();
        while (iter_set != filtered.end())
        {
            const auto netif = net_set.find(*iter_set);
            if (netif == net_set.end())
            {
                return false;
            }
            if (!filter(netif->second))
            {
                iter_set = filtered.erase(iter_set);
            }
            else
            {
                ++iter_set;
            }
        }
    }

    return true;
}
}  // namespace os
}  // namespace score
