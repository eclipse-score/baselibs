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
#include <stdlib.h>

#include <gtest/gtest.h>
#include <limits>

namespace score
{
namespace os
{
namespace
{

TEST(SafeMemoryAllocatorTest, MallocFail)
{
    // This test verifies that malloc fails gracefully when attempting to allocate excessive memory,
    // returning a null pointer and setting errno to ENOMEM as expected.
    // ThreadSanitizer (TSAN) is expected to fail in this scenario because its allocator terminates
    // the program on such failures instead of returning null. Therefore, we only run the test outside of TSAN.
#if !defined(__SANITIZE_THREAD__)
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("ParentRequirement", "SCR-109773");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "This test verifies that an attempt to allocate more than the maximum amount of memory using "
                         "malloc reports an error.");
    auto result = ::malloc(std::numeric_limits<std::size_t>::max() / 2);

    ASSERT_EQ(result, nullptr);
#else
    GTEST_SKIP() << "Not supported in TSAN due to allocator limitations causing termination on allocation failure.";
#endif
}

TEST(SafeMemoryAllocatorTest, ReallocFail)
{
    // This test verifies that realloc fails gracefully when attempting to allocate excessive memory,
    // returning a null pointer and setting errno to ENOMEM as expected.
    // ThreadSanitizer (TSAN) is expected to fail in this scenario because its allocator terminates
    // the program on such failures instead of returning null. Therefore, we only run the test outside of TSAN.
#if !defined(__SANITIZE_THREAD__)
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("ParentRequirement", "SCR-109773");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "This test verifies that an attempt to allocate more than the maximum amount of memory using "
                         "realloc reports an error.");
    auto ptr = ::malloc(1);
    ASSERT_NE(ptr, nullptr);

    auto realloc_size = std::numeric_limits<std::size_t>::max() / 2;
    auto realloc_result = ::realloc(ptr, realloc_size);

    ASSERT_EQ(realloc_result, nullptr);

    if (!realloc_result)
        ::free(ptr);
#else
    GTEST_SKIP() << "Not supported in TSAN due to allocator limitations causing termination on allocation failure.";
#endif
}

}  // namespace
}  // namespace os
}  // namespace score
