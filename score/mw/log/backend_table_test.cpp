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
#include "score/mw/log/backend_table.h"

#include "score/mw/log/detail/empty_recorder.h"

#include "gtest/gtest.h"

namespace score
{
namespace mw
{
namespace log
{
namespace
{

using detail::Configuration;
using detail::EmptyRecorder;

class BackendTableTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Reset the backend table before each test to ensure test isolation.
        gBackendCreators.fill(nullptr);
    }

    void TearDown() override
    {
        // Clean up after each test.
        gBackendCreators.fill(nullptr);
    }
};

// --- ModeToSlotIndex tests ---

TEST(ModeToSlotIndexTest, ConsoleModeMapsToSlotZero)
{
    EXPECT_EQ(ModeToSlotIndex(LogMode::kConsole), 0U);
}

TEST(ModeToSlotIndexTest, FileModeMapsToSlotOne)
{
    EXPECT_EQ(ModeToSlotIndex(LogMode::kFile), 1U);
}

TEST(ModeToSlotIndexTest, RemoteModeMapsToSlotTwo)
{
    EXPECT_EQ(ModeToSlotIndex(LogMode::kRemote), 2U);
}

TEST(ModeToSlotIndexTest, SystemModeMapsToSlotThree)
{
    EXPECT_EQ(ModeToSlotIndex(LogMode::kSystem), 3U);
}

TEST(ModeToSlotIndexTest, CustomModeMapsToSlotFour)
{
    EXPECT_EQ(ModeToSlotIndex(LogMode::kCustom), 4U);
}

TEST(ModeToSlotIndexTest, InvalidModeReturnsSentinel)
{
    EXPECT_EQ(ModeToSlotIndex(LogMode::kInvalid), kMaxBackendSlots);
}

// --- RegisterBackend tests ---

TEST_F(BackendTableTest, RegisterBackendSetsSlotForConsole)
{
    RecordProperty("Description", "RegisterBackend shall populate the correct slot for kConsole.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    auto creator = [](const Configuration&, score::cpp::pmr::memory_resource*) -> std::unique_ptr<Recorder> {
        return std::make_unique<EmptyRecorder>();
    };

    RegisterBackend(LogMode::kConsole, creator);

    EXPECT_NE(gBackendCreators[0U], nullptr);
}

TEST_F(BackendTableTest, RegisterBackendSetsSlotForFile)
{
    RecordProperty("Description", "RegisterBackend shall populate the correct slot for kFile.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    auto creator = [](const Configuration&, score::cpp::pmr::memory_resource*) -> std::unique_ptr<Recorder> {
        return std::make_unique<EmptyRecorder>();
    };

    RegisterBackend(LogMode::kFile, creator);

    EXPECT_NE(gBackendCreators[1U], nullptr);
}

TEST_F(BackendTableTest, RegisterBackendIgnoresInvalidMode)
{
    RecordProperty("Description", "RegisterBackend shall not write to the table for kInvalid mode.");
    RecordProperty("TestingTechnique", "Boundary test");

    auto creator = [](const Configuration&, score::cpp::pmr::memory_resource*) -> std::unique_ptr<Recorder> {
        return std::make_unique<EmptyRecorder>();
    };

    RegisterBackend(LogMode::kInvalid, creator);

    for (std::size_t i = 0U; i < kMaxBackendSlots; ++i)
    {
        EXPECT_EQ(gBackendCreators[i], nullptr);
    }
}

// --- IsBackendAvailable tests ---

TEST_F(BackendTableTest, IsBackendAvailableReturnsFalseWhenNotRegistered)
{
    RecordProperty("Description", "IsBackendAvailable shall return false for unregistered modes.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    EXPECT_FALSE(IsBackendAvailable(LogMode::kConsole));
    EXPECT_FALSE(IsBackendAvailable(LogMode::kFile));
    EXPECT_FALSE(IsBackendAvailable(LogMode::kRemote));
    EXPECT_FALSE(IsBackendAvailable(LogMode::kSystem));
    EXPECT_FALSE(IsBackendAvailable(LogMode::kCustom));
}

TEST_F(BackendTableTest, IsBackendAvailableReturnsTrueWhenRegistered)
{
    RecordProperty("Description", "IsBackendAvailable shall return true after registration.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    auto creator = [](const Configuration&, score::cpp::pmr::memory_resource*) -> std::unique_ptr<Recorder> {
        return std::make_unique<EmptyRecorder>();
    };

    RegisterBackend(LogMode::kFile, creator);

    EXPECT_TRUE(IsBackendAvailable(LogMode::kFile));
    EXPECT_FALSE(IsBackendAvailable(LogMode::kConsole));
}

TEST_F(BackendTableTest, IsBackendAvailableReturnsFalseForInvalidMode)
{
    RecordProperty("Description", "IsBackendAvailable shall return false for kInvalid.");
    RecordProperty("TestingTechnique", "Boundary test");

    EXPECT_FALSE(IsBackendAvailable(LogMode::kInvalid));
}

// --- CreateRecorderForMode tests ---

TEST_F(BackendTableTest, CreateRecorderForModeReturnsNullptrWhenNotRegistered)
{
    RecordProperty("Description", "CreateRecorderForMode shall return nullptr for unregistered modes.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    const Configuration config;
    auto recorder = CreateRecorderForMode(LogMode::kFile, config, nullptr);

    EXPECT_EQ(recorder, nullptr);
}

TEST_F(BackendTableTest, CreateRecorderForModeReturnsRecorderWhenRegistered)
{
    RecordProperty("Description", "CreateRecorderForMode shall return a valid Recorder for registered modes.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    auto creator = [](const Configuration&, score::cpp::pmr::memory_resource*) -> std::unique_ptr<Recorder> {
        return std::make_unique<EmptyRecorder>();
    };

    RegisterBackend(LogMode::kConsole, creator);

    const Configuration config;
    auto recorder = CreateRecorderForMode(LogMode::kConsole, config, nullptr);

    EXPECT_NE(recorder, nullptr);
}

TEST_F(BackendTableTest, CreateRecorderForModeReturnsNullptrForInvalidMode)
{
    RecordProperty("Description", "CreateRecorderForMode shall return nullptr for kInvalid.");
    RecordProperty("TestingTechnique", "Boundary test");

    const Configuration config;
    auto recorder = CreateRecorderForMode(LogMode::kInvalid, config, nullptr);

    EXPECT_EQ(recorder, nullptr);
}

// --- BackendRegistrant tests ---

TEST_F(BackendTableTest, BackendRegistrantRegistersCreatorDuringConstruction)
{
    RecordProperty("Description", "BackendRegistrant shall register a creator function during construction.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    auto creator = [](const Configuration&, score::cpp::pmr::memory_resource*) -> std::unique_ptr<Recorder> {
        return std::make_unique<EmptyRecorder>();
    };

    EXPECT_FALSE(IsBackendAvailable(LogMode::kRemote));

    const BackendRegistrant registrant{LogMode::kRemote, creator};

    EXPECT_TRUE(IsBackendAvailable(LogMode::kRemote));
}

// --- Multiple backend registration test ---

TEST_F(BackendTableTest, MultipleBackendsCanBeRegistered)
{
    RecordProperty("Description", "Multiple distinct backends shall coexist in the table without conflicts.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    auto console_creator = [](const Configuration&, score::cpp::pmr::memory_resource*) -> std::unique_ptr<Recorder> {
        return std::make_unique<EmptyRecorder>();
    };
    auto file_creator = [](const Configuration&, score::cpp::pmr::memory_resource*) -> std::unique_ptr<Recorder> {
        return std::make_unique<EmptyRecorder>();
    };

    RegisterBackend(LogMode::kConsole, console_creator);
    RegisterBackend(LogMode::kFile, file_creator);

    EXPECT_TRUE(IsBackendAvailable(LogMode::kConsole));
    EXPECT_TRUE(IsBackendAvailable(LogMode::kFile));
    EXPECT_FALSE(IsBackendAvailable(LogMode::kRemote));
}

// --- Overwrite test ---

TEST_F(BackendTableTest, RegisterBackendOverwritesPreviousRegistration)
{
    RecordProperty("Description", "A second RegisterBackend call for the same mode shall overwrite the first.");
    RecordProperty("TestingTechnique", "Requirements-based test");

    auto first_creator = [](const Configuration&, score::cpp::pmr::memory_resource*) -> std::unique_ptr<Recorder> {
        return std::make_unique<EmptyRecorder>();
    };
    auto second_creator = [](const Configuration&, score::cpp::pmr::memory_resource*) -> std::unique_ptr<Recorder> {
        return nullptr;
    };

    RegisterBackend(LogMode::kConsole, first_creator);
    const RecorderCreatorFn original_fn = gBackendCreators[0U];

    RegisterBackend(LogMode::kConsole, second_creator);
    const RecorderCreatorFn overwritten_fn = gBackendCreators[0U];

    EXPECT_NE(original_fn, overwritten_fn);

    // Verify the second creator is the one that's active (returns nullptr, not EmptyRecorder).
    const Configuration config;
    auto recorder = CreateRecorderForMode(LogMode::kConsole, config, nullptr);
    EXPECT_EQ(recorder, nullptr);
}

}  // namespace
}  // namespace log
}  // namespace mw
}  // namespace score
