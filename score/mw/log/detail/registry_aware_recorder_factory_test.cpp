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
#include "score/mw/log/detail/registry_aware_recorder_factory.h"

#include "score/mw/log/backend_table.h"
#include "score/mw/log/configuration/target_config_reader_mock.h"
#include "score/mw/log/detail/composite_recorder.h"
#include "score/mw/log/detail/empty_recorder.h"
#include "score/mw/log/detail/error.h"
#include "score/mw/log/detail/irecorder_factory.h"
#include "score/mw/log/detail/text_recorder/text_recorder.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <score/utility.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <type_traits>

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

template <typename ConcreteRecorder>
bool IsRecorderOfType(const std::unique_ptr<Recorder>& recorder) noexcept
{
    static_assert(std::is_base_of<Recorder, ConcreteRecorder>::value,
                  "Concrete recorder shall be derived from Recorder base class");
    return dynamic_cast<const ConcreteRecorder*>(recorder.get()) != nullptr;
}

template <typename ConcreteRecorder>
bool ContainsRecorderOfType(const CompositeRecorder& composite) noexcept
{
    for (const auto& recorder : composite.GetRecorders())
    {
        if (IsRecorderOfType<ConcreteRecorder>(recorder))
        {
            return true;
        }
    }
    return false;
}

class RegistryAwareRecorderFactoryTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Save original table state so console_registrant's static registration persists.
        saved_table_ = gBackendCreators;
    }

    void TearDown() override
    {
        // Restore the original table state.
        gBackendCreators = saved_table_;
    }

    std::array<RecorderCreatorFn, kMaxBackendSlots> saved_table_{};
};

class RegistryAwareRecorderFactoryConfigFixture : public RegistryAwareRecorderFactoryTest
{
  public:
    void SetUp() override
    {
        RegistryAwareRecorderFactoryTest::SetUp();
        score::cpp::ignore = unsetenv("MW_LOG_CONFIG_FILE");
    }

    void TearDown() override
    {
        score::cpp::ignore = unsetenv("MW_LOG_CONFIG_FILE");
        if (!temp_path_.empty())
        {
            score::cpp::ignore = std::remove(temp_path_.c_str());
            temp_path_.clear();
        }
        RegistryAwareRecorderFactoryTest::TearDown();
    }

    std::unique_ptr<Recorder> CreateFromConfiguration() noexcept
    {
        auto config_reader_mock = std::make_unique<TargetConfigReaderMock>();
        ON_CALL(*config_reader_mock, ReadConfig).WillByDefault(testing::Invoke([&]() {
            return config_result_;
        }));
        return RegistryAwareRecorderFactory{}.CreateFromConfiguration(std::move(config_reader_mock),
                                                                      score::cpp::pmr::get_default_resource());
    }

    void SetTargetConfigReaderResult(score::Result<Configuration> result) noexcept
    {
        config_result_ = result;
    }

    void SetConfigurationWithLogMode(const std::unordered_set<LogMode>& log_modes,
                                     Configuration config = Configuration{}) noexcept
    {
        config.SetLogMode(log_modes);
        SetTargetConfigReaderResult(config);
    }

    void SetupTempConfigFile(const std::string& json_content)
    {
        temp_path_ = "/tmp/registry_aware_recorder_factory_test.json";
        std::ofstream file(temp_path_);
        file << json_content;
        file.close();
        score::cpp::ignore = setenv("MW_LOG_CONFIG_FILE", temp_path_.c_str(), 1);
    }

  protected:
    score::Result<Configuration> config_result_;
    std::string temp_path_;
};

TEST(CreateRecorderFactoryTest, CreateRecorderFactoryReturnsNonNullFactory)
{
    RecordProperty("Description", "CreateRecorderFactory shall return a valid IRecorderFactory pointer.");
    RecordProperty("TestingTechnique", "Interface-based test");

    auto factory = CreateRecorderFactory();

    EXPECT_NE(factory, nullptr);
}

