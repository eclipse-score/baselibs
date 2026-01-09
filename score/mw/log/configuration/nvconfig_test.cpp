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
#include "nvconfig.h"
#include "nvconfigfactory.h"
#include <fstream>
#include <iostream>

#include "score/mw/log/log_level.h"

#include "gtest/gtest.h"

namespace
{

using NvConfig = score::mw::log::NvConfig;
using NvConfigFactory = score::mw::log::NvConfigFactory;
using INvConfig = score::mw::log::INvConfig;

class NonVerboseConfig : public ::testing::Test
{
  public:
    const std::string JSON_PATH()
    {
        return get_path("test-class-id.json");
    };
    const std::string JSON_PATH_2()
    {
        return get_path("second-test-class-id.json");
    };
    const std::string EMPTY_FILE()
    {
        return get_path("empty-class-id.json");
    };
    const std::string EMPTY_JSON_OBJECT()
    {
        return get_path("error-parse-empty-json-object.json");
    };
    const std::string WRONG_JSON_PATH()
    {
        return get_path("wrong-path-class-id.json");
    };
    const std::string EMPTY_JSON()
    {
        return get_path("empty-json-class-id.json");
    };
    const std::string ERROR_PARSE_1_PATH()
    {
        return get_path("error-parse-1-json-class-id.json");
    };
    const std::string ERROR_CONTENT_1_PATH()
    {
        return get_path("error-content-1-json-class-id.json");
    };
    const std::string ERROR_CONTENT_2_PATH()
    {
        return get_path("error-content-2-json-class-id.json");
    };
    const std::string ERROR_CONTENT_3_PATH()
    {
        return get_path("error-content-3-json-class-id.json");
    };
    const std::string ERROR_CONTENT_WRONG_ID_VALUE()
    {
        return get_path("error-content-wrong-id-value.json");
    };

  private:
    const std::string get_path(const std::string& file_name)
    {
        const std::string default_path = "score/mw/log/configuration/test/data/" + file_name;

        std::ifstream file(default_path);
        if (file.is_open())
        {
            file.close();
            return default_path;
        }
        else
        {
            return "external/safe_posix_platform/" + default_path;
        }
    }
};

TEST_F(NonVerboseConfig, NvConfigReturnsExpectedValues)
{
    RecordProperty("Requirement", "SCR-1633147");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Logging libraries use static configuration based on .json files.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    auto result = NvConfigFactory::CreateAndInit(JSON_PATH());
    ASSERT_TRUE(result.has_value());  // Verify config was created successfully
    auto& nvc1 = result.value();
    EXPECT_EQ(301,
              nvc1.getDltMsgDesc("score::logging::PersistentLogFileEvent")->GetIdMsgDescriptor());  // id value reading
    EXPECT_EQ(
        1,
        static_cast<int>(
            nvc1.getDltMsgDesc("score::logging::PersistentLogFileEvent")->GetLogLevel()));  // loglevel value reading
    EXPECT_EQ(4,
              static_cast<int>(nvc1.getDltMsgDesc("poseng::logging::ReprocessingCycle")
                                   ->GetLogLevel()));  // loglevel value reading when using default loglevel
    EXPECT_EQ(
        std::string_view{"Repr"},
        nvc1.getDltMsgDesc("aas::logging::ReprocessingCycle")->GetCtxId().GetStringView());  // ctxid value reading
    EXPECT_EQ(
        std::string_view{"PE"},
        nvc1.getDltMsgDesc("poseng::logging::ReprocessingCycle")->GetAppId().GetStringView());  // appid value reading
    EXPECT_EQ(std::string_view{"PERL"},
              nvc1.getDltMsgDesc("score::logging::PersistentLogFileEvent")
                  ->GetCtxId()
                  .GetStringView());  // ctxid value reading other example
}

TEST_F(NonVerboseConfig, NvConfigReturnsExpectedValuesWithOtherFile)
{
    RecordProperty("Requirement", "SCR-1633147");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Logging libraries use static configuration based on .json files.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto result = NvConfigFactory::CreateAndInit(JSON_PATH_2());
    ASSERT_TRUE(result.has_value());  // Verify config was created successfully
    auto& nvc2 = result.value();
    EXPECT_EQ(8650816, nvc2.getDltMsgDesc("score::plg::awa::DebugData")->GetIdMsgDescriptor());  // id value reading
    EXPECT_EQ(
        3,
        static_cast<int>(nvc2.getDltMsgDesc("adp::logging::DynamicInsight")->GetLogLevel()));  // loglevel value reading
    EXPECT_EQ(2,
              static_cast<int>(nvc2.getDltMsgDesc("score::logging::timesync::DltTimeSyncTimestamp")
                                   ->GetLogLevel()));  // loglevel value reading other example
    EXPECT_EQ(8650814,
              nvc2.getDltMsgDesc("score::pcp::CrocStateTraceable")
                  ->GetIdMsgDescriptor());  // id value reading other example
    EXPECT_EQ(
        std::string_view{"Repr"},
        nvc2.getDltMsgDesc("aas::logging::ReprocessingEvent")->GetCtxId().GetStringView());  // ctxid value reading
    EXPECT_EQ(std::string_view{"Fasi"},
              nvc2.getDltMsgDesc("score::sli::TsfBaseConfig")->GetAppId().GetStringView());  // appid value reading
    EXPECT_EQ(std::string_view{"DTNV"},
              nvc2.getDltMsgDesc("score::plg::driving_tube::DiagnosticLogsData")
                  ->GetCtxId()
                  .GetStringView());  // ctxid value reading other example
    EXPECT_EQ(std::string_view{"Plan"},
              nvc2.getDltMsgDesc("score::plg::driving_tube::DiagnosticLogsData")
                  ->GetAppId()
                  .GetStringView());  // appid value reading other example
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorOpenWhenGivenEmptyFile)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the inability of parsing a general empty file.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto result = NvConfigFactory::CreateAndInit(EMPTY_FILE());
    ASSERT_FALSE(result.has_value());  // error parse because it is a general empty file
    EXPECT_EQ(static_cast<uint8_t>(score::mw::log::NvConfigErrorCode::kParseError), *result.error());
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorOpenWhenGivenPathToNonExistentFile)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263537");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "ara::log shall discard configuration files that are not found.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto result = NvConfigFactory::CreateAndInit(WRONG_JSON_PATH());
    ASSERT_FALSE(result.has_value());  // error parse because the file doesn't exist
    EXPECT_EQ(static_cast<uint8_t>(score::mw::log::NvConfigErrorCode::kParseError), *result.error());
}

