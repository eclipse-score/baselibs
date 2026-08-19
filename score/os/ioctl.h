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
#ifndef SCORE_LIB_OS_IOCTL_H
#define SCORE_LIB_OS_IOCTL_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <cstdint>

namespace score
{
namespace os
{

class Ioctl : public ObjectSeam<Ioctl>
{
  public:
    static Ioctl& instance() noexcept;
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:The wrapper function is identifiable through namespace*/
    virtual score::cpp::expected_blank<Error> ioctl(const std::int32_t d,
                                                    const std::int32_t request,
                                                    void* const arg) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:The wrapper function is identifiable through namespace */

    virtual ~Ioctl() = default;

  protected:
    Ioctl() = default;
    Ioctl(const Ioctl&) = default;
    Ioctl(Ioctl&&) = default;
    Ioctl& operator=(const Ioctl&) = default;
    Ioctl& operator=(Ioctl&&) = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_IOCTL_H
