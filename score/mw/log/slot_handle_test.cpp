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
#include "score/mw/log/slot_handle.h"

#include "gtest/gtest.h"

namespace score
{
namespace mw
{
namespace log
{
namespace
{

const SlotHandle::RecorderIdentifier kInvalidRecorder{255};
const SlotHandle::RecorderIdentifier kInvalidRecorderBoundary{SlotHandle::kMaxRecorders};
const SlotIndex kSlotValue{3};
const SlotHandle::RecorderIdentifier kRecorderValue{1};

void ExpectAllSlotsUnassigned(const SlotHandle& handle)
{
    for (SlotIndex i = 0; i < SlotHandle::kMaxRecorders; ++i)
    {
        EXPECT_EQ(handle.GetSlot(SlotHandle::RecorderIdentifier{i}), SlotIndex{});
        EXPECT_EQ(handle.IsRecorderActive(SlotHandle::RecorderIdentifier{i}), false);
    }
}

TEST(SlotHandle, SlotHandleShallDefaultInitializeToZero)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the default initialization of SlotHandle instance.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{};
    EXPECT_EQ(handle.GetSelectedRecorder(), SlotHandle::RecorderIdentifier{0});
    EXPECT_EQ(handle.GetSlotOfSelectedRecorder(), SlotIndex{0});
    ExpectAllSlotsUnassigned(handle);
}

TEST(SlotHandle, GetSlotOfSelectedRecorderShallReturnCorrectSlot)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check Set/Get slot method.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{};
    handle.SetSlot(kSlotValue, kRecorderValue);
    handle.SetSelectedRecorder(kRecorderValue);
    EXPECT_EQ(handle.GetSlotOfSelectedRecorder(), kSlotValue);
}

TEST(SlotHandle, GetSlotShallReturnCorrectValue)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check getting the slot whose Set via constructor.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{kSlotValue};
    EXPECT_EQ(handle.GetSlot(SlotHandle::RecorderIdentifier{0}), kSlotValue);
}

TEST(SlotHandle, GetSlotShallReturnZeroOnIncorrectValue)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Check the in-ability of getting the slot in case of invalid recorder identifier value.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{kSlotValue};
    EXPECT_EQ(handle.GetSlot(kInvalidRecorder), 0);
}

TEST(SlotHandle, GetSlotShallReturnZeroOnIncorrectValueCornerCase)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Check the in-ability of getting the slot in case of invalid recorder identifier value.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{kSlotValue};
    EXPECT_EQ(handle.GetSlot(kInvalidRecorderBoundary), 0);
}

TEST(SlotHandle, SetSlotShallSetCorrectValue)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the ability of seting slot properly.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{kSlotValue};
    handle.SetSlot(kSlotValue, kRecorderValue);
    EXPECT_EQ(handle.GetSlot(kRecorderValue), kSlotValue);
}

TEST(SlotHandle, SetSlotShallDiscardInvalidRecorderCornerCase)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the in-ability of setting invalid recorder identifier value.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{};
    handle.SetSlot(kSlotValue, kInvalidRecorderBoundary);
    ExpectAllSlotsUnassigned(handle);
}

TEST(SlotHandle, SetSlotShallDiscardInvalidRecorder)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the in-ability of setting invalid recorder identifier value.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{};
    handle.SetSlot(kSlotValue, kInvalidRecorder);
    ExpectAllSlotsUnassigned(handle);
}

TEST(SlotHandle, SetSelectedRecorderShallReturnCorrectValue)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of setting selected recorder.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{};
    handle.SetSelectedRecorder(kRecorderValue);
    EXPECT_EQ(handle.GetSelectedRecorder(), kRecorderValue);
}

TEST(SlotHandle, SetSelectedRecorderShallIgnoreInvalidValue)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of SetSelectedRecorder to ignore invalid recorder.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{};
    handle.SetSelectedRecorder(kRecorderValue);
    handle.SetSelectedRecorder(kInvalidRecorder);
    EXPECT_EQ(handle.GetSelectedRecorder(), kRecorderValue);
}

TEST(SlotHandle, SetSelectedRecorderShallIgnoreInvalidValueCornerCase)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of SetSelectedRecorder to ignore invalid recorder.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{};
    handle.SetSelectedRecorder(kRecorderValue);
    handle.SetSelectedRecorder(kInvalidRecorderBoundary);
    EXPECT_EQ(handle.GetSelectedRecorder(), kRecorderValue);
}

TEST(SlotHandle, GetSlotAvailableShallReturnTrueOnAssigned)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of checking the availability of specific slot.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{kSlotValue};
    EXPECT_EQ(handle.IsRecorderActive(SlotHandle::RecorderIdentifier{}), true);
}

TEST(SlotHandle, GetSlotAvailableShallReturnFalseOnInvalidRecorder)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of checking the availability of specific slot.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{};
    EXPECT_EQ(handle.IsRecorderActive(kInvalidRecorder), false);
}

TEST(SlotHandle, GetSlotAvailableShallReturnFalseOnInvalidRecorderCornerCase)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of checking the availability of specific slot.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle handle{};
    EXPECT_EQ(handle.IsRecorderActive(kInvalidRecorderBoundary), false);
}

TEST(SlotHandle, ShallBeEqualIffSelectedRecorderAndSlotsAreEqual)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the equality of SlotHandle instances.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle rhs{};
    SlotHandle lhs{};
    for (SlotIndex i = 0; i < SlotHandle::kMaxRecorders; i++)
    {
        rhs.SetSlot(i, SlotHandle::RecorderIdentifier{i});
        lhs.SetSlot(i, SlotHandle::RecorderIdentifier{i});
    }

    rhs.SetSelectedRecorder(kRecorderValue);
    lhs.SetSelectedRecorder(kRecorderValue);

    EXPECT_EQ(lhs, rhs);
}

TEST(SlotHandle, ShallBeUnequalIfSelectedRecorderUnequal)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the inequality of SlotHandle instances.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle rhs{};
    SlotHandle lhs{};

    lhs.SetSelectedRecorder(kRecorderValue);

    EXPECT_NE(lhs, rhs);
}

TEST(SlotHandle, ShallBeUnequalIfAnySlotUnequal)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the inequality of SlotHandle instances.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle rhs{};
    SlotHandle lhs{};

    lhs.SetSlot(kSlotValue, kRecorderValue);

    EXPECT_NE(lhs, rhs);
}

TEST(SlotHandle, ShallBeUnequalIfAnySlotUnequalAssigned)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the inequality of SlotHandle instances.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::mw::log::SlotHandle");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SlotHandle rhs{};
    SlotHandle lhs{};

    lhs.SetSlot(kSlotValue, {});

    EXPECT_NE(lhs, rhs);
}

}  // namespace
}  // namespace log
}  // namespace mw
}  // namespace score
