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

#include <gtest/gtest.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory_resource>
#include <string>
#include <system_error>
#include <vector>

#include "score/flatbuffers/load_buffer.hpp"

namespace score
{

namespace flatbuffers
{

namespace test
{

using score::flatbuffers::LoadBuffer;

/// Test fixture that manages a temporary directory for test files.
/// All test files are created under this directory and cleaned up
/// automatically.
class LoadFlatbufferTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        RecordProperty("Verifies", "ADDID");
        RecordProperty("Description", "defensive error handling is not part of this test suite");

        test_dir_ = std::filesystem::current_path().string();
    }

    void TearDown() override
    {
        for (const auto& file : files_)
        {
            std::filesystem::remove(file.c_str());
        }
    }

    /// Creates a file with the given binary content and returns its Path.
    score::filesystem::Path WriteFile(const std::string& name, const std::vector<uint8_t>& content) const
    {
        const std::string filepath = test_dir_ + "/LoadFlatbufferTest" + name;
        std::ofstream ofs(filepath, std::ios::binary);
        if (!ofs.is_open())
        {
            ADD_FAILURE() << "Failed to open file for writing: " << filepath;
            return score::filesystem::Path{};
        }
        ofs.write(static_cast<const char*>(static_cast<const void*>(content.data())),
                  static_cast<std::streamsize>(content.size()));
        ofs.close();
        files_.push_back(filepath);
        return score::filesystem::Path{filepath};
    }

    /// Convenience overload for string content.
    score::filesystem::Path WriteFile(const std::string& name, const std::string& content) const
    {
        return WriteFile(name, std::vector<uint8_t>(content.begin(), content.end()));
    }

    mutable std::vector<std::string> files_;
    std::string test_dir_;
};

void ExpectVectorLoad(const score::filesystem::Path& path, const std::vector<uint8_t>& expected)
{
    SCOPED_TRACE(path.CStr());
    const auto result = LoadBuffer(path);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), expected);
}

void ExpectPmrVectorLoad(const score::filesystem::Path& path, const std::vector<uint8_t>& expected)
{
    SCOPED_TRACE(path.CStr());
    std::pmr::vector<uint8_t> data;
    const auto result = LoadBuffer(path, data);
    ASSERT_TRUE(result.has_value());
    const std::vector<uint8_t> data_copy(data.cbegin(), data.cend());
    EXPECT_EQ(data_copy, expected);
}

void ExpectVectorLoad(const score::filesystem::Path& path, const score::os::Error::Code& expected)
{
    SCOPED_TRACE(path.CStr());
    const auto result = LoadBuffer(path);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), expected);
}

void ExpectPmrVectorLoad(const score::filesystem::Path& path, const score::os::Error::Code& expected)
{
    SCOPED_TRACE(path.CStr());
    std::pmr::vector<uint8_t> data;
    const auto result = LoadBuffer(path, data);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), expected);
}

TEST_F(LoadFlatbufferTest, LoadsRegularFileContents)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "loaded content from file");

    const std::vector<uint8_t> content{'H', 'e', 'l', 'l', 'o'};
    const auto path = WriteFile("hello.bin", content);

    ExpectVectorLoad(path, content);
    ExpectPmrVectorLoad(path, content);
}

TEST_F(LoadFlatbufferTest, LoadsEmptyFileAsEmptyVector)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "empty file is loaded as empty vector");

    const auto path = WriteFile("empty.bin", std::vector<uint8_t>{});

    ExpectVectorLoad(path, std::vector<uint8_t>{});
    ExpectPmrVectorLoad(path, std::vector<uint8_t>{});
}

TEST_F(LoadFlatbufferTest, LoadsBinaryContentIncludingNullBytes)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "null bytes are not truncated, file is read in binary mode");

    const std::vector<uint8_t> content{0x00U, 0x01U, 0xFEU, 0xFFU, 0x00U, 0xABU};
    const auto path = WriteFile("binary.bin", content);

    ExpectVectorLoad(path, content);
    ExpectPmrVectorLoad(path, content);
}

