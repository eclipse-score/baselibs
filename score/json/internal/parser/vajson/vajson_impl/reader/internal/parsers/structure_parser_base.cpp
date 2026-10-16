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
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>

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
namespace
{

/// \brief           Number of hexadecimal digits of a \\uXXXX escape sequence
constexpr std::size_t kHexDigitsPerEscape{4U};
/// \brief           Number of bits encoded by a single hexadecimal digit
constexpr std::uint32_t kHexDigitWidth{4U};
/// \brief           Value of the first hexadecimal digit represented by a letter
constexpr std::uint32_t kHexadecimalLetterOffset{10U};
/// \brief           First code point that UTF-8 encodes in more than a single byte
constexpr std::uint32_t kMultiByteFirst{0x80U};
/// \brief           First code point that UTF-8 encodes in three bytes
constexpr std::uint32_t kThreeByteFirst{0x800U};
/// \brief           First code point that UTF-8 encodes in four bytes, and that UTF-16 encodes as a surrogate pair
constexpr std::uint32_t kSupplementaryPlaneFirst{0x10000U};
/// \brief           Marker bits of the leading byte of a two byte UTF-8 sequence
constexpr std::uint32_t kTwoByteMarker{0xC0U};
/// \brief           Marker bits of the leading byte of a three byte UTF-8 sequence
constexpr std::uint32_t kThreeByteMarker{0xE0U};
/// \brief           Marker bits of the leading byte of a four byte UTF-8 sequence
constexpr std::uint32_t kFourByteMarker{0xF0U};
/// \brief           Marker bits of a UTF-8 continuation byte
constexpr std::uint32_t kContinuationMarker{0x80U};
/// \brief           Mask of the code point bits carried by a UTF-8 continuation byte
constexpr std::uint32_t kContinuationMask{0x3FU};
/// \brief           Number of bits a UTF-8 continuation byte contributes to the code point
constexpr std::uint32_t kContinuationWidth{6U};
/// \brief           First code unit of the UTF-16 high surrogate range
constexpr std::uint32_t kHighSurrogateFirst{0xD800U};
/// \brief           First code unit of the UTF-16 low surrogate range
constexpr std::uint32_t kLowSurrogateFirst{0xDC00U};
/// \brief           Last code unit of the UTF-16 surrogate range
constexpr std::uint32_t kSurrogateLast{0xDFFFU};
/// \brief           Number of bits a high surrogate contributes to the code point
constexpr std::uint32_t kSurrogateWidth{10U};
/// \brief           Error message for a high surrogate that is not completed by a low surrogate
constexpr StringView kMissingLowSurrogate{"Unicode escape: High surrogate is not followed by a low surrogate!"sv};

/// \brief           Converts a hexadecimal digit into its numerical value
/// \param[in]       digit
///                  The character to convert.
/// \return          The value of the digit, or an empty Optional if the character is no hexadecimal digit.
auto HexDigitValue(const char digit) noexcept -> Optional<std::uint32_t>
{
    Optional<std::uint32_t> ret_val{};
    const auto value = std::char_traits<char>::to_int_type(digit);
    if ((value >= '0') && (value <= '9'))
    {
        ret_val = static_cast<std::uint32_t>(value - '0');
    }
    else if ((value >= 'a') && (value <= 'f'))
    {
        ret_val = static_cast<std::uint32_t>(value - 'a') + kHexadecimalLetterOffset;
    }
    else if ((value >= 'A') && (value <= 'F'))
    {
        ret_val = static_cast<std::uint32_t>(value - 'A') + kHexadecimalLetterOffset;
    }
    else
    {
        // The character is no hexadecimal digit.
    }
    return ret_val;
}

/// \brief           Checks whether a UTF-16 code unit is a high surrogate
/// \param[in]       code_unit
///                  The code unit to check.
/// \return          True if the code unit is a high surrogate.
auto IsHighSurrogate(const std::uint32_t code_unit) noexcept -> bool
{
    return (code_unit >= kHighSurrogateFirst) && (code_unit < kLowSurrogateFirst);
}

/// \brief           Checks whether a UTF-16 code unit is a low surrogate
/// \param[in]       code_unit
///                  The code unit to check.
/// \return          True if the code unit is a low surrogate.
auto IsLowSurrogate(const std::uint32_t code_unit) noexcept -> bool
{
    return (code_unit >= kLowSurrogateFirst) && (code_unit <= kSurrogateLast);
}

/// \brief           Combines a UTF-16 surrogate pair into the code point it denotes
/// \param[in]       high_surrogate
///                  The leading code unit of the pair.
/// \param[in]       low_surrogate
///                  The trailing code unit of the pair.
/// \return          The combined code point.
auto CombineSurrogatePair(const std::uint32_t high_surrogate, const std::uint32_t low_surrogate) noexcept
    -> std::uint32_t
{
    return kSupplementaryPlaneFirst +
           (((high_surrogate - kHighSurrogateFirst) << kSurrogateWidth) + (low_surrogate - kLowSurrogateFirst));
}

/// \brief           Appends a Unicode code point to a buffer, encoded as UTF-8
/// \param[in]       code_point
///                  The code point to encode, must not be a surrogate.
/// \param[in,out]   buffer
///                  The buffer to append to.
void AppendUtf8(const std::uint32_t code_point, String& buffer) noexcept
{
    const auto append = [&buffer](const std::uint32_t byte) noexcept {
        buffer.push_back(std::char_traits<char>::to_char_type(static_cast<std::int32_t>(byte)));
    };

    if (code_point < kMultiByteFirst)
    {
        append(code_point);
    }
    else if (code_point < kThreeByteFirst)
    {
        append(kTwoByteMarker | (code_point >> kContinuationWidth));
        append(kContinuationMarker | (code_point & kContinuationMask));
    }
    else if (code_point < kSupplementaryPlaneFirst)
    {
        append(kThreeByteMarker | (code_point >> (2U * kContinuationWidth)));
        append(kContinuationMarker | ((code_point >> kContinuationWidth) & kContinuationMask));
        append(kContinuationMarker | (code_point & kContinuationMask));
    }
    else
    {
        append(kFourByteMarker | (code_point >> (3U * kContinuationWidth)));
        append(kContinuationMarker | ((code_point >> (2U * kContinuationWidth)) & kContinuationMask));
        append(kContinuationMarker | ((code_point >> kContinuationWidth) & kContinuationMask));
        append(kContinuationMarker | (code_point & kContinuationMask));
    }
}

}  // namespace

