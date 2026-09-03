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
#ifndef LIB_OS_LINUX_UTILS_NETWORK_INTERFACE_MOCK_H
#define LIB_OS_LINUX_UTILS_NETWORK_INTERFACE_MOCK_H

#include <gmock/gmock.h>

#include "score/os/linux/utils/network_interface.h"
#include <iomanip>

namespace score
{
namespace os
{

class LinuxNetworkInterfaceMock
{
  public:
    LinuxNetworkInterfaceMock();
    ~LinuxNetworkInterfaceMock() {};

    MOCK_METHOD(bool, bFillInterfaceInformation, (network_settings&));
    MOCK_METHOD(bool,
                bFilterInterfaces,
                (network_settings & net_set,
                 std::set<std::string>& filtered,
                 const std::vector<std::function<bool(const network_interface&)>>& filters));
    MOCK_METHOD(bool, bIsInterfaceBridge, (const std::string& ifname));
    MOCK_METHOD(bool, bIsInterfacePhyDevice, (const std::string& ifname));
};

} /* namespace os */
} /* namespace score */

#endif  // LIB_OS_LINUX_UTILS_NETWORK_INTERFACE_MOCK_H
