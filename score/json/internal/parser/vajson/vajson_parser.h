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
#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_VAJSON_PARSER_H
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_VAJSON_PARSER_H

#include "score/json/internal/model/any.h"
#include "score/result/result.h"

#include "amsr/json/reader.h"

#include <deque>
#include <memory>
#include <string>

namespace score
{
namespace json
{

/// \brief General purpose JSON parser that uses vaJSON from Vector. It abstracts the vendor specific API and returns
/// a custom tree of data elements.
class VajsonParser final : private amsr::json::v2::Parser
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

  private:
    using amsr::json::v2::Parser::Parser;

    auto GetData() noexcept -> score::Result<Any>;

    template <typename T>
    auto Store(T&& value) noexcept -> score::Result<score::json::Any*>
    {
        if (hierarchy_.empty() == true)
        {
            root_ = std::forward<T>(value);
            return &root_;
        }
        else
        {
            Any* current_node = hierarchy_.top();
            auto current_node_list = current_node->As<score::json::List>();
            if (current_node_list.has_value())
            {
                // False positive, we are not forwarding a value to other functions.
                // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
                auto& current_node_list_ref = current_node_list->get();
                return &(current_node_list_ref.emplace_back(std::forward<T>(value)));
            }
            else
            {
                if (!last_key_.empty())  // LCOV_EXCL_BR_LINE (see comment below)
                {
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
                else
                {
                    // This case never happens as the underlying parser aborts before trying to store a value without
                    // a key. Nonetheless, we handle this gracefully should the underlying implementation change.
                    return MakeUnexpected(Error::kParsingError); /* LCOV_EXCL_LINE */
                }
            }
        }
    }

    /// \brief Updates the state of the parser to store any newly parsed data in the provided container
    template <typename T>
    auto StartContainer(T&& value) noexcept -> amsr::json::ParserResult;

    /// \brief Updates the state of the parser to store any newly parsed data in the previous container
    auto EndContainer() noexcept -> amsr::json::ParserResult;

    auto OnNull() noexcept -> amsr::json::ParserResult override;
    auto OnBool(bool value) noexcept -> amsr::json::ParserResult override;
    auto OnNumber(amsr::json::JsonNumber value) noexcept -> amsr::json::ParserResult override;
    auto OnString(ara::core::StringView value) noexcept -> amsr::json::ParserResult override;
    auto OnKey(ara::core::StringView key) noexcept -> amsr::json::ParserResult override;
    auto OnStartObject() noexcept -> amsr::json::ParserResult override;
    auto OnEndObject(std::size_t) noexcept -> amsr::json::ParserResult override;
    auto OnStartArray() noexcept -> amsr::json::ParserResult override;
    auto OnEndArray(std::size_t) noexcept -> amsr::json::ParserResult override;
    auto OnUnexpectedEvent() noexcept -> amsr::json::ParserResult override;

    template <typename... NumberType>
    auto OnNumber(amsr::json::JsonNumber& value) noexcept -> amsr::json::ParserResult
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
                   ? amsr::json::ParserState::kRunning
                   : amsr::json::ParserResult::FromError(amsr::json::JsonErrc::kInvalidJson);
    }

    std::string last_key_{};
    std::stack<Any*> hierarchy_{};
    Any root_{};
};

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_VAJSON_PARSER_H
