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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fcntl.h>
#include <grp.h>
#include <utility>

namespace
{

using ::testing::HasSubstr;

constexpr auto file_name = "/tmp/my_acl_file.txt";

class AclTestFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        // Given a file where only the user has access
        file_descriptor_ = open(file_name, O_CREAT | O_RDWR, 0600);

        // When using ACL to give access to the current group
        auto acl_result = unit_.acl_get_fd(file_descriptor_);
        ASSERT_TRUE(acl_result.has_value());

        acl_ = acl_result.value();
        unit_.acl_create_entry(&acl_, &entry_);
    }

    void TearDown() override
    {
        unit_.acl_free(acl_);
        close(file_descriptor_);
        ::unlink(file_name);
    }
    score::os::AclInstance unit_{};
    ::score::os::Acl::Entry entry_{};
    acl_t acl_;
    int file_descriptor_;
};

TEST_F(AclTestFixture, CanAddGroupEntries)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall offer adding group entries");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ASSERT_TRUE(unit_.acl_set_tag_type(entry_, ::score::os::Acl::Tag::kGroup).has_value());

    gid_t group_identifier = ::getgid();
    ASSERT_TRUE(unit_.acl_set_qualifier(entry_, &group_identifier).has_value());

    ::score::os::Acl::Permissions permissions{};
    unit_.acl_get_permset(entry_, &permissions);
    unit_.acl_clear_perms(permissions);
    ASSERT_TRUE(unit_.acl_add_perm(permissions, ::score::os::Acl::Permission::kExecute).has_value());
    ASSERT_TRUE(unit_.acl_add_perm(permissions, ::score::os::Acl::Permission::kWrite).has_value());
    ASSERT_TRUE(unit_.acl_add_perm(permissions, ::score::os::Acl::Permission::kRead).has_value());

    ASSERT_TRUE(unit_.acl_calc_mask(&acl_).has_value());

    ASSERT_TRUE(unit_.acl_valid(acl_).has_value());

    ASSERT_TRUE(unit_.acl_set_fd(file_descriptor_, acl_).has_value());

    // Then the group is added accordingly
    auto* result = ::acl_get_file(file_name, ACL_TYPE_ACCESS);

    ssize_t len{};
    char* text = ::acl_to_text(result, &len);
    ASSERT_GE(len, 0);

    std::string acl_text{text, static_cast<std::size_t>(len)};
    ::acl_free(text);

    // Replace all occurrences of \040 with a space
    std::string escape_seq = "\\040";
    std::string replacement = " ";

    size_t pos = 0;
    while ((pos = acl_text.find(escape_seq, pos)) != std::string::npos)
    {
        acl_text.replace(pos, escape_seq.length(), replacement);
        pos += replacement.length();  // Move past the replacement
    }

    auto* const group = ::getgrgid(group_identifier);

    std::stringstream expected{};
    expected << "group:";
    expected << group->gr_name;
    expected << ":rwx";

    EXPECT_THAT(acl_text, HasSubstr(expected.str()));

    ::acl_free(result);
}

// Test to Set tag type to owning group
TEST_F(AclTestFixture, SettingTagTokOwningGroup)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall offer adding tags to onwing group");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ASSERT_TRUE(unit_.acl_set_tag_type(entry_, ::score::os::Acl::Tag::kOwningGroup).has_value());
    ASSERT_EQ(unit_.acl_get_tag_type(entry_).value(), ::score::os::Acl::Tag::kOwningGroup);
}

// Test to Set tag type to MaximumAllowedPermissions and return ACL_MASK
TEST_F(AclTestFixture, SettingTagTokMaximumAllowedPermissions)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall offer setting of maximum permission");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ASSERT_TRUE(unit_.acl_set_tag_type(entry_, ::score::os::Acl::Tag::kMaximumAllowedPermissions).has_value());
    ASSERT_EQ(unit_.acl_get_tag_type(entry_).value(), ::score::os::Acl::Tag::kMaximumAllowedPermissions);
}

// Test to Set tag type to kOther
TEST_F(AclTestFixture, SettingTagTokOther)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall offer set tag type to kOther");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ASSERT_TRUE(unit_.acl_set_tag_type(entry_, ::score::os::Acl::Tag::kOther).has_value());
    ASSERT_EQ(unit_.acl_get_tag_type(entry_).value(), ::score::os::Acl::Tag::kOther);
}

// Test to Set tag type to kOwningUser and return ACL_USER_OBJ
TEST_F(AclTestFixture, SettingTagTokOwningUser)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall offer set tag to kOwningUser");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ASSERT_TRUE(unit_.acl_set_tag_type(entry_, ::score::os::Acl::Tag::kOwningUser).has_value());
    ASSERT_EQ(unit_.acl_get_tag_type(entry_).value(), ::score::os::Acl::Tag::kOwningUser);
}

