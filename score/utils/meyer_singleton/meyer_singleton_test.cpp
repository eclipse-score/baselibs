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
#include "score/utils/meyer_singleton/meyer_singleton.h"

#include "score/concurrency/atomic_indirector.h"
#include "score/concurrency/atomic_mock.h"
#include "score/utils/meyer_singleton/test/single_test_per_process_fixture.h"

#include <score/assert_support.hpp>
#include <score/callback.hpp>
#include <score/utility.hpp>

#include <gtest/gtest.h>

#include <cstdint>

namespace score::singleton
{
namespace
{

using namespace ::testing;

template <typename T>
using AtomicIndirectorMock = concurrency::AtomicIndirectorMock<T>;

constexpr int kDummyInt{10U};
constexpr double kDummyDouble{2.0};

class InitializationTrackedClass
{
  public:
    InitializationTrackedClass()
    {
        class_initialization_count_++;
    }

    InitializationTrackedClass(const InitializationTrackedClass&) = delete;
    InitializationTrackedClass& operator=(const InitializationTrackedClass&) & = delete;
    InitializationTrackedClass(InitializationTrackedClass&&) noexcept = delete;
    InitializationTrackedClass& operator=(InitializationTrackedClass&&) & noexcept = delete;

    ~InitializationTrackedClass() = default;

    static std::atomic<std::uint8_t> class_initialization_count_;
};
std::atomic<std::uint8_t> InitializationTrackedClass::class_initialization_count_{0U};

class DummyClass
{
  public:
    DummyClass(int a_in, double b_in) : a_{a_in}, b_{b_in} {}

    DummyClass(const DummyClass&) = delete;
    DummyClass& operator=(const DummyClass&) & = delete;
    DummyClass(DummyClass&&) noexcept = delete;
    DummyClass& operator=(DummyClass&&) & noexcept = delete;

    ~DummyClass() = default;

    int a_;
    double b_;
};

class AtomicMockGuard
{
  public:
    AtomicMockGuard()
    {
        AtomicIndirectorMock<InitializationState>::SetMockObject(&atomic_mock_);
    }

    ~AtomicMockGuard()
    {
        AtomicIndirectorMock<InitializationState>::SetMockObject(nullptr);
    }

    AtomicMockGuard(const AtomicMockGuard&) = delete;
    AtomicMockGuard& operator=(const AtomicMockGuard&) & = delete;
    AtomicMockGuard(AtomicMockGuard&&) noexcept = delete;
    AtomicMockGuard& operator=(AtomicMockGuard&&) & noexcept = delete;

    concurrency::AtomicMock<InitializationState> atomic_mock_{};
};

class MeyerSingletonFixture : public test::SingleTestPerProcessFixture
{
  public:
    ~MeyerSingletonFixture()
    {
        InitializationTrackedClass::class_initialization_count_ = 0U;
    }

    score::cpp::callback<InitializationTrackedClass()> CreateCallCountingInitializationCallback()
    {
        return [this]() -> InitializationTrackedClass {
            callable_call_count_++;
            return InitializationTrackedClass{};
        };
    }

    score::cpp::callback<DummyClass()> CreateDummyInitializationCallback(const int int_value, const double double_value)
    {
        return [int_value, double_value]() -> DummyClass {
            return DummyClass{int_value, double_value};
        };
    }