TEST_F(NonVerboseConfig, NvConfigReturnsOkWhenGivenEmptyJsonFile)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of parsing an empty JSON file.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto result = NvConfigFactory::CreateAndInit(EMPTY_JSON());
    EXPECT_TRUE(result.has_value());  // ok because this json file doesn't have items
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorParseIfEmptyObject)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies the inability of parsing JSON file that has array instead of JSON object as a root node.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto result = NvConfigFactory::CreateAndInit(EMPTY_JSON_OBJECT());
    ASSERT_FALSE(result.has_value());  // array instead of json object as one of the values
    EXPECT_EQ(static_cast<uint8_t>(score::mw::log::NvConfigErrorCode::kParseError), *result.error());
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorParseIfThereIsSomethingElseInstedOfObjectAsValue)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the inability of parsing a JSON file that does not have object as value.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto result = NvConfigFactory::CreateAndInit(ERROR_PARSE_1_PATH());
    ASSERT_FALSE(result.has_value());  // aray instead of json object as one of the values
    EXPECT_EQ(static_cast<uint8_t>(score::mw::log::NvConfigErrorCode::kParseError), *result.error());
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorContentIfCtxidValuePairDoesntExistsForOneObject)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies the inability of parsing JSON file that misses ctxid key-value pair for one of the objects.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto result = NvConfigFactory::CreateAndInit(ERROR_CONTENT_1_PATH());
    ASSERT_FALSE(result.has_value());  // ctxid key-value pair is missing in one of the objects
    EXPECT_EQ(static_cast<uint8_t>(score::mw::log::NvConfigErrorCode::kContentError), *result.error());
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorContentIfAppidValuePairDoesntExistsForOneObject)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies the inability of parsing JSON file that misses appid key-value pair for one of the objects.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto result = NvConfigFactory::CreateAndInit(ERROR_CONTENT_2_PATH());
    ASSERT_FALSE(result.has_value());  // appid key-value pair is missing in one of the objects
    EXPECT_EQ(static_cast<uint8_t>(score::mw::log::NvConfigErrorCode::kContentError), *result.error());
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorContentIfIdValuePairDoesntExistsForOneObject)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies the inability of parsing JSON file that misses id key-value pair for one of the objects.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto result = NvConfigFactory::CreateAndInit(ERROR_CONTENT_3_PATH());
    ASSERT_FALSE(result.has_value());  // id key-value pair is missing in one of the objects
    EXPECT_EQ(static_cast<uint8_t>(score::mw::log::NvConfigErrorCode::kContentError), *result.error());
}

TEST_F(NonVerboseConfig, NvConfigReturnsErrorIfIdDataTypeIsWrong)
{
    RecordProperty("Requirement", "SCR-1633147, SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the inability of parsing JSON file that has wrong id value.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto result = NvConfigFactory::CreateAndInit(ERROR_CONTENT_WRONG_ID_VALUE());
    ASSERT_FALSE(result.has_value());  // wrong ID data type (string instead of int).
    EXPECT_EQ(static_cast<uint8_t>(score::mw::log::NvConfigErrorCode::kContentError), *result.error());
}

}  // namespace
