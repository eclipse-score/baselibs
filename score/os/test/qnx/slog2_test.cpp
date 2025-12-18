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
#include "score/os/qnx/slog2.h"
#include "score/os/qnx/slog2_impl.h"

#include <gtest/gtest.h>

#include <sys/slog2.h>

namespace
{
// ---------- score/os/qnx/slog2_impl.cpp --------
class Slog2ImplFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        unit_ = std::make_unique<score::os::qnx::Slog2Impl>();
    }

    std::unique_ptr<score::os::qnx::Slog2> unit_;
};

TEST_F(Slog2ImplFixture, slog2_registerReturnsErrorIfSetNumBuffersToZero)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Slog Register returns Error If Set Num Buffers To Zero");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    slog2_buffer_set_config_t buffer_config{};
    slog2_buffer_t buffer_handle[2]{};

    buffer_config.buffer_set_name = "test_program";
    buffer_config.num_buffers = 0;  // Invalid number of buffers, should be 2. Causes an error

    // Register the buffer set
    auto result = unit_->slog2_register(&buffer_config, buffer_handle, 0);
    EXPECT_FALSE(result);

    if (result.has_value() == true)
    {
        // this should not happen, nonetheless if test failed resets the state of slogger2 for the process
        ::slog2_reset();
    }
}

TEST_F(Slog2ImplFixture, slog2SetVerbosityFailsWhenInvalidVerbosity)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Slog Set Verbosity Fails When Invalid Verbosity");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr uint8_t invalid_level = INT8_MAX;
    slog2_buffer_t buffer_handle{};

    EXPECT_EQ(-1, unit_->slog2_set_verbosity(buffer_handle, invalid_level));
}

TEST_F(Slog2ImplFixture, slog2cReturnsErrorIfNotRegisteredSlog2)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Slog C returns Error If Not Registered Slog");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    slog2_buffer_t buffer_handle[1]{};
    EXPECT_FALSE(unit_->slog2c(buffer_handle[0], 0, SLOG2_INFO, "fails to log"));
}

TEST_F(Slog2ImplFixture, slog2fReturnsErrorIfNotRegisteredSlog2)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Slog F returns Error If Not Registered Slog");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    slog2_buffer_t buffer_handle[1]{};
    EXPECT_FALSE(unit_->slog2f(buffer_handle[0], 0, SLOG2_INFO, "formmated: %s", "fails to log"));
}

TEST_F(Slog2ImplFixture, RegisterAndLogFlow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Register And Log Flow");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    /*
        This test registers to buffer within slog2. Afterwards trying to log with different severity level
    */
    slog2_buffer_set_config_t buffer_config{};
    slog2_buffer_t buffer_handle[2]{};

    buffer_config.buffer_set_name = "test_program";
    buffer_config.num_buffers = 2;  // These two buffers are configured below

    // Request an initial verbosity level
    buffer_config.verbosity_level = SLOG2_INFO;

    // Configure the first buffers
    buffer_config.buffer_config[0].buffer_name = "hi_rate_logging";
    buffer_config.buffer_config[0].num_pages = 7;
    // and second one buffer
    buffer_config.buffer_config[1].buffer_name = "lo_rate_logging";
    buffer_config.buffer_config[1].num_pages = 1;

    // Register the buffer set
    ASSERT_TRUE(unit_->slog2_register(&buffer_config, buffer_handle, 0));

    EXPECT_TRUE(unit_->slog2f(
        buffer_handle[0], 0, SLOG2_INFO, "Writing a formatted string into the buffer: %s", "hello world"));
    EXPECT_TRUE(unit_->slog2c(buffer_handle[0], 0, SLOG2_INFO, "Writing a constant string into the buffer."));

    // Write something to the 'lo rate' buffer (i.e., buffer 1)
    EXPECT_TRUE(unit_->slog2c(buffer_handle[1], 0, SLOG2_NOTICE, "This string will be logged."));

    // The verbosity level will prevent this from being written to the
    // slog2 buffer (severity > verbosity_level).
    EXPECT_TRUE(unit_->slog2c(buffer_handle[0], 0, SLOG2_DEBUG1, "This string should not be logged."));

    // The verbosity level is set to SLOG2_DEBUG2
    EXPECT_EQ(0, unit_->slog2_set_verbosity(buffer_handle[0], SLOG2_DEBUG2));

    // slog2 buffer (severity < verbosity_level).
    EXPECT_TRUE(
        unit_->slog2c(buffer_handle[0], 0, SLOG2_DEBUG1, "This string will be logged (severity < verbosity_level)."));

    // Reset the state of slogger2 for the process
    EXPECT_EQ(0, unit_->slog2_reset());
}

TEST(Slog2Test, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PMR Default Shall Return Impl Instance");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::qnx::Slog2::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::qnx::Slog2Impl*>(instance.get()));
}

}  // namespace
