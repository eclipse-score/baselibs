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
#ifndef SCORE_MW_LOG_DETAIL_LOGGING_IDENTIFIER_H
#define SCORE_MW_LOG_DETAIL_LOGGING_IDENTIFIER_H

#include <string_view>

#include "visitor/visit_as_struct.h"

#include <array>
#include <functional>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief Contains a 4 byte identifier that can be used for ECU, application or context identifiers.
class LoggingIdentifier
{
  public:
    /// \brief Crops the given string view to a maximum of kMaxLength bytes.
    explicit LoggingIdentifier(const std::string_view) noexcept;

    /// \brief Returns the underlying fixed-length string view.
    std::string_view GetStringView() const noexcept;

    /// \brief Use this to enable LoggingIdentifier as a key in a map.
    class HashFunction
    {
      public:
        size_t operator()(const LoggingIdentifier& id) const;
    };

    friend bool operator==(const LoggingIdentifier&, const LoggingIdentifier&) noexcept;
    friend bool operator!=(const LoggingIdentifier&, const LoggingIdentifier&) noexcept;

    LoggingIdentifier() noexcept = default;
    LoggingIdentifier& operator=(const LoggingIdentifier& rhs) noexcept = default;
    LoggingIdentifier& operator=(LoggingIdentifier&& rhs) noexcept = default;
    LoggingIdentifier(const LoggingIdentifier&) noexcept = default;
    LoggingIdentifier(LoggingIdentifier&&) noexcept = default;
    ~LoggingIdentifier() noexcept = default;

    /// \brief This maximum is fixed to 4 bytes by the DLT protocol standard.
    /*
     Deviation from Rule A0-1-1:
     - A project shall not contain instances of non-volatile variables being given values that are not subsequently
     used.

     Justification:
     - This is a False Positive as it's already used.
     */
    // coverity[autosar_cpp14_a0_1_1_violation : FALSE]
    constexpr static std::size_t kMaxLength{4UL};

    // This variable is public because of necessity to pass it to STRUCT_VISITABLE in log_entry.h
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::array<std::string_view::value_type, kMaxLength> data_{};
};

// NOLINTBEGIN(score-struct-usage-compliance) justified by design
// Forward declaration for struct_visitable_impl is required for implementation
// std::forward<T>(s) added due to CB-#10171555
// coverity[autosar_cpp14_a18_9_4_violation]
// coverity[autosar_cpp14_m3_2_3_violation]
// coverity[autosar_cpp14_a7_1_2_violation]
// coverity[autosar_cpp14_a0_1_1_violation]
// coverity[autosar_cpp14_a2_10_4_violation]
STRUCT_VISITABLE(LoggingIdentifier, data_)

// NOLINTEND(score-struct-usage-compliance) justified by design

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_RUNTIME_H
