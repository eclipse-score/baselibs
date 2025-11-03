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
#ifndef SCORE_LIB_JSON_JSON_SERIALIZER_H
#define SCORE_LIB_JSON_JSON_SERIALIZER_H

#include "score/json/json_parser.h"

#include "score/result/result.h"

#include <visitor/visit.h>
#include <visitor/visit_as_struct.h>

#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

namespace score::json
{

template <typename T, typename = void>
class JsonSerializer;

/// Converts a JSON hierarchy into a type that can be deserialized from JSON.
///
/// If the given JSON hierarchy does not fit to the given type, an error is returned. This can happen mainly for two
/// reasons: Either the inner types could not be deserialized because their type didn't match, or a field that is not
/// optional was not found inside the JSON hierarchy.
///
/// For structures to be deserialized, it needs to be visitable. This requires the macro STRUCT_VISITABLE to be used
/// with the given type and all attributes that shall be deserialized need to be passed to the macro.
///
/// \tparam T The type to deserialize. The type needs to be default-constructible.
/// \param any The JSON hierarchy to deserialize.
/// \return A result containing the deserialized object, or an error if deserialization failed.
template <typename T>
[[nodiscard]] inline Result<T> FromJsonAny(Any any)
{
    return JsonSerializer<T>::FromAny(std::move(any));
}

/// Converts a type that can be serialized into JSON into a JSON hierarchy.
///
/// For structures to be serialized, it needs to be visitable. This requires the macro STRUCT_VISITABLE to be used
/// with the given type and all attributes that shall be serialized need to be passed to the macro.
///
/// \tparam T The type to serialize.
/// \return The JSON hierarchy representing the given type.
template <typename T>
[[nodiscard]] inline Any ToJsonAny(T&& val)
{
    return JsonSerializer<T>::ToAny(std::forward<T>(val));
}

namespace detail
{

// Helper function to determine whether a struct is visitable, used during template specialization of JsonSerializer for
// visitable structs, see below.
//
// Coverity claims that this function isn't used. This is incorrect, as it is used in the template specialization of
// visitable structs below.
template <typename T>
// coverity[autosar_cpp14_a0_1_3_violation]
inline constexpr bool IsVisitableImpl(std::size_t)
{
    return true;
}

/// Helper template to determine whether a field is an std::optional or derives from std::optional and can be skipped
/// during deserialization
template <typename T, typename = void>
struct IsOptional : std::false_type
{
};

template <typename T>
struct IsOptional<T, std::enable_if_t<std::is_base_of_v<std::optional<typename T::value_type>, T>>> : std::true_type
{
};

namespace serializer
{

/// Helper template to determine whether a type has a ToAny method
template <typename T, typename = void>
struct HasToAny : std::false_type
{
};

template <typename T>
struct HasToAny<T, std::void_t<decltype(std::declval<T>().ToAny())>> : std::true_type
{
};

/// Data used during serialization of a struct.
struct SerializeAsJson
{
    // This structure is used as an internal structure carrying the state during serialization of a JSON. Converting
    // this to a real class would be detrimental to readability and would not contribute to maintainability. Therefore,
    // even though the members aren't POD-types, we still want a lightweight struct instead of a fully blown class.
    // coverity[autosar_cpp14_m11_0_1_violation]
    Object object;
};

/// Helper function to iterate through the fields of a struct during serialization.
// This is the overload for the base case where no fields are in the struct or when the recursive looping through the
// struct is finished.
// Therefore, we need to overload this function twice, once for processing one field and a second time for the base case
// where all fields have been iterated, if any.
template <typename T, std::size_t>
// coverity[autosar_cpp14_a2_10_4_violation]
inline void JsonSerializeStructImpl(Object&)
{
}

// See the base case for the reason to overload. Since we also want to forward where possible, we need to use universal
// references. The AUTOSAR rule wants to avoid this because it might lead to confusion. However, in this case, it is
// obvious which version is called since the universal reference only occurs in the non-base case.
template <typename T, std::size_t FieldIndex, typename Field, typename... Fields>
// coverity[autosar_cpp14_a13_3_1_violation]
// coverity[autosar_cpp14_a2_10_4_violation]
inline void JsonSerializeStructImpl(Object& visitor, Field&& field, Fields&&... fields)
{
    auto field_value = JsonSerializer<score::cpp::remove_cvref_t<Field>>::ToAny(std::forward<Field>(field));
    if (!field_value.template As<Null>().has_value())
    {
        const auto insertion_result =
            visitor.emplace(common::visitor::struct_visitable<T>::field_name(FieldIndex), std::move(field_value));
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(insertion_result.second, "Duplicate field name in struct serialization");
    }
    JsonSerializeStructImpl<T, FieldIndex + 1, Fields...>(visitor, std::forward<Fields>(fields)...);
}

template <typename T, typename... Fields>
inline void visit_as_struct(SerializeAsJson& visitor, T&&, Fields&&... fields)
{
    JsonSerializeStructImpl<T, 0, Fields...>(visitor.object, std::forward<Fields>(fields)...);
}

}  // namespace serializer

namespace deserializer
{

/// Helper template to determine whether a type has a FromAny method
template <typename T, typename = void>
struct HasFromAny : std::false_type
{
};

template <typename T>
struct HasFromAny<T, std::void_t<decltype(T::FromAny(Any{}))>> : std::true_type
{
};

/// Data used during deserialization of a struct.
struct DeserializeAsJson
{
    // This structure is used as an internal structure carrying the state during deserialization of a JSON. Converting
    // this to a real class would be detrimental to readability and would not contribute to maintainability. Therefore,
    // even though the members aren't POD-types, we still want a lightweight struct instead of a fully blown class.
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::reference_wrapper<Object> object;
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::optional<result::Error> error;
};

/// Helper function to iterate through the fields of a struct during deserialization.
// This is the overload for the base case where no fields are in the struct or when the recursive looping through the
// struct is finished.
// Therefore, we need to overload this function twice, once for processing one field and a second time for the base case
// where all fields have been iterated, if any.
template <typename T, std::size_t>
// coverity[autosar_cpp14_a2_10_4_violation]
inline void JsonDeserializeStructImpl(DeserializeAsJson&)
{
}

template <typename T, std::size_t FieldIndex, typename Field, typename... Fields>
inline void JsonDeserializeStructImpl(DeserializeAsJson& visitor, Field& field, Fields&... fields)
{
    const auto field_name = common::visitor::struct_visitable<T>::field_name(FieldIndex);
    auto& obj = visitor.object.get();
    if (obj.count(field_name) > 0U)
    {
        if (auto field_content = JsonSerializer<score::cpp::remove_cvref_t<Field>>::FromAny(std::move(obj[field_name])))
        {
            field = std::move(field_content).value();
            JsonDeserializeStructImpl<T, FieldIndex + 1, Fields...>(visitor, fields...);
        }
        else
        {
            score::cpp::ignore = visitor.error.emplace(std::move(field_content).error());
        }
    }
    else
    {
        // This is a false positive from coverity. Apparently, "if constexpr" is unknown to this tool.
        // coverity[autosar_cpp14_a7_1_8_violation]
        // coverity[autosar_cpp14_m6_4_1_violation]
        if constexpr (IsOptional<score::cpp::remove_cvref_t<Field>>::value)
        {
            // Optional fields are not considered an error
            JsonDeserializeStructImpl<T, FieldIndex + 1, Fields...>(visitor, fields...);
        }
        else
        {
            score::cpp::ignore = visitor.error.emplace(Error::kKeyNotFound, "Missing mandatory field in JSON object");
        }
    }
}

template <typename T, typename... Fields>
inline void visit_as_struct(DeserializeAsJson& visitor, T&&, Fields&... fields)
{
    JsonDeserializeStructImpl<T, 0, Fields...>(visitor, fields...);
}

}  // namespace deserializer

}  // namespace detail

/// Class template that defines how a given type can be serialized and deserialized to JSON.
///
/// This class template needs to be specialized for each type that can be serialized or deserialized from JSON.
///
/// Any specialization needs to provide two static methods: ToAny and FromAny. The base case assumes that the type
/// includes two methods, ToAny and FromAny that have the following signatures:
///
/// static Result<T> FromAny(Any any);
/// Any ToAny();
///
/// It is also ok to specialize JsonSerializer for custom types if this is more suited to your needs, e.g. if you cannot
/// or do not want to add the required extra methods.
///
/// \tparam T The type to serialize or deserialize.
/// \tparam Enabled An SFINAE parameter that can be used to enable/disable certain specializations based on type
/// properties.
template <typename T, typename Enabled>
class JsonSerializer
{
  public:
    template <typename U>
    [[nodiscard]] static Any ToAny(U&& obj)
    {
        return std::forward<U>(obj).ToAny();
    }

