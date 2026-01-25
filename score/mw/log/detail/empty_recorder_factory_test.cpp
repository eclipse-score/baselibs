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

#include "score/mw/log/detail/empty_recorder_factory.h"
#include "gtest/gtest.h"

#include <type_traits>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{
template <typename ConcreteRecorder>
bool IsRecorderOfType(const std::unique_ptr<Recorder>& recorder) noexcept
{
    static_assert(std::is_base_of<Recorder, ConcreteRecorder>::value,
                  "Concrete recorder shall be derived from Recorder base class");

    return dynamic_cast<const ConcreteRecorder*>(recorder.get()) != nullptr;
}

TEST(EmptyRecorderFactory, TestCreateEmptyRecorder)
{
    const Configuration config;
    auto recorder = EmptyRecorderFactory{}.CreateLogRecorder(config, nullptr);
    ASSERT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST(EmptyRecorderFactory, TestCreateEmptyRecorderWithParameter)
{
    const Configuration config;
    // fcntl_instance is not used, so nullptr is ok for test
    auto recorder = EmptyRecorderFactory{nullptr}.CreateLogRecorder(config, nullptr);
    ASSERT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
