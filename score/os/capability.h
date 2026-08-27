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
#ifndef SCORE_LIB_OS_CAPABILITY_H
#define SCORE_LIB_OS_CAPABILITY_H

#include "score/expected.hpp"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include <functional>
#include <memory>
#include <vector>

namespace score
{

namespace os
{

// light wrapper alias for cap_t which prevents leaking linux headers here
using ProcessCapabilitySets = std::unique_ptr<void, std::function<void(void*)>>;

enum class CapabilitySets : std::uint8_t
{
    Permitted,
    Inherited,
    Effective
};

enum class Capability : std::uint8_t
{
    SysAdmin,
    Kill,
    Chown,
    DacOverride,
    DacReadSearch,
    Fowner,
    Fsetid,
    Setgid,
    Setuid,
    Setpcap,
    LinuxImmutable,
    NetBindService,
    NetBroadCast,
    NetAdmin,
    NetRaw,
    IpcLock,
    IpcOwner,
    SysModule,
    SysRawio,
    SysChroot,
    SysPtrace,
    SysPacct,
    Sysboot,
    SysNice,
    SysResource,
    SysTime,
    SysTtyConfig,
    Mknod,
    Lease,
    AuditWrite,
    AuditControl,
    Setfcap,
    MacOverride,
    MacAdmin,
    Syslog,
    WakeAlarm,
    BlockSuspend,
    AuditRead,
    InvalidValue
};

class ProcessCapabilities : public ObjectSeam<ProcessCapabilities>
{
  public:
    static ProcessCapabilities& instance() noexcept;

    virtual score::cpp::expected_blank<Error> DropCapabilityFromBoundingSet(const Capability capability) noexcept = 0;

    virtual score::cpp::expected<ProcessCapabilitySets, Error> GetProcessCapabilitySets() noexcept = 0;

    virtual score::cpp::expected_blank<Error> SetProcessCapabilitySets(
        const ProcessCapabilitySets& process_capabilities) noexcept = 0;

    virtual score::cpp::expected_blank<Error> free(void* const object) noexcept = 0;

    virtual score::cpp::expected_blank<Error> SetCapabilitiesInCapabilitySet(
        const ProcessCapabilitySets& process_capabilities,
        const CapabilitySets destination_set,
        const std::vector<Capability>& capabilities_to_set,
        const bool new_capability_value) noexcept = 0;

    /*
     * compare has no explicit error cenarios, the only forseable situation in which it might error out
     * is if either of the capability sets points to null
     */
    virtual std::int32_t compare(const ProcessCapabilitySets& capability_set_a,
                                 const ProcessCapabilitySets& capability_set_b) noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> prctl(const std::int32_t option,
                                                            const std::uint64_t arg2,
                                                            const std::uint64_t arg3,
                                                            const std::uint64_t arg4,
                                                            const std::uint64_t arg5) noexcept = 0;

    score::cpp::expected_blank<Error> ReduceProcessCapabilitiesTo(const std::vector<Capability>& capabilities) noexcept;

    virtual ~ProcessCapabilities() = default;

  protected:
    ProcessCapabilities() = default;
    ProcessCapabilities(const ProcessCapabilities&) = default;
    ProcessCapabilities(ProcessCapabilities&&) = default;
    ProcessCapabilities& operator=(const ProcessCapabilities&) = default;
    ProcessCapabilities& operator=(ProcessCapabilities&&) = default;

  private:
    std::vector<Capability> DetermineCapabilitiesToDrop(const std::vector<Capability>& capabilities_to_keep);
    score::cpp::expected_blank<Error> DropUnwantedCapabilities(const std::vector<Capability>& capabilities_to_drop);
    score::cpp::expected_blank<Error> ActivateRequiredCapabilities(
        const std::vector<Capability>& required_capabilities);
};

}  // namespace os

}  // namespace score

#endif
