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
#include "capability.h"

#ifdef __linux__
#include "score/os/linux/capability.h"
#else
#include "score/os/qnx/capability.h"
#endif

#include "score/mw/log/logging.h"
#include "score/os/errno_logging.h"
#include <utility>

namespace score
{

namespace os
{

std::vector<Capability> ProcessCapabilities::DetermineCapabilitiesToDrop(
    const std::vector<Capability>& capabilities_to_keep)
{
    const std::vector<Capability> all_capabilities = {
        Capability::SysAdmin,      Capability::Kill,       Capability::Chown,          Capability::DacOverride,
        Capability::DacReadSearch, Capability::Fowner,     Capability::Fsetid,         Capability::Setgid,
        Capability::Setuid,        Capability::Setpcap,    Capability::LinuxImmutable, Capability::NetBindService,
        Capability::NetBroadCast,  Capability::NetAdmin,   Capability::NetRaw,         Capability::IpcLock,
        Capability::IpcOwner,      Capability::SysModule,  Capability::SysRawio,       Capability::SysChroot,
        Capability::SysPtrace,     Capability::SysPacct,   Capability::Sysboot,        Capability::SysNice,
        Capability::SysResource,   Capability::SysTime,    Capability::SysTtyConfig,   Capability::Mknod,
        Capability::Lease,         Capability::AuditWrite, Capability::AuditControl,   Capability::Setfcap,
        Capability::MacOverride,   Capability::MacAdmin,   Capability::Syslog,         Capability::WakeAlarm,
        Capability::BlockSuspend,  Capability::AuditRead};
    std::vector<Capability> capabilities_to_drop = {};

    for (const Capability capability : all_capabilities)
    {
        // if the capability is not in the list of capabilities to keep, add it to the list of capabilities to drop
        if (std::find(capabilities_to_keep.begin(), capabilities_to_keep.end(), capability) ==
            capabilities_to_keep.end())
        {
            capabilities_to_drop.push_back(capability);
        }
    }

    return capabilities_to_drop;
}

score::cpp::expected_blank<Error> ProcessCapabilities::DropUnwantedCapabilities(
    const std::vector<Capability>& capabilities_to_drop)
{
    if (capabilities_to_drop.empty() == true)
    {
        return {};
    }

    for (const Capability capability : capabilities_to_drop)
    {
        // drop capabilities from the process bounding set
        const score::cpp::expected_blank<Error> drop_bound_result = this->DropCapabilityFromBoundingSet(capability);
        if (!drop_bound_result.has_value())
        {
            mw::log::LogError() << "Failed to drop capability from bounding set " << drop_bound_result.error();
            return score::cpp::make_unexpected(drop_bound_result.error());
        }
    }

    score::cpp::expected<ProcessCapabilitySets, Error> get_process_capabilities_result =
        this->GetProcessCapabilitySets();
    if (!get_process_capabilities_result.has_value())
    {
        mw::log::LogError() << "Failed to get current process capability sets: "
                            << get_process_capabilities_result.error();
        return score::cpp::make_unexpected(get_process_capabilities_result.error());
    }

    ProcessCapabilitySets process_capabilities = std::move(get_process_capabilities_result).value();

    const score::cpp::expected_blank<Error> drop_from_permitted_set_result = this->SetCapabilitiesInCapabilitySet(
        process_capabilities, CapabilitySets::Permitted, capabilities_to_drop, false);
    if (!drop_from_permitted_set_result.has_value())
    {
        mw::log::LogError() << "Failed to drop capabilities from permitted set: "
                            << drop_from_permitted_set_result.error();
        return score::cpp::make_unexpected(drop_from_permitted_set_result.error());
    }

    const score::cpp::expected_blank<Error> drop_from_effective_set_result = this->SetCapabilitiesInCapabilitySet(
        process_capabilities, CapabilitySets::Effective, capabilities_to_drop, false);
    if (!drop_from_effective_set_result.has_value())
    {
        mw::log::LogError() << "Failed to drop capabilities from effective set: "
                            << drop_from_effective_set_result.error();
        return score::cpp::make_unexpected(drop_from_effective_set_result.error());
    }

    const score::cpp::expected_blank<Error> set_process_capability_set_result =
        ProcessCapabilities::instance().SetProcessCapabilitySets(process_capabilities);
    if (!set_process_capability_set_result.has_value())
    {
        mw::log::LogError() << "Failed to set process capability sets: " << set_process_capability_set_result.error();
        return score::cpp::make_unexpected(set_process_capability_set_result.error());
    }

    return {};
}

score::cpp::expected_blank<Error> ProcessCapabilities::ActivateRequiredCapabilities(
    const std::vector<Capability>& required_capabilities)
{
    if (required_capabilities.empty() == true)
    {
        return {};
    }

    score::cpp::expected<ProcessCapabilitySets, Error> get_process_capabilities_result =
        this->GetProcessCapabilitySets();
    if (!get_process_capabilities_result.has_value())
    {
        mw::log::LogError() << "Failed to get current process capability sets: "
                            << get_process_capabilities_result.error();
        return score::cpp::make_unexpected(get_process_capabilities_result.error());
    }

    ProcessCapabilitySets process_capabilities = std::move(get_process_capabilities_result).value();

    const score::cpp::expected_blank<Error> raise_in_effective_set_result = this->SetCapabilitiesInCapabilitySet(
        process_capabilities, CapabilitySets::Effective, required_capabilities, true);
    if (!raise_in_effective_set_result.has_value())
    {
        mw::log::LogError() << "Failed to raise needed capabilities into the effective set: "
                            << raise_in_effective_set_result.error();
        return score::cpp::make_unexpected(raise_in_effective_set_result.error());
    }

    const score::cpp::expected_blank<Error> set_process_capability_set_result =
        this->SetProcessCapabilitySets(process_capabilities);
    if (!set_process_capability_set_result.has_value())
    {
        mw::log::LogError() << "Failed to set process capability sets: " << set_process_capability_set_result.error();
        return score::cpp::make_unexpected(set_process_capability_set_result.error());
    }

    return {};
}

score::cpp::expected_blank<Error> ProcessCapabilities::ReduceProcessCapabilitiesTo(
    const std::vector<Capability>& capabilities) noexcept
{
    std::vector<Capability> capabilities_to_keep = capabilities;

    if (capabilities_to_keep.size() > 0)
    {
        // detect invalid values in the list of capabilities to keep
        for (const Capability capability : capabilities_to_keep)
        {
            if (capability == Capability::InvalidValue)
            {
                mw::log::LogError() << "Invalid value found in list of capabilities to keep";
                return score::cpp::make_unexpected(Error::createFromErrno(EINVAL));
            }
        }

        // remove duplicates from the list of capabilities to keep
        for (std::int64_t i = static_cast<std::int64_t>(capabilities_to_keep.size() - 1); i >= 0; --i)
        {
            for (std::int64_t j = static_cast<std::int64_t>(capabilities_to_keep.size() - 1); j >= 0; --j)
            {
                const Capability ith_capability = capabilities_to_keep[static_cast<std::size_t>(i)];
                const Capability jth_capability = capabilities_to_keep[static_cast<std::size_t>(j)];
                if (i != j && ith_capability == jth_capability)
                {
                    // remove index j and update the index of i such that we dont skip any entries in the list
                    capabilities_to_keep.erase(capabilities_to_keep.begin() + j);
                    if (j < i)
                    {
                        i--;
                    }
                    mw::log::LogWarn() << "Duplicate entry found in list of capabilities to keep";
                }
            }
        }
    }

    const std::vector<Capability> capabilities_to_drop = this->DetermineCapabilitiesToDrop(capabilities_to_keep);
    score::cpp::expected_blank<Error> result = this->DropUnwantedCapabilities(capabilities_to_drop);
    if (!result.has_value())
    {
        mw::log::LogError() << "Failed to drop unwanted capabilities " << result.error();
        return result;
    }

    result = this->ActivateRequiredCapabilities(capabilities_to_keep);
    if (!result.has_value())
    {
        mw::log::LogError() << "Failed to activate capabilities needed by process " << result.error();
        return result;
    }

    return {};
}

ProcessCapabilities& ProcessCapabilities::instance() noexcept
{
#ifdef __linux__
    static LinuxProcessCapabilitiesImpl instance;
#else
    static QNXProcessCapabilitiesImpl instance;
#endif
    return select_instance(instance);
}

}  // namespace os

}  // namespace score
