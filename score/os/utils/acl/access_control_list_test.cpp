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

#include "score/os/mocklib/acl_mock.h"

#include <gtest/gtest.h>

#if defined(__QNX__)
#include <sys/acl.h>
#define ACL_GET_PERM ::acl_get_perm_np
#else
#include <acl/libacl.h>
#define ACL_GET_PERM ::acl_get_perm
#endif

#include <score/expected.hpp>
#include <string.h>
#include <cstdlib>
#include <utility>

namespace
{

using namespace ::testing;

class AclFixture : public Test
{
  public:
    uid_t user_identifier_{42};
    score::os::MockGuard<score::os::AclMock> os_mock_{};
    score::os::Acl::FileDescriptor file_descriptor_{17};
    std::string file_path_{"/tmp/test_file"};
    score::os::AccessControlList unit_{file_descriptor_};
    score::cpp::expected_blank<score::os::Error> error_{
        score::cpp::make_unexpected(score::os::Error::createFromErrno())};
    score::cpp::expected_blank<score::os::Error> no_error_{};
    const score::os::Acl::Entry dummy_entry_{reinterpret_cast<score::os::Acl::Entry>(15)};
};

class PermissionVerificationParam
    : public ::testing::TestWithParam<std::tuple<bool, std::string, std::vector<::score::os::Acl::Permission>>>
{
};

TEST_P(PermissionVerificationParam, AclVerification)
{
    score::os::MockGuard<score::os::AclMock> os_mock_{};
    auto permissions = std::get<std::vector<::score::os::Acl::Permission>>(GetParam());
    auto acl_text = std::get<std::string>(GetParam());
    char acl_text_copy[1024];
    auto is_verify_success = std::get<bool>(GetParam());

    strncpy(acl_text_copy, acl_text.data(), sizeof(acl_text_copy) - 1);

    // Expecting that we get the given acl text output from a call to acl_to_text()
    EXPECT_CALL(*os_mock_, acl_to_text(_, _))
        .WillOnce(Invoke([&acl_text_copy](const ::score::os::Acl::AclCollection&,
                                          ssize_t* size) -> score::cpp::expected<char*, score::os::Error> {
            *size = static_cast<ssize_t>(strlen(acl_text_copy));
            return acl_text_copy;
        }));
    // Expecting that acl_free gets called twice, once for freeing acl_text and once finally, when acl_t gets destroyed.
    EXPECT_CALL(*os_mock_, acl_free(_)).Times(2);

    score::os::AccessControlList unit{42};
    // When verifying with permissions containing a read-perm
    const auto result = unit.VerifyMaskPermissions(permissions);

    // That verification result has a value
    ASSERT_TRUE(result.has_value());
    // and the value is "true"
    EXPECT_EQ(result.value(), is_verify_success);
}

const std::vector<std::tuple<bool, std::string, std::vector<::score::os::Acl::Permission>>> acl_permissions{
    {true,
     "user::rw-\n"
     "user:foresightmapprovisiond:rw-\n"
     "user:aascomhandlerd:rw-\n"
     "user:senseassessmentd:rw-\n"
     "group::---\n"
     "mask::rw-\n"
     "other::---",
     {::score::os::Acl::Permission::kRead, ::score::os::Acl::Permission::kWrite}},
    {true,
     "user::rw-\n"
     "user:foresightmapprovisiond:rw-\n"
     "user:aascomhandlerd:rw-\n"
     "user:senseassessmentd:rw-\n"
     "group::---\n"
     "mask::rw-\n"
     "other::---",
     {::score::os::Acl::Permission::kRead}},
    {true,
     "user::rw-\n"
     "user:foresightmapprovisiond:rw-\n"
     "user:aascomhandlerd:rw-\n"
     "user:senseassessmentd:rw-\n"
     "group::---\n"
     "mask::rwx\n"
     "other::---",
     {::score::os::Acl::Permission::kRead, ::score::os::Acl::Permission::kExecute}},
    {false,
     "user::rw-\n"
     "user:foresightmapprovisiond:rw-\n"
     "user:aascomhandlerd:rw-\n"
     "user:senseassessmentd:rw-\n"
     "group::---\n"
     "mask::-w-\n"
     "other::---",
     {::score::os::Acl::Permission::kRead, ::score::os::Acl::Permission::kWrite}},
    {false,
     "user::rw-\n"
     "user:foresightmapprovisiond:rw-\n"
     "user:aascomhandlerd:rw-\n"
     "user:senseassessmentd:rw-\n"
     "group::---\n"
     "mask::---\n"
     "other::---",
     {::score::os::Acl::Permission::kRead}},
    {false,
     "user::rw-\n"
     "user:foresightmapprovisiond:rw-\n"
     "user:aascomhandlerd:rw-\n"
     "user:senseassessmentd:rw-\n"
     "group::---\n"
     "mask::r--\n"
     "other::---",
     {::score::os::Acl::Permission::kWrite}},
    {false,
     "user::rw-\n"
     "user:foresightmapprovisiond:rw-\n"
     "user:aascomhandlerd:rw-\n"
     "user:senseassessmentd:rw-\n"
     "group::---\n"
     "mask::rw-\n"
     "other::--x",
     {::score::os::Acl::Permission::kRead, ::score::os::Acl::Permission::kExecute}}};

INSTANTIATE_TEST_SUITE_P(AclPermissionValidation, PermissionVerificationParam, ::testing::ValuesIn(acl_permissions));

TEST_F(AclFixture, VerifyPermissionsError)
{
    // Given an ACL text as returned by acl_to_text() with a "good" mask
    char sample_ACL_text[]{
        "user::rw-\n"
        "user:foresightmapprovisiond:rw-\n"
        "user:aascomhandlerd:rw-\n"
        "user:senseassessmentd:rw-\n"
        "group::---\n"
        "mask::r--\n"
        "other::---"};

    std::vector<::score::os::Acl::Permission> permissions{::score::os::Acl::Permission::kRead};

    // Expecting that we get an error from a call to acl_to_text()
    EXPECT_CALL(*os_mock_, acl_to_text(_, _))
        .WillOnce(Invoke([sample_ACL_text](const ::score::os::Acl::AclCollection&,
                                           ssize_t* size) -> score::cpp::expected<char*, score::os::Error> {
            *size = static_cast<ssize_t>(strlen(sample_ACL_text));
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }));

    score::os::AccessControlList unit{file_descriptor_};
    // When verifying with permissions containing a read-perm
    const auto result = unit.VerifyMaskPermissions(permissions);

    // That verification result has no value
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, VerifyAclError)
{
    // Given an ACL text as returned by acl_to_text() with a "good" mask
    char sample_ACL_text[]{
        "user::rw-\n"
        "user:foresightmapprovisiond:rw-\n"
        "user:aascomhandlerd:rw-\n"
        "user:senseassessmentd:rw-\n"
        "group::---\n"
        "mas::r--\n"
        "other::---"};

    char acl_text_copy[1024] = {0};
    strncpy(acl_text_copy, sample_ACL_text, sizeof(acl_text_copy) - 1);

    std::vector<::score::os::Acl::Permission> permissions{::score::os::Acl::Permission::kRead};

    // Expecting that we get an error from a call to acl_to_text()
    EXPECT_CALL(*os_mock_, acl_to_text(_, _))
        .WillOnce(Invoke([&acl_text_copy](const ::score::os::Acl::AclCollection&,
                                          ssize_t* size) -> score::cpp::expected<char*, score::os::Error> {
            *size = static_cast<ssize_t>(strlen(acl_text_copy));
            return acl_text_copy;
        }));

    score::os::AccessControlList unit{file_descriptor_};
    // When verifying with permissions containing a read-perm
    const auto result = unit.VerifyMaskPermissions(permissions);

    // That verification result has value
    ASSERT_TRUE(result.has_value());
    // value is false since acl format is invalid
    ASSERT_FALSE(result.value());
}

TEST_F(AclFixture, ErrorOnReadingAclFromFile)
{
    // Given nothing
    // Expecting that we get an error while reading ACL from a file
    EXPECT_CALL(*os_mock_, acl_get_fd(file_descriptor_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));

    // When creating our AccessControlList
    score::os::AccessControlList unit{file_descriptor_};
    const auto result = unit.AllowUser(user_identifier_, ::score::os::Acl::Permission::kWrite);

    // That allowing users returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, FindUserIdsWithPermission_ErrorOnReadingAclFromFile)
{
    // Given nothing
    // Expecting that we get an error while reading ACL from a file
    EXPECT_CALL(*os_mock_, acl_get_fd(file_descriptor_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));

    // When creating our AccessControlList
    score::os::AccessControlList unit{file_descriptor_};
    const auto result = unit.FindUserIdsWithPermission(::score::os::Acl::Permission::kWrite);

    // That finding users with specific permission returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, FindUserIdsWithPermission_ErrorOnReadingAclFromFilePath)
{
    // Given nothing
    // Expecting that we get an error while reading ACL from a file
    EXPECT_CALL(*os_mock_, acl_get_file(file_path_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));

    // When creating our AccessControlList
    score::os::AccessControlList unit{file_path_};
    const auto result = unit.FindUserIdsWithPermission(::score::os::Acl::Permission::kWrite);

    // That allowing users returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, FindUserIdsWithPermission_GetAclEntryFails)
{
    // Given
    EXPECT_CALL(*os_mock_, acl_get_fd(file_descriptor_));

    // Expecting that we get an error while reading ACL entry
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));

    // When creating our AccessControlList
    score::os::AccessControlList unit{file_descriptor_};
    const auto result = unit.FindUserIdsWithPermission(::score::os::Acl::Permission::kWrite);

    // That finding users with specific permission returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, FindUserIdsWithPermission_GetAclFirstEntryReturnsNull)
{
    // Given
    EXPECT_CALL(*os_mock_, acl_get_fd(file_descriptor_));
    // Expecting that we get a nullptr while reading first ACL entry
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry)).WillOnce(Return(score::cpp::nullopt));

    // When creating our AccessControlList
    score::os::AccessControlList unit{file_descriptor_};
    const auto result = unit.FindUserIdsWithPermission(::score::os::Acl::Permission::kWrite);

    // That finding users with specific permission returns an error
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value().empty());
}

