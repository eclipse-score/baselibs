/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#include <gtest/gtest.h>

#include <score/utility.hpp>

#include <exception>
#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace score
{
namespace
{

void IndicateThatNoExceptionWasThrown()
{
    // Since we are using ASSERT_DEATHs below (which expect a non-zero exit code of the process),
    // we indicate a failure by simply exiting with code 0 here instead of non-zero.
    std::cerr << "\nFAILED: no exception was thrown but should have\n" << std::endl;
    std::exit(0);
}

struct SizeTThrower
{
    using ExceptionType = std::size_t;
    auto operator()()
    {
        throw std::size_t{};
    }
};

struct BadAllocThrower
{
    using ExceptionType = std::bad_alloc;
    auto operator()()
    {
        score::cpp::ignore = std::allocator<std::uint8_t>{}.allocate(std::numeric_limits<std::size_t>::max());
        FAIL() << "this line must never be reached";
    }
};

struct BadCastThrower
{
    using ExceptionType = std::bad_cast;
    auto operator()()
    {
        std::bad_cast derived{};
        std::exception& base = derived;
        score::cpp::ignore = dynamic_cast<std::bad_alloc&>(base);
        IndicateThatNoExceptionWasThrown();
    }
};

struct BadFunctionCallThrower
{
    using ExceptionType = std::bad_function_call;
    auto operator()()
    {
        std::function<void()> function;
        function();
        IndicateThatNoExceptionWasThrown();
    }
};

struct BadTypeIdThrower
{
    using ExceptionType = std::bad_typeid;
    auto operator()()
    {
        std::bad_typeid* ptr{nullptr};
        typeid(*ptr).name();
        IndicateThatNoExceptionWasThrown();
    }
};

struct BadWeakPtrThrower
{
    using ExceptionType = std::bad_weak_ptr;
    auto operator()()
    {
        std::shared_ptr<std::bad_weak_ptr>{std::weak_ptr<std::bad_weak_ptr>{}};
        IndicateThatNoExceptionWasThrown();
    }
};

struct LogicErrorThrower
{
    using ExceptionType = std::logic_error;
    auto operator()()
    {
        throw std::logic_error{"for testing"};
    }
};

struct RuntimeErrorThrower
{
    using ExceptionType = std::runtime_error;
    auto operator()()
    {
        throw std::runtime_error{"for testing"};
    }
};

template <typename ExceptionThrower>
class TestSafeExcept : public ::testing::Test
{
};

using ExceptionThrowerTypes = ::testing::Types<SizeTThrower,
                                               BadAllocThrower,
                                               BadCastThrower,
                                               BadFunctionCallThrower,
                                               BadTypeIdThrower,
                                               BadWeakPtrThrower,
                                               LogicErrorThrower,
                                               RuntimeErrorThrower>;

TYPED_TEST_SUITE(TestSafeExcept, ExceptionThrowerTypes, /*unused*/);

TYPED_TEST(TestSafeExcept, AllocationOfExceptionAbortsExecutionImmediately)
{
    this->RecordProperty("DerivationTechnique", "requirements-analysis");
    this->RecordProperty("ParentRequirement", "SSR-6593458");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty(
        "Description",
        "This test verifies that an exception, when being thrown, causes immediate abortion of the process.");

    using ExceptionThrower = TypeParam;

    try
    {
        // Given a piece of code where an exception gets thrown
        auto code_snippet = [] {
            ExceptionThrower exception_thrower{};
            exception_thrower();
        };

        // When safe_except lib got linked as done for this test
        // And the piece of code from above gets run
        // Then immediate termination is expected
        ASSERT_DEATH(code_snippet(), ".*");
    }
    catch (...)
    {
        FAIL() << "this line must never be reached";
    }
}

TYPED_TEST(TestSafeExcept, AllocationOfExceptionAbortsExecutionImmediatelyEvenWhenBeingCaughtByType)
{
    this->RecordProperty("DerivationTechnique", "requirements-analysis");
    this->RecordProperty("ParentRequirement", "SSR-6593458");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty(
        "Description",
        "This test verifies that an exception, when being allocated, causes immediate abortion of the process even "
        "for the case where an appropriate catch-block exists that would handle the thrown exception appropriately.");

    using ExceptionThrower = TypeParam;
    using ExceptionType = typename ExceptionThrower::ExceptionType;

    // Given a piece of code where an exception gets thrown but also caught by exact exception type
    auto code_snippet = [] {
        try
        {
            ExceptionThrower exception_thrower{};
            exception_thrower();
        }
        catch (const ExceptionType&)
        {
            FAIL() << "this line must never be reached";
        }
    };

    // When safe_except lib got linked as done for this test
    // And the piece of code from above gets run
    // Then immediate termination is expected
    ASSERT_DEATH(code_snippet(), ".*");
}

TYPED_TEST(TestSafeExcept, AllocationOfExceptionAbortsExecutionImmediatelyEvenWhenBeingCaughtByWildcard)
{
    this->RecordProperty("DerivationTechnique", "requirements-analysis");
    this->RecordProperty("ParentRequirement", "SSR-6593458");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty(
        "Description",
        "This test verifies that an exception, when being allocated, causes immediate abortion of the process even "
        "for the case where a wildcard catch-block exists that would handle the thrown exception appropriately.");

    using ExceptionThrower = TypeParam;

    // Given a piece of code where an exception gets thrown but also caught by wildcard
    auto code_snippet = [] {
        try
        {
            ExceptionThrower exception_thrower{};
            exception_thrower();
        }
        catch (...)
        {
            FAIL() << "this line must never be reached";
        }
    };

    // When safe_except lib got linked as done for this test
    // And the piece of code from above gets run
    // Then immediate termination is expected
    ASSERT_DEATH(code_snippet(), ".*");
}

}  // namespace
}  // namespace score
