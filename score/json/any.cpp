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

#include "score/json/any.h"

#include <score/assert.hpp>
#include <type_traits>

namespace score
{
namespace json
{

// Default constructor has special init so it has a different implementation
// coverity[autosar_cpp14_a12_1_5_violation: FALSE]
Any::Any() : value_(Null{}) {}

// std::visit throws std::bad_variant_access if as-variant(value_i).valueless_by_exception() is true
// this case is already handled inside an assertion
// coverity[autosar_cpp14_a15_5_3_violation]
bool operator==(const Any& lhs, const Any& rhs) noexcept
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(!lhs.value_.valueless_by_exception());
    return std::visit(
        [&rhs](const auto& value) noexcept {
            using ValueType = std::decay_t<decltype(value)>;
            const auto* const other{std::get_if<ValueType>(&rhs.value_)};
            if (other == nullptr)
            {
                return false;  // The variants hold different types.
            }
            if constexpr (std::is_same_v<ValueType, std::unique_ptr<Object>> ||
                          std::is_same_v<ValueType, std::unique_ptr<List>>)
            {
                return *value == **other;
            }
            else
            {
                return value == *other;
            }
        },
        lhs.value_);
}

// std::visit throws std::bad_variant_access if as-variant(value_i).valueless_by_exception() is true
// this case is already handled inside an assertion
// coverity[autosar_cpp14_a15_5_3_violation]
Any Any::CloneByValue() const noexcept
{
    class Visitor
    {
      public:
        Any operator()(const bool v)
        {
            return Any(v);
        }
        Any operator()(const json::Number v)
        {
            return Any(v);
        }
        Any operator()(std::string v)
        {
            return Any(std::move(v));
        }
        Any operator()(const Null v)
        {
            return Any(v);
        }
        // This is a false positive, we here using lvalue reference.
        // coverity[autosar_cpp14_a8_4_12_violation : FALSE]
        Any operator()(const std::unique_ptr<json::Object>& v)
        {
            json::Object object;
            for (const auto& item : *v)  // LCOV_EXCL_BR_LINE (FP: "Decision couldn't be analyzed" gcov issue)
            {
                object[item.first] = item.second.CloneByValue();
            }
            Any result{std::move(object)};
            return result;
        }
        // This is a false positive, we here using lvalue reference.
        // coverity[autosar_cpp14_a8_4_12_violation : FALSE]
        Any operator()(const std::unique_ptr<json::List>& v)
        {
            json::List list;
            for (const auto& item : *v)
            {
                list.push_back(item.CloneByValue());
            }
            Any result{std::move(list)};
            return result;
        }
    };

    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(!value_.valueless_by_exception());
    return std::visit(Visitor{}, value_);
}

}  // namespace json
}  // namespace score
