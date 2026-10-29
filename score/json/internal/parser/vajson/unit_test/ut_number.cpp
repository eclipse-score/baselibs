/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
/*!        \file
 *        \brief  Unit tests for Number
 *
 *********************************************************************************************************************/
#include <gtest/gtest.h>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

#include "score/json/internal/parser/vajson/util/number.h"
#include "score/json/internal/parser/vajson/util/types.h"
#include "score/language/futurecpp/include/score/optional.hpp"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{
/*!
 * \brief           Unqualified access to gtest classes
 */

using namespace ::testing;  // NOLINT(build/namespaces)

/*!
 * \brief           Unqualified access to the string view
 */
using view = std::string_view;

/*!
 * \brief           A ASCII number
 */
using UUT = JsonNumber;

namespace
{

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
constexpr inline auto CompareFloat(T lhs, T rhs) noexcept -> bool
{
    return (std::abs(lhs - rhs) < std::numeric_limits<T>::epsilon());
}

/*!
 * \brief           Creates an Optional from a Result
 * \tparam          T
 *                  Type of value.
 * \tparam          E
 *                  Type of error.
 * \tparam          Opt
 *                  Type of Optional.
 * \param[in]       result
 *                  to convert.
 * \return          The Optional containing the value of the Result, or an empty Optional if the Result contains an
 *                  error.
 */
template <typename T, typename Opt = score::cpp::optional<T>>
auto ToOptional(const Result<T>& result) noexcept -> Opt
{
    Opt optional{score::cpp::nullopt};
    if (result.has_value())
    {
        optional.emplace(result.value());
    }
    return optional;
}

/*!
 * \brief           Small abstraction for test values consisting of a string & number value
 */
template <typename T>
class TestValue
{
  public:
    /*!
     * \brief           Constructor from string - view will end with \0
     * \param[in]       str
     *                  the string to be used for the conversion
     * \param[in]       expected
     *                  the expected converted value
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    TestValue(std::string&& str, T expected) : value_{str}, with_zero_{true}, expected_{expected}
    {
        ShortenView();
    }

    /*!
     * \brief           Constructor from string - view will end with \0
     * \param[in]       str
     *                  the string to be used for the conversion
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    explicit TestValue(std::string&& str) : value_{str}, with_zero_{true}, expected_{}
    {
        ShortenView();
    }

    /*!
     * \brief           Get a TestValue without \0
     * \param[in]       str
     *                  the string to convert
     * \return          a constructed TestValue
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    static TestValue<T> Nonzeroed(std::string&& str)
    {
        return TestValue<T>(std::move(str), false, score::cpp::optional<T>{});
    }

    /*!
     * \brief           Get a TestValue without \0
     * \param[in]       str
     *                  the string to convert
     * \param[in]       expected
     *                  the expected converted value
     * \return          a constructed TestValue
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    static TestValue<T> Nonzeroed(std::string&& str, T expected)
    {
        return TestValue<T>(std::move(str), false, score::cpp::optional<T>{expected});
    }

    /*!
     * \brief           Gets a view of the string with potentially manipulated length
     */
    view View() const
    {
        return view(this->mod_value_.data(), this->mod_value_.size());
    }

    /*!
     * \brief           Gets the expected output from the Conversion function
     */
    score::cpp::optional<T> Expected() const
    {
        return this->expected_;
    }

    /*!
     * \brief           Enable GTest to print the type without resorting to bytes
     * \param[in]       os
     *                  the out stream
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    void Debug(::std::ostream& os) const noexcept
    {
        auto v = this->View();
        auto exp = this->Expected();
        os << "Value{Input: " << std::string(v.data(), v.size());
        if (exp.has_value())
        {
            os << ", Expected: " << static_cast<std::int64_t>(exp.value());
        }
        os << "}";
    }

  private:
    /*!
     * \brief           Constructor from string
     * \param[in]       str
     *                  the string to be used for the conversion
     * \param[in]       with_zero
     *                  if true, the whole string will be used (with \0)
     * \param[in]       expected
     *                  the expected converted value
     * \context         ANY
     * \pre             -
     * \threadsafe      TRUE, for different this pointer
     */
    TestValue(std::string&& str, bool with_zero, score::cpp::optional<T>&& expected)
        : value_{str}, with_zero_{with_zero}, expected_{std::move(expected)}
    {
        ShortenView();
    }

