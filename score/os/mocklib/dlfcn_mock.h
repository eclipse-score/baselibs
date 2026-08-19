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

#ifndef SCORE_LIB_OS_MOCKLIB_DLFCN_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_DLFCN_MOCK_H

#include "score/os/dlfcn.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class DlfcnMock : public Dlfcn
{
  public:
    MOCK_METHOD((score::cpp::expected<void*, Error>), dlopen, (const char*, DlOpenFlag), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<void*, Error>), dlsym, (void*, const char*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), dlclose, (void*), (const, noexcept, override));
    MOCK_METHOD((std::optional<std::string_view>), dlerror, (), (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_DLFCN_MOCK_H