TEST_F(AclFixture, FindUserIdsWithPermission_GetAclTagTypeReturnsError)
{
    // Given
    EXPECT_CALL(*os_mock_, acl_get_fd(file_descriptor_));
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry)).WillOnce(Return(dummy_entry_));
    // Expecting that we get an error when reading acl tag type
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));

    // When creating our AccessControlList
    score::os::AccessControlList unit{file_descriptor_};
    const auto result = unit.FindUserIdsWithPermission(::score::os::Acl::Permission::kWrite);

    // That finding users with specific permission returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, FindUserIdsWithPermission_NoAclEntryExistsWithUsertagType)
{
    // Given
    EXPECT_CALL(*os_mock_, acl_get_fd(file_descriptor_));
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry)).WillOnce(Return(dummy_entry_));
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclNextEntry)).WillOnce(Return(score::cpp::nullopt));
    // Expecting that no acl entry exist with user tag type
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry_)).WillOnce(Return(score::os::Acl::Tag::kOwningUser));

    // When creating our AccessControlList
    score::os::AccessControlList unit{file_descriptor_};
    const auto result = unit.FindUserIdsWithPermission(::score::os::Acl::Permission::kWrite);

    // That finding users with specific permission returns an error
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value().empty());
}

TEST_F(AclFixture, FindUserIdsWithPermission_GetAcNextEntryReturnsError)
{
    // Given
    EXPECT_CALL(*os_mock_, acl_get_fd(file_descriptor_));
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry)).WillOnce(Return(dummy_entry_));
    // Expecting that we get an error while reading next acl entry
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclNextEntry))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry_)).WillOnce(Return(score::os::Acl::Tag::kOwningUser));

    // When creating our AccessControlList
    score::os::AccessControlList unit{file_descriptor_};
    const auto result = unit.FindUserIdsWithPermission(::score::os::Acl::Permission::kWrite);

    // That finding users with specific permission returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, FindUserIdsWithPermission_GetQualifierFromAclEntryReturnsError)
{
    // Given
    EXPECT_CALL(*os_mock_, acl_get_fd(file_descriptor_));
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry)).WillOnce(Return(dummy_entry_));
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry_)).WillOnce(Return(score::os::Acl::Tag::kUser));
    // Expecting that we get an error while reading qualifier from an ACL entry
    EXPECT_CALL(*os_mock_, acl_get_qualifier(dummy_entry_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL))));

    // When creating our AccessControlList
    score::os::AccessControlList unit{file_descriptor_};
    const auto result = unit.FindUserIdsWithPermission(::score::os::Acl::Permission::kWrite);

    // That finding users with specific permission returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, FindUserIdsWithPermission_CheckingPermissionReturnError)
{
    uid_t user_uid = 3030U;
    ::score::os::Acl::Permission permission = ::score::os::Acl::Permission::kExecute;
    ::score::os::Acl::Permissions permset{};
    // Given
    EXPECT_CALL(*os_mock_, acl_get_fd(file_descriptor_));
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry)).WillOnce(Return(dummy_entry_));
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry_)).WillOnce(Return(score::os::Acl::Tag::kUser));
    EXPECT_CALL(*os_mock_, acl_get_qualifier(dummy_entry_)).WillOnce(Return(&user_uid));
    EXPECT_CALL(*os_mock_, acl_get_permset(dummy_entry_, _)).WillOnce(SetArgPointee<1>(permset));
    // Expecting that we get an awhen checking a given permission is present in an ACL permissions set
    EXPECT_CALL(*os_mock_, acl_get_perm(permset, permission))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));

    // When creating our AccessControlList
    score::os::AccessControlList unit{file_descriptor_};
    const auto result = unit.FindUserIdsWithPermission(permission);

    // That finding users with specific permission returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, FindUserIdsWithPermission_GivenPermissionIsNotPresentInThePermisssionsSet)
{
    uid_t user_uid = 3030U;
    uid_t* uid_ptr = &user_uid;
    std::vector<score::os::IAccessControlList::UserIdentifier> expected_user_list{};
    ::score::os::Acl::Permission permission = ::score::os::Acl::Permission::kExecute;
    ::score::os::Acl::Permissions permset{};

    // Given
    EXPECT_CALL(*os_mock_, acl_get_entry).WillOnce(Return(dummy_entry_)).WillOnce(Return(score::cpp::nullopt));
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry_)).WillOnce(Return(score::os::Acl::Tag::kUser));
    EXPECT_CALL(*os_mock_, acl_get_qualifier(dummy_entry_)).WillOnce([uid_ptr](auto) -> void* {
        return uid_ptr;
    });
    EXPECT_CALL(*os_mock_, acl_get_permset(dummy_entry_, _)).WillOnce(SetArgPointee<1>(permset));
    // Expecting that the permission isn't present in the permissions set.
    EXPECT_CALL(*os_mock_, acl_get_perm(permset, permission)).WillOnce(Return(false));
    EXPECT_CALL(*os_mock_, acl_free).Times(2);

    // When creating our AccessControlList
    const auto result = unit_.FindUserIdsWithPermission(permission);

    // That finding users with specific permission returns an error
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), expected_user_list);
}

