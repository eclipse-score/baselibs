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
#ifndef SCORE_LIB_OS_UTILS_MACHINE_SEAM_H
#define SCORE_LIB_OS_UTILS_MACHINE_SEAM_H

#include "score/os/ObjectSeam.h"

namespace score
{
namespace os
{

class Machine : public ObjectSeam<Machine>
{
  public:
    static Machine& instance() noexcept;

    virtual bool is_qemu() const noexcept = 0;

    ~Machine() override = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Machine(const Machine&) = delete;
    Machine& operator=(const Machine&) = delete;
    Machine(Machine&& other) = delete;
    Machine& operator=(Machine&& other) = delete;

  protected:
    Machine() = default;
};

}  // namespace os
}  // namespace score

#endif