    [[nodiscard]] static Result<T> FromAny(Any any)
    {
        return T::FromAny(std::move(any));
    }
};

/// This is the definition of the base case which assumes it is a visitable structure. For a structure to be visitable,
/// the macro STRUCT_VISITABLE has to be used and all the attributes that shall be serializable or deserializable need
/// to be passed. It then serializes and deserializes all attributes of the struct in the sequence of their appearance
/// in the STRUCT_VISITABLE macro.
///
/// This specialization is only used if the visitable type does _not_ have ToAny and/or FromAny. If at least one of
/// these methods is present, the general case is used which forwards the call to these methods of the type.
template <typename T>
class JsonSerializer<
    T,
    std::enable_if_t<detail::IsVisitableImpl<score::cpp::remove_cvref_t<T>>(
                         common::visitor::struct_visitable<score::cpp::remove_cvref_t<T>>::fields) &&
                     !(detail::serializer::HasToAny<T>::value || detail::deserializer::HasFromAny<T>::value)>>
{
  public:
    template <typename U>
    [[nodiscard]] static Any ToAny(U&& obj)
    {
        detail::serializer::SerializeAsJson visitor{};
        common::visitor::visit(visitor, std::forward<U>(obj));
        return Any{std::move(visitor.object)};
    }

    [[nodiscard]] static Result<T> FromAny(Any any)
    {
        if (auto obj = any.As<Object>())
        {
            T result;
            detail::deserializer::DeserializeAsJson deserialize_as_json{*obj, std::nullopt};
            common::visitor::visit(deserialize_as_json, result);
            if (!deserialize_as_json.error.has_value())
            {
                return result;
            }
            else
            {
                return MakeUnexpected<T>(deserialize_as_json.error.value());
            }
        }
        else
        {
            return MakeUnexpected<T>(obj.error());
        }
    }
};

// This is the specialization for all number types (integer as well as floats)
template <typename N>
class JsonSerializer<N, std::enable_if_t<std::is_arithmetic_v<N> && !std::is_same_v<N, bool>>>
{
  public:
    [[nodiscard]] static Any ToAny(N number)
    {
        return Any{number};
    }

