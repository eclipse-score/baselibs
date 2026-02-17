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

#include "score/mw/log/detail/empty_recorder.h"

#if defined(KCONSOLE_LOGGING)
#include "score/mw/log/detail/text_recorder/text_recorder.h"
#endif

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

TEST(RuntimeTest, RuntimeInitializationWithoutPointer)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "This suite only exists to test the first branch of the runtime initialization. Since this is "
                   "static state we need a separate binary for this.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    Runtime::SetRecorder(nullptr, nullptr);
    // Do not add additional tests here, but in runtime_test.cpp.
    auto& recorder = Runtime::GetRecorder();

#if defined(KCONSOLE_LOGGING)
    EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
#else
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
#endif

    // GetRecorder() shall always return a valid reference to a recorder.
    // We enforce checking this by calling an arbitrary method on the reference.
    // Address sanitizer and valgrind would detect a memory error if the implementation is faulty.
    recorder.IsLogEnabled(LogLevel::kVerbose, "");
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
