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
#ifndef SCORE_LIB_OS_MOUNT_H
#define SCORE_LIB_OS_MOUNT_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <cstdint>

namespace score
{
namespace os
{

class Mount : public ObjectSeam<Mount>
{
  public:
    static Mount& instance() noexcept;

    enum class Flag : std::int32_t
    {
        kNone = 0,
        kReadOnly = 1,
    };
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
    // Wrapper must be backward compatible for the applications that use linux specific mount() (without datalen arg)
    // NOLINTNEXTLINE(google-default-arguments) see comment above
    virtual score::cpp::expected_blank<Error> mount(const char* const special_file,
                                             const char* const dir,
                                             const char* const fstype,
                                             const Mount::Flag flags,
                                             const void* const data,
                                             const std::int32_t datalen = -1) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> umount(const char* const target) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */

    virtual ~Mount() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Mount(const Mount&) = delete;
    Mount& operator=(const Mount&) = delete;
    Mount(Mount&& other) = delete;
    Mount& operator=(Mount&& other) = delete;

  protected:
    Mount() = default;
};

}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Mount::Flag> : public std::true_type
{
};
}  // namespace score

#endif  // SCORE_LIB_OS_MOUNT_H