    [[nodiscard]] static Result<N> FromAny(Any any)
    {
        if (auto number = any.As<Number>())
        {
            if (auto val = number->get().As<N>(); val.has_value())
            {
                return val;
            }
            else
            {
                return MakeUnexpected(Error::kWrongType, "Number not convertible to expected arithmetic type");
            }
        }
        else
        {
            return MakeUnexpected(Error::kWrongType, "Expected a number");
        }
    }
};

// This is the specialization for bool
template <>
class JsonSerializer<bool>
{
  public:
    [[nodiscard]] static Any ToAny(const bool boolean)
    {
        return Any{boolean};
    }

    [[nodiscard]] static Result<bool> FromAny(const Any any)
    {
        if (auto boolean = any.As<bool>())
        {
            return boolean;
        }
        else
        {
            return MakeUnexpected(Error::kWrongType, "Expected a bool");
        }
    }
};

// Specialization for std::string
template <>
class JsonSerializer<std::string>
{
  public:
    [[nodiscard]] static Any ToAny(std::string str)
    {
        return Any{std::move(str)};
    }

    [[nodiscard]] static Result<std::string> FromAny(Any any)
    {
        if (auto str = any.As<std::string>(); str.has_value())
        {
            return str->get();
        }
        else
        {
            return MakeUnexpected(Error::kWrongType, "Expected a string");
        }
    }
};

// Specialization for std::vector. The enclosed type also needs to be JSON serializable.
template <typename T>
class JsonSerializer<std::vector<T>>
{
  public:
    [[nodiscard]] static Any ToAny(const std::vector<T>& vec)
    {
        List json_list{};
        json_list.reserve(vec.size());
        auto inserter = std::back_inserter(json_list);
        std::transform(vec.begin(), vec.end(), inserter, [](const T& val) {
            return JsonSerializer<T>::ToAny(val);
        });
        return Any{std::move(json_list)};
    }

    [[nodiscard]] static Result<std::vector<T>> FromAny(Any any)
    {
        if (auto list = any.As<List>())
        {
            std::vector<T> result{};
            result.reserve(list->get().size());
            for (auto& list_entry : list->get())
            {
                if (auto val = JsonSerializer<T>::FromAny(std::move(list_entry)); val.has_value())
                {
                    result.push_back(*std::move(val));
                }
                else
                {
                    return MakeUnexpected(Error::kWrongType, "List entry not of expected type");
                }
            }
            return result;
        }
        else
        {
            return MakeUnexpected(Error::kWrongType, "Expected a list");
        }
    }
};

// Specialization for fields wrapped in std::optional. It's basically a noop, providing an empty Any in the
// serialization case so that empty fields aren't serialized at all.
template <typename T>
class JsonSerializer<std::optional<T>>
{
  public:
    template <typename U = std::optional<T>>
    [[nodiscard]] static Any ToAny(U&& opt)
    {
        if (opt.has_value())
        {
            return JsonSerializer<T>::ToAny(std::forward<U>(opt).value());
        }
        else
        {
            return {};
        }
    }

    [[nodiscard]] static Result<std::optional<T>> FromAny(Any any)
    {
        if (auto val = JsonSerializer<T>::FromAny(std::move(any)))
        {
            return std::optional<T>{std::move(val).value()};
        }
        else
        {
            return MakeUnexpected(Error::kWrongType, "Optional value not of expected type");
        }
    }
};

// Also provide a specialization for Any so that a user can also put Any into a struct or a vector if desired.
template <>
class JsonSerializer<Any>
{
  public:
    [[nodiscard]] static Any ToAny(Any any)
    {
        return any;
    }

    [[nodiscard]] static Result<Any> FromAny(Any any)
    {
        return any;
    }
};

}  // namespace score::json

#endif  // SCORE_LIB_JSON_JSON_SERIALIZER_H
