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

#ifndef SCORE_LIB_JSON_JSON_PARSER_H
#define SCORE_LIB_JSON_JSON_PARSER_H

#include "score/json/i_json_parser.h"
#include "score/json/internal/model/any.h"
#include "score/json/internal/model/error.h"
#include "score/result/result.h"

#include <string_view>

namespace score
{
namespace json
{

class JsonParser : public IJsonParser
{
  public:
    /// \brief Parses the underlying file and creates a tree of JSON data
    /// \param file_path The path to the file that shall be parsed
    /// \return root to the tree of JSON data, error on error
    score::Result<Any> FromFile(const std::string_view file_path) const noexcept override;

    /// \brief Parses the underlying buffer and creates a tree of JSON data
    /// \param buffer The string_view that shall be parsed
    /// \return root to the tree of JSON data, error on error
    score::Result<Any> FromBuffer(const std::string_view buffer) const noexcept override;
};

/// \brief Parses the underlying buffer and creates a tree of JSON data
/// \return root to the tree of JSON data, on error will terminate
Any operator"" _json(const char* const data, const size_t size);

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_JSON_PARSER_H
