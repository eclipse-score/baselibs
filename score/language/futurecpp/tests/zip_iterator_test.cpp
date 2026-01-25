/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///

#include <score/zip_iterator.hpp>
#include <score/zip_iterator.hpp> // test include guard

#include <score/assert_support.hpp>
#include <score/span.hpp>
#include <score/tuple.hpp>
#include <score/utility.hpp>

#include <forward_list>
#include <iterator>
#include <list>
#include <tuple>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

namespace
{

using forward_container = std::forward_list<double>;
using bidirecional_container = std::list<double>;
using random_access_container = std::vector<double>;

static_assert(score::cpp::detail::is_forward_iterator<forward_container::iterator>::value, "");
static_assert(score::cpp::detail::is_bidirectional_iterator<bidirecional_container::iterator>::value, "");
static_assert(score::cpp::detail::is_random_access_iterator<random_access_container::iterator>::value, "");

template <typename ContainerT>
class container_test : public ::testing::Test
{
public:
    using ContainerType = ContainerT;
};

template <typename ContainerT>
class container_test_basics : public container_test<ContainerT>
{
};

template <typename ContainerT>
class container_test_bi_directional : public container_test<ContainerT>
{
};

template <typename ContainerT>
class container_test_random_access : public container_test<ContainerT>
{
};

template <typename>
class general_zip_iterator_test : public ::testing::Test
{
};

using AllTestTypes = ::testing::Types<forward_container, bidirecional_container, random_access_container>;
using BiDirTestTypes = ::testing::Types<bidirecional_container, random_access_container>;
using RaTestTypes = ::testing::Types<random_access_container>;

using GeneralZipTestTypes =
    ::testing::Types<std::tuple<forward_container, forward_container, std::forward_iterator_tag>,
                     std::tuple<forward_container, bidirecional_container, std::forward_iterator_tag>,
                     std::tuple<forward_container, random_access_container, std::forward_iterator_tag>,
                     std::tuple<bidirecional_container, bidirecional_container, std::bidirectional_iterator_tag>,
                     std::tuple<bidirecional_container, random_access_container, std::bidirectional_iterator_tag>,
                     std::tuple<random_access_container, random_access_container, std::random_access_iterator_tag>>;

TYPED_TEST_SUITE(container_test_basics, AllTestTypes, /*unused*/);
TYPED_TEST_SUITE(container_test_bi_directional, BiDirTestTypes, /*unused*/);
TYPED_TEST_SUITE(container_test_random_access, RaTestTypes, /*unused*/);

TYPED_TEST_SUITE(general_zip_iterator_test, GeneralZipTestTypes, /*unused*/);

const auto make_zip_iterator_begin = [](auto... iterators) {
    return score::cpp::make_zip_range(score::cpp::make_range_pair(iterators, iterators)...).begin();
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, zip_iterator_type)
{
    const std::initializer_list<double> data{1.2, 3.4, 5.6};

    std::tuple_element_t<0U, TypeParam> container1{data};
    std::tuple_element_t<1U, TypeParam> container2{data};
    using expected_iterator_type =
        score::cpp::zip_iterator<typename decltype(container1)::iterator, typename decltype(container2)::iterator>;

    auto begin = make_zip_iterator_begin(container1.begin(), std::next(container2.begin()));
    EXPECT_TRUE((std::is_same<std::decay_t<decltype(begin)>, expected_iterator_type>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, zip_iterator_category)
{
    using Container1 = std::tuple_element_t<0U, TypeParam>;
    using Container2 = std::tuple_element_t<1U, TypeParam>;
    using iterator_category = std::tuple_element_t<2U, TypeParam>;
    using zip_iterator_type = score::cpp::zip_iterator<typename Container1::iterator, typename Container2::iterator>;

    EXPECT_TRUE(
        (std::is_same<typename std::iterator_traits<zip_iterator_type>::iterator_category, iterator_category>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, zip_iterator_value_type)
{
    using Container1 = std::tuple_element_t<0U, TypeParam>;
    using Container2 = std::tuple_element_t<1U, TypeParam>;
    using zip_iterator_type = score::cpp::zip_iterator<typename Container1::iterator, typename Container2::iterator>;

    EXPECT_TRUE((std::is_same<typename std::iterator_traits<zip_iterator_type>::value_type,
                              score::cpp::detail::zipped_tuple_like<double, double>>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, zip_iterator_reference_type)
{
    using Container1 = std::tuple_element_t<0U, TypeParam>;
    using Container2 = std::tuple_element_t<1U, TypeParam>;
    using zip_iterator_type = score::cpp::zip_iterator<typename Container1::iterator, typename Container2::iterator>;

    EXPECT_TRUE((std::is_same<typename std::iterator_traits<zip_iterator_type>::reference,
                              score::cpp::detail::zipped_tuple_like<double&, double&>>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, zip_iterator_pointer_type)
{
    using Container1 = std::tuple_element_t<0U, TypeParam>;
    using Container2 = std::tuple_element_t<1U, TypeParam>;
    using zip_iterator_type = score::cpp::zip_iterator<typename Container1::iterator, typename Container2::iterator>;

    EXPECT_TRUE((std::is_same<typename std::iterator_traits<zip_iterator_type>::pointer,
                              score::cpp::detail::zipped_tuple_like<double*, double*>>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, dereference)
{
    const std::initializer_list<double> data{1.2, 3.4, 5.6};
    std::tuple_element_t<0U, TypeParam> container1{data};
    std::tuple_element_t<1U, TypeParam> container2{data};

    auto begin = make_zip_iterator_begin(container1.begin(), std::next(container2.begin()));

    EXPECT_EQ(*begin, std::make_tuple(1.2, 3.4));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, preincrement)
{
    const std::initializer_list<double> data{1.2, 3.4, 5.6};
    std::tuple_element_t<0U, TypeParam> container1{data};
    std::tuple_element_t<1U, TypeParam> container2{data};

    auto begin = make_zip_iterator_begin(container1.begin(), std::next(container2.begin()));

    EXPECT_EQ(*++begin, std::make_tuple(3.4, 5.6));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, copy)
{
    const std::initializer_list<double> data{1.2};
    std::tuple_element_t<0U, TypeParam> container{data};

    auto begin = make_zip_iterator_begin(container.begin());

    using value_type = typename std::iterator_traits<score::cpp::remove_cvref_t<decltype(begin)>>::value_type;
    const value_type result{*begin};

    EXPECT_EQ(result, std::make_tuple(1.2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, copy_does_not_modify_copied_from)
{
    const std::initializer_list<double> data{1.2};
    std::tuple_element_t<0U, TypeParam> container{data};

    auto begin = make_zip_iterator_begin(container.begin());

    using value_type = typename std::iterator_traits<score::cpp::remove_cvref_t<decltype(begin)>>::value_type;
    value_type result{*begin};

    EXPECT_EQ(result, std::make_tuple(1.2));

    std::get<0>(result) = 23.0;

    EXPECT_EQ(result, std::make_tuple(23.0));
    EXPECT_EQ(*begin, std::make_tuple(1.2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, copy_assign)
{
    const std::initializer_list<double> data{1.2};
    std::tuple_element_t<0U, TypeParam> container{data};

    auto begin = make_zip_iterator_begin(container.begin());

    using value_type = typename std::iterator_traits<score::cpp::remove_cvref_t<decltype(begin)>>::value_type;
    value_type result{};
    result = *begin;

    EXPECT_EQ(result, std::make_tuple(1.2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, move)
{
    const std::initializer_list<double> data{1.2};
    std::tuple_element_t<0U, TypeParam> container{data};

    auto begin = make_zip_iterator_begin(container.begin());

    using value_type = typename std::iterator_traits<score::cpp::remove_cvref_t<decltype(begin)>>::value_type;
    const value_type result{std::move(*begin)};

    EXPECT_EQ(result, std::make_tuple(1.2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, move_assign)
{
    const std::initializer_list<double> data{1.2};
    std::tuple_element_t<0U, TypeParam> container{data};

    auto begin = make_zip_iterator_begin(container.begin());

    using value_type = typename std::iterator_traits<score::cpp::remove_cvref_t<decltype(begin)>>::value_type;
    value_type result{};
    result = std::move(*begin);

    EXPECT_EQ(result, std::make_tuple(1.2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, assigment_through_to_original_container)
{
    const std::initializer_list<double> data{1.2, 3.4, 5.6};

    std::tuple_element_t<0U, TypeParam> container1{data};
    std::tuple_element_t<1U, TypeParam> container2{data};

    const auto container1_iter = container1.begin();
    const auto container2_iter = std::next(container2.begin());
    auto begin = make_zip_iterator_begin(container1_iter, container2_iter);

    *begin = std::tuple<double, double>{7.8, 9.1};

    EXPECT_EQ(*container1_iter, 7.8);
    EXPECT_EQ(*container2_iter, 9.1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, mixed_mutable_and_immutable_iterators_value_type)
{
    const std::initializer_list<double> data{1.2, 3.4, 5.6};
    score::cpp::span<const double> view(&*data.begin(), data.size());
    std::tuple_element_t<0U, TypeParam> container1{data};
    std::tuple_element_t<1U, TypeParam> container2{data};

    auto mixed_qualified_types = make_zip_iterator_begin(view.begin(), container1.begin(), container2.begin());
    using actual = typename std::iterator_traits<decltype(mixed_qualified_types)>::reference;
    EXPECT_TRUE(std::tuple_size<actual>::value == 3U);
    EXPECT_TRUE((std::is_same<std::tuple_element_t<0U, actual>, const double&>::value));
    EXPECT_TRUE((std::is_same<std::tuple_element_t<1U, actual>, double&>::value));
    EXPECT_TRUE((std::is_same<std::tuple_element_t<2U, actual>, double&>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, mixed_mutable_and_immutable_iterators_reference_type)
{
    const std::initializer_list<double> data{1.2, 3.4, 5.6};
    score::cpp::span<const double> view(&*data.begin(), data.size());
    std::tuple_element_t<0U, TypeParam> container1{data};
    std::tuple_element_t<1U, TypeParam> container2{data};

    auto mixed_qualified_types = make_zip_iterator_begin(view.begin(), container1.begin(), container2.begin());
    EXPECT_TRUE((std::is_same<typename std::iterator_traits<decltype(mixed_qualified_types)>::reference,
                              score::cpp::detail::zipped_tuple_like<const double&, double&, double&>>::value));

    EXPECT_EQ(*mixed_qualified_types, std::make_tuple(1.2, 1.2, 1.2));
    *container1.begin() = 9.1;
    *container2.begin() = 7.8;
    EXPECT_EQ(*mixed_qualified_types, std::make_tuple(1.2, 9.1, 7.8));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, mixed_mutable_and_immutable_iterators_pointer_type)
{
    const std::initializer_list<double> data{1.2, 3.4, 5.6};
    score::cpp::span<const double> view(&*data.begin(), data.size());
    std::tuple_element_t<0U, TypeParam> container1{data};
    std::tuple_element_t<1U, TypeParam> container2{data};

    auto mixed_qualified_types = make_zip_iterator_begin(view.begin(), container1.begin(), container2.begin());
    EXPECT_TRUE((std::is_same<typename std::iterator_traits<decltype(mixed_qualified_types)>::pointer,
                              score::cpp::detail::zipped_tuple_like<const double*, double*, double*>>::value));
    EXPECT_EQ(*mixed_qualified_types, std::make_tuple(1.2, 1.2, 1.2));
    *container1.begin() = 9.1;
    *container2.begin() = 7.8;
    EXPECT_EQ(*mixed_qualified_types, std::make_tuple(1.2, 9.1, 7.8));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(general_zip_iterator_test, mixed_mutable_and_immutable_iterators_dereference)
{
    const std::initializer_list<double> data{1.2, 3.4, 5.6};
    score::cpp::span<const double> view(&*data.begin(), data.size());
    std::tuple_element_t<0U, TypeParam> container1{data};
    std::tuple_element_t<1U, TypeParam> container2{data};

    const auto mixed_qualified_types = make_zip_iterator_begin(view.begin(), container1.begin(), container2.begin());

    EXPECT_EQ(*mixed_qualified_types, std::make_tuple(1.2, 1.2, 1.2));
    *container1.begin() = 9.1;
    *container2.begin() = 7.8;
    EXPECT_EQ(*mixed_qualified_types, std::make_tuple(1.2, 9.1, 7.8));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_basics, copy_constructor)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};

    const auto begin = make_zip_iterator_begin(container.begin(), std::next(container.begin()));

    decltype(begin) copy{begin};

    EXPECT_EQ(*copy, *begin);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_basics, swap)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};

    auto iter1 = make_zip_iterator_begin(container.begin(), std::next(container.begin()));
    auto iter2 = iter1;
    ++iter1;

    EXPECT_EQ(*iter1, std::make_tuple(3.4, 5.6));
    EXPECT_EQ(*iter2, std::make_tuple(1.2, 3.4));

    std::swap(iter1, iter2);
    EXPECT_EQ(*iter1, std::make_tuple(1.2, 3.4));
    EXPECT_EQ(*iter2, std::make_tuple(3.4, 5.6));

    swap(*iter1, *iter2);
    EXPECT_EQ(*container.begin(), 3.4);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_basics, copy_assignment)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};

    auto iter1 = make_zip_iterator_begin(container.begin(), std::next(container.begin()));
    const auto iter2 = iter1;
    ++iter1;

    EXPECT_NE(*iter1, *iter2);

    iter1 = iter2;

    EXPECT_EQ(*iter1, *iter2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_basics, equality_operators)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};
    const auto range =
        score::cpp::make_zip_range(container, score::cpp::make_range_pair(std::next(std::begin(container)), std::end(container)));
    auto begin = range.begin();
    auto end = range.end();

    EXPECT_TRUE(begin != end);
    EXPECT_FALSE(begin == end);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_basics, post_increment)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};

    auto begin = make_zip_iterator_begin(container.begin(), std::next(std::begin(container)));

    EXPECT_EQ(*begin++, std::make_tuple(1.2, 3.4));
    EXPECT_EQ(*begin, std::make_tuple(3.4, 5.6));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_basics, default_construcitble)
{
    using zip_iterator_type =
        score::cpp::zip_iterator<typename TestFixture::ContainerType::iterator, typename TestFixture::ContainerType::iterator>;

    EXPECT_NO_FATAL_FAILURE(zip_iterator_type{});
    EXPECT_TRUE((std::is_default_constructible<zip_iterator_type>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_basics, use_in_stl_algorithm_with_forward_iterator_requirement)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};
    const auto range = score::cpp::make_zip_range(score::cpp::make_range_pair(container.begin(), std::next(container.begin(), 2)),
                                           score::cpp::make_range_pair(std::next(container.begin()), container.end()));

    auto begin = range.begin();
    auto end = range.end();

    const auto element = std::make_tuple(3.4, 5.6);
    auto iter = std::find(begin, end, element);
    ASSERT_TRUE(iter != end);
    EXPECT_EQ(*iter, element);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_bi_directional, predecrement)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};

    const auto first = make_zip_iterator_begin(container.begin(), std::next(container.begin()));
    auto second = std::next(first);

    EXPECT_TRUE((--second) == first);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_bi_directional, postdecrement)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};

    const auto first = make_zip_iterator_begin(container.begin(), std::next(container.begin()));
    auto second = std::next(first);

    EXPECT_TRUE((second--) != first);
    EXPECT_TRUE(second == first);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_bi_directional, use_in_stl_algorithm_with_bidirectional_iterator_requirement)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};
    const auto range = score::cpp::make_zip_range(score::cpp::make_range_pair(container.begin(), std::next(container.begin(), 2)),
                                           score::cpp::make_range_pair(std::next(container.begin()), container.end()));
    auto begin = range.begin();
    auto end = range.end();

    EXPECT_EQ(*begin, std::make_tuple(1.2, 3.4));
    EXPECT_EQ(*std::next(begin), std::make_tuple(3.4, 5.6));
    std::reverse(begin, end);
    EXPECT_EQ(*begin, std::make_tuple(3.4, 5.6));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_random_access, comparison_operator)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};

    const auto range = score::cpp::make_zip_range(container, score::cpp::make_range_pair(container.rbegin(), container.rend()));
    const auto begin = range.begin();
    const auto end = range.end();

    EXPECT_TRUE(begin < end);
    EXPECT_TRUE(begin <= begin);

    EXPECT_TRUE(end > begin);
    EXPECT_TRUE(begin >= begin);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_random_access, offset_iterator_operators)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};

    const auto range = score::cpp::make_zip_range(container, score::cpp::make_range_pair(container.rbegin(), container.rend()));
    auto begin = range.begin();

    const auto first_data = std::make_tuple(1.2, 5.6);
    const auto second_data = std::make_tuple(3.4, 3.4);
    const auto third_data = std::make_tuple(5.6, 1.2);

    EXPECT_EQ(*(begin + 1), second_data);
    EXPECT_EQ((begin + 1), (1 + begin));

    begin += 2;
    EXPECT_EQ(*begin, third_data);

    EXPECT_EQ(*(begin - 1), second_data);
    begin -= 2;
    EXPECT_EQ(*begin, first_data);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_random_access, difference_operator)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};

    const auto range = score::cpp::make_zip_range(container, score::cpp::make_range_pair(container.rbegin(), container.rend()));
    const auto begin = range.begin();
    const auto end = range.end();

    EXPECT_EQ((end - begin), container.size());

    const auto end_with_inconsistent_offset = make_zip_iterator_begin(std::prev(container.end()), container.rend());
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(end_with_inconsistent_offset - begin);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TYPED_TEST(container_test_random_access, subscript_operator)
{
    typename TestFixture::ContainerType container{1.2, 3.4, 5.6};

    const auto range = score::cpp::make_zip_range(container, score::cpp::make_range_pair(container.rbegin(), container.rend()));
    auto begin = range.begin();

    EXPECT_EQ(begin[0], std::make_tuple(1.2, 5.6));
    EXPECT_EQ(begin[1], std::make_tuple(3.4, 3.4));
    EXPECT_EQ(begin[2], std::make_tuple(5.6, 1.2));

    begin[0] = std::make_tuple(7.7, 9.9);

    const auto cbegin = begin;

    EXPECT_EQ(cbegin[0], std::make_tuple(7.7, 9.9));
    EXPECT_EQ(cbegin[1], std::make_tuple(3.4, 3.4));
    EXPECT_EQ(cbegin[2], std::make_tuple(9.9, 7.7));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TEST(make_zip_range, typical_data)
{
    const std::vector<double> a{1.0, 2.0, 3.0, 4.0, 5.0};
    const std::array<double, 2> b{1.0, 2.0};
    const std::array<double, 2> c{1.0, 2.0};

    std::int32_t value{1};
    for (const auto zipped : score::cpp::make_zip_range(a, b, c))
    {
        const auto expected = static_cast<double>(value);
        EXPECT_EQ(std::get<0>(zipped), expected);
        EXPECT_EQ(std::get<1>(zipped), expected);
        EXPECT_EQ(std::get<2>(zipped), expected);
        ++value;
    }
    EXPECT_EQ(value, 3);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TEST(make_zip_range, get_iterator_tuple)
{
    const std::vector<double> a{1.0};
    const std::array<double, 1> b{2.0};
    const std::array<double, 1> c{3.0};

    const auto zipped = score::cpp::make_zip_range(a, b, c);

    EXPECT_EQ(1.0, *std::get<0>(zipped.begin().get_iterator_tuple()));
    EXPECT_EQ(2.0, *std::get<1>(zipped.begin().get_iterator_tuple()));
    EXPECT_EQ(3.0, *std::get<2>(zipped.begin().get_iterator_tuple()));

    EXPECT_EQ(a.begin(), std::get<0>(zipped.begin().get_iterator_tuple()));
    EXPECT_EQ(b.begin(), std::get<1>(zipped.begin().get_iterator_tuple()));
    EXPECT_EQ(c.begin(), std::get<2>(zipped.begin().get_iterator_tuple()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18579441
TEST(make_sized_zip_range, typical_data)
{
    const std::vector<double> a{1.0, 2.0, 3.0, 4.0, 5.0};
    const std::array<double, 3> b{1.0, 2.0, 3.0};
    const std::array<double, 4> c{1.0, 2.0, 3.0, 4.0};

    std::int32_t value{1};
    for (const auto zipped : score::cpp::make_sized_zip_range(2, a, b, c))
    {
        const auto expected = static_cast<double>(value);
        EXPECT_EQ(std::get<0>(zipped), expected);
        EXPECT_EQ(std::get<1>(zipped), expected);
        EXPECT_EQ(std::get<2>(zipped), expected);
        ++value;
    }
    EXPECT_EQ(value, 3);
}

} // namespace
