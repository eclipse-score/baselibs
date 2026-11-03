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

#include "score/serialization/types/length_prefixed_vector.h"

#include <gtest/gtest.h>
#include <score/vector.hpp>

#include <deque>
namespace score
{
namespace serialization
{
namespace types
{
namespace
{

template <typename T>
class LengthPrefixedVectorTest : public ::testing::Test
{
  public:
    void VerifyContents(const T vec, std::vector<typename T::value_type> expected_vec) const noexcept
    {
        EXPECT_EQ(vec.size(), expected_vec.size());

        score::cpp::ignore = VerifyContents(std::cbegin(vec), expected_vec);
    }

    typename T::const_iterator VerifyContents(typename T::const_iterator it,
                                              std::vector<typename T::value_type> expected_vec) const noexcept
    {
        auto expected = std::cbegin(expected_vec);
        for (; (expected != std::cend(expected_vec)); expected++, it++)
        {
            EXPECT_EQ(*expected, *it);
        }
        EXPECT_EQ(std::cend(expected_vec), expected);
        return it;
    }
};

using MyTypes = ::testing::Types<LengthPrefixedVector<std::uint8_t, std::uint16_t>,
                                 LengthPrefixedVector<std::uint8_t, std::uint16_t, std::allocator<std::uint8_t>>>;

// clang complains without unused
// error: must specify at least one argument for '...' parameter of variadic macro
// [-Werror,-Wgnu-zero-variadic-macro-arguments]
TYPED_TEST_SUITE(LengthPrefixedVectorTest, MyTypes, /* unused */);

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_InitializerListConstructor)
{
    TypeParam vec{1, 2, 3, 4};
    this->VerifyContents(vec, {1, 2, 3, 4});
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_SizeConstructor)
{
    TypeParam vec(8);
    this->VerifyContents(vec, {0, 0, 0, 0, 0, 0, 0, 0});
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_SizeCustomValueConstructor)
{
    TypeParam vec(8, 0xFFU);
    this->VerifyContents(vec, {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU});
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_InputIterator)
{
    score::cpp::pmr::vector<std::uint8_t> vec{1, 2, 3, 4};
    TypeParam lvec(std::begin(vec), std::end(vec));
    this->VerifyContents(lvec, {1, 2, 3, 4});
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_CopyConstructor)
{
    TypeParam vec1{1, 2, 3, 4};
    TypeParam vec2 = vec1;
    this->VerifyContents(vec2, {1, 2, 3, 4});
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_MoveConstructor)
{
    TypeParam vec1{1, 2, 3, 4};
    TypeParam vec2 = std::move(vec1);
    this->VerifyContents(vec2, {1, 2, 3, 4});

    EXPECT_EQ(vec1.size(), 0);
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_VectorCopyConstructor)
{
    std::vector<std::uint8_t, typename TypeParam::allocator_type> vec1{1, 2, 3, 4};
    TypeParam vec2 = vec1;
    this->VerifyContents(vec2, {1, 2, 3, 4});
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_VectorMoveConstructor)
{
    std::vector<std::uint8_t, typename TypeParam::allocator_type> vec1{1, 2, 3, 4};
    TypeParam vec2 = std::move(vec1);
    this->VerifyContents(vec2, {1, 2, 3, 4});
    EXPECT_EQ(vec1.size(), 0);
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_CopyAssignment)
{
    TypeParam vec1{1, 2, 3, 4};
    TypeParam vec2{5, 6, 7, 8};
    vec2 = vec1;
    this->VerifyContents(vec2, {1, 2, 3, 4});
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_MoveAssignment)
{
    TypeParam vec1{1, 2, 3, 4};
    TypeParam vec2{5, 6, 7, 8};
    vec2 = std::move(vec1);
    this->VerifyContents(vec2, {1, 2, 3, 4});

    TypeParam temp;
    std::swap(vec1, temp);

    EXPECT_TRUE(vec1.size() == 0);
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_VectorCopyAssignment)
{
    std::vector<std::uint8_t, typename TypeParam::allocator_type> vec1{1, 2, 3, 4};
    TypeParam vec2{5, 6, 7, 8};
    vec2 = vec1;
    this->VerifyContents(vec2, {1, 2, 3, 4});
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_VectorMoveAssignment)
{
    std::vector<std::uint8_t, typename TypeParam::allocator_type> vec1{1, 2, 3, 4};
    TypeParam vec2{5, 6, 7, 8};
    vec2 = std::move(vec1);
    this->VerifyContents(vec2, {1, 2, 3, 4});

    EXPECT_EQ(vec1.size(), 0);
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_InitializerListAssignment)
{
    TypeParam vec2{5, 6, 7, 8};
    vec2 = {1, 2, 3, 4};

    this->VerifyContents(vec2, {1, 2, 3, 4});
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_DefaultConstructor)
{
    TypeParam vec;
    static_assert(std::is_same<decltype(vec.size()), std::uint16_t>::value, "Wrong type");

    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.cbegin(), vec.cend());
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_Pushback)
{
    TypeParam vec;

    vec.push_back(0xDD);
    EXPECT_EQ(vec.size(), 1);
    EXPECT_NE(vec.cbegin(), vec.cend());
    EXPECT_EQ(*vec.cbegin(), 0xDD);
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_Insert)
{
    std::deque<std::uint8_t> dq{1, 2, 3, 4};
    TypeParam vec{5, 6, 7, 8};

    vec.insert(std::cbegin(vec), std::cbegin(dq), std::cend(dq));
    this->VerifyContents(vec, {1, 2, 3, 4, 5, 6, 7, 8});
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_Clear)
{
    TypeParam vec{1, 2, 3, 4};
    vec.clear();
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.cbegin(), vec.cend());
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_ResizeDefaultValue)
{
    TypeParam vec{1, 2, 3, 4};

    vec.resize(42);
    EXPECT_EQ(vec.size(), 42);

    auto it = this->VerifyContents(std::cbegin(vec), {1, 2, 3, 4});
    this->VerifyContents(it, std::vector<std::uint8_t>(38, 0U));
}

TYPED_TEST(LengthPrefixedVectorTest, PrimitiveType_ResizeCustomValue)
{
    TypeParam vec{1, 2, 3, 4};

    vec.resize(42, 0xCCU);
    EXPECT_EQ(vec.size(), 42);
    auto it = this->VerifyContents(std::cbegin(vec), {1, 2, 3, 4});
    this->VerifyContents(it, std::vector<std::uint8_t>(38, 0xCCU));
}

TEST(LengthPrefixedVectorDeathTest, PrimitiveType_PushbackTooManyValues)
{
    LengthPrefixedVector<std::uint8_t, std::uint8_t> vec(std::numeric_limits<std::uint8_t>::max());
    EXPECT_DEATH_IF_SUPPORTED({ vec.push_back(42); }, "");
}

class NonCopyable
{
  public:
    NonCopyable() = default;
    explicit NonCopyable(std::uint32_t foo) : foo_{foo} {}

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) noexcept = default;

    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable& operator=(NonCopyable&&) noexcept = default;