TEST(CreateRecorderFactoryTest, CreateStubReturnsEmptyRecorder)
{
    RecordProperty("Description", "CreateRecorderFactory()->CreateStub() shall return an EmptyRecorder.");
    RecordProperty("TestingTechnique", "Interface-based test");

    auto factory = CreateRecorderFactory();
    auto recorder = factory->CreateStub();

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryTest, CreateWithConsoleLoggingOnlyReturnsRecorder)
{
    RecordProperty("Description",
                   "CreateWithConsoleLoggingOnly shall return a valid Recorder when console backend is registered.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    // Console backend is registered via the static BackendRegistrant in console_registrant.cpp
    // which is linked as part of the :minimal target.
    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateWithConsoleLoggingOnly(score::cpp::pmr::get_default_resource());

    EXPECT_NE(recorder, nullptr);

    if (IsBackendAvailable(LogMode::kConsole))
    {
        EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
    }
    else
    {
        EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
    }
}

TEST_F(RegistryAwareRecorderFactoryTest, CreateWithConsoleLoggingOnlyWithNullResourceFallsBackToDefault)
{
    RecordProperty("Description", "CreateWithConsoleLoggingOnly shall use default memory resource when given nullptr.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateWithConsoleLoggingOnly(nullptr);

    EXPECT_NE(recorder, nullptr);

    if (IsBackendAvailable(LogMode::kConsole))
    {
        EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
    }
    else
    {
        EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
    }
}

TEST_F(RegistryAwareRecorderFactoryTest, CreateWithConsoleLoggingOnlyFallsBackToEmptyIfConsoleNotRegistered)
{
    RecordProperty("Description",
                   "CreateWithConsoleLoggingOnly shall return EmptyRecorder if console backend is not registered.");
    RecordProperty("TestingTechnique", "Boundary test");

    // Clear the backend table to simulate no console backend.
    gBackendCreators.fill(nullptr);

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateWithConsoleLoggingOnly(score::cpp::pmr::get_default_resource());

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryTest, CreateStubReturnsEmptyRecorder)
{
    RecordProperty("Description", "CreateStub shall return an EmptyRecorder.");
    RecordProperty("TestingTechnique", "Interface-based test");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateStub();

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryTest, CreateFromConfigurationFallsBackToConsoleWhenMemoryResourceIsNull)
{
    RecordProperty("Description",
                   "CreateFromConfiguration shall fall back to console logging when given a null memory resource.");
    RecordProperty("TestingTechnique", "Boundary test");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateFromConfiguration(nullptr);

    EXPECT_NE(recorder, nullptr);
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture, ConfigurationWithConfigReaderMemoryErrorShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will return empty recorder in case of configuration error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    auto config_reader_mock = std::make_unique<TargetConfigReaderMock>();
    score::cpp::pmr::memory_resource* null_memory_resource = nullptr;
    auto recorder =
        RegistryAwareRecorderFactory{}.CreateFromConfiguration(std::move(config_reader_mock), null_memory_resource);
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture, ConfigurationErrorShallFallbackToConsoleLogging)
{
    RecordProperty("Requirement", "SCR-861534, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will return text recorder in case of configuration error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    SetTargetConfigReaderResult(score::MakeUnexpected(Error::kConfigurationFilesNotFound));
    auto recorder = CreateFromConfiguration();

    if (IsBackendAvailable(LogMode::kConsole))
    {
        EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
    }
    else
    {
        EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
    }
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture, NoRecorderConfiguredShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can return empty recorder if no recorder is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    SetConfigurationWithLogMode({});

    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture, ConsoleConfiguredShallReturnConsoleRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can create ConsoleRecorder if console is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    SetConfigurationWithLogMode({LogMode::kConsole});
    auto recorder = CreateFromConfiguration();

    if (IsBackendAvailable(LogMode::kConsole))
    {
        EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
    }
    else
    {
        EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
    }
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture, InvalidLogModeShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will create EmptyRecorder in case of invalid log mode.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    SetConfigurationWithLogMode({LogMode::kInvalid});
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture, OutOfRangeLogModeShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will create EmptyRecorder in case of out of range log mode.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    const LogMode out_of_range_log_mode = static_cast<LogMode>(240);

    SetConfigurationWithLogMode({out_of_range_log_mode});
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture, MultipleLogModesShallReturnCompositeRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "RecorderFactory shall create CompositeRecorder in case of multiple log mode is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    // Clear and register two fake backends to be independent of flag combinations.
    gBackendCreators.fill(nullptr);

    RegisterBackend(
        LogMode::kFile,
        [](const Configuration& /*config*/, score::cpp::pmr::memory_resource* /*mem*/) -> std::unique_ptr<Recorder> {
            return std::make_unique<EmptyRecorder>();
        });

    RegisterBackend(
        LogMode::kConsole,
        [](const Configuration& /*config*/, score::cpp::pmr::memory_resource* /*mem*/) -> std::unique_ptr<Recorder> {
            return std::make_unique<EmptyRecorder>();
        });

    SetConfigurationWithLogMode({LogMode::kFile, LogMode::kConsole});
    auto recorder = CreateFromConfiguration();
    ASSERT_TRUE(IsRecorderOfType<CompositeRecorder>(recorder));
    const auto& composite_recorder = *dynamic_cast<CompositeRecorder*>(recorder.get());

    EXPECT_EQ(composite_recorder.GetRecorders().size(), config_result_->GetLogMode().size());
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture, CreateRecorderFromLogModeMemoryErrorShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Recorder created using LogMode will return empty recorder in case of memory error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    score::cpp::pmr::memory_resource* null_memory_resource = nullptr;
    Configuration config{};
    auto recorder =
        RegistryAwareRecorderFactory{}.CreateRecorderFromLogMode(LogMode::kConsole, config, null_memory_resource);
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture, CreateFromConfigurationFallsBackToConsoleWhenConfigFilesNotFound)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "CreateFromConfiguration(memory_resource) falls back to console when no config files are found.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    // MW_LOG_CONFIG_FILE is unset and no default config files exist in the test sandbox.
    // This exercises the !result.has_value() TRUE branch -> CreateWithConsoleLoggingOnly.
    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateFromConfiguration(score::cpp::pmr::get_default_resource());

    EXPECT_NE(recorder, nullptr);
    if (IsBackendAvailable(LogMode::kConsole))
    {
        EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
    }
    else
    {
        EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
    }
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture,
       CreateFromConfigurationWithConsoleModeParsesConfigAndReturnsSingleRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "CreateFromConfiguration creates a single console recorder when kConsole mode is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    SetupTempConfigFile(R"({"logMode": "kConsole"})");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateFromConfiguration(score::cpp::pmr::get_default_resource());

    EXPECT_NE(recorder, nullptr);
    if (IsBackendAvailable(LogMode::kConsole))
    {
        EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
    }
    else
    {
        EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
    }
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture,
       CreateFromConfigurationWithUnavailableModeConsoleAvailableFallsBackToConsole)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "CreateFromConfiguration falls back to console when the requested log mode is unavailable.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    // kFile is not registered in this test binary, so IsBackendAvailable(kFile) returns false.
    gBackendCreators[ModeToSlotIndex(LogMode::kFile)] = nullptr;

    SetupTempConfigFile(R"({"logMode": "kFile"})");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateFromConfiguration(score::cpp::pmr::get_default_resource());

    EXPECT_NE(recorder, nullptr);
    EXPECT_FALSE(IsBackendAvailable(LogMode::kFile));
    if (IsBackendAvailable(LogMode::kConsole))
    {
        EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
    }
    else
    {
        EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
    }
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture,
       CreateFromConfigurationWithNoAvailableBackendAndNoConsoleReturnsEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "CreateFromConfiguration falls through to CreateWithConsoleLoggingOnly when recorders list is empty.");
    RecordProperty("TestingTechnique", "Boundary test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    gBackendCreators.fill(nullptr);

    SetupTempConfigFile(R"({"logMode": "kFile"})");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateFromConfiguration(score::cpp::pmr::get_default_resource());

    EXPECT_FALSE(IsBackendAvailable(LogMode::kFile));
    EXPECT_FALSE(IsBackendAvailable(LogMode::kConsole));
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture,
       CreateFromConfigurationWithMultipleAvailableModesReturnsCompositeRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "CreateFromConfiguration creates CompositeRecorder when multiple log modes are configured and "
                   "available.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    // Clear and register both backends explicitly to be independent of build flag combinations.
    gBackendCreators.fill(nullptr);

    RegisterBackend(
        LogMode::kFile,
        [](const detail::Configuration& /*config*/, score::cpp::pmr::memory_resource* /*mem*/) -> std::unique_ptr<Recorder> {
            return std::make_unique<EmptyRecorder>();
        });

    RegisterBackend(
        LogMode::kConsole,
        [](const detail::Configuration& /*config*/, score::cpp::pmr::memory_resource* /*mem*/) -> std::unique_ptr<Recorder> {
            return std::make_unique<EmptyRecorder>();
        });

    SetupTempConfigFile(R"({"logMode": "kConsole|kFile"})");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateFromConfiguration(score::cpp::pmr::get_default_resource());

    ASSERT_TRUE(IsRecorderOfType<CompositeRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture,
       CreateFromConfigurationAvailableBackendReturningNullRecorderResultsInEmpty)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "CreateFromConfiguration skips a recorder when the backend creator returns nullptr.");
    RecordProperty("TestingTechnique", "Boundary test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    RegisterBackend(
        LogMode::kFile,
        [](const detail::Configuration& /*config*/, score::cpp::pmr::memory_resource* /*mem*/) -> std::unique_ptr<Recorder> {
            return nullptr;
        });
    // Clear console so the fallback path also returns nullptr -> recorders stays empty.
    gBackendCreators[ModeToSlotIndex(LogMode::kConsole)] = nullptr;

    SetupTempConfigFile(R"({"logMode": "kFile"})");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateFromConfiguration(score::cpp::pmr::get_default_resource());

    // kFile returns nullptr -> not added -> recorders empty -> CreateWithConsoleLoggingOnly
    // -> console not registered -> EmptyRecorder.
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RegistryAwareRecorderFactoryConfigFixture, CreateFromConfigurationConsoleFallbackCreatorReturningNullIsSkipped)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "CreateFromConfiguration skips console fallback recorder when its creator returns nullptr.");
    RecordProperty("TestingTechnique", "Boundary test");
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements

    // kFile not available -> else branch -> console fallback -> console creator returns nullptr -> not added.
    gBackendCreators[ModeToSlotIndex(LogMode::kFile)] = nullptr;
    RegisterBackend(
        LogMode::kConsole,
        [](const detail::Configuration& /*config*/, score::cpp::pmr::memory_resource* /*mem*/) -> std::unique_ptr<Recorder> {
            return nullptr;
        });

    SetupTempConfigFile(R"({"logMode": "kFile"})");

    RegistryAwareRecorderFactory factory;
    auto recorder = factory.CreateFromConfiguration(score::cpp::pmr::get_default_resource());

    // Console fallback returns nullptr -> not added -> recorders empty -> CreateWithConsoleLoggingOnly
    // -> console returns nullptr -> EmptyRecorder.
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
