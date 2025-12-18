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
#include "static_reflection_with_serialization/serialization/visit_serialize.h"
#include "static_reflection_with_serialization/serialization/visit_size.h"
#include "static_reflection_with_serialization/serialization/visit_type_traits.h"
#include "visitor_test_types.h"

#include <gtest/gtest.h>
#include <chrono>
#include <cmath>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

using test::assignable_container;
using test::clearable_container;
using test::resizeable_container;

namespace score
{
namespace common
{
namespace visitor
{

/**
 * Generic compare function for types that have defined operator== and
 * for C-style arrays
 */
template <typename T, std::enable_if_t<!is_vector_serializable<T>::value, int> = 0>
bool cmp(const T& op1, const T& op2)
{
    return op1 == op2;
}

/**
 * Generic compare function for containers that might not have operator==
 * defined but whose elements have defined operator==
 */
template <typename T, std::enable_if_t<is_vector_serializable<T>::value, int> = 0>
bool cmp(const T& op1, const T& op2)
{
    return std::equal(op1.cbegin(),
                      op1.cbegin() + static_cast<std::ptrdiff_t>(op1.size()),  //
                      op2.cbegin(),
                      op2.cbegin() + static_cast<std::ptrdiff_t>(op2.size()));
}

template <typename T, size_t N>
bool cmp(const T (&op1)[N], const T (&op2)[N])
{
    return std::equal(std::cbegin(op1), std::cend(op1), std::cbegin(op2), std::cend(op2));
}

/**
 * Generic copy-assignment for types that have defined operator= and
 * for C-style arrays
 */
template <typename T>
T& copyAssignment(T& lhs, const T& rhs)
{
    return (lhs = rhs);
}

template <typename T, size_t N>
decltype(auto) copyAssignment(T (&lhs)[N], const T (&rhs)[N])
{
    std::copy(std::cbegin(rhs), std::cend(rhs), std::begin(lhs));
    return lhs;
}

/**
 * Get Value from struct from appropriate fields
 * based on std::tuple with member pointers
 */
template <typename ValuesStructT, typename V, typename VP>
void getValue(const ValuesStructT&, V&, VP ValuesStructT::*);

template <typename ValuesStructT, typename V>
void getValue(const ValuesStructT& valuesStruct, V& valueOut, V ValuesStructT::*fieldMemberPtr)
{
    copyAssignment(valueOut, valuesStruct.*fieldMemberPtr);
}

template <typename ValuesStructT, typename V, typename... Args>
void getValue(const ValuesStructT& valuesStruct, V& valueOut, V ValuesStructT::*fieldMemberPtr, Args...)
{
    copyAssignment(valueOut, valuesStruct.*fieldMemberPtr);
}

template <typename ValuesStructT, typename V, typename VP, typename... Args>
void getValue(const ValuesStructT& valuesStruct, V& valueOut, VP ValuesStructT::*fieldMemberPtr, Args... args)
{
    (void)fieldMemberPtr;
    getValue(valuesStruct, valueOut, args...);
}

template <typename ValuesStructT, typename V, typename... VP, size_t... Index>
void getValue(const ValuesStructT& valuesStruct,
              V& valueOut,
              const std::tuple<VP...>& fieldMemberPtrs,
              std::index_sequence<Index...>)
{
    getValue(valuesStruct, valueOut, std::get<Index>(fieldMemberPtrs)...);
}

template <typename ValuesStructT, typename V, typename... VP>
void getValue(const ValuesStructT& valuesStruct, V& valueOut, const std::tuple<VP...>& fieldMemberPtrs)
{
    getValue(valuesStruct, valueOut, fieldMemberPtrs, std::index_sequence_for<VP...>());
}

struct real_alloc_t
{
    using offset_t = uint32_t;
    using subsize_t = uint16_t;
};

// Test structs
struct S1
{
    int f1;

    bool operator==(S1 const& op2) const
    {
        return f1 == op2.f1;
    }
};

template <typename T>
class CustomAllocator : public std::allocator<T>
{
  public:
    template <typename U>
    struct rebind
    {
        using other = CustomAllocator<U>;
    };
};

STRUCT_VISITABLE(S1, f1)

using MemcpyType = std::chrono::steady_clock::time_point;

// MEMCPY_SERIALIZABLE(MemcpyType, score::common::visitor::payload_tags::unsigned_le)
MEMCPY_SERIALIZABLE(payload_tags::unsigned_le, MemcpyType)

struct CustomFloat
{
    float value{0.0F};
};

bool operator==(const CustomFloat& lhs, const CustomFloat& rhs)
{
    return std::fabs(lhs.value - rhs.value) < 0.001F;
}

template <typename T>
struct is_custom_float : std::is_same<T, CustomFloat>
{
};

MEMCPY_SERIALIZABLE_IF(score::common::visitor::payload_tags::ieee754_float_le,
                       T,
                       is_custom_float<std::remove_const_t<T>>::value)

struct S2
{
    MemcpyType f1;

    bool operator==(S2 const& op2) const
    {
        return f1 == op2.f1;
    }
};

STRUCT_VISITABLE(S2, f1)

struct S3
{
    CustomFloat f1;