    std::uint32_t GetFoo() const noexcept
    {
        return foo_;
    }

  private:
    std::uint32_t foo_;
};

class LengthPrefixedVectorNonCopyableTest : public ::testing::Test
{
};

TEST_F(LengthPrefixedVectorNonCopyableTest, NonCopyableType_SizeConstructor)
{
    LengthPrefixedVector<NonCopyable, std::uint16_t> vec(8);
    EXPECT_EQ(vec.size(), 8);
    for (const auto& f : vec)
    {
        EXPECT_EQ(f.GetFoo(), 0);
    }
}
TEST_F(LengthPrefixedVectorNonCopyableTest, NonCopyableType_DefaultConstructor)
{
    LengthPrefixedVector<NonCopyable, std::uint16_t> vec;
    static_assert(std::is_same<decltype(vec.size()), std::uint16_t>::value, "Wrong type");

    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.cbegin(), vec.cend());
}

TEST_F(LengthPrefixedVectorNonCopyableTest, NonCopyableType_Pushback)
{
    LengthPrefixedVector<NonCopyable, std::uint16_t> vec;

    vec.push_back(NonCopyable{0xDD});
    EXPECT_EQ(vec.size(), 1);
    EXPECT_NE(vec.cbegin(), vec.end());
    EXPECT_EQ(vec.cbegin()->GetFoo(), 0xDD);
}

TEST_F(LengthPrefixedVectorNonCopyableTest, NonCopyableType_Clear)
{
    LengthPrefixedVector<NonCopyable, std::uint16_t> vec(8);

    vec.clear();
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.cbegin(), vec.cend());
}

TEST_F(LengthPrefixedVectorNonCopyableTest, NonCopyableType_ResizeDefaultValue)
{
    LengthPrefixedVector<NonCopyable, std::uint16_t> vec(8);

    vec.resize(42);
    EXPECT_EQ(vec.size(), 42);
    for (const auto& f : vec)
    {
        EXPECT_EQ(f.GetFoo(), 0);
    }
}

}  // namespace
}  // namespace types
}  // namespace serialization
}  // namespace score
