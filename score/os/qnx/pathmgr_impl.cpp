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
#include "score/os/qnx/pathmgr_impl.h"

#include <sys/pathmgr.h>

namespace score
{
namespace os
{

score::cpp::expected_blank<Error> PathmgrImpl::pathmgr_symlink(const char* const path,
                                                        const char* const symlink) const noexcept
{
    if (::pathmgr_symlink(path, symlink) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<Error> PathmgrImpl::pathmgr_unlink(const char* const symlink) const noexcept
{
    if (::pathmgr_unlink(symlink) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}

}  // namespace os
}  // namespace score
