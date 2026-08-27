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
#include "score/concurrency/simple_task.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <future>
#include <memory>

namespace score
{
namespace concurrency
{
namespace
{

class SimpleTaskTest : public ::testing::Test
{
};

TEST_F(SimpleTaskTest, ConstructionAndDestruction)
{
    std::promise<void> p{};
    auto task = SimpleTaskFactory::Make(score::cpp::pmr::get_default_resource(), [](const score::cpp::stop_token&) {});
}

TEST_F(SimpleTaskTest, ConstructionAndDestructionOnHeap)
{
    auto unique_task =
        SimpleTaskFactory::Make(score::cpp::pmr::get_default_resource(), [](const score::cpp::stop_token&) {});
    unique_task.reset();
}

TEST_F(SimpleTaskTest, ConstructionAndDestructionOnHeapWithBasePointer)
{
    score::cpp::pmr::unique_ptr<Task> unique_task =
        SimpleTaskFactory::Make(score::cpp::pmr::get_default_resource(), [](const score::cpp::stop_token&) {});
    unique_task.reset();
}

TEST_F(SimpleTaskTest, ExecutesVoidCallback)
{
    // Given a manually created SimpleTask<> with a void callback
    bool executed{false};
    auto unit = SimpleTaskFactory::Make(score::cpp::pmr::get_default_resource(),
                                        [&executed](const score::cpp::stop_token&) noexcept -> void {
                                            executed = true;
                                        });

    // When executing the function call operator
    (*unit)(score::cpp::stop_token{});

    // That the callback was executed
    ASSERT_TRUE(executed);
}

TEST_F(SimpleTaskTest, ExecutesCallbackWithParameter)
{
    // Given a manually created SimpleTask<> with an parameter
    std::int32_t observer{0};
    auto unit = SimpleTaskFactory::Make(
        score::cpp::pmr::get_default_resource(),
        [&observer](const score::cpp::stop_token&, const std::int32_t a) noexcept -> void {
            observer = a;
        },
        42);

    // When executing the function call operator
    (*unit)(score::cpp::stop_token{});

    // That the callback uses the correct parameter
    ASSERT_EQ(observer, 42);
}

TEST_F(SimpleTaskTest, ExecutesCallbackWithReturn)
{
    // Given a manually created SimpleTask<> with a return value
    auto unit = SimpleTaskFactory::MakeWithTaskResult(score::cpp::pmr::get_default_resource(),
                                                      [](const score::cpp::stop_token&) -> std::int32_t {
                                                          return 42;
                                                      });

    // When executing the function call operator
    (*unit.second)(score::cpp::stop_token{});

    // That the return value is stored in a future
    ASSERT_EQ(unit.first.Get().value(), 42);
}

class CustomTask
{
  public:
    std::uint32_t operator()(const score::cpp::stop_token) noexcept
    {
        return 42;
    }

  protected:
    // This member could perfectly be private, we only care that the size of the CustomTask is big enough
    // We set it to protected in this way there is no warning regarding unused private member
    // For the purpose of this test it is perfectly fine that is not used
    std::array<std::uint8_t, 255> heavy_workload_to_be_bigger_than_default_score_future_cpp_callback_size_{};
};

static_assert(sizeof(CustomTask) > 200, "The class is not big enough for these tests");

TEST_F(SimpleTaskTest, MakerFunctionWithCustomTask)
{
    // Given a task created using the make_* function
    CustomTask custom_task{};
    auto unit = SimpleTaskFactory::MakeWithTaskResult(score::cpp::pmr::get_default_resource(), custom_task);

    // When executing the function call operator
    (*unit.second)(score::cpp::stop_token{});

    // That the result is stored in a future
    ASSERT_EQ(unit.first.Get().value(), 42);
}

}  // namespace
}  // namespace concurrency
}  // namespace score
