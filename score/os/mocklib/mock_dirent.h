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
#ifndef SCORE_LIB_OS_MOCKLIB_MOCK_DIRENT_H
#define SCORE_LIB_OS_MOCKLIB_MOCK_DIRENT_H

#include "score/os/dirent.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shadowing function name is intended. */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */

class MockDirent : public Dirent
{
  public:
    MOCK_METHOD((score::cpp::expected<DIR*, score::os::Error>),
                opendir,
                (const char* name),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<struct dirent*, score::os::Error>),
                readdir,
                (DIR * dirp),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                scandir,
                (const char* dirp,
                 struct dirent*** namelist,
                 std::int32_t (*filter)(const struct dirent*),
                 std::int32_t (*compar)(const struct dirent**, const struct dirent**)),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>), closedir, (DIR * dirp), (const, noexcept, override));
};

/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shadowing function name is intended. */

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_MOCK_DIRENT_H
