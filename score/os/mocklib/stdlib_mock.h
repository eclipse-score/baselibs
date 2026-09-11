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
#ifndef SCORE_LIB_OS_MOCKLIB_STDLIB_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_STDLIB_MOCK_H

#include "score/os/stdlib.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class StdlibMock : public Stdlib
{
  public:
    MOCK_METHOD(score::cpp::expected_blank<Error>, system_call, (const std::string&), (const, noexcept, override));
    MOCK_METHOD(void, exit, (int), (const, noexcept, override));
    MOCK_METHOD(void, quick_exit, (int), (const, noexcept, override));
    MOCK_METHOD(char*, getenv, (const char*), (const, noexcept, override));
    MOCK_METHOD(Result<int>, setenv, (const char*, const char*, int), (const, noexcept, override));
    MOCK_METHOD(Result<int>, unsetenv, (const char*), (const, noexcept, override));
    MOCK_METHOD(Result<void*>, calloc, (size_t, size_t), (const, noexcept, override));
    MOCK_METHOD(void, free, (void*), (const, noexcept, override));
    MOCK_METHOD(Result<char*>, realpath, (const char*, char*), (const, noexcept, override));
    MOCK_METHOD(Result<int>, mkstemp, (char*), (const, noexcept, override));
    MOCK_METHOD(Result<int>, mkstemps, (char*, int), (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_STDLIB_MOCK_H
