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
#include "score/mw/log/configuration/target_config_reader.h"
#include "score/mw/log/configuration/configuration_file_discoverer_mock.h"
#include "score/mw/log/detail/error.h"
#include <fstream>
#include <iostream>

#include "gtest/gtest.h"

using testing::_;

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

const std::string_view kEcuConfigEcuId{"ECU1"};
const std::string_view kEcuConfigAppId{"UNKN"};
const std::string_view kDefaultConfigAppId{"NONE"};
const std::string_view kAppConfigAppId{"App1"};
const LogLevel kEcuConfigLogLevel{LogLevel::kInfo};
const LogLevel kAppConfigLogLevel{LogLevel::kError};
const std::unordered_set<LogMode> kEcuConfigLogMode{LogMode::kRemote,
                                                    LogMode::kConsole,
                                                    LogMode::kFile,
                                                    LogMode::kSystem};
const std::unordered_set<LogMode> kAppConfigLogMode{LogMode::kRemote};
const std::string_view kAppDescription{"Application One Description"};
const std::size_t kEcuConfigStackBufferSize{3000};
const std::size_t kEcuConfigRingBufferSize{4096};
const LogLevel kEcuConfigLogLevelConsole{LogLevel::kVerbose};
const std::string_view kAppConfigLogFilePath{"/var/tmp"};
const ContextLogLevelMap kCombinedContextLogLevel{{LoggingIdentifier{"DTC"}, LogLevel::kInfo},
                                                  {LoggingIdentifier{"FOO"}, LogLevel::kWarn},
                                                  {LoggingIdentifier{"vcip"}, LogLevel::kDebug},
                                                  {LoggingIdentifier{"vcom"}, LogLevel::kOff}};
const ContextLogLevelMap kEcuConfigContextLogLevel{{LoggingIdentifier{"DTC"}, LogLevel::kInfo},
                                                   {LoggingIdentifier{"FOO"}, LogLevel::kWarn},
                                                   {LoggingIdentifier{"vcip"}, LogLevel::kError},
                                                   {LoggingIdentifier{"vcom"}, LogLevel::kOff}};
const ContextLogLevelMap kContextConfigLogLevel{{LoggingIdentifier{"vcip"}, LogLevel::kError},
                                                {LoggingIdentifier{"vcom"}, LogLevel::kOff}};
const std::size_t kNumberOfSlots{8};
const std::size_t kSlotSizeBytes{1500};
const std::size_t kDatarouterUid{1038};
const bool kDynamicDatarouterIdentifiers{true};
class TargetConfigReaderFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        auto discoverer_mock = std::make_unique<ConfigurationFileDiscovererMock>();

        ON_CALL(*discoverer_mock, FindConfigurationFiles).WillByDefault([&]() {
            return configuration_file_paths_;
        });

        reader_ = std::make_unique<TargetConfigReader>(std::move(discoverer_mock));
    }

    void TearDown() override {}

    void SetConfigurationFiles(std::vector<std::string> files)
    {
        configuration_file_paths_ = files;
    }

    TargetConfigReader& GetReader()
    {
        return *reader_;
    }

    /// \brief Example config with all possible configuration settings.
    const std::string kEcuConfigFile()
    {
        return get_path("ecu_config.json");
    };
    const std::string kAppConfigFile()
    {
        return get_path("app_config.json");
    };
    const std::string kSyntaxErrorConfigFile()
    {
        return get_path("syntax_error.json");
    };
    const std::string kInvalidAppConfigFile()
    {
        return get_path("invalid_app_config.json");
    };
    const std::string kInvalidConfigFilePath()
    {
        return get_path("___nonexistent___.json");
    };
    const std::string kEmptyConfigFile()
    {
        return get_path("empty_config.json");
    };
    const std::string kCtxLevelBrokenConfigFile()
    {
        return get_path("context_level_broken_config.json");
    };
    const std::string kErrorContent()
    {
        return get_path("error-json-structure.json");
    };
    const std::string kWrongLogLevel()
    {
        return get_path("wrong-loglevel-value.json");
    };
    const std::string kWrongContextConfig()
    {
        return get_path("wrong-context-config-value.json");
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

    std::vector<std::string> configuration_file_paths_{kEcuConfigFile(), kAppConfigFile()};
    std::unique_ptr<TargetConfigReader> reader_;
};

