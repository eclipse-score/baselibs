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
#include "./capability.h"

#include <score/utility.hpp>
#include <sys/prctl.h>
#include <errno.h>
#include <cassert>
#include <memory>
#include <unordered_map>

#include <sys/capability.h>

namespace score
{

namespace os
{

using LinuxProcessCapabilitySetsType = cap_t;

const std::unordered_map<CapabilitySets, cap_flag_t> linux_capability_sets_identifier_map = {
    {CapabilitySets::Permitted, CAP_PERMITTED},
    {CapabilitySets::Inherited, CAP_INHERITABLE},
    {CapabilitySets::Effective, CAP_EFFECTIVE}};

const std::unordered_map<Capability, cap_value_t> linux_capability_identifier_map = {
    {Capability::SysAdmin, CAP_SYS_ADMIN},
    {Capability::Kill, CAP_KILL},
    {Capability::Chown, CAP_CHOWN},
    {Capability::DacOverride, CAP_DAC_OVERRIDE},
    {Capability::DacReadSearch, CAP_DAC_READ_SEARCH},
    {Capability::Fowner, CAP_FOWNER},
    {Capability::Fsetid, CAP_FSETID},
    {Capability::Setgid, CAP_SETGID},
    {Capability::Setuid, CAP_SETUID},
    {Capability::Setpcap, CAP_SETPCAP},
    {Capability::LinuxImmutable, CAP_LINUX_IMMUTABLE},
    {Capability::NetBindService, CAP_NET_BIND_SERVICE},
    {Capability::NetBroadCast, CAP_NET_BROADCAST},
    {Capability::NetAdmin, CAP_NET_ADMIN},
    {Capability::NetRaw, CAP_NET_RAW},
    {Capability::IpcLock, CAP_IPC_LOCK},
    {Capability::IpcOwner, CAP_IPC_OWNER},
    {Capability::SysModule, CAP_SYS_MODULE},
    {Capability::SysRawio, CAP_SYS_RAWIO},
    {Capability::SysChroot, CAP_SYS_CHROOT},
    {Capability::SysPtrace, CAP_SYS_PTRACE},
    {Capability::SysPacct, CAP_SYS_PACCT},
    {Capability::Sysboot, CAP_SYS_BOOT},
    {Capability::SysNice, CAP_SYS_NICE},
    {Capability::SysResource, CAP_SYS_RESOURCE},
    {Capability::SysTime, CAP_SYS_TIME},
    {Capability::SysTtyConfig, CAP_SYS_TTY_CONFIG},
    {Capability::Mknod, CAP_MKNOD},
    {Capability::Lease, CAP_LEASE},
    {Capability::AuditWrite, CAP_AUDIT_WRITE},
    {Capability::AuditControl, CAP_AUDIT_CONTROL},
    {Capability::Setfcap, CAP_SETFCAP},
    {Capability::MacOverride, CAP_MAC_OVERRIDE},
    {Capability::MacAdmin, CAP_MAC_ADMIN},
    {Capability::Syslog, CAP_SYSLOG},
    {Capability::WakeAlarm, CAP_WAKE_ALARM},
    {Capability::BlockSuspend, CAP_BLOCK_SUSPEND},
    {Capability::AuditRead, CAP_AUDIT_READ},
    {Capability::InvalidValue, -1}};

score::cpp::expected_blank<score::os::Error> LinuxProcessCapabilitiesImpl::DropCapabilityFromBoundingSet(
    const Capability capability) noexcept
{
    assert(capability != Capability::InvalidValue);
    if (::cap_drop_bound(linux_capability_identifier_map.at(capability)) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(errno));
    }
    return {};
}

score::cpp::expected<ProcessCapabilitySets, Error> LinuxProcessCapabilitiesImpl::GetProcessCapabilitySets() noexcept
{
    ProcessCapabilitySets process_capabilities(static_cast<void*>(::cap_get_proc()), [this](void* ptr) {
        this->free(ptr);
    });
    if (process_capabilities.get() == nullptr)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(errno));
    }
    return {std::move(process_capabilities)};
}

score::cpp::expected_blank<Error> LinuxProcessCapabilitiesImpl::SetProcessCapabilitySets(
    const ProcessCapabilitySets& process_capabilities) noexcept
{
    assert(process_capabilities.get() != nullptr);
    if (::cap_set_proc(static_cast<LinuxProcessCapabilitySetsType>(process_capabilities.get())) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(errno));
    }
    return {};
}

score::cpp::expected_blank<Error> LinuxProcessCapabilitiesImpl::free(void* const object) noexcept
{
    assert(object != nullptr);
    if (::cap_free(object) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(errno));
    }
    return {};
}

score::cpp::expected_blank<Error> LinuxProcessCapabilitiesImpl::SetCapabilitiesInCapabilitySet(
    const ProcessCapabilitySets& process_capabilities,
    const CapabilitySets destination_set,
    const std::vector<Capability>& capabilities_to_set,
    const bool new_capability_value) noexcept
{
    assert(process_capabilities.get() != nullptr);
    assert(capabilities_to_set.size() > 0);
    for (const Capability capability : capabilities_to_set)
    {
        score::cpp::ignore = capability;  // required due to asserts being "compiled-away" in non-debug builds
        assert(capability != Capability::InvalidValue);
    }

    // there is always an upper bound to the number of capabilities since no duplicates are allowed, therefore
    // we can save on allocations by using a static array.
#define CAPABILTY_COUNT 39
    assert(capabilities_to_set.size() <= CAPABILTY_COUNT);
    cap_value_t capabilities[CAPABILTY_COUNT];
    for (std::uint8_t i = 0; i < capabilities_to_set.size(); ++i)
    {
        capabilities[i] = linux_capability_identifier_map.at(capabilities_to_set.at(static_cast<std::size_t>(i)));
    }

    if (::cap_set_flag(
            static_cast<LinuxProcessCapabilitySetsType>(process_capabilities.get()),
            linux_capability_sets_identifier_map.at(destination_set),
            static_cast<std::int32_t>(capabilities_to_set.size()),  // unfortunately the underlying API uses 32-bit ints
                                                                    // so we will have to live with this narrowing cast
            static_cast<cap_value_t*>(capabilities),
            new_capability_value ? CAP_SET : CAP_CLEAR) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(errno));
    }
    return {};
}

std::int32_t LinuxProcessCapabilitiesImpl::compare(const ProcessCapabilitySets& capability_set_a,
                                                   const ProcessCapabilitySets& capability_set_b) noexcept
{
    assert(capability_set_a.get() != nullptr && capability_set_b.get() != nullptr);
    return ::cap_compare(static_cast<LinuxProcessCapabilitySetsType>(capability_set_a.get()),
                         static_cast<LinuxProcessCapabilitySetsType>(capability_set_b.get()));
}

score::cpp::expected<std::int32_t, Error> LinuxProcessCapabilitiesImpl::prctl(const std::int32_t option,
                                                                              const std::uint64_t arg2,
                                                                              const std::uint64_t arg3,
                                                                              const std::uint64_t arg4,
                                                                              const std::uint64_t arg5) noexcept
{
    const std::int32_t result = ::prctl(option, arg2, arg3, arg4, arg5);
    if (result == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(errno));
    }
    return result;
}

}  // NAMESPACE os

}  // namespace score
