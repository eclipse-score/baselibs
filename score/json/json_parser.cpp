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

#include "score/json/json_parser.h"

#include "score/json/internal/parser/vajson/vajson_impl/reader.h"

#include <iostream>
#include <stack>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

namespace score
{
namespace json
{
namespace
{

/// @brief Builds the tree of data elements from the events reported by the vaJson parser.
class AnyTreeBuilder final : private score::json::vajson::v2::Parser
{
  public:
    using score::json::vajson::v2::Parser::Parser;

    /// @brief Parses the data given to the constructor
    /// @return Any as root of the tree, error on error
    auto GetData() noexcept -> score::Result<Any>;

  private:
    template <typename T>
    auto Store(T&& value) noexcept -> score::Result<score::json::Any*>
    {
        if (hierarchy_.empty())
        {
            root_ = std::forward<T>(value);
            return &root_;
        }

        Any* current_node = hierarchy_.top();
        auto current_node_list = current_node->As<score::json::List>();
        if (current_node_list.has_value())
        {
            // False positive, we are not forwarding a value to other functions.
            // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
            auto& current_node_list_ref = current_node_list->get();
            return &(current_node_list_ref.emplace_back(std::forward<T>(value)));
        }

        if (last_key_.empty())  // LCOV_EXCL_BR_LINE (see comment below)
        {
            // This case never happens as the underlying parser aborts before trying to store a value without
            // a key. Nonetheless, we handle this gracefully should the underlying implementation change.
            return MakeUnexpected(Error::kParsingError); /* LCOV_EXCL_LINE */
        }

        auto node_object = current_node->As<score::json::Object>();
        if (!node_object.has_value())  // LCOV_EXCL_BR_LINE (see comment below)
        {
            // This case in production can't happen.
            // the hierarchy_ stack is filled via StartContainer()
            // StartContainer is only called with score::json::List and score::json::Object
            // Thus the hiearchy only holds such objects.
            // score::json::List is handled in the branch above. Thus in the current branch
            // the node can only be of type score::json::Object
            return MakeUnexpected(Error::kParsingError); /* LCOV_EXCL_LINE */
        }
        // False positive, all values passed as rvalue.
        // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
        auto it = node_object->get().emplace(std::piecewise_construct_t(std::piecewise_construct),
                                             std::forward_as_tuple(std::string{last_key_}),
                                             std::forward_as_tuple(std::forward<T>(value)));
        return &((*(it.first)).second);
    }

    /// @brief Updates the state of the parser to store any newly parsed data in the provided container
    template <typename T>
    auto StartContainer(T&& value) noexcept -> score::json::vajson::ParserResult;

    /// @brief Updates the state of the parser to store any newly parsed data in the previous container
    auto EndContainer() noexcept -> score::json::vajson::ParserResult;

    auto OnNull() noexcept -> score::json::vajson::ParserResult override;
    auto OnBool(bool value) noexcept -> score::json::vajson::ParserResult override;
    auto OnNumber(score::json::vajson::JsonNumber value) noexcept -> score::json::vajson::ParserResult override;
    auto OnString(std::string_view value) noexcept -> score::json::vajson::ParserResult override;
    auto OnKey(std::string_view key) noexcept -> score::json::vajson::ParserResult override;
    auto OnStartObject() noexcept -> score::json::vajson::ParserResult override;
    auto OnEndObject(std::size_t) noexcept -> score::json::vajson::ParserResult override;
    auto OnStartArray() noexcept -> score::json::vajson::ParserResult override;
    auto OnEndArray(std::size_t) noexcept -> score::json::vajson::ParserResult override;
    auto OnUnexpectedEvent() noexcept -> score::json::vajson::ParserResult override;

