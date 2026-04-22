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

#include "score/json/internal/writer/json_serialize/json_serialize.h"

#include <score/assert.hpp>
#include <score/utility.hpp>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <string>

namespace score
{
namespace json
{

JsonSerialize::JsonSerialize(std::ostream& out_stream) : out_stream_{out_stream} {}

void JsonSerialize::SerializeType(const std::reference_wrapper<const std::string>& json_string) const
{
    out_stream_ << std::quoted(json_string.get().data());
}

void JsonSerialize::SerializeType(const std::reference_wrapper<const Null>& json_null) const
{
    score::cpp::ignore = json_null;
    out_stream_ << "null";
    score::cpp::ignore = out_stream_.flush();
}

bool JsonSerialize::SerializeType(const score::json::Any& json_data, const std::uint16_t tab_count)
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(tab_count < std::numeric_limits<std::uint16_t>::max());
    // LCOV_EXCL_BR_START (Decision Coverage: Not reachable. Branch
    // excluded from coverage report. See comment below)
    if (!Serialize(json_data, static_cast<uint16_t>(tab_count)))
    {
        // This code can never be reached because template constraints of type Any make sure only valid underlying
        // types are used.
        return false; /* LCOV_EXCL_LINE */
    }
    // LCOV_EXCL_BR_STOP
    score::cpp::ignore = out_stream_.flush();
    return true;
}

// Justification: broken_link_c/issue/15410189
// NOLINTNEXTLINE(misc-no-recursion) recursion justified for json parsing
bool JsonSerialize::SerializeType(const Object& json_data, const std::uint16_t tab_count)
{
    out_stream_ << "{\n";
    for (const auto& pair : json_data)  // LCOV_EXCL_BR_LINE (FP: "Decision couldn't be analyzed" gcov issue)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(tab_count < std::numeric_limits<std::uint16_t>::max());
        Indent(static_cast<uint16_t>(tab_count + 1U));
        out_stream_ << '"' << pair.first.GetAsStringView().data() << '"';
        out_stream_ << ": ";

        // LCOV_EXCL_BR_START (Decision Coverage: Not reachable. Branch
        // excluded from coverage report. See comment below)
        if (!Serialize(pair.second, static_cast<uint16_t>(tab_count + 1U)))
        {
            // This code can never be reached because template constraints of type Any make sure only valid underlying
            // types are used.
            return false; /* LCOV_EXCL_LINE */
        }
        // LCOV_EXCL_BR_STOP

        out_stream_ << ",\n";
    }
    // move position by -2 if object contains element in order to remove the last comma and newline
    // move position by -1 if object is empty in order to remove the last newline only
    const std::ostream::pos_type last_comma_position{json_data.empty() ? static_cast<std::ostream::pos_type>(-1)
                                                                       : static_cast<std::ostream::pos_type>(-2)};
    score::cpp::ignore = out_stream_.seekp(last_comma_position, out_stream_.cur);
    out_stream_ << '\n';
    Indent(tab_count);
    out_stream_ << '}';
    score::cpp::ignore = out_stream_.flush();

    return true;
}

// Justification: broken_link_c/issue/15410189
// NOLINTNEXTLINE(misc-no-recursion) recursion justified for json parsing
bool JsonSerialize::SerializeType(const List& json_data, const std::uint16_t tab_count)
{
    out_stream_ << "[\n";
    for (const Any& value : json_data)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(tab_count < std::numeric_limits<std::uint16_t>::max());
        Indent(static_cast<uint16_t>(tab_count + 1U));

        // LCOV_EXCL_BR_START (Decision Coverage: Not reachable. Branch
        // excluded from coverage report. See comment below)
        if (!Serialize(value, static_cast<uint16_t>(tab_count + 1U)))
        {
            // This code can never be reached because template constraints of type Any make sure only valid underlying
            // types are used.
            return false; /* LCOV_EXCL_LINE */
        }
        // LCOV_EXCL_BR_STOP

        out_stream_ << ",\n";
    }
    // move position by -2 if list contains element in order to remove the last comma and newline
    // move position by -1 if list is empty in order to remove the last newline only
    const std::ostream::pos_type last_comma_position{json_data.empty() ? static_cast<std::ostream::pos_type>(-1)
                                                                       : static_cast<std::ostream::pos_type>(-2)};
    score::cpp::ignore = out_stream_.seekp(last_comma_position, out_stream_.cur);
    out_stream_ << '\n';
    Indent(tab_count);
    out_stream_ << ']';
    score::cpp::ignore = out_stream_.flush();

    return true;
}

void JsonSerialize::Indent(const std::uint16_t indent_count) const
{
    const constexpr std::uint32_t space_count{4U};
    for (std::uint32_t current_indent{0U}; current_indent < space_count * indent_count; ++current_indent)
    {
        out_stream_ << ' ';
    }
}

// Justification: broken_link_c/issue/15410189
// NOLINTNEXTLINE(misc-no-recursion) recursion justified for json parsing
bool JsonSerialize::Serialize(const Any& value, const std::uint16_t tab_count)
{
    // False positive, the operands do not contain binary operators.
    // coverity[autosar_cpp14_a5_2_6_violation : FALSE]
    return SerializeIfType<std::string>(value) || SerializeIfType<std::uint64_t>(value) ||
           SerializeIfType<std::int64_t>(value) || SerializeIfType<float>(value) || SerializeIfType<double>(value) ||
           SerializeIfType<Null>(value) || SerializeIfType<List>(value, tab_count) ||
           SerializeIfType<Object>(value, tab_count);
}

template <typename T, std::enable_if_t<!(std::is_same<T, List>::value || std::is_same<T, Object>::value), bool>>
bool JsonSerialize::SerializeIfType(const Any& json_data)
{
    if (json_data.As<T>().has_value() == true)
    {
        SerializeType(json_data.As<T>().value());
        return true;
    }
    return false;
}

template <typename T, std::enable_if_t<std::is_same<T, List>::value || std::is_same<T, Object>::value, bool>>
// Justification: broken_link_c/issue/15410189
// NOLINTNEXTLINE(misc-no-recursion) recursion justified for json parsing
bool JsonSerialize::SerializeIfType(const Any& json_data, std::uint16_t tab_count)
{
    if (json_data.As<T>().has_value() == true)
    {
        return SerializeType(json_data.As<T>().value().get(), tab_count);
    }
    return false;
}

}  // namespace json
}  // namespace score
