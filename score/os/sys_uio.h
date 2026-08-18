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
#ifndef SCORE_LIB_OS_SYS_UIO_H
#define SCORE_LIB_OS_SYS_UIO_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <cstdint>

#include <sys/uio.h>

namespace score
{
namespace os
{

class SysUio : public ObjectSeam<SysUio>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static SysUio& instance() noexcept;

    static score::cpp::pmr::unique_ptr<SysUio> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    virtual score::cpp::expected<std::int64_t, Error> writev(const std::int32_t fd,
                                                             const struct iovec* iovec_ptr,
                                                             const std::int32_t count) const noexcept = 0;

    virtual ~SysUio() = default;

  protected:
    SysUio() = default;
    SysUio(const SysUio&) = default;
    SysUio(SysUio&&) = default;
    SysUio& operator=(const SysUio&) = default;
    SysUio& operator=(SysUio&&) = default;
};

}  // namespace os
}  // namespace score
#endif  // SCORE_LIB_OS_SYS_UIO_H
