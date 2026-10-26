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

#include "score/serialization/Serialization.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <score/static_vector.hpp>

#include <vector>

namespace score
{
namespace platform
{
namespace testing
{

class SerializationTest : public ::testing::Test
{
  public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SerializationTest, serialize_in_place_uint8_to_array)
{
    // Arrange
    const std::array<std::uint8_t, 1> expected_output = {0xA3};
    constexpr std::uint8_t value = 0xA3;

    // Act
    std::array<std::uint8_t, 1> serialized_output{};
    std::int64_t offset = 0;
    serialize_in_place(serialized_output, value, offset);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
}

TEST_F(SerializationTest, serialize_in_place_uint8_to_span_of_array)
{
    // Arrange
    const std::array<std::uint8_t, 1> expected_output = {0xA3};
    constexpr std::uint8_t value = 0xA3;

    // Act
    std::array<std::uint8_t, 1> serialized_output{};
    score::cpp::span<std::uint8_t> serialized_output_view{serialized_output};
    std::int64_t offset = 0;
    serialize_in_place(serialized_output_view, value, offset);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, sizeof(decltype(value)));
}

TEST_F(SerializationTest, serialize_and_allocate_uint16_to_vector_without_swapping_bytes)
{
    // Arrange
    const std::vector<std::uint8_t> expected_output = {0x01, 0x04};
    constexpr std::uint16_t value = 1024 + 1;

    // Act
    std::vector<std::uint8_t> serialized_output{};
    const auto result = serialize_and_allocate(serialized_output, value, false);
    ASSERT_TRUE(result.has_value());

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
}

TEST_F(SerializationTest, serialize_and_allocate_uint16_to_static_vector_without_swapping_bytes)
{
    // Arrange
    const score::cpp::static_vector<std::uint8_t, 2> expected_output = {0x01, 0x04};
    constexpr std::uint16_t value = 1024 + 1;

    // Act
    score::cpp::static_vector<std::uint8_t, 2> serialized_output{};
    const auto result = serialize_and_allocate(serialized_output, value, false);
    ASSERT_TRUE(result.has_value());

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
}

TEST_F(SerializationTest, serialize_in_place_uint16_to_array_without_swapping_bytes)
{
    // Arrange
    const std::array<std::uint8_t, 2> expected_output = {0x01, 0x04};
    constexpr std::uint16_t value = 1024 + 1;

    // Act
    std::array<std::uint8_t, 2> serialized_output{};
    std::int64_t offset = 0;
    serialize_in_place(serialized_output, value, offset, false);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, sizeof(decltype(value)));
}

TEST_F(SerializationTest, serialize_in_place_uint16_to_span_of_array_without_swapping_bytes)
{
    // Arrange
    const std::array<std::uint8_t, 2> expected_output = {0x01, 0x04};
    constexpr std::uint16_t value = 1024 + 1;

    // Act
    std::array<std::uint8_t, 2> serialized_output{};
    score::cpp::span<std::uint8_t> serialized_output_view{serialized_output};
    std::int64_t offset = 0;
    serialize_in_place(serialized_output_view, value, offset, false);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, sizeof(decltype(value)));
}

TEST_F(SerializationTest, deserialize_uint8_from_vector_buffer)
{
    // Arrange
    const std::vector<std::uint8_t> value{0xA3};
    const std::uint8_t expected_output = 10 * 16 + 3;

    std::size_t offset{0};
    const std::size_t expected_offset{1};

    // Act
    const auto output = deserialize<std::uint8_t>(value, offset);

    // Assert
    ASSERT_EQ(offset, expected_offset);
    ASSERT_EQ(output, expected_output);
}

