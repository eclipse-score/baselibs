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
#include "score/mw/log/detail/file_recorder/dlt_message_builder.h"
#include "score/mw/log/detail/file_recorder/dlt_message_builder_types.h"

#include "gtest/gtest.h"
#include <gmock/gmock.h>

#include <arpa/inet.h>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{

using ::testing::_;
using ::testing::HasSubstr;
using ::testing::MatchesRegex;
using ::testing::StrEq;

const std::map<std::uint8_t, std::string> levels = {
    {0, "off"},
    {1, "fatal"},
    {2, "error"},
    {3, "warn"},
    {4, "info"},
    {5, "debug"},
    {6, "verbose"},
    {7, "undefined"},
};

class DltMessageBuilderFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        auto& log_entry = log_record_.getLogEntry();

        log_entry.app_id = LoggingIdentifier{"TMB"};
        log_entry.ctx_id = LoggingIdentifier{"CTX"};
        log_entry.num_of_args = 7;
        log_entry.log_level = LogLevel::kWarn;
        log_entry.payload = ByteVector{'p', 'a', 'y', 'l', 'o', 'a', 'd'};
    }
    void TearDown() override {}

  protected:
    DltMessageBuilder unit_{"XECU"};
    LogRecord log_record_;
};

TEST_F(DltMessageBuilderFixture, ShallDepleteAfterHeaderAndPayload)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00405");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Dlt will deplete after header and payload.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    unit_.SetNextMessage(log_record_);
    const auto first = unit_.GetNextSpan();
    EXPECT_TRUE(first.has_value());
    const auto second = unit_.GetNextSpan();
    EXPECT_TRUE(second.has_value());
    const auto end = unit_.GetNextSpan();
    EXPECT_FALSE(end.has_value());
}

TEST_F(DltMessageBuilderFixture, StorageHeaderShallHaveSpecificElements)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00405");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Dlt Storage Header format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    unit_.SetNextMessage(log_record_);

    const auto header_span = unit_.GetNextSpan().value();
    DltStorageHeader storage_header{};
    std::memcpy(&storage_header, header_span.data(), sizeof(storage_header));

    const std::array<std::uint8_t, kDltIdSize> dlt_pattern{'D', 'L', 'T', '\1'};
    EXPECT_EQ(storage_header.pattern, dlt_pattern);
    const std::array<std::uint8_t, kDltIdSize> dlt_ecu{'E', 'C', 'U', '\0'};
    EXPECT_EQ(storage_header.ecu, dlt_ecu);
}

TEST_F(DltMessageBuilderFixture, HeaderShallHaveSpecificElements)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00458, PRS_Dlt_00319, PRS_Dlt_00094");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Dlt Header format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    unit_.SetNextMessage(log_record_);

    DltVerboseHeader verbose_header{};

    const auto header_span = unit_.GetNextSpan().value();
    std::memcpy(&verbose_header, header_span.data() + sizeof(DltStorageHeader), sizeof(verbose_header));

    EXPECT_EQ(verbose_header.standard.len, htons(29UL));
    EXPECT_EQ(verbose_header.standard.mcnt, 0UL);

    const std::array<std::uint8_t, kDltIdSize> ecu{'X', 'E', 'C', 'U'};
    EXPECT_EQ(verbose_header.extra.ecu, ecu);
}

TEST_F(DltMessageBuilderFixture, ExtendedHeaderShallHaveSpecificElements)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00617");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Dlt Extended Header format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    unit_.SetNextMessage(log_record_);

    DltVerboseHeader verbose_header{};

    const auto header_span = unit_.GetNextSpan().value();
    std::memcpy(&verbose_header, header_span.data() + sizeof(DltStorageHeader), sizeof(verbose_header));

    const std::array<std::uint8_t, kDltIdSize> app_id{'T', 'M', 'B', '\0'};
    EXPECT_EQ(verbose_header.extended.apid, app_id);
    const std::array<std::uint8_t, kDltIdSize> ctx_id{'C', 'T', 'X', '\0'};
    EXPECT_EQ(verbose_header.extended.ctid, ctx_id);
}

TEST_F(DltMessageBuilderFixture, PayloadShallHaveSetText)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00459");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Dlt message format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    unit_.SetNextMessage(log_record_);

    std::ignore = unit_.GetNextSpan();
    const auto payload_span = unit_.GetNextSpan().value();
    const auto string_content =
        std::string(reinterpret_cast<const char*>(payload_span.data()), static_cast<std::size_t>(payload_span.size()));
    EXPECT_THAT(string_content, StrEq("payload"));
}

TEST(DltMessageBuilderFunctionTest, TestDisableDltExtendedHeader)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00459");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies DLT standard header types without the exteded header enabled.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    score::mw::log::detail::DltVerboseHeader dlt_header;
    constexpr std::uint16_t kMessageSize{0};
    constexpr std::uint8_t kMessageCount{0};
    constexpr bool kUseExtendedHeader = false;

    // When disable the extended header.
    score::mw::log::detail::ConstructDltStandardHeaderTypes(
        dlt_header.standard, kMessageSize, kMessageCount, kUseExtendedHeader);

    // The DLT header type should not contain extended header parameter enabled.
    constexpr auto kHeaderType = kDltHtypWEID | kDltHtypWTMS | kDltHtypVERS;
    EXPECT_EQ(dlt_header.standard.htyp, kHeaderType);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
