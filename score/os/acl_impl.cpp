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
#include "score/os/acl_impl.h"

// Note 1
// Suppress "AUTOSAR C++14 M6-4-5" and "AUTOSAR C++14 M6-4-3", The rule states: An unconditional throw or break
// statement shall terminate every nonempty switch-clause." and "A switch statement shall be a well-formed
// switch statement.", respectively. This is false positive. The `return` statement in this case clause
// unconditionally exits the function, making an additional `break` statement redundant.

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected<Acl::AclCollection, score::os::Error> AclInstance::acl_get_fd(
    const Acl::FileDescriptor file_descriptor) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    auto* const acl = ::acl_get_fd(file_descriptor);
    if (acl == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return acl;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected<Acl::AclCollection, score::os::Error> AclInstance::acl_get_file(const std::string file_path) const noexcept
{
    auto* const acl = ::acl_get_file(file_path.c_str(), ACL_TYPE_ACCESS);
    if (acl == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return acl;
}
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<score::cpp::optional<Acl::Entry>, score::os::Error> AclInstance::acl_get_entry(
    const Acl::AclCollection collection,
    const Acl::EntryIndex index) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
{
    Acl::Entry entry{};
    /* It is not possible to cover one branch i.e. default */
    /* Hence, added suppression */
    // coverity[autosar_cpp14_m6_4_3_violation] see Note 1
    switch (::acl_get_entry(collection, index, &entry))  // LCOV_EXCL_BR_LINE
    {
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case 0:  // no entry found (anymore)
            return score::cpp::nullopt;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case 1:  // found next entry
            return entry;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case -1:  // error
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        /* KW_SUPPRESS_START:MISRA.SWITCH.NO_BREAK:Terminated with terminate function from std */
        // LCOV_EXCL_START
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        default:
            /* We have done manual code analysis for the lines that can be covered. */
            /* But there is a limitation for LCOV. Coverage is impossible to achieve, */
            /* since coverage buffers are not flushed correctly in death tests on QNX */
            std::terminate();
            // LCOV_EXCL_STOP
            /* KW_SUPPRESS_END:MISRA.SWITCH.NO_BREAK:Terminated with terminate function from std */
    }
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected<AclInstance::Tag, score::os::Error> AclInstance::acl_get_tag_type(const Acl::Entry entry) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    acl_tag_t tag{ACL_UNDEFINED_TAG};
    /* It is immposible to cover the return statement */
    /* Hence, added suppression */
    if (::acl_get_tag_type(entry, &tag) == 0)  // LCOV_EXCL_BR_LINE
    {
        return ConvertFromPlatformDependentTag(tag);
    }
    else
    {
        /* When passing an invalid entry as a parameter to acl_get_tag_type, the test is getting */
        /* aborted from the os method i.e. acl_get_tag_type and the below line is skipped. Thus coverage */
        /* is impossible to achieve for the below line on QNX*/
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected<void*, score::os::Error> AclInstance::acl_get_qualifier(const Acl::Entry entry) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    void* const qualifier = ::acl_get_qualifier(entry);
    if (qualifier != nullptr)
    {
        return qualifier;
    }
    else
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<score::os::Error> AclInstance::acl_create_entry(Acl::AclCollection* const acl,
                                                                  Acl::Entry* const entry) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::acl_create_entry(acl, entry) != 0)  // LCOV_EXCL_BR_LINE
    {
        /* When passing an invalid acl as a parameter to acl_create_entry, the test is getting */
        /* aborted from the os method i.e. acl_create_entry and the below line is skipped. Thus coverage */
        /* is impossible to achieve for the below line on QNX */
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<score::os::Error> AclInstance::acl_set_tag_type(const Acl::Entry entry,
                                                                  const Acl::Tag tag) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::acl_set_tag_type(entry, ConvertToPlatformDependentTag(tag)) != 0)  // LCOV_EXCL_BR_LINE
    {
        /* When passing an invalid tag as a paramter to acl_set_tag_type, the test is getting */
        /* aborted from the os call method i.e. acl_set_tag_type and the below line is skipped. Thus coverage */
        /* is impossible to achieve for the below line on QNX */
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<score::os::Error> AclInstance::acl_set_qualifier(const Acl::Entry entry,
                                                                   const void* const qualifier) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::acl_set_qualifier(entry, qualifier) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected<bool, score::os::Error> AclInstance::acl_get_perm(const Acl::Permissions permission_set,
                                                              const Acl::Permission permission) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
// coverity[autosar_cpp14_a16_0_1_violation] Different implementation required for linux and QNX
#if defined(__linux__)
    const auto result = ::acl_get_perm(permission_set, ConvertToPlatformDependentPermission(permission));
// coverity[autosar_cpp14_a16_0_1_violation] Different implementation required for linux and QNX
#elif defined(__QNX__)
    const auto result = ::acl_get_perm_np(permission_set, ConvertToPlatformDependentPermission(permission));
// coverity[autosar_cpp14_a16_0_1_violation] Different implementation required for linux and QNX
#else
    score::cpp::make_unexpected(score::os::Error::Code::kUnexpected);
// coverity[autosar_cpp14_a16_0_1_violation] Different implementation required for linux and QNX
#endif
    if (result == -1)  // LCOV_EXCL_BR_LINE: Justification provided below
    {
        // Manual analysis:
        // As per QNX documentation, the function returns error when the permission argument isn't a valid permission
        // within an ACL entry. It is not possible to pass invalid permission value as the enum class Acl::Permission
        // does not contain an invalid value. Thus coverage is impossible to achieve for the below line.
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE Justification provided above
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
void AclInstance::acl_get_permset(const Acl::Entry entry, Acl::Permissions* const permissions) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    score::cpp::ignore = ::acl_get_permset(entry, permissions);
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
void AclInstance::acl_clear_perms(const Acl::Permissions permissions) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    score::cpp::ignore = ::acl_clear_perms(permissions);
}
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<score::os::Error> AclInstance::acl_add_perm(const Acl::Permissions permissions,
                                                              const Acl::Permission permission) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::acl_add_perm(permissions, ConvertToPlatformDependentPermission(permission)) != 0)  // LCOV_EXCL_BR_LINE
    {
        /* When passing an invalid permission as a parameter to acl_add_perm, the test is getting */
        /* aborted from the os call method i.e. acl_add_perm and the below line is skipped. Thus coverage */
        /* is impossible to achieve for the below line on QNX */
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<score::os::Error> AclInstance::acl_calc_mask(Acl::AclCollection* const acl) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::acl_calc_mask(acl) != 0)  // LCOV_EXCL_BR_LINE
    {
        /* When passing an invalid acl parameter to acl_mask_cal_mask, the test is getting */
        /* aborted from the os call method i.e. acl_cal_mask and the below line is skipped. Thus coverage */
        /* is impossible to achieve for the below line on QNX */
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<score::os::Error> AclInstance::acl_valid(const Acl::AclCollection acl) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::acl_valid(acl) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<score::os::Error> AclInstance::acl_set_fd(const Acl::FileDescriptor file_descriptor,
                                                            const Acl::AclCollection acl) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::acl_set_fd(file_descriptor, acl) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
void AclInstance::acl_free(void* const object) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    score::cpp::ignore = ::acl_free(object);
}

acl_tag_t AclInstance::ConvertToPlatformDependentTag(const ::score::os::Acl::Tag tag) const noexcept
{
    /* It is not possible to cover one branch i.e. default */
    /* Hence, added suppression */
    // coverity[autosar_cpp14_m6_4_3_violation] see Note 1
    switch (tag)  // LCOV_EXCL_BR_LINE
    {
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Tag::kGroup:
            return ACL_GROUP; /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Tag::kOwningGroup:
            return ACL_GROUP_OBJ; /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Tag::kMaximumAllowedPermissions:
            return ACL_MASK; /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Tag::kOther:
            return ACL_OTHER; /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Tag::kUser:
            return ACL_USER; /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Tag::kOwningUser:
            return ACL_USER_OBJ; /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
        /* KW_SUPPRESS_START:MISRA.SWITCH.NO_BREAK:Terminated with terminate function from std */
        // LCOV_EXCL_START
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        default:
            /* We have done manual code analysis for the lines that can be covered. */
            /* But there is a limitation for LCOV. Coverage is impossible to achieve, */
            /* since coverage buffers are not flushed correctly in death tests on QNX */
            std::terminate();
            // LCOV_EXCL_STOP
            /* KW_SUPPRESS_END:MISRA.SWITCH.NO_BREAK:Terminated with terminate function from std */
    }
}

::score::os::Acl::Tag AclInstance::ConvertFromPlatformDependentTag(const acl_tag_t tag) const noexcept
{
    /* It is not possible to cover one branch i.e. default */
    /* Hence, added suppression */
    // coverity[autosar_cpp14_m6_4_3_violation] see Note 1
    switch (tag)  // LCOV_EXCL_BR_LINE
    {
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ACL_GROUP: /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
            return Tag::kGroup;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ACL_GROUP_OBJ: /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
            return Tag::kOwningGroup;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ACL_MASK: /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
            return Tag::kMaximumAllowedPermissions;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ACL_OTHER: /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
            return Tag::kOther;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ACL_USER: /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
            return Tag::kUser;
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case ACL_USER_OBJ: /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
            return Tag::kOwningUser;
        /* KW_SUPPRESS_START:MISRA.SWITCH.NO_BREAK:Terminated with terminate function from std */
        // LCOV_EXCL_START
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        default:
            /* We have done manual code analysis for the lines that can be covered. */
            /* But there is a limitation for LCOV. Coverage is impossible to achieve, */
            /* since coverage buffers are not flushed correctly in death tests on QNX */
            std::terminate();
            // LCOV_EXCL_STOP
            /* KW_SUPPRESS_END:MISRA.SWITCH.NO_BREAK:Terminated with terminate function from std */
    }
}

acl_perm_t AclInstance::ConvertToPlatformDependentPermission(const ::score::os::Acl::Permission permission) const noexcept
{
    /* It is not possible to cover one branch i.e. default */
    /* Hence, added suppression */
    // coverity[autosar_cpp14_m6_4_3_violation] see Note 1
    switch (permission)  // LCOV_EXCL_BR_LINE
    {
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Permission::kExecute:
            return static_cast<acl_perm_t>(ACL_EXECUTE); /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Permission::kRead:
            return static_cast<acl_perm_t>(ACL_READ); /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        case Permission::kWrite:
            return static_cast<acl_perm_t>(ACL_WRITE); /* KW_SUPPRESS:MISRA.USE.EXPANSION:OS library macros */
        /* KW_SUPPRESS_START:MISRA.SWITCH.NO_BREAK:Terminated with terminate function from std */
        // LCOV_EXCL_START
        // coverity[autosar_cpp14_m6_4_5_violation] see Note 1
        default:
            /* We have done manual code analysis for the lines that can be covered. */
            /* But there is a limitation for LCOV. Coverage is impossible to achieve, */
            /* since coverage buffers are not flushed correctly in death tests on QNX */
            std::terminate();
            // LCOV_EXCL_STOP
            /* KW_SUPPRESS_END:MISRA.SWITCH.NO_BREAK:Terminated with terminate function from std */
    }
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<char*, score::os::Error> AclInstance::acl_to_text(const Acl::AclCollection& acl,
                                                              ssize_t* const len_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if ((acl == nullptr) || (len_p == nullptr))
    {
        errno = EINVAL;
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }

    auto* const acl_text = ::acl_to_text(acl, len_p);
    if (acl_text == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }

    return acl_text;
}

}  // namespace os
}  // namespace score
