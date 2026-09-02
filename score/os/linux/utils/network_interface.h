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
#ifndef LIB_OS_LINUX_UTILS_NETWORK_INTERFACE_H
#define LIB_OS_LINUX_UTILS_NETWORK_INTERFACE_H

#include <array>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <map>
#include <memory>
#include <set>
#include <vector>

extern "C" {
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <netdb.h>
}

namespace score
{
namespace os
{

using MacAddress = std::array<std::uint8_t, 6>;
using Ipv4Address = std::array<std::uint8_t, 4>;
constexpr auto MacSize = std::tuple_size<MacAddress>::value;
constexpr auto Ipv4Size = std::tuple_size<Ipv4Address>::value;

struct LinkStats
{
    uint64_t rx_packets; /* total packets received */
    uint64_t tx_packets; /* total packets transmitted */
    uint64_t rx_bytes;   /* total bytes received */
    uint64_t tx_bytes;   /* total bytes transmitted */
    uint64_t rx_errors;  /* bad packets received */
    uint64_t tx_errors;  /* packet transmit problems */
    uint64_t rx_dropped; /* no space in linux buffers */
    uint64_t tx_dropped; /* no space available in linux */
    uint64_t multicast;  /* multicast packets received */
    uint64_t collisions;

    /* detailed rx_errors: */
    uint64_t rx_length_errors;
    uint64_t rx_over_errors;   /* receiver ring buff overflow */
    uint64_t rx_crc_errors;    /* recved pkt with crc error */
    uint64_t rx_frame_errors;  /* recv'd frame alignment error */
    uint64_t rx_fifo_errors;   /* recv'r fifo overrun */
    uint64_t rx_missed_errors; /* receiver missed packet */

    /* detailed tx_errors */
    uint64_t tx_aborted_errors;
    uint64_t tx_carrier_errors;
    uint64_t tx_fifo_errors;
    uint64_t tx_heartbeat_errors;
    uint64_t tx_window_errors;

    /* for cslip etc */
    uint64_t rx_compressed;
    uint64_t tx_compressed;

    uint64_t rx_nohandler; /* dropped, no handler found */
};

enum class DeviceClass : size_t
{
    Bridge = 0uL,
    Loopback = 1uL,
    Sit = 2uL,
    Vlan = 3uL,
    Ethernet = 4uL,
    Unknown = 5uL
};

struct network_interface
{
    bool admin_state;
    bool plugged;
    MacAddress mac;
    Ipv4Address ipv4;
    size_t vlan_id;
    LinkStats stats;
    int if_index;
    DeviceClass dev_class;
    network_interface()
        : admin_state{false},
          plugged{false},
          mac{0},
          ipv4{0},
          vlan_id{0},
          stats{},
          if_index{0xFF},
          dev_class{DeviceClass::Unknown}
    {
    }
};

using network_settings = std::map<const std::string, network_interface>;
void pIfaddrsDeleter(ifaddrs* const addList);
using pIfaddrsList = std::unique_ptr<ifaddrs, decltype(&pIfaddrsDeleter)>;

pIfaddrsList make_pIfAddrsList() noexcept;

/*
 If the path does not exist or if_name is empty stat_sysfs still returns false
*/
bool stat_sysfs(const std::string& sysfs_path);

bool bIsInterfaceBridge(const std::string& if_name) noexcept;

bool bIsInterfacePhyDevice(const std::string& if_name) noexcept;

void vCopyInterfaceStatistics(LinkStats& stats, const ifaddrs* const ifa);

DeviceClass bFindIfaceClass(const sockaddr_ll* const sock_ll, const std::string& ifname);

bool bFillInterfaceInformation(network_settings& interfaces) noexcept;

bool filter_adminup_interfaces(const network_interface& netif);

bool filter_plugged_interfaces(const network_interface& netif);

bool filter_ethernet_interfaces(const network_interface& netif);

bool filter_vlan_interfaces(const network_interface& netif);

bool filter_bridges_interfaces(const network_interface& netif);

bool bFilterInterfaces(network_settings& net_set,
                       std::set<std::string>& filtered,
                       const std::vector<std::function<bool(const network_interface&)>>& filters);

}  // namespace os
}  // namespace score
#endif  // LIB_OS_LINUX_UTILS_NETWORK_INTERFACE_H
