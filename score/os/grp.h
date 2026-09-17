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
#ifndef SCORE_LIB_OS_GRP_H
#define SCORE_LIB_OS_GRP_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include <score/expected.hpp>

#include <cstdint>

namespace score
{
namespace os
{

static constexpr std::size_t max_groupname_length{20U};

/// @brief Buffer struct
struct GroupBuffer
{
    // Wrapped function's signature requires C-style array
    // NOLINTBEGIN(modernize-avoid-c-arrays) see comment above
    // Suppress "AUTOSAR C++14 A9-6-1" rule findings. This rule declares: "Data types used for interfacing with hardware
    // or conforming to communication protocols shall be trivial, standard-layout and only contain members of types with
    // defined sizes."
    // Violation due to std::size_t which is typedef as unsigned long, which is of non standard type,
    // No harm for implementation.
    // coverity[autosar_cpp14_a9_6_1_violation]
    char name[max_groupname_length + static_cast<std::size_t>(1)];
    // NOLINTEND(modernize-avoid-c-arrays)
    // Rationale: Violation due to gid_t which is typedef as unsigned int, which is of non standard type,
    // No harm for implementation.
    // coverity[autosar_cpp14_a9_6_1_violation]
    gid_t gid;
};

class Grp : public ObjectSeam<Grp>
{
  public:
    static Grp& instance() noexcept;

    /// Get information about the group with a given name.
    /// Contrary to the original system call, this call may block.
    /// Contrary to the original system call, this call will return k
    /// \param group The name of the group
    /// \return A structure holding the name and id of the group or an error
    virtual score::cpp::expected<GroupBuffer, Error> getgrnam(const std::string& group) const noexcept = 0;

    virtual ~Grp() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Grp(const Grp&) = delete;
    Grp& operator=(const Grp&) = delete;
    Grp(Grp&& other) = delete;
    Grp& operator=(Grp&& other) = delete;

  protected:
    Grp() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_GRP_H
