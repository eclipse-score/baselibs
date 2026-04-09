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
 *        \brief  A SAX-Style JSON parser.
 *
 *      \details  Parses JSON text from a stream and sends events synchronously to an Implementer.
 *
 *********************************************************************************************************************/
/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <algorithm>
#include <cstring>
#include <string>

#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/depth_counter.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/json_ops.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/structure_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/parser_state.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/number.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{

auto StructureParserBase::Parse() noexcept -> Result<score::Blank>
{
    // Skip all whitespace to so that we will detect empty documents immediately.

    ParserResult result{MakeResult(this->GetJsonOps().SkipWhitespace(),
                                   MakeError(JsonErrc::kInvalidJson, "StructureParser::Parse: Document is empty."))
                            .transform([](score::Blank) noexcept {
                                return ParserState::kRunning;
                            })};

    // Run parser
    while (result == ParserState::kRunning)
    {
        result = this->ParseValue();
    }
    return Drop(result);
}

/*!
 /// \internal
 * - If the parsed string is "null" and a value is expected:
 *   - Call the OnNull callback and return its Result.
 * - Otherwise:
 *   - Return an error.
 /// \endinternal
 */
auto StructureParserBase::ParseNull() noexcept -> ParserResult
{

    return (this->GetJsonOps().CheckString("ull"sv, "StructureParser::ParseNull: Expected 'null'\0"))
        .and_then([this](score::Blank) noexcept {
            return this->GetState().AddValue();
        })
        .and_then([this](score::Blank) noexcept {
            return this->OnNull();
        });
}

/*!
 /// \spec
 * requires true;
 /// \endspec
 * \internal
 * - If the parsed string is "true" and a value is expected:
 *   - Call the OnBool callback with the value "true".
 *   - Return the Result of the callback.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto StructureParserBase::ParseTrue() noexcept -> ParserResult
{

    return (this->GetJsonOps().CheckString("rue"sv, "StructureParser::ParseTrue: Expected 'true'\0"sv))
        .and_then([this](score::Blank) noexcept {
            return this->GetState().AddValue();
        })
        .and_then([this](score::Blank) noexcept {
            return this->OnBool(true);
        });
}

/*!
 * \internal
 * - If the parsed string is "false" and a value is expected:
 *   - Call the OnBool callback with the value "false" and return its Result.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto StructureParserBase::ParseFalse() noexcept -> ParserResult
{

    return (this->GetJsonOps().CheckString("alse"sv, "StructureParser::ParseFalse: Expected 'false'\0"sv))
        .and_then([this](score::Blank) noexcept {
            return this->GetState().AddValue();
        })
        .and_then([this](score::Blank) noexcept {
            return this->OnBool(false);
        });
}

/*!
 * \internal
 * - If a value is expected:
 *   - Parse the number string.
 *   - Create a new JsonNumber from the string.
 *   - If the JsonNumber could be successfully created:
 *     - Call the OnNumber callback with the JsonNumber and return its Result.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto StructureParserBase::ParseNumber(const char cur) noexcept -> ParserResult
{
    return (this->GetState().AddValue())
        .and_then([this, cur](score::Blank) noexcept {
            return JsonNumber::New(this->GetNumber(cur));
        })
        .and_then([this](JsonNumber num) noexcept {
            return this->OnNumber(num);
        });
}

/*!
 * \internal
 * - Convert a JSON string to a C++ string.
 * - Parse this unescaped string.
 * \endinternal
 */
auto StructureParserBase::ParseString() noexcept -> ParserResult
{
    return ReadJsonString().and_then([this](CStringView string) noexcept {
        return ParseUnescapedString(string);
    });
}

/*!
 * \internal
 * - Skip all whitespace characters.
 * - If the string is followed by a colon and a key is expected:
 *   - Store the string as the current key.
 *   - Call the OnKey callback with the key.
 * - If the string is not followed by a colon a value is expected:
 *   - Call the OnString callback with the string.
 * - Return the Result of the callback.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto StructureParserBase::ParseUnescapedString(CStringView string) noexcept -> ParserResult
{

    static_cast<void>(this->GetJsonOps().SkipWhitespace());
    ParserResult result{ParserState::kRunning};

    if (this->GetJsonOps().Skip(':'))
    {
        result = this->GetState().AddKey().and_then([this, &string](score::Blank) noexcept {
            std::string_view const view{string};
            this->GetJsonDocument().StoreCurrentKey(view);

            return this->OnKey(string);
        });
    }
    else
    {
        result = this->GetState().AddValue().and_then([this, &string](score::Blank) noexcept {
            return this->OnString(string);
        });
    }

    return result;
}

/*!
 * \internal
 * - Read the string until either an escape sequence or a string termination occurs.
 * - While all operations are successful & an escape sequence is encountered
 *  - Unescape the character sequence
 *  - And read the string until either an escape sequence or a string termination occurs.
 * - Take the string termination character
 * - Return the result.
 * \endinternal
 */
