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
#include "score/network/vlan.h"

#include "score/os/socket.h"
#include "score/os/version.h"

namespace score
{
namespace os
{

namespace
{

#if defined(SPP_OS_QNX_VERSION) && SPP_OS_QNX_VERSION
#if SPP_OS_VERSION_GTE(SPP_OS_QNX_VERSION, 8, 0)
constexpr auto kVlanPrioOption = -1;  // SO_VLANPRIO is not available in QNX 8.0
#else
constexpr auto kVlanPrioOption = SO_VLANPRIO;
#endif
#else  // defined(SPP_OS_QNX_VERSION) && SPP_OS_QNX_VERSION
constexpr auto kVlanPrioOption = SO_PRIORITY;
#endif

class VlanImpl final : public Vlan
{
    score::cpp::expected_blank<Error> SetVlanPriorityOfSocket(const std::uint8_t pcp_priority,
                                                       const std::int32_t file_descriptor) noexcept override
    {
        return score::os::Socket::instance().setsockopt(
            file_descriptor, SOL_SOCKET, kVlanPrioOption, &pcp_priority, sizeof(pcp_priority));
    }
};

}  // namespace

Vlan& Vlan::instance() noexcept
{
    static VlanImpl instance;
    return select_instance(instance);
}

}  // namespace os
}  // namespace score
