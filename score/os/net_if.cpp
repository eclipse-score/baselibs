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
#include "score/os/net_if.h"

#include <net/if.h>

namespace score
{
namespace os
{

namespace
{

class NetIfImpl final : public NetIf
{
    score::cpp::expected<std::uint32_t, Error> if_nametoindex(const std::string& ifname) const noexcept override
    {
        const std::uint32_t ret{::if_nametoindex(ifname.c_str())};
        if (ret == 0)
        {
            return score::cpp::make_unexpected(Error::createFromErrno());
        }
        return ret;
    }
};

}  // namespace

}  // namespace os
}  // namespace score

score::os::NetIf& score::os::NetIf::instance() noexcept
{
    static score::os::NetIfImpl instance;
    return select_instance(instance);
}
