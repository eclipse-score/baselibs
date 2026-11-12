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
/// @brief  A SAX-Style JSON parser.
///
/// @details  Parses JSON text from a stream and sends events synchronously to the derived class.

#include "score/json/internal/parser/vajson/structure_parser.h"

#include <algorithm>
#include <cstring>
#include <string>

#include "score/json/internal/parser/vajson/depth_counter.h"
#include "score/json/internal/parser/vajson/json_data.h"
#include "score/json/internal/parser/vajson/json_error_domain.h"
#include "score/json/internal/parser/vajson/json_number.h"
#include "score/json/internal/parser/vajson/json_ops.h"
#include "score/json/internal/parser/vajson/parser_state.h"
#include "score/json/internal/parser/vajson/types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{

StructureParser::StructureParser(JsonData& doc) noexcept : json_ops_{doc} {}

auto StructureParser::Parse() noexcept -> Result<void>
{
    // Skip all whitespace to so that we will detect empty documents immediately.

    ParserResult result{MakeResult(this->json_ops_.SkipWhitespace(),
                                   MakeError(JsonErrc::kInvalidJson, "StructureParser::Parse: Document is empty."))
                            .transform([](void) noexcept {
                                return ParserState::kRunning;
                            })};

    // Run parser
    while (result == ParserState::kRunning)
    {
        result = this->ParseValue();
    }
    return Drop(result);
}

auto StructureParser::GetState() noexcept -> DepthCounter&
{
    return this->GetJsonDocument().GetState();
}

auto StructureParser::GetJsonDocument() noexcept -> JsonData&
{

    return this->json_ops_.GetJsonDocument();
}

/// @internal
/// - If the parsed string is "null" and a value is expected:
///   - Call the OnNull callback and return its Result.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto StructureParser::ParseNull() noexcept -> ParserResult
{

    return (this->json_ops_.CheckString("ull"sv, "StructureParser::ParseNull: Expected 'null'\0"))
        .and_then([this](void) noexcept {
            return this->GetState().AddValue();
        })
        .and_then([this](void) noexcept {
            return this->OnNull();
        });
}

/// @internal
/// - If the parsed string is "true" and a value is expected:
///   - Call the OnBool callback with the value "true".
///   - Return the Result of the callback.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto StructureParser::ParseTrue() noexcept -> ParserResult
{

    return (this->json_ops_.CheckString("rue"sv, "StructureParser::ParseTrue: Expected 'true'\0"sv))
        .and_then([this](void) noexcept {
            return this->GetState().AddValue();
        })
        .and_then([this](void) noexcept {
            return this->OnBool(true);
        });
}

/// @internal
/// - If the parsed string is "false" and a value is expected:
///   - Call the OnBool callback with the value "false" and return its Result.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto StructureParser::ParseFalse() noexcept -> ParserResult
{

    return (this->json_ops_.CheckString("alse"sv, "StructureParser::ParseFalse: Expected 'false'\0"sv))
        .and_then([this](void) noexcept {
            return this->GetState().AddValue();
        })
        .and_then([this](void) noexcept {
            return this->OnBool(false);
        });
}

/// @internal
/// - If a value is expected:
///   - Parse the number string.
///   - Create a new JsonNumber from the string.
///   - If the JsonNumber could be successfully created:
///     - Call the OnNumber callback with the JsonNumber and return its Result.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto StructureParser::ParseNumber(const char cur) noexcept -> ParserResult
{
    return (this->GetState().AddValue())
        .and_then([this, cur](void) noexcept {
            return JsonNumber::New(this->GetNumber(cur));
        })
        .and_then([this](JsonNumber num) noexcept {
            return this->OnNumber(num);
        });
}

/// @internal
/// - Convert a JSON string to a C++ string.
/// - Parse this unescaped string.
/// @endinternal
auto StructureParser::ParseString() noexcept -> ParserResult
{
    return ReadJsonString().and_then([this](CStringView string) noexcept {
        return ParseUnescapedString(string);
    });
}

/// @internal
/// - Skip all whitespace characters.
/// - If the string is followed by a colon and a key is expected:
///   - Call the OnKey callback with the key.
/// - If the string is not followed by a colon a value is expected:
///   - Call the OnString callback with the string.
/// - Return the Result of the callback.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto StructureParser::ParseUnescapedString(CStringView string) noexcept -> ParserResult
{

    static_cast<void>(this->json_ops_.SkipWhitespace());
    ParserResult result{ParserState::kRunning};

    if (this->json_ops_.Skip(':'))
    {
        result = this->GetState().AddKey().and_then([this, &string](void) noexcept {
            return this->OnKey(string);
        });
    }
    else
    {
        result = this->GetState().AddValue().and_then([this, &string](void) noexcept {
            return this->OnString(string);
        });
    }

    return result;
}

