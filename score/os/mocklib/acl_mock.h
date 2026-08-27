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
#ifndef SCORE_LIB_OS_MOCKLIB_ACL_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_ACL_MOCK_H

#include "score/os/acl.h"

#include "score/expected.hpp"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class AclMock : public Acl
{
  public:
    MOCK_METHOD((score::cpp::expected<AclCollection, score::os::Error>),
                acl_get_fd,
                (FileDescriptor),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<AclCollection, score::os::Error>),
                acl_get_file,
                (std::string),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                acl_create_entry,
                (AclCollection*, Entry*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<score::cpp::optional<Entry>, score::os::Error>),
                acl_get_entry,
                (AclCollection, EntryIndex),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<Tag, score::os::Error>), acl_get_tag_type, (Entry), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                acl_set_tag_type,
                (Entry, Tag),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<void*, score::os::Error>),
                acl_get_qualifier,
                (Entry),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                acl_set_qualifier,
                (Entry, const void*),
                (const, noexcept, override));
    MOCK_METHOD((void), acl_get_permset, (Entry, Permissions*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<bool, score::os::Error>),
                acl_get_perm,
                (Permissions, Permission),
                (const, noexcept, override));
    MOCK_METHOD((void), acl_clear_perms, (Permissions), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                acl_add_perm,
                (Permissions, Permission),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                acl_calc_mask,
                (AclCollection*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                acl_valid,
                (AclCollection),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                acl_set_fd,
                (FileDescriptor, AclCollection),
                (const, noexcept, override));
    MOCK_METHOD((void), acl_free, (void*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<char*, score::os::Error>),
                acl_to_text,
                (const AclCollection&, ssize_t*),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_ACL_MOCK_H
