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
#ifndef SCORE_LIB_OS_UTILS_ACL_ACCESS_CONTROL_LIST_H
#define SCORE_LIB_OS_UTILS_ACL_ACCESS_CONTROL_LIST_H

#include "score/os/utils/acl/i_access_control_list.h"

#include <score/optional.hpp>
#include <vector>

namespace score
{
namespace os
{

class AccessControlList : public IAccessControlList
{
  public:
    explicit AccessControlList(const score::os::Acl::FileDescriptor);
    explicit AccessControlList(const std::string&);
    ~AccessControlList() override;

    AccessControlList(AccessControlList&&) noexcept = delete;
    AccessControlList& operator=(AccessControlList&&) noexcept = delete;
    AccessControlList(const AccessControlList&) = delete;
    AccessControlList& operator=(const AccessControlList&) = delete;

    score::cpp::expected_blank<score::os::Error> AllowUser(const UserIdentifier, const Acl::Permission) override;
    score::cpp::expected<bool, score::os::Error> VerifyMaskPermissions(
        const std::vector<::score::os::Acl::Permission>&) const override;
    score::cpp::expected<std::vector<UserIdentifier>, score::os::Error> FindUserIdsWithPermission(
        const Acl::Permission permission) const noexcept override;

  private:
    AccessControlList(const score::os::Acl::FileDescriptor, const std::string&);
    void InitializeAcl(const score::cpp::expected<::score::os::Acl::AclCollection, ::score::os::Error> result);
    static score::cpp::expected_blank<score::os::Error> SetUser(const UserIdentifier, ::score::os::Acl::Entry&);
    static score::cpp::expected_blank<score::os::Error> AddPermission(const Acl::Permission, ::score::os::Acl::Entry&);
    template <typename F>
    /// Finds the first entry that matches a given predicate callable
    /// \param predicate Callable that will inspect an entry and return whether or not a match was found.
    ///                  The expected signature of the callable is
    ///                  score::cpp::expected<bool, score::os::Error>(score::os::Acl::Entry)
    /// \return An entry if a matching one was found, score::cpp::nullopt otherwise, or an error if, during entry inspection,
    ///         an error was encountered.
    score::cpp::expected<score::cpp::optional<Acl::Entry>, Error> FindFirstEntry(F&& predicate) const noexcept;

    bool CheckMaskPermissions(const std::string& acl_text,
                              ssize_t len,
                              const std::vector<::score::os::Acl::Permission>& permissions) const;

    bool ArePermissionsValid(const std::string& acl_text,
                             std::size_t mask_pos,
                             const std::vector<::score::os::Acl::Permission>& permissions) const;

    ::score::os::Acl::AclCollection acl_;
    score::cpp::optional<::score::os::Error> error_;
    ::score::os::Acl::FileDescriptor file_descriptor_;
    std::string file_path_;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_ACL_ACCESS_CONTROL_LIST_H
