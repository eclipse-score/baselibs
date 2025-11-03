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
#include "serialization/visit_type_traits.h"
#include "visitor_test_types.h"

#include <gtest/gtest.h>
#include <array>
#include <string>
#include <vector>

namespace score
{
namespace common
{
namespace visitor
{

TEST(vistor_type_traits, is_vector_serializable)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Logging library shall provide an annotation mechanism for data structures to support automatic "
        "serialization/deserialization, So, we are checking some data types to be treated for vector serialization.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    using test::clearable_container;
    using std_basic_string = std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
    static_assert(is_vector_serializable<std::vector<int>>::value, "Error: std::vector should be vector-serializable");
    static_assert(is_vector_serializable<std::vector<std::vector<int>>>::value,
                  "Error: std::vector<std::vector> should be vector-serializable");
    static_assert(is_vector_serializable<clearable_container<int>>::value,
                  "Error: clearable_container should be vector-serializable");
    static_assert(is_vector_serializable<clearable_container<double, std::allocator<double>>>::value,
                  "Error: clearable_container should be vector-serializable");
    static_assert(is_vector_serializable<std::string>::value, "Error: std::string should be vector-serializable");
    static_assert(is_vector_serializable<std_basic_string>::value,
                  "Error: std::basic_string should be vector-serializable");
}

TEST(vistor_type_traits, is_not_vector_serializable)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Logging library shall provide an annotation mechanism for data structures to support automatic "
                   "serialization/deserialization. So, we are checking those some data types shouldn't be treated as "
                   "vector serialization.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    using test::unserializable_container;
    static_assert(!is_vector_serializable<std::array<int, 3>>::value,
                  "Error: std::array should NOT be vector-serializable");
    static_assert(!is_vector_serializable<unserializable_container<int>>::value,
                  "Error: unserializable_container should NOT be vector-serializable");
    static_assert(!is_vector_serializable<unserializable_container<double, std::allocator<double>>>::value,
                  "Error: unserializable_container should NOT be vector-serializable");
}

}  // namespace visitor
}  // namespace common
}  // namespace score
