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
/*!        \file
 *        \brief  A specialized parser for objects.
 *      \details  Enables easy parsing of simple object structures.
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_OBJECT_PARSER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_OBJECT_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/functional.hpp"
#include "score/json/internal/parser/vajson/vajson_impl/reader/v2/single_object_parser.h"
#include <utility>

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief           A parser that parses key value pairs

class ObjectParser final : public v2::SingleObjectParser
{
    /// \brief           Type of function to be executed when the objects are read
    using Fn = score::cpp::move_only_function<ResultBlank(StringView)>;

  public:
    /// \brief           Constructs an ObjectParser
    /// \details         Callback must take the object key as an std::string_view and return ResultBlank.
    /// \param[in]       doc
    ///                  JSON document to parse.
    /// \param[in]       fn
    ///                  Function to execute on every object key.
    /// \param[in]       object_already_open
    ///                  Specify if the object has already been opened. Defaults to false.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    ObjectParser(JsonData& doc, Fn&& fn, bool object_already_open = false) noexcept
        : v2::SingleObjectParser{doc, object_already_open}, fn_{std::move(fn)}
    {
    }

    /// \brief           Event for object keys
    /// \param[in]       key
    ///                  Parsed key.
    /// \return          kRunning if the callback function succeeds, or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - Execute the callback with the current key.
    /// - If the callback succeeds:
    ///   - Return kRunning.
    /// - Otherwise:
    ///   - Return the error of the callback.
    /// \endinternal
    auto OnKey(std::string_view key) noexcept -> ParserResult final
    {

        return std::forward<Fn>(this->fn_)(key).transform([](score::Blank) noexcept {
            return ParserState::kRunning;
        });
    }

  private:
    /// \brief           Function to be executed on every element
    Fn fn_;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_OBJECT_PARSER_H_
