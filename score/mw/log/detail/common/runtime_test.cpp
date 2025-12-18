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
#include "score/mw/log/runtime.h"

#include "score/mw/log/detail/common/recorder_config.h"
#include "score/mw/log/detail/empty_recorder.h"
#include "score/mw/log/recorder_mock.h"

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
bool IsRecorderOfType(const Recorder& recorder) noexcept
{
    static_assert(std::is_base_of<Recorder, ConcreteRecorder>::value,
                  "Concrete recorder shall be derived from Recorder base class");

    return dynamic_cast<const ConcreteRecorder*>(&recorder) != nullptr;
}

class RuntimeFixture : public ::testing::Test
{
  public:
    RecorderMock recorder_mock_{};
};

TEST_F(RuntimeFixture, CanSetALoggingBackend)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability to set backend logging without exception.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given an empty process

    // When setting the recorder for e.g. testing purposes
    Runtime::SetRecorder(&recorder_mock_);

    // Then no exception happens (API test)
}

TEST_F(RuntimeFixture, CanRetrieveSetRecorder)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability to get the recorder properly.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given the recorder was already set
    Runtime::SetRecorder(&recorder_mock_);

    // When trying to read the current recorder
    // Then it is the one that was previously stored
    EXPECT_EQ(&recorder_mock_, &Runtime::GetRecorder());
}

TEST_F(RuntimeFixture, CanRetrieveFallbackRecorder)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability to get a fallback recorder.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given the runtime was initialized

    // When trying to read the fallback recorder
    const auto& recorder = Runtime::GetFallbackRecorder();

    // Then we receive a text recorder if KConsole enabled, otherwise empty recorder
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RuntimeFixture, DefaultRecorderShallBeReturned)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verify the ability of returning the default recorder in case of null recorder is set.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given the unset recorder:
    Runtime::SetRecorder(&recorder_mock_);
    auto& recorder = Runtime::GetRecorder();
    Runtime::SetRecorder(nullptr);

    EXPECT_TRUE(IsRecorderOfType<ConsoleRecorderType>(Runtime::GetRecorder()));

    // Revert previously stored recorder:
    Runtime::SetRecorder(&recorder);
}

TEST_F(RuntimeFixture, WithLoggerContainerHasFreeCapacityExpectedThatNewLoggerContainsCorrectContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verify if logger container has capacity will create this logger if it's not available.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::string_view context{"ctx"};
    EXPECT_EQ(context, Runtime::GetLoggerContainer().GetLogger(context).GetContext());
}

TEST(RuntimeTest, RuntimeInitializationWithPointer)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "This suite only exists to test the second branch of the runtime initialization. Since this is "
                   "static state we need a separate binary for this.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // This suite only exists to test the second branch of the runtime initialization.
    // Since this is static state we need a separate binary for this.
    EmptyRecorder r;
    Runtime::SetRecorder(&r);
    EXPECT_EQ(&Runtime::GetRecorder(), &r);
    Runtime::SetRecorder(nullptr);
    // Even after resetting the recorder GetRecorder() shall return a valid reference to a stub recorder.
    // We enforce checking this by calling an arbitrary method on the reference.
    // Address sanitizer and valgrind would detect a memory error if the implementation is faulty.
    Runtime::GetRecorder().IsLogEnabled(LogLevel::kVerbose, "");
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
