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
#include "score/json/internal/model/number.h"

#include "gtest/gtest.h"

#include <cfloat>
#include <limits>

namespace score
{
namespace json
{
namespace
{

template <typename Result, typename Value>
void CheckResult(Result res, Value val, bool expect_has_value)
{
    // We allow casting from double to float by user request
    // and it leads to precision loss. Because of that we can't
    // compare float and double directly and we are casting
    // 'val' to float first to verify correctess of a conversion
    constexpr bool res_is_float = std::is_same_v<Result, score::Result<float>>;
    constexpr bool val_is_double = std::is_same_v<Value, double>;
    EXPECT_EQ(res.has_value(), expect_has_value);
    if (res.has_value() && expect_has_value)
    {
        if (res_is_float && val_is_double)
        {
            EXPECT_EQ(res.value(), static_cast<float>(val));
        }
        else
        {
            EXPECT_EQ(res.value(), val);
        }
    }
}

template <typename Unsigned>
void TestFromUnsignedToAnyOtherType(bool uint8_ok,
                                    bool uint16_ok,
                                    bool uint32_ok,
                                    bool uint64_ok,
                                    bool int8_ok,
                                    bool int16_ok,
                                    bool int32_ok,
                                    bool int64_ok,
                                    bool float_ok,
                                    bool double_ok)
{
    constexpr Unsigned kMax = std::numeric_limits<Unsigned>::max();
    constexpr Unsigned kSignedMax = kMax / 2 - 1;
    constexpr Unsigned kOne = 1;
    constexpr Unsigned kZero = 0;

    CheckResult(Number{kMax}.As<uint8_t>(), kMax, uint8_ok);
    CheckResult(Number{kMax}.As<uint16_t>(), kMax, uint16_ok);
    CheckResult(Number{kMax}.As<uint32_t>(), kMax, uint32_ok);
    CheckResult(Number{kMax}.As<uint64_t>(), kMax, uint64_ok);
    CheckResult(Number{kMax}.As<int8_t>(), kMax, int8_ok);
    CheckResult(Number{kMax}.As<int16_t>(), kMax, int16_ok);
    CheckResult(Number{kMax}.As<int32_t>(), kMax, int32_ok);
    CheckResult(Number{kMax}.As<int64_t>(), kMax, int64_ok);
    CheckResult(Number{kMax}.As<float>(), kMax, float_ok);
    CheckResult(Number{kMax}.As<double>(), kMax, double_ok);
    CheckResult(Number{kMax}.As<bool>(), false, false);

    CheckResult(Number{kSignedMax}.As<std::make_signed_t<Unsigned>>(), kSignedMax, true);

    CheckResult(Number{kOne}.As<bool>(), true, true);
    CheckResult(Number{kZero}.As<bool>(), false, true);
}

TEST(Number, FromUint8ToAnyOtherType)
{
    RecordProperty("Verifies", "::score::json::Number::As");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests conversion of uint8 max value to different data types.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    TestFromUnsignedToAnyOtherType<uint8_t>(true,   // uint8
                                            true,   // uint16
                                            true,   // uint32
                                            true,   // uint64
                                            false,  // int8
                                            true,   // int16
                                            true,   // int32
                                            true,   // int64
                                            true,   // float
                                            true    // double
    );
}

TEST(Number, FromUint16ToAnyOtherType)
{
    RecordProperty("Verifies", "::score::json::Number::As");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests conversion of uint16 max value to different data types.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    TestFromUnsignedToAnyOtherType<uint16_t>(false,  // uint8
                                             true,   // uint16
                                             true,   // uint32
                                             true,   // uint64
                                             false,  // int8
                                             false,  // int16
                                             true,   // int32
                                             true,   // int64
                                             true,   // float
                                             true    // double
    );
}

TEST(Number, FromUint32ToAnyOtherType)
{
    RecordProperty("Verifies", "::score::json::Number::As");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests conversion of uint32 max value to different data types.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    TestFromUnsignedToAnyOtherType<uint32_t>(false,  // uint8
                                             false,  // uint16
                                             true,   // uint32
                                             true,   // uint64
                                             false,  // int8
                                             false,  // int16
                                             false,  // int32
                                             true,   // int64
                                             false,  // float
                                             true    // double
    );
}

TEST(Number, FromUint64ToAnyOtherType)
{
    RecordProperty("Verifies", "::score::json::Number::As");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests conversion of uint64 max value to different data types.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    TestFromUnsignedToAnyOtherType<uint64_t>(false,  // uint8
                                             false,  // uint16
                                             false,  // uint32
                                             true,   // uint64
                                             false,  // int8
                                             false,  // int16
                                             false,  // int32
                                             false,  // int64
                                             false,  // float
                                             false   // double
    );
}

template <typename Signed>
void TestFromSignedToAnyOtherType(bool int8_ok,
                                  bool int16_ok,
                                  bool int32_ok,
                                  bool int64_ok,
                                  bool float_ok,
                                  bool double_ok)
{
    // The parser should only return a signed integer if the value is negative.
    // So any cast to an unsigend shall fail if the number contains a signed integer.

    constexpr Signed kMax = std::numeric_limits<int8_t>::max();
    constexpr Signed kMin = std::numeric_limits<int8_t>::min();
    constexpr Signed kSmall = 100;
    constexpr Signed kOne = 1;
    constexpr Signed kZero = 0;

    CheckResult(Number{kMax}.As<uint8_t>(), 0, false);
    CheckResult(Number{kMax}.As<uint16_t>(), 0, false);
    CheckResult(Number{kMax}.As<uint32_t>(), 0, false);
    CheckResult(Number{kMax}.As<uint64_t>(), 0, false);
    CheckResult(Number{kMax}.As<int8_t>(), kMax, int8_ok);
    CheckResult(Number{kMax}.As<int16_t>(), kMax, int16_ok);
    CheckResult(Number{kMax}.As<int32_t>(), kMax, int32_ok);
    CheckResult(Number{kMax}.As<int64_t>(), kMax, int64_ok);
    CheckResult(Number{kMax}.As<bool>(), false, false);

    CheckResult(Number{kMin}.As<uint8_t>(), 0, false);
    CheckResult(Number{kMin}.As<uint16_t>(), 0, false);
    CheckResult(Number{kMin}.As<uint32_t>(), 0, false);
    CheckResult(Number{kMin}.As<uint64_t>(), 0, false);
    CheckResult(Number{kMin}.As<int8_t>(), kMin, int8_ok);
    CheckResult(Number{kMin}.As<int16_t>(), kMin, int16_ok);
    CheckResult(Number{kMin}.As<int32_t>(), kMin, int32_ok);
    CheckResult(Number{kMin}.As<int64_t>(), kMin, int64_ok);
    CheckResult(Number{kMin}.As<bool>(), false, false);

    CheckResult(Number{kSmall}.As<uint8_t>(), 0, false);
    CheckResult(Number{kSmall}.As<uint16_t>(), 0, false);
    CheckResult(Number{kSmall}.As<uint32_t>(), 0, false);
    CheckResult(Number{kSmall}.As<uint64_t>(), 0, false);
    CheckResult(Number{kSmall}.As<int8_t>(), kSmall, int8_ok);
    CheckResult(Number{kSmall}.As<int16_t>(), kSmall, int16_ok);
    CheckResult(Number{kSmall}.As<int32_t>(), kSmall, int32_ok);
    CheckResult(Number{kSmall}.As<int64_t>(), kSmall, int64_ok);
    CheckResult(Number{kSmall}.As<bool>(), false, false);

    CheckResult(Number{kMax}.As<float>(), kMax, float_ok);
    CheckResult(Number{kMax}.As<double>(), kMax, double_ok);

    CheckResult(Number{kMin}.As<float>(), kMin, float_ok);
    CheckResult(Number{kMin}.As<double>(), kMin, double_ok);

    CheckResult(Number{kOne}.As<bool>(), true, true);
    CheckResult(Number{kZero}.As<bool>(), false, true);
}

TEST(Number, FromInt8ToAnyOtherType)
{
    RecordProperty("Verifies", "::score::json::Number::As");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests conversion of int8 different values to different data types.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    TestFromSignedToAnyOtherType<std::int8_t>(true,  // int8
                                              true,  // int16
                                              true,  // int32
                                              true,  // int64
                                              true,  // float
                                              true   // double
    );
}

TEST(Number, FromInt16ToAnyOtherType)
{
    RecordProperty("Verifies", "::score::json::Number::As");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests conversion of int16 different values to different data types.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    TestFromSignedToAnyOtherType<std::int16_t>(false,  // int8
                                               true,   // int16
                                               true,   // int32
                                               true,   // int64
                                               true,   // float
                                               true    // double
    );
}

TEST(Number, FromInt32ToAnyOtherType)
{
    RecordProperty("Verifies", "::score::json::Number::As");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests conversion of int32 different values to different data types.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    TestFromSignedToAnyOtherType<std::int32_t>(false,  // int8
                                               false,  // int16
                                               true,   // int32
                                               true,   // int64
                                               false,  // float
                                               true    // double
    );
}

TEST(Number, FromInt64ToAnyOtherType)
{
    RecordProperty("Verifies", "::score::json::Number::As");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests conversion of int64 different values to different data types.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    TestFromSignedToAnyOtherType<std::int64_t>(false,  // int8
                                               false,  // int16
                                               false,  // int32
                                               true,   // int64
                                               false,  // float
                                               false   // double
    );
}

template <typename Input>
void TestValueToAnyOtherType(Input input,
                             bool uint8_ok,
                             bool uint16_ok,
                             bool uint32_ok,
                             bool uint64_ok,
                             bool int8_ok,
                             bool int16_ok,
                             bool int32_ok,
                             bool int64_ok,
                             bool float_ok,
                             bool double_ok,
                             bool bool_ok)
{
    CheckResult(Number{input}.As<uint8_t>(), input, uint8_ok);
    CheckResult(Number{input}.As<uint16_t>(), input, uint16_ok);
    CheckResult(Number{input}.As<uint32_t>(), input, uint32_ok);
    CheckResult(Number{input}.As<uint64_t>(), input, uint64_ok);
    CheckResult(Number{input}.As<int8_t>(), input, int8_ok);
    CheckResult(Number{input}.As<int16_t>(), input, int16_ok);
    CheckResult(Number{input}.As<int32_t>(), input, int32_ok);
    CheckResult(Number{input}.As<int64_t>(), input, int64_ok);
    CheckResult(Number{input}.As<float>(), input, float_ok);
    CheckResult(Number{input}.As<bool>(), input, bool_ok);

    // Floating-point numbers are always stored as double. Having input as a float
    // and trying to get it as a double throws assertion since this should be unreachable case.
    if (std::is_same<Input, float>::value == true)
    {
        EXPECT_DEATH(Number{input}.As<double>(), "");
    }
    else
    {
        CheckResult(Number{input}.As<double>(), input, double_ok);
    }
}

constexpr float GetMaximumIntegerInFloat()
{
    // Returns the maximum integer that can be represented in float without precision loss.
    static_assert(std::numeric_limits<float>::radix == 2, "only float with  binary radix supported");

    return 1UL << std::numeric_limits<float>::digits;
}

constexpr double GetMaximumIntegerInDouble()
{
    // Returns the maximum integer that can be represented in double without precision loss.
    static_assert(std::numeric_limits<float>::radix == 2, "only float with  binary radix supported");
    return 1UL << std::numeric_limits<double>::digits;
}

template <typename FloatingPoint>
void TestFloatingPointValues(FloatingPoint maximum_integer)
{
    bool float_ok = std::is_same<FloatingPoint, float>::value;
    bool maximum_int_in_i32_range =
        static_cast<uint64_t>(maximum_integer) <= static_cast<uint64_t>(std::numeric_limits<std::int32_t>::max());

    TestValueToAnyOtherType<FloatingPoint>(std::numeric_limits<FloatingPoint>::max(),
                                           false,     // uint8
                                           false,     // uint16
                                           false,     // uint32
                                           false,     // uint64
                                           false,     // int8
                                           false,     // int16
                                           false,     // int32
                                           false,     // int64
                                           float_ok,  // float
                                           true,      // double
                                           false      // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(std::numeric_limits<FloatingPoint>::min(),
                                           false,  // uint8
                                           false,  // uint16
                                           false,  // uint32
                                           false,  // uint64
                                           false,  // int8
                                           false,  // int16
                                           false,  // int32
                                           false,  // int64
                                           true,   // float
                                           true,   // double
                                           false   // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(std::numeric_limits<FloatingPoint>::lowest(),
                                           false,     // uint8
                                           false,     // uint16
                                           false,     // uint32
                                           false,     // uint64
                                           false,     // int8
                                           false,     // int16
                                           false,     // int32
                                           false,     // int64
                                           float_ok,  // float
                                           true,      // double
                                           false      // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(std::numeric_limits<std::uint8_t>::max(),
                                           true,   // uint8
                                           true,   // uint16
                                           true,   // uint32
                                           true,   // uint64
                                           false,  // int8
                                           true,   // int16
                                           true,   // int32
                                           true,   // int64
                                           true,   // float
                                           true,   // double
                                           false   // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(std::numeric_limits<std::uint16_t>::max(),
                                           false,  // uint8
                                           true,   // uint16
                                           true,   // uint32
                                           true,   // uint64
                                           false,  // int8
                                           false,  // int16
                                           true,   // int32
                                           true,   // int64
                                           true,   // float
                                           true,   // double
                                           false   // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(static_cast<FloatingPoint>(std::numeric_limits<std::uint32_t>::max()),
                                           false,                      // uint8
                                           false,                      // uint16
                                           !maximum_int_in_i32_range,  // uint32
                                           !maximum_int_in_i32_range,  // uint64
                                           false,                      // int8
                                           false,                      // int16
                                           false,                      // int32
                                           !maximum_int_in_i32_range,  // int64
                                           true,                       // float
                                           true,                       // double
                                           false                       // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(static_cast<FloatingPoint>(std::numeric_limits<std::uint64_t>::max()),
                                           false,  // uint8
                                           false,  // uint16
                                           false,  // uint32
                                           false,  // uint64
                                           false,  // int8
                                           false,  // int16
                                           false,  // int32
                                           false,  // int64
                                           true,   // float
                                           true,   // double
                                           false   // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(std::numeric_limits<std::int8_t>::max(),
                                           true,  // uint8
                                           true,  // uint16
                                           true,  // uint32
                                           true,  // uint64
                                           true,  // int8
                                           true,  // int16
                                           true,  // int32
                                           true,  // int64
                                           true,  // float
                                           true,  // double
                                           false  // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(std::numeric_limits<std::int8_t>::min(),
                                           false,  // uint8
                                           false,  // uint16
                                           false,  // uint32
                                           false,  // uint64
                                           true,   // int8
                                           true,   // int16
                                           true,   // int32
                                           true,   // int64
                                           true,   // float
                                           true,   // double
                                           false   // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(std::numeric_limits<std::int16_t>::max(),
                                           false,  // uint8
                                           true,   // uint16
                                           true,   // uint32
                                           true,   // uint64
                                           false,  // int8
                                           true,   // int16
                                           true,   // int32
                                           true,   // int64
                                           true,   // float
                                           true,   // double
                                           false   // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(std::numeric_limits<std::int16_t>::min(),
                                           false,  // uint8
                                           false,  // uint16
                                           false,  // uint32
                                           false,  // uint64
                                           false,  // int8
                                           true,   // int16
                                           true,   // int32
                                           true,   // int64
                                           true,   // float
                                           true,   // double
                                           false   // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(static_cast<FloatingPoint>(std::numeric_limits<std::int32_t>::max()),
                                           false,                      // uint8
                                           false,                      // uint16
                                           !maximum_int_in_i32_range,  // uint32
                                           !maximum_int_in_i32_range,  // uint64
                                           false,                      // int8
                                           false,                      // int16
                                           !maximum_int_in_i32_range,  // int32
                                           !maximum_int_in_i32_range,  // int64
                                           true,                       // float
                                           true,                       // double
                                           false                       // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(static_cast<FloatingPoint>(std::numeric_limits<std::int32_t>::min()),
                                           false,                      // uint8
                                           false,                      // uint16
                                           false,                      // uint32
                                           false,                      // uint64
                                           false,                      // int8
                                           false,                      // int16
                                           !maximum_int_in_i32_range,  // int32
                                           !maximum_int_in_i32_range,  // int64
                                           true,                       // float
                                           true,                       // double
                                           false                       // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(maximum_integer,
                                           false,                     // uint8
                                           false,                     // uint16
                                           maximum_int_in_i32_range,  // uint32
                                           true,                      // uint64
                                           false,                     // int8
                                           false,                     // int16
                                           maximum_int_in_i32_range,  // int32
                                           true,                      // int64
                                           true,                      // float
                                           true,                      // double
                                           false                      // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(-maximum_integer,
                                           false,                     // uint8
                                           false,                     // uint16
                                           false,                     // uint32
                                           false,                     // uint64
                                           false,                     // int8
                                           false,                     // int16
                                           maximum_int_in_i32_range,  // int32
                                           true,                      // int64
                                           true,                      // float
                                           true,                      // double
                                           false                      // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(FloatingPoint{1.0},
                                           true,  // uint8
                                           true,  // uint16
                                           true,  // uint32
                                           true,  // uint64
                                           true,  // int8
                                           true,  // int16
                                           true,  // int32
                                           true,  // int64
                                           true,  // float
                                           true,  // double
                                           true   // bool
    );

    TestValueToAnyOtherType<FloatingPoint>(FloatingPoint{0.0},
                                           true,  // uint8
                                           true,  // uint16
                                           true,  // uint32
                                           true,  // uint64
                                           true,  // int8
                                           true,  // int16
                                           true,  // int32
                                           true,  // int64
                                           true,  // float
                                           true,  // double
                                           true   // bool
    );
}

TEST(Number, FromFloatToAnyOtherType)
{
    RecordProperty("Verifies", "::score::json::Number::As");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests conversion of float values to different data types.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    TestFloatingPointValues<float>(GetMaximumIntegerInFloat());
}

TEST(Number, FromDoubleToAnyOtherType)
{
    RecordProperty("Verifies", "::score::json::Number::As");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests conversion of double values to different data types.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    TestFloatingPointValues<double>(GetMaximumIntegerInDouble());
}

template <typename T1, typename T2>
void TestEqualOperator(T1 lhs, T2 rhs, bool result)
{
    EXPECT_EQ(Number(lhs) == Number(rhs), result);
}

TEST(Number, CheckEqualOperator)
{
    RecordProperty("Verifies", "::score::json::Number::operator==");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests the equal comparator of Number.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");
    {
        uint64_t content = 42;
        uint64_t wrong_content = 43;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, static_cast<uint32_t>(content), false);
    }
    {
        uint32_t content = 42;
        uint32_t wrong_content = 43;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, static_cast<uint16_t>(content), false);
    }
    {
        uint16_t content = 42;
        uint16_t wrong_content = 43;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, static_cast<uint8_t>(content), false);
    }
    {
        uint8_t content = 42;
        uint8_t wrong_content = 43;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, static_cast<int8_t>(content), false);
    }

    {
        int64_t content = -42;
        int64_t wrong_content = -43;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, static_cast<int32_t>(content), false);
    }
    {
        int32_t content = -42;
        int32_t wrong_content = -43;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, static_cast<int16_t>(content), false);
    }

    {
        int16_t content = -42;
        int16_t wrong_content = -43;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, static_cast<int8_t>(content), false);
    }

    {
        int8_t content = -42;
        int8_t wrong_content = -43;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, static_cast<int64_t>(content), false);
    }

    {
        float content = 42.0;
        float wrong_content = 43.0;
        ;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, static_cast<double>(content), false);
    }

    {
        double content = -42.0;
        double wrong_content = -43.0;
        ;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, static_cast<float>(content), false);
    }
}

}  // namespace
}  // namespace json
}  // namespace score
