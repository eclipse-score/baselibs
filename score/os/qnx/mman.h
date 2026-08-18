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
#ifndef SCORE_LIB_OS_QNX_MMAN_H
#define SCORE_LIB_OS_QNX_MMAN_H

#include "score/expected.hpp"
#include "score/os/errno.h"

#include <sys/mman.h>

namespace score
{
namespace os
{
namespace qnx
{
class MmanQnx
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<void*, Error> mmap(void* const addr,
                                                    const std::size_t length,
                                                    const std::int32_t protection,
                                                    const std::int32_t flags,
                                                    const std::int32_t fd,
                                                    const std::int64_t offset) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<void*, Error> mmap64(void* addr,
                                                      const std::size_t length,
                                                      const std::int32_t protection,
                                                      const std::int32_t flags,
                                                      const std::int32_t fd,
                                                      const std::int64_t offset) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> munmap(void* const addr, const std::size_t length) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<uintptr_t, Error> mmap_device_io(const std::size_t length,
                                                                  const uint64_t address) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> munmap_device_io(const uintptr_t address,
                                                                       const std::size_t length) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> shm_open(const char* const pathname,
                                                               const std::int32_t oflag,
                                                               const mode_t mode) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> shm_open_handle(const shm_handle_t handle,
                                                                      const std::int32_t flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> shm_create_handle(const std::int32_t fd,
                                                                        const pid_t pid,
                                                                        const std::int32_t flags,
                                                                        shm_handle_t* const handlep,
                                                                        const std::uint32_t options) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> shm_ctl(std::int32_t fd,
                                                              const std::int32_t flags,
                                                              const std::uint64_t paddr,
                                                              const std::uint64_t size) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> mem_offset(const void* addr,
                                                         const std::int32_t fd,
                                                         const std::size_t length,
                                                         off_t* offset,
                                                         std::size_t* contig_len) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> mem_offset64(const void* addr,
                                                           const std::int32_t fd,
                                                           const std::size_t length,
                                                           off64_t* offset,
                                                           std::size_t* contig_len) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~MmanQnx() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    MmanQnx(const MmanQnx&) = delete;
    MmanQnx& operator=(const MmanQnx&) = delete;
    MmanQnx(MmanQnx&& other) = delete;
    MmanQnx& operator=(MmanQnx&& other) = delete;

  protected:
    MmanQnx() = default;
};
}  // namespace qnx
}  // namespace os
}  // namespace score
#endif  // SCORE_LIB_OS_QNX_MMAN_H
