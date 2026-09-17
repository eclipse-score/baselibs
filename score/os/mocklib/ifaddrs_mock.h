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
#ifndef SCORE_LIB_OS_MOCKLIB_IFADDRS_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_IFADDRS_MOCK_H

#include "score/os/ifaddrs.h"
#include <ifaddrs.h>
#include <sys/socket.h>

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class IfaddrsMock : public Ifaddrs
{
  public:
    IfaddrsMock();

    ~IfaddrsMock();

    MOCK_METHOD((score::cpp::expected<ifaddrs*, Error>), getifaddrs, (), (const, noexcept, override));

    MOCK_METHOD((void), freeifaddrs, (ifaddrs*), (const, noexcept, override));

    void AddIfaddrsListEntry(const std::string& name,
                             std::uint32_t address,
                             bool should_add_netmask = false,
                             std::uint32_t netmask = 0,
                             std::uint16_t family_addr = AF_INET);

    score::cpp::expected<ifaddrs*, Error> GetIfaddrsList() const noexcept;

  private:
    ifaddrs* ifaddrs_list_;
};

}  // namespace os

}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_IFADDRS_MOCK_H