    /*!
     * \brief           Shorten the data and the length of the input by 1
     */
    void ShortenView() noexcept
    {
        std::size_t len = this->value_.size();
        if (!this->with_zero_)
        {
            len -= 1;
        }
        this->mod_value_ = this->value_.substr(0, len);
    }

    /*!
     * \brief           the stored string
     */
    std::string value_;

    /*!
     * \brief           The modified string
     */
    std::string mod_value_{};

    /*!
     * \brief           should have zero?
     */
    bool with_zero_;

    /*!
     * \brief           The expected value
     */
    score::cpp::optional<T> expected_;
};

/*!
 * \brief           Enable GTest to print the type without resorting to bytes
 * \param[in]       os
 *                  the out stream
 * \param[in]       val
 *                  the testvalue to print
 * \return          the out stream
 * \context         ANY
 * \pre             -
 * \threadsafe      TRUE, for different this pointer
 */
template <typename T>
::std::ostream& operator<<(::std::ostream& os, const TestValue<T>& val)
{
    val.Debug(os);
    return os;
}

/*!
 * \brief           Fixture for all parameterized number tests
 */
template <typename Num, typename = typename std::enable_if_t<std::is_arithmetic<Num>::value>>
class NumberTest : public testing::TestWithParam<TestValue<Num>>
{
  protected:
    /*!
     * \brief           Sets up the fixture
     */
    void SetUp() noexcept override
    {
        params_.emplace(this->GetParam());
    }
    /*!
     * \brief           Gets the Input into the Conversion function
     */
    view Input() const
    {
        return this->params_->View();
    }

    /*!
     * \brief           Gets the constructed number
     */
    UUT GetNumber() const
    {
        return UUT(Input());
    }

    /*!
     * \brief           Gets the expected output from the Conversion function
     */
    Optional<Num> Expected() const
    {
        return this->params_->Expected();
    }

    /*!
     * \brief           Gets the actual output from the Conversion function As<Signed>
     */
    template <typename Signed = Num, typename = typename std::enable_if_t<std::is_signed<Signed>::value>>
    Optional<Signed> ActualSigned() const
    {
        return ToOptional(UUT::New(Input()).and_then([](UUT num) noexcept {
            return num.TryAs<Signed>();
        }));
    }

    /*!
     * \brief           Gets the actual output from the Conversion function As<Unsigned>
     */
    template <typename Unsigned = Num, typename = typename std::enable_if_t<std::is_unsigned<Unsigned>::value>>
    Optional<Unsigned> ActualUnsigned() const
    {
        return ToOptional(UUT::New(Input()).and_then([](UUT num) noexcept {
            return num.TryAs<Unsigned>();
        }));
    }

    /*!
     * \brief           Gets the actual output from the Conversion function As<Float>
     */
    template <typename Float = Num, typename = typename std::enable_if_t<std::is_floating_point<Float>::value>>
    Optional<Float> ActualFloat() const
    {
        return ToOptional(UUT::New(Input()).and_then([](UUT num) noexcept {
            return num.TryAs<Float>();
        }));
    }

    /*!
     * \brief           Container for test in/out parameters
     */
    Optional<TestValue<Num>> params_;
};

class UT__Number : public ::testing::Test
{
};
}  // namespace

/*!
 * \brief           Tests if IsDigit works as expected.
 * \trace           CREQ-Json-ProvidedSerializers
 */
TEST_F(UT__Number, IsDigit)
{
    ASSERT_FALSE(internal::util::IsDigit('/'));
    ASSERT_TRUE(internal::util::IsDigit('0'));
    ASSERT_TRUE(internal::util::IsDigit('9'));
    ASSERT_FALSE(internal::util::IsDigit(':'));
}

/*
 * General & Misc Tests
 */

/*!
 * \brief           Tests if ones ans zeros can be parsed into boolean values
 * \trace           CREQ-Json-ProvidedSerializers
 */
