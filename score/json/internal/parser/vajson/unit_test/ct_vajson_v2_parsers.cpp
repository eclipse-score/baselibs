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
 *        \brief  Contains component tests for VaJson V2 Parsers.
 *
 *
 *********************************************************************************************************************/
/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <array>
#include <cmath>
#include <functional>
#include <string>
#include <string_view>

#include "gtest/gtest.h"

#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/composition_parser_impl.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/v2/parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/v2/single_array_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/v2/single_object_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/number.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"
#include "score/result/result.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{
namespace
{

template <typename T>
using optional = score::cpp::optional<T>;

using Strings = std::vector<std::string>;
using Key = std::string;
using Keys = std::vector<Key>;
/*!
 * \brief           A parser that accepts anything that is a valid JSON file without schema validation
 */
struct AnyParser final : public v2::Parser
{
    using v2::Parser::Parser;

    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }
};

/*!
 * \brief           Parser that is only interested in strings
 */
struct StringParser final : public v2::Parser
{
    /*!
     * \brief           Default Constructor
     */
    explicit StringParser(JsonData& doc) : v2::Parser(doc) {}

    /*!
     * \brief           event call for the json file reader
     * \param[in]       str
     *                  std::string found in the json file
     * \return          bool the status of the event call used by the reader class to make sure that the reading process
     *                  is succeeded
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    ParserResult OnString(std::string_view str) noexcept final
    {
        this->strings.push_back(std::string{str});

        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }

    /*!
     * \brief           std::string contain the parsed value
     */
    Strings strings{};
};

/*!
 * \brief           Parser that is only interested in Objects
 */
class ObjectParser final : public v2::Parser
{
  public:
    /*!
     * \brief           Constructor from file reference
     */
    explicit ObjectParser(JsonData& doc) : v2::Parser(doc), start_(0), end_(0), balance_(0) {}

    /*!
     * \brief           Count the number of start of object calls
     * \return          Running
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    ParserResult OnStartObject() noexcept final
    {
        this->start_ += 1;
        this->balance_ += 1;
        return ParserState::kRunning;
    }

    /*!
     * \brief           Count the number of end of object calls
     * \return          Running
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    ParserResult OnEndObject(std::size_t) noexcept final
    {
        ParserResult result{ParserState::kRunning};
        this->end_ += 1;
        if (this->balance_ == 0)
        {
            result = score::MakeUnexpected(JsonErrc::kUserValidationFailed, "EndObject called too often");
        }
        this->balance_ -= 1;
        return result;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }

    /*!
     * \brief           number of start events
     */
    std::streamsize start_{};

    /*!
     * \brief           number of end events
     */
    std::streamsize end_{};

    /*!
     * \brief           balance of events
     */
    std::streamsize balance_{};
};

/*!
 * \brief           Parser that is only interested in numbers
 */
class NumberParser final : public v2::Parser
{
  public:
    /*!
     * \brief           Default Constructor
     */
    explicit NumberParser(JsonData& doc) : v2::Parser(doc) {}

    /*!
     * \brief           Count the number of numbers
     * \details         on each number event, we categorize and Count events
     * \param[in]       n
     *                  value of the number JSON value
     * \return          Running
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    ParserResult OnNumber(JsonNumber) noexcept final
    {
        this->number_ += 1;

        return ParserState::kRunning;
    }

    ParserResult OnUnexpectedEvent() noexcept final
    {
        return ParserState::kRunning;
    }

    /*!
     * \brief           number of total number events
     */
    std::size_t number_{0U};
};

/*!
 * \brief           Parser that tries to convert into a integer type, floating type & returns the raw value
 * \tparam          IntType
 *                  The integer type to convert into
 * \tparam          FloatType
 *                  The floating type to convert into
 */
template <typename IntType, typename FloatType>
class CustomNumberParser final : public v2::Parser
{
  public:
    using v2::Parser::Parser;

    auto OnNumber(JsonNumber n) noexcept -> ParserResult final
    {
        this->integer_ = n.As<IntType>();
        this->float_ = n.As<FloatType>();

        this->raw_ = n.Convert([](std::string_view sv) noexcept {
            return sv;
        });
        return ParserState::kRunning;
    }

    auto GetInteger() noexcept -> optional<IntType>
    {
        return this->integer_;
    }
    auto GetFloat() noexcept -> optional<FloatType>
    {
        return this->float_;
    }

