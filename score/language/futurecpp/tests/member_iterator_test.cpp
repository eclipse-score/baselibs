///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/member_iterator.hpp>
#include <score/member_iterator.hpp> // check include guard

#include <algorithm>
#include <forward_list>
#include <list>
#include <vector>

#include <gtest/gtest.h>

namespace
{
struct test_struct
{
    double m1;
    int m2;
};

using forward_container = std::forward_list<test_struct>;
using bidirecional_container = std::list<test_struct>;
using random_access_container = std::vector<test_struct>;

static_assert(score::cpp::detail::is_forward_iterator<forward_container::iterator>::value, "");
static_assert(score::cpp::detail::is_bidirectional_iterator<bidirecional_container::iterator>::value, "");
static_assert(score::cpp::detail::is_random_access_iterator<random_access_container::iterator>::value, "");

template <typename ContainerT>
class container_test : public ::testing::Test
{
public:
    ContainerT setup_container()
    {
        test_struct t1{1.2, 3};
        test_struct t2{4.5, 6};
        test_struct t3{7.8, 9};
        return {t1, t2, t3};
    }
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

} // namespace
using AllTestTypes = ::testing::Types<forward_container, bidirecional_container, random_access_container>;
using BiDirTestTypes = ::testing::Types<bidirecional_container, random_access_container>;
using RaTestTypes = ::testing::Types<random_access_container>;

TYPED_TEST_SUITE(container_test_basics, AllTestTypes, /*unused*/);
TYPED_TEST_SUITE(container_test_bi_directional, BiDirTestTypes, /*unused*/);
TYPED_TEST_SUITE(container_test_random_access, RaTestTypes, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18129029
TYPED_TEST(container_test_basics, base_object_test)
{
    auto container = this->setup_container();
    score::cpp::member_iterator<decltype(container.begin()), double, &test_struct::m1> begin(container.begin());
    score::cpp::member_iterator<decltype(container.end()), double, &test_struct::m1> end(container.end());

    EXPECT_EQ(*begin, 1.2);
    EXPECT_EQ(*++begin, 4.5);
    EXPECT_EQ(begin.base().m1, 4.5);
    EXPECT_EQ(begin.base().m2, 6);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18129029
TYPED_TEST(container_test_basics, basic_iterator_concepts)
{
    auto container = this->setup_container();
    score::cpp::member_iterator<decltype(container.begin()), double, &test_struct::m1> begin(container.begin());
    score::cpp::member_iterator<decltype(container.end()), double, &test_struct::m1> end(container.end());

    EXPECT_EQ(*begin, 1.2);
    EXPECT_EQ(*++begin, 4.5);

    decltype(begin) new_begin(begin);
    EXPECT_EQ(*new_begin, 4.5);

    begin = container.begin();
    EXPECT_EQ(*begin, 1.2);
    std::swap(begin, new_begin);
    EXPECT_EQ(*begin, 4.5);
    EXPECT_EQ(*new_begin, 1.2);

    begin = new_begin;
    EXPECT_EQ(*begin, *new_begin);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18129029
TYPED_TEST(container_test_basics, input_iterator_concepts)
{
    auto container = this->setup_container();
    score::cpp::member_iterator<decltype(container.begin()), double, &test_struct::m1> begin(container.begin());
    score::cpp::member_iterator<decltype(container.end()), double, &test_struct::m1> end(container.end());

    EXPECT_EQ(*begin++, 1.2);
    EXPECT_EQ(*begin, 4.5);

    EXPECT_TRUE(begin != end);
    EXPECT_FALSE(begin == end);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18129029
TYPED_TEST(container_test_basics, forward_iterator_concepts)
{
    auto container = this->setup_container();
    score::cpp::member_iterator<decltype(container.begin()), double, &test_struct::m1> begin; // default constructable
    begin = (container.begin());
    score::cpp::member_iterator<decltype(container.end()), double, &test_struct::m1> end(container.end());

    auto iter = std::find(begin, end, 4.5);
    ASSERT_TRUE(iter != end);
    EXPECT_EQ(*iter, 4.5);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18129029
TYPED_TEST(container_test_bi_directional, bidirectional_iterator_concepts)
{
    auto container = this->setup_container();
    score::cpp::member_iterator<decltype(container.begin()), double, &test_struct::m1> begin(container.begin());
    score::cpp::member_iterator<decltype(container.end()), double, &test_struct::m1> end(container.end());

    auto new_begin = std::next(begin);
    EXPECT_TRUE(--new_begin == begin);
    EXPECT_EQ(*new_begin, 1.2);

    new_begin = begin++;
    EXPECT_EQ(*begin--, 4.5);
    EXPECT_EQ(*new_begin, 1.2);
    EXPECT_EQ(*begin, 1.2);
    EXPECT_EQ(new_begin, begin);
    begin = container.begin();
    std::reverse(begin, end);
    EXPECT_EQ(*begin, 7.8);
    EXPECT_EQ(container.begin()->m1, 7.8);
    EXPECT_EQ(container.begin()->m2, 3);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18129029
TYPED_TEST(container_test_random_access, random_access_iterator_concepts)
{
    auto container = this->setup_container();
    score::cpp::member_iterator<decltype(container.begin()), int, &test_struct::m2> begin(container.begin());
    score::cpp::member_iterator<decltype(container.end()), int, &test_struct::m2> end(container.end());

    EXPECT_TRUE(begin < end);
    EXPECT_TRUE(begin <= begin);

    EXPECT_TRUE(end > begin);
    EXPECT_TRUE(begin >= begin);

    EXPECT_EQ(*(begin + 1), 6);
    EXPECT_EQ((begin + 1), (1 + begin));

    begin += 2;
    EXPECT_EQ(*begin, 9);

    EXPECT_EQ(*(begin - 1), 6);
    begin -= 2;
    EXPECT_EQ((end - begin), container.size());

    EXPECT_EQ(begin[0], 3);
    EXPECT_EQ(begin[1], 6);
    EXPECT_EQ(begin[2], 9);
}
