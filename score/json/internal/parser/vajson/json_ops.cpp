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
/// @file
/// @brief  json ops

#include "score/json/internal/parser/vajson/json_ops.h"
#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>

#include "score/assert.hpp"
#include "score/json/error.h"
#include "score/json/internal/parser/vajson/json_data.h"
namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{

JsonOps::JsonOps(JsonData& json_data) noexcept : data_(json_data) {}

auto JsonOps::Take() noexcept -> char
{
    return this->TryTake().value();
}

auto JsonOps::TryTake() noexcept -> Result<char>
{
    std::string_view& input{this->GetInput()};
    Result<char> result{
        score::MakeUnexpected(Error::kParsingError, "JsonOps::TryTake: Unexpected end of the document.")};

    if (!input.empty())
    {
        result = Result<char>{input.front()};
        input.remove_prefix(1U);
    }

    return result;
}

auto JsonOps::Move() noexcept -> bool
{
    std::string_view& input{this->GetInput()};
    const bool result{!input.empty()};
    if (result)
    {
        input.remove_prefix(1U);
    }
    return result;
}

/// @internal
/// - If the next character matches:
///   - Consume it, return true.
/// - Otherwise:
///   - Return false.
/// @endinternal
auto JsonOps::Skip(const char character) noexcept -> bool
{
    std::string_view& input{this->GetInput()};
    const bool result{(!input.empty()) && (input.front() == character)};
    if (result)
    {
        input.remove_prefix(1U);
    }
    return result;
}

/// @internal
/// - Check that the string contains what is expected.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto JsonOps::CheckString(const std::string_view string, const std::string_view error_msg) noexcept
    -> score::Result<void>
{
    score::Result<void> result{score::MakeUnexpected(Error::kParsingError, error_msg)};
    if (this->ReadString(string))
    {
        result.emplace();
    }
    return result;
}

/// @internal
/// - If the next characters are equal to the given string:
///   - Consume them, return true.
/// - Otherwise:
///   - Return false.
/// @endinternal
auto JsonOps::ReadString(const std::string_view string) noexcept -> bool
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(!string.empty(),
                                                      "JsonOps::ReadString: Cannot check for empty string");

    std::string_view& input{this->GetInput()};
    const bool result{input.substr(0U, string.size()) == string};
    if (result)
    {
        input.remove_prefix(string.size());
    }
    return result;
}

/// @internal
/// - Skip all white space characters.
/// @endinternal
auto JsonOps::SkipWhitespace() noexcept -> bool
{
    constexpr static StringView kWhitespaceChars{" \n\r\t"sv};
    std::string_view& input{this->GetInput()};
    input.remove_prefix(std::min(input.find_first_not_of(kWhitespaceChars), input.size()));
    return !input.empty();
}

/// @internal
/// - Consume all characters up to the first delimiter or the end of the document.
/// - If characters have been consumed:
///   - Trigger the given action on them.
/// - Return the delimiter that was found, or EOF.
/// @endinternal
auto JsonOps::ReadUntil(const std::string_view delimiter,
                        const score::cpp::move_only_function<void(std::string_view)>& callback) noexcept
    -> Result<OptChar>
{
    std::string_view& input{this->GetInput()};
    const std::size_t length{std::min(input.find_first_of(delimiter), input.size())};
    if (length > 0U)
    {
        callback(input.substr(0U, length));
        input.remove_prefix(length);
    }

    Result<OptChar> result{OptChar{-1}};
    if (!input.empty())
    {
        result = Result<OptChar>{OptChar{std::char_traits<char>::to_int_type(input.front())}};
    }
    return result;
}

auto JsonOps::GetJsonDocument() & noexcept -> JsonData&
{

    return this->data_.get();
}

auto JsonOps::GetInput() & noexcept -> std::string_view&
{

    return this->GetJsonDocument().GetInput();
}

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score