TEST_F(UT__Number, OnesAndZerosCanBeParsedToBool)
{
    Result<bool> result{UUT::New("1").and_then([](JsonNumber num) noexcept {
        return num.TryAs<bool>();
    })};
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), true);

    result = UUT::New("0").and_then([](JsonNumber num) noexcept {
        return num.TryAs<bool>();
    });
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), false);

    result = UUT::New("2").and_then([](JsonNumber num) noexcept {
        return num.TryAs<bool>();
    });
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);

    result = UUT::New("-1").and_then([](JsonNumber num) noexcept {
        return num.TryAs<bool>();
    });
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
}

/*!
 * \brief           Tests that errno is unset before the next conversion
 * \trace           score::json::vajson::JsonNumber::New
 */
TEST_F(UT__Number, ErrnoIsUnsetBeforeConversion)
{
    Result<std::uint64_t> result_64{UUT::New("0x1FFFFFFFFFFFFFFFF").and_then([](JsonNumber num) noexcept {
        return num.TryAs<std::uint64_t>();
    })};
    ASSERT_FALSE(result_64.has_value());
    ASSERT_EQ(result_64.error(), JsonErrc::kInvalidJson);

    Result<std::uint8_t> result_8{UUT::New("1").and_then([](JsonNumber num) noexcept {
        return num.TryAs<std::uint8_t>();
    })};
    ASSERT_TRUE(result_8.has_value());
    ASSERT_EQ(result_8.value(), 1);

    result_64 = UUT::New("0x1FFFFFFFFFFFFFFFF").and_then([](JsonNumber num) noexcept {
        return num.TryAs<std::uint64_t>();
    });
    ASSERT_FALSE(result_64.has_value());
    ASSERT_EQ(result_64.error(), JsonErrc::kInvalidJson);

    result_8 = UUT::New("1").and_then([](JsonNumber num) noexcept {
        return num.TryAs<std::uint8_t>();
    });
    ASSERT_TRUE(result_8.has_value());
    ASSERT_EQ(result_8.value(), 1);

    const Result<std::double_t> result_d{UUT::New("this_is_no_double").and_then([](JsonNumber num) noexcept {
        return num.TryAs<std::double_t>();
    })};
    ASSERT_FALSE(result_d.has_value());
    ASSERT_EQ(result_d.error(), JsonErrc::kInvalidJson);

    result_8 = UUT::New("1").and_then([](JsonNumber num) noexcept {
        return num.TryAs<std::uint8_t>();
    });
    ASSERT_TRUE(result_8.has_value());
    ASSERT_EQ(result_8.value(), 1);
}

/*!
 * \brief           Tests if numbers can be manually parsed by the user
 * \trace           score::json::vajson::JsonNumber::New
 */
TEST_F(UT__Number, StringsCanBeCustomParsed)
{
    view sv = "1234567890";
    Result<UUT> uut{UUT::New(sv)};
    ASSERT_TRUE(uut.has_value());

    const auto result = uut.value().Convert([&sv](view v) noexcept {
        EXPECT_EQ(v, sv);
        return Optional<int>{10};
    });
    EXPECT_EQ(result, Optional<int>{10});
}

/*!
 * \brief           Tests if numbers can be requested as JsonNumber
 * \trace           score::json::vajson::JsonNumber
 */
TEST_F(UT__Number, AsJsonNumber)
{
    view sv = "1234567890";
    Result<UUT> uut{UUT::New(sv)};
    ASSERT_TRUE(uut.has_value());

    const auto result = uut.value().As<JsonNumber>();
    ASSERT_TRUE(result.has_value());
}

/*!
 * Tests that certain invalid numbers are detected.
 *
 * - Assert that octal numbers are detected as invalid.
 * - Assert that a single minus sign is detected as invalid.
 * - Assert that an empty view is detected as invalid.
 * - Assert that a number to a negative char is detected as invalid.
 * \trace           score::json::vajson::JsonNumber::New
 */
