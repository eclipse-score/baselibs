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

#include "score/serialization/serialize_iterator.h"

#include "score/serialization/error/serialization_error.h"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <vector>

namespace score
{
namespace serialization
{
namespace
{

std::array<std::uint8_t, 512> global_buffer{};
std::array<std::uint8_t, 3> global_buffer_deserialization{1, 2, 3};
std::array<std::uint8_t, 0> global_buffer_empty{};

static std::tuple<std::vector<std::uint8_t>,
                  std::array<std::uint8_t, 512>,
                  score::cpp::static_vector<std::uint8_t, 512>,
                  std::list<std::uint8_t>,
                  std::deque<std::uint8_t>,
                  score::cpp::span<std::uint8_t>>
    allParams{{}, {}, {}, {}, {}, {global_buffer}};

static std::tuple<std::vector<std::uint8_t>,
                  std::array<std::uint8_t, 3>,
                  score::cpp::static_vector<std::uint8_t, 512>,
                  std::list<std::uint8_t>,
                  std::deque<std::uint8_t>,
                  score::cpp::span<std::uint8_t>>
    deserialization_params{{1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {global_buffer_deserialization}};

static std::tuple<std::vector<std::uint8_t>,
                  std::array<std::uint8_t, 0>,
                  std::list<std::uint8_t>,
                  std::deque<std::uint8_t>,
                  score::cpp::span<std::uint8_t>>
    emptyParams{{}, {}, {}, {}, {global_buffer_empty}};

template <typename T>
struct SerializeIteratorFixture : public ::testing::Test
{
    SerializeIteratorFixture() : buffer{std::get<T>(allParams)} {}
    T buffer;
};

template <typename T>
struct DeSerializeIteratorFixture : public ::testing::Test
{
    DeSerializeIteratorFixture() : buffer{std::get<T>(deserialization_params)} {}
    T buffer;
};

template <typename T>
struct DeSerializeIteratorFixtureEmptyBuffer : public ::testing::Test
{
    DeSerializeIteratorFixtureEmptyBuffer() : buffer{std::get<T>(emptyParams)} {}
    T buffer;
};

TYPED_TEST_SUITE_P(SerializeIteratorFixture);
TYPED_TEST_SUITE_P(DeSerializeIteratorFixture);
TYPED_TEST_SUITE_P(DeSerializeIteratorFixtureEmptyBuffer);

TYPED_TEST_P(SerializeIteratorFixture, WriteMultipleDataElementsViaIterator)
{
    // Given a buffer and a serialization iterator for that buffer
    auto iterator = SerializeInto(this->buffer);

    // When writing data into it
    iterator = 0x01;
    iterator = 0x02;

    // Then all data is written at the expected location
    auto data = this->buffer.begin();
    EXPECT_EQ(*data++, 0x01);
    EXPECT_EQ(*data++, 0x02);
}

TYPED_TEST_P(SerializeIteratorFixture, CanGetNumberOfWrittenElements)
{
    // Given a serialization iterator where two elements have been writen into
    auto iterator = SerializeInto(this->buffer);
    iterator = 0x01;
    iterator = 0x02;

    // When querying for the result of the operations
    const auto result = iterator.GetResult();

    // Then all data is written at the expected location
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);
}

TYPED_TEST_P(SerializeIteratorFixture, CanIndicateError)
{
    // Given a serialization iterator which is currently serializing data
    auto iterator = SerializeInto(this->buffer);
    iterator = 0x01;

    // When indicating that a serialization error appeared
    iterator = MakeUnexpected(SerializationError::kOutOfBoundsWrite);
    iterator = 0x02;

    // Then the issue is correctly reported, even if another positive operation happened.
    const auto result = iterator.GetResult();
    ASSERT_FALSE(result.has_value());
}

TYPED_TEST_P(DeSerializeIteratorFixture, CanIncrement)
{
    // Given a deserialization iterator for a buffer that contains three data elements
    auto iterator = DeSerializeIterator<TypeParam>(this->buffer);

    // When iterating twice
    iterator++;
    ++iterator;

    // Then we are at the correct position
    const auto result = iterator.GetResult();
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), 2);
    EXPECT_EQ(*iterator, 3);
}

TYPED_TEST_P(DeSerializeIteratorFixture, DoesNotAdvanceOverEnd)
{
    // Given a deserialization iterator for a buffer that contains three data elements
    auto iterator = DeSerializeIterator<TypeParam>(this->buffer);

    // When advancing more elements than available
    std::advance(iterator, 5);

    // Then an error is indicated
    const auto result = iterator.GetResult();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), SerializationError::kOutOfBoundsWrite);
}

TYPED_TEST_P(DeSerializeIteratorFixture, DoesNotAdvanceOverEndPostfix)
{
    // Given a deserialization iterator for a buffer that contains three data elements
    auto iterator = DeSerializeIterator<TypeParam>(this->buffer);

    // When advancing more elements than available
    for (std::int32_t i = 0; i < 5; ++i)
    {
        iterator++;
    }

    // No Error is indicated and the iterator does not move past the end.
    const auto result = iterator.GetResult();
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), this->buffer.size());
}

TYPED_TEST_P(DeSerializeIteratorFixture, CanGetEndFromIterator)
{
    // Given a deserialization iterator for a buffer that contains three data elements and its respective end iterator
    auto iterator = DeSerializeIterator<TypeParam>(this->buffer);
    const auto end = iterator.End();

    // When advancing more elements than available
    std::advance(iterator, 5);

    // Then both are end iterators
    EXPECT_TRUE(iterator == end);
}

