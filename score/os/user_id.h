/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_OS_USER_ID_H
#define SCORE_LIB_OS_USER_ID_H

#include <sys/types.h>

namespace score
{
namespace os
{

namespace details
{

/// \brief Strong type for a POSIX identity (uid_t / gid_t).
/// \details POSIX leaves width and signedness of uid_t and gid_t unspecified (e.g. unsigned on Linux, signed on QNX).
/// Storing the native value keeps the conversion lossless, while the restricted interface only allows comparisons
/// between identities of the same kind. This rules out implicit integral conversions and mixed-signedness comparisons.
template <typename NativeType, typename Tag>
class PosixId final
{
  public:
    using native_type = NativeType;

    constexpr PosixId() noexcept = default;
    constexpr explicit PosixId(const NativeType native_value) noexcept : value_{native_value} {}

    /// \brief Native value, intended for passing the identity to POSIX APIs.
    constexpr NativeType native() const noexcept
    {
        return value_;
    }

    friend constexpr bool operator==(const PosixId lhs, const PosixId rhs) noexcept
    {
        return lhs.value_ == rhs.value_;
    }

    friend constexpr bool operator!=(const PosixId lhs, const PosixId rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const PosixId lhs, const PosixId rhs) noexcept
    {
        return lhs.value_ < rhs.value_;
    }

  private:
    NativeType value_{};
};

struct UserIdTag
{
};
struct GroupIdTag
{
};

}  // namespace details

using UserId = details::PosixId<uid_t, details::UserIdTag>;
using GroupId = details::PosixId<gid_t, details::GroupIdTag>;

// Suppress "AUTOSAR C++14 M5-0-4" and "M5-19-1" rule findings. POSIX defines (uid_t)-1 / (gid_t)-1 as "do not change"
// for chown(); the conversion is well-defined for both signed and unsigned native types.
// coverity[autosar_cpp14_m5_0_4_violation]
// coverity[autosar_cpp14_m5_19_1_violation]
inline constexpr UserId kUnchangedUserId{static_cast<uid_t>(-1)};
// coverity[autosar_cpp14_m5_0_4_violation]
// coverity[autosar_cpp14_m5_19_1_violation]
inline constexpr GroupId kUnchangedGroupId{static_cast<gid_t>(-1)};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_USER_ID_H
