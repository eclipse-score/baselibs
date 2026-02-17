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
#include "score/os/utils/acl/access_control_list.h"
#include "score/utility.hpp"

#include <cstring>

namespace
{

score::cpp::expected<bool, ::score::os::Error> IsEntryForUser(
    const ::score::os::Acl::Entry entry,
    const ::score::os::AccessControlList::UserIdentifier user_identifier) noexcept
{

    auto& operating_system = ::score::os::Acl::instance();

    auto tag_result = operating_system.acl_get_tag_type(entry);
    if (tag_result.has_value())
    {
        if (*tag_result == ::score::os::Acl::Tag::kUser)
        {
            auto qualifier_result = operating_system.acl_get_qualifier(entry);
            if (qualifier_result.has_value())
            {
                // Suppress "AUTOSAR C++14 M5-2-8", The rule states: "An object with integer type or pointer to void
                // type shall not be converted to an object with pointer type."
                // Rationale: Cannot get rid of casting of
                // qualifier_result.value() as its needed to be generic to be used in different areas and no harm from
                // casting void pointer.
                // coverity[autosar_cpp14_m5_2_8_violation]
                auto* const entry_uid = static_cast<uid_t*>(qualifier_result.value());
                const bool is_same_uid = *entry_uid == user_identifier;
                operating_system.acl_free(entry_uid);
                return is_same_uid;
            }
            else
            {
                return score::cpp::make_unexpected(std::move(qualifier_result).error());
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return score::cpp::make_unexpected(std::move(tag_result).error());
    }
}

}  // namespace

::score::os::AccessControlList::AccessControlList(const score::os::Acl::FileDescriptor file_descriptor)
    : AccessControlList{file_descriptor, ""}
{
}

::score::os::AccessControlList::AccessControlList(const std::string& file_path)
    : AccessControlList{score::os::Acl::FileDescriptor{}, file_path}
{
}

::score::os::AccessControlList::AccessControlList(const score::os::Acl::FileDescriptor file_descriptor,
                                                const std::string& file_path)
    : IAccessControlList{}, acl_{}, file_descriptor_{file_descriptor}, file_path_{file_path}
{
    auto& operating_system = ::score::os::Acl::instance();
    if (!file_path_.empty())
    {
        InitializeAcl(operating_system.acl_get_file(file_path_));
    }
    else
    {
        InitializeAcl(operating_system.acl_get_fd(file_descriptor_));
    }
}

void ::score::os::AccessControlList::InitializeAcl(
    const score::cpp::expected<::score::os::Acl::AclCollection, ::score::os::Error> result)
{
    if (result.has_value())
    {
        acl_ = result.value(); /* LCOV_EXCL_BR_LINE: Branching is due to hidden exception handling */
    }
    else
    {
        /* Branching is due to hidden exception handling */
        score::cpp::ignore = error_.emplace(result.error()); /* LCOV_EXCL_BR_LINE */
    }
}

::score::os::AccessControlList::~AccessControlList()
{
    if (!error_.has_value())
    {
        ::score::os::Acl::instance().acl_free(acl_);
    }
}

auto ::score::os::AccessControlList::AllowUser(const UserIdentifier user_identifier, const Acl::Permission permission)
    -> score::cpp::expected_blank<score::os::Error>
{
    if (error_.has_value())
    {
        /* Branching is due to hidden exception handling */
        return score::cpp::make_unexpected(error_.value()); /* LCOV_EXCL_BR_LINE */
    }

    auto& operating_system = ::score::os::Acl::instance();
    auto find_entry_result = FindFirstEntry([user_identifier](const ::score::os::Acl::Entry entry_value) {
        /* Branching is due to hidden exception handling */
        return IsEntryForUser(entry_value, user_identifier);
    }); /* LCOV_EXCL_BR_LINE */

    ::score::os::Acl::Entry entry{};
    // If we didn't encounter an error during iteration...
    if (find_entry_result.has_value())
    {
        // ...and in fact found a matching entry...
        /* All the branches are covered for if statement. */
        /* This seems to be an issue with LCOV. */
        /* Hence, added suppression. */
        if (find_entry_result.value().has_value()) /* LCOV_EXCL_BR_LINE */
        {
            // ...we'll use it from this point forward
            /* Branching is due to hidden exception handling */
            entry = std::move(find_entry_result).value().value(); /* LCOV_EXCL_BR_LINE */
        }
        else
        {
            // ...otherwise we will create a new entry and set the user
            auto acl_result = operating_system.acl_create_entry(&acl_, &entry);
            if (!acl_result.has_value())
            {
                return acl_result;
            }
            /* Branching is due to hidden exception handling */
            acl_result = SetUser(user_identifier, entry); /* LCOV_EXCL_BR_LINE */
            if (!acl_result.has_value())
            {
                return acl_result;
            }
        }
    }
    else
    {
        /* Branching is due to hidden exception handling */
        return score::cpp::make_unexpected(std::move(find_entry_result).error()); /* LCOV_EXCL_BR_LINE */
    }

    /* Branching is due to hidden exception handling */
    auto result = AddPermission(permission, entry); /* LCOV_EXCL_BR_LINE */
    if (!result.has_value())
    {
        return result;
    }

    result = operating_system.acl_calc_mask(&acl_);
    if (!result.has_value())
    {
        return result;
    }

    result = operating_system.acl_valid(acl_);
    if (!result.has_value())
    {
        return result;
    }

    return operating_system.acl_set_fd(file_descriptor_, acl_);
}

auto ::score::os::AccessControlList::VerifyMaskPermissions(
    const std::vector<::score::os::Acl::Permission>& permissions) const -> score::cpp::expected<bool, score::os::Error>
{
    auto& operating_system = ::score::os::Acl::instance();
    ssize_t len{};
    auto acl_text_result = operating_system.acl_to_text(acl_, &len);
    if (!acl_text_result.has_value())
    {
        return score::cpp::make_unexpected(std::move(acl_text_result).error());
    }

    const std::string acl_text{acl_text_result.value()};
    bool mask_permissions_ok = CheckMaskPermissions(acl_text, len, permissions);
    operating_system.acl_free(acl_text_result.value());
    return mask_permissions_ok;
}

bool ::score::os::AccessControlList::CheckMaskPermissions(
    const std::string& acl_text,
    ssize_t len,
    const std::vector<::score::os::Acl::Permission>& permissions) const
{
    const std::string kMaskPrefix{"mask::"};
    const std::string kMaskBroken{"mask::---"};
    // 9 = 6 + 3
    constexpr std::uint8_t MASK_STR_LENGTH{9U};

    if (acl_text.find(kMaskBroken) != std::string::npos)
    {
        return false;
    }

    std::size_t mask_pos = acl_text.find(kMaskPrefix);
    if ((mask_pos == std::string::npos) || ((mask_pos + MASK_STR_LENGTH) > static_cast<std::size_t>(len)))
    {
        return false;
    }

    return ArePermissionsValid(acl_text, mask_pos, permissions);
}

bool ::score::os::AccessControlList::ArePermissionsValid(const std::string& acl_text,
                                                       std::size_t mask_pos,
                                                       const std::vector<::score::os::Acl::Permission>& permissions) const
{
    constexpr std::uint8_t kOffsetR{6U};
    constexpr std::uint8_t kOffsetW{7U};
    constexpr std::uint8_t kOffsetX{8U};

    for (const auto perm : permissions)
    {
        // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
        // not lead to data loss.".
        // Rationale: mask_pos + any offset cannot oveflow (checked above)
        // coverity[autosar_cpp14_a4_7_1_violation]
        if ((perm == Acl::Permission::kRead) && (acl_text.at(mask_pos + kOffsetR) == '-'))
        {
            return false;
        }
        // Rationale: see comment above
        // coverity[autosar_cpp14_a4_7_1_violation]
        if ((perm == Acl::Permission::kWrite) && (acl_text.at(mask_pos + kOffsetW) == '-'))
        {
            return false;
        }
        // Rationale: see comment above
        // coverity[autosar_cpp14_a4_7_1_violation]
        if ((perm == Acl::Permission::kExecute) && (acl_text.at(mask_pos + kOffsetX) == '-'))
        {
            return false;
        }
    }
    return true;
}

auto ::score::os::AccessControlList::SetUser(const UserIdentifier user_identifier, ::score::os::Acl::Entry& entry)
    -> score::cpp::expected_blank<score::os::Error>
{
    auto& operating_system = ::score::os::Acl::instance();
    const auto result = operating_system.acl_set_tag_type(entry, ::score::os::Acl::Tag::kUser);
    if (!result.has_value())
    {
        return result;
    }

    return operating_system.acl_set_qualifier(entry, &user_identifier);
}

auto ::score::os::AccessControlList::AddPermission(const Acl::Permission permission, ::score::os::Acl::Entry& entry)
    -> score::cpp::expected_blank<score::os::Error>
{
    auto& operating_system = ::score::os::Acl::instance();

    ::score::os::Acl::Permissions permissions{};
    operating_system.acl_get_permset(entry, &permissions);

    return operating_system.acl_add_perm(permissions, permission);
}

template <typename F>
auto ::score::os::AccessControlList::FindFirstEntry(F&& predicate) const noexcept
    -> score::cpp::expected<score::cpp::optional<Acl::Entry>, Error>
{
    auto& operating_system = Acl::instance();

    // We do not handle errors here since we assume that acl_ is valid since FindFirstEntry is called after checking
    // for errors during construction, which is the only reason why this call can fail with EINVAL besides giving a bad
    // element iteration value.
    score::cpp::optional<Acl::Entry> entry{operating_system.acl_get_entry(acl_, Acl::kAclFirstEntry).value()};
    while (entry.has_value())
    {
        // Suppress "AUTOSAR C++14 A18-9-2" rule findings. This rule stated: "Forwarding values to other functions
        // shall be done via: (1) std::move if the value is an rvalue reference,
        // (2) std::forward if the value is forwarding reference"
        // False positive: This is a function call.
        // coverity[autosar_cpp14_a18_9_2_violation]
        auto check_entry_result = predicate(entry.value());
        /* KW_SUPPRESS_START:MISRA.STMT.COND.NOT_BOOLEAN:False positive- has_value() will return a boolean */
        if (check_entry_result.has_value())
        /* KW_SUPPRESS_END:MISRA.STMT.COND.NOT_BOOLEAN:False positive- has_value() will return a boolean */
        {
            if (static_cast<bool>(check_entry_result.value()))
            {
                return entry;
            }
            else
            {
                // See above regarding error handling.
                entry = operating_system.acl_get_entry(acl_, Acl::kAclNextEntry).value();
            }
        }
        else
        {
            return score::cpp::make_unexpected(std::move(check_entry_result).error());
        }
    }

    return score::cpp::nullopt;
}

score::cpp::expected<std::vector<score::os::IAccessControlList::UserIdentifier>, score::os::Error>
score::os::AccessControlList::FindUserIdsWithPermission(const Acl::Permission permission) const noexcept
{
    if (error_.has_value())
    {
        /* Branching is due to hidden exception handling */
        return score::cpp::make_unexpected(error_.value()); /* LCOV_EXCL_BR_LINE */
    }

    const auto& operating_system = Acl::instance();
    std::vector<UserIdentifier> user_ids_with_perms{};

    auto entry = operating_system.acl_get_entry(acl_, Acl::kAclFirstEntry);
    if (!entry.has_value())
    {
        return score::cpp::make_unexpected(entry.error());
    }

    while (entry.value().has_value())
    {
        const auto acl_entry = entry.value().value();

        const auto tag_type = operating_system.acl_get_tag_type(acl_entry);
        if (!tag_type.has_value())
        {
            return score::cpp::make_unexpected(tag_type.error());
        }

        if (tag_type.value() == Acl::Tag::kUser)
        {
            const auto uid_ptr = operating_system.acl_get_qualifier(acl_entry);
            if (!uid_ptr.has_value())
            {
                return score::cpp::make_unexpected(uid_ptr.error());
            }

            Acl::Permissions permset{};
            operating_system.acl_get_permset(acl_entry, &permset);

            const auto is_permission_set = operating_system.acl_get_perm(permset, permission);
            if (!is_permission_set.has_value())
            {
                operating_system.acl_free(uid_ptr.value());
                return score::cpp::make_unexpected(is_permission_set.error());
            }

            if (is_permission_set.value())
            {
                score::os::IAccessControlList::UserIdentifier exec_uid = {};
                // This is safe because uid_t is expected to be TriviallyCopyable, ensuring that std::memcpy()
                // correctly copies the value without violating strict aliasing or object lifetime rules.
                // NOLINTNEXTLINE(score-banned-function): See above
                std::ignore = std::memcpy(&exec_uid, uid_ptr.value(), sizeof(uid_t));
                user_ids_with_perms.push_back(exec_uid);
            }
            operating_system.acl_free(uid_ptr.value());
        }

        entry = operating_system.acl_get_entry(acl_, Acl::kAclNextEntry);
        if (!entry.has_value())
        {
            return score::cpp::make_unexpected(entry.error());
        }
    }

    return user_ids_with_perms;
}
