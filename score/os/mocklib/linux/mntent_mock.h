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
#ifndef LIB_OS_MOCKLIB_MNTENT_MOCK_H
#define LIB_OS_MOCKLIB_MNTENT_MOCK_H

#include "score/os/linux/endmntent.h"
#include "score/os/linux/getmntent.h"
#include "score/os/linux/setmntent.h"
#include <gmock/gmock.h>

namespace score
{
namespace os
{

struct MntentMock
{
    MntentMock();

    MOCK_METHOD(FILE*, setMntent, (const char* file, const char* mode));
    MOCK_METHOD(struct mntent*, getMntent, (FILE * stream));
    MOCK_METHOD(int, endMntent, (FILE * stream));
};

} /* namespace os */
} /* namespace score */

#endif  // LIB_OS_MOCKLIB_MNTENT_MOCK_H