TEST_F(AclFixture, FindUserIdsWithPermission_UserFoundWithRequestedPermission)
{
    uid_t user_uid = 3030U;
    uid_t* uid_ptr = &user_uid;
    std::vector<score::os::IAccessControlList::UserIdentifier> expected_user_list{user_uid};
    ::score::os::Acl::Permission permission = ::score::os::Acl::Permission::kExecute;
    ::score::os::Acl::Permissions permset{};

    // Given
    EXPECT_CALL(*os_mock_, acl_get_entry).WillOnce(Return(dummy_entry_)).WillOnce(Return(score::cpp::nullopt));
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry_)).WillOnce(Return(score::os::Acl::Tag::kUser));
    EXPECT_CALL(*os_mock_, acl_get_qualifier(dummy_entry_)).WillOnce([uid_ptr](auto) -> void* {
        return uid_ptr;
    });
    EXPECT_CALL(*os_mock_, acl_get_permset(dummy_entry_, _)).WillOnce(SetArgPointee<1>(permset));
    // Expecting that the permission is present in the permissions set
    EXPECT_CALL(*os_mock_, acl_get_perm(permset, permission)).WillOnce(Return(true));
    EXPECT_CALL(*os_mock_, acl_free).Times(2);

    // When creating our AccessControlList
    const auto result = unit_.FindUserIdsWithPermission(permission);

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), expected_user_list);
}

