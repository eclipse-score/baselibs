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
#include "score/mw/log/configuration/configuration_file_discoverer.h"
#include "score/os/libgen.h"
#include "score/os/mocklib/stdlib_mock.h"
#include "score/os/mocklib/unistdmock.h"
#include "score/os/utils/mocklib/pathmock.h"

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

const std::string kGlobalConfigFile{"/etc/ecu_logging_config.json"};
const std::string kLocalConfigFileInPwdEtc{"etc/logging.json"};
const std::string kLocalConfigFileInPwd{"logging.json"};
const std::string kLocalConfigFileInExecPath{"/opt/app/etc/logging.json"};
const std::string kExecPath{"/opt/app/bin/app"};
const score::os::Error kError{score::os::Error::createFromErrno(0)};
const char* gKEnvConfigFilePath = "/persistent/app/logging.json";

class ConfigurationFileDiscovererFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        ON_CALL(*unistd_mock, access(_, _))
            .WillByDefault([this](const char* pathname, score::os::Unistd::AccessMode mode) {
                return this->OnAccessCall(pathname, mode);
            });

        ON_CALL(*path_mock, get_exec_path()).WillByDefault([this]() {
            return this->OnExecPathCall();
        });

        ON_CALL(*path_mock, get_parent_dir(_))
            .WillByDefault([libgen_mock = libgen_mock_pmr.get()](const std::string& path) {
                std::vector<std::string::value_type> path_mutable{path.begin(), path.end()};
                path_mutable.push_back('\0');
                return std::string{libgen_mock->dirname(path_mutable.data())};
            });

        ON_CALL(*stdlib_mock, getenv(_)).WillByDefault(::testing::Return(const_cast<char*>(gKEnvConfigFilePath)));
    }

    void TearDown() override {}

    score::cpp::expected_blank<score::os::Error> OnAccessCall(const char* pathname, score::os::Unistd::AccessMode mode)
    {
        score::cpp::expected_blank<score::os::Error> result{score::cpp::make_unexpected(kError)};

        if (mode != score::os::Unistd::AccessMode::kExists)
        {
            return result;
        }

        if (std::find(existing_files.begin(), existing_files.end(), std::string{pathname}) != existing_files.end())
        {
            result = score::cpp::expected_blank<score::os::Error>{};
        }

        return result;
    }

    score::cpp::expected<std::string, score::os::Error> OnExecPathCall() const
    {
        score::cpp::expected<std::string, score::os::Error> result{score::cpp::make_unexpected(kError)};

        if (!exec_path_shall_fail)
        {
            result = kExecPath;
        }

        return result;
    }

    void AddExistingFile(std::string path)
    {
        existing_files.push_back(path);
    }

    void SetExecPathShallFail(bool shall_fail = true)
    {
        exec_path_shall_fail = shall_fail;
    }

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();

    score::cpp::pmr::unique_ptr<os::Libgen> libgen_mock_pmr = os::Libgen::Default(memory_resource);

    score::cpp::pmr::unique_ptr<os::UnistdMock> unistd_mock_pmr = score::cpp::pmr::make_unique<os::UnistdMock>(memory_resource);
    score::cpp::pmr::unique_ptr<os::PathMock> path_mock_pmr = score::cpp::pmr::make_unique<os::PathMock>(memory_resource);
    score::cpp::pmr::unique_ptr<os::StdlibMock> stdlib_mock_pmr = score::cpp::pmr::make_unique<os::StdlibMock>(memory_resource);

    os::UnistdMock* unistd_mock = unistd_mock_pmr.get();
    os::PathMock* path_mock = path_mock_pmr.get();
    os::StdlibMock* stdlib_mock = stdlib_mock_pmr.get();

    score::cpp::pmr::unique_ptr<os::Unistd> unistd_mock_ptr = std::move(unistd_mock_pmr);
    score::cpp::pmr::unique_ptr<os::Path> path_mock_ptr = std::move(path_mock_pmr);
    score::cpp::pmr::unique_ptr<os::Stdlib> stdlib_mock_ptr = std::move(stdlib_mock_pmr);

    ConfigurationFileDiscoverer discoverer{std::move(path_mock_ptr),
                                           std::move(stdlib_mock_ptr),
                                           std::move(unistd_mock_ptr)};
    std::vector<std::string> existing_files;
    bool exec_path_shall_fail{};
};

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindGlobalConfigurationFile)
{
    RecordProperty("Requirement", "SCR-1634075");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that the discoverer shall find the global configuration file.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);

    // ... then ...
    const auto result = discoverer.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInCwdEtc)
{
    RecordProperty("Requirement", "SCR-1633294");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies that the discoverer shall find the application specific configuration file under <cwd>/etc.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInPwdEtc);
    AddExistingFile(kLocalConfigFileInPwd);

    // ... then ...
    const auto result = discoverer.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), kLocalConfigFileInPwdEtc);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInCwd)
{
    RecordProperty("Requirement", "SCR-1633294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the discoverer shall find the application specific configuration file under <cwd>.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInPwd);

    // ... then ...
    const auto result = discoverer.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), kLocalConfigFileInPwd);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInExecPath)
{
    RecordProperty("Requirement", "SCR-1633294");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies that the discoverer shall find the application specific configuration file under the binary path.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInExecPath);
    AddExistingFile(kLocalConfigFileInPwdEtc);
    AddExistingFile(kLocalConfigFileInPwd);

    // ... then ...
    const auto result = discoverer.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), kLocalConfigFileInExecPath);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInEnvPath)
{
    RecordProperty("Requirement", "SCR-1633294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the discoverer shall find the application specific configuration file under the "
                   "environment variable path.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(gKEnvConfigFilePath);

    // ... then ...
    const auto result = discoverer.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), gKEnvConfigFilePath);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInEnvPathOverrideCwdEtc)
{
    RecordProperty("Requirement", "SCR-1633294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the discoverer shall find the application specific configuration file under the "
                   "environment variable path. The <cwd>/etc. path should be ignored");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInPwdEtc);
    AddExistingFile(gKEnvConfigFilePath);

    // ... then ...
    const auto result = discoverer.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), gKEnvConfigFilePath);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInEnvPathOverrideCwd)
{
    RecordProperty("Requirement", "SCR-1633294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the discoverer shall find the application specific configuration file under the "
                   "environment variable path. The <cwd> path should be ignored");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInPwd);
    AddExistingFile(gKEnvConfigFilePath);

    // ... then ...
    const auto result = discoverer.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), gKEnvConfigFilePath);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInEnvPathOverrideExecPath)
{
    RecordProperty("Requirement", "SCR-1633294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the discoverer shall find the application specific configuration file under the "
                   "environment variable path. The binary path should be ignored");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInExecPath);
    AddExistingFile(gKEnvConfigFilePath);

    // ... then ...
    const auto result = discoverer.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), gKEnvConfigFilePath);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallReturnEmptyIfNothingExists)
{
    RecordProperty("Requirement", "SCR-1633294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that the discoverer shall return an empty list if no config file exists.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When no file exists then ...
    const auto result = discoverer.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 0);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallReturnEmptyIfExecPathFails)
{
    RecordProperty("Requirement", "SCR-1633294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that the discoverer shall return an empty list if an error occurs.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When...
    AddExistingFile(kLocalConfigFileInExecPath);
    SetExecPathShallFail();

    // ... then ...
    const auto result = discoverer.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 0);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
