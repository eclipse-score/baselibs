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
#ifndef SCORE_LIB_OS_UNAME_H
#define SCORE_LIB_OS_UNAME_H

#include "score/os/ObjectSeam.h"

#include <score/optional.hpp>

#include <cstdint>
#include <string>

namespace score
{
namespace os
{

struct SystemInfo
{
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::string sysname;
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::string nodename;
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::string release;
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::string version;
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::string machine;
};

class Uname : public ObjectSeam<Uname>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static Uname& instance() noexcept;

    virtual score::cpp::optional<SystemInfo> GetUname() = 0;

    ~Uname() override = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Uname(const Uname&) = delete;
    Uname& operator=(const Uname&) = delete;
    Uname(Uname&& other) = delete;
    Uname& operator=(Uname&& other) = delete;

  protected:
    Uname() = default;
};

}  // namespace os
}  // namespace score
#endif  // SCORE_LIB_OS_UNAME_H
