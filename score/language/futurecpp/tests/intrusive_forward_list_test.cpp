/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

#include <score/private/container/intrusive_forward_list.hpp>
#include <score/private/container/intrusive_forward_list.hpp> // test include guard

#include <score/assert_support.hpp>
#include <score/utility.hpp>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace detail
{
namespace
{

struct element : intrusive_forward_list_node
{
    explicit element(const std::int32_t value) : intrusive_forward_list_node{}, x{value} {}
    element(const element&) = default;
    element& operator=(const element&) = default;
    element(element&&) = default;
    element& operator=(element&&) = default;
    ~element() { SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(!is_linked()); }

    std::int32_t x;
};

class given_empty_intrusive_forward_list : public ::testing::Test
{
protected:
    element a_{23};
    element b_{42};
    element c_{72};
    intrusive_forward_list<element> list_{};
};

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenDefaultConstructedList_ExpectEmptyList) { EXPECT_TRUE(list_.empty()); }

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenMovedList_ExpectEmptyList)
{
    intrusive_forward_list<element> l{std::move(list_)};
    EXPECT_TRUE(l.empty());
    EXPECT_TRUE(list_.empty());
}

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenMoveAssignedList_ExpectEmptyList)
{
    intrusive_forward_list<element> l{};
    l = std::move(list_);
    EXPECT_TRUE(l.empty());
    EXPECT_TRUE(list_.empty());
}

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenClear_ExpectEmptyList)
{
    list_.clear();
    EXPECT_TRUE(list_.empty());
}

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenPopFrontOnEmptyList_ExpectContractViolation)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(list_.pop_front());
}

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenBackOnEmptyList_ExpectContractViolation)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(list_.back());
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::as_const(list_).back());
}

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenFrontOnEmptyList_ExpectContractViolation)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(list_.front());
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::as_const(list_).front());
}

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenPushAnElement_ExpectElementIsPartOfList)
{
    list_.push_back(a_);

    EXPECT_FALSE(list_.empty());
    EXPECT_EQ(&a_, &list_.front());
    EXPECT_EQ(&a_, &list_.back());
    EXPECT_EQ(&a_, &score::cpp::as_const(list_).front());
    EXPECT_EQ(&a_, &score::cpp::as_const(list_).back());
}

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenPushAnElement_ExpectElementIsLinked)
{
    EXPECT_FALSE(a_.is_linked());

    list_.push_back(a_);

    EXPECT_TRUE(a_.is_linked());
}

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenSwapAndPushAnElementToThis_ExpectThisHasAnElementAndOtherIsEmpty)
{
    intrusive_forward_list<element> other{};

    EXPECT_TRUE(other.empty());
    EXPECT_TRUE(list_.empty());

    list_.swap(other);
    list_.push_back(a_);

    EXPECT_TRUE(other.empty());
    EXPECT_FALSE(list_.empty());
    EXPECT_EQ(&a_, &list_.front());
}

// NOTRACING
TEST_F(given_empty_intrusive_forward_list, GivenSwapAndPushAnElementToOther_ExpectThisIsEmptyAndOtherHasAnElement)
{
    intrusive_forward_list<element> other{};

    EXPECT_TRUE(other.empty());
    EXPECT_TRUE(list_.empty());

    list_.swap(other);
    other.push_back(a_);

    EXPECT_TRUE(list_.empty());
    EXPECT_FALSE(other.empty());
    EXPECT_EQ(&a_, &other.front());
}

class given_intrusive_forward_list_with_single_element : public given_empty_intrusive_forward_list
{
protected:
    given_intrusive_forward_list_with_single_element() : given_empty_intrusive_forward_list{} { list_.push_back(a_); }
};

// NOTRACING
TEST_F(given_intrusive_forward_list_with_single_element, GivenMoveAssignedList_ExpectBothListsEmpty)
{
    intrusive_forward_list<element> l{};
    EXPECT_TRUE(l.empty());
    EXPECT_FALSE(list_.empty());

    list_ = std::move(l);

    EXPECT_TRUE(l.empty());
    EXPECT_TRUE(list_.empty());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_single_element, GivenMoveAssignedListToSelf_ExpectIdentity)
{
    list_ = static_cast<intrusive_forward_list<element>&&>(list_);
    EXPECT_FALSE(list_.empty());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_single_element, GivenPopOnSingleElementList_ExpectEmptyList)
{
    list_.pop_front();
    EXPECT_TRUE(list_.empty());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_single_element, GivenPop_ExpectElementIsUnlinked)
{
    EXPECT_TRUE(a_.is_linked());

    list_.pop_front();

    EXPECT_FALSE(a_.is_linked());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_single_element, GivenClear_ExpectEmptyList)
{
    list_.clear();
    EXPECT_TRUE(list_.empty());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_single_element, GivenClear_ExpectElementIsUnlinked)
{
    EXPECT_TRUE(a_.is_linked());

    list_.clear();

    EXPECT_FALSE(a_.is_linked());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_single_element, GivenClear_ExpectElementsCanBePushedAgain)
{
    list_.clear();
    EXPECT_TRUE(list_.empty());

    list_.push_back(a_);
    EXPECT_EQ(&a_, &list_.front());
    EXPECT_EQ(&a_, &list_.back());
    EXPECT_FALSE(list_.empty());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_single_element, GivenPushElementTwice_ExpectContractViolation)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(list_.push_back(a_));
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_single_element, GivenPopAndPushElementAgain_ExpectElementIsAgainPartOfList)
{
    list_.pop_front();
    EXPECT_TRUE(list_.empty());

    list_.push_back(a_);
    EXPECT_EQ(&a_, &list_.front());
    EXPECT_EQ(&a_, &list_.back());
    EXPECT_FALSE(list_.empty());
}

