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
#ifndef SCORE_LIB_OS_MOUNT_IMPL_H
#define SCORE_LIB_OS_MOUNT_IMPL_H

#include "score/os/mount.h"

namespace score
{
namespace os
{
class MountImpl final : public Mount
{
  public:
    constexpr MountImpl() = default;
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
    // Wrapper must be backward compatible for the applications that use linux specific mount() (without datalen arg)
    // NOLINTNEXTLINE(google-default-arguments) see comment above
    score::cpp::expected_blank<Error> mount(const char* const special_file,
                                            const char* const dir,
                                            const char* const fstype,
                                            const Mount::Flag flags,
                                            const void* const data,
                                            const std::int32_t datalen = -1) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> umount(const char* const target) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
};
}  // namespace os
}  // namespace score
#endif  // SCORE_LIB_OS_MOUNT_IMPL_H
