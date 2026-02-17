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
#include "score/language/safecpp/safe_atomics/try_atomic_add.h"
#include "score/language/safecpp/safe_atomics/error.h"
#include "score/language/safecpp/safe_math/details/test_type_collection.h"
#include "score/language/safecpp/safe_math/error.h"
#include "score/memory/shared/atomic_mock.h"

#include "gtest/gtest.h"
#include "score/result/result.h"

#include <limits>

namespace score::safe_atomics
{
namespace
{

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::WithArgs;

template <typename T>
class AtomicFetchAddTest : public ::testing::Test
{
  public:
    AtomicFetchAddTest& GivenAMockedAtomicIndirector()
    {
        memory::shared::AtomicIndirectorMock<T>::SetMockObject(&atomic_mock_);
        return *this;
    }

    void TearDown() override
    {
        memory::shared::AtomicIndirectorMock<T>::SetMockObject(nullptr);
    }

    memory::shared::AtomicMock<T> atomic_mock_{};
};

TYPED_TEST_SUITE(AtomicFetchAddTest, safe_math::IntegerTypes, /*unused*/);

TYPED_TEST(AtomicFetchAddTest, FetchAddingWithoutOverflowWorks)
{
    // When trying to fetch_add a value which would not overflow if fetch_added
    std::atomic<TypeParam> test_value{10};
    const TypeParam addition_value{15};
    const auto fetch_added_value_result = TryAtomicAdd(test_value, addition_value);

    // Then the value before fetch_adding should be returned
    ASSERT_TRUE(fetch_added_value_result.has_value());
    EXPECT_EQ(fetch_added_value_result.value(), TypeParam{10});

    // and the atomic should have been fetch_added
    EXPECT_EQ(test_value.load(), TypeParam{25});
}

TYPED_TEST(AtomicFetchAddTest, FetchAddingWithOverflowReturnsError)
{
    // When trying to fetch_add a value which would overflow if fetch_added
    std::atomic<TypeParam> test_value{std::numeric_limits<TypeParam>::max()};
    const TypeParam addition_value{1};
    const auto fetch_added_value_result = TryAtomicAdd(test_value, addition_value);

    // Then an error should be returned
    ASSERT_FALSE(fetch_added_value_result.has_value());
    EXPECT_EQ(fetch_added_value_result.error(), ErrorCode::kExceedsNumericLimits);

    // and the atomic should not have been fetch_added
    EXPECT_EQ(test_value.load(), std::numeric_limits<TypeParam>::max());
}

TYPED_TEST(AtomicFetchAddTest, WillReTryAtomicAddIfCompareExchangeFails)
{
    this->GivenAMockedAtomicIndirector();

    // Expecting that compare_exchange_strong will be called twice, first returning false then true
    ::testing::InSequence in_sequence{};
    EXPECT_CALL(this->atomic_mock_, compare_exchange_strong(_, _, _)).WillOnce(Return(false));
    EXPECT_CALL(this->atomic_mock_, compare_exchange_strong(_, _, _)).WillOnce(Return(true));

    // When trying to fetch_add a value which would not overflow if fetch_added
    std::atomic<TypeParam> test_value{10U};
    const TypeParam addition_value{11};
    constexpr std::size_t max_retries{10};
    const auto fetch_added_value_result = details::TryAtomicAddImpl<TypeParam, memory::shared::AtomicIndirectorMock>(
        test_value, addition_value, max_retries);

    // Then the value before fetch_adding should be returned
    ASSERT_TRUE(fetch_added_value_result.has_value());
    EXPECT_EQ(fetch_added_value_result.value(), TypeParam{10});
}

TYPED_TEST(AtomicFetchAddTest, WillReturnErrorIfCompareExchangeFailsNumRetriesTimes)
{
    this->GivenAMockedAtomicIndirector();

    // Expecting that compare_exchange_strong will be called num_retries times which returns false every time
    constexpr std::size_t num_retries{5U};
    EXPECT_CALL(this->atomic_mock_, compare_exchange_strong(_, _, _)).Times(num_retries).WillRepeatedly(Return(false));

    // When trying to fetch_add a value which would not overflow if fetch_added
    std::atomic<TypeParam> test_value{10U};
    const TypeParam addition_value{11};
    const auto fetch_added_value_result = details::TryAtomicAddImpl<TypeParam, memory::shared::AtomicIndirectorMock>(
        test_value, addition_value, num_retries);

    // Then an error should be returned
    ASSERT_FALSE(fetch_added_value_result.has_value());
    EXPECT_EQ(fetch_added_value_result.error(), ErrorCode::kMaxRetriesReached);
}

}  // namespace
}  // namespace score::safe_atomics