    std::uint8_t callable_call_count_{0U};
};

using MeyerSingletonGetInstanceFixture = MeyerSingletonFixture;
TEST_F(MeyerSingletonGetInstanceFixture, CallingWillInitializeObjectWithProvidedArguments)
{
    TestInSeparateProcess([]() {
        // When calling GetInstance with construction parameters
        auto& singleton = MeyerSingleton<DummyClass>::GetInstance(kDummyInt, kDummyDouble);

        // Then the template object will be initialized with the provided construction parameters
        EXPECT_EQ(singleton.a_, kDummyInt);
        EXPECT_EQ(singleton.b_, kDummyDouble);
    });
}

using MeyerSingletonGetInstanceWithCallableFixture = MeyerSingletonFixture;
TEST_F(MeyerSingletonGetInstanceWithCallableFixture, CallingWillInitializeObjectWithProvidedArguments)
{
    TestInSeparateProcess([]() {
        // When calling GetInstance with callable which constructs object with construction parameters
        auto construction_lambda = []() {
            return DummyClass{kDummyInt, kDummyDouble};
        };
        auto& singleton = MeyerSingleton<DummyClass>::GetInstanceInitializedWithCallable(construction_lambda);

        // Then the template object will be initialized with the provided construction parameters
        EXPECT_EQ(singleton.a_, kDummyInt);
        EXPECT_EQ(singleton.b_, kDummyDouble);
    });
}

TEST_F(MeyerSingletonGetInstanceFixture, CallingTwiceWillInitializeObjectOnce)
{
    TestInSeparateProcess([]() {
        // When calling GetInstance twice
        const std::uint8_t initialization_count_before_call{InitializationTrackedClass::class_initialization_count_};

        score::cpp::ignore = MeyerSingleton<InitializationTrackedClass>::GetInstance();
        const std::uint8_t initialization_count_after_first_call{
            InitializationTrackedClass::class_initialization_count_};

        score::cpp::ignore = MeyerSingleton<InitializationTrackedClass>::GetInstance();
        const std::uint8_t initialization_count_after_second_call{
            InitializationTrackedClass::class_initialization_count_};

        // Then the object should only have been initialized after the first call to GetInstance
        EXPECT_EQ(initialization_count_before_call, 0U);
        EXPECT_EQ(initialization_count_after_first_call, 1U);
        EXPECT_EQ(initialization_count_after_second_call, 1U);
    });
}

TEST_F(MeyerSingletonGetInstanceWithCallableFixture, CallingTwiceWillInitializeObjectOnce)
{
    TestInSeparateProcess([]() {
        auto construction_lambda = []() {
            return InitializationTrackedClass{};
        };

        // When calling GetInstance twice
        const std::uint8_t initialization_count_before_call{InitializationTrackedClass::class_initialization_count_};

        score::cpp::ignore =
            MeyerSingleton<InitializationTrackedClass>::GetInstanceInitializedWithCallable(construction_lambda);
        const std::uint8_t initialization_count_after_first_call{
            InitializationTrackedClass::class_initialization_count_};

        score::cpp::ignore =
            MeyerSingleton<InitializationTrackedClass>::GetInstanceInitializedWithCallable(construction_lambda);
        const std::uint8_t initialization_count_after_second_call{
            InitializationTrackedClass::class_initialization_count_};

        // Then the object should only have been initialized after the first call to GetInstanceWithCallable
        EXPECT_EQ(initialization_count_before_call, 0U);
        EXPECT_EQ(initialization_count_after_first_call, 1U);
        EXPECT_EQ(initialization_count_after_second_call, 1U);
    });
}

TEST_F(MeyerSingletonGetInstanceWithCallableFixture, CallingTwiceWillCallInitializationCallableOnce)
{
    TestInSeparateProcess([this]() {
        // Given an initialization callable which tracks how many times it's been invoked
        auto construction_callable = CreateCallCountingInitializationCallback();

        // When calling GetInstance twice
        const std::uint8_t callable_call_count_before_call{callable_call_count_};

        score::cpp::ignore =
            MeyerSingleton<InitializationTrackedClass>::GetInstanceInitializedWithCallable(construction_callable);
        const std::uint8_t callable_call_count_after_first_call{callable_call_count_};

        score::cpp::ignore =
            MeyerSingleton<InitializationTrackedClass>::GetInstanceInitializedWithCallable(construction_callable);
        const std::uint8_t callable_call_count_after_second_call{callable_call_count_};

        // Then the object should only have been initialized after the first call to GetInstanceWithCallable
        EXPECT_EQ(callable_call_count_before_call, 0U);
        EXPECT_EQ(callable_call_count_after_first_call, 1U);
        EXPECT_EQ(callable_call_count_after_second_call, 1U);
    });
}

TEST_F(MeyerSingletonGetInstanceFixture, CallingWhileOtherThreadIsCallingGetInstanceReturnsValidObject)
{
    TestInSeparateProcess([]() {
        AtomicMockGuard atomic_mock_guard{};

        ON_CALL(atomic_mock_guard.atomic_mock_, load(_)).WillByDefault(Return(InitializationState::NOT_INITIALIZED));

        // Expecting that compare_exchange_strong tries to set the state to INITIALIZED_WITHOUT_CALLABLE but this
        // returns false and behaves as if another thread had called GetInstance which set the value to
        // INITIALIZED_WITHOUT_CALLABLE in the meantime
        EXPECT_CALL(atomic_mock_guard.atomic_mock_,
                    compare_exchange_strong(_, InitializationState::INITIALIZED_WITHOUT_CALLABLE, _))
            .WillOnce(WithArg<0>(Invoke(([](auto& expected) {
                expected = InitializationState::INITIALIZED_WITHOUT_CALLABLE;
                return false;
            }))));

        // When calling GetInstance
        auto& singleton = MeyerSingleton<DummyClass, AtomicIndirectorMock>::GetInstance(kDummyInt, kDummyDouble);

        // Then a valid object will be returned (in production, this will be the object constructed by the other
        // thread which wrote to the InitializationState at the same time as this thread. But in the test, it is the
        // object created by this thread (since the other thread doesn't actually exist, we just emulate its behaviour
        // with the atomic_mock_). This test is mainly ensuring that we don't crash due to a violated precondition.
        EXPECT_EQ(singleton.a_, kDummyInt);
        EXPECT_EQ(singleton.b_, kDummyDouble);
    });
}

TEST_F(MeyerSingletonGetInstanceWithCallableFixture,
       CallingWhileOtherThreadIsCallingGetInstanceWithCallableReturnsValidObject)
{
    TestInSeparateProcess([]() {
        AtomicMockGuard atomic_mock_guard{};

        ON_CALL(atomic_mock_guard.atomic_mock_, load(_)).WillByDefault(Return(InitializationState::NOT_INITIALIZED));

        // Expecting that compare_exchange_strong tries to set the state to INITIALIZED_WITH_CALLABLE but this
        // returns false and behaves as if another thread called GetInstanceWithCallable had set the value to
        // INITIALIZED_WITH_CALLABLE in the meantime
        EXPECT_CALL(atomic_mock_guard.atomic_mock_,
                    compare_exchange_strong(_, InitializationState::INITIALIZED_WITH_CALLABLE, _))
            .WillOnce(WithArg<0>(Invoke(([](auto& expected) {
                expected = InitializationState::INITIALIZED_WITH_CALLABLE;
                return false;
            }))));

        // When calling GetInstance with callable which constructs object with construction parameters
        auto construction_lambda = []() {
            return DummyClass{kDummyInt, kDummyDouble};
        };
        auto& singleton =
            MeyerSingleton<DummyClass, AtomicIndirectorMock>::GetInstanceInitializedWithCallable(construction_lambda);

        // Then a valid object will be returned (in production, this will be the object constructed by the other
        // thread which wrote to the InitializationState at the same time as this thread. But in the test, it is the
        // object created by this thread (since the other thread doesn't actually exist, we just emulate its behaviour
        // with the atomic_mock_). This test is mainly ensuring that we don't crash due to a violated precondition.
        EXPECT_EQ(singleton.a_, kDummyInt);
        EXPECT_EQ(singleton.b_, kDummyDouble);
    });
}

TEST_F(MeyerSingletonGetInstanceFixture, CallingAfterCallingGetInstanceWithCallableTerminates)
{
    TestInSeparateProcess([this]() {
        // Given that GetInstanceInitializedWithCallable was already called
        score::cpp::ignore = MeyerSingleton<InitializationTrackedClass>::GetInstanceInitializedWithCallable(
            CreateCallCountingInitializationCallback());

        // When calling GetInstance
        // Then the program terminates
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(
            score::cpp::ignore = MeyerSingleton<InitializationTrackedClass>::GetInstance());
    });
}

TEST_F(MeyerSingletonGetInstanceFixture, CallingWhileOtherThreadIsCallingGetInstanceWithCallableTerminates)
{
    TestInSeparateProcess([]() {
        AtomicMockGuard atomic_mock_guard{};

        ON_CALL(atomic_mock_guard.atomic_mock_, load(_)).WillByDefault(Return(InitializationState::NOT_INITIALIZED));

        // Expecting that compare_exchange_strong tries to set the state to INITIALIZED_WITHOUT_CALLABLE but this
        // returns false and behaves as if another thread called GetInstanceWithCallable had set the value to
        // INITIALIZED_WITH_CALLABLE in the meantime
        EXPECT_CALL(atomic_mock_guard.atomic_mock_,
                    compare_exchange_strong(_, InitializationState::INITIALIZED_WITHOUT_CALLABLE, _))
            .WillOnce(WithArg<0>(Invoke(([](auto& expected) {
                expected = InitializationState::INITIALIZED_WITH_CALLABLE;
                return false;
            }))));

        // When calling GetInstance
        // Then the program terminates
        using MeyerSingletonType = MeyerSingleton<DummyClass, AtomicIndirectorMock>;
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::ignore =
                                                              MeyerSingletonType::GetInstance(kDummyInt, kDummyDouble));
    });
}

TEST_F(MeyerSingletonGetInstanceWithCallableFixture, CallingAfterCallingGetInstanceTerminates)
{
    TestInSeparateProcess([this]() {
        // Given that GetInstance was already called
        score::cpp::ignore = MeyerSingleton<DummyClass>::GetInstance(kDummyInt + 1, kDummyDouble + 1.0);

        // When calling GetInstanceInitializedWithCallable
        // Then the program terminates
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(
            score::cpp::ignore = MeyerSingleton<DummyClass>::GetInstanceInitializedWithCallable(
                CreateDummyInitializationCallback(kDummyInt, kDummyDouble)));
    });
}

TEST_F(MeyerSingletonGetInstanceWithCallableFixture, CallingWhileOtherThreadIsCallingGetInstanceTerminates)
{
    TestInSeparateProcess([this]() {
        AtomicMockGuard atomic_mock_guard{};

        ON_CALL(atomic_mock_guard.atomic_mock_, load(_)).WillByDefault(Return(InitializationState::NOT_INITIALIZED));

        // Expecting that compare_exchange_strong tries to set the state to INITIALIZED_WITH_CALLABLE but this
        // returns false and behaves as if another thread had called GetInstance which set the value to
        // INITIALIZED_WITHOUT_CALLABLE in the meantime
        EXPECT_CALL(atomic_mock_guard.atomic_mock_,
                    compare_exchange_strong(_, InitializationState::INITIALIZED_WITH_CALLABLE, _))
            .WillOnce(WithArg<0>(Invoke(([](auto& expected) {
                expected = InitializationState::INITIALIZED_WITHOUT_CALLABLE;
                return false;
            }))));

        // When calling GetInstance
        // Then the program terminates
        using MeyerSingletonType = MeyerSingleton<DummyClass, AtomicIndirectorMock>;
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(
            score::cpp::ignore = MeyerSingletonType::GetInstanceInitializedWithCallable(
                CreateDummyInitializationCallback(kDummyInt, kDummyDouble)));
    });
}

}  // namespace
}  // namespace score::singleton
