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

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_JSON_SERIALIZE_JSON_SERIALIZE_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_JSON_SERIALIZE_JSON_SERIALIZE_H

#include "score/json/internal/model/any.h"
#include "score/json/internal/model/null.h"
#include "score/result/result.h"

#include <limits>
#include <ostream>
#include <type_traits>

namespace score
{
namespace json
{

class JsonSerialize final
{
  public:
    explicit JsonSerialize(std::ostream& out_stream);
    ~JsonSerialize() = default;

    JsonSerialize(const JsonSerialize& other) = delete;
    JsonSerialize(JsonSerialize&& other) noexcept = default;
    JsonSerialize& operator=(const JsonSerialize& other) = delete;
    JsonSerialize& operator=(JsonSerialize&& other) = delete;

    template <typename T>
    score::Result<void> operator<<(const T& json_data)
    {
        Result<void> result{MakeUnexpected(Error::kWrongType)};
        if (SerializeType(json_data,
                          static_cast<std::uint16_t>(0)))  // LCOV_EXCL_BR_LINE (Decision Coverage: Not reachable.
                                                           // Branch excluded from coverage report. See comment below)
        {
            result = Result<void>{};
        }
        // Never happens because SerializeType always returns true
        return result; /* LCOV_EXCL_LINE */
    }

  private:
    bool Serialize(const score::json::Any& value, const std::uint16_t tab_count);
    template <typename T,
              std::enable_if_t<!(std::is_same<T, score::json::List>::value || std::is_same<T, score::json::Object>::value),
                               bool> = true>
    bool SerializeIfType(const score::json::Any& json_data);
    template <typename T,
              std::enable_if_t<std::is_same<T, score::json::List>::value || std::is_same<T, score::json::Object>::value,
                               bool> = true>
    // Justification: broken_link_c/issue/15410189
    // NOLINTNEXTLINE(misc-no-recursion) recursion justified for json parsing
    bool SerializeIfType(const score::json::Any& json_data, std::uint16_t tab_count);
    template <typename T, std::enable_if_t<!std::is_floating_point<T>::value, bool> = true>
    void SerializeType(const T& t) const
    {
        out_stream_ << t;
        score::cpp::ignore = out_stream_.flush();
    }
    template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    void SerializeType(const T& json_floating_point) const
    {
        score::cpp::ignore = out_stream_.precision(std::numeric_limits<T>::max_digits10);
        out_stream_ << json_floating_point;
        score::cpp::ignore = out_stream_.flush();
    }
    void SerializeType(const std::reference_wrapper<const std::string>& json_string) const;
    void SerializeType(const std::reference_wrapper<const score::json::Null>& json_null) const;
    bool SerializeType(const score::json::Object& json_data, const std::uint16_t tab_count);
    bool SerializeType(const score::json::List& json_data, const std::uint16_t tab_count);
    bool SerializeType(const score::json::Any& json_data, const std::uint16_t tab_count);
    void Indent(const std::uint16_t indent_count) const;

    std::ostream& out_stream_;
};

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_JSON_SERIALIZE_JSON_SERIALIZE_H
