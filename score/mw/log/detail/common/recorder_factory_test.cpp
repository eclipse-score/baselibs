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
#include "score/mw/log/detail/common/recorder_factory.h"
#include "score/mw/log/detail/common/recorder_config.h"

#include "score/mw/log/configuration/target_config_reader_mock.h"
#include "score/mw/log/detail/common/composite_recorder.h"
#include "score/mw/log/detail/empty_recorder.h"
#include "score/mw/log/detail/error.h"

#include "score/os/mocklib/fcntl_mock.h"

#include "gtest/gtest.h"

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

using ::testing::_;
using ::testing::Return;

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
    bool return_value = false;
    for (const auto& recorder : composite.GetRecorders())
    {
        if (IsRecorderOfType<ConcreteRecorder>(recorder))
        {
            return_value = true;
            break;
        }
    }
    return return_value;
}

class RecorderFactoryConfigFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        memory_resource_ = score::cpp::pmr::get_default_resource();
    }
    void TearDown() override {}

    std::unique_ptr<Recorder> CreateFromConfiguration() noexcept
    {
        auto config_reader_mock = std::make_unique<TargetConfigReaderMock>();
        ON_CALL(*config_reader_mock, ReadConfig).WillByDefault(testing::Invoke([&]() {
            return config_result_;
        }));
        return RecorderFactory{}.CreateFromConfiguration(std::move(config_reader_mock),
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

  protected:
    score::Result<Configuration> config_result_;
    score::cpp::pmr::memory_resource* memory_resource_ = nullptr;
};

TEST_F(RecorderFactoryConfigFixture, ConfigurationMemoryErrorShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will return empty recorder in case of configuration error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    score::cpp::pmr::memory_resource* null_memory_resource = nullptr;
    auto recorder = RecorderFactory{}.CreateFromConfiguration(null_memory_resource);
    EXPECT_EQ(nullptr, recorder);
}

TEST_F(RecorderFactoryConfigFixture, ConfigurationWithConfigReaderMemoryErrorShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will return empty recorder in case of configuration error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto config_reader_mock = std::make_unique<TargetConfigReaderMock>();
    score::cpp::pmr::memory_resource* null_memory_resource = nullptr;
    auto recorder = RecorderFactory{}.CreateFromConfiguration(std::move(config_reader_mock), null_memory_resource);
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, CreateConsoleLoggingOnlyMemoryErrorShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Create Console Logging Recorder will return empty recorder in case of memory error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    score::cpp::pmr::memory_resource* null_memory_resource = nullptr;
    auto recorder = RecorderFactory{}.CreateWithConsoleLoggingOnly(null_memory_resource);
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, CreateRecorderFromLogModeMemoryErrorShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Recorder created using LogMode will return empty recorder in case of memory error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    score::cpp::pmr::memory_resource* null_memory_resource = nullptr;
    Configuration config{};
    auto fcntl_mock = score::cpp::pmr::make_unique<score::os::FcntlMock>(memory_resource_);
    auto recorder = RecorderFactory{}.CreateRecorderFromLogMode(
        LogMode::kFile, config, std::move(fcntl_mock), null_memory_resource);
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, CreateRecorderFromLogModeOutOfRangeShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Recorder created based on invalid LogMode will return empty recorder.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    score::cpp::pmr::memory_resource* null_memory_resource = nullptr;
    Configuration config{};
    const LogMode mode = static_cast<LogMode>(std::numeric_limits<std::underlying_type_t<LogMode>>::max());
    auto fcntl_mock = score::cpp::pmr::make_unique<score::os::FcntlMock>(memory_resource_);
    auto recorder =
        RecorderFactory{}.CreateRecorderFromLogMode(mode, config, std::move(fcntl_mock), null_memory_resource);
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, ConfigurationErrorShallFallbackToConsoleLogging)
{
    RecordProperty("Requirement", "SCR-861534, SCR-7263552");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will return text recorder in case of configuration error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetTargetConfigReaderResult(score::MakeUnexpected(Error::kConfigurationFilesNotFound));
    auto recorder = CreateFromConfiguration();

    EXPECT_TRUE(IsRecorderOfType<ConsoleRecorderType>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, NoRecorderConfiguredShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can return empty recorder if no recorder is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // TargetConfigReader shall return a config with no active recorders.
    SetConfigurationWithLogMode({});

    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, FileConfiguredShallReturnFileRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can create FileRecorder if file is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kFile});
    auto recorder = CreateFromConfiguration();

    EXPECT_TRUE(IsRecorderOfType<FileRecorderType>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, FileConfiguredShallReturnEmptyRecorderWithInvalidFile)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can create FileRecorder if file is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    Configuration config;
    constexpr auto kInvalidPath = "!@#";
    config.SetLogFilePath(kInvalidPath);
    SetConfigurationWithLogMode({LogMode::kFile}, config);
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, ConsoleConfiguredShallReturnConsoleRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can create ConsoleRecorder if console is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kConsole});
    auto recorder = CreateFromConfiguration();

    EXPECT_TRUE(IsRecorderOfType<ConsoleRecorderType>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, CustomConfiguredShallReturnCustomRecorder)
{
    SetConfigurationWithLogMode({LogMode::kCustom});
    auto recorder = CreateFromConfiguration();

    EXPECT_TRUE(IsRecorderOfType<CustomRecorderType>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, InvalidLogModeShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will create EmptyRecorder in case of invalid log mode.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kInvalid});
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, OutOfRangeLogModeShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will create EmptyRecorder in case of out of range log mode.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // selected number should not be listed in LogMode enum definition
    const LogMode outOfRangeLogMode = static_cast<LogMode>(240);

    SetConfigurationWithLogMode({outOfRangeLogMode});
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, MultipleLogModesShallReturnCompositeRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "RecorderFactory shall create CompositeRecorder in case of multiple log mode is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kFile, LogMode::kConsole, LogMode::kRemote});
    auto recorder = CreateFromConfiguration();
    ASSERT_TRUE(IsRecorderOfType<CompositeRecorder>(recorder));
    const auto& composite_recorder = *dynamic_cast<CompositeRecorder*>(recorder.get());

    EXPECT_EQ(composite_recorder.GetRecorders().size(), config_result_->GetLogMode().size());

    EXPECT_TRUE(ContainsRecorderOfType<FileRecorderType>(composite_recorder));

    EXPECT_TRUE(ContainsRecorderOfType<ConsoleRecorderType>(composite_recorder));
}

TEST_F(RecorderFactoryConfigFixture, CreateWithConsoleLoggingOnlyShallReturnConsoleRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of creating a specific logging.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto recorder = RecorderFactory{}.CreateWithConsoleLoggingOnly(score::cpp::pmr::get_default_resource());

    EXPECT_TRUE(IsRecorderOfType<ConsoleRecorderType>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, CreateStubShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory shall create EmptyRecorder in case of using CreateStub.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto recorder = RecorderFactory{}.CreateStub();

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, SystemConfiguredShallReturnSlogRecorder)
{
    RecordProperty("ParentRequirement", "SCR-8017693");
    RecordProperty("Description",
                   "The system logger backend shall be enabled if and only if the log mode contains 'kSystem'");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kSystem});
    auto recorder = CreateFromConfiguration();

    // Console recorder shall be reused for slog backend. For slogger we also need to output ASCII like on the console.
    EXPECT_TRUE(IsRecorderOfType<SystemRecorderType>(recorder));
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