class given_intrusive_forward_list_with_three_element : public given_intrusive_forward_list_with_single_element
{
protected:
    given_intrusive_forward_list_with_three_element() : given_intrusive_forward_list_with_single_element{}
    {
        list_.push_back(b_);
        list_.push_back(c_);
    }
};

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenMovedList_ExpectEmptyList)
{
    intrusive_forward_list<element> l{std::move(list_)};
    EXPECT_EQ(&a_, &l.front());
    EXPECT_EQ(&c_, &l.back());
    EXPECT_TRUE(list_.empty());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenMoveAssignedList_ExpectEmptyList)
{
    intrusive_forward_list<element> l{};
    l = std::move(list_);
    EXPECT_EQ(&a_, &l.front());
    EXPECT_EQ(&c_, &l.back());
    EXPECT_TRUE(list_.empty());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenPopThreeElements_ExpectdEmptyList)
{
    EXPECT_EQ(&a_, &list_.front());
    EXPECT_EQ(&c_, &list_.back());

    list_.pop_front();
    EXPECT_EQ(&b_, &list_.front());
    EXPECT_EQ(&c_, &list_.back());

    list_.pop_front();
    EXPECT_EQ(&c_, &list_.front());
    EXPECT_EQ(&c_, &list_.back());

    list_.pop_front();
    EXPECT_TRUE(list_.empty());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenCopiedElementFromList_ExpectElementCanBePushedAgain)
{
    element copy_of_a{list_.front()};
    EXPECT_FALSE(copy_of_a.is_linked());

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(list_.push_back(copy_of_a));
    EXPECT_EQ(&a_, &list_.front());
    EXPECT_EQ(&copy_of_a, &list_.back());
    EXPECT_TRUE(copy_of_a.is_linked());

    list_.clear(); // we push a stack element so clear the list
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenCopyAssignedElementFromList_ExpectElementCanBePushedAgain)
{
    element copy_of_a{72};
    copy_of_a = list_.front();
    EXPECT_FALSE(copy_of_a.is_linked());

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(list_.push_back(copy_of_a));
    EXPECT_EQ(&a_, &list_.front());
    EXPECT_EQ(&copy_of_a, &list_.back());
    EXPECT_TRUE(copy_of_a.is_linked());

    list_.clear(); // we push a stack element so clear the list
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenMovedElementFromList_ExpectElementCanBePushedAgain)
{
    element copy_of_a{std::move(list_.front())};
    EXPECT_FALSE(copy_of_a.is_linked());

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(list_.push_back(copy_of_a));
    EXPECT_EQ(&a_, &list_.front());
    EXPECT_EQ(&copy_of_a, &list_.back());
    EXPECT_TRUE(copy_of_a.is_linked());

    list_.clear(); // we push a stack element so clear the list
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenMoveAssignedElementFromList_ExpectElementCanBePushedAgain)
{
    element copy_of_a{72};
    copy_of_a = std::move(list_.front());
    EXPECT_FALSE(copy_of_a.is_linked());

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(list_.push_back(copy_of_a));
    EXPECT_EQ(&a_, &list_.front());
    EXPECT_EQ(&copy_of_a, &list_.back());
    EXPECT_TRUE(copy_of_a.is_linked());

    list_.clear(); // we push a stack element so clear the list
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenReferenceToElementFromList_ExpectContractViolation)
{
    element& reference_to_a{list_.front()};
    EXPECT_TRUE(reference_to_a.is_linked());

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(list_.push_back(reference_to_a));
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenPushElementTwice_ExpectContractViolation)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(list_.push_back(a_));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(list_.push_back(b_));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(list_.push_back(c_));
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenPopAndPushElementAgain_ExpectElementIsAgainPartOfList)
{
    list_.pop_front();
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(list_.push_back(a_));
    EXPECT_EQ(&b_, &list_.front());
    EXPECT_EQ(&a_, &list_.back());

    list_.pop_front();
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(list_.push_back(b_));
    EXPECT_EQ(&c_, &list_.front());
    EXPECT_EQ(&b_, &list_.back());

    list_.pop_front();
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(list_.push_back(c_));
    EXPECT_EQ(&a_, &list_.front());
    EXPECT_EQ(&c_, &list_.back());
}

// NOTRACING
TEST_F(given_intrusive_forward_list_with_three_element, GivenSecondListAndPushElementsAgain_ExpectContractViolation)
{
    intrusive_forward_list<element> another_list{};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(another_list.push_back(a_));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(another_list.push_back(b_));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(another_list.push_back(c_));
}

} // namespace
} // namespace detail
} // namespace score::cpp