    auto Raw() noexcept -> std::string_view
    {
        return this->raw_;
    }

    optional<IntType> integer_{};
    optional<FloatType> float_{};
    std::string_view raw_;
};

/*!
 * \brief           Parser that is only interested in nulls
 */
class NullParser final : public v2::Parser
{
  public:
    /*!
     * \brief           Constructor from file reference
     */
    explicit NullParser(JsonData& doc) : v2::Parser(doc), null_(0) {}

    /*!
     * \brief           Count the number of null calls
     * \return          Running
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    ParserResult OnNull() noexcept final
    {
        this->null_ += 1;
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }

    /*!
     * \brief           number of null events
     */
    std::size_t null_{};
};

/*!
 * \brief           Parser that is only interested in Keys
 */
struct KeyParser final : public v2::Parser
{
    /*!
     * \brief           Default Constructor
     */
    explicit KeyParser(JsonData& doc) : v2::Parser(doc) {}

    /*!
     * \brief           event call for the json file reader
     * \param[in]       Key
     *                  Key found in the json file
     * \return          bool the status of the event call used by the reader class to make sure that the reading process
     *                  is succeeded
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    ParserResult OnKey(std::string_view key) noexcept final
    {
        this->keys.push_back(std::string{key});

        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }

    /*!
     * \brief           string contain the parsed value
     */
    Keys keys;
};

/*!
 * \brief           A parser that accepts anything that is a valid JSON file without schema validation
 */
struct InvalidDetectionParser final : public v2::Parser
{
    using Parser::Parser;

    /*!
     * \brief           Always return KRunning, because we want the parser to fail because of invalid JSON, not
     * unexpected events \details         The Counter is increased to simulate the modification of class data members
     */
    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        counter++;
        return ParserState::kRunning;
    }

    /*!
     * \brief           A counter that counts the number of calls
     */
    std::size_t counter{0U};
};

/*!
 * \brief           SingleArrayParser for the parameterized tests
 */
class TestArrayParser final : public v2::SingleArrayParser
{
  public:
    /*!
     * \brief           Constructor
     */
    using SingleArrayParser::SingleArrayParser;

    /*!
     * \brief           Event call for an array element
     * \return          ParserResult to indicate that the parsing handler executed successfully.
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    auto OnElement() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }
};

/*!
 * \brief           SingleObjectParser for the parameterized tests
 */
class TestObjectParser : public v2::SingleObjectParser
{
  public:
    /*!
     * \brief           Constructor
     */
    using SingleObjectParser::SingleObjectParser;

    /*!
     * \brief           Event call for a Key
     * \return          ParserResult to indicate that the parsing handler executed successfully.
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    auto OnKey(std::string_view) noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }
};

/*!
 * \brief           NestedObjectParser for the parameterized tests
 */
class NestedObjectParser final : public v2::Parser
{
  public:
    /*!
     * \brief           Constructor
     * \param[in]       doc
     *                  The document to parse.
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    explicit NestedObjectParser(JsonData& doc) : Parser{doc}, doc_{doc} {}

    /*!
     * \brief           Event call for StartObject
     * \return          KRunning to continue parsing.
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    auto OnStartObject() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }

    /*!
     * \brief           Event call for EndObject
     * \return          The Result of calling a SingleObjectParser twice.
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    auto OnEndObject(std::size_t) noexcept -> ParserResult final
    {
        TestObjectParser object_parser{this->doc_};
        return object_parser.Parse().and_then([&object_parser]() noexcept {
            return object_parser.SubParse();
        });
    }

    /*!
     * \brief           Event call for unexpected events
     * \return          KRunning to continue parsing.
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }

  private:
    /*!
     * \brief           The document to parse
     */
    JsonData& doc_;
};

/*!
 * \brief           CommaArrayParser for the parameterized tests
 */
class CommaArrayParser final : public v2::Parser
{
  public:
    /*!
     * \brief           Constructor
     * \param[in]       doc
     *                  The document to parse.
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    explicit CommaArrayParser(JsonData& doc) : Parser{doc}, doc_{doc} {}

    /*!
     * \brief           Event call for StartObject
     * \return          KRunning to continue parsing.
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    auto OnStartObject() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }

    /*!
     * \brief           Event call for StartObject
     * \return          KRunning to continue parsing.
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    auto OnKey(std::string_view) noexcept -> ParserResult final
    {
        return TestArrayParser{this->doc_}.SubParse();
    }

  private:
    /*!
     * \brief           The document to parse
     */
    JsonData& doc_;
};

class CustomStringParser : public v2::Parser
{
  public:
    using v2::Parser::Parser;

