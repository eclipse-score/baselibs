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
#ifndef SCORE_LIB_OS_QNX_PATHMGR_H
#define SCORE_LIB_OS_QNX_PATHMGR_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

namespace score
{
namespace os
{

class Pathmgr : public ObjectSeam<Pathmgr>
{
  public:
    static Pathmgr& instance() noexcept;

    virtual score::cpp::expected_blank<Error> pathmgr_symlink(const char* const path,
                                                       const char* const symlink) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> pathmgr_unlink(const char* const symlink) const noexcept = 0;

    virtual ~Pathmgr() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Pathmgr(const Pathmgr&) = delete;
    Pathmgr& operator=(const Pathmgr&) = delete;
    Pathmgr(Pathmgr&& other) = delete;
    Pathmgr& operator=(Pathmgr&& other) = delete;

  protected:
    Pathmgr() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_PATHMGR_H
