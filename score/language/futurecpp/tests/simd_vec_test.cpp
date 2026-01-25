/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

#include <score/simd.hpp>
#include <score/simd.hpp> // test include guard

#include <score/assert_support.hpp>

#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <numeric>
#include <type_traits>

#include <gtest/gtest.h>

namespace
{

template <typename T, typename V>
struct rebind
{
    // part of C++ standard https://en.cppreference.com/w/cpp/numeric/simd.html
    // but currently not implemented by `amp`. It creates a type of `mask<T>` with the size of `V`.
    // vector registers have a fixed length (for example 128 Bits). scale with ratio of both simd value types.
    using type = score::cpp::simd::vec<T, sizeof(T) / sizeof(typename V::value_type) * score::cpp::simd::vec<T>::size()>;
};

/// @brief returns a sequence 1,2,3...
template <typename TypeParam>
auto integer_sequence()
{
    std::array<typename TypeParam::value_type, TypeParam::size()> v;
    std::iota(v.begin(), v.end(), typename TypeParam::value_type{1});
    return v;
}

/// @brief returns a sequence ...-3,-2,-1
template <typename TypeParam>
auto negative_integer_sequence()
{
    std::array<typename TypeParam::value_type, TypeParam::size()> v;
    std::iota(v.begin(), v.end(), -static_cast<std::ptrdiff_t>(TypeParam::size()));
    return v;
}

/// @brief returns a sequence 1,-2,3,-4...
template <typename TypeParam>
auto alternating_integer_sequence()
{
    std::array<typename TypeParam::value_type, TypeParam::size()> v{integer_sequence<TypeParam>()};
    for (std::size_t i{1U}; i < v.size(); i += 2)
    {
        v[i] = -v[i];
    }
    return v;
}

/// @brief returns a sequence true,false,true,false...
template <std::size_t N>
auto alternating_boolean_sequence()
{
    std::array<bool, N> v;

    bool start{true};
    for (std::size_t i{0U}; i < v.size(); ++i)
    {
        v[i] = start;
        start = !start;
    }
    return v;
}

template <typename T>
class generator
{
    using type = typename T::value_type;
    static constexpr std::size_t size{T::size()};

public:
    explicit generator(const std::array<type, size> v) : v_{v} {}

    template <std::size_t M>
    constexpr type operator()(std::integral_constant<std::size_t, M>) const noexcept
    {
        return std::get<std::integral_constant<std::size_t, M>{}()>(v_);
    }
    constexpr type operator[](std::size_t i) const { return v_[i]; }

private:
    std::array<type, size> v_;
};

template <typename T>
class simd_vec_fixture : public testing::Test
{
};

using ElementTypes = ::testing::Types<score::cpp::simd::vec<std::int32_t>,
                                      score::cpp::simd::vec<float>,
                                      score::cpp::simd::vec<double>,
                                      rebind<float, score::cpp::simd::vec<std::uint8_t>>::type>;
TYPED_TEST_SUITE(simd_vec_fixture, ElementTypes, /*unused*/);

template <typename T>
class simd_floating_point_fixture : public testing::Test
{
};

using ElementFloatingPointTypes =
    ::testing::Types<score::cpp::simd::vec<float>, score::cpp::simd::vec<double>, rebind<float, score::cpp::simd::vec<std::uint8_t>>::type>;
TYPED_TEST_SUITE(simd_floating_point_fixture, ElementFloatingPointTypes, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, TrivialAndStandardLayout)
{
    static_assert(std::is_standard_layout<TypeParam>::value, "failed");
    static_assert(std::is_trivial<TypeParam>::value, "failed");
    static_assert(std::is_trivially_copyable<TypeParam>::value, "failed");
    static_assert(std::is_trivially_default_constructible<TypeParam>::value, "failed");
    static_assert(std::is_trivially_copy_constructible<TypeParam>::value, "failed");
    static_assert(std::is_trivially_move_constructible<TypeParam>::value, "failed");
    static_assert(std::is_trivially_copy_assignable<TypeParam>::value, "failed");
    static_assert(std::is_trivially_move_assignable<TypeParam>::value, "failed");
    static_assert(std::is_trivially_destructible<TypeParam>::value, "failed");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Broadcast)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam a{value_type{23}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(value_type{23}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Alignment)
{
#if defined(__SSE4_2__)
    // __m128{i,d} has 16 byte alignment
    EXPECT_EQ(score::cpp::simd::alignment_v<TypeParam>, 16);
#elif defined(__ARM_NEON)
    // Neon types have 16 byte alignment
    EXPECT_EQ(score::cpp::simd::alignment_v<TypeParam>, 16);
#else
    // scalar backend is element aligned
    EXPECT_EQ(score::cpp::simd::alignment_v<TypeParam>, sizeof(typename TypeParam::value_type));
#endif
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, InitializeWithGenerator)
{
    const generator<TypeParam> gen{integer_sequence<TypeParam>()};
    const TypeParam a{gen};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(gen[i], a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_vec_fixture, InitializeByDefaultIsUnaligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars};

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_vec_fixture, InitializeUnaligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars, score::cpp::simd::element_aligned};

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_vec_fixture, InitializeAligned)
{
    alignas(score::cpp::simd::alignment_v<TypeParam>) const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars, score::cpp::simd::vector_aligned};