    auto OnString(std::string_view) noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }
};

/*!
 * \brief           Parser that is only interested in bools
 */
class BoolParser final : public v2::Parser
{
  public:
    /*!
     * \brief           Construct from document reference
     */
    explicit BoolParser(JsonData& doc) : v2::Parser(doc) {}

    /*!
     * \brief           Count the number of true respectively false values
     * \details         on each boolean event, we increment the true or false counter
     * \param[in]       b
     *                  value of the boolean JSON value
     * \return          Running
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    ParserResult OnBool(bool b) noexcept final
    {
        if (b)
        {
            this->true_ += 1;
        }
        else
        {
            this->false_ += 1;
        }
        return ParserState::kRunning;
    }

    ParserResult OnUnexpectedEvent() noexcept final
    {
        return ParserState::kRunning;
    }

    /*!
     * \brief           number of true events
     */
    std::size_t true_{0U};
    /*!
     * \brief           number of false events
     */
    std::size_t false_{0U};
};

/*!
 * \brief           Parser that is only interested in Arrays
 */
class ArrayParser final : public v2::Parser
{
  public:
    /*!
     * \brief           Constructor from file reference
     */
    explicit ArrayParser(JsonData& doc) : v2::Parser(doc), start_(0), end_(0), balance_(0) {}

    /*!
     * \brief           Count the number of start of array calls
     * \return          Running
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    ParserResult OnStartArray() noexcept final
    {
        this->start_ += 1;
        this->balance_ += 1;
        return ParserState::kRunning;
    }

    /*!
     * \brief           Count the number of end of array calls
     * \return          Running
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    ParserResult OnEndArray(std::size_t) noexcept final
    {
        ParserResult result{ParserState::kRunning};
        this->end_ += 1;
        if (this->balance_ == 0)
        {
            result = score::MakeUnexpected(JsonErrc::kUserValidationFailed, "EndArray called too often");
        }
        this->balance_ -= 1;
        return result;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }

    /*!
     * \brief           number of start events
     */
    std::streamsize start_{};

    /*!
     * \brief           number of end events
     */
    std::streamsize end_{};

    /*!
     * \brief           balance of events
     */
    std::streamsize balance_{};
};

class ElementCounterParser final : public v2::Parser
{
  public:
    std::size_t count_{0U};

    using v2::Parser::Parser;

    auto OnEndArray(std::size_t size) noexcept -> ParserResult final
    {
        this->count_ = size;
        return ParserState::kRunning;
    }

    auto OnEndObject(std::size_t size) noexcept -> ParserResult final
    {
        this->count_ = size;
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return ParserState::kRunning;
    }
};

/*!
 * \brief           Parser to test protected methods
 */
class VirtualParserChild final : public v2::Parser
{
  public:
    using v2::Parser::Parser;

    auto GetKey() const noexcept -> CStringView
    {
        return v2::Parser::GetCurrentKey();
    }

