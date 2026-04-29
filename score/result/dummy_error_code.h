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

#ifndef SCORE_LIB_RESULT_DUMMY_ERROR_CODE_H
#define SCORE_LIB_RESULT_DUMMY_ERROR_CODE_H

#include "score/result/error.h"
#include "score/result/result.h"

#include <string_view>

namespace score::result
{
namespace
{

enum class DummyErrorCode : ErrorCode
{
    kFirstError,
    kSecondError,
};

class DummyErrorDomain final : public ErrorDomain
{
    [[nodiscard]] std::string_view MessageFor(const ErrorCode& code) const noexcept override
    {
        switch (static_cast<DummyErrorCode>(code))
        {
            case DummyErrorCode::kFirstError:
                return "First Error!";
            case DummyErrorCode::kSecondError:
                return "Second Error!";
            default:
                return "Unknown Error!";
        }
    }
};

constexpr DummyErrorDomain dummy_error_domain;

}  // namespace
}  // namespace score::result

#endif  // SCORE_LIB_RESULT_DUMMY_ERROR_CODE_H