TEST_F(LoadFlatbufferTest, LoadsSingleByteFile)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "single byte is loaded from file");

    const std::vector<uint8_t> content{'X'};
    const auto path = WriteFile("one_byte.bin", content);

    ExpectVectorLoad(path, content);
    ExpectPmrVectorLoad(path, content);
}

TEST_F(LoadFlatbufferTest, LoadsLarge5MBFileCorrectly)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "large content is loaded from file");

    constexpr auto kSize = static_cast<const std::size_t>(5U * 1024U * 1024U);  // 5 MB
    constexpr int kPrimeModulus = 251;
    std::vector<uint8_t> content(kSize);
    for (std::size_t i = 0U; i < kSize; ++i)
    {
        content[i] = static_cast<uint8_t>(i % kPrimeModulus);  // prime modulus for varied pattern
    }
    const auto path = WriteFile("large.bin", content);

    ExpectVectorLoad(path, content);
    ExpectPmrVectorLoad(path, content);
}

TEST_F(LoadFlatbufferTest, LoadsFileContainingOnlyNullBytes)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "only nulls is loaded from file");

    const std::vector<uint8_t> content(512, '\0');
    const auto path = WriteFile("nulls.bin", content);

    ExpectVectorLoad(path, content);
    ExpectPmrVectorLoad(path, content);
}

TEST_F(LoadFlatbufferTest, NonexistentFileReturnsError)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("Description", "kNoSuchFileOrDirectory is returned if file does not exist");

    const score::filesystem::Path path{test_dir_ + "/does_not_exist.bin"};

    ExpectVectorLoad(path, score::os::Error::Code::kNoSuchFileOrDirectory);
    ExpectPmrVectorLoad(path, score::os::Error::Code::kNoSuchFileOrDirectory);
}

TEST_F(LoadFlatbufferTest, NoReadPermissionReturnsPermissionDenied)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("Description", "kPermissionDenied is returned if the file cannot be accessed");

    if (::getuid() == 0U)
    {
        GTEST_SKIP() << "Cannot test permission denial when running as root";
    }

    const auto path = WriteFile("no_perm.bin", "secret");
    ::chmod(path.CStr(), 0000);

    ExpectVectorLoad(path, score::os::Error::Code::kPermissionDenied);
    ExpectPmrVectorLoad(path, score::os::Error::Code::kPermissionDenied);
}

TEST_F(LoadFlatbufferTest, DirectoryPathFailsOnReadWithIsADirectory)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("Description", "kIsADirectory is returned if path is a directory instead of a file");

    const auto path = test_dir_;

    ExpectVectorLoad(path, score::os::Error::Code::kIsADirectory);
    ExpectPmrVectorLoad(path, score::os::Error::Code::kIsADirectory);
}

TEST_F(LoadFlatbufferTest, PmrOverloadFailsOnResize)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("Description",
                   "kNotEnoughSpace is returned if user defined buffer is insufficient to hold the data");

    constexpr auto kSize = static_cast<const std::size_t>(1024 * 1024);  // 1 MB
    constexpr int kPrimeModulus = 251;
    constexpr std::size_t kBufferSize = 1024U;

    std::vector<uint8_t> content(kSize);
    for (std::size_t i = 0U; i < kSize; ++i)
    {
        content[i] = static_cast<uint8_t>(i % kPrimeModulus);
    }

    const auto path = WriteFile("1mb.bin", content);

    std::array<std::byte, kBufferSize> buffer{};  // not enough to fit 1mb file
    std::pmr::monotonic_buffer_resource mbr{buffer.data(), buffer.size(), std::pmr::null_memory_resource()};
    std::pmr::polymorphic_allocator<uint8_t> pmr_alloc{&mbr};
    std::pmr::vector<uint8_t> data{pmr_alloc};
    const auto result = LoadBuffer(path, data);
    ASSERT_EQ(result.error(), score::os::Error::Code::kNotEnoughSpace);
    ASSERT_EQ(data.size(), 0U);
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score
