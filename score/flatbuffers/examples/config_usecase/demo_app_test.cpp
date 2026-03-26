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

/// @file demo_app_test.cpp
/// @brief Demonstrates loading and reading a FlatBuffer binary config
///        using flatbufferscpp (FlatBuffers C++ API)
///
/// Build and run via Bazel:
///   bazel test //demo_config_usecase:demo_app

// Generated C++ accessor header produced by generate_cpp() – depends on flatbufferscpp
#include "score/flatbuffers/examples/config_usecase/component_config.h"

// FlatBuffers verifier (part of flatbufferscpp / @flatbuffers headers)
#include "flatbuffers/verifier.h"

#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>

TEST(DemoAppTest, LoadsAndVerifiesBuffer)
{
    // -------------------------------------------------------------------------
    // Step 1: Load the binary FlatBuffer file from disk (standard file I/O)
    // -------------------------------------------------------------------------
    const std::string bin_path = "score/flatbuffers/examples/config_usecase/demo_config.bin";

    /// TODO: Replace with planned binary file loading utility.

    std::ifstream file(bin_path, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(file.is_open()) << "Failed to open binary config from '" << bin_path << "'";

    const auto file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(file_size);
    ASSERT_TRUE(file.read(buffer.data(), file_size)) << "Failed to read binary config from '" << bin_path << "'";
    file.close();

    // -------------------------------------------------------------------------
    // Step 2: Verify buffer integrity (flatbuffercpp / FlatBuffers verifier)
    // -------------------------------------------------------------------------
    flatbuffers::Verifier verifier(reinterpret_cast<const uint8_t*>(buffer.data()), buffer.size());

    ASSERT_TRUE(my_component::demo::VerifyMyComponentConfigBuffer(verifier))
        << "FlatBuffer verification failed for '" << bin_path << "'";

    // -------------------------------------------------------------------------
    // Step 3: Access config values via the generated flatbuffercpp API
    // -------------------------------------------------------------------------
    const my_component::demo::MyComponentConfig* config = my_component::demo::GetMyComponentConfig(buffer.data());

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
