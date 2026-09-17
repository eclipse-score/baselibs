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
#include "score/os/mocklib/ifaddrs_mock.h"

#include <netinet/in.h>

namespace score
{
namespace os
{
IfaddrsMock::IfaddrsMock() : ifaddrs_list_(nullptr)
{
    ON_CALL(*this, getifaddrs()).WillByDefault(testing::Invoke(this, &IfaddrsMock::GetIfaddrsList));
}

IfaddrsMock::~IfaddrsMock()
{
    while (ifaddrs_list_ != nullptr)
    {
        const auto ifa_next = ifaddrs_list_->ifa_next;
        delete[] ifaddrs_list_->ifa_name;
        delete ifaddrs_list_->ifa_addr;
        delete ifaddrs_list_->ifa_netmask;
        delete ifaddrs_list_;
        ifaddrs_list_ = ifa_next;
    }
}

void IfaddrsMock::AddIfaddrsListEntry(const std::string& name,
                                      std::uint32_t address,
                                      bool should_add_netmask,
                                      std::uint32_t netmask,
                                      std::uint16_t family_addr)
{
    auto new_ifa = new ifaddrs{};
    new_ifa->ifa_name = new char[name.length() + 1U]{};
    name.copy(new_ifa->ifa_name, std::string::npos);

    sockaddr_in sockaddr_in_aux{};
    sockaddr_in_aux.sin_family = family_addr;
    sockaddr_in_aux.sin_addr = in_addr{address};
    sockaddr_in_aux.sin_port = 0;

    new_ifa->ifa_addr = reinterpret_cast<sockaddr*>(new sockaddr_in{sockaddr_in_aux});

    if (should_add_netmask)
    {
        sockaddr_in_aux.sin_addr = in_addr{netmask};
        new_ifa->ifa_netmask = reinterpret_cast<sockaddr*>(new sockaddr_in{sockaddr_in_aux});
    }
    if (ifaddrs_list_ == nullptr)
    {
        ifaddrs_list_ = new_ifa;
    }
    else
    {
        auto ifa = ifaddrs_list_;
        while (ifa->ifa_next != nullptr)
        {
            ifa = ifa->ifa_next;
        }
        ifa->ifa_next = new_ifa;
    }
}

score::cpp::expected<ifaddrs*, Error> IfaddrsMock::GetIfaddrsList() const noexcept
{
    return ifaddrs_list_;
}
}  // namespace os

}  // namespace score
