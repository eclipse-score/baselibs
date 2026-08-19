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

/// @file demo_config_test.cpp
/// @brief Demonstrates loading of a FlatBuffer binary config and reading the config fields.

#include "score/flatbuffers/load_buffer.hpp"
// Generated C++ accessor header produced by generate_cpp() for demo.fbs
#include "flatbuffers/config_usecase/component_config.h"

#include "flatbuffers/verifier.h"

#include <gtest/gtest.h>

#include <string_view>
#include <vector>

TEST(DemoAppTest, LoadsAndVerifiesBuffer)
{
    // -------------------------------------------------------------------------
    // Step 1: Load the binary FlatBuffer file from disk (standard file I/O)
    // -------------------------------------------------------------------------
    const std::string_view bin_path = "flatbuffers/config_usecase/demo_config.bin";
    const auto buffer = score::flatbuffers::LoadBuffer(bin_path);
    ASSERT_TRUE(buffer.has_value()) << buffer.error().ToString();

    // -------------------------------------------------------------------------
    // Step 2: Verify structural integrity of the FlatBuffer
    // -------------------------------------------------------------------------
    flatbuffers::Verifier verifier(buffer.value().data(), buffer.value().size());

    ASSERT_TRUE(my_component::demo::VerifyMyComponentConfigBuffer(verifier))
        << "FlatBuffer structural verification failed for '" << bin_path << "'";

    // -------------------------------------------------------------------------
    // Step 3: Access config values via the generated APIs
    // -------------------------------------------------------------------------
    const my_component::demo::MyComponentConfig* config =
        my_component::demo::GetMyComponentConfig(buffer.value().data());

    ASSERT_NE(config, nullptr);
    EXPECT_EQ(config->component_name()->str(), "demo_component");
    EXPECT_EQ(config->component_id(), 42U);
    EXPECT_EQ(config->thread_pool_size(), 4U);
    EXPECT_EQ(config->max_memory_mb(), 64U);

    const my_component::demo::AdvancedSettings* adv = config->advanced_settings();
    ASSERT_NE(adv, nullptr);
    EXPECT_EQ(adv->mode(), my_component::demo::OperationMode::PERIODIC);

    ASSERT_NE(adv->allowed_priorities(), nullptr);
    EXPECT_EQ(adv->allowed_priorities()->size(), 2U);
    EXPECT_EQ(adv->allowed_priorities()->Get(0), 1U);
    EXPECT_EQ(adv->allowed_priorities()->Get(1), 2U);

    ASSERT_NE(adv->feature_flags(), nullptr);
    ASSERT_EQ(adv->feature_flags()->size(), 1U);
    const my_component::demo::FeatureFlag* flag = adv->feature_flags()->Get(0);
    EXPECT_EQ(flag->name()->str(), "enable_logging");
    EXPECT_TRUE(flag->enabled());
}
