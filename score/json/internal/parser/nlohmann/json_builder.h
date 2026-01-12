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

#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_NLOHMANN_JSON_BUILDER_H
#define SCORE_LIB_JSON_INTERNAL_PARSER_NLOHMANN_JSON_BUILDER_H

#include "score/json/internal/model/any.h"
#include "score/result/result.h"

#include "nlohmann/json.hpp"

#include <memory>
#include <stack>
#include <string>

namespace score::json
{

/// \brief General purpose JSON parser that uses nlohmann. It abstracts the vendor specific API and returns
/// a custom tree of data elements.
class JsonBuilder
{
  public:
    auto GetData() -> score::Result<score::json::Any>;
    auto HandleEvent(nlohmann::json::parse_event_t event, nlohmann::json& parsed) -> bool;

  private:
    template <typename T>
    auto TryToStoreAsList(T& value, score::json::Any& node) -> score::Result<score::json::Any*>
    {
        auto node_as_list = node.As<score::json::List>();
        if (!node_as_list.has_value())
        {
            return score::MakeUnexpected(score::json::Error::kParsingError, "Failed to get list.");
        }
        // False positive, we are not forwarding a value to other functions.
        // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
        auto& node_as_list_ref = node_as_list.value().get();
        return &(node_as_list_ref.emplace_back(std::forward<T>(value)));
    }

    template <typename T>
    auto TryToStoreAsObject(T& value, score::json::Any& node) -> score::Result<score::json::Any*>
    {
        if (last_key_.empty())  // LCOV_EXCL_BR_LINE (see comment below)
        {
            // This case never happens.
            return score::MakeUnexpected(score::json::Error::kParsingError, "Failed to get last key"); /* LCOV_EXCL_LINE */
        }

        auto node_as_object = node.As<score::json::Object>();
        if (!node_as_object.has_value())
        {
            return score::MakeUnexpected(score::json::Error::kParsingError, "Failed to get object.");
        }
        // False positive, all values passed as rvalue.
        // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
        auto it = node_as_object.value().get().emplace(std::piecewise_construct_t(std::piecewise_construct),
                                                       std::forward_as_tuple(std::string{last_key_}),
                                                       std::forward_as_tuple(std::forward<T>(value)));
        return &((*(it.first)).second);
    }

    template <typename T>
    auto Store(T&& value) -> score::Result<score::json::Any*>
    {
        if (hierarchy_.empty() == true)
        {
            root_ = std::forward<T>(value);
            return &root_;
        }
        else
        {
            score::json::Any* current_node = hierarchy_.top();
            if (current_node == nullptr)
            {
                return score::MakeUnexpected(score::json::Error::kParsingError);
            }

            auto result_list = TryToStoreAsList<T>(value, *current_node);
            if (result_list.has_value())
            {
                return result_list;
            }
            auto result_object = TryToStoreAsObject<T>(value, *current_node);
            return result_object;
        }
    }

    template <typename T>
    auto StartContainer(T&& value) -> bool
    {
        auto result = Store(std::forward<T>(value));
        if (!result.has_value())  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable. Branch excluded from
                                  // coverage report. See comment below)
        {
            // Coverage: Not reachable. Line excluded from coverage report.
            // This case never happens, because condition can't be fulfilled due to Store implementantion
            return false; /* LCOV_EXCL_LINE */
        }
        else
        {
            // Justification: Using std::move or std::forward on a raw pointer does not provide any benefit
            // coverity[autosar_cpp14_a18_9_2_violation]
            hierarchy_.push(result.value());
        }
        return true;
    }

    auto EndContainer() -> bool;

    auto StoreValue(nlohmann::json& parsed) -> bool;

    std::string last_key_{};
    std::stack<score::json::Any*> hierarchy_{};
    score::json::Any root_{};
};

}  // namespace score::json

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_NLOHMANN_JSON_BUILDER_H
