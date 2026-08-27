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
#ifndef SCORE_LIB_OS_IOCTL_IMPL_H
#define SCORE_LIB_OS_IOCTL_IMPL_H

#include "score/os/ioctl.h"

namespace score
{
namespace os
{

class IoctlImpl final : public Ioctl
{
  public:
    constexpr IoctlImpl() = default;

  private:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:The wrapper function is identifiable through namespace*/
    score::cpp::expected_blank<Error> ioctl(const std::int32_t d,
                                            const std::int32_t request,
                                            void* const arg) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper Function is identifiable through namespace usage */
};
}  // namespace os
}  // namespace score
#endif