TEST_F(SerializationTest, deserialize_uint8_from_array)
{
    // Arrange
    const std::array<std::uint8_t, 1> value{0xA3};
    const std::uint8_t expected_output = 10 * 16 + 3;

    std::size_t offset{0};
    const std::size_t expected_offset{1};

    // Act
    const auto output = deserialize<std::uint8_t>(value, offset);

    // Assert
    ASSERT_EQ(offset, expected_offset);
    ASSERT_EQ(output, expected_output);
}

TEST_F(SerializationTest, deserialize_uint8_from_span_of_array)
{
    // Arrange
    const std::array<std::uint8_t, 1> value{0xA3};
    const score::cpp::span<const std::uint8_t> value_view{value};
    const std::uint8_t expected_output = 10 * 16 + 3;

    std::size_t offset{0};
    const std::size_t expected_offset{1};

    // Act
    const auto output = deserialize<std::uint8_t>(value_view, offset);

    // Assert
    ASSERT_EQ(offset, expected_offset);
    ASSERT_EQ(output, expected_output);
}

TEST_F(SerializationTest, deserialize_uint32_from_vector_buffer)
{
    // Arrange
    const std::vector<std::uint8_t> value{0x0, 0x0, 0x1, 0x2, 0x3, 0x4};
    const std::uint32_t expected_output = 0x1020304;

    std::size_t offset{2};
    const std::size_t expected_offset{6};

    // Act
    const auto output = deserialize<std::uint32_t>(value, offset);

    // Assert
    ASSERT_EQ(offset, expected_offset);
    ASSERT_EQ(output, expected_output);
}

TEST_F(SerializationTest, deserialize_uint32_from_vector_buffer_swap)
{
    // Arrange
    const std::vector<std::uint8_t> value{0x0, 0x0, 0x1, 0x2, 0x3, 0x4};
    const std::uint32_t expected_output = 0x4030201;

    std::size_t offset{2};
    const std::size_t expected_offset{6};

    // Act
    const auto output = deserialize<std::uint32_t>(value, offset, false);

    // Assert
    ASSERT_EQ(offset, expected_offset);
    ASSERT_EQ(output, expected_output);
}

TEST_F(SerializationTest, deserialize_uint32_from_array)
{
    // Arrange
    const std::array<std::uint8_t, 6> value{0x0, 0x0, 0x1, 0x2, 0x3, 0x4};
    const std::uint32_t expected_output = 0x1020304;

    std::size_t offset{2};
    const std::size_t expected_offset{6};

    // Act
    const auto output = deserialize<std::uint32_t>(value, offset);

    // Assert
    ASSERT_EQ(offset, expected_offset);
    ASSERT_EQ(output, expected_output);
}

TEST_F(SerializationTest, deserialize_uint32_from_span_of_array)
{
    // Arrange
    const std::array<std::uint8_t, 6> value{0x0, 0x0, 0x1, 0x2, 0x3, 0x4};
    const score::cpp::span<const std::uint8_t> value_view{value};
    const std::uint32_t expected_output = 0x1020304;

    std::size_t offset{2};
    const std::size_t expected_offset{6};

    // Act
    const auto output = deserialize<std::uint32_t>(value_view, offset);

    // Assert
    ASSERT_EQ(offset, expected_offset);
    ASSERT_EQ(output, expected_output);
}

TEST_F(SerializationTest, serialize_and_allocate_uint16_to_vector_buffer)
{
    // Arrange
    const std::vector<std::uint8_t> expected_output = {0xA3, 0XCC};
    constexpr std::uint16_t value = 0xA3CC;

    // Act
    std::vector<std::uint8_t> serialized_output{};
    const auto result = serialize_and_allocate(serialized_output, value);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, sizeof(decltype(value)));

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
}

TEST_F(SerializationTest, serialize_and_allocate_uint16_to_static_vector_buffer)
{
    // Arrange
    const score::cpp::static_vector<std::uint8_t, 2> expected_output = {0xA3, 0XCC};
    constexpr std::uint16_t value = 0xA3CC;

    // Act
    score::cpp::static_vector<std::uint8_t, 2> serialized_output{};
    const auto result = serialize_and_allocate(serialized_output, value);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, sizeof(decltype(value)));

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
}