TYPED_TEST_P(DeSerializeIteratorFixture, CheckBufferNotEmpty)
{
    // Given a deserialization iterator for a buffer that contains some elements
    auto iterator = DeSerializeIterator<TypeParam>(this->buffer);

    // The iterator should confirm that the container is not empty (begin != end)
    EXPECT_TRUE(iterator != iterator.End());
}

TYPED_TEST_P(DeSerializeIteratorFixtureEmptyBuffer, CheckBufferEmpty)
{
    // Given a deserialization iterator for a buffer that contains 0 elements
    auto iterator = DeSerializeIterator<TypeParam>(this->buffer);

    // The iterator should confirm that the container is empty
    EXPECT_TRUE(iterator == iterator.End());
}

REGISTER_TYPED_TEST_SUITE_P(SerializeIteratorFixture,
                            WriteMultipleDataElementsViaIterator,
                            CanGetNumberOfWrittenElements,
                            CanIndicateError);

REGISTER_TYPED_TEST_SUITE_P(DeSerializeIteratorFixture,
                            CanIncrement,
                            DoesNotAdvanceOverEnd,
                            DoesNotAdvanceOverEndPostfix,
                            CanGetEndFromIterator,
                            CheckBufferNotEmpty);

REGISTER_TYPED_TEST_SUITE_P(DeSerializeIteratorFixtureEmptyBuffer, CheckBufferEmpty);

using Types = testing::Types<std::vector<std::uint8_t>,
                             std::array<std::uint8_t, 512>,
                             score::cpp::static_vector<std::uint8_t, 512>,
                             std::list<std::uint8_t>,
                             std::deque<std::uint8_t>,
                             score::cpp::span<std::uint8_t>>;
INSTANTIATE_TYPED_TEST_SUITE_P(Typed, SerializeIteratorFixture, Types, /*unused*/);

using TypesForDeserializationTests = testing::Types<std::vector<std::uint8_t>,
                                                    std::array<std::uint8_t, 3>,
                                                    score::cpp::static_vector<std::uint8_t, 512>,
                                                    std::list<std::uint8_t>,
                                                    std::deque<std::uint8_t>,
                                                    score::cpp::span<std::uint8_t>>;
INSTANTIATE_TYPED_TEST_SUITE_P(Typed, DeSerializeIteratorFixture, TypesForDeserializationTests, /*unused*/);

using TypesForDeserializationEmptyBufferTests = testing::Types<std::vector<std::uint8_t>,
                                                               std::array<std::uint8_t, 0>,
                                                               std::list<std::uint8_t>,
                                                               std::deque<std::uint8_t>,
                                                               score::cpp::span<std::uint8_t>>;

INSTANTIATE_TYPED_TEST_SUITE_P(Typed, DeSerializeIteratorFixtureEmptyBuffer, TypesForDeserializationEmptyBufferTests,
                               /*unused*/);

TEST(SerializeIteratorTest, OverflowsOnStaticTypes)
{
    // Given some pre-allocated buffer that is already filled
    std::array<std::uint8_t, 2> buffer{};
    auto iterator = SerializeInto(buffer);
    iterator = 0x01;
    iterator = 0x02;

    // When adding more data into it and thus overflowing
    iterator = 0x03;
    iterator = 0x04;

    // Then our serialization was not successful
    const auto result = iterator.GetResult();
    ASSERT_FALSE(result.has_value());
}

TEST(SerializeIteratorTest, OverflowsOnBoundedContainer)
{
    // Given a static_vector whose capacity is smaller than the data we want to write
    score::cpp::static_vector<std::uint8_t, 1> buffer{};
    auto iterator = SerializeInto(buffer);

    // When writing more data than the capacity allows
    iterator = 0x01;
    iterator = 0x02;

    // Then our serialization was not successful instead of aborting
    const auto result = iterator.GetResult();
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SerializationError::kOutOfBoundsWrite);
}

TEST(SerializeIteratorTest, ProvideSerializationWindows)
{
    // Given a buffer where we want to serialize into an offset
    std::vector<std::uint8_t> buffer{};
    buffer.resize(8);
    auto view = score::cpp::span<std::uint8_t>{buffer};
    auto sub_view = view.subspan(4);
    auto iterator = SerializeInto(sub_view);

    // When serializing
    iterator = 0x01;
    iterator = 0x02;
    iterator = 0x03;

    // Then the correct number of bytes are serialized in the correct location
    const auto result = iterator.GetResult();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);

    auto data = buffer.begin();
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x01);
    EXPECT_EQ(*data++, 0x02);
    EXPECT_EQ(*data++, 0x03);
    EXPECT_EQ(*data++, 0x00);
}

TEST(SerializeIteratorTest, CountsOnlyBytesWrittenIntoNonEmptyGrowableContainer)
{
    // Given a growable container that already holds two bytes
    std::vector<std::uint8_t> buffer{0xAA, 0xBB};
    auto iterator = SerializeInto(buffer);

    // When serializing one more byte
    iterator = 0x01;

    // Then only the byte written through this iterator is counted, not the preexisting content
    const auto result = iterator.GetResult();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    // And the preexisting content is preserved, the new byte appended
    ASSERT_EQ(buffer.size(), 3U);
    EXPECT_EQ(buffer[0], 0xAA);
    EXPECT_EQ(buffer[1], 0xBB);
    EXPECT_EQ(buffer[2], 0x01);
}

}  // namespace
}  // namespace serialization
}  // namespace score