    for (std::size_t i{0U}; i < vector.size(); ++i)
    {
        EXPECT_EQ(scalars[i], vector[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_vec_fixture, InitializeAligned_WhenCopyingFromUnalignedMemory_ThenPreconditionViolated)
{
    if (score::cpp::simd::alignment_v<TypeParam> == alignof(typename TypeParam::value_type))
    {
        GTEST_SKIP() << "alignment of `score::cpp::simd::vec` the same as single element cannot trigger an unaligned load";
    }

    using value_type = typename TypeParam::value_type;
    alignas(score::cpp::simd::alignment_v<TypeParam>) const std::array<value_type, TypeParam::size() + 1U> scalars{};
    const score::cpp::span<const value_type, TypeParam::size()> r{&scalars[1U], TypeParam::size()};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((TypeParam{r, score::cpp::simd::vector_aligned}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, CannotConstructWhenSizeIsNotAConstantExpression)
{
    static_assert(!std::is_constructible_v<score::cpp::simd::vec<std::int32_t>, std::vector<std::int32_t>>);
    static_assert(!std::is_constructible_v<score::cpp::simd::vec<std::int32_t>, score::cpp::span<std::int32_t>>);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_vec_fixture, StoreByDefaultIsUnaligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars};
    std::array<typename TypeParam::value_type, vector.size()> result;
    score::cpp::simd::unchecked_store(vector, result);

    EXPECT_EQ(result, scalars);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_vec_fixture, StoreUnaligned)
{
    const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars};
    std::array<typename TypeParam::value_type, vector.size()> result;
    score::cpp::simd::unchecked_store(vector, result, score::cpp::simd::element_aligned);

    EXPECT_EQ(result, scalars);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_vec_fixture, StoreAligned)
{
    using value_type = typename TypeParam::value_type;
    const auto scalars{integer_sequence<TypeParam>()};
    const TypeParam vector{scalars};
    alignas(score::cpp::simd::alignment_v<TypeParam>) std::array<value_type, vector.size()> result;
    score::cpp::simd::unchecked_store(vector, result, score::cpp::simd::vector_aligned);

    EXPECT_EQ(result, scalars);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050, CB-#18397902
TYPED_TEST(simd_vec_fixture, StoreAligned_WhenCopyingToUnalignedMemory_ThenPreconditionViolated)
{
    if (score::cpp::simd::alignment_v<TypeParam> == alignof(typename TypeParam::value_type))
    {
        GTEST_SKIP() << "alignment of `score::cpp::simd::vec` the same as single element cannot trigger an unaligned store";
    }

    using value_type = typename TypeParam::value_type;
    const TypeParam vector{value_type{23}};
    alignas(score::cpp::simd::alignment_v<TypeParam>) std::array<value_type, vector.size() + 1U> scalars;
    const score::cpp::span<value_type, TypeParam::size()> result{&scalars[1U], vector.size()};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::simd::unchecked_store(vector, result, score::cpp::simd::vector_aligned));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, CannotConstruct_WhenSizeIsNotAConstantExpression)
{
    const auto test = [](auto&& r) -> decltype(score::cpp::simd::unchecked_store(std::declval<TypeParam>(),
                                                                          std::forward<decltype(r)>(r))) {};

    using value_type = typename TypeParam::value_type;

    { // cannot call `unchecked_store` because types have runtime size
        static_assert(!std::is_invocable_v<decltype(test), std::vector<value_type>>);
        static_assert(!std::is_invocable_v<decltype(test), std::vector<value_type>&>);
        static_assert(!std::is_invocable_v<decltype(test), score::cpp::span<value_type>>);
        static_assert(!std::is_invocable_v<decltype(test), score::cpp::span<value_type>&>);
    }

    { // cannot store to range because it is `const`, i.e., non-modifiable
        static_assert(!std::is_invocable_v<decltype(test), score::cpp::span<const value_type, TypeParam::size()>>);
        static_assert(!std::is_invocable_v<decltype(test), score::cpp::span<const value_type, TypeParam::size()>&>);
        static_assert(!std::is_invocable_v<decltype(test), std::array<value_type, TypeParam::size()>>);
    }

    { // sanity check that test works
        static_assert(std::is_invocable_v<decltype(test), score::cpp::span<value_type, TypeParam::size()>>);
        static_assert(std::is_invocable_v<decltype(test), score::cpp::span<value_type, TypeParam::size()>&>);
        static_assert(std::is_invocable_v<decltype(test), std::array<value_type, TypeParam::size()>&>);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Access_WhenOutOfBounds_ThenPreconditionViolated)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam a{value_type{23}};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(a[a.size()]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Add)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = a + TypeParam{value_type{1}};
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(seq[i] + value_type{1}, r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, AddFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(all_of(inf == one + inf));
    EXPECT_TRUE(all_of(is_nan(one + nan)));

    EXPECT_TRUE(all_of(inf == inf + inf));
    EXPECT_TRUE(all_of(-inf == -inf + -inf));

    EXPECT_TRUE(all_of(is_nan(inf + -inf)));
    EXPECT_TRUE(all_of(is_nan(-inf + inf)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, AssignmentAdd)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    TypeParam a{seq};
    a += TypeParam{value_type{1}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(seq[i] + value_type{1}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Subtract)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = a - TypeParam{value_type{1}};
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(seq[i] - value_type{1}, r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, SubtractFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(all_of(TypeParam{value_type{0}} == one - one));
    EXPECT_TRUE(all_of(-inf == one - inf));
    EXPECT_TRUE(all_of(is_nan(one - nan)));

    EXPECT_TRUE(all_of(-inf == -inf - inf));
    EXPECT_TRUE(all_of(inf == inf - -inf));

    EXPECT_TRUE(all_of(is_nan(inf - inf)));
    EXPECT_TRUE(all_of(is_nan(-inf - -inf)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, AssignmentSubtract)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    TypeParam a{seq};
    a -= TypeParam{value_type{1}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(seq[i] - value_type{1}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Multiply)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = a * TypeParam{value_type{2}};
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(seq[i] * value_type{2}, r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, MultiplyFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam two{value_type{2}};
    const TypeParam zero{value_type{0}};

    EXPECT_TRUE(all_of(TypeParam{value_type{4}} == two * two));
    EXPECT_TRUE(all_of(inf == two * inf));
    EXPECT_TRUE(all_of(is_nan(two * nan)));

    EXPECT_TRUE(all_of(is_nan(zero * inf)));
    EXPECT_TRUE(all_of(is_nan(-zero * inf)));
    EXPECT_TRUE(all_of(is_nan(zero * -inf)));
    EXPECT_TRUE(all_of(is_nan(-zero * -inf)));
    EXPECT_TRUE(all_of(is_nan(inf * zero)));
    EXPECT_TRUE(all_of(is_nan(-inf * zero)));
    EXPECT_TRUE(all_of(is_nan(inf * -zero)));
    EXPECT_TRUE(all_of(is_nan(-inf * -zero)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, AssignmentMultiply)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    TypeParam a{seq};
    a *= TypeParam{value_type{2}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(seq[i] * value_type{2}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Divide)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = a / TypeParam{value_type{2}};
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(seq[i] / value_type{2}, r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, DivideFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam two{value_type{2}};
    const TypeParam zero{value_type{0}};

    EXPECT_TRUE(all_of(TypeParam{value_type{1}} == two / two));
    EXPECT_TRUE(all_of(zero == two / inf));
    EXPECT_TRUE(all_of(inf == two / zero));
    EXPECT_TRUE(all_of(is_nan(two / nan)));

    EXPECT_TRUE(all_of(is_nan(zero / zero)));
    EXPECT_TRUE(all_of(is_nan(-zero / zero)));
    EXPECT_TRUE(all_of(is_nan(zero / -zero)));
    EXPECT_TRUE(all_of(is_nan(-zero / -zero)));
    EXPECT_TRUE(all_of(is_nan(inf / inf)));
    EXPECT_TRUE(all_of(is_nan(-inf / inf)));
    EXPECT_TRUE(all_of(is_nan(inf / -inf)));
    EXPECT_TRUE(all_of(is_nan(-inf / -inf)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, AssignmentDivide)
{
    using value_type = typename TypeParam::value_type;
    const auto seq = integer_sequence<TypeParam>();
    TypeParam a{seq};
    a /= TypeParam{value_type{2}};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(seq[i] / value_type{2}, a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Negate)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = -a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(-seq[i], r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, NegateFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const value_type nan{std::numeric_limits<value_type>::quiet_NaN()};
    const value_type inf{std::numeric_limits<value_type>::infinity()};
    const value_type zero{0};

    static_assert(std::is_floating_point<value_type>::value, "failed");
    using bits = std::conditional_t<std::is_same<float, value_type>::value, std::uint32_t, std::uint64_t>;

    for (std::size_t i{0U}; i < TypeParam::size(); ++i)
    {
        EXPECT_EQ(score::cpp::bit_cast<bits>(nan), score::cpp::bit_cast<bits>(TypeParam{nan}[i]));
        EXPECT_EQ(score::cpp::bit_cast<bits>(-nan), score::cpp::bit_cast<bits>(TypeParam{-nan}[i]));
        EXPECT_EQ(score::cpp::bit_cast<bits>(inf), score::cpp::bit_cast<bits>(TypeParam{inf}[i]));
        EXPECT_EQ(score::cpp::bit_cast<bits>(-inf), score::cpp::bit_cast<bits>(TypeParam{-inf}[i]));
        EXPECT_EQ(score::cpp::bit_cast<bits>(zero), score::cpp::bit_cast<bits>(TypeParam{zero}[i]));
        EXPECT_EQ(score::cpp::bit_cast<bits>(-zero), score::cpp::bit_cast<bits>(TypeParam{-zero}[i]));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Equal_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = a == a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_TRUE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Equal_DifferentValue)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = alternating_integer_sequence<TypeParam>();

    const TypeParam a{seq_a};
    const TypeParam b{seq_b};

    const auto r = a == b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, EqualFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(none_of(one == nan));
    EXPECT_TRUE(none_of(nan == one));
    EXPECT_TRUE(none_of(nan == inf));
    EXPECT_TRUE(none_of(inf == nan));
    EXPECT_TRUE(none_of(nan == -inf));
    EXPECT_TRUE(none_of(-inf == nan));
    EXPECT_TRUE(all_of(inf == inf));
    EXPECT_TRUE(none_of(-inf == inf));
    EXPECT_TRUE(none_of(inf == -inf));
    EXPECT_TRUE(all_of(-inf == -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, NotEqual_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = a != a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_FALSE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, NotEqual_DifferentValue)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = alternating_integer_sequence<TypeParam>();

    const TypeParam a{seq_a};
    const TypeParam b{seq_b};

    const auto r = a != b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], !expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, NotEqualFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(all_of(one != nan));
    EXPECT_TRUE(all_of(nan != one));
    EXPECT_TRUE(all_of(nan != inf));
    EXPECT_TRUE(all_of(inf != nan));
    EXPECT_TRUE(all_of(nan != -inf));
    EXPECT_TRUE(all_of(-inf != nan));
    EXPECT_TRUE(none_of(inf != inf));
    EXPECT_TRUE(all_of(-inf != inf));
    EXPECT_TRUE(all_of(inf != -inf));
    EXPECT_TRUE(none_of(-inf != -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, LessThan_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = a < a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_FALSE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, LessThan_DifferentValue)
{
    const auto seq_a = alternating_integer_sequence<TypeParam>();
    const auto seq_b = integer_sequence<TypeParam>();

    const TypeParam a{seq_a};
    const TypeParam b{seq_b};

    const auto r = a < b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], !expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, LessThanFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(all_of(-one < one));
    EXPECT_TRUE(none_of(one < one));
    EXPECT_TRUE(none_of(one < nan));
    EXPECT_TRUE(none_of(nan < one));
    EXPECT_TRUE(none_of(nan < inf));
    EXPECT_TRUE(none_of(inf < nan));
    EXPECT_TRUE(none_of(nan < -inf));
    EXPECT_TRUE(none_of(-inf < nan));
    EXPECT_TRUE(none_of(inf < inf));
    EXPECT_TRUE(all_of(-inf < inf));
    EXPECT_TRUE(none_of(inf < -inf));
    EXPECT_TRUE(none_of(-inf < -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, LessEqual_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = a <= a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_TRUE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, LessEqual_DifferentValue)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = alternating_integer_sequence<TypeParam>();

    const TypeParam a{seq_a};
    const TypeParam b{seq_b};

    const auto r = a <= b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, LessEqualFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(none_of(one <= nan));
    EXPECT_TRUE(none_of(nan <= one));
    EXPECT_TRUE(none_of(nan <= inf));
    EXPECT_TRUE(none_of(inf <= nan));
    EXPECT_TRUE(none_of(nan <= -inf));
    EXPECT_TRUE(none_of(-inf <= nan));
    EXPECT_TRUE(all_of(inf <= inf));
    EXPECT_TRUE(all_of(-inf <= inf));
    EXPECT_TRUE(none_of(inf <= -inf));
    EXPECT_TRUE(all_of(-inf <= -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, GreateThan_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = a > a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_FALSE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, GreaterThan_DifferentValue)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = alternating_integer_sequence<TypeParam>();

    const TypeParam a{seq_a};
    const TypeParam b{seq_b};

    const auto r = a > b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], !expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, GreaterThanFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(none_of(one > nan));
    EXPECT_TRUE(none_of(nan > one));
    EXPECT_TRUE(none_of(nan > inf));
    EXPECT_TRUE(none_of(inf > nan));
    EXPECT_TRUE(none_of(nan > -inf));
    EXPECT_TRUE(none_of(-inf > nan));
    EXPECT_TRUE(none_of(inf > inf));
    EXPECT_TRUE(none_of(-inf > inf));
    EXPECT_TRUE(all_of(inf > -inf));
    EXPECT_TRUE(none_of(-inf > -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, GreateEqual_SameValue)
{
    const auto seq = integer_sequence<TypeParam>();
    const TypeParam a{seq};

    const auto r = a >= a;
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_TRUE(r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, GreaterEqual_DifferentValue)
{
    const auto seq_a = alternating_integer_sequence<TypeParam>();
    const auto seq_b = integer_sequence<TypeParam>();

    const TypeParam a{seq_a};
    const TypeParam b{seq_b};

    const auto r = a >= b;
    const auto expected = alternating_boolean_sequence<TypeParam::size()>();
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], expected[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, GreaterEqualFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(none_of(one >= nan));
    EXPECT_TRUE(none_of(nan >= one));
    EXPECT_TRUE(none_of(inf >= nan));
    EXPECT_TRUE(none_of(-inf >= nan));
    EXPECT_TRUE(none_of(nan >= inf));
    EXPECT_TRUE(none_of(nan >= -inf));
    EXPECT_TRUE(all_of(inf >= inf));
    EXPECT_TRUE(none_of(-inf >= inf));
    EXPECT_TRUE(all_of(inf >= -inf));
    EXPECT_TRUE(all_of(-inf >= -inf));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Min)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();

    const TypeParam a{seq_a};
    const TypeParam b{seq_b};

    const auto r = min(a, b);
    const auto r_swapped = min(b, a);
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], std::min(seq_a[i], seq_b[i]));
        EXPECT_EQ(r_swapped[i], r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, MinFloatSpecialValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam one{value_type{1}};

    EXPECT_TRUE(all_of(-inf == min(one, -inf)));
    EXPECT_TRUE(all_of(one == min(one, nan)));
    EXPECT_TRUE(all_of(is_nan(min(nan, one))));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Max)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();

    const TypeParam a{seq_a};
    const TypeParam b{seq_b};

    const auto r = max(a, b);
    const auto r_swapped = max(b, a);
    for (std::size_t i{0U}; i < r.size(); ++i)
    {
        EXPECT_EQ(r[i], std::max(seq_a[i], seq_b[i]));
        EXPECT_EQ(r_swapped[i], r[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_floating_point_fixture, MaxSpecialFloatValues)
{
    using value_type = typename TypeParam::value_type;

    const TypeParam nan{std::numeric_limits<value_type>::quiet_NaN()};
    const TypeParam inf{std::numeric_limits<value_type>::infinity()};
    const TypeParam two{value_type{2}};

    EXPECT_TRUE(all_of(inf == max(two, inf)));
    EXPECT_TRUE(all_of(two == max(two, nan)));
    EXPECT_TRUE(all_of(is_nan(max(nan, two))));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Clamp)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam one{value_type{1}};
    const TypeParam low{value_type{-1}};
    const TypeParam high{value_type{1}};

    EXPECT_TRUE(all_of(TypeParam{value_type{0}} == clamp(TypeParam{value_type{0}}, low, high)));
    EXPECT_TRUE(all_of(low == clamp(TypeParam{value_type{-2}}, low, high)));
    EXPECT_TRUE(all_of(high == clamp(TypeParam{value_type{2}}, low, high)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, Clscore_future_cpp_WhenNoValidBoundaryInterval_ThenPreconditionViolated)
{
    using value_type = typename TypeParam::value_type;
    const TypeParam one{value_type{1}};
    const TypeParam low{value_type{-1}};
    const TypeParam high{value_type{1}};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(clamp(one, high, low));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd_vec, ConvertFloatToInt)
{
    const auto seq = integer_sequence<score::cpp::simd::vec<float>>();
    const score::cpp::simd::vec<std::int32_t> b{score::cpp::simd::vec<float>{seq}};

    for (std::size_t i{0U}; i < b.size(); ++i)
    {
        EXPECT_EQ(static_cast<std::int32_t>(seq[i]), b[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd_vec, ConvertIntToFloat)
{
    const auto seq = integer_sequence<score::cpp::simd::vec<std::int32_t>>();
    const score::cpp::simd::vec<float> b{score::cpp::simd::vec<std::int32_t>{seq}};

    for (std::size_t i{0U}; i < b.size(); ++i)
    {
        EXPECT_EQ(static_cast<float>(seq[i]), b[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TEST(simd_vec, ConvertCharToFloat)
{
    const auto seq = []() {
        std::array<std::uint8_t, rebind<float, score::cpp::simd::vec<std::uint8_t>>::type::size()> v;
        std::iota(v.begin(), v.end(), 1U);
        return v;
    }();
    const rebind<float, score::cpp::simd::vec<std::uint8_t>>::type b{seq};
    static_assert(b.size() == seq.size());

    for (std::size_t i{0U}; i < b.size(); ++i)
    {
        EXPECT_EQ(static_cast<std::uint8_t>(seq[i]), b[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, WhereAssignment)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();
    const auto seq_mask = alternating_boolean_sequence<TypeParam::size()>();

    TypeParam a{seq_a};
    const typename TypeParam::mask_type mask{generator<typename TypeParam::mask_type>{seq_mask}};

    where(mask, a) = TypeParam{seq_b};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(a[i], seq_mask[i] ? seq_b[i] : seq_a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, WhereAssignmentAdd)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();
    const auto seq_mask = alternating_boolean_sequence<TypeParam::size()>();

    TypeParam a{seq_a};
    const typename TypeParam::mask_type mask{generator<typename TypeParam::mask_type>{seq_mask}};

    where(mask, a) += TypeParam{seq_b};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(a[i], seq_mask[i] ? seq_a[i] + seq_b[i] : seq_a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, WhereAssignmentSubtract)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();
    const auto seq_mask = alternating_boolean_sequence<TypeParam::size()>();

    TypeParam a{seq_a};
    const typename TypeParam::mask_type mask{generator<typename TypeParam::mask_type>{seq_mask}};

    where(mask, a) -= TypeParam{seq_b};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(a[i], seq_mask[i] ? seq_a[i] - seq_b[i] : seq_a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, WhereAssignmentMultiply)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();
    const auto seq_mask = alternating_boolean_sequence<TypeParam::size()>();

    TypeParam a{seq_a};
    const typename TypeParam::mask_type mask{generator<typename TypeParam::mask_type>{seq_mask}};

    where(mask, a) *= TypeParam{seq_b};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(a[i], seq_mask[i] ? seq_a[i] * seq_b[i] : seq_a[i]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18398050
TYPED_TEST(simd_vec_fixture, WhereAssignmentDivide)
{
    const auto seq_a = integer_sequence<TypeParam>();
    const auto seq_b = negative_integer_sequence<TypeParam>();
    const auto seq_mask = alternating_boolean_sequence<TypeParam::size()>();

    TypeParam a{seq_a};
    const typename TypeParam::mask_type mask{generator<typename TypeParam::mask_type>{seq_mask}};

    where(mask, a) /= TypeParam{seq_b};

    for (std::size_t i{0U}; i < a.size(); ++i)
    {
        EXPECT_EQ(a[i], seq_mask[i] ? seq_a[i] / seq_b[i] : seq_a[i]);
    }
}

} // namespace
