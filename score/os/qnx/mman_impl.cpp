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
#include "score/os/qnx/mman_impl.h"
#include <sys/mman.h>
#include <sys/types.h>
namespace score
{
namespace os
{
namespace qnx
{

score::cpp::expected<void*, Error> MmanQnxImpl::mmap(void* const addr,
                                                     const std::size_t length,
                                                     const std::int32_t protection,
                                                     const std::int32_t flags,
                                                     const std::int32_t fd,
                                                     const std::int64_t offset) const noexcept
{
    void* const ret{::mmap(addr, length, protection, flags, fd, offset)};
    // Suppress "autosar_cpp14_m5_2_9_violation" rule finding. This rule states: "A cast shall not
    // convert a pointer type to an integral type."
    // Rationale: Cast is happening outside our code domain
    // coverity[autosar_cpp14_m5_2_9_violation]
    // coverity[autosar_cpp14_a5_2_2_violation] MAP_FAILED is builtin macro
    if (ret == MAP_FAILED)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<void*, Error> MmanQnxImpl::mmap64(void* addr,
                                                       const std::size_t length,
                                                       const std::int32_t protection,
                                                       const std::int32_t flags,
                                                       const std::int32_t fd,
                                                       const std::int64_t offset) const noexcept
{
#if defined(__QNX__) && defined(__LP64__)
    // On LP64 QNX, mmap already uses 64-bit offsets.
    void* const ret{::mmap(addr, length, protection, flags, fd, offset)};
#else
    void* const ret{::mmap64(addr, length, protection, flags, fd, offset)};
#endif
    if (ret == MAP_FAILED)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected_blank<Error> MmanQnxImpl::munmap(void* const addr, const std::size_t length) const noexcept
{
    if (::munmap(addr, length) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}
score::cpp::expected<uintptr_t, Error> MmanQnxImpl::mmap_device_io(const std::size_t length,
                                                                   const uint64_t address) const noexcept
{
    const uintptr_t ret{::mmap_device_io(length, address)};
    /* Not possible for ::mmap_device_io to return the error MAP_DEVICE_FAILED through unit test */
    // Cast is happening outside our code domain
    // coverity[autosar_cpp14_m5_2_9_violation]
    // coverity[autosar_cpp14_a5_2_2_violation] MAP_DEVICE_FAILED is builtin macro
    if (ret == MAP_DEVICE_FAILED)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> MmanQnxImpl::munmap_device_io(const uintptr_t address,
                                                                        const std::size_t length) const noexcept
{
    const std::int32_t ret{::munmap_device_io(address, length)};
    /* Not possible for ::munmap_device_io to return -1 through unit test */
    if (ret == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> MmanQnxImpl::shm_open(const char* const pathname,
                                                                const std::int32_t oflag,
                                                                const mode_t mode) const noexcept
{
    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret{::shm_open(pathname, oflag, mode)};
    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> MmanQnxImpl::shm_open_handle(const shm_handle_t handle,
                                                                       const std::int32_t flags) const noexcept
{
    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret{::shm_open_handle(handle, flags)};
    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> MmanQnxImpl::shm_create_handle(const std::int32_t fd,
                                                                         const pid_t pid,
                                                                         const std::int32_t flags,
                                                                         shm_handle_t* const handlep,
                                                                         const std::uint32_t options) const noexcept
{
    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret{::shm_create_handle(fd, pid, flags, handlep, options)};
    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> MmanQnxImpl::shm_ctl(std::int32_t fd,
                                                               const std::int32_t flags,
                                                               const std::uint64_t paddr,
                                                               const std::uint64_t size) const noexcept
{
    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret{::shm_ctl(fd, flags, paddr, size)};
    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected_blank<Error> MmanQnxImpl::mem_offset(const void* addr,
                                                          const std::int32_t fd,
                                                          const std::size_t length,
                                                          off_t* offset,
                                                          std::size_t* contig_len) const noexcept
{
    if (::mem_offset(addr, fd, length, offset, contig_len) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<Error> MmanQnxImpl::mem_offset64(const void* addr,
                                                            const std::int32_t fd,
                                                            const std::size_t length,
                                                            off64_t* offset,
                                                            std::size_t* contig_len) const noexcept
{
#if defined(__QNX__) && defined(__LP64__)
    // On LP64 QNX only mem_offset() exists; off_t is 64-bit already.
    if (::mem_offset(addr, fd, length, offset, contig_len) == -1)
#else
    if (::mem_offset64(addr, fd, length, offset, contig_len) == -1)
#endif
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}
}  // namespace qnx
}  // namespace os
}  // namespace score