TEST_F(SerializationTest, serialize_in_place_uint16_to_array)
{
    // Arrange
    const std::array<std::uint8_t, 2> expected_output = {0xA3, 0XCC};
    constexpr std::uint16_t value = 0xA3CC;

    // Act
    std::array<std::uint8_t, 2> serialized_output{};
    std::int64_t offset = 0;
    serialize_in_place(serialized_output, value, offset);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, sizeof(decltype(value)));
}

TEST_F(SerializationTest, serialize_in_place_uint16_to_span_of_array)
{
    // Arrange
    const std::array<std::uint8_t, 2> expected_output = {0xA3, 0XCC};
    constexpr std::uint16_t value = 0xA3CC;

    // Act
    std::array<std::uint8_t, 2> serialized_output{};
    score::cpp::span<std::uint8_t> serialized_output_view{serialized_output};
    std::int64_t offset = 0;
    serialize_in_place(serialized_output_view, value, offset);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, sizeof(decltype(value)));
}

TEST_F(SerializationTest, serialize_and_allocate_uint32_to_vector_without_swapping_bytes)
{
    // Arrange
    const std::vector<std::uint8_t> expected_output = {0xA3, 0XCC, 0x12, 0x21};
    constexpr std::uint32_t value = 0x2112CCA3;

    // Act
    std::vector<std::uint8_t> serialized_output{};
    const auto result = serialize_and_allocate(serialized_output, value, false);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, sizeof(decltype(value)));

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
}

TEST_F(SerializationTest, serialize_and_allocate_uint32_to_static_vector_without_swapping_bytes)
{
    // Arrange
    const score::cpp::static_vector<std::uint8_t, 4> expected_output = {0xA3, 0XCC, 0x12, 0x21};
    constexpr std::uint32_t value = 0x2112CCA3;

    // Act
    score::cpp::static_vector<std::uint8_t, 4> serialized_output{};
    const auto result = serialize_and_allocate(serialized_output, value, false);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, sizeof(decltype(value)));

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
}

TEST_F(SerializationTest, serialize_in_place_uint32_to_array_without_swapping_bytes)
{
    // Arrange
    const std::array<std::uint8_t, 4> expected_output = {0xA3, 0XCC, 0x12, 0x21};
    constexpr std::uint32_t value = 0x2112CCA3;

    // Act
    std::array<std::uint8_t, 4> serialized_output{};
    std::int64_t offset = 0;
    serialize_in_place(serialized_output, value, offset, false);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, sizeof(decltype(value)));
}

TEST_F(SerializationTest, serialize_in_place_uint32_to_span_of_array_without_swapping_bytes)
{
    // Arrange
    const std::array<std::uint8_t, 4> expected_output = {0xA3, 0XCC, 0x12, 0x21};
    constexpr std::uint32_t value = 0x2112CCA3;

    // Act
    std::array<std::uint8_t, 4> serialized_output{};
    const score::cpp::span<const std::uint8_t> serialized_output_view{serialized_output};
    std::int64_t offset = 0;
    serialize_in_place(serialized_output, value, offset, false);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, sizeof(decltype(value)));
}

TEST_F(SerializationTest, deserialize_uint32_from_readonly_array)
{
    // Arrange
    const std::array<const std::uint8_t, 8> value{0x0, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6};
    const std::uint32_t expected_output = 0x1020304;

    std::size_t offset{2};
    const std::size_t expected_offset{6};

    // Act
    const auto output = deserialize<std::uint32_t>(value, offset);

    // Assert
    ASSERT_EQ(offset, expected_offset);
    ASSERT_EQ(output, expected_output);
}