    bool operator==(S3 const& op2) const
    {
        return f1 == op2.f1;
    }
};

STRUCT_VISITABLE(S3, f1)

struct InitialTypeValues
{
    uint8_t uint8tValue{0};
    uint32_t uint32tValue{0};
    std::string stringValue;
    char char5Value[5];
    std::vector<int> vectorIntValue;
    std::vector<int16_t> vectorInt16Value;
    std::vector<int, CustomAllocator<int>> vectorWithAllocValue;
    clearable_container<int> clearableContainerIntValue;
    clearable_container<int, CustomAllocator<int>> clearableContainerWithAllocValue;
    resizeable_container<int> resizeableContainerIntValue;
    resizeable_container<int, CustomAllocator<int>> resizeableContainerWithAllocValue;
    assignable_container<int> assignableContainerIntValue;
    assignable_container<int, CustomAllocator<int>> assignableContainerWithAllocValue;
    std::tuple<int, int> tupleValue;
    std::pair<int, double> pairValue;
    S1 s1Value;
    S2 s2Value;
    S3 s3Value;
};

// clang-format off

auto gInitalValuePointers = std::make_tuple(
    &InitialTypeValues::uint8tValue,
    &InitialTypeValues::uint32tValue,
    &InitialTypeValues::stringValue,
    &InitialTypeValues::char5Value,
    &InitialTypeValues::vectorIntValue,
    &InitialTypeValues::vectorInt16Value,
    &InitialTypeValues::vectorWithAllocValue,
    &InitialTypeValues::clearableContainerIntValue,
    &InitialTypeValues::clearableContainerWithAllocValue,
    &InitialTypeValues::resizeableContainerIntValue,
    &InitialTypeValues::resizeableContainerWithAllocValue,
    &InitialTypeValues::assignableContainerIntValue,
    &InitialTypeValues::assignableContainerWithAllocValue,
    &InitialTypeValues::tupleValue,
    &InitialTypeValues::pairValue,
    &InitialTypeValues::s1Value,
    &InitialTypeValues::s2Value,
    &InitialTypeValues::s3Value);

InitialTypeValues const gInitialValues {
    0xFF, // uint8_t
    0xFFAF, // uint32_t
    "DummyString", // std::string
    "1234", // char[5]
    {1, 2, 3}, // std::vector<int>
    {1, 2, 3}, // std::vector<int16_t>
    {1, 2, 3}, // std::vector<int, CustomAllocator<int>>
    {1, 2, 3}, // clearable_container<int>
    {1, 2, 3}, // clearable_container<int, CustomAllocator<int>>
    {1, 2, 3}, // resizeable_container<int>
    {1, 2, 3}, // resizeable_container<int, CustomAllocator<int>>
    {1, 2, 3}, // assignable_container<int>
    {1, 2, 3}, // assignable_container<int, CustomAllocator<int>>
    {1, 2}, // std::tuple<int, int>
    {1, 2.0}, // std::pair<int, double>
    {1}, // struct S1
    {MemcpyType(std::chrono::seconds(1))}, // struct S2
    {CustomFloat{1.0F}}, // struct S3
};

using SerializedTypes = ::testing::Types<
    uint8_t,
    uint32_t,
    std::string,
    char[5],
    std::vector<int>,
    std::vector<int16_t>,
    std::vector<int, CustomAllocator<int>>,
    clearable_container<int>,
    clearable_container<int, CustomAllocator<int>>,
    resizeable_container<int>,
    resizeable_container<int, CustomAllocator<int>>,
    assignable_container<int>,
    assignable_container<int, CustomAllocator<int>>,
    std::tuple<int, int>,
    std::pair<int, double>,
    S1,
    S2,
    S3>;

// clang-format on

template <typename T>
class SizeVisitorFixture : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(SizeVisitorFixture);

TYPED_TEST_P(SizeVisitorFixture, whenDataSerializedAndThenDeserializedDataShouldBeTheSame)
{
    ::testing::Test::RecordProperty("ParentRequirement", "SCR-1633893");
    ::testing::Test::RecordProperty("ASIL", "B");
    ::testing::Test::RecordProperty("Description",
                                    "logging library shall provide an annotation mechanism for data structures to "
                                    "support automatic serialization/deserialization.");
    ::testing::Test::RecordProperty("TestingTechnique", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "Analysis of requirements");

    using s = serializer_t<real_alloc_t>;
    using ssize = serialized_size_t<real_alloc_t>;

    char buffer[1000];

    TypeParam inData;
    getValue(gInitialValues, inData, gInitalValuePointers);
    real_alloc_t::offset_t serializedDataByteSize = s::serialize(inData, buffer, sizeof(buffer));

    TypeParam outData;
    s::deserialize(buffer, sizeof(buffer), outData);
    real_alloc_t::offset_t deserializedDataByteSize = ssize::serialized_size<real_alloc_t::offset_t>(outData);
    EXPECT_EQ(deserializedDataByteSize, serializedDataByteSize);
    EXPECT_TRUE(cmp(inData, outData));
}

REGISTER_TYPED_TEST_SUITE_P(SizeVisitorFixture, whenDataSerializedAndThenDeserializedDataShouldBeTheSame);
INSTANTIATE_TYPED_TEST_SUITE_P(SizeVisitorTypeFixture, SizeVisitorFixture, SerializedTypes, /*unused*/);

}  // namespace visitor
}  // namespace common
}  // namespace score