    /// Capture the key when OnKey is triggered by Parse(), then accept any value type.
    auto OnKey(StringView) noexcept -> ParserResult override
    {
        this->captured_key_ = GetKey();
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    CStringView captured_key_{""};
};

// ---------------------------------------------------------------------------
// ParserFails parametrized test
// ---------------------------------------------------------------------------

/// Holds a parametrized parser-fail test case.
struct ParserInput
{
    std::function<score::Result<void>(std::string_view)> parser_call;
    std::string_view input;
    std::string_view message;
    JsonErrc error;
    std::string name;
};

/// Call to the BoolParser
std::function<score::Result<void>(std::string_view)> BoolParserCall{
    [](std::string_view input) noexcept -> score::Result<void> {
        auto doc = JsonData::FromBuffer(input);
        if (!doc.has_value())
        {
            return score::Result<void>{score::unexpect, doc.error()};
        }
        internal::BoolParser parser{doc.value(), [](bool) noexcept -> score::Result<void> {
                                        return score::Result<void>{};
                                    }};
        return parser.Parse();
    }};

/// Call to the KeyParser
std::function<score::Result<void>(std::string_view)> KeyParserCall{
    [](std::string_view input) noexcept -> score::Result<void> {
        auto doc = JsonData::FromBuffer(input);
        if (!doc.has_value())
        {
            return score::Result<void>{score::unexpect, doc.error()};
        }
        internal::KeyParser parser{doc.value(), [](StringView) noexcept -> score::Result<void> {
                                       return score::Result<void>{};
                                   }};
        return parser.Parse();
    }};

/// Call to the NumberParser
std::function<score::Result<void>(std::string_view)> NumberParserCall{
    [](std::string_view input) noexcept -> score::Result<void> {
        auto doc = JsonData::FromBuffer(input);
        if (!doc.has_value())
        {
            return score::Result<void>{score::unexpect, doc.error()};
        }
        internal::NumberParser<std::uint8_t> parser{doc.value(), [](std::uint8_t) noexcept -> score::Result<void> {
                                                        return score::Result<void>{};
                                                    }};
        return parser.Parse();
    }};

/// Call to the StringParser
std::function<score::Result<void>(std::string_view)> StringParserCall{
    [](std::string_view input) noexcept -> score::Result<void> {
        auto doc = JsonData::FromBuffer(input);
        if (!doc.has_value())
        {
            return score::Result<void>{score::unexpect, doc.error()};
        }
        internal::StringParser parser{doc.value(), [](StringView) noexcept -> score::Result<void> {
                                          return score::Result<void>{};
                                      }};
        return parser.Parse();
    }};

/// Call to the SingleArrayParser (test subclass)
std::function<score::Result<void>(std::string_view)> SingleArrayParserCall{
    [](std::string_view input) noexcept -> score::Result<void> {
        auto doc = JsonData::FromBuffer(input);
        if (!doc.has_value())
        {
            return score::Result<void>{score::unexpect, doc.error()};
        }
        TestArrayParser parser{doc.value()};
        return parser.Parse();
    }};

/// Call to the NestedObjectParser
std::function<score::Result<void>(std::string_view)> NestedObjectParserCall{
    [](std::string_view input) noexcept -> score::Result<void> {
        auto doc = JsonData::FromBuffer(input);
        if (!doc.has_value())
        {
            return score::Result<void>{score::unexpect, doc.error()};
        }
        NestedObjectParser parser{doc.value()};
        return parser.Parse();
    }};

/// Call to an CommaArrayParser to test structural comma errors
std::function<score::Result<void>(std::string_view)> CommaArrayParserCall{
    [](std::string_view input) noexcept -> score::Result<void> {
        auto doc = JsonData::FromBuffer(input);
        if (!doc.has_value())
        {
            return score::Result<void>{score::unexpect, doc.error()};
        }
        CommaArrayParser parser{doc.value()};
        return parser.Parse();
    }};

/// Call to the SingleObjectParser (test subclass)
std::function<score::Result<void>(std::string_view)> SingleObjectParserCall{
    [](std::string_view input) noexcept -> score::Result<void> {
        auto doc = JsonData::FromBuffer(input);
        if (!doc.has_value())
        {
            return score::Result<void>{score::unexpect, doc.error()};
        }
        TestObjectParser parser{doc.value()};
        return parser.Parse();
    }};

/// Call to the CustomStringParser
std::function<score::Result<void>(std::string_view)> CustomStringParserCall{
    [](std::string_view input) noexcept -> score::Result<void> {
        auto doc = JsonData::FromBuffer(input);
        if (!doc.has_value())
        {
            return score::Result<void>{score::unexpect, doc.error()};
        }
        CustomStringParser parser{doc.value()};
        return parser.Parse();
    }};

/// Input for the ParserFails parametrized test
std::vector<ParserInput> ParserV2TestInput{
    {BoolParserCall, R"([ } ])", "Expected to parse a boolean.", JsonErrc::kUserValidationFailed, "BoolParserFails"},
    {KeyParserCall, R"([ } ])", "Expected to parse a key.", JsonErrc::kUserValidationFailed, "KeyParserFails"},
    {NumberParserCall, R"([ } ])", "Expected to parse a number.", JsonErrc::kUserValidationFailed, "NumberParserFails"},
    {StringParserCall, R"([ } ])", "Expected to parse a string.", JsonErrc::kUserValidationFailed, "StringParserFails"},
    {SingleArrayParserCall,
     "42",
     "Expected to parse an array of elements.",
     JsonErrc::kUserValidationFailed,
     "SingleArrayParserFails"},
    {SingleArrayParserCall,
     R"([[)",
     "Did not expect nested elements",
     JsonErrc::kUserValidationFailed,
     "SingleArrayParserFailsOnNesting"},
    {CommaArrayParserCall,
     R"({"key":,[]})",
     "StructureParser::ParseComma: Unexpected comma.",
     JsonErrc::kInvalidJson,
     "SingleArrayParserFailsOnComma"},
    {SingleObjectParserCall,
     "42",
     "Expected to parse an object of elements.",
     JsonErrc::kUserValidationFailed,
     "SingleObjectParserFails"},
    {SingleObjectParserCall,
     R"({"key" : { "key2": {)",
     "Did not expect nested elements",
     JsonErrc::kUserValidationFailed,
     "NestedObjectParserEnter"},
    {NestedObjectParserCall,
     R"({"key":{}})",
     "Cannot leave level",
     JsonErrc::kUserValidationFailed,
     "NestedObjectParserLeave"},
    {CustomStringParserCall,
     R"(,)",
     "StructureParser::ParseComma: Unexpected comma.",
     JsonErrc::kInvalidJson,
     "CommaOnly"},
    {CustomStringParserCall,
     R"("aaa" {)",
     "DepthCounter::AddElement: Multiple top level elements.",
     JsonErrc::kInvalidJson,
     "MultipleTopLevelElements"},
};

// ---------------------------------------------------------------------------
// Invalid JSON test data (subset of representative cases)
// ---------------------------------------------------------------------------

struct InvalidJsonCase
{
    std::string_view json;
    std::string name;
};

std::vector<InvalidJsonCase> InvalidJsonCases{
    // Empty document
    {"", "kEmptyDocument"},
    // Malformed keywords - wrong length
    {"tru", "kUnknownType1"},
    {"fals", "kUnknownType2"},
    {"truee", "kUnknownType3"},
    {"falsee", "kUnknownType4"},
    {"nul", "kUnknownType5"},
    // Malformed keywords - wrong case
    {"False", "kUnknownType6"},
    {"True", "kUnknownType7"},
    {"Null", "kUnknownType8"},
    // Malformed keywords - correct length/case but wrong characters
    {"nxyl", "kUnknownType9"},
    {"n00l", "kUnknownType10"},
    {"txye", "kUnknownType11"},
    {"t12e", "kUnknownType12"},
    {"fxyze", "kUnknownType13"},
    {"f456e", "kUnknownType14"},
    // Invalid JSON sequences
    {R"({ "key": , "another_key": )", "kKeyWithoutValue"},
    {R"({ "key": "value", "value", "key": 1 })", "kValueWithoutKey"},
    {R"({"key")", "kEofAfterKey"},
    {R"({ "key": } )", "kUnexpectedObjectClose1"},
    {R"([ } ])", "kUnexpectedObjectClose2"},
    {R"({ [ })", "kUnexpectedArrayOpen"},
    {R"({ ] })", "kUnexpectedArrayClose"},
    {R"("This goes on until the end of file)", "kNonterminatingString1"},
    {R"({ "this is a key: 1, "k": 1 } )", "kNonterminatingString2"},
    {R"(-)", "kNonterminatingNumber1"},
    {R"(000)", "kNonterminatingNumber2"},
    {R"(0a)", "kNonterminatingNumber3"},
    {R"(0z)", "kNonterminatingNumber4"},
    {"{", "kNonTerminatingObject"},
    {"[", "kNonTerminatingArray"},
    {R"("key": 2)", "kTopLevelKeys"},
    {R"(2, 3, 4)", "kMultipleTopLevelElements"},
    {R"({}{})", "kMultipleTopLevelObjects"},
    {R"(2 3 4)", "kMultipleValues"},
    {R"("test\u1234")", "kUnicodeEscape"},
    {R"("test\{1234\}")", "kUnknownEscape"},
    {"", "kNoData"},
    {" \t\n\r", "kOnlyWhiteSpace"},
    {"]", "kOnlyArrayClose"},
    {"}", "kOnlyObjectClose"},
    {"{{", "kObjectOpenInsteadOfKey"},
    {"{[", "kArrayOpenInsteadOfKey"},
    {R"({"true", ""})", "kStringInsteadOfKey"},
    {R"({"true" })", "kValueInsteadOfKey"},
    {R"({1234"true" })", "kNumberAndString"},
    {R"({"key":1 "Key":2})", "kMissingCommaInObject"},
    {R"(["abc" "def"])", "kMissingCommaInArray"},
    {R"({unquoted_key: "keys must be quoted"})", "kUnquotedKey"},
    {R"(["Comma after the close"],)", "kCommaAfterClose"},
    {R"(["Extra close"]])", "kExtraArrayAfterClose"},
    {R"({"Extra value after close": true} "misplaced quoted value")", "kExtraValueAfterClose"},
    {R"({"Illegal expression": 1 + 2})", "kIllegalExpression"},
    {R"({"Illegal invocation": alert()})", "kIllegalInvocation"},
    {R"(["Illegal backslash escape: \x15"])", "kIllegalEscape"},
    {R"([\naked])", "kNaked"},
    {R"(["Illegal backslash escape: \017"])", "kIllegalEscapeNumber"},
    {R"({"Missing colon" null})", "kMissingColon"},
    {R"({"Double colon":: null})", "kDoubleColon"},
    {R"({"Comma instead of colon", null})", "kCommaInsteadOfColon"},
    {R"(["Colon instead of comma": false])", "kColonInsteadOfComma"},
    {R"(["Bad value", truth])", "kBadValue"},
    {R"(['single quote'])", "kSingleQuote"},
    {R"(["tab\   character\   in\  string\  "])", "kBackslashinString"},
    {"[\"line\\\nbreak\"]", "kBackslashLineBreak"},
    {R"({"Comma instead if closing brace": true,)", "kCommaInsteadOfClosing"},
    {R"([{}{}])", "kArrayWithoutComma"},
    {R"([]123)", "kTrailingValue"},
    {R"([]"key":)", "kTrailingKey"},
    // Octal numbers (unsupported notation)
    {"0123", "kOctalNumber1"},
    {"-010", "kOctalNumber2"},
};

}  // namespace

