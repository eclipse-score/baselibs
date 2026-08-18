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
#ifndef SCORE_LIB_OS_LINUX_CAPABILITY_H
#define SCORE_LIB_OS_LINUX_CAPABILITY_H

#include "score/os/capability.h"

namespace score
{

namespace os
{

class LinuxProcessCapabilitiesImpl : public ProcessCapabilities
{
  public:
    virtual score::cpp::expected_blank<Error> DropCapabilityFromBoundingSet(
        const Capability capability) noexcept override;

    virtual score::cpp::expected<ProcessCapabilitySets, Error> GetProcessCapabilitySets() noexcept override;

    virtual score::cpp::expected_blank<Error> SetProcessCapabilitySets(
        const ProcessCapabilitySets& process_capabilities) noexcept override;

    virtual score::cpp::expected_blank<Error> free(void* const object) noexcept override;

    virtual score::cpp::expected_blank<Error> SetCapabilitiesInCapabilitySet(
        const ProcessCapabilitySets& process_capabilities,
        const CapabilitySets destination_set,
        const std::vector<Capability>& capabilities_to_set,
        const bool new_capability_value) noexcept override;

    virtual std::int32_t compare(const ProcessCapabilitySets& capability_set_a,
                                 const ProcessCapabilitySets& capability_set_b) noexcept override;

    virtual score::cpp::expected<std::int32_t, Error> prctl(const std::int32_t option,
                                                            const std::uint64_t arg2,
                                                            const std::uint64_t arg3,
                                                            const std::uint64_t arg4,
                                                            const std::uint64_t arg5) noexcept override;
};

}  // namespace os

}  // namespace score

#endif /* SCORE_LIB_OS_LINUX_CAPABILITY_H */