class FindUsersWithPermissionParam
    : public ::testing::TestWithParam<std::tuple<bool, std::string, ::score::os::Acl::Permission, std::vector<uid_t>>>
{
};

TEST_P(FindUsersWithPermissionParam, FindUserIdsWithPermissionTest)
{
    score::os::AclMock os_mock_{};
    score::os::Acl::FileDescriptor file_descriptor{17};
    const auto expected_result = std::get<0>(GetParam());
    std::string acl_text = std::get<1>(GetParam());
    const auto permission = std::get<2>(GetParam());
    std::vector<score::os::IAccessControlList::UserIdentifier> expected_user_list = std::get<3>(GetParam());

    acl_t acl = ::acl_from_text(acl_text.c_str());
    ASSERT_NE(acl, nullptr) << "Failed to create ACL from text";

    EXPECT_CALL(os_mock_, acl_get_fd(file_descriptor)).WillOnce(Return(acl));

    score::os::Acl::set_testing_instance(os_mock_);
    score::os::AccessControlList unit{file_descriptor};
    score::os::Acl::restore_instance();

    const auto result = unit.FindUserIdsWithPermission(permission);

    ASSERT_EQ(result.has_value(), expected_result);
    ASSERT_EQ(result.value(), expected_user_list);
}

const std::vector<std::tuple<bool, std::string, ::score::os::Acl::Permission, std::vector<uid_t>>>
    acl_user_permission_entries{{true,
                                 "user::rw-\n"
                                 "user:3020:rw-\n"
                                 "user:3025:rw-\n"
                                 "user:3035:rw-\n"
                                 "group::---\n"
                                 "mask::rw-\n"
                                 "other::---",
                                 ::score::os::Acl::Permission::kExecute,
                                 {}},
                                {true,
                                 "user::rw-\n"
                                 "user:3020:rwx\n"
                                 "user:3025:rw-\n"
                                 "user:3035:rwx\n"
                                 "group::---\n"
                                 "mask::rwx\n"
                                 "other::---",
                                 ::score::os::Acl::Permission::kExecute,
                                 {3020, 3035}},
                                {true,
                                 "user::rw-\n"
                                 "group::---\n"
                                 "mask::rwx\n"
                                 "other::---",
                                 ::score::os::Acl::Permission::kExecute,
                                 {}},
                                {true,
                                 "user::rw-\n"
                                 "user:3020:rw-\n"
                                 "user:3025:rwx\n"
                                 "user:3035:rw-\n"
                                 "group::---\n"
                                 "mask::rwx\n"
                                 "other::---",
                                 ::score::os::Acl::Permission::kExecute,
                                 {3025}},
                                {true,
                                 "user::rw-\n"
                                 "user:3020:rw-\n"
                                 "user:3025:rwx\n"
                                 "user:3035:rw-\n"
                                 "group::---\n"
                                 "mask::rwx\n"
                                 "other::---",
                                 ::score::os::Acl::Permission::kRead,
                                 {3020, 3025, 3035}}};

