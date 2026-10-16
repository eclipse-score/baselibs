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
#ifndef SCORE_LIB_OS_UTILS_ACL_I_ACCESS_CONTROL_LIST_H
#define SCORE_LIB_OS_UTILS_ACL_I_ACCESS_CONTROL_LIST_H

#include "score/os/acl.h"
#include "score/os/errno.h"
#include "score/os/user_id.h"

#include "score/expected.hpp"

#include <vector>

namespace score
{
namespace os
{

/// \brief AccessControlList extends the permissions functionality of an POSIX system, be enabling a more fine grained
/// definition of access rights. This class comes with a higher-level of abstraction for the direct POSIX calls.
class IAccessControlList
{
  public:
    using UserIdentifier = UserId;

    /// \brief Assigns the given permission towards the given user. This can be invoked multiple times for any number of
    /// user and permission combinations.
    /// \return blank if no error occurred, the error otherwise
    virtual score::cpp::expected_blank<score::os::Error> AllowUser(const UserIdentifier, const Acl::Permission) = 0;
    /// \brief Verify whether current mask on the ACL matches given permissions. I.e. for every permission in the list
    ///        the corresponding entry in the mask must not be "-"
    virtual score::cpp::expected<bool, score::os::Error> VerifyMaskPermissions(
        const std::vector<::score::os::Acl::Permission>&) const = 0;

    /// \brief Retrieves the list of user IDs with the specified permission.
    virtual score::cpp::expected<std::vector<UserIdentifier>, score::os::Error> FindUserIdsWithPermission(
        const Acl::Permission) const noexcept = 0;

    virtual ~IAccessControlList() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    IAccessControlList(const IAccessControlList&) = delete;
    IAccessControlList& operator=(const IAccessControlList&) = delete;
    IAccessControlList(IAccessControlList&& other) = delete;
    IAccessControlList& operator=(IAccessControlList&& other) = delete;

  protected:
    IAccessControlList() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_ACL_I_ACCESS_CONTROL_LIST_H
