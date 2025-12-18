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
#include "score/os/getopt.h"

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{

TEST(GetOptTest, GetoptTestSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetOptTest Getopt Test Success");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    optind = 1;
    char program_name[] = "GetoptSuccess";

    char arg1[] = "-a";
    char arg2[] = "ananas";
    char* argv[] = {program_name, arg1, arg2, nullptr};
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    EXPECT_EQ(score::os::Getopt::instance().getoptind(), 1);

    auto ret = score::os::Getopt::instance().getopt(argc, argv, "a:");
    EXPECT_EQ(ret, 'a');
    EXPECT_STREQ(optarg, "ananas");

    EXPECT_EQ(optind, 3);
}

TEST(GetOptTest, GetOptLong)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetOptTest Get Opt Long");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    optind = 1;
    char program_name[] = "GetOptLong";
    char arg1[] = "-a";
    char arg2[] = "ananas";
    char arg3[] = "-b";
    char arg4[] = "bananas";
    char* argv[] = {program_name, arg1, arg2, arg3, arg4, nullptr};
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    static struct option longopts[] = {
        {"ananas", required_argument, nullptr, 'a'},
        {"bananas", required_argument, nullptr, 'b'},
        {nullptr, 0, nullptr, 0}  // The last element has to be all zeros
    };

    std::int32_t longindex;
    std::int32_t ret = 0;
    while ((ret = score::os::Getopt::instance().getopt_long(argc, argv, "a:b:", longopts, &longindex)) != -1)
    {
        switch (ret)
        {
            case 'a':
                EXPECT_STREQ(optarg, "ananas");
                break;
            case 'b':
                EXPECT_STREQ(optarg, "bananas");
                break;
            default:
                // Unexpected return value
                FAIL() << "Unexpected return value: " << ret;
                break;
        }
    }
    ASSERT_EQ(optind, argc);
}

TEST(GetOptTest, GetoptTestFailureUnknownOption)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetOptTest Getopt Test Failure Unknown Option");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    optind = 1;
    char program_name[] = "GetoptSuccess";

    char arg1[] = "-x";
    char* argv[] = {program_name, arg1, nullptr};
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    auto ret = score::os::Getopt::instance().getopt(argc, argv, "a:");
    EXPECT_EQ(ret, '?');
    EXPECT_EQ(optopt, 'x');

    EXPECT_EQ(optind, 2);
}

TEST(GetOptTest, GetoptindTest)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetOptTest Getoptind Test");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    optind = 4;
    EXPECT_EQ(score::os::Getopt::instance().getoptind(), 4);
}

TEST(GetOptTest, GetopterrTest)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetOptTest Getopterr Test");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    opterr = 5;
    EXPECT_EQ(score::os::Getopt::instance().getopterr(), 5);
}

TEST(GetOptTest, GetoptoptTest)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetOptTest Getoptopt Test");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    optopt = 3;
    EXPECT_EQ(score::os::Getopt::instance().getoptopt(), 3);
}

}  // namespace
}  // namespace os
}  // namespace score