INSTANTIATE_TEST_SUITE_P(FindUserPermissionWithAcl,
                         FindUsersWithPermissionParam,
                         ::testing::ValuesIn(acl_user_permission_entries));

class FindUsersWithPermissionParamFilePath
    : public ::testing::TestWithParam<std::tuple<bool, std::string, ::score::os::Acl::Permission, std::vector<uid_t>>>
{
};

TEST_P(FindUsersWithPermissionParamFilePath, FindUserIdsWithPermissionTestFilePath)
{
    score::os::AclMock os_mock_{};
    std::string file_path{"/tmp/test_file"};
    const auto expected_result = std::get<0>(GetParam());
    std::string acl_text = std::get<1>(GetParam());
    const auto permission = std::get<2>(GetParam());
    std::vector<score::os::IAccessControlList::UserIdentifier> expected_user_list = std::get<3>(GetParam());

    acl_t acl = ::acl_from_text(acl_text.c_str());
    ASSERT_NE(acl, nullptr) << "Failed to create ACL from text";

    EXPECT_CALL(os_mock_, acl_get_file(file_path)).WillOnce(Return(acl));

    score::os::Acl::set_testing_instance(os_mock_);
    score::os::AccessControlList unit{file_path};
    score::os::Acl::restore_instance();

    const auto result = unit.FindUserIdsWithPermission(permission);

    ASSERT_EQ(result.has_value(), expected_result);
    ASSERT_EQ(result.value(), expected_user_list);
}

INSTANTIATE_TEST_SUITE_P(FindUserPermissionWithAclFilePath,
                         FindUsersWithPermissionParamFilePath,
                         ::testing::ValuesIn(acl_user_permission_entries));

