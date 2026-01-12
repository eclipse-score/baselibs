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
#ifndef SCORE_LIB_OS_UTILS_NETWORK_VLAN_H
#define SCORE_LIB_OS_UTILS_NETWORK_VLAN_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

namespace score
{
namespace os
{

class Vlan : public ObjectSeam<Vlan>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static Vlan& instance() noexcept;

    /// \brief Sets the IEEE 802.1Q PCP field for a given file descriptor to define the priority of the packets sent by
    /// this socket.
    /// \arg file_descriptor valid socket file handle
    virtual score::cpp::expected_blank<Error> SetVlanPriorityOfSocket(const std::uint8_t pcp_priority,
                                                               const std::int32_t file_descriptor) noexcept = 0;

    virtual ~Vlan() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_NETWORK_VLAN_H