// ---------------------------------------------------------------------------
// Test: CountElements
// ---------------------------------------------------------------------------

/*!
 * Test that the parser correctly counts elements inside objects.
 */
TEST(CT__VaJson__V2__Parsers, CountElements)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({"key":["val"], "key":{"key":"val"}, "key":{}})"});
    ASSERT_TRUE(data.has_value());
    ElementCounterParser parser{data.value()};

    ASSERT_TRUE(parser.Parse().has_value());
    ASSERT_EQ(parser.count_, 3U);
}

// ---------------------------------------------------------------------------
// Test: CountArrayElements
// ---------------------------------------------------------------------------

/*!
 * Test that the parser correctly counts elements inside arrays.
 */
TEST(CT__VaJson__V2__Parsers, CountArrayElements)
{
    auto data = JsonData::FromBuffer(std::string_view{R"([{"key":"val"}, {"key":"val"}, {"key:":"val"}])"});
    ASSERT_TRUE(data.has_value());
    ElementCounterParser parser{data.value()};

    ASSERT_TRUE(parser.Parse().has_value());
    ASSERT_EQ(parser.count_, 3U);
}

// ---------------------------------------------------------------------------
// Test: DetectEmptyDocument
// ---------------------------------------------------------------------------

/*!
 * Test that the JSON parser recognizes empty files.
 */
