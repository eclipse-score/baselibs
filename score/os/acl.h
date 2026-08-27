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
#ifndef SCORE_LIB_OS_ACL_H
#define SCORE_LIB_OS_ACL_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include <score/expected.hpp>
#include <score/optional.hpp>

#include <sys/acl.h>

#if defined(__linux__)
#include <acl/libacl.h>
#include <sys/types.h>
#endif

namespace score
{
namespace os
{

/**
 * \brief This class abstracts the access to the low-level OS functions of Access Control Lists
 *
 * \details It implements an abstraction towards the IEEE 1003.1e draft 17 (“POSIX.1e”) part.
 * It is supported by:
 * - Linux: https://man7.org/linux/man-pages/man5/acl.5.html
 * - QNX: https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/acl.html
 *
 * Be aware that upon usage, you might need to first check if your underlying file-system supports this.
 * For Linux ext3/4 support ACL extensions, for QNX QNX6FS and ramfs also support it.
 *
 * Be ware, ACL is not the same as _normal_ mode handling (e.g. via chmod). It is an extension to this concept.
 *
 * Notice, that a higher level abstraction is provided in score/os/utils/acl/acl.h
 * You should use the high-level abstraction in 99% of the cases.
 */
class Acl : public ObjectSeam<Acl>
{
  public:
    static Acl& instance() noexcept;

    using FileDescriptor = std::int32_t;
    using AclCollection = acl_t;
    using Entry = acl_entry_t;
    using Permissions = acl_permset_t;
    using EntryIndex = std::int32_t;

    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:OS library macros*/
    static constexpr EntryIndex kAclFirstEntry{ACL_FIRST_ENTRY};
    static constexpr EntryIndex kAclNextEntry{ACL_NEXT_ENTRY};
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:OS library macros*/

    enum class Tag : std::int32_t
    {
        kGroup,
        kOwningGroup,
        kMaximumAllowedPermissions,
        kOther,
        kUser,
        kOwningUser,
    };

    enum class Permission : std::int32_t
    {
        kExecute,
        kRead,
        kWrite,
    };

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<AclCollection, score::os::Error> acl_get_fd(const FileDescriptor) const noexcept = 0;
    virtual score::cpp::expected<AclCollection, score::os::Error> acl_get_file(const std::string) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> acl_create_entry(AclCollection* const,
                                                                          Entry* const) const noexcept = 0;
    virtual score::cpp::expected<score::cpp::optional<Entry>, score::os::Error> acl_get_entry(
        const AclCollection,
        const EntryIndex) const noexcept = 0;
    virtual score::cpp::expected<Tag, score::os::Error> acl_get_tag_type(const Entry) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> acl_set_tag_type(const Entry, const Tag) const noexcept = 0;
    virtual score::cpp::expected<void*, score::os::Error> acl_get_qualifier(const Entry) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> acl_set_qualifier(const Entry,
                                                                           const void* const) const noexcept = 0;
    virtual score::cpp::expected<bool, score::os::Error> acl_get_perm(const Permissions,
                                                                      const Permission) const noexcept = 0;
    virtual void acl_get_permset(const Entry, Permissions* const) const noexcept = 0;
    virtual void acl_clear_perms(const Permissions) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> acl_add_perm(const Permissions,
                                                                      const Permission) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> acl_calc_mask(AclCollection* const) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> acl_valid(const AclCollection) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> acl_set_fd(const FileDescriptor,
                                                                    const AclCollection) const noexcept = 0;
    virtual void acl_free(void* const) const noexcept = 0;
    virtual score::cpp::expected<char*, score::os::Error> acl_to_text(const AclCollection& acl,
                                                                      ssize_t* const len_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~Acl() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Acl(const Acl&) = delete;
    Acl& operator=(const Acl&) = delete;
    Acl(Acl&& other) = delete;
    Acl& operator=(Acl&& other) = delete;

  protected:
    Acl() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_ACL_H
