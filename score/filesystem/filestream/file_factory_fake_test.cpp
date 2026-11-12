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
#include "score/filesystem/filestream/file_factory_fake.h"
#include "score/filesystem/filestream/bad_string_stream_collection.h"
#include "score/filesystem/filestream/simple_string_stream_collection.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(FileFactoryFakeTest, CanOpenStringStream)
{
    SimpleStringStreamCollection collection{};
    FileFactoryFake unit{collection};
    {
        auto file_result = unit.Open("/tmp/some_file", std::ios_base::out);

        ASSERT_TRUE(file_result.has_value());

        *file_result.value() << "Foo";
    }

    const auto& file = unit.Get("/tmp/some_file");
    EXPECT_EQ(file.str(), "Foo");
}

TEST(FileFactoryFakeTest, FailedToOpenStringStream)
{
    BadStringStreamCollection bad_collection{};  // LCOV_EXCL_LINE
    FileFactoryFake unit{bad_collection};

    const auto file_result = unit.Open("/tmp/some_file", std::ios_base::out);

    ASSERT_FALSE(file_result.has_value());
    EXPECT_EQ(static_cast<ErrorCode>(*file_result.error()), ErrorCode::kCouldNotOpenFileStream);
}

TEST(FileFactoryFakeTest, AtomicUpdateOfFailedStreamKeepsContent)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__mock_fake");
    RecordProperty("Description",
                   "Check that the fake keeps the previous content when an atomic update stream has failed.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "error-guessing");

    SimpleStringStreamCollection collection{};
    FileFactoryFake unit{collection};
    unit.Get("/tmp/some_file").str("Foo");
    {
        auto file_result = unit.AtomicUpdate(
            "/tmp/some_file", std::ios_base::out | std::ios_base::trunc, kUseTargetFileUID | kUseTargetFileGID);
        ASSERT_TRUE(file_result.has_value());
        *file_result.value() << "Bar";
        file_result.value()->setstate(std::ios_base::failbit);
    }

    EXPECT_EQ(unit.Get("/tmp/some_file").str(), "Foo");
}

TEST(FileFactoryFakeTest, AtomicUpdateKeepsContentOfStreamFailingAfterClose)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__mock_fake");
    RecordProperty("Description",
                   "Check that the fake keeps the content committed by Close() when the stream fails afterwards.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "error-guessing");

    SimpleStringStreamCollection collection{};
    FileFactoryFake unit{collection};
    {
        auto file_result = unit.AtomicUpdate(
            "/tmp/some_file", std::ios_base::out | std::ios_base::trunc, kUseTargetFileUID | kUseTargetFileGID);
        ASSERT_TRUE(file_result.has_value());
        *file_result.value() << "Bar";
        ASSERT_TRUE(file_result.value()->Close().has_value());
        file_result.value()->setstate(std::ios_base::failbit);
    }

    EXPECT_EQ(unit.Get("/tmp/some_file").str(), "Bar");
}

}  // namespace
}  // namespace filesystem
}  // namespace score
