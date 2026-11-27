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
#include "score/os/user_id.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <type_traits>

namespace score
{
namespace os
{
namespace
{

static_assert(std::is_trivially_copyable<UserId>::value, "UserId must be trivially copyable");
static_assert(std::is_trivially_copyable<GroupId>::value, "GroupId must be trivially copyable");
static_assert(sizeof(UserId) == sizeof(uid_t), "UserId must not add storage overhead");
static_assert(sizeof(GroupId) == sizeof(gid_t), "GroupId must not add storage overhead");

static_assert(!std::is_convertible<uid_t, UserId>::value, "Construction from native must be explicit");
static_assert(!std::is_convertible<std::int64_t, UserId>::value, "No implicit construction from integers");
static_assert(!std::is_convertible<UserId, uid_t>::value, "No implicit conversion to native");
static_assert(!std::is_convertible<UserId, std::int64_t>::value, "No implicit conversion to integers");
static_assert(!std::is_convertible<UserId, GroupId>::value, "UserId and GroupId must not be interchangeable");
static_assert(!std::is_convertible<GroupId, UserId>::value, "UserId and GroupId must not be interchangeable");

TEST(UserIdTest, DefaultConstructedIsZero)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Default constructed identities hold the native value zero.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    EXPECT_EQ(UserId{}.native(), static_cast<uid_t>(0));
    EXPECT_EQ(GroupId{}.native(), static_cast<gid_t>(0));
}

TEST(UserIdTest, NativeValueRoundTripsWithoutLoss)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The native value is preserved, including the POSIX (uid_t)-1 sentinel.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    const uid_t native_uid{1234};
    EXPECT_EQ(UserId{native_uid}.native(), native_uid);
    EXPECT_EQ(kUnchangedUserId.native(), static_cast<uid_t>(-1));
    EXPECT_EQ(kUnchangedGroupId.native(), static_cast<gid_t>(-1));
}

TEST(UserIdTest, ComparesByNativeValue)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Identities of the same kind compare by their native value.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const UserId a{static_cast<uid_t>(1)};
    const UserId b{static_cast<uid_t>(2)};
    EXPECT_TRUE(a == UserId{static_cast<uid_t>(1)});
    EXPECT_TRUE(a != b);
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);

    EXPECT_TRUE(GroupId{static_cast<gid_t>(5)} == GroupId{static_cast<gid_t>(5)});
    EXPECT_TRUE(GroupId{static_cast<gid_t>(5)} != GroupId{static_cast<gid_t>(6)});
}

TEST(UserIdTest, UnchangedSentinelDiffersFromRoot)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The chown() 'do not change' sentinel is distinct from root.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    EXPECT_NE(kUnchangedUserId, UserId{});
    EXPECT_NE(kUnchangedGroupId, GroupId{});
}

}  // namespace
}  // namespace os
}  // namespace score
