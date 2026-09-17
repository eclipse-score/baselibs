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
#ifndef SCORE_LIB_OS_LINUX_STATFS_H
#define SCORE_LIB_OS_LINUX_STATFS_H

#include <sys/statfs.h>
#include <cstdint>

namespace score
{
namespace os
{

std::int32_t statfs(const char* file, struct statfs* buf);

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_LINUX_STATFS_H