TEST(CT__VaJson__V2__Parsers, DetectEmptyDocument)
{
    auto doc = JsonData::FromBuffer(std::string_view{""});
    ASSERT_TRUE(doc.has_value());

    InvalidDetectionParser parser{doc.value()};

    const auto result = parser.Parse();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
}

// ---------------------------------------------------------------------------
// Test: DetectInvalidJson (parametrized)
// ---------------------------------------------------------------------------

class CT__VaJson__V2__Parsers__InvalidJson : public ::testing::TestWithParam<InvalidJsonCase>
{
};

/*!
 * Test that JSON documents are checked for validity.
 *
 * - Create JSON Document with a defect.
 * - Parse JSON Document.
 * - Assert that the parser aborts with a non-UserValidationFailed error.
 *
 * \trace           CREQ-Json-Validation
 */
TEST_P(CT__VaJson__V2__Parsers__InvalidJson, DetectInvalidJson)
{
    auto doc = JsonData::FromBuffer(GetParam().json);
    ASSERT_TRUE(doc.has_value());

    InvalidDetectionParser parser{doc.value()};
    const auto result = parser.Parse();
    ASSERT_FALSE(result.has_value()) << "Parser returned successfully for input: " << GetParam().json;
    // kUserValidationFailed should only occur if the JSON doesn't match the user schema,
    // never in case of an invalid JSON structure.
    ASSERT_NE(result.error(), JsonErrc::kUserValidationFailed);
}

