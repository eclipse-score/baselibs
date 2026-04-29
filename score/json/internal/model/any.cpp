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

#include "score/json/internal/model/any.h"
#include "score/overload.hpp"

#include <score/assert.hpp>

namespace score
{
namespace json
{

// Default constractor has special init so it has a different implemetation
// coverity[autosar_cpp14_a12_1_5_violation: FALSE]
Any::Any() : value_(Null{}) {}

// std::visit throws std::bad_variant_access if as-variant(value_i).valueless_by_exception() is true
// this case is already handled inside an assertion
// coverity[autosar_cpp14_a15_5_3_violation]
bool operator==(const Any& lhs, const Any& rhs) noexcept
{
    if (lhs.value_.index() != rhs.value_.index())
    {
        return false;  // The variants hold different types
    }

    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(!lhs.value_.valueless_by_exception());
    return std::visit(score::cpp::overload(
                          [&rhs](const bool value) noexcept {
                              // LCOV_EXCL_BR_START Failed branch is unreachable since types are compared before
                              if (const auto result = rhs.As<bool>(); result.has_value())
                              // LCOV_EXCL_BR_STOP
                              {
                                  return value == *result;
                              }
                              return false;  // LCOV_EXCL_LINE Unreachable since types are compared before
                          },
                          [&rhs](const std::string& value) noexcept {
                              // LCOV_EXCL_BR_START Failed branch is unreachable since types are compared before
                              if (const auto result = rhs.As<std::string>(); result.has_value())
                              // LCOV_EXCL_BR_STOP
                              {
                                  return value == result->get();
                              }
                              return false;  // LCOV_EXCL_LINE Unreachable since types are compared before
                          },
                          [&rhs](const Number& value) noexcept {
                              // LCOV_EXCL_BR_START Failed branch is unreachable since types are compared before
                              if (const auto result = rhs.As<Number>(); result.has_value())
                              // LCOV_EXCL_BR_STOP
                              {
                                  return value == *result;
                              }
                              return false;  // LCOV_EXCL_LINE Unreachable since types are compared before
                          },
                          // The code is present in a single line due to clang format failure.
                          // coverity[autosar_cpp14_a7_1_7_violation]
                          [&rhs](Null) noexcept {
                              return rhs.As<Null>().has_value();
                          },
                          // This is a false positive, we here using lvalue reference.
                          // coverity[autosar_cpp14_a8_4_12_violation : FALSE]
                          [&rhs](const std::unique_ptr<Object>& value) noexcept {
                              // LCOV_EXCL_BR_START Failed branch is unreachable since types are compared before
                              if (const auto result = rhs.As<Object>(); result.has_value())
                              // LCOV_EXCL_BR_STOP
                              {
                                  return *value == result->get();
                              }
                              return false;  // LCOV_EXCL_LINE Unreachable since types are compared before
                          },
                          // This is a false positive, we here using lvalue reference.
                          // coverity[autosar_cpp14_a8_4_12_violation : FALSE]
                          [&rhs](const std::unique_ptr<List>& value) noexcept {
                              // LCOV_EXCL_BR_START Failed branch is unreachable since types are compared before
                              if (const auto result = rhs.As<List>(); result.has_value())
                              // LCOV_EXCL_BR_STOP
                              {
                                  return *value == result->get();
                              }
                              return false;  // LCOV_EXCL_LINE Unreachable since types are compared before
                          }),
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
