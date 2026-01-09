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
#include "score/os/qnx_pthread.h"

#include <gtest/gtest.h>

namespace
{

TEST(PthreadNameTest, GetNameFailsWhenBufferSizeTooBig)
{

    RecordProperty("ParentRequirement", "SCR-4977102");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get Name Fails When Buffer Size Too Big");
    RecordProperty("TestingTechnique", "T-REQ");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::size_t size_too_big = static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()) + 1;
    char buffer[4096];  // fails to allocate buffer with `size_too_big` amount on stack. Follow up operation which
                        // utilizes buffer should not overflow it due to checking the length (passing size_too_big)
                        // before accessing the buffer
    score::os::QnxPthread pthread{};
    const auto read_result = pthread.getname_np(pthread.self(), buffer, size_too_big);

    ASSERT_FALSE(read_result.has_value());
}

TEST(QnxPthreadTest, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PMR Default Shall Return Impl Instance");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Pthread::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::QnxPthread*>(instance.get()));
}

}  // namespace
