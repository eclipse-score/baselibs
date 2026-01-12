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
#ifndef SCORE_LIB_JSON_I_JSON_PARSER_MOCK_H
#define SCORE_LIB_JSON_I_JSON_PARSER_MOCK_H

#include "score/json/i_json_parser.h"

#include <gmock/gmock.h>

namespace score
{
namespace json
{

class IJsonParserMock : public IJsonParser
{
  public:
    MOCK_METHOD(score::Result<Any>, FromFile, (const std::string_view file_path), (const, noexcept, override));
    MOCK_METHOD(score::Result<Any>, FromBuffer, (const std::string_view buffer), (const, noexcept, override));
};

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_I_JSON_PARSER_MOCK_H
