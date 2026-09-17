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

#ifndef SCORE_LIB_OS_IFADDRS_H
#define SCORE_LIB_OS_IFADDRS_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"
#include <ifaddrs.h>

namespace score
{
namespace os
{

/// @brief Wrapper class for the ifaddrs.h module
class Ifaddrs : public ObjectSeam<Ifaddrs>
{
  public:
    static Ifaddrs& instance() noexcept;

    virtual score::cpp::expected<ifaddrs*, Error> getifaddrs() const noexcept = 0;

    virtual void freeifaddrs(ifaddrs* ifa) const noexcept = 0;

    virtual ~Ifaddrs() = default;
    Ifaddrs() = default;
    Ifaddrs(const Ifaddrs& other) = delete;
    Ifaddrs(Ifaddrs&& other) = delete;
    Ifaddrs& operator=(const Ifaddrs& other) = delete;
    Ifaddrs& operator=(Ifaddrs&& other) = delete;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_IFADDRS_H
