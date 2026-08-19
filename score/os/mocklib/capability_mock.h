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
#ifndef SCORE_LIB_OS_MOCKLIB_LINUX_CAPABILITY_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_LINUX_CAPABILITY_MOCK_H

#include "score/os/capability.h"

#include <gmock/gmock.h>

namespace score
{

namespace os
{

class ProcessCapabilitiesMock : public ProcessCapabilities
{
  public:
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                DropCapabilityFromBoundingSet,
                (Capability capability),
                (noexcept, override));
    MOCK_METHOD((score::cpp::expected<ProcessCapabilitySets, Error>),
                GetProcessCapabilitySets,
                (),
                (noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                SetProcessCapabilitySets,
                (const ProcessCapabilitySets& process_capabilities),
                (noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), free, (void* const object), (noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                SetCapabilitiesInCapabilitySet,
                (const ProcessCapabilitySets& process_capabilities,
                 const CapabilitySets destination_set,
                 const std::vector<Capability>& capabilities_to_set,
                 bool new_capability_value),
                (noexcept, override));
    MOCK_METHOD((std::int32_t),
                compare,
                (const ProcessCapabilitySets& capability_set_a, const ProcessCapabilitySets& capability_set_b),
                (noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                prctl,
                (std::int32_t option, std::uint64_t arg2, std::uint64_t arg3, std::uint64_t arg4, std::uint64_t arg5),
                (noexcept, override));
};

}  // namespace os

}  // namespace score

#endif