TEST_F(UT__Number, InvalidNumber)
{
    {
        view sv = "0a";
        Result<UUT> uut{UUT::New(sv)};
        ASSERT_FALSE(uut.has_value());
        ASSERT_EQ(uut.error(), JsonErrc::kInvalidJson);
    }
    {
        view sv = "-";
        Result<UUT> uut{UUT::New(sv)};
        ASSERT_FALSE(uut.has_value());
        ASSERT_EQ(uut.error(), JsonErrc::kInvalidJson);
    }
    {
        view sv = "";
        Result<UUT> uut{UUT::New(sv)};
        ASSERT_FALSE(uut.has_value());
        ASSERT_EQ(uut.error(), JsonErrc::kInvalidJson);
    }
    {
        // Using an extended ASCII character does not work since this document is UTF-8 encoded.
        unsigned char ch{'a'};
        // 'a' is 0b0110'0001 so after the shift the most significant bit is 1.
        ch = static_cast<unsigned char>(ch << 1);
        const std::string str(1, static_cast<char>(ch));
        view sv{str};

        Result<UUT> uut{UUT::New(sv)};
        ASSERT_FALSE(uut.has_value());
        ASSERT_EQ(uut.error(), JsonErrc::kInvalidJson);
    }
}

/*
 * Signed integer tests
 */

/*!
 * \brief           Test value for Int8
 */
using I8Value = TestValue<std::int8_t>;
/*!
 * \brief           Test fixture for Int8 Success
 */
class UT__Number__I8Success : public NumberTest<std::int8_t>
{
};
/*!
 * \brief           Test fixture for Int8 Failure
 */
class UT__Number__I8Failure : public NumberTest<std::int8_t>
{
};

/*!
 * \brief           Test that checks that conversions succeed if valid values are passed
 * \trace           score::json::vajson::JsonNumber::TryAs
 */
TEST_P(UT__Number__I8Success, ConvertsValidIntegers8bit)
{
    auto actual = ActualSigned();
    ASSERT_TRUE(actual.has_value());
    EXPECT_EQ(actual, Expected());
}

/*!
 * \brief           Instanciation of testcases for multiple values with successful conversion
 * \details         Input data:
 *                  - Positive and negative integers within the range of a std::int8_t, including min, max,
 *                    intermediate, and zero values
 */
INSTANTIATE_TEST_SUITE_P(SignedIntTest8bit,
                         UT__Number__I8Success,
                         Values(I8Value::Nonzeroed("0,", 0),
                                I8Value::Nonzeroed("1;", 1),
                                I8Value::Nonzeroed("-1;", -1),
                                I8Value("0", 0),
                                I8Value("-0", 0),
                                I8Value("-1", -1),
                                I8Value("1", 1),
                                I8Value("-128", -128),
                                I8Value("127", 127)));

/*!
 * \brief           Test that checks that conversions fail if invalid values are passed
 * \trace           score::json::vajson::JsonNumber::TryAs
 */
TEST_P(UT__Number__I8Failure, FailsOnInvalidIntegers8bit)
{
    auto actual = ActualSigned();

    ASSERT_FALSE(actual.has_value());
}

/*!
 * \brief           Instanciation of testcases for multiple values where conversion fails
 * \details         Input data:
 *                  - Numbers that cannot be converted to a std::int8_t, such as floating point, scientific notation,
 *                    invalid characters, and to integers that are too big
 */
INSTANTIATE_TEST_SUITE_P(SignedIntTestFail8bit,
                         UT__Number__I8Failure,
                         Values(I8Value::Nonzeroed(";,"),
                                I8Value::Nonzeroed("1.01;"),
                                I8Value::Nonzeroed("-1e-2;"),
                                I8Value("0.0"),
                                I8Value("-0.0"),
                                I8Value("128"),
                                I8Value("-129")));

/*
 * Unsigned integer tests
 */

/*!
 * \brief           Test value for Uint8
 */
using U8Value = TestValue<std::uint8_t>;
/*!
 * \brief           Test fixture for Int8 Success
 */
class UT__Number__U8Success : public NumberTest<std::uint8_t>
{
};
/*!
 * \brief           Test fixture for Int8 Failure
 */
class UT__Number__U8Failure : public NumberTest<std::uint8_t>
{
};

/*!
 * \brief           Test that checks that conversions succeed if valid values are passed
 * \trace           score::json::vajson::JsonNumber::TryAs
 */
