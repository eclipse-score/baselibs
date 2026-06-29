/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_OS_UTILS_MOCKLIB_VLAN_MOCK_H
#define SCORE_LIB_OS_UTILS_MOCKLIB_VLAN_MOCK_H

#include "score/network/vlan.h"
#include "score/os/errno.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class VlanMock : public Vlan
{
  public:
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                SetVlanPriorityOfSocket,
                (const std::uint8_t, const std::int32_t),
                (noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_MOCKLIB_VLAN_MOCK_H
