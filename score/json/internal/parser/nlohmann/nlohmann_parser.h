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

#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_NLOHMANN_NLOHMANN_PARSER_H
#define SCORE_LIB_JSON_INTERNAL_PARSER_NLOHMANN_NLOHMANN_PARSER_H

#include "score/json/internal/model/any.h"
#include "score/result/result.h"

namespace score::json
{

/// \brief General purpose JSON parser that uses nlohmann. It abstracts the vendor specific API and returns
/// a custom tree of data elements.
class NlohmannParser
{
  public:
    /// \brief Constructs a data-tree from a JSON file
    /// \param file_path The JSON file to read
    /// \return Any as root of the tree, nullptr on error
    static auto FromFile(const std::string_view file_path) -> score::Result<Any>;

    /// \brief Constructs a data-tree from a string containing JSON
    /// \param buffer The string_view containing JSON
    /// \return Any as root of the tree, nullptr on error
    static auto FromBuffer(const std::string_view buffer) -> score::Result<Any>;
};

}  // namespace score::json

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_NLOHMANN_NLOHMANN_PARSER_H