TEST_P(UT__Number__U8Success, ConvertsValidIntegers8bit)
{
    auto actual = ActualUnsigned();
    ASSERT_TRUE(actual.has_value());
    EXPECT_EQ(actual, Expected());
}

/*!
 * \brief           Instanciation of testcases for multiple values with successful conversion
 * \details         Input data:
 *                  - Positive integers within the range of a std::uint8_t including min, max, intermediate, and zero
 *                    values
 */
INSTANTIATE_TEST_SUITE_P(UnsignedIntTest8bit,
                         UT__Number__U8Success,
                         Values(U8Value::Nonzeroed("0,", 0),
                                U8Value::Nonzeroed("1;", 1),
                                U8Value("0", 0),
                                U8Value("1", 1),
                                U8Value("127", 127),
                                U8Value("255", 255)));

/*!
 * \brief           Test that checks that conversions fail if invalid values are passed
 * \trace           CREQ-Json-Validation
 */
TEST_P(UT__Number__U8Failure, FailsOnInvalidIntegers8bit)
{
    auto actual = ActualUnsigned();

    ASSERT_FALSE(actual.has_value());
}

/*!
 * \brief           Instanciation of testcases for multiple values where conversion fails
 * \details         Input data:
 *                  - Numbers that cannot be converted to a std::uint8_t, such as negative values, floating point,
 *                    scientific notation, invalid characters, and integers that are too big
 */
INSTANTIATE_TEST_SUITE_P(UnsignedIntTestFail8bit,
                         UT__Number__U8Failure,
                         Values(U8Value::Nonzeroed("-1;"),
                                U8Value::Nonzeroed(";,"),
                                U8Value::Nonzeroed("1.01;"),
                                U8Value::Nonzeroed("-1e-2;"),
                                U8Value("-0"),
                                U8Value("-1"),
                                U8Value("0.0"),
                                U8Value("-0.0"),
                                U8Value("256")));

/*
 * Floating point number tests
 */

/*!
 * \brief           Test value for Double
 */
using DoubleValue = TestValue<double>;
/*!
 * \brief           Test fixture for Int8 Success
 */
class UT__Number__DoubleSuccess : public NumberTest<double>
{
};
/*!
 * \brief           Test fixture for Int8 Failure
 */
class UT__Number__DoubleFailure : public NumberTest<double>
{
};

/*!
 * \brief           Test that checks that conversions succeed if valid values are passed
 * \trace           score::json::vajson::JsonNumber::TryAs
 */
TEST_P(UT__Number__DoubleSuccess, ConvertsValidDoubles)
{
    auto actual = ActualFloat();
    ASSERT_TRUE(actual.has_value());
    ASSERT_TRUE(CompareFloat(actual.value(), Expected().value()));
}

/*!
 * \brief           Instanciation of testcases for multiple values with successful conversion
 * \details         Input data:
 *                  - Positive and negative numbers, such as integers and floats, including zero
 */
INSTANTIATE_TEST_SUITE_P(FloatTestDouble,
                         UT__Number__DoubleSuccess,
                         Values(DoubleValue("0", 0),
                                DoubleValue::Nonzeroed("1;", 1),
                                DoubleValue::Nonzeroed("-1;", -1),
                                DoubleValue("0", 0),
                                DoubleValue("-0", 0),
                                DoubleValue("-1", -1),
                                DoubleValue("1", 1),
                                DoubleValue::Nonzeroed("1.01;", 1.01),
                                DoubleValue::Nonzeroed("-1e-2;", -0.01),
                                DoubleValue("0.0", 0.0),
                                DoubleValue("-0.0", 0.0),
                                DoubleValue::Nonzeroed("-0.1E+2;", -10)));

/*!
 * \brief           Test that checks that conversions fail if invalid values are passed
 * \trace           score::json::vajson::JsonNumber::TryAs
 */
TEST_P(UT__Number__DoubleFailure, FailsOnInvalidDoubles)
{
    auto actual = ActualFloat();

    ASSERT_FALSE(actual.has_value());
}

/*!
 * \brief           Instanciation of testcases for multiple values where conversion fails
 * \details         Input data:
 *                  - Numbers that cannot be converted to a double, such as invalid characters, bases without exponent,
 *                    exponent with a base of zero, exponent in octal notation
 */
