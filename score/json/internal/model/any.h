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

#ifndef SCORE_LIB_JSON_INTERNAL_MODEL_ANY_H
#define SCORE_LIB_JSON_INTERNAL_MODEL_ANY_H

#include "score/json/internal/model/error.h"
#include "score/json/internal/model/null.h"
#include "score/json/internal/model/number.h"
#include "score/memory/string_comparison_adaptor.h"
#include "score/result/result.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace score
{
namespace json
{

class Any;

using List = std::vector<score::json::Any>;
using Object = std::map<score::memory::StringComparisonAdaptor, score::json::Any>;
/// \brief Represents a JSON value, where the current type is unknown
///
/// \details JSON can represent different kinds of types:
/// * bool, numeric, string, null, lists and objects
/// These types can be aggregated in any order or what so ever. Since at compile time its not clear how the structure
/// of these types looks like, we have to work with a type placeholder. This is implemented by this Any class.
///
/// _Note_: Trying to access or construct this class with a type other then the defined ones in the variant, will lead
/// to compilation errors.
class Any
{
  public:
    /// \brief Default constructor for an empty Any
    /// We represent this by setting the value to Null.
    Any();

    /// \brief Construct Any with a specific type
    /// \tparam T Supported types are: bool, std::string, Null
    /// \param value The value that Any shall hold
    template <typename T,
              typename std::enable_if_t<!std::is_same<T, Result<Any>>::value, bool> = true,
              typename std::enable_if_t<(std::is_same<T, bool>::value) ||
                                            (!std::is_arithmetic<T>::value && !std::is_same<T, Object>::value &&
                                             !std::is_same<T, List>::value),
                                        bool> = true>
    explicit Any(T value) : value_{std::move(value)}
    {
    }

    /// \brief Convenience constructor for a number type
    /// \tparam T Supported types are: Number
    /// \param value The value that Any shall hold
    template <typename T,
              // Without this we get a bogus error that score::cpp::expected cannot to construct Any from
              // score::cpp::expected<Any,...>.
              typename = std::enable_if_t<!std::is_same<T, score::Result<Any>>::value, bool>,
              typename = std::enable_if_t<!std::is_same<T, bool>::value, bool>,
              typename = std::enable_if_t<std::is_arithmetic<T>::value, bool>>
    // False positive, explicit has the correct place which is before Any.
    // coverity[autosar_cpp14_a7_1_8_violation : FALSE]
    explicit Any(T value) : value_{score::json::Number{value}}
    {
    }

    /// \brief Construct Any with a specific type
    /// \param value The object that Any shall hold
    template <typename T, typename std::enable_if_t<std::is_same_v<T, Object> || std::is_same_v<T, List>, int> = 0>
    explicit Any(T value) : value_{std::make_unique<T>(std::forward<T>(value))}
    {
    }

    /// \brief Operator to directly assign an object
    /// \param value The object that Any shall hold
    template <typename T,
              typename std::enable_if_t<(std::is_same<T, bool>::value) ||
                                            (!std::is_arithmetic<T>::value && !std::is_same<T, Object>::value &&
                                             !std::is_same<T, List>::value && !std::is_same<T, Any>::value),
                                        int> = 0>
    // NOLINTNEXTLINE(misc-unconventional-assign-operator) used for directly assigning an object.
    Any& operator=(T&& value)
    {
        value_ = std::forward<T>(value);
        return *this;
    }

    /// \brief Operator to directly assign an object
    /// \param value The Object or List that Any shall hold
    template <typename T, typename std::enable_if_t<std::is_same_v<T, Object> || std::is_same_v<T, List>, int> = 0>
    // Justification: This overload does not confuse developers because it is constrained.
    // coverity[autosar_cpp14_a13_3_1_violation]
    Any& operator=(T&& value)
    {
        value_ = std::make_unique<T>(std::forward<T>(value));  // LCOV_EXCL_BR_LINE (no branching operators)
        return *this;
    }

    /// \brief Convenience Operator to directly assign an number
    /// \param value The object that Any shall hold
    template <typename T,
              typename = std::enable_if_t<!std::is_same<T, bool>::value, bool>,
              typename = std::enable_if_t<std::is_arithmetic<T>::value, bool>>
    // Justification: This overload does not confuse developers because it is constrained.
    // coverity[autosar_cpp14_a13_3_1_violation : FALSE]
    Any& operator=(T value)
    {
        value_ = json::Number{value};
        return *this;
    }

    Any(Any&& t) = default;
    // False positive, this is the move assignement operator not a forwarding reference overload.
    // coverity[autosar_cpp14_a13_3_1_violation : FALSE]
    Any& operator=(Any&& t) = default;
    Any(const Any& t) = delete;
    Any& operator=(const Any& t) = delete;

    ~Any() = default;

    Any CloneByValue() const noexcept;
    friend bool operator==(const Any& lhs, const Any& rhs) noexcept;

    /// \brief Interpret array as type T
    /// \tparam T The type that this shall be interpreted
    /// \return A result containing the value if the underlying variant holds it, an kWrongType Error otherwise
    template <typename T,
              typename = std::enable_if_t<!std::is_same<T, Object>::value, bool>,
              typename = std::enable_if_t<!std::is_same<T, List>::value, bool>,
              typename = std::enable_if_t<!std::is_same<T, std::string_view>::value, bool>,
              typename = std::enable_if_t<!std::is_arithmetic<T>::value>>
    score::Result<std::reference_wrapper<T>> As() noexcept
    {
        auto* value = std::get_if<T>(&value_);
        if (value != nullptr)
        {
            return *value;
        }
        return score::MakeUnexpected(score::json::Error::kWrongType);
    }

    /// \brief Interpret array as type T
    /// \tparam T The type that this shall be interpreted
    /// \return A result containing the value if the underlying variant holds it, an kWrongType Error otherwise
    template <typename T,
              typename = std::enable_if_t<!std::is_same<T, Object>::value, bool>,
              typename = std::enable_if_t<!std::is_same<T, List>::value, bool>,
              typename = std::enable_if_t<!std::is_arithmetic<T>::value>,
              typename = std::enable_if_t<!std::is_same<T, std::string_view>::value, bool>>
    score::Result<std::reference_wrapper<const T>> As() const noexcept
    {
        const auto* const value = std::get_if<T>(&value_);
        if (value != nullptr)
        {
            return *value;
        }

        return score::MakeUnexpected(score::json::Error::kWrongType);
    }

    /// \brief Interpret array as type T
    /// \tparam T The type that this shall be interpreted
    /// \return A result containing the value if the underlying variant holds it, an kWrongType Error otherwise
    template <typename T,
              typename = std::enable_if_t<std::is_same<T, List>::value || std::is_same<T, Object>::value, bool>>
    score::Result<std::reference_wrapper<T>> As() noexcept
    {
        auto* value = std::get_if<std::unique_ptr<T>>(&value_);
        if (value != nullptr)
        {
            return **value;
        }
        return score::MakeUnexpected(score::json::Error::kWrongType);
    }

    /// \brief Interpret array as type T
    /// \tparam T The type that this shall be interpreted
    /// \return A result containing the value if the underlying variant holds it, an kWrongType Error otherwise
    template <typename T,
              typename = std::enable_if_t<std::is_same<T, List>::value || std::is_same<T, Object>::value, bool>>
    score::Result<std::reference_wrapper<const T>> As() const noexcept
    {
        const auto* const value = std::get_if<std::unique_ptr<T>>(&value_);
        if (value != nullptr)
        {
            return **value;
        }
        return score::MakeUnexpected(score::json::Error::kWrongType);
    }

    /// \brief Convenience method to directly convert a JSON number or boolean into arithmetic type.
    template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value, bool>>
    score::Result<T> As() const noexcept
    {
        const auto* const value = std::get_if<Number>(&value_);
        if (value != nullptr)
        {
            return value->As<T>();
        }

        // Fallback iff the element contains a bool.
        const auto* const boolean = std::get_if<bool>(&value_);
        if (boolean != nullptr)
        {
            // The cast from a bool to every arithmetic type is well-defined.
            return static_cast<T>(*boolean);
        }

        return score::MakeUnexpected(score::json::Error::kWrongType);
    }

    /// \brief Convenience method to get a string_view to a JSON string element.
    template <typename T, typename = std::enable_if_t<std::is_same<T, std::string_view>::value, bool>>
    score::Result<std::string_view> As() const noexcept
    {
        const auto* const value_string = std::get_if<std::string>(&value_);
        if (value_string != nullptr)
        {
            return std::string_view{value_string->c_str(), value_string->size()};
        }

        return score::MakeUnexpected(score::json::Error::kWrongType);
    }

  private:
    std::variant<bool, Number, std::string, Null, std::unique_ptr<Object>, std::unique_ptr<List>> value_;
};

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_MODEL_ANY_H