/// @internal
/// - Read the string until either an escape sequence or a string termination occurs.
/// - While all operations are successful & an escape sequence is encountered
///  - Unescape the character sequence
///  - And read the string until either an escape sequence or a string termination occurs.
/// - Take the string termination character
/// - Return the result.
/// @endinternal
auto StructureParser::ReadJsonString() noexcept -> Result<CStringView>
{
    String& buffer{this->GetJsonDocument().GetClearedStringBuffer()};

    const auto reader{[this, &buffer]() noexcept {
        return this->json_ops_.ReadUntil(R"("\)"sv,

                                         [&buffer](StringView buf) noexcept {
                                             static_cast<void>(buffer.append(buf.begin(), buf.end()));
                                         });
    }};

    Result<OptChar> ret_val{reader()};
    while (ret_val.has_value())
    {
        if (!(*ret_val == '\\'))
        {
            break;
        }
        // We encountered an escape.

        // Take the backslash itself

        static_cast<void>(this->json_ops_.Move());

        // Unescape the escaped character

        ret_val = this->json_ops_

                      .TryTake()
                      .and_then([this](const char escaped) noexcept {
                          return this->UnescapeChar(escaped);
                      })
                      .and_then([&reader, &buffer](const char unescaped) noexcept {
                          buffer.push_back(unescaped);
                          // And retry

                          return reader();
                      });
    }

    return Filter(
               ret_val,
               [](const OptChar opt) noexcept {
                   return opt.HasValue();
               },
               MakeError(JsonErrc::kInvalidJson, "ReadJsonString: Runaway string."))
        // Take the " character

        .transform([this](OptChar) noexcept {
            static_cast<void>(this->json_ops_.Move());
            return this->GetJsonDocument().GetCurrentString();
        });
}

/// @internal
/// - transform all characters to the appropriate escape sequence.
/// - If a unicode escape is encountered return an error.
/// - If an unknown/invalid escape is encountered return an error.
/// @endinternal
auto StructureParser::UnescapeChar(const char escaped) noexcept -> Result<char>
{
    Result<char> ret_val{escaped};
    switch (std::char_traits<char>::to_int_type(escaped))
    {
        case std::char_traits<char>::to_int_type('b'):
            ret_val = '\b';
            break;
        case std::char_traits<char>::to_int_type('f'):
            ret_val = '\f';
            break;
        case std::char_traits<char>::to_int_type('n'):
            ret_val = '\n';
            break;
        case std::char_traits<char>::to_int_type('r'):
            ret_val = '\r';
            break;
        case std::char_traits<char>::to_int_type('t'):
            ret_val = '\t';
            break;
        case std::char_traits<char>::to_int_type('\\'):
        case std::char_traits<char>::to_int_type('/'):
        case std::char_traits<char>::to_int_type('"'):
            break;
        case std::char_traits<char>::to_int_type('u'):

            ret_val = MakeErrorResult<char>(JsonErrc::kInvalidJson, "Unicode escape: \\u notation is not supported!");
            break;
        default:

            ret_val = MakeErrorResult<char>(JsonErrc::kInvalidJson, "Unknown escape sequence!");
            break;
    }
    return ret_val;
}

/// @internal
/// - If an object can be added to the stack:
///   - Call the OnStartObject callback and return its Result.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto StructureParser::ParseStartObject() noexcept -> ParserResult
{
    return this->GetState().AddObject().and_then([this](void) noexcept {
        return this->OnStartObject();
    });
}

/// @internal
/// - Move the position in the file past the closing brace.
/// - Pop the Object from the ItemStack.
/// - If the pop object call was successful:
///   - Call the OnStartObject callback with the number of parsed keys and return its Result.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto StructureParser::ParseEndObject() noexcept -> ParserResult
{
    return this->GetState().PopObject().and_then([this](std::size_t count) noexcept {
        return this->OnEndObject(count);
    });
}

/// @internal
/// - If an array can be added to the stack:
///   - Call the OnStartArray callback and return its Result.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto StructureParser::ParseStartArray() noexcept -> ParserResult
{
    return this->GetState().AddArray().and_then(

        [this](void) noexcept {
            return this->OnStartArray();
        });
}

