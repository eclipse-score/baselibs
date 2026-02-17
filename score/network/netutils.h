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
#ifndef SCORE_LIB_OS_UTILS_NETUTILS_H
#define SCORE_LIB_OS_UTILS_NETUTILS_H

#include "score/network/ipv4_address.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include <score/expected.hpp>
#include <score/optional.hpp>
#include <array>
#include <cstdint>
#include <string>

namespace score
{
namespace os
{

constexpr std::uint8_t MacAddressLen{6U};
using MacAddress = std::array<std::uint8_t, MacAddressLen>;

class Netutils : public ObjectSeam<Netutils>
{
  public:
    static Netutils& instance() noexcept;

    virtual score::cpp::expected<std::uint32_t, score::os::Error> get_net_mask(const std::string& ifc_name) const noexcept = 0;
    virtual score::cpp::expected<std::uint32_t, score::os::Error> get_ifcip_address(
        const std::string& ifc_name) const noexcept = 0;
    virtual score::cpp::expected<Ipv4Address, score::os::Error> get_ifc_ip_address_net_mask(
        const std::string& ifc_name) const noexcept = 0;
    virtual score::cpp::expected<MacAddress, score::os::Error> get_mac_address(const std::string& ifc_name) const noexcept = 0;
    virtual score::cpp::optional<std::uint32_t> get_default_gateway_ip4() const noexcept = 0;

    virtual score::cpp::expected_blank<score::os::Error> set_ip_address(const std::string& ifc_name,
                                                               const Ipv4Address& ip_addr) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> set_alias_ip_address(const std::string& ifc_name,
                                                                     const Ipv4Address& ip_addr) const noexcept = 0;

    virtual ~Netutils() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_NETUTILS_H