auto StructureParserBase::Parse() noexcept -> Result<void>
{
    // Skip all whitespace to so that we will detect empty documents immediately.

    ParserResult result{MakeResult(this->GetJsonOps().SkipWhitespace(),
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
        .and_then([this](void) noexcept {
            return this->GetState().AddValue();
        })
        .and_then([this](void) noexcept {
            return this->OnNull();
        });
}

/*!
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
        .and_then([this](void) noexcept {
            return this->GetState().AddValue();
        })
        .and_then([this](void) noexcept {
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
        .and_then([this](void) noexcept {
            return this->GetState().AddValue();
        })
        .and_then([this](void) noexcept {
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
        .and_then([this, cur](void) noexcept {
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
        result = this->GetState().AddKey().and_then([this, &string](void) noexcept {
            const std::string_view view{string};
            this->GetJsonDocument().StoreCurrentKey(view);

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
                      .and_then([this, &buffer](const char escaped) noexcept {
                          return this->Unescape(escaped, buffer);
                      })
                      .and_then([&reader]() noexcept {
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
 * - If a unicode escape is encountered:
 *   - Decode the code point and append it UTF-8 encoded.
 * - Otherwise:
 *   - Unescape the single character and append it.
 * \endinternal
 */
auto StructureParserBase::Unescape(const char escaped, String& buffer) noexcept -> Result<void>
{
    Result<void> ret_val{};
    if (std::char_traits<char>::to_int_type(escaped) == std::char_traits<char>::to_int_type('u'))
    {
        ret_val = this->UnescapeCodePoint().transform([&buffer](const std::uint32_t code_point) noexcept {
            // UnescapeCodePoint rejects unpaired surrogates, so the code point is always encodable.
            AppendUtf8(code_point, buffer);
        });
    }
    else
    {
        ret_val = UnescapeChar(escaped).transform([&buffer](const char unescaped) noexcept {
            buffer.push_back(unescaped);
        });
    }
    return ret_val;
}

/*!
 * \internal
 * - Read the escaped UTF-16 code unit.
 * - If it is a high surrogate:
 *   - Read the low surrogate that must follow and combine both into a single code point.
 * - If it is a low surrogate:
 *   - Return an error, because the high surrogate is missing.
 * - Otherwise:
 *   - The code unit already is the code point.
 * \endinternal
 */
auto StructureParserBase::UnescapeCodePoint() noexcept -> Result<std::uint32_t>
{
    Result<std::uint32_t> ret_val{this->ReadCodeUnit()};

    if (ret_val.has_value())
    {
        const std::uint32_t code_unit{ret_val.value()};
        if (IsHighSurrogate(code_unit))
        {
            ret_val = this->ReadLowSurrogate().transform([code_unit](const std::uint32_t low_surrogate) noexcept {
                return CombineSurrogatePair(code_unit, low_surrogate);
            });
        }
        else if (IsLowSurrogate(code_unit))
        {
            ret_val = MakeErrorResult<std::uint32_t>(
                JsonErrc::kInvalidJson, "Unicode escape: Low surrogate is not preceded by a high surrogate!");
        }
        else
        {
            // The code unit is no surrogate, hence it already is the code point.
        }
    }

    return ret_val;
}

/*!
 * \internal
 * - Require a second unicode escape and read its code unit.
 * - If it is no low surrogate, return an error.
 * \endinternal
 */
auto StructureParserBase::ReadLowSurrogate() noexcept -> Result<std::uint32_t>
{
    Result<std::uint32_t> ret_val{MakeErrorResult<std::uint32_t>(JsonErrc::kInvalidJson, kMissingLowSurrogate)};

    if (this->GetJsonOps().Skip('\\') && this->GetJsonOps().Skip('u'))
    {
        ret_val = this->ReadCodeUnit();
        if (ret_val.has_value() && (!IsLowSurrogate(ret_val.value())))
        {
            ret_val = MakeErrorResult<std::uint32_t>(JsonErrc::kInvalidJson, kMissingLowSurrogate);
        }
    }

    return ret_val;
}

/*!
 * \internal
 * - Take four characters and accumulate their hexadecimal values.
 * - If any of them is no hexadecimal digit or the document ends, return an error.
 * \endinternal
 */
auto StructureParserBase::ReadCodeUnit() noexcept -> Result<std::uint32_t>
{
    std::uint32_t code_unit{0U};
    bool valid{true};
    Result<std::uint32_t> ret_val{
        MakeErrorResult<std::uint32_t>(JsonErrc::kInvalidJson, "Unicode escape: Expected a hexadecimal digit!")};

    for (std::size_t index{0U}; (index < kHexDigitsPerEscape) && valid; ++index)
    {
        const Result<char> taken{this->GetJsonOps().TryTake()};
        if (!taken.has_value())
        {
            ret_val = MakeErrorResult<std::uint32_t>(
                JsonErrc::kInvalidJson, "Unicode escape: Unexpected end of input while reading hexadecimal digits!");
            valid = false;
        }
        else
        {
            const Optional<std::uint32_t> digit{HexDigitValue(taken.value())};
            if (digit.has_value())
            {
                code_unit = (code_unit << kHexDigitWidth) | digit.value();
            }
            else
            {
                // ret_val already holds the non-hexadecimal digit error.
                valid = false;
            }
        }
    }

    if (valid)
    {
        ret_val = Result<std::uint32_t>{code_unit};
    }
    return ret_val;
}

/*!
 * \internal
 * - transform all characters to the appropriate escape sequence.
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
    return this->GetState().AddObject().and_then([this]() noexcept {
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

        [this](void) noexcept {
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
        .transform([](void) noexcept {
            return ParserState::kRunning;
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