// Test to Set tag type to kGroup and return ACL_GROUP
TEST_F(AclTestFixture, GettingTagaclgroup)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall offer set tag to kGroup");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ASSERT_TRUE(unit_.acl_set_tag_type(entry_, ::score::os::Acl::Tag::kGroup).has_value());
    ASSERT_EQ(unit_.acl_get_tag_type(entry_).value(), ::score::os::Acl::Tag::kGroup);
}

// Test to Get Qualifier to return error
TEST_F(AclTestFixture, GetQualifierReturnErrorIfPassInvalidEntry)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall return error to get qualifier with default value");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto val = unit_.acl_get_qualifier(entry_);
    EXPECT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), score::os::Error::Code::kInvalidArgument);
}

// Test to set qualifier to return error
TEST_F(AclTestFixture, SetQualifierReturnErrorIfPassInvalidQualifier)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall return error if pass invalid qualifier");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto val = unit_.acl_set_qualifier(entry_, nullptr);
    EXPECT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), score::os::Error::Code::kInvalidArgument);
}

// Test acl_valid to return error
TEST_F(AclTestFixture, AclValidToReturnErrorIfPassInvalidAcl)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall return error if pass invalid ACL");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto val = unit_.acl_valid(acl_);
    EXPECT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(AclTestFixture, AclToTextToReturnOkIfPassValidAcl)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall return error if pass invalid ACL");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto* result = ::acl_get_file(file_name, ACL_TYPE_ACCESS);
    ssize_t len{0};
    auto res = unit_.acl_to_text(result, &len);
    ASSERT_TRUE(res.has_value());

    ::acl_free(res.value());
    ::acl_free(result);
}

TEST_F(AclTestFixture, AclToTextToReturnErrorIfPassInvalidAcl)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall return error if pass invalid ACL");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto* result = ::acl_get_file("", ACL_TYPE_ACCESS);
    ssize_t len{0};
    auto res = unit_.acl_to_text(result, &len);
    ASSERT_FALSE(res.has_value());

    ::acl_free(result);
}

// Test to set fd return error if invalid parameters are passed
TEST_F(AclTestFixture, AclSetFdToReturnErrorIfPassInvalidParam)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall return error if pass invalid param");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto val = unit_.acl_set_fd(file_descriptor_, acl_);
    EXPECT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), score::os::Error::Code::kInvalidArgument);
}

// Test to Get Entry to return Error if an Invalid Index is passed
TEST_F(AclTestFixture, AclGetEntryToReturnErrorIfPassInvalidIndex)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall return error if pass invalid index");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

#if defined(__QNX__)
    score::os::Acl::EntryIndex invalid_index = -1;
    const auto val = unit_.acl_get_entry(acl_, invalid_index);
    EXPECT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), score::os::Error::Code::kInvalidArgument);
#endif
}

TEST_F(AclTestFixture, AclGetPermissionTest)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description",
                   "ACL shall true when the permission is present in the permissions set; false otherwise.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ASSERT_TRUE(unit_.acl_set_tag_type(entry_, ::score::os::Acl::Tag::kGroup).has_value());

    const gid_t group_identifier = ::getgid();
    ASSERT_TRUE(unit_.acl_set_qualifier(entry_, &group_identifier).has_value());

    ::score::os::Acl::Permissions permissions{};
    unit_.acl_get_permset(entry_, &permissions);
    unit_.acl_clear_perms(permissions);
    ASSERT_TRUE(unit_.acl_add_perm(permissions, ::score::os::Acl::Permission::kExecute).has_value());
    ASSERT_TRUE(unit_.acl_add_perm(permissions, ::score::os::Acl::Permission::kRead).has_value());

    auto result = unit_.acl_get_perm(permissions, ::score::os::Acl::Permission::kExecute);
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value());

    result = unit_.acl_get_perm(permissions, ::score::os::Acl::Permission::kRead);
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value());

    result = unit_.acl_get_perm(permissions, ::score::os::Acl::Permission::kWrite);
    ASSERT_TRUE(result.has_value());
    ASSERT_FALSE(result.value());
}

TEST_F(AclTestFixture, AclGetFilePositiveTest)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall return correct acl for given filename if the file exists.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto acl_result = unit_.acl_get_file(file_name);
    ASSERT_TRUE(acl_result.has_value());

    unit_.acl_free(acl_result.value());
}

TEST_F(AclTestFixture, AclGetFileNegativeTest)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "ACL shall return error if provided path is empty.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const std::string filepath{""};
    const auto acl_result = unit_.acl_get_file(filepath);
    ASSERT_FALSE(acl_result.has_value());
    EXPECT_EQ(acl_result.error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

}  // namespace
