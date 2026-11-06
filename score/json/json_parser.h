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

#ifndef SCORE_JSON_JSON_PARSER_H
#define SCORE_JSON_JSON_PARSER_H

#include "score/json/i_json_parser.h"
#include "score/json/internal/model/any.h"
#include "score/json/internal/model/error.h"
#include "score/result/result.h"

#include <cstddef>
#include <string_view>

namespace score
{
namespace json
{

/// @brief Parses JSON with the vaJson parser.
class JsonParser : public IJsonParser
{
  public:
    /// @brief Default maximum number of nested arrays and objects, including the root container.
    static constexpr std::size_t kDefaultMaxNestingDepth{64U};

    /// @brief Creates a parser with the default maximum nesting depth.
    JsonParser() noexcept = default;

    /// @brief Creates a parser with a per-instance maximum nesting depth.
    /// @param max_nesting_depth Inclusive container depth limit; zero permits only scalar roots.
    /// @details The integrator must validate the limit against stack budgets for subsequent tree operations.
    explicit JsonParser(const std::size_t max_nesting_depth) noexcept : max_nesting_depth_{max_nesting_depth} {}

    /// @brief Parses the underlying file and creates a tree of JSON data
    /// @param file_path The path to the file that shall be parsed
    /// @return root to the tree of JSON data, error on error
    score::Result<Any> FromFile(const std::string_view file_path) const noexcept override;

    /// @brief Parses the underlying buffer and creates a tree of JSON data
    /// @param buffer The string_view that shall be parsed
    /// @return root to the tree of JSON data, error on error
    score::Result<Any> FromBuffer(const std::string_view buffer) const noexcept override;

  private:
    /// @brief Inclusive container depth limit applied independently to every parse call.
    const std::size_t max_nesting_depth_{kDefaultMaxNestingDepth};
};

/// @brief Parses the underlying buffer and creates a tree of JSON data
/// @return root to the tree of JSON data, on error will terminate
Any operator""_json(const char* const data, const size_t size);

}  // namespace json
}  // namespace score

#endif  // SCORE_JSON_JSON_PARSER_H
