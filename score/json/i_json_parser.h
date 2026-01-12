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

#ifndef SCORE_LIB_JSON_I_JSON_PARSER_H
#define SCORE_LIB_JSON_I_JSON_PARSER_H

#include "score/json/internal/model/any.h"
#include "score/json/internal/model/object.h"
#include "score/result/result.h"

#include <string_view>

namespace score
{
namespace json
{

class IJsonParser
{
  public:
    virtual score::Result<Any> FromFile(const std::string_view file_path) const noexcept = 0;
    virtual score::Result<Any> FromBuffer(const std::string_view buffer) const noexcept = 0;
    IJsonParser() noexcept = default;
    IJsonParser(const IJsonParser&) = delete;
    IJsonParser(IJsonParser&&) noexcept = delete;
    IJsonParser& operator=(const IJsonParser&) = delete;
    IJsonParser& operator=(IJsonParser&&) noexcept = delete;
    virtual ~IJsonParser();
};

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_I_JSON_PARSER_H
