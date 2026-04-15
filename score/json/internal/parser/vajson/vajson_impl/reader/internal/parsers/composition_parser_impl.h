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
 *        \brief  composition parser impl
 *
 *********************************************************************************************************************/
#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_COMPOSITION_PARSER_IMPL_H_
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_COMPOSITION_PARSER_IMPL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/composition_parser.h"

#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/array_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/bool_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/key_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/number_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/object_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/string_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include <utility>
namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{

template <typename Mixin>
template <typename Fn>
auto CompositionParser<Mixin>::Key(Fn fn) noexcept -> CallableReturnsResult<Fn, std::string_view, R>
{
    static_assert(ReturnsResultVoid<Fn, std::string_view>::value, "Must return vajson::ResultBlank");

    return KeyParser(this->doc_, std::forward<Fn>(fn)).SubParse();
}

template <typename Mixin>
template <typename Fn>
auto CompositionParser<Mixin>::Bool(Fn fn) noexcept -> CallableReturnsResult<Fn, bool, R>
{
    static_assert(ReturnsResultVoid<Fn, bool>::value, "Must return vajson::ResultBlank");

    return BoolParser(this->doc_, std::forward<Fn>(fn)).SubParse();
}

template <typename Mixin>
template <typename T, typename Fn>
auto CompositionParser<Mixin>::Number(Fn fn) noexcept -> CallableReturnsResult<Fn, T, R>
{
    static_assert(ReturnsResultVoid<Fn, T>::value, "Must return vajson::ResultBlank");

    return NumberParser<T>(this->doc_, std::forward<Fn>(fn)).SubParse();
}

template <typename Mixin>
template <typename Fn>
auto CompositionParser<Mixin>::String(Fn fn) noexcept -> CallableReturnsResult<Fn, std::string_view, R>
{
    static_assert(ReturnsResultVoid<Fn, std::string_view>::value, "Must return vajson::ResultBlank");

    return StringParser(this->doc_, std::forward<Fn>(fn)).SubParse();
}

template <typename Mixin>
template <typename Fn>
auto CompositionParser<Mixin>::Binary(Fn fn) noexcept -> CallableReturnsResult<Fn, Bytes, R>
{
    static_assert(ReturnsResultVoid<Fn, Bytes>::value, "Must return vajson::ResultBlank");

    return BinaryParser(this->doc_, std::forward<Fn>(fn)).SubParse();
}

template <typename Mixin>
template <typename Fn>
auto CompositionParser<Mixin>::Array(Fn fn) noexcept -> CallableReturnsResult<Fn, std::size_t, R>
{
    static_assert(ReturnsResultVoid<Fn, std::size_t>::value, "Must return vajson::ResultBlank");

    return ArrayParser(this->doc_, std::forward<Fn>(fn)).SubParse();
}

template <typename Mixin>
template <typename Fn>
auto CompositionParser<Mixin>::Object(Fn fn, bool object_already_open) noexcept
    -> CallableReturnsResult<Fn, std::string_view, R>
{
    static_assert(ReturnsResultVoid<Fn, std::string_view>::value, "Must return vajson::ResultBlank");

    return ObjectParser(doc_, std::forward<Fn>(fn), object_already_open).SubParse();
}

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_COMPOSITION_PARSER_IMPL_H_