TEST_F(TargetConfigReaderFixture, NoConfigFilesShallFail)
{
    RecordProperty("Requirement", "SCR-7263537, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TargetConfigReader shall return an error if no configuration files are found");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // For the case that no configuration files exist.
    SetConfigurationFiles({});

    // ReadConfig shall return an error.
    EXPECT_EQ(GetReader().ReadConfig().error(), Error::kConfigurationFilesNotFound);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseEcuIdFromEcuConfig)
{
    RecordProperty("Requirement", "SCR-1633316, SCR-1634075");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader shall parse the DLT ECU ID from the configuration file correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetEcuId(), kEcuConfigEcuId);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseAppIdFromAppConfig)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader shall parse the DLT Application ID from the configuration file correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetAppId(), kAppConfigAppId);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseLogLevelFromAppConfig)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "TargetConfigReader shall parse the DLT Application log level from the configuration file correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetDefaultLogLevel(), kAppConfigLogLevel);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseLogModeFromAppConfig)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader shall parse the logging mode from the configuration file correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetLogMode(), kAppConfigLogMode);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseAppDescriptionFromAppConfig)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader shall parse the application description from the configuration file correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetAppDescription(), kAppDescription);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseBufferOverwriteOnFullStatusFromAppConfig)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "TargetConfigReader shall parse the overwrite ring buffer option from the configuration file correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_TRUE(GetReader().ReadConfig()->GetRingBufferOverwriteOnFull());
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseStackBufferSizeFromEcuConfig)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "TargetConfigReader shall parse the stack buffer size in shared memory from the configuration file correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetStackBufferSize(), kEcuConfigStackBufferSize);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseRingBufferSizeFromEcuConfig)
{
    RecordProperty("Requirement", "SCR-1633316, SCR-1634075");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "TargetConfigReader shall parse the ring buffer size in shared memory from the configuration file correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetRingBufferSize(), kEcuConfigRingBufferSize);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseLogLevelConsoleFromEcuConfig)
{
    RecordProperty("Requirement", "SCR-1633316, SCR-1634075");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader shall parse the default log level threshold for console logging from the "
                   "configuration file correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetDefaultConsoleLogLevel(), kEcuConfigLogLevelConsole);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseLogFilePathFromAppConfig)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader shall parse the default log file path for console logging from the "
                   "configuration file correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetLogFilePath(), kAppConfigLogFilePath);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseContextLogLevelFromEcuAndAppConfig)
{
    RecordProperty("Requirement", "SCR-1633316, SCR-1634075");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader shall parse and combine the context log levels from the "
                   "configuration files correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetContextLogLevel(), kCombinedContextLogLevel);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseNumberOfSlots)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TargetConfigReader shall parse the number of slots for preallocation correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetNumberOfSlots(), kNumberOfSlots);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseSlotSizeBytes)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TargetConfigReader shall parse the size of the slots for preallocation correctly.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetSlotSizeInBytes(), kSlotSizeBytes);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseDynamicDatarouterIdentifiers)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader shall parse if datarouter dyanmic identifiers flag is enabled or not.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetDynamicDatarouterIdentifiers(), kDynamicDatarouterIdentifiers);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallParseDataRouterUid)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TargetConfigReader shall parse datarouter user ID.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_EQ(GetReader().ReadConfig()->GetDataRouterUid(), kDatarouterUid);
}

TEST_F(TargetConfigReaderFixture, AppConfigSyntaxErrorShallFallbackToEcuConfig)
{
    RecordProperty("Requirement", "SCR-7263547");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "TargetConfigReader fall back to the ECU config file if the application config files contains syntax errors.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // The application config contains a syntax error.
    SetConfigurationFiles({kEcuConfigFile(), kSyntaxErrorConfigFile()});

    // ReadConfig shall still return the value from the ECU config.
    EXPECT_EQ(GetReader().ReadConfig()->GetAppId(), kEcuConfigAppId);
}

TEST_F(TargetConfigReaderFixture, WrongStructureConfigFileShallCauseDefaultAppId)
{
    RecordProperty("Requirement", "SCR-7263547");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader fall back to the hard-coded default application id if the configuration file "
                   "does not contain a valid JSON structure");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // The application config has wrong structure.
    SetConfigurationFiles({kErrorContent()});

    // ReadConfig shall return the default value.
    EXPECT_EQ(GetReader().ReadConfig()->GetAppId(), kDefaultConfigAppId);
}

TEST_F(TargetConfigReaderFixture, WrongLogLevelValueShallFallbackToEcuConfig)
{
    RecordProperty("Requirement", "SCR-7263547");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader fall back to the ECU config file if the if the logLevelThresholdConsole "
                   "has wrong value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // The application config has wrong structure.
    SetConfigurationFiles({kWrongLogLevel()});

    // ReadConfig shall return the default value.
    EXPECT_EQ(GetReader().ReadConfig()->GetAppId(), kEcuConfigAppId);
}

TEST_F(TargetConfigReaderFixture, AppConfigInvalidLogLevelFallbackToEcuConfig)
{
    RecordProperty("Requirement", "SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader fall back to the valid value from the ECU configuration file if the application "
                   "config file contains an invalid log level.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // The application config contains invalid default log level.
    SetConfigurationFiles({kEcuConfigFile(), kInvalidAppConfigFile()});

    // ReadConfig shall still return the value from the ECU config.
    EXPECT_EQ(GetReader().ReadConfig()->GetDefaultLogLevel(), kEcuConfigLogLevel);
}

TEST_F(TargetConfigReaderFixture, AppConfigInvalidLogModeFallbackToEcuConfig)
{
    RecordProperty("Requirement", "SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader fall back to the valid value from the ECU configuration file if the application "
                   "config file contains an invalid log mode.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // The application config contains invalid log mode.
    SetConfigurationFiles({kEcuConfigFile(), kInvalidAppConfigFile()});

    // ReadConfig shall still return the value from the ECU config.
    EXPECT_EQ(GetReader().ReadConfig()->GetLogMode(), kEcuConfigLogMode);
}

TEST_F(TargetConfigReaderFixture, AppConfigInvalidContextConfigFallbackToEcuConfig)
{
    RecordProperty("Requirement", "SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader fall back to the valid value from the ECU configuration file if the application "
                   "config file contains an invalid log level for a context.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // The application config contains invalid context log level entries.
    SetConfigurationFiles({kEcuConfigFile(), kInvalidAppConfigFile()});

    // ReadConfig shall still return the value from the ECU config.
    EXPECT_EQ(GetReader().ReadConfig()->GetContextLogLevel(), kEcuConfigContextLogLevel);
}

TEST_F(TargetConfigReaderFixture, WrongEntriesToContextConfigslShallReturnEmptyContextLogLevel)
{
    RecordProperty("Requirement", "SCR-7263548");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader shall return empty context config log level when providing wrong entries.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // The application config contains invalid context config entries.
    SetConfigurationFiles({kWrongContextConfig()});

    // ReadConfig shall returns empty context.
    EXPECT_TRUE(GetReader().ReadConfig()->GetContextLogLevel().empty());
}

TEST_F(TargetConfigReaderFixture, WhenInvalidFilePathReaderShallReturnDefaultAppId)
{
    RecordProperty("Requirement", "SCR-7263537, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader fall back to the hard-coded default application id if the configuration file "
                   "does not exist");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // The application config does not exist
    SetConfigurationFiles({kInvalidConfigFilePath()});

    // ReadConfig shall return the default value.
    EXPECT_EQ(GetReader().ReadConfig()->GetAppId(), kDefaultConfigAppId);
}

TEST_F(TargetConfigReaderFixture, EmptyConfigFileShallCauseDefaultAppId)
{
    RecordProperty("Requirement", "SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader fall back to the hard-coded default application id if the configuration file "
                   "does not contain any value");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // The application config does not exist
    SetConfigurationFiles({kEmptyConfigFile()});

    // ReadConfig shall return the default value.
    EXPECT_EQ(GetReader().ReadConfig()->GetAppId(), kDefaultConfigAppId);
}

TEST_F(TargetConfigReaderFixture, ConfigReaderShallFallBackToContextLogLevelDefaultWhenNoValidValueInConfigurationFiles)
{
    RecordProperty("Requirement", "SCR-7263548, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "TargetConfigReader fall back to the hard-coded default context log level values if there is no "
                   "valid value in the configuration files");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    SetConfigurationFiles({kCtxLevelBrokenConfigFile()});

    EXPECT_EQ(GetReader().ReadConfig()->GetContextLogLevel(), ContextLogLevelMap{});
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