/// @internal
/// - Move the position in the file past the closing bracket.
/// - Pop the Array from the ItemStack.
/// - If the pop array call was successful:
///   - Call the OnEndArray callback with the number of parsed elements and return its Result.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto StructureParser::ParseEndArray() noexcept -> ParserResult
{
    return this->GetState().PopArray().and_then([this](std::size_t count) noexcept {
        return this->OnEndArray(count);
    });
}

/// @internal
/// - If the comma at this position is valid:
///   - Return kRunning to continue parsing.
/// - Otherwise:
///   - Return the error.
/// @endinternal
auto StructureParser::ParseComma() noexcept -> ParserResult
{
    return MakeResult(this->GetState().AddComma(),
                      {JsonErrc::kInvalidJson, "StructureParser::ParseComma: Unexpected comma."})
        .transform([](void) noexcept {
            return ParserState::kRunning;
        });
}

/// @internal
/// - Skip all whitespace characters.
/// - If the stream has ended and the ItemStack is empty:
///   - Return kFinished.
/// - If there are still new elements in the stream:
///   - Parse the next character.
///   - Determine the expected type to follow based on the parsed character.
///   - Call the specialized parser method for this type.
///   - Return the Result of the call.
/// - Otherwise:
///   - Return an error.
/// @endinternal
auto StructureParser::ParseValue() noexcept -> ParserResult
{
    ParserResult result{ParserState::kRunning};

    if (!this->json_ops_.SkipWhitespace())
    {
        result = this->GetState().CheckEndOfFile();
    }

    if (result == ParserState::kRunning)
    {
        static_cast<void>(this->GetJsonDocument().GetClearedStringBuffer());

        const char cur{this->json_ops_.Take()};
        switch (std::char_traits<char>::to_int_type(cur))
        {
            case std::char_traits<char>::to_int_type('n'):
                result = this->ParseNull();
                break;
            case std::char_traits<char>::to_int_type('t'):
                result = this->ParseTrue();
                break;
            case std::char_traits<char>::to_int_type('f'):
                result = this->ParseFalse();
                break;
            case std::char_traits<char>::to_int_type('"'):
                result = this->ParseString();
                break;
            case std::char_traits<char>::to_int_type('{'):
                result = this->ParseStartObject();
                break;
            case std::char_traits<char>::to_int_type('}'):
                result = this->ParseEndObject();
                break;
            case std::char_traits<char>::to_int_type('['):
                result = this->ParseStartArray();
                break;
            case std::char_traits<char>::to_int_type(']'):
                result = this->ParseEndArray();
                break;
            case std::char_traits<char>::to_int_type(','):
                result = this->ParseComma();
                break;
            case std::char_traits<char>::to_int_type('-'):
            case std::char_traits<char>::to_int_type('0'):
            case std::char_traits<char>::to_int_type('1'):
            case std::char_traits<char>::to_int_type('2'):
            case std::char_traits<char>::to_int_type('3'):
            case std::char_traits<char>::to_int_type('4'):
            case std::char_traits<char>::to_int_type('5'):
            case std::char_traits<char>::to_int_type('6'):
            case std::char_traits<char>::to_int_type('7'):
            case std::char_traits<char>::to_int_type('8'):
            case std::char_traits<char>::to_int_type('9'):
                result = this->ParseNumber(cur);
                break;
            default:

                result = MakeErrorResult<ParserState>(JsonErrc::kInvalidJson, "ParseValue: Got unknown JSON token.");
                break;
        }
    }
    return result;
}

/// @internal
/// - Parse all characters and add them to the number string until a comma, a closing brace/bracket, or a whitespace
///   character is encountered.
/// @endinternal
auto StructureParser::GetNumber(const char first) noexcept -> CStringView
{
    constexpr static StringView kLimiterChars{",}] \n\r\t"};
    String& buffer{this->GetJsonDocument().GetStringBuffer()};

    buffer.push_back(first);

    static_cast<void>(this->json_ops_

                          .ReadUntil(StringView{kLimiterChars},
                                     [&buffer](StringView ch) noexcept {
                                         static_cast<void>(buffer.append(ch.begin(), ch.end()));
                                     })
                          .value());

    // Create a string view from the number-string
    return this->GetJsonDocument().GetCurrentString();
}

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score