INSTANTIATE_TEST_SUITE_P(InvalidJson,
                         CT__VaJson__V2__Parsers__InvalidJson,
                         ::testing::ValuesIn(InvalidJsonCases),
                         [](const testing::TestParamInfo<InvalidJsonCase>& info) noexcept {
                             return info.param.name;
                         });

// ---------------------------------------------------------------------------
// Test: JsonNumber (NumbersCanBeParsedIntoUserdefinedFormats)
// ---------------------------------------------------------------------------

/*!
 * Test that vaJson can convert JSON numbers into user defined formats.
 *
 * \trace           CREQ-Json-ProvidedSerializers
 */
TEST(CT__VaJson__V2__Parsers, NumbersCanBeParsedIntoUserdefinedFormats)
{
    using Num = JsonNumber;

    {
        auto result = Num::New("1");
        ASSERT_TRUE(result.has_value());
        Num number{result.value()};

        EXPECT_TRUE(number.TryAs<std::int8_t>().has_value());
        EXPECT_TRUE(number.As<Num>().has_value());
        EXPECT_EQ(number.As<bool>(), Optional<bool>{true});
        EXPECT_EQ(number.As<std::int8_t>(), Optional<std::int8_t>{std::int8_t{1}});
        EXPECT_EQ(number.As<std::uint64_t>(), Optional<std::uint64_t>{1U});
        StringView num_string{number.Convert([](StringView sv) noexcept {
            return sv;
        })};
        EXPECT_EQ(num_string, "1");
        const auto actual = number.As<float>();
        ASSERT_TRUE(actual.has_value());
        EXPECT_NEAR(actual.value(), 1.f, 1e-6f);
    }

    {
        auto result = Num::New("2.5");
        ASSERT_TRUE(result.has_value());
        Num number{result.value()};

        EXPECT_FALSE(number.TryAs<std::int8_t>().has_value());
        EXPECT_TRUE(number.As<Num>().has_value());
        EXPECT_EQ(number.As<bool>(), Optional<bool>{});
        EXPECT_EQ(number.As<std::int8_t>(), Optional<std::int8_t>{});
        EXPECT_EQ(number.As<std::uint64_t>(), Optional<std::uint64_t>{});
        StringView num_string{number.Convert([](StringView sv) noexcept {
            return sv;
        })};
        EXPECT_EQ(num_string, "2.5");
        const auto actual = number.As<float>();
        ASSERT_TRUE(actual.has_value());
        EXPECT_NEAR(actual.value(), 2.5f, 1e-6f);
    }
}

// ---------------------------------------------------------------------------
// Test: ParseDecimalIntegers (FromBuffer case only)
// ---------------------------------------------------------------------------

/*!
 * Test that vaJson can convert JSON decimal integer numbers into user defined formats.
 *
 * \trace           CREQ-Json-ProvidedSerializers
 */
TEST(CT__VaJson__V2__Parsers, ParseDecimalIntegers)
{
    using Num = JsonNumber;

    {
        auto result = Num::New("12345");
        ASSERT_TRUE(result.has_value());
        Num number{result.value()};
        EXPECT_EQ(number.As<std::int32_t>(), Optional<std::int32_t>{12345});
        ASSERT_TRUE(number.As<double>().has_value());
        EXPECT_DOUBLE_EQ(number.As<double>().value(), 12345.0);
        EXPECT_EQ(number.Convert([](StringView sv) noexcept {
            return sv;
        }),
                  StringView{"12345"});
    }

    {
        auto result = Num::New("-12345");
        ASSERT_TRUE(result.has_value());
        Num number{result.value()};
        EXPECT_EQ(number.As<std::int32_t>(), Optional<std::int32_t>{-12345});
        ASSERT_TRUE(number.As<double>().has_value());
        EXPECT_DOUBLE_EQ(number.As<double>().value(), -12345.0);
    }

    {
        auto result = Num::New("0");
        ASSERT_TRUE(result.has_value());
        Num number{result.value()};
        EXPECT_EQ(number.As<std::int32_t>(), Optional<std::int32_t>{0});
        ASSERT_TRUE(number.As<double>().has_value());
        EXPECT_DOUBLE_EQ(number.As<double>().value(), 0.0);
    }
}