auto StructureParserBase::ReadJsonString() noexcept -> Result<CStringView>
{
    String& buffer{this->GetJsonDocument().GetClearedStringBuffer()};

    const auto reader = [this, &buffer]() noexcept {
        return this->GetJsonOps().ReadUntil(R"("\)"sv,

                                            [&buffer](StringView buf) noexcept {
                                                static_cast<void>(buffer.append(buf.begin(), buf.end()));
                                            });
    };

    Result<OptChar> ret_val{reader()};
    while (ret_val.has_value())
    {
        if (!(*ret_val == '\\'))
        {
            break;
        }
        // We encountered an escape.

        // Take the backslash itself

        static_cast<void>(this->GetJsonOps().Move());

        // Unescape the escaped character

        ret_val = this->GetJsonOps()

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
            static_cast<void>(this->GetJsonOps().Move());
            return this->GetJsonDocument().GetCurrentString();
        });
}

/*!
 * \internal
 * - transform all characters to the appropriate escape sequence.
 * - If a unicode escape is encountered return an error.
 * - If an unknown/invalid escape is encountered return an error.
 * \endinternal
 */
auto StructureParserBase::UnescapeChar(const char escaped) noexcept -> Result<char>
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

/*!
 * \internal
 * - If an object can be added to the stack:
 *   - Call the OnStartObject callback and return its Result.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto StructureParserBase::ParseStartObject() noexcept -> ParserResult
{
    return this->GetState().AddObject().and_then([this](score::Blank) noexcept {
        return this->OnStartObject();
    });
}

/*!
 * \internal
 * - Move the position in the file past the closing brace.
 * - Pop the Object from the ItemStack.
 * - If the pop object call was successful:
 *   - Call the OnStartObject callback with the number of parsed keys and return its Result.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto StructureParserBase::ParseEndObject() noexcept -> ParserResult
{
    return this->GetState().PopObject().and_then([this](std::size_t count) noexcept {
        return this->OnEndObject(count);
    });
}

/*!
 * \internal
 * - If an array can be added to the stack:
 *   - Call the OnStartArray callback and return its Result.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto StructureParserBase::ParseStartArray() noexcept -> ParserResult
{
    return this->GetState().AddArray().and_then(

        [this](score::Blank) noexcept {
            return this->OnStartArray();
        });
}

/*!
 * \internal
 * - Move the position in the file past the closing bracket.
 * - Pop the Array from the ItemStack.
 * - If the pop array call was successful:
 *   - Call the OnEndArray callback with the number of parsed elements and return its Result.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto StructureParserBase::ParseEndArray() noexcept -> ParserResult
{
    return this->GetState().PopArray().and_then([this](std::size_t count) noexcept {
        return this->OnEndArray(count);
    });
}

/*!
 * \internal
 * - If the comma at this position is valid:
 *   - Return kRunning to continue parsing.
 * - Otherwise:
 *   - Return the error.
 * \endinternal
 */
auto StructureParserBase::ParseComma() noexcept -> ParserResult
{
    return MakeResult(this->GetState().AddComma(),
                      {JsonErrc::kInvalidJson, "StructureParser::ParseComma: Unexpected comma."})
        .transform([](score::Blank) noexcept {
            return ParserState::kRunning;
        });
}

/*!
 * \internal
 * - Read the next four bytes.
 * - If necessary, convert the value from network to platform endianness.
 * \endinternal
 */
auto StructureParserBase::ParseLength() noexcept -> Result<std::uint32_t>
{
    constexpr std::uint8_t kPrefixSize{4};
    std::uint32_t result{0};
    // clang-format off

  return this->GetJsonOps()

      .ReadExactly(kPrefixSize,
                   [&result](StringView view) noexcept {
                     static_cast<void>(std::memcpy(&result, view.data(), sizeof(result)));

                     result = be32toh(result);
                   })
      .transform([&result](score::Blank) noexcept { return result; });
    // clang-format on
}

/*!
 * \internal
 * - Parse the length of the binary content.
 * - Read the binary content.
 * - Execute the callback and return its result.
 * \endinternal
 */
