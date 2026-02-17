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

namespace score
{
namespace mw
{
namespace log
{

namespace
{
bool IsRecorderValid(const SlotHandle::RecorderIdentifier recorder) noexcept
{
    return recorder.value < SlotHandle::kMaxRecorders;
}
}  // namespace

SlotHandle::SlotHandle(const SlotIndex slot) noexcept
{
    SetSlot(slot, RecorderIdentifier{});
}

SlotIndex SlotHandle::GetSlotOfSelectedRecorder() const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) bound already checked
    return recorder_to_slot_[selected_recorder_.value];
}

SlotIndex SlotHandle::GetSlot(const RecorderIdentifier recorder) const noexcept
{
    if (!IsRecorderValid(recorder))
    {
        return {};
    }
    /*
    Deviation from Rule A5-2-5:
    - An array or container shall not be accessed beyond its range
    Justification:
    - boundary check in function IsRecorderValid.
    */
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index) bound already checked
    // coverity[autosar_cpp14_a5_2_5_violation]
    return recorder_to_slot_[recorder.value];
    // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
}

void SlotHandle::SetSlot(const SlotIndex slot, const RecorderIdentifier recorder) noexcept
{
    if (!IsRecorderValid(recorder))
    {
        return;
    }

    /*
    Deviation from Rule A5-2-5:
    - An array or container shall not be accessed beyond its range
    Justification:
    - boundary check in function IsRecorderValid.
    */
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index) bound already checked
    // coverity[autosar_cpp14_a5_2_5_violation]
    recorder_slot_available_[recorder.value] = true;
    // coverity[autosar_cpp14_a5_2_5_violation]
    recorder_to_slot_[recorder.value] = slot;
    // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
}

SlotHandle::RecorderIdentifier SlotHandle::GetSelectedRecorder() const noexcept
{
    return selected_recorder_;
}

void SlotHandle::SetSelectedRecorder(const RecorderIdentifier recorder) noexcept
{
    if (!IsRecorderValid(recorder))
    {
        return;
    }

    selected_recorder_ = recorder;
}

bool SlotHandle::IsRecorderActive(const RecorderIdentifier recorder) const noexcept
{
    if (!IsRecorderValid(recorder))
    {
        return false;
    }

    /*
    Deviation from Rule A5-2-5:
    - An array or container shall not be accessed beyond its range
    Justification:
    - boundary check in function IsRecorderValid.
    */
    // coverity[autosar_cpp14_a5_2_5_violation]
    return recorder_slot_available_[recorder.value];
}

bool operator==(const SlotHandle& l_value, const SlotHandle& r_value) noexcept
{
    return (((l_value.selected_recorder_ == r_value.selected_recorder_) &&
             (std::equal(l_value.recorder_to_slot_.begin(),
                         l_value.recorder_to_slot_.end(),
                         r_value.recorder_to_slot_.begin()))) == true);
}

bool operator!=(const SlotHandle& l_value, const SlotHandle& r_value) noexcept
{
    return !(l_value == r_value);
}

bool operator==(const SlotHandle::RecorderIdentifier& l_value, const SlotHandle::RecorderIdentifier& r_value) noexcept
{
    return l_value.value == r_value.value;
}

}  // namespace log
}  // namespace mw
}  // namespace score
