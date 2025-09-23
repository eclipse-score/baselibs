///
/// \file
/// \copyright Copyright (C) 2016-2018, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/multi_span.hpp>
#include <score/multi_span.hpp> // test include guard

#include <score/assert_support.hpp>

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <sstream>

#include <gtest/gtest.h>

///
/// \test Test: Check bounds rank
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TEST(multi_span, bounds_rank)
{
    // tests for rank==1
    {
        const score::cpp::bounds<1>::size_type val = score::cpp::bounds<1>::rank;
        EXPECT_EQ(1, val);
    }
    // tests for rank==2
    {
        const score::cpp::bounds<2>::size_type val = score::cpp::bounds<2>::rank;
        EXPECT_EQ(2, val);
    }
    // tests for rank==3
    {
        const score::cpp::bounds<3>::size_type val = score::cpp::bounds<3>::rank;
        EXPECT_EQ(3, val);
    }
    // tests for rank==4
    {
        const score::cpp::bounds<4>::size_type val = score::cpp::bounds<4>::rank;
        EXPECT_EQ(4, val);
    }
    // tests for rank==5
    {
        const score::cpp::bounds<5>::size_type val = score::cpp::bounds<5>::rank;
        EXPECT_EQ(5, val);
    }
}

///
/// \test Test: Check function 'make_bounds'
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TEST(multi_span, make_bounds_rank_1)
{
    {
        const score::cpp::bounds<1> bnd = score::cpp::make_bounds(42);
        EXPECT_EQ(42, bnd[0]);
    }
    {
        const score::cpp::bounds<1> bnd = {42U};
        EXPECT_EQ(42, bnd[0]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TEST(multi_span, make_bounds_rank_2)
{
    {
        const score::cpp::bounds<2> bnd = score::cpp::make_bounds(13, 42);
        EXPECT_EQ(13, bnd[0]);
        EXPECT_EQ(42, bnd[1]);
    }
    {
        const score::cpp::bounds<2> bnd = {13U, 42U};
        EXPECT_EQ(13, bnd[0]);
        EXPECT_EQ(42, bnd[1]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TEST(multi_span, make_bounds_rank_3)
{
    {
        const score::cpp::bounds<3> bnd = score::cpp::make_bounds(5, 13, 42);
        EXPECT_EQ(5, bnd[0]);
        EXPECT_EQ(13, bnd[1]);
        EXPECT_EQ(42, bnd[2]);
    }
    {
        const score::cpp::bounds<3> bnd = {5U, 13U, 42U};
        EXPECT_EQ(5, bnd[0]);
        EXPECT_EQ(13, bnd[1]);
        EXPECT_EQ(42, bnd[2]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TEST(multi_span, make_bounds_rank_4)
{
    {
        const score::cpp::bounds<4> bnd = score::cpp::make_bounds(5, 13, 42, 32);
        EXPECT_EQ(5, bnd[0]);
        EXPECT_EQ(13, bnd[1]);
        EXPECT_EQ(42, bnd[2]);
        EXPECT_EQ(32, bnd[3]);
    }
    {
        const score::cpp::bounds<4> bnd = {5U, 13U, 42U, 32U};
        EXPECT_EQ(5, bnd[0]);
        EXPECT_EQ(13, bnd[1]);
        EXPECT_EQ(42, bnd[2]);
        EXPECT_EQ(32, bnd[3]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TEST(multi_span, make_bounds_rank_5)
{
    {
        const score::cpp::bounds<5> bnd = score::cpp::make_bounds(5, 13, 42, 32, 64);
        EXPECT_EQ(5, bnd[0]);
        EXPECT_EQ(13, bnd[1]);
        EXPECT_EQ(42, bnd[2]);
        EXPECT_EQ(32, bnd[3]);
        EXPECT_EQ(64, bnd[4]);
    }
    {
        const score::cpp::bounds<5> bnd = {5U, 13U, 42U, 32U, 64U};
        EXPECT_EQ(5, bnd[0]);
        EXPECT_EQ(13, bnd[1]);
        EXPECT_EQ(42, bnd[2]);
        EXPECT_EQ(32, bnd[3]);
        EXPECT_EQ(64, bnd[4]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TEST(multi_span, make_bounds_zero)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::make_bounds(0));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::make_bounds(0, 0));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::make_bounds(0, 0, 0));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::make_bounds(0, 0, 0, 0));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::make_bounds(0, 0, 0, 0, 0));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::bounds<1> bnd(0));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::bounds<2> bnd(0U, 0U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::bounds<3> bnd(0U, 0U, 0U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::bounds<4> bnd(0U, 0U, 0U, 0U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::bounds<5> bnd(0U, 0U, 0U, 0U, 0U));
}

using ranks_of_interest = ::testing::Types<std::integral_constant<std::size_t, 1>,
                                           std::integral_constant<std::size_t, 2>,
                                           std::integral_constant<std::size_t, 3>,
                                           std::integral_constant<std::size_t, 4>,
                                           std::integral_constant<std::size_t, 5>>;

template <typename T>
class bounds_fixture : public ::testing::Test
{
    static constexpr std::size_t rank{T::value};

protected:
    score::cpp::bounds<rank> make_bounds() const { return score::cpp::bounds<rank>(make_array()); }

    score::cpp::offset<rank> make_valid_offset(score::cpp::bounds<rank> const& bnd) const
    {
        score::cpp::offset<rank> idx;
        for (std::size_t i = 0; i < rank; ++i)
        {
            int val = std::rand();
            idx[i] = static_cast<std::ptrdiff_t>(val) * static_cast<std::ptrdiff_t>(bnd[i] / RAND_MAX);
        }
        return idx;
    }
    std::array<std::size_t, rank> make_array() const
    {
        std::array<std::size_t, 5> a = {5, 13, 42, 32, 64};
        SCORE_LANGUAGE_FUTURECPP_ASSERT(a.size() >= rank);
        std::array<std::size_t, rank> output{};
        for (std::size_t i{0}; i < rank; ++i)
        {
            output[i] = a[i];
        }
        return output;
    }
};

TYPED_TEST_SUITE(bounds_fixture, ranks_of_interest, /*unused*/);

///
/// \test Test: Check bounds default constructors
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TYPED_TEST(bounds_fixture, bounds_default_constructors)
{
    const score::cpp::bounds<TypeParam::value> bnd = this->make_bounds();
    const score::cpp::bounds<TypeParam::value> copy(bnd);
    EXPECT_EQ(bnd, copy);
    score::cpp::bounds<TypeParam::value> assign;
    assign = bnd;
    EXPECT_EQ(bnd, assign);
}

///
/// \test Test: Check bounds size
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TYPED_TEST(bounds_fixture, bounds_size)
{
    const score::cpp::bounds<TypeParam::value> bnd = this->make_bounds();
    const std::array<std::size_t, TypeParam::value> d = this->make_array();
    EXPECT_EQ(std::accumulate(d.begin(), d.end(), std::ptrdiff_t(1), std::multiplies<std::ptrdiff_t>()),
              static_cast<std::ptrdiff_t>(bnd.size()));
}

///
/// \test Test: Check bounds size
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TYPED_TEST(bounds_fixture, bounds_contains)
{
    using difference_type = typename score::cpp::bounds<TypeParam::value>::iterator::difference_type;

    const score::cpp::bounds<TypeParam::value> bnd = this->make_bounds();
    typename score::cpp::bounds<TypeParam::value>::const_iterator begin = bnd.begin();
    typename score::cpp::bounds<TypeParam::value>::const_iterator end(bnd.begin() + static_cast<difference_type>(bnd.size()));
    EXPECT_TRUE(bnd.contains(this->make_valid_offset(bnd)));
    EXPECT_TRUE(!bnd.contains(*(--begin)));
    EXPECT_TRUE(!bnd.contains(*end));
}

///
/// \test Test: Check bounds access
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TYPED_TEST(bounds_fixture, multi_span_bounds_access)
{
    const score::cpp::bounds<TypeParam::value> bnd = this->make_bounds();
    const std::array<std::size_t, TypeParam::value> d = this->make_array();
    for (std::size_t i = 0; i < TypeParam::value; ++i)
    {
        EXPECT_EQ(d[i], bnd[i]);
    }
}

template <typename T>
class bounds_iterator_fixture : public ::testing::Test
{
    static constexpr std::size_t rank{T::value};

protected:
    score::cpp::bounds<rank> make_bounds() const { return score::cpp::bounds<rank>(make_array()); }

    score::cpp::offset<rank> get_offset(std::ptrdiff_t v) const
    {
        score::cpp::offset<rank> idx;
        idx[rank - 1] = v;
        return idx;
    }
    std::ptrdiff_t linearize(score::cpp::bounds<rank> const& bnd, score::cpp::offset<rank> const& idx) const
    {
        std::ptrdiff_t res = 0;
        const score::cpp::offset<rank> stride = score::cpp::make_stride(bnd);

        for (std::size_t i = 0; i < rank; ++i)
        {
            res += idx[i] * stride[i];
        }
        return res;
    }

private:
    std::array<std::size_t, rank> make_array() const
    {
        std::array<std::size_t, 5> a = {10, 2, 3, 4, 5};
        SCORE_LANGUAGE_FUTURECPP_ASSERT(a.size() >= rank);
        std::array<std::size_t, rank> output{};
        for (std::size_t i{0}; i < rank; ++i)
        {
            output[rank - i - 1] = a[i];
        }
        return output;
    }
};
TYPED_TEST_SUITE(bounds_iterator_fixture, ranks_of_interest, /*unused*/);

///
/// \test Test: Check iterator default constructors
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TYPED_TEST(bounds_iterator_fixture, multi_span_iterator_default_ctor)
{
    typename score::cpp::bounds<TypeParam::value>::iterator it(this->make_bounds());
    typename score::cpp::bounds<TypeParam::value>::iterator copy(it);
    EXPECT_EQ(it, copy);
}

///
/// \test Test: Check iterator arithmetic
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TYPED_TEST(bounds_iterator_fixture, multi_span_iterator_arithmetic)
{

    using difference_type = typename score::cpp::bounds<TypeParam::value>::iterator::difference_type;

    const score::cpp::bounds<TypeParam::value> bnd = this->make_bounds();
    typename score::cpp::bounds<TypeParam::value>::iterator begin(bnd);
    const typename score::cpp::offset<TypeParam::value>::size_type val = begin->rank;
    EXPECT_EQ(TypeParam::value, val);
    typename score::cpp::bounds<TypeParam::value>::iterator end(begin + static_cast<difference_type>(bnd.size()));
    typename score::cpp::bounds<TypeParam::value>::iterator rend(begin - 1);
    EXPECT_EQ(score::cpp::offset<TypeParam::value>(), *begin);
    EXPECT_EQ(static_cast<difference_type>(bnd.size()), end - begin);
    typename score::cpp::bounds<TypeParam::value>::iterator it(bnd);
    it += 5;
    EXPECT_EQ(this->get_offset(5), *(it));
    EXPECT_EQ(score::cpp::offset<TypeParam::value>(), *(it - 5));
    EXPECT_EQ(this->get_offset(9), *(it + 4));
    EXPECT_EQ(this->get_offset(6), it[1]);
    EXPECT_EQ(end, (it + static_cast<difference_type>(bnd.size())));
    EXPECT_EQ(rend, (it - static_cast<difference_type>(bnd.size())));
}

///
/// \test Test: Check iterator increase
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TYPED_TEST(bounds_iterator_fixture, multi_span_iterator_increase)
{
    const score::cpp::bounds<TypeParam::value> bnd = this->make_bounds();
    typename score::cpp::bounds<TypeParam::value>::const_iterator begin = bnd.begin();
    typename score::cpp::bounds<TypeParam::value>::const_iterator end = bnd.end();

    std::int32_t count = 0;
    for (typename score::cpp::bounds<TypeParam::value>::const_iterator it = begin; it < end; ++it)
    {
        EXPECT_EQ(count, this->linearize(bnd, *it));
        ++count;
    }
}

///
/// \test Test: Check iterator decrease
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TYPED_TEST(bounds_iterator_fixture, multi_span_iterator_decrease)
{
    const score::cpp::bounds<TypeParam::value> bnd = this->make_bounds();
    typename score::cpp::bounds<TypeParam::value>::const_reverse_iterator rbegin = bnd.rbegin();
    typename score::cpp::bounds<TypeParam::value>::const_reverse_iterator rend = bnd.rend();

    std::int32_t count = static_cast<std::int32_t>(bnd.size()) - 1;
    for (typename score::cpp::bounds<TypeParam::value>::const_reverse_iterator it = rbegin; it < rend; ++it)
    {
        EXPECT_EQ(count, this->linearize(bnd, *it));
        --count;
    }
}

///
/// \test Test: Check iterator comparison operators
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694869
TYPED_TEST(bounds_iterator_fixture, comparison_operator_given_equal)
{
    const score::cpp::detail::bounds_iterator<std::size_t, TypeParam::value> lhs_it{this->make_bounds()};
    const score::cpp::detail::bounds_iterator<std::size_t, TypeParam::value> rhs_it{score::cpp::bounds<TypeParam::value>{}};
    EXPECT_FALSE(lhs_it != rhs_it);
    EXPECT_TRUE(lhs_it == rhs_it);
    EXPECT_TRUE(lhs_it <= rhs_it);
    EXPECT_FALSE(lhs_it > rhs_it);
    EXPECT_TRUE(lhs_it >= rhs_it);
}

///
/// \test Test: Check offset rank
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694782
TEST(multi_span, offset_rank)
{
    // tests for rank==1
    {
        const score::cpp::offset<1>::size_type val = score::cpp::offset<1>::rank;
        EXPECT_EQ(1, val);
    }
    // tests for rank==2
    {
        const score::cpp::offset<2>::size_type val = score::cpp::offset<2>::rank;
        EXPECT_EQ(2, val);
    }
    // tests for rank==3
    {
        const score::cpp::offset<3>::size_type val = score::cpp::offset<3>::rank;
        EXPECT_EQ(3, val);
    }
    // tests for rank==4
    {
        const score::cpp::offset<4>::size_type val = score::cpp::offset<4>::rank;
        EXPECT_EQ(4, val);
    }
    // tests for rank==5
    {
        const score::cpp::offset<5>::size_type val = score::cpp::offset<5>::rank;
        EXPECT_EQ(5, val);
    }
}

///
/// \test Test: Check function 'make_offset'
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694782
TEST(multi_span, make_offset_rank_1)
{
    {
        score::cpp::offset<1> o = score::cpp::make_offset(1);
        EXPECT_EQ(1, o[0]);
    }
    {
        score::cpp::offset<1> o = {1};
        EXPECT_EQ(1, o[0]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694782
TEST(multi_span, make_offset_rank_2)
{
    {
        score::cpp::offset<2> o = score::cpp::make_offset(1, 2);
        EXPECT_EQ(1, o[0]);
        EXPECT_EQ(2, o[1]);
    }
    {
        score::cpp::offset<2> o = {1, 2};
        EXPECT_EQ(1, o[0]);
        EXPECT_EQ(2, o[1]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694782
TEST(multi_span, make_offset_rank_3)
{
    {
        score::cpp::offset<3> o = score::cpp::make_offset(1, 2, 3);
        EXPECT_EQ(1, o[0]);
        EXPECT_EQ(2, o[1]);
        EXPECT_EQ(3, o[2]);
    }
    {
        score::cpp::offset<3> o = {1, 2, 3};
        EXPECT_EQ(1, o[0]);
        EXPECT_EQ(2, o[1]);
        EXPECT_EQ(3, o[2]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694782
TEST(multi_span, make_offset_rank_4)
{
    {
        score::cpp::offset<4> o = score::cpp::make_offset(1, 2, 3, 4);
        EXPECT_EQ(1, o[0]);
        EXPECT_EQ(2, o[1]);
        EXPECT_EQ(3, o[2]);
        EXPECT_EQ(4, o[3]);
    }
    {
        score::cpp::offset<4> o = {1, 2, 3, 4};
        EXPECT_EQ(1, o[0]);
        EXPECT_EQ(2, o[1]);
        EXPECT_EQ(3, o[2]);
        EXPECT_EQ(4, o[3]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694782
TEST(multi_span, make_offset_rank_5)
{
    {
        score::cpp::offset<5> o = score::cpp::make_offset(1, 2, 3, 4, 5);
        EXPECT_EQ(1, o[0]);
        EXPECT_EQ(2, o[1]);
        EXPECT_EQ(3, o[2]);
        EXPECT_EQ(4, o[3]);
        EXPECT_EQ(5, o[4]);
    }
    {
        score::cpp::offset<5> o = {1, 2, 3, 4, 5};
        EXPECT_EQ(1, o[0]);
        EXPECT_EQ(2, o[1]);
        EXPECT_EQ(3, o[2]);
        EXPECT_EQ(4, o[3]);
        EXPECT_EQ(5, o[4]);
    }
}

template <typename T>
class offset_fixture : public ::testing::Test
{
    static constexpr std::size_t rank{T::value};

protected:
    score::cpp::offset<rank> make_offset() const { return score::cpp::offset<rank>(make_array()); }

    std::array<std::ptrdiff_t, rank> make_array() const
    {
        std::array<std::ptrdiff_t, 5> a = {1, 2, 3, 4, 5};
        SCORE_LANGUAGE_FUTURECPP_ASSERT(a.size() >= rank);
        std::array<std::ptrdiff_t, rank> output{};
        for (std::size_t i{0}; i < rank; ++i)
        {
            output[i] = a[i];
        }
        return output;
    }
};
TYPED_TEST_SUITE(offset_fixture, ranks_of_interest, /*unused*/);

///
/// \test Test: Check offset default constructors
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694782
TYPED_TEST(offset_fixture, multi_span_offset_default_ctor)
{
    const score::cpp::offset<TypeParam::value> idx = this->make_offset();
    score::cpp::offset<TypeParam::value> copy(idx);
    EXPECT_EQ(idx, copy);
}

///
/// \test Test: Check offset access
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694782
TYPED_TEST(offset_fixture, multi_span_offset_access)
{
    const score::cpp::offset<TypeParam::value> idx = this->make_offset();
    const std::array<std::ptrdiff_t, TypeParam::value> d = this->make_array();
    for (std::size_t i = 0; i < TypeParam::value; ++i)
    {
        EXPECT_EQ(d[i], idx[i]);
    }
}

///
/// \test Test: Check offset comparison
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694782
TYPED_TEST(offset_fixture, multi_span_offset_comparison)
{
    score::cpp::offset<TypeParam::value> idx1 = this->make_offset();
    const score::cpp::offset<TypeParam::value> idx2 = idx1 * 2;
    EXPECT_EQ(idx1, idx1);
    EXPECT_NE(idx1, idx2);
    idx1 = idx2;
    EXPECT_EQ(idx1, idx2);
}

///
/// \test Test: Check offset comparison
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694782
TYPED_TEST(offset_fixture, multi_span_offset_arithmetic)
{
    score::cpp::offset<TypeParam::value> idx = this->make_offset();
    const std::array<std::ptrdiff_t, TypeParam::value> d = this->make_array();
    std::array<std::ptrdiff_t, TypeParam::value> cmp;
    std::transform(d.begin(), d.end(), cmp.begin(), [](std::ptrdiff_t v) { return v * 2; });
    idx += this->make_offset();
    EXPECT_EQ(score::cpp::offset<TypeParam::value>(cmp), idx);
    idx -= this->make_offset();
    EXPECT_EQ(this->make_offset(), idx);
    EXPECT_EQ(score::cpp::offset<TypeParam::value>(cmp), idx + this->make_offset());
    EXPECT_EQ(score::cpp::offset<TypeParam::value>(), idx - this->make_offset());
    idx *= 2;
    EXPECT_EQ(score::cpp::offset<TypeParam::value>(cmp), idx);
    idx /= 2;
    EXPECT_EQ(this->make_offset(), idx);
    EXPECT_EQ(score::cpp::offset<TypeParam::value>(cmp), idx * 2);
    EXPECT_EQ(this->make_offset(), (idx + this->make_offset()) / 2);
}

///
/// \test Test: Check multi_span rank
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, multi_span_rank)
{
    // tests for rank==1
    {
        const score::cpp::multi_span<int, 1>::size_type val = score::cpp::multi_span<int, 1>::rank;
        EXPECT_EQ(1, val);
    }
    // tests for rank==2
    {
        const score::cpp::multi_span<int, 2>::size_type val = score::cpp::multi_span<int, 2>::rank;
        EXPECT_EQ(2, val);
    }
    // tests for rank==3
    {
        const score::cpp::multi_span<int, 3>::size_type val = score::cpp::multi_span<int, 3>::rank;
        EXPECT_EQ(3, val);
    }
    // tests for rank==4
    {
        const score::cpp::multi_span<int, 4>::size_type val = score::cpp::multi_span<int, 4>::rank;
        EXPECT_EQ(4, val);
    }
    // tests for rank==5
    {
        const score::cpp::multi_span<int, 5>::size_type val = score::cpp::multi_span<int, 5>::rank;
        EXPECT_EQ(5, val);
    }
}

///
/// \test Test: Check convenience functions for multi_span
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, dynamic_array_as_multi_span_rank_1)
{
    std::unique_ptr<int[]> data(new int[1000]);
    score::cpp::multi_span<int, 1> view = score::cpp::as_multi_span(score::cpp::make_bounds(1000), data.get());
    EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1000));
    EXPECT_EQ(view.size(), 1000);
    EXPECT_EQ(view.stride(), score::cpp::make_offset(1));
    EXPECT_EQ(view.data(), data.get());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, dynamic_array_as_multi_span_rank_2)
{
    std::unique_ptr<int[]> data(new int[1000]);
    score::cpp::multi_span<int, 2> view = score::cpp::as_multi_span(score::cpp::make_bounds(100, 10), data.get());
    EXPECT_EQ(view.bounds(), score::cpp::make_bounds(100, 10));
    EXPECT_EQ(view.size(), 1000);
    EXPECT_EQ(view.stride(), score::cpp::make_offset(10, 1));
    EXPECT_EQ(view.data(), data.get());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, dynamic_array_as_multi_span_rank_3)
{
    std::unique_ptr<int[]> data(new int[1000]);
    score::cpp::multi_span<int, 3> view = score::cpp::as_multi_span(score::cpp::make_bounds(10, 10, 10), data.get());
    EXPECT_EQ(view.bounds(), score::cpp::make_bounds(10, 10, 10));
    EXPECT_EQ(view.size(), 1000);
    EXPECT_EQ(view.stride(), score::cpp::make_offset(100, 10, 1));
    EXPECT_EQ(view.data(), data.get());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, dynamic_array_as_multi_span_rank_4)
{
    std::unique_ptr<int[]> data(new int[1000]);
    score::cpp::multi_span<int, 4> view = score::cpp::as_multi_span(score::cpp::make_bounds(1, 10, 10, 10), data.get());
    EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1, 10, 10, 10));
    EXPECT_EQ(view.size(), 1000);
    EXPECT_EQ(view.stride(), score::cpp::make_offset(1000, 100, 10, 1));
    EXPECT_EQ(view.data(), data.get());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, dynamic_array_as_multi_span_rank_5)
{
    std::unique_ptr<int[]> data(new int[1000]);
    score::cpp::multi_span<int, 5> view = score::cpp::as_multi_span(score::cpp::make_bounds(1, 1, 10, 10, 10), data.get());
    EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1, 1, 10, 10, 10));
    EXPECT_EQ(view.size(), 1000);
    EXPECT_EQ(view.stride(), score::cpp::make_offset(1000, 1000, 100, 10, 1));
    EXPECT_EQ(view.data(), data.get());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, static_array_as_multi_span_rank_1)
{
    int data[1000];
    {
        score::cpp::multi_span<int, 1> view = score::cpp::as_multi_span(score::cpp::make_bounds(1000), data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1000));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1));
        EXPECT_EQ(view.data(), data);
    }
    {
        score::cpp::multi_span<int, 1> view = score::cpp::as_multi_span(data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1000));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1));
        EXPECT_EQ(view.data(), data);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, static_array_as_multi_span_rank_2)
{
    {
        int data[1000];
        score::cpp::multi_span<int, 2> view = score::cpp::as_multi_span(score::cpp::make_bounds(100, 10), data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(100, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(10, 1));
        EXPECT_EQ(view.data(), data);
    }
    {
        // note: functionality currently not supported
        // int data[100][10];
        // score::cpp::multi_span<int, 2> view = score::cpp::as_multi_span(data);
        // EXPECT_EQ(view.bounds() , score::cpp::make_bounds(100, 10));
        // EXPECT_EQ(view.size() , 1000);
        // EXPECT_EQ(view.stride() , score::cpp::make_offset(10, 1));
        // EXPECT_EQ(view.data() , data);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, static_array_as_multi_span_rank_3)
{
    {
        int data[1000];
        score::cpp::multi_span<int, 3> view = score::cpp::as_multi_span(score::cpp::make_bounds(10, 10, 10), data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(10, 10, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(100, 10, 1));
        EXPECT_EQ(view.data(), data);
    }
    {
        // note: functionality currently not supported
        // int data[10][10][10];
        // score::cpp::multi_span<int, 3> view = score::cpp::as_multi_span(data);
        // EXPECT_EQ(view.bounds() , score::cpp::make_bounds(10, 10, 10));
        // EXPECT_EQ(view.size() , 1000);
        // EXPECT_EQ(view.stride() , score::cpp::make_offset(100, 10, 1));
        // EXPECT_EQ(view.data() , data);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, static_array_as_multi_span_rank_4)
{
    {
        int data[1000];
        score::cpp::multi_span<int, 4> view = score::cpp::as_multi_span(score::cpp::make_bounds(1, 10, 10, 10), data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1, 10, 10, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1000, 100, 10, 1));
        EXPECT_EQ(view.data(), data);
    }
    {
        // note: functionality currently not supported
        // int data[1][10][10][10];
        // score::cpp::multi_span<int, 4> view = score::cpp::as_multi_span(data);
        // EXPECT_EQ(view.bounds() , score::cpp::make_bounds(1, 10, 10, 10));
        // EXPECT_EQ(view.size() , 1000);
        // EXPECT_EQ(view.stride() , score::cpp::make_offset(1000, 100, 10, 1));
        // EXPECT_EQ(view.data() , data);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, static_array_as_multi_span_rank_5)
{
    {
        int data[1000];
        score::cpp::multi_span<int, 5> view = score::cpp::as_multi_span(score::cpp::make_bounds(1, 1, 10, 10, 10), data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1, 1, 10, 10, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1000, 1000, 100, 10, 1));
        EXPECT_EQ(view.data(), data);
    }
    {
        // note: functionality currently not supported
        // int data[1][1][10][10][10];
        // score::cpp::multi_span<int, 5> view = score::cpp::as_multi_span(data);
        // EXPECT_EQ(view.bounds() , score::cpp::make_bounds(1, 1, 10, 10, 10));
        // EXPECT_EQ(view.size() , 1000);
        // EXPECT_EQ(view.stride() , score::cpp::make_offset(1000, 1000, 100, 10, 1));
        // EXPECT_EQ(view.data() , data);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, const_std_array_as_multi_span_rank_1)
{
    std::array<int, 1000> data;
    std::array<int, 1000> const& const_data = data;
    {
        score::cpp::multi_span<int const, 1> view = score::cpp::as_multi_span(score::cpp::make_bounds(1000), const_data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1000));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1));
        EXPECT_EQ(view.data(), const_data.data());
    }
    {
        score::cpp::multi_span<int const, 1> view = score::cpp::as_multi_span(const_data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1000));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1));
        EXPECT_EQ(view.data(), const_data.data());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, std_array_as_multi_span_rank_1)
{
    std::array<int, 1000> data;
    {
        score::cpp::multi_span<int, 1> view = score::cpp::as_multi_span(score::cpp::make_bounds(1000), data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1000));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1));
        EXPECT_EQ(view.data(), data.data());
    }
    {
        score::cpp::multi_span<int, 1> view = score::cpp::as_multi_span(data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1000));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1));
        EXPECT_EQ(view.data(), data.data());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, std_array_as_multi_span_rank_2)
{
    std::array<int, 1000> data;
    {
        score::cpp::multi_span<int, 2> view = score::cpp::as_multi_span(score::cpp::make_bounds(100, 10), data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(100, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(10, 1));
        EXPECT_EQ(view.data(), data.data());
    }
    {
        std::array<int, 1000> const& const_data = data;
        score::cpp::multi_span<int const, 2> view = score::cpp::as_multi_span(score::cpp::make_bounds(100, 10), const_data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(100, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(10, 1));
        EXPECT_EQ(view.data(), const_data.data());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, std_array_as_multi_span_rank_3)
{
    std::array<int, 1000> data;
    {
        score::cpp::multi_span<int, 3> view = score::cpp::as_multi_span(score::cpp::make_bounds(10, 10, 10), data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(10, 10, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(100, 10, 1));
        EXPECT_EQ(view.data(), data.data());
    }
    {
        std::array<int, 1000> const& const_data = data;
        score::cpp::multi_span<int const, 3> view = score::cpp::as_multi_span(score::cpp::make_bounds(10, 10, 10), const_data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(10, 10, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(100, 10, 1));
        EXPECT_EQ(view.data(), const_data.data());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, std_array_as_multi_span_rank_4)
{
    std::array<int, 1000> data;
    {
        score::cpp::multi_span<int, 4> view = score::cpp::as_multi_span(score::cpp::make_bounds(1, 10, 10, 10), data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1, 10, 10, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1000, 100, 10, 1));
        EXPECT_EQ(view.data(), data.data());
    }
    {
        std::array<int, 1000> const& const_data = data;
        score::cpp::multi_span<int const, 4> view = score::cpp::as_multi_span(score::cpp::make_bounds(1, 10, 10, 10), const_data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1, 10, 10, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1000, 100, 10, 1));
        EXPECT_EQ(view.data(), const_data.data());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TEST(multi_span, std_array_as_multi_span_rank_5)
{
    std::array<int, 1000> data;
    {
        score::cpp::multi_span<int, 5> view = score::cpp::as_multi_span(score::cpp::make_bounds(1, 1, 10, 10, 10), data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1, 1, 10, 10, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1000, 1000, 100, 10, 1));
        EXPECT_EQ(view.data(), data.data());
    }
    {
        std::array<int, 1000> const& const_data = data;
        score::cpp::multi_span<int const, 5> view = score::cpp::as_multi_span(score::cpp::make_bounds(1, 1, 10, 10, 10), const_data);
        EXPECT_EQ(view.bounds(), score::cpp::make_bounds(1, 1, 10, 10, 10));
        EXPECT_EQ(view.size(), 1000);
        EXPECT_EQ(view.stride(), score::cpp::make_offset(1000, 1000, 100, 10, 1));
        EXPECT_EQ(view.data(), const_data.data());
    }
}

template <typename T>
class multi_span_fixture : public ::testing::Test
{
    static constexpr std::size_t rank{T::value};

protected:
    multi_span_fixture() : array_data_{}
    {
        std::fill(data_, data_ + array_data_.size(), 0);
        std::fill(array_data_.begin(), array_data_.end(), 0);
        const auto bnd = make_bounds();
        std::size_t bounds_index{1};
        for (std::size_t i = 0; i < rank - 1; ++i)
        {
            bounds_index *= bnd[i];
        }
        array_data_[bounds_index] = rank;
    }

    score::cpp::bounds<rank> make_bounds() const { return score::cpp::bounds<rank>(make_array()); }

    std::array<std::size_t, rank> make_array() const
    {
        std::array<std::size_t, rank> output{};
        switch (rank)
        {
        case 1: // fallthrough
        case 2: // fallthrough
        case 3:
            output.fill(10);
            break;
        case 4:
            output.fill(5);
            break;
        case 5:
            output.fill(3);
            break;
        }
        return output;
    }

    int data_[1000];
    std::array<int, 1000> array_data_;
};

TYPED_TEST_SUITE(multi_span_fixture, ranks_of_interest, /*unused*/);

///
/// \test Test: Default created copy-constructor multi_span is trivial copy-constructible
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TYPED_TEST(multi_span_fixture, multi_span_copy_construct_is_trivial)
{
    static_assert(std::is_trivially_copy_constructible<score::cpp::multi_span<int, TypeParam::value>>::value, "");
}

///
/// \test Test: Default constructor multi_span
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TYPED_TEST(multi_span_fixture, multi_span_default_ctor)
{
    using Bounds = typename score::cpp::multi_span<int, TypeParam::value>::bounds_type;
    using Stride = typename score::cpp::multi_span<int, TypeParam::value>::offset_type;
    score::cpp::multi_span<int, TypeParam::value> default_constructed;
    EXPECT_EQ(default_constructed.data(), nullptr);
    EXPECT_EQ(default_constructed.size(), 0);
    EXPECT_EQ(default_constructed.bounds(), Bounds{});
    EXPECT_EQ(default_constructed.stride(), Stride{});
    score::cpp::multi_span<const int, TypeParam::value> const_copy(default_constructed);
    score::cpp::multi_span<const int, TypeParam::value> const_assign;
    const_assign = default_constructed;
}

///
/// \test Test: multi_span on dynamic array
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TYPED_TEST(multi_span_fixture, multi_span_test_dynamic_array)
{

    std::unique_ptr<int[]> data(new int[this->array_data_.size()]);
    score::cpp::multi_span<int, TypeParam::value> view(this->make_bounds(), data.get());
    score::cpp::multi_span<int, TypeParam::value> copy(view);
    EXPECT_EQ(view.bounds(), copy.bounds());
    EXPECT_EQ(view.size(), copy.size());
    EXPECT_EQ(view.stride(), copy.stride());
    EXPECT_EQ(view.data(), copy.data());
    score::cpp::multi_span<int, TypeParam::value> assign;
    assign = view;
    EXPECT_EQ(view.bounds(), assign.bounds());
    EXPECT_EQ(view.size(), assign.size());
    EXPECT_EQ(view.stride(), assign.stride());
    EXPECT_EQ(view.data(), assign.data());
    score::cpp::multi_span<const int, TypeParam::value> const_view(this->make_bounds(), data.get());
}

///
/// \test Test: multi_span on static C-array
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TYPED_TEST(multi_span_fixture, multi_span_test_static_c_array)
{

    score::cpp::multi_span<int, TypeParam::value> view(this->make_bounds(), this->data_);
    score::cpp::multi_span<int, TypeParam::value> copy(view);
    EXPECT_EQ(view.bounds(), copy.bounds());
    EXPECT_EQ(view.size(), copy.size());
    EXPECT_EQ(view.stride(), copy.stride());
    EXPECT_EQ(view.data(), copy.data());
    score::cpp::multi_span<int, TypeParam::value> assign;
    assign = view;
    EXPECT_EQ(view.bounds(), assign.bounds());
    EXPECT_EQ(view.size(), assign.size());
    EXPECT_EQ(view.stride(), assign.stride());
    EXPECT_EQ(view.data(), assign.data());
    score::cpp::multi_span<const int, TypeParam::value> const_view(this->make_bounds(), this->data_);
}

///
/// \test Test: multi_span on static std-array
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TYPED_TEST(multi_span_fixture, multi_span_test_static_std_array)
{

    {
        score::cpp::multi_span<int, TypeParam::value> view(this->make_bounds(), this->array_data_);
        score::cpp::multi_span<int, TypeParam::value> copy(view);
        EXPECT_EQ(view.bounds(), copy.bounds());
        EXPECT_EQ(view.size(), copy.size());
        EXPECT_EQ(view.stride(), copy.stride());
        EXPECT_EQ(view.data(), copy.data());
        score::cpp::multi_span<int, TypeParam::value> assign;
        assign = view;
        EXPECT_EQ(view.bounds(), assign.bounds());
        EXPECT_EQ(view.size(), assign.size());
        EXPECT_EQ(view.stride(), assign.stride());
        EXPECT_EQ(view.data(), assign.data());
        score::cpp::multi_span<const int, TypeParam::value> const_view(this->make_bounds(), this->array_data_);
    }
    {
        std::array<int, 1000> const& data = this->array_data_;
        score::cpp::multi_span<int const, TypeParam::value> view(this->make_bounds(), data);
        score::cpp::multi_span<int const, TypeParam::value> copy(view);
        EXPECT_EQ(view.bounds(), copy.bounds());
        EXPECT_EQ(view.size(), copy.size());
        EXPECT_EQ(view.stride(), copy.stride());
        EXPECT_EQ(view.data(), copy.data());
        score::cpp::multi_span<int const, TypeParam::value> assign;
        assign = view;
        EXPECT_EQ(view.bounds(), assign.bounds());
        EXPECT_EQ(view.size(), assign.size());
        EXPECT_EQ(view.stride(), assign.stride());
        EXPECT_EQ(view.data(), assign.data());
    }
}

///
/// \test Test: Check multi_span access
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TYPED_TEST(multi_span_fixture, multi_span_access)
{
    score::cpp::multi_span<int, TypeParam::value> view = as_multi_span(this->make_bounds(), this->array_data_);
    std::array<std::ptrdiff_t, TypeParam::value> offset_values{};
    offset_values[0] = 1;
    const auto offset = score::cpp::offset<TypeParam::value>(offset_values);
    EXPECT_EQ(view[offset], static_cast<int>(TypeParam::value));
    EXPECT_EQ(view.at(offset), static_cast<int>(TypeParam::value));
}

template <typename T>
using multi_span_fixture_rank_equal_1 = multi_span_fixture<T>;
using rank_of_one = ::testing::Types<std::integral_constant<std::size_t, 1>>;
TYPED_TEST_SUITE(multi_span_fixture_rank_equal_1, rank_of_one, /*unused*/);

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TYPED_TEST(multi_span_fixture_rank_equal_1, multi_span_slice)
{
    score::cpp::multi_span<int, 1> view = as_multi_span(this->make_bounds(), this->array_data_);
    EXPECT_EQ(view[1], 1);
}

template <typename T>
using multi_span_fixture_rank_greater_one = multi_span_fixture<T>;

using ranks_greater_one = ::testing::Types<std::integral_constant<std::size_t, 2>,
                                           std::integral_constant<std::size_t, 3>,
                                           std::integral_constant<std::size_t, 4>,
                                           std::integral_constant<std::size_t, 5>>;
TYPED_TEST_SUITE(multi_span_fixture_rank_greater_one, ranks_greater_one, /*unused*/);

///
/// \test Test: Check multi_span slicing
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18694884
TYPED_TEST(multi_span_fixture_rank_greater_one, multi_span_slice)
{
    score::cpp::multi_span<int, TypeParam::value> view = as_multi_span(this->make_bounds(), this->array_data_);
    score::cpp::multi_span<int, TypeParam::value - 1> s = view[1];
    score::cpp::bounds<TypeParam::value - 1> bnd;
    for (std::size_t i = 1; i < TypeParam::value; ++i)
    {
        bnd[i - 1] = view.bounds()[i];
    }
    EXPECT_EQ(s.bounds(), bnd);
    EXPECT_EQ(s.size(), bnd.size());
    score::cpp::offset<TypeParam::value> idx;
    idx[0] = 1;
    EXPECT_EQ(s.data(), &view[idx]);
}
