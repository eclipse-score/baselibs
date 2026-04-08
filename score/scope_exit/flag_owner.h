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
#ifndef SCORE_LIB_SCOPE_EXIT_FLAG_OWNER_H
#define SCORE_LIB_SCOPE_EXIT_FLAG_OWNER_H

namespace score::utils
{

/// \brief Helper class which maintains a flag that has a single owner.
/// When an object of this class is move constructed or move assigned, the flag of the moved-from object will be
/// cleared.
class FlagOwner
{
  public:
    explicit FlagOwner(bool initial_value) : flag_{initial_value} {}

    ~FlagOwner() noexcept = default;

    FlagOwner(const FlagOwner&) = delete;
    FlagOwner& operator=(const FlagOwner&) & = delete;

    FlagOwner(FlagOwner&& other) noexcept : flag_{other.flag_}
    {
        // Suppress "AUTOSAR C++14 A18-9-2" rule findings.
        // We are not forwarding any value here, we clear the container.
        // coverity[autosar_cpp14_a18_9_2_violation]
        other.Clear();
    }

    FlagOwner& operator=(FlagOwner&& other) & noexcept
    {
        if (this != &other)
        {
            flag_ = other.flag_;
            // Same justification as in the move constructor.
            // coverity[autosar_cpp14_a18_9_2_violation]
            other.Clear();
        }
        return *this;
    }

    void Set()
    {
        flag_ = true;
    }
    void Clear()
    {
        flag_ = false;
    }

    bool IsSet() const
    {
        return flag_;
    }

  private:
    bool flag_;
};

}  // namespace score::utils

#endif  // SCORE_LIB_SCOPE_EXIT_FLAG_OWNER_H
