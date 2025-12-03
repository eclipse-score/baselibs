///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/expected.hpp>
#include <score/expected.hpp> // test include guard

#include <gtest/gtest.h>

#include <cstdint>
#include <type_traits>

namespace
{

class non_copyable_type
{
public:
    non_copyable_type() : num_{0} {}
    non_copyable_type(int num) : num_{num} {}
    non_copyable_type(const non_copyable_type&) = delete;
    non_copyable_type& operator=(const non_copyable_type&) = delete;

    non_copyable_type(non_copyable_type&&) = default;
    non_copyable_type& operator=(non_copyable_type&&) = default;
    ~non_copyable_type() = default;
    int num() const { return num_; }
    bool operator==(const non_copyable_type& other) const { return num_ == other.num_; }

private:
    int num_;
};

class non_movable_type
{
public:
    non_movable_type() : num_{0} {}
    non_movable_type(int num) : num_{num} {}
    non_movable_type(const non_movable_type&) = default;
    non_movable_type& operator=(const non_movable_type&) = default;

    non_movable_type(non_movable_type&&) = delete;
    non_movable_type& operator=(non_movable_type&&) = delete;
    ~non_movable_type() = default;
    int num() const { return num_; }
    bool operator==(const non_movable_type& other) const { return num_ == other.num_; }

private:
    int num_;
};

class non_default_constructible_type
{
public:
    non_default_constructible_type() = delete;
};

} // namespace

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16630920
TEST(UnexpectedTest, InitFromValue)
{
    const int some_lvalue{42};
    score::cpp::unexpected<int> unexpected_lvalue{some_lvalue};
    EXPECT_EQ(unexpected_lvalue.error(), 42);
    score::cpp::unexpected<non_copyable_type> unexpected_rvalue{non_copyable_type{42}};
    EXPECT_EQ(unexpected_rvalue.error().num(), 42);
    non_movable_type some_non_movable_lvalue{42};
    score::cpp::unexpected<non_movable_type> unexpected_non_movable_lvalue{some_non_movable_lvalue};
    EXPECT_EQ(unexpected_non_movable_lvalue.error().num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16630920
TEST(UnexpectedTest, LValueGetters)
{
    const int some_lvalue{42};
    score::cpp::unexpected<int> unexpected_lvalue{some_lvalue};
    EXPECT_EQ(unexpected_lvalue.error(), 42);
    unexpected_lvalue.error() = 23;
    EXPECT_EQ(unexpected_lvalue.error(), 23);
    const score::cpp::unexpected<int>& unexpected_lvalue_ref = unexpected_lvalue;
    EXPECT_EQ(unexpected_lvalue_ref.error(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16630920
TEST(UnexpectedTest, LValueGettersNonMovable)
{
    const non_movable_type some_lvalue{42};
    score::cpp::unexpected<non_movable_type> unexpected_lvalue{some_lvalue};
    EXPECT_EQ(unexpected_lvalue.error(), 42);
    const score::cpp::unexpected<non_movable_type>& unexpected_lvalue_ref = unexpected_lvalue;
    EXPECT_EQ(unexpected_lvalue_ref.error(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16630920
TEST(UnexpectedTest, RValueGetters)
{
    score::cpp::unexpected<non_copyable_type> some_error{non_copyable_type{42}};
    EXPECT_EQ(some_error.error().num(), 42);
    non_copyable_type&& new_val{std::move(some_error).error()};
    EXPECT_EQ(new_val.num(), 42);
    const score::cpp::unexpected<non_copyable_type> some_const_error{non_copyable_type{42}};
    EXPECT_EQ(some_const_error.error().num(), 42);
    const non_copyable_type&& new_val_other{std::move(some_const_error).error()};
    EXPECT_EQ(new_val_other.num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16630920
TEST(UnexpectedTest, MakeUnexpected)
{
    const int some_lvalue{42};
    score::cpp::unexpected<int> unexpected_lvalue{score::cpp::make_unexpected(some_lvalue)};
    EXPECT_EQ(unexpected_lvalue.error(), 42);
    score::cpp::unexpected<non_copyable_type> unexpected_rvalue{score::cpp::make_unexpected(non_copyable_type{42})};
    EXPECT_EQ(unexpected_rvalue.error().num(), 42);
    non_movable_type non_movable_value{42};
    score::cpp::unexpected<non_movable_type> unexpected_non_movable_value{score::cpp::make_unexpected(non_movable_value)};
    EXPECT_EQ(unexpected_non_movable_value.error(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitEmpty)
{
    score::cpp::expected<int, int> val;
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value(), 0);
    score::cpp::expected<int, std::string> val_string;
    ASSERT_TRUE(static_cast<bool>(val_string));
    EXPECT_EQ(val_string.value(), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitEmptyNonDefaultConstructible)
{
    using ExpectedType = score::cpp::expected<non_default_constructible_type, std::int32_t>;
    EXPECT_FALSE(std::is_default_constructible<non_default_constructible_type>::value);
    EXPECT_FALSE(std::is_default_constructible<ExpectedType>::value);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitEmptyErrorNonCopyable)
{
    score::cpp::expected<int, non_copyable_type> val;
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value(), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitEmptyErrorNonMovable)
{
    score::cpp::expected<int, non_movable_type> val;
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value(), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromValue)
{
    score::cpp::expected<int, int> val{42};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromValueErrorNonCopyable)
{
    score::cpp::expected<int, non_copyable_type> val{42};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromValueErrorNonMovable)
{
    score::cpp::expected<int, non_movable_type> val{42};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromValueNoCopy)
{
    score::cpp::expected<non_copyable_type, int> val{non_copyable_type{42}};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value().num(), 42);
    EXPECT_EQ(val->num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromValueNoMove)
{
    non_movable_type value_42{42};
    score::cpp::expected<non_movable_type, int> val{value_42};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value().num(), 42);
    EXPECT_EQ(val->num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromValueNoCopyErrorNonCopyable)
{
    score::cpp::expected<non_copyable_type, non_copyable_type> val{non_copyable_type{42}};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value().num(), 42);
    EXPECT_EQ(val->num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromValueNoMoveErrorNonMovable)
{
    non_movable_type value_42{42};
    score::cpp::expected<non_movable_type, non_movable_type> val{value_42};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value().num(), 42);
    EXPECT_EQ(val->num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromOtherExpectedNoCopy)
{
    score::cpp::expected<non_copyable_type, int> val{score::cpp::expected<non_copyable_type, int>{non_copyable_type(42)}};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value().num(), 42);
    EXPECT_EQ(val->num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromOtherExpectedNoMove)
{
    non_movable_type value_42{42};
    score::cpp::expected<non_movable_type, int> val{score::cpp::expected<non_movable_type, int>{value_42}};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value().num(), 42);
    EXPECT_EQ(val->num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromOtherExpectedNoCopyNonCopyableError)
{
    score::cpp::expected<non_copyable_type, non_copyable_type> val{
        score::cpp::expected<non_copyable_type, non_copyable_type>{non_copyable_type(42)}};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value().num(), 42);
    EXPECT_EQ(val->num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromOtherExpectedNoMoveNonMovableError)
{
    non_movable_type value_42(42);
    score::cpp::expected<non_movable_type, non_movable_type> val{score::cpp::expected<non_movable_type, non_movable_type>{value_42}};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value().num(), 42);
    EXPECT_EQ(val->num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitFromOtherExpected)
{
    score::cpp::expected<int, int> val_init{42};
    score::cpp::expected<int, int> val{val_init};
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(static_cast<bool>(val));
    EXPECT_EQ(val.value(), 42);
}

namespace
{
struct bool_convertible
{
    operator bool() const { return true; }
};
} // namespace

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InitWithConversion)
{
    score::cpp::expected<long, char> val_init_long{2};
    score::cpp::expected<bool, char> val_init_bool{bool_convertible{}};

    ASSERT_TRUE(val_init_long);
    ASSERT_EQ(val_init_long.value(), 2L);

    ASSERT_TRUE(val_init_bool);
    ASSERT_EQ(val_init_bool.value(), true);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, UnexpectedBlank)
{
    score::cpp::expected_blank<int> blank{score::cpp::make_unexpected(42)};
    ASSERT_FALSE(blank);
    EXPECT_EQ(blank.error(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, UnexpectedBlankNoCopy)
{
    score::cpp::expected_blank<non_copyable_type> blank{score::cpp::make_unexpected(non_copyable_type{42})};
    ASSERT_FALSE(blank);
    EXPECT_EQ(blank.error().num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, UnexpectedBlankNoMove)
{
    non_movable_type value_42{42};
    score::cpp::expected_blank<non_movable_type> blank{score::cpp::make_unexpected(value_42)};
    ASSERT_FALSE(blank);
    EXPECT_EQ(blank.error().num(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, ExpectedBlank)
{
    score::cpp::expected_blank<int> blank{};
    ASSERT_TRUE(blank);
    EXPECT_EQ(blank.value(), score::cpp::blank{});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, ExpectedBlankNonCopyableError)
{
    score::cpp::expected_blank<non_copyable_type> blank{};
    ASSERT_TRUE(blank);
    EXPECT_EQ(blank.value(), score::cpp::blank{});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, ExpectedBlankNonMovableError)
{
    score::cpp::expected_blank<non_movable_type> blank{};
    ASSERT_TRUE(blank);
    EXPECT_EQ(blank.value(), score::cpp::blank{});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, CopyAssign)
{
    score::cpp::expected<int, int> val{42};
    const score::cpp::expected<int, int> temp_expected_23{23};
    const score::cpp::expected<int, int> temp_unexpected_23{score::cpp::make_unexpected(23)};
    const score::cpp::expected<int, int> temp_unexpected_42{score::cpp::make_unexpected(42)};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 42);
    // Assign expected <- expected
    val = temp_expected_23;
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 23);
    // Assign expected <- unexpected
    val = temp_unexpected_23;
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 23);
    // Assign unexpected <- unexpected
    val = temp_unexpected_42;
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 42);
    // Assign unexpected <- expected
    val = temp_expected_23;
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, MoveAssign)
{
    score::cpp::expected<non_copyable_type, int> val{non_copyable_type{42}};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 42);
    // Assign expected <- expected
    val = score::cpp::expected<non_copyable_type, int>{non_copyable_type{23}};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 23);
    // Assign expected <- unexpected
    val = score::cpp::expected<non_copyable_type, int>{score::cpp::make_unexpected(23)};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 23);
    // Assign unexpected <- unexpected
    val = score::cpp::expected<non_copyable_type, int>{score::cpp::make_unexpected(42)};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 42);
    // Assign unexpected <- expected
    val = score::cpp::expected<non_copyable_type, int>{non_copyable_type{23}};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, CopyAssignNonMovable)
{
    non_movable_type value_42{42};
    non_movable_type value_23{23};
    score::cpp::expected<non_movable_type, int> val{value_42};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 42);
    // Assign expected <- expected
    val = score::cpp::expected<non_movable_type, int>{value_23};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 23);
    // Assign expected <- unexpected
    val = score::cpp::expected<non_movable_type, int>{score::cpp::make_unexpected(23)};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 23);
    // Assign unexpected <- unexpected
    val = score::cpp::expected<non_movable_type, int>{score::cpp::make_unexpected(42)};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 42);
    // Assign unexpected <- expected
    val = score::cpp::expected<non_movable_type, int>{value_23};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, MoveAssignNonCopyableError)
{
    score::cpp::expected<non_copyable_type, non_copyable_type> val{non_copyable_type{42}};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 42);
    // Assign expected <- expected
    val = score::cpp::expected<non_copyable_type, non_copyable_type>{non_copyable_type{23}};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 23);
    // Assign expected <- unexpected
    val = score::cpp::expected<non_copyable_type, non_copyable_type>{score::cpp::make_unexpected(non_copyable_type{23})};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 23);
    // Assign unexpected <- unexpected
    val = score::cpp::expected<non_copyable_type, non_copyable_type>{score::cpp::make_unexpected(non_copyable_type{42})};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 42);
    // Assign unexpected <- expected
    val = score::cpp::expected<non_copyable_type, non_copyable_type>{non_copyable_type{23}};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, CopyAssignNonMovableError)
{
    non_movable_type value_42{42};
    non_movable_type value_23{23};
    score::cpp::expected<non_movable_type, non_movable_type> val{value_42};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 42);
    // Assign expected <- expected
    val = score::cpp::expected<non_movable_type, non_movable_type>{value_23};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 23);
    // Assign expected <- unexpected
    val = score::cpp::expected<non_movable_type, non_movable_type>{score::cpp::make_unexpected(value_23)};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 23);
    // Assign unexpected <- unexpected
    val = score::cpp::expected<non_movable_type, non_movable_type>{score::cpp::make_unexpected(value_42)};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 42);
    // Assign unexpected <- expected
    val = score::cpp::expected<non_movable_type, non_movable_type>{value_23};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, MoveAssignToExpectedFromValue)
{
    score::cpp::expected<non_copyable_type, int> val{non_copyable_type{42}};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val->num(), 42);
    val = non_copyable_type{23};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val->num(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, CopyAssignToExpectedFromValue)
{
    non_movable_type value_1{42};
    score::cpp::expected<non_movable_type, int> val{value_1};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val->num(), 42);
    non_movable_type value_2{23};
    val = value_2;
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val->num(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, MoveAssignToUnexpectedFromValue)
{
    score::cpp::expected<non_copyable_type, int> val{score::cpp::make_unexpected(42)};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 42);
    val = non_copyable_type{23};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val->num(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, CopyAssignToUnexpectedFromValue)
{
    score::cpp::expected<non_movable_type, int> val{score::cpp::make_unexpected(42)};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 42);
    non_movable_type value{23};
    val = value;
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val->num(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, CopyAssignUnexpected)
{
    score::cpp::expected<int, int> val{42};
    const score::cpp::unexpected<int> error_1{42};
    const score::cpp::unexpected<int> error_2{23};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 42);
    val = error_1;
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 42);
    val = error_2;
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, MoveAssignUnexpected)
{
    score::cpp::expected<non_copyable_type, int> val{non_copyable_type{42}};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val->num(), 42);
    val = score::cpp::unexpected<int>{42};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 42);
    val = score::cpp::unexpected<int>{23};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, CopyAssignNonMovableUnexpected)
{
    non_movable_type value{42};
    score::cpp::expected<non_movable_type, non_movable_type> val{value};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val->num(), 42);
    non_movable_type error_1{42};
    val = score::cpp::unexpected<non_movable_type>{error_1};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 42);
    non_movable_type error_2{23};
    val = score::cpp::unexpected<non_movable_type>{error_2};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, MoveAssignUnexpectedNonCopyableError)
{
    score::cpp::expected<non_copyable_type, non_copyable_type> val{non_copyable_type{42}};
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val->num(), 42);
    val = score::cpp::unexpected<non_copyable_type>{42};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error().num(), 42);
    val = score::cpp::unexpected<non_copyable_type>{23};
    ASSERT_FALSE(val.has_value());
    EXPECT_EQ(val.error().num(), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, SwapBothExpected)
{
    score::cpp::expected<non_copyable_type, int> val_1{non_copyable_type{42}};
    score::cpp::expected<non_copyable_type, int> val_2{non_copyable_type{23}};
    ASSERT_TRUE(val_1.has_value());
    EXPECT_EQ(val_1.value(), 42);
    ASSERT_TRUE(val_2.has_value());
    EXPECT_EQ(val_2.value(), 23);
    val_1.swap(val_2);
    ASSERT_TRUE(val_1.has_value());
    EXPECT_EQ(val_1.value(), 23);
    ASSERT_TRUE(val_2.has_value());
    EXPECT_EQ(val_2.value(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, SwapBothUnexpected)
{
    score::cpp::expected<non_copyable_type, int> val_1{score::cpp::make_unexpected(42)};
    score::cpp::expected<non_copyable_type, int> val_2{score::cpp::make_unexpected(23)};
    ASSERT_FALSE(val_1.has_value());
    EXPECT_EQ(val_1.error(), 42);
    ASSERT_FALSE(val_2.has_value());
    EXPECT_EQ(val_2.error(), 23);
    val_1.swap(val_2);
    ASSERT_FALSE(val_1.has_value());
    EXPECT_EQ(val_1.error(), 23);
    ASSERT_FALSE(val_2.has_value());
    EXPECT_EQ(val_2.error(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, SwapFromExpectedToUnexpected)
{
    score::cpp::expected<non_copyable_type, int> val_1{non_copyable_type{42}};
    score::cpp::expected<non_copyable_type, int> val_2{score::cpp::make_unexpected(23)};
    ASSERT_TRUE(val_1.has_value());
    EXPECT_EQ(val_1.value(), 42);
    ASSERT_FALSE(val_2.has_value());
    EXPECT_EQ(val_2.error(), 23);
    val_1.swap(val_2);
    ASSERT_FALSE(val_1.has_value());
    EXPECT_EQ(val_1.error(), 23);
    ASSERT_TRUE(val_2.has_value());
    EXPECT_EQ(val_2.value(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, SwapFromUnexpectedToExpected)
{
    score::cpp::expected<non_copyable_type, int> val_1{score::cpp::make_unexpected(42)};
    score::cpp::expected<non_copyable_type, int> val_2{non_copyable_type{23}};
    ASSERT_FALSE(val_1.has_value());
    EXPECT_EQ(val_1.error(), 42);
    ASSERT_TRUE(val_2.has_value());
    EXPECT_EQ(val_2.value(), 23);
    val_1.swap(val_2);
    ASSERT_TRUE(val_1.has_value());
    EXPECT_EQ(val_1.value(), 23);
    ASSERT_FALSE(val_2.has_value());
    EXPECT_EQ(val_2.error(), 42);
}

struct equality_inspection
{
    bool operator==(const equality_inspection&) const
    {
        equality_operator_called = true;
        return true;
    }
    bool operator!=(const equality_inspection&) const
    {
        inequality_operator_called = true;
        return true;
    }
    mutable bool equality_operator_called{false};
    mutable bool inequality_operator_called{false};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, EqualityOperatorsWithBothNoValue)
{
    const score::cpp::expected<int, int> val_1{score::cpp::make_unexpected(42)};
    const score::cpp::expected<int, char> val_2{score::cpp::make_unexpected(static_cast<char>(23))};

    EXPECT_TRUE(val_1 == val_1);
    EXPECT_FALSE(val_1 == val_2);

    EXPECT_FALSE(val_1 != val_1);
    EXPECT_TRUE(val_1 != val_2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, EqualityOperatorsWithOneValueAndOneNoValue)
{
    const score::cpp::expected<char, int> val_1{score::cpp::make_unexpected(42)};
    const score::cpp::expected<int, int> val_3{val_1.error()};

    EXPECT_FALSE(val_1 == val_3);
    EXPECT_FALSE(val_3 == val_1);

    EXPECT_TRUE(val_1 != val_3);
    EXPECT_TRUE(val_3 != val_1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, EqualityOperatorsWithBothValue)
{
    const score::cpp::expected<char, int> val_1{static_cast<char>(42)};
    const score::cpp::expected<int, char> val_2{43};

    EXPECT_TRUE(val_1 == val_1);
    EXPECT_FALSE(val_1 == val_2);

    EXPECT_FALSE(val_1 != val_1);
    EXPECT_TRUE(val_1 != val_2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, EqualityOperatorsPropagatedToWrappedType)
{

    score::cpp::expected<equality_inspection, int> val_1{};
    score::cpp::expected<int, equality_inspection> val_2{score::cpp::make_unexpected(equality_inspection{})};

    ASSERT_TRUE(val_1.has_value());
    EXPECT_FALSE(val_1->equality_operator_called);
    EXPECT_FALSE(val_1->inequality_operator_called);
    EXPECT_TRUE(val_1 == val_1);
    EXPECT_TRUE(val_1->equality_operator_called);
    EXPECT_FALSE(std::move(val_1).value().inequality_operator_called);

    ASSERT_FALSE(val_2.has_value());
    EXPECT_FALSE(val_2.error().equality_operator_called);
    EXPECT_FALSE(val_2.error().inequality_operator_called);
    EXPECT_TRUE(val_2 == val_2);
    EXPECT_TRUE(val_2.error().equality_operator_called);
    EXPECT_FALSE(std::move(val_2).error().inequality_operator_called);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, InEqualityOperatorsPropagatedToWrappedType)
{
    score::cpp::expected<equality_inspection, int> val_1{};
    score::cpp::expected<int, equality_inspection> val_2{score::cpp::make_unexpected(equality_inspection{})};

    ASSERT_TRUE(val_1.has_value());
    EXPECT_FALSE(val_1->equality_operator_called);
    EXPECT_FALSE(val_1->inequality_operator_called);
    EXPECT_TRUE(val_1 != val_1);
    EXPECT_FALSE(val_1->equality_operator_called);
    EXPECT_TRUE(std::move(val_1).value().inequality_operator_called);

    ASSERT_FALSE(val_2.has_value());
    EXPECT_FALSE(val_2.error().equality_operator_called);
    EXPECT_FALSE(val_2.error().inequality_operator_called);
    EXPECT_TRUE(val_2 != val_2);
    EXPECT_FALSE(val_2.error().equality_operator_called);
    EXPECT_TRUE(std::move(val_2).error().inequality_operator_called);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedDeathTest, InitFromUnexpected)
{
    score::cpp::expected<int, int> val{score::cpp::make_unexpected(42)};
    ASSERT_FALSE(val.has_value());
    ASSERT_FALSE(static_cast<bool>(val));
    EXPECT_EQ(val.error(), 42);
    EXPECT_DEATH(val.value(), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedDeathTest, InitFromUnexpectedNoCopy)
{
    score::cpp::expected<non_copyable_type, int> val{score::cpp::make_unexpected(42)};
    ASSERT_FALSE(val.has_value());
    ASSERT_FALSE(static_cast<bool>(val));
    EXPECT_EQ(val.error(), 42);
    EXPECT_DEATH(val.value(), "");
    EXPECT_DEATH(val->num(), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedDeathTest, InitFromUnexpectedNoMove)
{
    score::cpp::expected<non_movable_type, int> val{score::cpp::make_unexpected(42)};
    ASSERT_FALSE(val.has_value());
    ASSERT_FALSE(static_cast<bool>(val));
    EXPECT_EQ(val.error(), 42);
    EXPECT_DEATH(val.value(), "");
    EXPECT_DEATH(val->num(), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedDeathTest, InitFromOtherUnexpected)
{
    score::cpp::expected<int, int> val_init{score::cpp::make_unexpected(42)};
    score::cpp::expected<int, int> val{val_init};
    ASSERT_FALSE(val.has_value());
    ASSERT_FALSE(static_cast<bool>(val));
    EXPECT_EQ(val.error(), 42);
    EXPECT_DEATH(val.value(), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedDeathTest, InitFromOtherUnexpectedNoCopy)
{
    score::cpp::expected<non_copyable_type, int> val{score::cpp::expected<non_copyable_type, int>{score::cpp::make_unexpected(42)}};
    ASSERT_FALSE(val.has_value());
    ASSERT_FALSE(static_cast<bool>(val));
    EXPECT_EQ(val.error(), 42);
    EXPECT_DEATH(val.value(), "");
    EXPECT_DEATH(val->num(), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedDeathTest, InitFromOtherUnexpectedNoMove)
{
    score::cpp::expected<non_movable_type, int> val{score::cpp::expected<non_movable_type, int>{score::cpp::make_unexpected(42)}};
    ASSERT_FALSE(val.has_value());
    ASSERT_FALSE(static_cast<bool>(val));
    EXPECT_EQ(val.error(), 42);
    EXPECT_DEATH(val.value(), "");
    EXPECT_DEATH(val->num(), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedDeathTest, InitMixedNonMoveNonCopyableExpectedFromOtherNonMovableUnexpected)
{
    score::cpp::expected<non_movable_type, non_copyable_type> val{score::cpp::make_unexpected(non_copyable_type{42})};
    ASSERT_FALSE(val.has_value());
    ASSERT_FALSE(static_cast<bool>(val));
    EXPECT_EQ(val.error(), 42);
    EXPECT_DEATH(val.value(), "");
    EXPECT_DEATH(val->num(), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TEST(ExpectedTest, ValueTypeAndErrorType)
{
    using type_1 = score::cpp::expected<int, bool>;
    ASSERT_TRUE((std::is_same_v<type_1::value_type, int>));
    ASSERT_TRUE((std::is_same_v<type_1::error_type, bool>));
    using type_2 = score::cpp::expected<char, float>;
    ASSERT_TRUE((std::is_same_v<type_2::value_type, char>));
    ASSERT_TRUE((std::is_same_v<type_2::error_type, float>));
}
