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
#include "score/os/utils/high_resolution_steady_clock.h"

#include "gtest/gtest.h"

namespace score
{
namespace os
{
namespace
{

TEST(HighResolutionSteadyClock, UnderlyingClock)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    RecordProperty("Description",
                   "This test ensures that the underlying clock of "
                   "HighResolutionSteadyClock is set appropriately per OS.");

    // Given an expected underlying clock depending on operating system
#ifdef __QNX__
    using ExpectedUnderlyingClock = std::chrono::high_resolution_clock;
#else
    using ExpectedUnderlyingClock = std::chrono::steady_clock;
#endif

    // Then HighResolutionSteadyClock's underlying clock must match that one
    constexpr bool underlying_clock_is_set_as_expected =
        std::is_same<HighResolutionSteadyClock::UnderlyingClock, ExpectedUnderlyingClock>::value;
    EXPECT_TRUE(underlying_clock_is_set_as_expected);
}

template <typename Timepoint>
constexpr auto NanosecondsOf(const Timepoint timepoint) noexcept
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(timepoint.time_since_epoch()).count();
}

TEST(HighResolutionSteadyClock, now)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    RecordProperty("Description",
                   "This test ensures that the values returned by HighResolutionSteadyClock's "
                   "method now() are consistent with the ones returned by the underlying clock.");

    // Given an expected underlying clock depending on operating system
#ifdef __QNX__
    using ExpectedUnderlyingClock = std::chrono::high_resolution_clock;
#else
    using ExpectedUnderlyingClock = std::chrono::steady_clock;
#endif

    // When reading values via both clock's method now()
    const auto time_before = ExpectedUnderlyingClock::now();
    const auto now_value_1 = HighResolutionSteadyClock::now();
    const auto time_middle = ExpectedUnderlyingClock::now();
    const auto now_value_2 = HighResolutionSteadyClock::now();
    const auto time_after = ExpectedUnderlyingClock::now();

    // Then HighResolutionSteadyClock's reported values must be within the ones read via ExpectedUnderlyingClock
    ASSERT_LE(NanosecondsOf(time_before), NanosecondsOf(now_value_1));
    ASSERT_LE(NanosecondsOf(now_value_1), NanosecondsOf(time_middle));
    ASSERT_LE(NanosecondsOf(time_middle), NanosecondsOf(now_value_2));
    ASSERT_LE(NanosecondsOf(now_value_2), NanosecondsOf(time_after));
}

}  // namespace
}  // namespace os
}  // namespace score
