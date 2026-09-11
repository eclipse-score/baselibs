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

#include "score/string_manipulation/arguments/arguments.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(GetArguments, ReturnsEmptyVectorWhenNoArguments)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__argument_conversion");
    RecordProperty("Description", "Check that an empty argument list produces an empty vector of string views.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    constexpr int kArgc = 0;
    const char* argv[] = {nullptr};
    auto arguments = score::string_manipulation::GetArguments(kArgc, argv);
    EXPECT_THAT(arguments, ::testing::IsEmpty());
}

TEST(GetArguments, ReturnsVectorWithProvidedArguments)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__argument_conversion");
    RecordProperty("Description", "Check that supplied command-line arguments are preserved in order as string views.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    constexpr int kArgc = 2;
    const char* argv[] = {"first", "second", nullptr};
    auto arguments = score::string_manipulation::GetArguments(kArgc, argv);
    EXPECT_THAT(arguments.size(), ::testing::Eq(2));
    EXPECT_THAT(arguments,
                ::testing::ElementsAre(score::safecpp::zstring_view{"first"}, score::safecpp::zstring_view{"second"}));
}