INSTANTIATE_TEST_SUITE_P(FloatTestFailDouble,
                         UT__Number__DoubleFailure,
                         Values(DoubleValue("-0."),
                                DoubleValue::Nonzeroed(";,"),
                                DoubleValue("0e1.2"),
                                DoubleValue("1.2e0x1"),
                                DoubleValue("0e"),
                                DoubleValue("0e+"),
                                DoubleValue("0e+ - 1")));

namespace
{

template <typename T>
std::optional<T> ParseNumberAs(const std::string& json_number)
{
    const auto result = score::json::vajson::JsonNumber::New(json_number).value().As<T>();
    if (result.has_value())
    {
        return result.value();
    }
    return {};
}

/**
 * @brief Smoke testing the basic ranges plus underflow and overflow.
 * Note: Exhausitve parser testing shall be done by the supplier of the underlying JSON parser.
 */
template <typename T>
void range_test(const ::std::string& underflow, const std::string& overflow)
{
    EXPECT_FALSE(ParseNumberAs<T>(underflow).has_value());
    EXPECT_FALSE(ParseNumberAs<T>(overflow).has_value());
}

TEST(Number, Bool)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of bool data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<bool>("-1", "2");
}

TEST(Number, Uint8)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint8 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::uint8_t>("-1", "256");
}

TEST(Number, Uint16)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint16 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::uint16_t>("-1", "65536");
}

TEST(Number, Uint32)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint32 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::uint32_t>("-1", "4294967296");
}

TEST(Number, Uint64)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint64 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::uint64_t>("-1", "18446744073709551616");
}

TEST(Number, Int8)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int8 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::int8_t>("-129", "128");
}

TEST(Number, Int16)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int16 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::int16_t>("-32769", "32768");
}

TEST(Number, Int32)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int32 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::int32_t>("-2147483649", "2147483648");
}

TEST(Number, Int64)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int64 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::int64_t>("-9223372036854775809", "9223372036854775808");
}

TEST(Number, Float)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of float data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<float>("-3.402823476385288598117e+38", "3.402823476385288598117e+38");
}

TEST(Number, Double)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of double data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<double>("-1.797693134862415708145e+308", "1.797693134862415708145e+308");
}

TEST(Number, WithDecimalPointWithoutFractionalPartCannotBeParsedAsInteger)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test interpreting number with decimal point as floating point only, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");                // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis");  // requirements

    // Tolerated behavior of vajson that interprets any number with decimal point as floating point only.
    EXPECT_EQ(ParseNumberAs<int64_t>("-1.0").has_value(), false);
}

TEST(Number, WithExponentialNotationWithoutFractionalPartCannotBeParsedAsInteger)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test interpreting number with exponent notation as floating point only, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");                // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis");  // requirements

    // Tolerated behavior of vajson that interprets any number with exponent notation as floating point only.
    EXPECT_EQ(ParseNumberAs<int64_t>("-1e2").has_value(), false);
    EXPECT_EQ(ParseNumberAs<int64_t>("-10e-1").has_value(), false);
}

TEST(Number, FloatingPointWithoutDecimalPoint)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Number without decimal point can be treated as floating point too, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");                // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis");  // requirements

    EXPECT_EQ(ParseNumberAs<float>("-18446744073709551615").value_or(-999), -18446744073709551615.0f);
    EXPECT_EQ(ParseNumberAs<double>("18446744073709551615").value_or(-999), 18446744073709551615.0);
}

TEST(Number, NearDoubleLimitsIsParsable)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Near-limit finite doubles are accepted across architectures.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    EXPECT_TRUE(ParseNumberAs<double>("2.225073858507201383090e-308").has_value());
    EXPECT_TRUE(ParseNumberAs<double>("1.797693134862315708145e+308").has_value());
}

TEST(Number, FloatingPointOverflowIsRejected)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Overflowing floating-point values are rejected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    EXPECT_FALSE(ParseNumberAs<double>("1e309").has_value());
    EXPECT_FALSE(ParseNumberAs<double>("-1e309").has_value());
}

}  // namespace

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