    template <typename... NumberType>
    // NOLINTNEXTLINE(bugprone-exception-escape) an exception terminates, as required by the safety concept
    auto OnNumber(score::json::vajson::JsonNumber& value) noexcept -> score::json::vajson::ParserResult
    {
        auto TryStoreResult = [this](auto result) -> bool {
            return (result.has_value() && Store(score::json::Number{result.value()}).has_value());
        };

        // Coverity warning: The operands of a logical && or || shall be parenthesized if the operands contain binary
        // operators. Fold expression automatically sets parenthesis when expression is unfolded. In this case we are
        // using Unary right fold '(E op ...) becomes (E1 op (... op (En-1 op En)))'. That means for OnNumber<int,
        // float, double>(value) fold expression will be: ( (TryStoreResult(value.As<int>())) || (
        // (TryStoreResult(value.As<float>())) || (TryStoreResult(value.As<double>())) ) )
        // coverity[autosar_cpp14_a5_2_6_violation]
        return ((TryStoreResult(value.As<NumberType>())) || ...)
                   ? score::json::vajson::ParserState::kRunning
                   : score::json::vajson::MakeErrorResult<score::json::vajson::ParserState>(
                         score::json::vajson::JsonErrc::kInvalidJson);
    }

    /// @brief Stores the most recently parsed object key.
    std::string last_key_{};
    /// @brief Tracks the hierarchy of containers currently being parsed.
    std::stack<Any*> hierarchy_{};
    /// @brief Stores the root of the parsed JSON tree.
    Any root_{};
};

auto AnyTreeBuilder::GetData() noexcept -> score::Result<score::json::Any>
{
    const auto parse_result = Parse();
    score::Result<score::json::Any> result{};
    if (parse_result.has_value())
    {
        std::swap(*result, root_);
    }
    else
    {
        result = MakeUnexpected(Error::kParsingError);
    }
    return result;
}

auto AnyTreeBuilder::OnNull() noexcept -> score::json::vajson::ParserResult
{
    auto result = Store(Null{});
    if (!result.has_value())  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from
                              // coverage report. See comment below)
    {
        // Coverage: Not reachable. Line excluded from coverage report.
        // This case never happens, because condition can't be fulfilled due to Store implementation
        return score::json::vajson::MakeErrorResult<score::json::vajson::ParserState>(
            score::json::vajson::JsonErrc::kInvalidJson); /* LCOV_EXCL_LINE */
    }
    return score::json::vajson::ParserState::kRunning;
}

auto AnyTreeBuilder::OnBool(bool value) noexcept -> score::json::vajson::ParserResult
{
    auto result = Store(value);
    if (!result.has_value())  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from
                              // coverage report. See comment below)
    {
        // Coverage: Not reachable. Line excluded from coverage report.
        // This case never happens, because condition can't be fulfilled due to Store implementation
        return score::json::vajson::MakeErrorResult<score::json::vajson::ParserState>(
            score::json::vajson::JsonErrc::kInvalidJson); /* LCOV_EXCL_LINE */
    }
    return score::json::vajson::ParserState::kRunning;
}

auto AnyTreeBuilder::OnNumber(score::json::vajson::JsonNumber value) noexcept -> score::json::vajson::ParserResult
{
    // The parser shall try first the unsigned types from smallest to largest.
    // Then it shall try the signed types from smallest to largest.
    // Finally it will try double.
    // This way the model can assume that the number type is the "smallest possible type" except from floating-point
    // numbers, they shall be always presented as a 'double'
    return OnNumber<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, double>(value);
}

auto AnyTreeBuilder::OnString(std::string_view value) noexcept -> score::json::vajson::ParserResult
{
    auto result = Store(std::string(value));
    if (!result.has_value())  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from
                              // coverage report. See comment below)
    {
        // Coverage: Not reachable. Line excluded from coverage report.
        // This case never happens, because condition can't be fulfilled due to Store implementation
        return score::json::vajson::MakeErrorResult<score::json::vajson::ParserState>(
            score::json::vajson::JsonErrc::kInvalidJson); /* LCOV_EXCL_LINE */
    }
    return score::json::vajson::ParserState::kRunning;
}

auto AnyTreeBuilder::OnKey(std::string_view key) noexcept -> score::json::vajson::ParserResult
{
    last_key_ = std::string(key);
    return score::json::vajson::ParserState::kRunning;
}

template <typename T>
// Suppress "AUTOSAR C++14 A15-5-3" rule finding. This rule states: "The std::terminate() function shall
// not be called implicitly". Since result.has_value() is checked before calling *result,
// std::bad_optional_access should never be thrown. This is false positive.
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
auto AnyTreeBuilder::StartContainer(T&& value) noexcept -> score::json::vajson::ParserResult
{
    auto result = Store(std::forward<T>(value));
    if (!result.has_value())  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from
                              // coverage report. See comment below)
    {
        // Coverage: Not reachable. Line excluded from coverage report.
        // This case never happens, because condition can't be fulfilled due to Store implementation
        return score::json::vajson::MakeErrorResult<score::json::vajson::ParserState>(
            score::json::vajson::JsonErrc::kInvalidJson); /* LCOV_EXCL_LINE */
    }

    // Justification for autosar_cpp14_a18_9_2: Using std::move or std::forward on a raw pointer does not provide
    // any benefit.
    // Justification for autosar_cpp14_a15_5_3: result will never be empty, thus *result will never
    // throw std::bad_optional_access.
    // coverity[autosar_cpp14_a18_9_2_violation]
    // coverity[autosar_cpp14_a15_5_3_violation : FALSE]
    hierarchy_.push(*result);
    return score::json::vajson::ParserState::kRunning;
}

auto AnyTreeBuilder::EndContainer() noexcept -> score::json::vajson::ParserResult
{
    if (!hierarchy_.empty())  // Check if the stack is not empty
    {
        hierarchy_.pop();
    }
    return score::json::vajson::ParserState::kRunning;
}

auto AnyTreeBuilder::OnStartObject() noexcept -> score::json::vajson::ParserResult
{
    return StartContainer(score::json::Object{});
}

auto AnyTreeBuilder::OnEndObject(std::size_t) noexcept -> score::json::vajson::ParserResult
{
    return EndContainer();
}

auto AnyTreeBuilder::OnStartArray() noexcept -> score::json::vajson::ParserResult
{
    return StartContainer(score::json::List{});
}

auto AnyTreeBuilder::OnEndArray(std::size_t) noexcept -> score::json::vajson::ParserResult
{
    return EndContainer();
}

auto AnyTreeBuilder::OnUnexpectedEvent() noexcept -> score::json::vajson::ParserResult
{
    // A call to this function means we have not implemented one of the necessary callbacks.
    // We set this equal to a failed user validation, since a missing callback means we do not support this type.
    return score::json::vajson::MakeErrorResult<score::json::vajson::ParserState>(
        score::json::vajson::JsonErrc::kUserValidationFailed);
}

}  // namespace

// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
// NOLINTNEXTLINE(bugprone-exception-escape) an exception terminates, as required by the safety concept
auto JsonParser::FromFile(const std::string_view file_path) const noexcept -> score::Result<Any>
{
    score::Result<Any> result = MakeUnexpected(Error::kParsingError);
    // NOLINTNEXTLINE(score-banned-function) Tolerated because JsonParser::FromFile is also on the banned function list
    auto json_data = score::json::vajson::JsonData::FromFile(file_path);
    if (json_data.has_value())
    {
        score::Result<Any> json_object = AnyTreeBuilder{json_data.value()}.GetData();
        if (json_object.has_value())
        {
            result = std::move(*json_object);
        }
    }
    return result;
}

// NOLINTNEXTLINE(bugprone-exception-escape) an exception terminates, as required by the safety concept
auto JsonParser::FromBuffer(const std::string_view buffer) const noexcept -> score::Result<Any>
{
    score::Result<Any> result = MakeUnexpected(Error::kParsingError);
    auto json_data = score::json::vajson::JsonData::FromBuffer(std::string_view{buffer.data(), buffer.size()});
    if (json_data.has_value())  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from coverage
                                // report.)
    // (else branch can't be hit due to internal implementation of JsonData::FromBuffer function that accepts all kind
    // of strings.)
    {
        auto json_object = AnyTreeBuilder{json_data.value()}.GetData();
        if (json_object.has_value())
        {
            result = std::move(*json_object);
        }
    }
    return result;
}

// False positive, user defined literal operator is used to perform conversion.
// coverity[autosar_cpp14_a13_1_3_violation : FALSE]
auto operator""_json(const char* const data, const size_t size) -> Any
{
    const JsonParser json_parser_obj;
    auto result = json_parser_obj.FromBuffer(std::string_view{data, size});
    if (!result.has_value())
    {
        // False positive, no function call.
        //  coverity[autosar_cpp14_m8_4_4_violation : FALSE]
        std::cerr << "JSON could not be parsed!" << std::endl;
        std::terminate();
    }
    return std::move(result).value();
}

}  // namespace json
}  // namespace score