TEST_F(AclFixture, ErrorWhileCreatingACLEntry)
{
    // Given a valid constructed unit
    // Expecting an error while creating a new ACL entry
    EXPECT_CALL(*os_mock_, acl_create_entry(_, _)).WillOnce(Return(error_));

    // When adding a new user entry shall be allowed
    const auto result = unit_.AllowUser(user_identifier_, ::score::os::Acl::Permission::kWrite);

    // That allowing users returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, ErrorWhileSettingTagForEntry)
{
    // Given a valid constructed unit
    // Expecting an error while setting the tag for the ACL entry
    EXPECT_CALL(*os_mock_, acl_create_entry(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_tag_type(_, _)).WillOnce(Return(error_));

    // When adding a new user entry shall be allowed
    const auto result = unit_.AllowUser(user_identifier_, ::score::os::Acl::Permission::kWrite);

    // That allowing users returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, ErrorWhileSettingUserIdForEntry)
{
    // Given a valid constructed unit
    // Expecting an error while setting the user-id for the ACL entry
    EXPECT_CALL(*os_mock_, acl_create_entry(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_tag_type(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_qualifier(_, _)).WillOnce(Return(error_));

    // When adding a new user entry shall be allowed
    const auto result = unit_.AllowUser(user_identifier_, ::score::os::Acl::Permission::kWrite);

    // That allowing users returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, ErrorWhileAddingPermissionsForACLEntry)
{
    // Given a valid constructed unit
    // Expecting an error while setting the permission for the ACL entry
    EXPECT_CALL(*os_mock_, acl_create_entry(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_tag_type(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_qualifier(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_add_perm(_, ::score::os::Acl::Permission::kWrite)).WillOnce(Return(error_));

    // When adding a new user entry shall be allowed
    const auto result = unit_.AllowUser(user_identifier_, ::score::os::Acl::Permission::kWrite);

    // That allowing users returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, ErrorWhileCalculatingMaskForACLEntry)
{
    // Given a valid constructed unit
    // Expecting an error while calculating the mask for the ACL entry
    EXPECT_CALL(*os_mock_, acl_create_entry(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_tag_type(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_qualifier(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_add_perm(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_calc_mask(_)).WillOnce(Return(error_));

    // When adding a new user entry shall be allowed
    const auto result = unit_.AllowUser(user_identifier_, ::score::os::Acl::Permission::kWrite);

    // That allowing users returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, ErrorWhileValidatingForACL)
{
    // Given a valid constructed unit
    // Expecting an error while validating the ACL
    EXPECT_CALL(*os_mock_, acl_create_entry(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_tag_type(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_qualifier(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_add_perm(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_calc_mask(_)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_valid(_)).WillOnce(Return(error_));

    // When adding a new user entry shall be allowed
    const auto result = unit_.AllowUser(user_identifier_, ::score::os::Acl::Permission::kWrite);

    // That allowing users returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, ErrorWhileSettingTheACL)
{
    // Given a valid constructed unit
    // Expecting an error while setting the ACL
    EXPECT_CALL(*os_mock_, acl_create_entry(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_tag_type(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_qualifier(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_add_perm(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_calc_mask(_)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_valid(_)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_fd(file_descriptor_, _)).WillOnce(Return(error_));

    // When adding a new user entry shall be allowed
    const auto result = unit_.AllowUser(user_identifier_, ::score::os::Acl::Permission::kWrite);

    // That allowing users returns an error
    ASSERT_FALSE(result.has_value());
}

TEST_F(AclFixture, NoErrorWhileAllowingNewUser)
{
    // Given a valid constructed unit
    // Expecting no errors
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry)).WillOnce(Return(score::cpp::nullopt));
    EXPECT_CALL(*os_mock_, acl_create_entry(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_tag_type(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_qualifier(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_add_perm(_, _)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_calc_mask(_)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_valid(_)).WillOnce(Return(no_error_));
    EXPECT_CALL(*os_mock_, acl_set_fd(file_descriptor_, _)).WillOnce(Return(no_error_));

    // When adding a new user entry shall be allowed
    const auto result = unit_.AllowUser(user_identifier_, ::score::os::Acl::Permission::kWrite);

    // That allowing users returns an error
    ASSERT_TRUE(result.has_value());
}

TEST_F(AclFixture, AddMultiplePermissionsPerUser)
{
    const auto dummy_entry2 = reinterpret_cast<score::os::Acl::Entry>(30);
    const auto dummy_entry3 = reinterpret_cast<score::os::Acl::Entry>(60);
    uid_t wrong_uid = user_identifier_ + 1;
    uid_t uid = user_identifier_;
    auto dummy_permissions{reinterpret_cast<score::os::Acl::Permissions>(38)};

    // Given a valid constructed unit that already has an entry for a group and two users
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry)).WillOnce(Return(dummy_entry_));
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclNextEntry))
        .WillOnce(Return(dummy_entry2))
        .WillOnce(Return(dummy_entry3));
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry_)).WillOnce(Return(score::os::Acl::Tag::kGroup));
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry2)).WillOnce(Return(score::os::Acl::Tag::kUser));
    EXPECT_CALL(*os_mock_, acl_get_qualifier(dummy_entry2)).WillOnce(Return(reinterpret_cast<void*>(&wrong_uid)));
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry3)).WillOnce(Return(score::os::Acl::Tag::kUser));
    EXPECT_CALL(*os_mock_, acl_get_qualifier(dummy_entry3)).WillOnce(Return(reinterpret_cast<void*>(&uid)));
    EXPECT_CALL(*os_mock_, acl_get_permset(dummy_entry3, _))
        .WillOnce([dummy_permissions](auto, score::os::Acl::Permissions* perms) {
            *perms = dummy_permissions;
        });
    EXPECT_CALL(*os_mock_, acl_add_perm(dummy_permissions, score::os::Acl::Permission::kRead))
        .WillOnce(Return(score::cpp::blank{}));

    const auto result = unit_.AllowUser(user_identifier_, score::os::Acl::Permission::kRead);

    ASSERT_TRUE(result.has_value());
}

TEST_F(AclFixture, ErrorWhenFailingToRetrieveQualifier)
{
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry)).WillOnce(Return(dummy_entry_));
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry_)).WillOnce(Return(score::os::Acl::Tag::kUser));
    EXPECT_CALL(*os_mock_, acl_get_qualifier(dummy_entry_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL))));
    const auto result = unit_.AllowUser(user_identifier_, score::os::Acl::Permission::kWrite);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(AclFixture, ErrorWhenFailingToRetrieveTag)
{
    EXPECT_CALL(*os_mock_, acl_get_entry(_, score::os::Acl::kAclFirstEntry)).WillOnce(Return(dummy_entry_));
    EXPECT_CALL(*os_mock_, acl_get_tag_type(dummy_entry_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL))));
    const auto result = unit_.AllowUser(user_identifier_, score::os::Acl::Permission::kWrite);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST(AclTest, AddRwPermissionForUser)
{
    // We need to use the current uid here as setting arbitrary uids seems to be prevented by the Bazel sandbox
    const uid_t user_identifer{::getuid()};
    std::string filename{};

    const char* test_tmpdir = ::getenv("TEST_TMPDIR");
    if (test_tmpdir != nullptr)
    {
        filename = test_tmpdir;
    }
    else
    {
        filename = "/tmp";
    }

    filename += "/acl-test-XXXXXX";

    // Given a dummy temporary file with the default access permissions (0600) and no explicit permission set for
    // the current user.
    int tempfile = ::mkstemp(&filename[0]);
    ::unlink(filename.c_str());

    // (Verify that user is not existing as a user entry)
    acl_t cur_acl = ::acl_get_fd(tempfile);
    ASSERT_NE(cur_acl, nullptr);

    acl_entry_t entry;
    int get_entry_result = ::acl_get_entry(cur_acl, ACL_FIRST_ENTRY, &entry);
    while (get_entry_result == 1)
    {
        acl_tag_t tag_type{};
        ASSERT_NE(-1, ::acl_get_tag_type(entry, &tag_type));
        if (tag_type == ACL_USER)
        {
            void* qualifier = ::acl_get_qualifier(entry);
            ASSERT_NE(nullptr, qualifier);
            ASSERT_NE(user_identifer, *static_cast<uid_t*>(qualifier));
            ::acl_free(qualifier);
        }
        get_entry_result = ::acl_get_entry(cur_acl, ACL_NEXT_ENTRY, &entry);
    }
    ASSERT_NE(get_entry_result, -1);
    ::acl_free(cur_acl);

    // when both read and write permissions are granted
    score::os::AccessControlList access_control_list{tempfile};
    auto allow_result = access_control_list.AllowUser(user_identifer, score::os::Acl::Permission::kRead);
    ASSERT_TRUE(allow_result.has_value());
    allow_result = access_control_list.AllowUser(user_identifer, score::os::Acl::Permission::kWrite);
    ASSERT_TRUE(allow_result.has_value());

    // then the user can be found in the ACL having rw rights
    cur_acl = ::acl_get_fd(tempfile);
    ASSERT_NE(cur_acl, nullptr);
    bool found{false};
    get_entry_result = ::acl_get_entry(cur_acl, ACL_FIRST_ENTRY, &entry);
    while (get_entry_result == 1 && !found)
    {
        acl_tag_t tag_type{};
        ASSERT_NE(-1, ::acl_get_tag_type(entry, &tag_type));
        if (tag_type == ACL_USER)
        {
            void* qualifier = ::acl_get_qualifier(entry);
            ASSERT_NE(nullptr, qualifier);
            if (*static_cast<uid_t*>(qualifier) == user_identifer)
            {
                found = true;
                acl_permset_t perms{};
                ASSERT_NE(-1, ::acl_get_permset(entry, &perms));
                ASSERT_EQ(1, ACL_GET_PERM(perms, ACL_READ));
                ASSERT_EQ(1, ACL_GET_PERM(perms, ACL_WRITE));
                ASSERT_EQ(0, ACL_GET_PERM(perms, ACL_EXECUTE));
            }
            ::acl_free(qualifier);
        }
        get_entry_result = ::acl_get_entry(cur_acl, ACL_NEXT_ENTRY, &entry);
    }
    ::acl_free(cur_acl);
    ASSERT_NE(get_entry_result, -1);
    ASSERT_TRUE(found);

    ::close(tempfile);
}

TEST(AclTest, AddRwPermissionForUserFilePath)
{
    // We need to use the current uid here as setting arbitrary uids seems to be prevented by the Bazel sandbox
    const uid_t user_identifer{::getuid()};
    std::string filename{};

    const char* test_tmpdir = ::getenv("TEST_TMPDIR");
    if (test_tmpdir != nullptr)
    {
        filename = test_tmpdir;
    }
    else
    {
        filename = "/tmp";
    }

    filename += "/acl-test-XXXXXX";

    // Given a dummy temporary file with the default access permissions (0600) and no explicit permission set for
    // the current user.
    int tempfile = ::mkstemp(&filename[0]);

    // (Verify that user is not existing as a user entry)
    acl_t cur_acl = ::acl_get_file(filename.c_str(), ACL_TYPE_ACCESS);
    ASSERT_NE(cur_acl, nullptr);

    acl_entry_t entry;
    int get_entry_result = ::acl_get_entry(cur_acl, ACL_FIRST_ENTRY, &entry);
    while (get_entry_result == 1)
    {
        acl_tag_t tag_type{};
        ASSERT_NE(-1, ::acl_get_tag_type(entry, &tag_type));
        if (tag_type == ACL_USER)
        {
            void* qualifier = ::acl_get_qualifier(entry);
            ASSERT_NE(nullptr, qualifier);
            ASSERT_NE(user_identifer, *static_cast<uid_t*>(qualifier));
            ::acl_free(qualifier);
        }
        get_entry_result = ::acl_get_entry(cur_acl, ACL_NEXT_ENTRY, &entry);
    }
    ASSERT_NE(get_entry_result, -1);
    ::acl_free(cur_acl);

    // when both read and write permissions are granted
    score::os::AccessControlList access_control_list{tempfile};
    auto allow_result = access_control_list.AllowUser(user_identifer, score::os::Acl::Permission::kRead);
    ASSERT_TRUE(allow_result.has_value());
    allow_result = access_control_list.AllowUser(user_identifer, score::os::Acl::Permission::kWrite);
    ASSERT_TRUE(allow_result.has_value());

    // then the user can be found in the ACL having rw rights
    cur_acl = ::acl_get_fd(tempfile);
    ASSERT_NE(cur_acl, nullptr);
    bool found{false};
    get_entry_result = ::acl_get_entry(cur_acl, ACL_FIRST_ENTRY, &entry);
    while (get_entry_result == 1 && !found)
    {
        acl_tag_t tag_type{};
        ASSERT_NE(-1, ::acl_get_tag_type(entry, &tag_type));
        if (tag_type == ACL_USER)
        {
            void* qualifier = ::acl_get_qualifier(entry);
            ASSERT_NE(nullptr, qualifier);
            if (*static_cast<uid_t*>(qualifier) == user_identifer)
            {
                found = true;
                acl_permset_t perms{};
                ASSERT_NE(-1, ::acl_get_permset(entry, &perms));
                ASSERT_EQ(1, ACL_GET_PERM(perms, ACL_READ));
                ASSERT_EQ(1, ACL_GET_PERM(perms, ACL_WRITE));
                ASSERT_EQ(0, ACL_GET_PERM(perms, ACL_EXECUTE));
            }
            ::acl_free(qualifier);
        }
        get_entry_result = ::acl_get_entry(cur_acl, ACL_NEXT_ENTRY, &entry);
    }
    ::acl_free(cur_acl);
    ASSERT_NE(get_entry_result, -1);
    ASSERT_TRUE(found);

    ::unlink(filename.c_str());
    ::close(tempfile);
}

}  // namespace