// ---------------------------------------------------------------------------
// Test: ParseHexadecimalIntegers
// ---------------------------------------------------------------------------

/*!
 * Test that vaJson can convert JSON hexadecimal numbers into user defined formats.
 *
 * \trace           CREQ-Json-ProvidedSerializers
 */
TEST(CT__VaJson__V2__Parsers, ParseHexadecimalIntegers)
{
    using Num = JsonNumber;

    auto result = Num::New("0xff");
    ASSERT_TRUE(result.has_value());
    Num number{result.value()};
    EXPECT_EQ(number.As<std::int32_t>(), Optional<std::int32_t>{255});
    ASSERT_TRUE(number.As<double>().has_value());
    EXPECT_DOUBLE_EQ(number.As<double>().value(), 255.0);
    EXPECT_EQ(number.Convert([](StringView sv) noexcept {
        return sv;
    }),
              StringView{"0xff"});
}

// ---------------------------------------------------------------------------
// Test: ParseFloats
// ---------------------------------------------------------------------------

/*!
 * Test that vaJson can convert JSON floating point numbers into user defined formats.
 *
 * \trace           CREQ-Json-ProvidedSerializers
 */
TEST(CT__VaJson__V2__Parsers, ParseFloats)
{
    using Num = JsonNumber;

    {
        auto result = Num::New("123.45");
        ASSERT_TRUE(result.has_value());
        Num number{result.value()};
        EXPECT_EQ(number.As<std::int32_t>(), Optional<std::int32_t>{});
        ASSERT_TRUE(number.As<double>().has_value());
        EXPECT_DOUBLE_EQ(number.As<double>().value(), 123.45);
    }

    {
        auto result = Num::New("-12.345");
        ASSERT_TRUE(result.has_value());
        Num number{result.value()};
        EXPECT_EQ(number.As<std::int32_t>(), Optional<std::int32_t>{});
        ASSERT_TRUE(number.As<double>().has_value());
        EXPECT_DOUBLE_EQ(number.As<double>().value(), -12.345);
    }

    {
        auto result = Num::New("0.1e1");
        ASSERT_TRUE(result.has_value());
        Num number{result.value()};
        EXPECT_EQ(number.As<std::int32_t>(), Optional<std::int32_t>{});
        ASSERT_TRUE(number.As<double>().has_value());
        EXPECT_DOUBLE_EQ(number.As<double>().value(), 1.0);
    }
}

// ---------------------------------------------------------------------------
// Test: ParserFails (parametrized)
// ---------------------------------------------------------------------------

class CT__VaJson__V2__Parsers__Parametrized__InvalidTypes : public ::testing::TestWithParam<ParserInput>
{
};

/*!
 * Test that the called parser fails for the given input sequence.
 */
TEST_P(CT__VaJson__V2__Parsers__Parametrized__InvalidTypes, ParserFails)
{
    const auto result = GetParam().parser_call(GetParam().input);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), GetParam().error);
    EXPECT_EQ(result.error().UserMessage(), GetParam().message);
}

INSTANTIATE_TEST_SUITE_P(InvalidTypes,
                         CT__VaJson__V2__Parsers__Parametrized__InvalidTypes,
                         ::testing::ValuesIn(ParserV2TestInput),
                         [](const testing::TestParamInfo<ParserInput>& currParam) noexcept {
                             return currParam.param.name;
                         });

/*!
 * Test that GetCurrentKey() method works with VirtualParser.
 *
 * \trace           CREQ-Json-Deserialization
 */
TEST(CT__VaJson__V2__Parsers, VirtualParserGetCurrentKey)
{
    const StringView input{R"({"year": 2023})"};
    auto doc = JsonData::FromBuffer(input);
    ASSERT_TRUE(doc.has_value());

    VirtualParserChild parser{doc.value()};
    const auto result = parser.Parse();
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(parser.captured_key_, CStringView{"year"});
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
