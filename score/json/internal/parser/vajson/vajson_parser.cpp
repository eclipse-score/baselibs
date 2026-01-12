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
#include "score/json/internal/parser/vajson/vajson_parser.h"

auto score::json::VajsonParser::FromFile(const std::string_view file_path) -> score::Result<score::json::Any>
{
    score::Result<score::json::Any> result = MakeUnexpected(Error::kParsingError);
    // NOLINTNEXTLINE(score-banned-function) Tolerated because JsonParser::FromFile is also on the banned function list
    auto json_data = amsr::json::JsonData::FromFile(file_path);
    if (json_data.HasValue() == true)
    {
        score::Result<score::json::Any> json_object = VajsonParser{json_data.Value()}.GetData();
        if (json_object.has_value())
        {
            result = std::move(*json_object);
        }
    }
    return result;
}

auto score::json::VajsonParser::FromBuffer(const std::string_view buffer) -> score::Result<score::json::Any>
{
    score::Result<score::json::Any> result = MakeUnexpected(Error::kParsingError);
    auto json_data = amsr::json::JsonData::FromBuffer(ara::core::StringView{buffer.data(), buffer.size()});
    if (json_data.HasValue() ==
        true)  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from coverage report.)
    // (else branch can't be hit due to internal impementation of JsonData::FromBuffer function that accepts all kind of
    // strings.)
    {
        auto json_object = VajsonParser{json_data.Value()}.GetData();
        if (json_object.has_value())
        {
            result = std::move(*json_object);
        }
    }
    return result;
}

auto score::json::VajsonParser::GetData() noexcept -> score::Result<score::json::Any>
{
    const auto result = Parse();
    if (result.HasValue() == true)
    {
        score::Result<score::json::Any> tmp{};
        std::swap(*tmp, root_);
        return tmp;
    }
    return MakeUnexpected(Error::kParsingError);
}

auto score::json::VajsonParser::OnNull() noexcept -> amsr::json::ParserResult
{
    auto result = Store(Null{});
    if (!result.has_value())  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from
                              // coverage report. See comment below)
    {
        // Coverage: Not reachable. Line excluded from coverage report.
        // This case never happens, because condition can't be fulfilled due to Store implementantion
        return amsr::json::ParserResult::FromError(amsr::json::JsonErrc::kInvalidJson); /* LCOV_EXCL_LINE */
    }
    return amsr::json::ParserState::kRunning;
}

auto score::json::VajsonParser::OnBool(bool value) noexcept -> amsr::json::ParserResult
{
    auto result = Store(value);
    if (!result.has_value())  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from
                              // coverage report. See comment below)
    {
        // Coverage: Not reachable. Line excluded from coverage report.
        // This case never happens, because condition can't be fulfilled due to Store implementantion
        return amsr::json::ParserResult::FromError(amsr::json::JsonErrc::kInvalidJson); /* LCOV_EXCL_LINE */
    }
    return amsr::json::ParserState::kRunning;
}

auto score::json::VajsonParser::OnNumber(amsr::json::JsonNumber value) noexcept -> amsr::json::ParserResult
{
    // The parser shall try first the unsigned types from smallest to largest.
    // Then it shall try the signed types from smallest to largest.
    // Finally it will try double.
    // This way the model can assume that the number type is the "smallest possible type" except from floating-point
    // numbers, they shall be always presented as a 'double'
    return OnNumber<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, double>(value);
}

auto score::json::VajsonParser::OnString(ara::core::StringView value) noexcept -> amsr::json::ParserResult
{
    auto result = Store(value.ToString());
    if (!result.has_value())  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from
                              // coverage report. See comment below)
    {
        // Coverage: Not reachable. Line excluded from coverage report.
        // This case never happens, because condition can't be fulfilled due to Store implementantion
        return amsr::json::ParserResult::FromError(amsr::json::JsonErrc::kInvalidJson); /* LCOV_EXCL_LINE */
    }
    return amsr::json::ParserState::kRunning;
}

auto score::json::VajsonParser::OnKey(ara::core::StringView key) noexcept -> amsr::json::ParserResult
{
    last_key_ = key.ToString();
    return amsr::json::ParserState::kRunning;
}

template <typename T>
auto score::json::VajsonParser::StartContainer(T&& value) noexcept -> amsr::json::ParserResult
{
    auto result = Store(std::forward<T>(value));
    if (!result.has_value())  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from
                              // coverage report. See comment below)
    {
        // Coverage: Not reachable. Line excluded from coverage report.
        // This case never happens, because condition can't be fulfilled due to Store implementantion
        return amsr::json::ParserResult::FromError(amsr::json::JsonErrc::kInvalidJson); /* LCOV_EXCL_LINE */
    }
    else
    {
        // Justification: Using std::move or std::forward on a raw pointer does not provide any benefit
        // coverity[autosar_cpp14_a18_9_2_violation]
        hierarchy_.push(*result);
    }
    return amsr::json::ParserState::kRunning;
}

auto score::json::VajsonParser::EndContainer() noexcept -> amsr::json::ParserResult
{
    if (!hierarchy_.empty())  // Check if the stack is not empty
    {
        hierarchy_.pop();
    }
    return amsr::json::ParserState::kRunning;
}

auto score::json::VajsonParser::OnStartObject() noexcept -> amsr::json::ParserResult
{
    return StartContainer(score::json::Object{});
}

auto score::json::VajsonParser::OnEndObject(std::size_t) noexcept -> amsr::json::ParserResult
{
    return EndContainer();
}

auto score::json::VajsonParser::OnStartArray() noexcept -> amsr::json::ParserResult
{
    return StartContainer(score::json::List{});
}

auto score::json::VajsonParser::OnEndArray(std::size_t) noexcept -> amsr::json::ParserResult
{
    return EndContainer();
}

auto score::json::VajsonParser::OnUnexpectedEvent() noexcept -> amsr::json::ParserResult
{
    // A call to this function means we have not implemented one of the necessary callbacks.
    // We set this equal to a failed user validation, since a missing callback means we do not support this type.
    return amsr::json::ParserResult::FromError(amsr::json::JsonErrc::kUserValidationFailed);
}
