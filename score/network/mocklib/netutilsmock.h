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
#ifndef SCORE_LIB_OS_UTILS_MOCKLIB_NETUTILSMOCK_H
#define SCORE_LIB_OS_UTILS_MOCKLIB_NETUTILSMOCK_H

#include "score/network/netutils.h"
#include "score/os/errno.h"

#include <gmock/gmock.h>
#include <string>

namespace score
{
namespace os
{

class NetutilsMock : public Netutils
{
  public:
    MOCK_METHOD((score::cpp::expected<std::uint32_t, Error>), get_net_mask, (const std::string&), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::uint32_t, Error>),
                get_ifcip_address,
                (const std::string&),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<Ipv4Address, Error>),
                get_ifc_ip_address_net_mask,
                (const std::string&),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<MacAddress, Error>), get_mac_address, (const std::string&), (const, noexcept, override));
    MOCK_METHOD((score::cpp::optional<std::uint32_t>), get_default_gateway_ip4, (), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                set_ip_address,
                (const std::string& ifc_name, const Ipv4Address& ip_addr),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                set_alias_ip_address,
                (const std::string& ifc_name, const Ipv4Address& ip_addr),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_MOCKLIB_NETUTILSMOCK_H
