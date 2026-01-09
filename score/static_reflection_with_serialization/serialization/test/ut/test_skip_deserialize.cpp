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
#include "static_reflection_with_serialization/serialization/skip_deserialize.h"

#include <type_traits>

#include <gmock/gmock.h>

namespace test
{

struct S
{
    int f1;
    int f2;
    int f3;
    std::vector<int> f4;
};

struct S1
{
    int f1;
    ::score::common::visitor::skip_deserialize<int> f2;
    ::score::common::visitor::skip_deserialize<int> f3;
    ::score::common::visitor::skip_deserialize<std::vector<int>> f4;
};

struct S2
{
    ::score::common::visitor::skip_deserialize<int> f1;
    int f2;
    ::score::common::visitor::skip_deserialize<int> f3;
    ::score::common::visitor::skip_deserialize<std::vector<int>> f4;
};

struct S3s
{
    ::score::common::visitor::skip_deserialize<int> f1;
    ::score::common::visitor::skip_deserialize<int> f2;
    int f3;
};

STRUCT_VISITABLE(S, f1, f2, f3, f4)

STRUCT_VISITABLE(S1, f1, f2, f3, f4)
static_assert(::score::common::visitor::is_payload_compatible<S1, S>(), "shall be compatible");

STRUCT_VISITABLE(S2, f1, f2, f3, f4)
static_assert(::score::common::visitor::is_payload_compatible<S2, S>(), "shall be compatible");

STRUCT_VISITABLE(S3s, f1, f2, f3)

}  // namespace test

static_assert(std::is_empty<::score::common::visitor::skip_deserialize<int>>(), "shall be empty");

TEST(serializer_visitor, skip_deserialize)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Logging library shall provide an annotation mechanism for data structures to support automatic "
                   "serialization/deserialization.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // S has an "added" std::vector member compared to S3;, it should be detected as incompatible
    EXPECT_FALSE((::score::common::visitor::is_payload_compatible<test::S3s, test::S>()));

    std::array<char, 1024> buffer;
    using serializer = ::score::common::visitor::logging_serializer;

    const test::S s{1, 2, 3, {4}};
    const auto size = serializer::serialize(s, buffer.data(), buffer.size());

    {  //  test signed char buffer serialization
        test::S1 s1;
        test::S2 s2;
        EXPECT_TRUE(serializer::deserialize(buffer.data(), size, s1) && s1.f1 == s.f1);
        EXPECT_TRUE(serializer::deserialize(buffer.data(), size, s2) && s2.f2 == s.f2);
    }

    {  //  test unsigned char buffer serialization
        std::array<std::uint8_t, 1024> ubuffer;
        EXPECT_EQ(serializer::serialize(s, ubuffer.data(), ubuffer.size()), size);

        test::S1 s1;
        test::S2 s2;
        EXPECT_TRUE(serializer::deserialize(ubuffer.data(), size, s1) && s1.f1 == s.f1);
        EXPECT_TRUE(serializer::deserialize(ubuffer.data(), size, s2) && s2.f2 == s.f2);
    }
}

TEST(serializer_visitor, skip_deserialize_test_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Skip deserialization in case the data to be serialized is bigger than the buffer.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    std::array<char, 4> buffer;
    using serializer = ::score::common::visitor::logging_serializer;

    const test::S s{1, 2, 3, {4}};
    const auto size = serializer::serialize(s, buffer.data(), buffer.size());
    EXPECT_EQ(size, 0);
}
