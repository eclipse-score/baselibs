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
#include "score/analysis/tracing/generic_trace_library/interface_types/generic_trace_api.h"
#include <gtest/gtest.h>

namespace score
{
namespace analysis
{
namespace tracing
{
namespace
{

TEST(GenericTraceApiStubTest, TraceForwardsToConfiguredImplementation)
{
    RecordProperty("PartiallyVerifies", "comp_req__trace__feature_flag");
    RecordProperty("Description",
                   "Check that GenericTraceAPI::Trace() routes through the implementation selected by the "
                   "'implementation' label_flag and returns the result produced by that implementation.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    constexpr TraceClientId kClientId{1U};
    constexpr TraceContextId kContextId{1U};
    const MetaInfoVariants::StdType meta_info{DltMetaInfo{DltProperties{}}};
    ShmDataChunkList data{};

    const TraceResult result = GenericTraceAPI::Trace(kClientId, meta_info, data, kContextId);

    EXPECT_TRUE(result.has_value());
}

}  // namespace
}  // namespace tracing
}  // namespace analysis
}  // namespace score
