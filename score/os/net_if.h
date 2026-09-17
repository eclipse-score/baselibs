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
#ifndef SCORE_LIB_OS_IF_H
#define SCORE_LIB_OS_IF_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <cstdint>
#include <string>

namespace score
{
namespace os
{

class NetIf : public ObjectSeam<NetIf>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static NetIf& instance() noexcept;

    virtual score::cpp::expected<std::uint32_t, Error> if_nametoindex(const std::string& ifname) const noexcept = 0;

    virtual ~NetIf() = default;

  protected:
    NetIf() = default;
    NetIf(const NetIf&) = default;
    NetIf(NetIf&&) = default;
    NetIf& operator=(const NetIf&) = default;
    NetIf& operator=(NetIf&&) = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_IF_H