TEST_F(SerializationTest, deserialize_uint32_from_readonly_span)
{
    // Arrange
    const std::array<std::uint8_t, 8> value{0x0, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6};
    const score::cpp::span<const std::uint8_t> value_view{value};
    const std::uint32_t expected_output = 0x1020304;

    std::size_t offset{2};
    const std::size_t expected_offset{6};

    // Act
    const auto output = deserialize<std::uint32_t>(value_view, offset);

    // Assert
    ASSERT_EQ(offset, expected_offset);
    ASSERT_EQ(output, expected_output);
}

TEST_F(SerializationTest, serialize_in_place_uint16_to_array_with_offset)
{
    // Arrange
    const std::array<std::uint8_t, 5> expected_output = {0xA3, 0xB3, 0xC3, 0xD3, 0xE3};
    constexpr std::uint16_t value = 0xB3C3;
    constexpr std::int64_t expected_initial_offset = 1;

    // Act
    std::array<std::uint8_t, 5> serialized_output{0xA3, 0x01, 0x02, 0xD3, 0xE3};
    std::int64_t offset = expected_initial_offset;
    serialize_in_place(serialized_output, value, offset);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, expected_initial_offset + sizeof(decltype(value)));
}

TEST_F(SerializationTest, serialize_in_place_uint16_to_array_with_offset_at_end)
{
    // Arrange
    const std::array<std::uint8_t, 5> expected_output = {0xA3, 0xB3, 0xC3, 0xD3, 0xE3};
    constexpr std::uint16_t value = 0xD3E3;
    constexpr std::int64_t expected_initial_offset = 3;

    // Act
    std::array<std::uint8_t, 5> serialized_output{0xA3, 0xB3, 0xC3, 0x01, 0x02};
    std::int64_t offset = expected_initial_offset;
    serialize_in_place(serialized_output, value, offset);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, expected_initial_offset + sizeof(decltype(value)));
}

TEST_F(SerializationTest, serialize_in_place_uint16_with_offset_to_array_without_swapping_bytes)
{
    // Arrange
    const std::array<std::uint8_t, 5> expected_output = {0xA3, 0xB3, 0xC3, 0xD3, 0xE3};
    constexpr std::uint16_t value = 0xC3B3;
    constexpr std::int64_t expected_initial_offset = 1;

    // Act
    std::array<std::uint8_t, 5> serialized_output{0xA3, 0x00, 0x00, 0xD3, 0xE3};
    std::int64_t offset = expected_initial_offset;
    serialize_in_place(serialized_output, value, offset, false);

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, expected_initial_offset + sizeof(decltype(value)));
}

TEST_F(SerializationTest, serialize_in_place_uint16_to_array_with_offset_exceeding_end_returns_false)
{
    // Arrange
    const std::array<std::uint8_t, 5> expected_output = {0xA3, 0xB3, 0xC3, 0xD3, 0xE3};
    constexpr std::uint16_t value = 0x0000;
    constexpr std::int64_t expected_initial_offset = 4;

    // Act
    std::array<std::uint8_t, 5> serialized_output{expected_output};
    std::int64_t offset = expected_initial_offset;
    ASSERT_DEATH(serialize_in_place(serialized_output, value, offset), ".*");

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, expected_initial_offset);
}

TEST_F(SerializationTest, serialize_in_place_uint16_to_array_with_insufficient_size_returns_false)
{
    // Arrange
    const std::array<std::uint8_t, 1> expected_output = {0xA3};
    constexpr std::uint16_t value = 0xB3C3;
    constexpr std::int64_t expected_initial_offset = 4;

    // Act
    std::array<std::uint8_t, 1> serialized_output{expected_output};
    std::int64_t offset = expected_initial_offset;
    ASSERT_DEATH(serialize_in_place(serialized_output, value, offset), ".*");

    // Assert
    ASSERT_EQ(serialized_output, expected_output);
    ASSERT_EQ(offset, expected_initial_offset);
}

}  // namespace testing
}  // namespace platform
}  // namespace score