auto StructureParserBase::ReadBinary(const score::cpp::move_only_function<ParserResult(StringView)>& callback) noexcept
    -> ParserResult
{
    return this->ParseLength().and_then([this, &callback](std::uint32_t length) noexcept {
        // Will always be overwritten.

        ParserResult result{MakeErrorResult<ParserState>(JsonErrc::kInvalidJson)};
        // clang-format off

    return And(this->GetJsonOps()

        .ReadExactly(length,

                     [&callback, &result](StringView view) noexcept {

                       result = callback(view);
                     }), result);
        // clang-format on
    });
}

/*!
 * \internal
 * - Add a key to the stack.
 * - If the key could be added:
 *   - Read the binary content.
 *   - Store the key.
 *   - Pass the stored key to the OnBinaryKey callback.
 * \endinternal
 */
auto StructureParserBase::ParseBinaryKey() noexcept -> ParserResult
{
    return this->GetState().AddKey().and_then([this](score::Blank) noexcept {
        return this->ReadBinary([this](StringView buf) noexcept {
            this->GetJsonDocument().StoreCurrentKey(buf);
            StringView const view{this->GetJsonDocument().GetCurrentKey()};

            return this->OnBinaryKey(view);
        });
    });
}

/*!
 * \internal
 * - Add a value to the stack.
 * - If the value could be added:
 *   - Read the binary content.
 *   - Pass it to the OnBinaryString callback.
 * \endinternal
 */
auto StructureParserBase::ParseBinaryString() noexcept -> ParserResult
{
    return this->GetState().AddValue().and_then([this](score::Blank) noexcept {
        return this->ReadBinary(

            [this](StringView buf) noexcept {
                return this->OnBinaryString(buf);
            });
    });
}

/*!
 * \internal
 * - Add a value to the stack.
 * - If the value could be added:
 *   - Read the binary content.
 *   - Pass it to the OnBinary callback.
 * \endinternal
 */
auto StructureParserBase::ParseBinaryValue() noexcept -> ParserResult
{
    return this->GetState().AddValue().and_then([this](score::Blank) noexcept {
        return this->ReadBinary([this](StringView buf) noexcept {
            score::cpp::span<char const> const view{buf.data(), buf.size()};

            return this->OnBinary(view);
        });
    });
}

/*!
 * \internal
 * - Skip all whitespace characters.
 * - If the stream has ended and the ItemStack is empty:
 *   - Return kFinished.
 * - If there are still new elements in the stream:
 *   - Parse the next character.
 *   - Determine the expected type to follow based on the parsed character.
 *   - Call the specialized parser method for this type.
 *   - Return the Result of the call.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto StructureParserBase::ParseValue() noexcept -> ParserResult
{
    ParserResult result{ParserState::kRunning};

    if (!this->GetJsonOps().SkipWhitespace())
    {
        result = this->GetState().CheckEndOfFile();
    }

    if (result == ParserState::kRunning)
    {
        static_cast<void>(this->GetJsonDocument().GetClearedStringBuffer());

        const char cur{this->GetJsonOps().Take()};
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
            case std::char_traits<char>::to_int_type('b'):
                result = this->ParseBinaryValue();
                break;
            case std::char_traits<char>::to_int_type('k'):
                result = this->ParseBinaryKey();
                break;
            case std::char_traits<char>::to_int_type('s'):
                result = this->ParseBinaryString();
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

/*!
 * \internal
 * - Parse all characters and add them to the number string until a comma, a closing brace/bracket, or a whitespace
 *   character is encountered.
 * \endinternal
 */
auto StructureParserBase::GetNumber(const char first) noexcept -> CStringView
{
    constexpr static StringView kLimiterChars{",}] \n\r\t"};
    String& buffer{this->GetJsonDocument().GetStringBuffer()};

    buffer.push_back(first);

    static_cast<void>(this->GetJsonOps()

                          .ReadUntil(StringView{kLimiterChars},
                                     [&buffer](StringView ch) noexcept {
                                         static_cast<void>(buffer.append(ch.begin(), ch.end()));
                                     })
                          .value());

    // Create a string view from the number-string
    return this->GetJsonDocument().GetCurrentString();
}

auto StructureParserBase::GetJsonDocument() noexcept -> JsonData&
{

    return this->GetJsonOps().GetJsonDocument();
}

auto StructureParserBase::GetJsonDocument() const noexcept -> const JsonData&
{

    return this->GetJsonOps().GetJsonDocument();
}

auto StructureParserBase::GetState() noexcept -> DepthCounter&
{
    return this->GetJsonDocument().GetState();
}

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score
