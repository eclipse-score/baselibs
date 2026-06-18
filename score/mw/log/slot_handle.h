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
#ifndef SCORE_MW_LOG_SLOT_HANDLE_H
#define SCORE_MW_LOG_SLOT_HANDLE_H

// Be careful what you include here. Each additional header will be included in logging.h and thus exposed to the user.
// We need to try to keep the includes low to reduce the compile footprint of using this library.
#include <array>
#include <bitset>
#include <cstdint>

namespace score
{
namespace mw
{
namespace log
{

using SlotIndex = std::uint16_t;

/// \brief Represents an identifier that is owned by a `LogStream` to identify the correct memory slot where the
/// streamed message shall be written.
///
/// \details We do not use a direct pointer to the memory, since this would expose implementation details. Further, the
/// user shall not be able to directly manipulate the memory, since the format of the memory must be well defined. For
/// more information visit the concept of an `Formatter`.
///
/// The recorder identifier exists to enable multiple active recorders at the same time. For more details see also the
/// CompositeRecorder class.
class SlotHandle final
{
  public:
    /// \brief Value to identify the destination recorder.
    struct RecorderIdentifier
    {
        // Member data in non-POD class types shall be private.
        // Justification:
        // - RecorderIdentifier is POD type and the design is on purpose
        // coverity[autosar_cpp14_m11_0_1_violation]
        std::size_t value{};
    };

    SlotHandle() noexcept = default;
    explicit SlotHandle(const SlotIndex) noexcept;

    /// \brief Returns the slot identifier corresponding to the currently selected recorder.
    SlotIndex GetSlotOfSelectedRecorder() const noexcept;

    /// \brief Gets the slot of the corresponding recorder identifier.
    /// \details On invalid recorder returns zero.
    SlotIndex GetSlot(const RecorderIdentifier) const noexcept;

    /// \brief Sets the slot of the according recorder identifier.
    /// \details Iff recorder value is invalid, the call will be discarded.
    void SetSlot(const SlotIndex, const RecorderIdentifier recorder = {{}}) noexcept;

    /// \brief Get the currently selected recorder.
    RecorderIdentifier GetSelectedRecorder() const noexcept;

    /// \brief Set the selected recorder.
    /// \details Invalid recorder value will be ignored.
    void SetSelectedRecorder(const RecorderIdentifier) noexcept;

    /// \brief Returns true if a slot in the corresponding recorder was available and reserved.
    bool IsRecorderActive(const RecorderIdentifier) const noexcept;

    friend bool operator==(const SlotHandle& l_value, const SlotHandle& r_value) noexcept;
    friend bool operator!=(const SlotHandle& l_value, const SlotHandle& r_value) noexcept;

    /// \brief Maximum number of recorders supported at the same time.
    // used in recorder_to_slot_, recorder_slot_available_ declaration.
    // coverity[autosar_cpp14_a0_1_1_violation : FALSE]
    static constexpr std::size_t kMaxRecorders{4U};

  private:
    std::array<SlotIndex, kMaxRecorders> recorder_to_slot_{};
    std::bitset<kMaxRecorders> recorder_slot_available_{};
    RecorderIdentifier selected_recorder_{};
};

bool operator==(const SlotHandle::RecorderIdentifier& l_value, const SlotHandle::RecorderIdentifier& r_value) noexcept;

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_SLOT_HANDLE_H
