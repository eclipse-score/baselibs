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
#include "score/mw/log/detail/recorder_factory_stub.h"
#include "score/mw/log/detail/empty_recorder.h"
#include "score/mw/log/detail/text_recorder/text_recorder.h"

#include "gtest/gtest.h"

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

TEST(RecorderFactoryStubTest, CreateStubShallReturnEmptyRecorderforCreateRecorderFactory)
{
    RecordProperty("Description", "CreateRecorderFactory shall return a RecorderFactory");
    RecordProperty("TestingTechnique", "Interface-based test");

    auto recorder_factory = CreateRecorderFactory();

    auto recorder_ = recorder_factory->CreateStub();

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder_));
}

TEST(RecorderFactoryStubTest, CreateStubShallReturnEmptyRecorder)
{
    RecordProperty("Description", "RecorderFactory shall create EmptyRecorder in case of using CreateStub.");
    RecordProperty("TestingTechnique", "Interface-based test");

    auto recorder_ = RecorderFactory{}.CreateStub();

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder_));
}

TEST(RecorderFactoryStubTest, CreateWithConsoleLoggingOnlyShallReturnPtr)
{
    RecordProperty(
        "Description",
        "RecorderFactoryStub shall return ptr to TextRecorder in case of using CreateWithConsoleLoggingOnly.");
    RecordProperty("TestingTechnique", "Interface-based test");

    score::cpp::pmr::memory_resource* null_memory_resource = nullptr;
    auto recorder_ = RecorderFactory{}.CreateWithConsoleLoggingOnly(null_memory_resource);

    EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder_));
}

TEST(RecorderFactoryStubTest, CreateFromConfigurationShallReturnPtr)
{
    RecordProperty("Description",
                   "RecorderFactoryStub shall return ptr to TextRecorder in case of using CreateFromConfiguration.");
    RecordProperty("TestingTechnique", "Interface-based test");

    score::cpp::pmr::memory_resource* null_memory_resource = nullptr;
    auto recorder_ = RecorderFactory{}.CreateFromConfiguration(null_memory_resource);

    EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder_));
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
