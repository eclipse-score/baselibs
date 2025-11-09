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
#include "serialization/for_logging.h"
#include "serialization/visit_serialize.h"
#include "visitor_test_types.h"

#include <array>
#include <chrono>
#include <type_traits>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using test::assignable_container;
using test::clearable_container;
using test::resizeable_container;

template <typename T>
bool is_equal(const T& op1, const T& op2)
{
    return std::equal(op1.cbegin(),
                      op1.cbegin() + static_cast<std::ptrdiff_t>(op1.size()),  //
                      op2.cbegin(),
                      op2.cbegin() + static_cast<std::ptrdiff_t>(op2.size()));
}

struct alloc_t
{
    using offset_t = uint16_t;
    using subsize_t = uint8_t;
};

template <typename T>
std::size_t check_serialized()
{
    using S = ::score::common::visitor::serialized_t<alloc_t, T>;
    static_assert(std::is_standard_layout<S>::value, "serialized type does not fit standard layout requirement");
    static_assert(alignof(S) == 1, "serialized type does not fit byte alignment requirement");
    return sizeof(S);
}

namespace
{
constexpr auto subsize_index_first_byte = 4UL;
constexpr auto subsize_index_second_byte = 5UL;
constexpr auto number_of_elements_index_start = 0x00UL;
constexpr auto number_of_elements_max_index_size = 4UL;
constexpr auto GetNumberOfElementsIndex(const std::size_t index)
{
    return index;
}
template <typename T>
constexpr auto GetElementsMaxSize(T& arr)
{
    using BufferElementType = typename std::remove_reference<decltype(arr[0])>::type;
    return std::numeric_limits<BufferElementType>::max();
}
}  // namespace

namespace test
{

struct StructOneSigned
{
    int f1;
};

struct S2
{
    int f1, f2;
};

struct S3
{
    int f1, f2, f3;
    S3() = default;
    S3(S3&) = delete;
    S3(const S3&) = delete;
};

struct SS2S3
{
    S2 s2;
    S3 s3;
};

struct SS2S3r
{
    S2& s2;
    S3& s3;
};

struct Opt1
{
    score::cpp::optional<std::vector<int>> opt_int;
};

struct Bitset1
{
    std::bitset<10> bitset;
};

struct Duration1
{
    std::chrono::seconds seconds;
    std::chrono::milliseconds milliseconds;
    std::chrono::nanoseconds nanoseconds;
    std::chrono::microseconds microseconds;
};

STRUCT_VISITABLE(StructOneSigned, f1)
STRUCT_VISITABLE(S2, f1, f2)
STRUCT_VISITABLE(S3, f1, f2, f3)
STRUCT_VISITABLE(SS2S3, s2, s3)
STRUCT_VISITABLE(SS2S3r, s2, s3)
STRUCT_VISITABLE(Opt1, opt_int)
STRUCT_VISITABLE(Bitset1, bitset)
STRUCT_VISITABLE(Duration1, seconds, milliseconds, nanoseconds, microseconds)

template <typename T>
struct S1w
{
    T f1;
};

template <typename T>
class CustomAllocator : public std::allocator<T>
{
  public:
    template <typename U>
    struct rebind
    {
        using other = CustomAllocator<U>;
    };
};

struct S1wvA
{
    std::vector<int, CustomAllocator<int>> f1;
};

using S1wa4 = S1w<std::array<int, 4>>;

struct VectorOfArrays3Ints
{
    std::vector<std::array<std::int32_t, 3UL>> data;
};
struct S1wc1
{
    clearable_container<int> f1;
};
struct S1wc2
{
    clearable_container<int, std::allocator<int>> f1;
};
struct S1wc3
{
    resizeable_container<int> f1;
};
struct S1wc4
{
    resizeable_container<int, std::allocator<int>> f1;
};
struct S1wc5
{
    assignable_container<int> f1;
};
struct S1wc6
{
    assignable_container<int, std::allocator<int>> f1;
};

STRUCT_VISITABLE(S1w<std::string>, f1)
STRUCT_VISITABLE(S1w<std::vector<int>>, f1)
STRUCT_VISITABLE(S1wvA, f1)
STRUCT_VISITABLE(S1wa4, f1)
STRUCT_VISITABLE(S1wc1, f1)
STRUCT_VISITABLE(S1wc2, f1)
STRUCT_VISITABLE(S1wc3, f1)
STRUCT_VISITABLE(S1wc4, f1)
STRUCT_VISITABLE(S1wc5, f1)
STRUCT_VISITABLE(S1wc6, f1)
STRUCT_VISITABLE(VectorOfArrays3Ints, data)

enum E
{
    E0,
    E1
};

TEST(serializer_visitor, serialized)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the serialization for different data type.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_EQ(check_serialized<char>(), sizeof(char));
    EXPECT_EQ(check_serialized<uint8_t>(), sizeof(uint8_t));
    EXPECT_EQ(check_serialized<uint16_t>(), sizeof(uint16_t));
    EXPECT_EQ(check_serialized<uint32_t>(), sizeof(uint32_t));
    EXPECT_EQ(check_serialized<uint64_t>(), sizeof(uint64_t));
    EXPECT_EQ(check_serialized<int8_t>(), sizeof(int8_t));
    EXPECT_EQ(check_serialized<int16_t>(), sizeof(int16_t));
    EXPECT_EQ(check_serialized<int32_t>(), sizeof(int32_t));
    EXPECT_EQ(check_serialized<int64_t>(), sizeof(int64_t));
    EXPECT_EQ(check_serialized<float>(), sizeof(float));
    EXPECT_EQ(check_serialized<double>(), sizeof(double));

    EXPECT_EQ((check_serialized<std::pair<uint32_t, uint8_t>>()), (sizeof(uint32_t) + sizeof(uint8_t)));
    EXPECT_EQ((check_serialized<std::tuple<uint8_t>>()), (sizeof(uint8_t)));
    EXPECT_EQ((check_serialized<std::tuple<uint32_t, uint16_t>>()), (sizeof(uint32_t) + sizeof(int16_t)));
    EXPECT_EQ((check_serialized<std::tuple<uint32_t, uint16_t, uint8_t>>()),
              (sizeof(uint32_t) + sizeof(int16_t) + sizeof(uint8_t)));
    EXPECT_EQ((check_serialized<score::cpp::optional<uint32_t>>()), sizeof(bool) + sizeof(uint32_t));
    EXPECT_EQ((check_serialized<std::bitset<4>>()), sizeof(uint64_t));

    EXPECT_EQ(check_serialized<std::string>(), sizeof(alloc_t::offset_t));

    EXPECT_EQ(check_serialized<std::vector<std::string>>(), sizeof(alloc_t::offset_t));

    EXPECT_EQ((check_serialized<std::array<std::string, 4>>()), sizeof(alloc_t::offset_t) * 4);

    EXPECT_EQ((check_serialized<clearable_container<std::string>>()), sizeof(alloc_t::offset_t));
    EXPECT_EQ((check_serialized<resizeable_container<std::string>>()), sizeof(alloc_t::offset_t));
    EXPECT_EQ((check_serialized<assignable_container<std::string>>()), sizeof(alloc_t::offset_t));

    EXPECT_EQ(check_serialized<uint8_t[5]>(), sizeof(uint8_t) * 5);

    EXPECT_NE(check_serialized<test::SS2S3r>(), sizeof(test::SS2S3r));
    EXPECT_EQ(check_serialized<test::SS2S3>(), sizeof(test::SS2S3));

    EXPECT_EQ(check_serialized<test::E>(), sizeof(test::E));
}

struct real_alloc_t
{
    using offset_t = uint32_t;
    using subsize_t = uint16_t;
};

struct subsize_too_small_alloc_t
{
    using offset_t = uint32_t;
    using subsize_t = uint8_t;
};

TEST(serializer_visitor, serializer)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the serialization and deserialization for different data type.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    using namespace ::score::common::visitor;
    using s = serializer_t<real_alloc_t>;
    std::uint8_t buffer[1024];

    using BufferElementType = std::remove_reference<decltype(*buffer)>::type;
    constexpr auto max_buffer_element_value = std::numeric_limits<BufferElementType>::max();

    std::tuple<int, int> t23in{2, 3};
    std::tuple<int, int> t23out;
    EXPECT_EQ(s::serialize(t23in, buffer, sizeof(buffer)), 2 * sizeof(int));
    s::deserialize(buffer, sizeof(buffer), t23out);
    EXPECT_EQ(t23in, t23out);

    std::pair<int, int> p23in{2, 3};
    std::pair<int, int> p23out;
    EXPECT_EQ(s::serialize(p23in, buffer, sizeof(buffer)), 2 * sizeof(int));
    s::deserialize(buffer, sizeof(buffer), p23out);
    EXPECT_EQ(p23in, p23out);

    //  Test 'const' overload too:
    s::deserialize(const_cast<const std::uint8_t*>(buffer), sizeof(buffer), p23out);
    EXPECT_EQ(p23in, p23out);

    test::StructOneSigned struct_one_signed_in{4};
    test::StructOneSigned struct_one_signed_out;
    EXPECT_EQ(s::serialize(struct_one_signed_in, buffer, sizeof(buffer)), sizeof(test::StructOneSigned));
    s::deserialize(buffer, sizeof(buffer), struct_one_signed_out);
    EXPECT_EQ(struct_one_signed_in.f1, struct_one_signed_out.f1);

    test::S2 s2in{5, 6};
    test::S2 s2out;
    EXPECT_EQ(s::serialize(s2in, buffer, sizeof(buffer)), sizeof(test::S2));
    s::deserialize(buffer, sizeof(buffer), s2out);
    EXPECT_EQ(s2in.f1, s2out.f1);
    EXPECT_EQ(s2in.f2, s2out.f2);

    test::SS2S3 ss2s3in{};
    test::SS2S3 ss2s3out{};
    EXPECT_EQ(s::serialize(ss2s3in, buffer, sizeof(buffer)), sizeof(test::SS2S3));
    s::deserialize(buffer, sizeof(buffer), ss2s3out);
    EXPECT_EQ(ss2s3in.s2.f1, ss2s3out.s2.f1);
    EXPECT_EQ(ss2s3in.s3.f3, ss2s3out.s3.f3);

    test::Opt1 opt1_in;
    test::Opt1 opt1_out;
    s::serialize(opt1_in, buffer, sizeof(buffer));
    s::deserialize(buffer, sizeof(buffer), opt1_out);
    EXPECT_FALSE(opt1_out.opt_int.has_value());
    EXPECT_EQ(opt1_in.opt_int, opt1_out.opt_int);

    test::Opt1 opt1_in_2;
    opt1_in_2.opt_int = {1, 5, 10};
    test::Opt1 opt1_out_2;
    EXPECT_EQ(s::serialize(opt1_in_2, buffer, sizeof(buffer)),
              sizeof(uint32_t) + sizeof(uint16_t) + sizeof(int) * 3 + sizeof(bool));
    s::deserialize(buffer, sizeof(buffer), opt1_out_2);
    EXPECT_TRUE(opt1_out_2.opt_int.has_value());
    EXPECT_EQ(opt1_in_2.opt_int, opt1_out_2.opt_int);

    test::Bitset1 bitset_in{5};
    test::Bitset1 bitset_out;
    EXPECT_EQ(s::serialize(bitset_in, buffer, sizeof(buffer)), sizeof(uint64_t));
    s::deserialize(buffer, sizeof(buffer), bitset_out);
    EXPECT_EQ(bitset_out.bitset, bitset_in.bitset);
    EXPECT_EQ(bitset_out.bitset.size(), 10U);
    EXPECT_EQ(bitset_out.bitset.to_ulong(), 5U);
    EXPECT_EQ(bitset_out.bitset.count(), 2U);

    bitset_in.bitset.set(9, true);
    EXPECT_EQ(s::serialize(bitset_in, buffer, sizeof(buffer)), sizeof(uint64_t));
    s::deserialize(buffer, sizeof(buffer), bitset_out);
    EXPECT_EQ(bitset_out.bitset, bitset_in.bitset);
    EXPECT_EQ(bitset_out.bitset.size(), 10U);
    EXPECT_EQ(bitset_out.bitset.to_ulong(), 517U);
    EXPECT_EQ(bitset_out.bitset.count(), 3U);

    test::Duration1 duration1_in;
    test::Duration1 duration1_out;
    duration1_in.seconds = std::chrono::seconds{1};
    duration1_in.milliseconds = std::chrono::milliseconds{std::numeric_limits<std::chrono::milliseconds::rep>::max()};
    duration1_in.nanoseconds = std::chrono::nanoseconds{std::numeric_limits<std::chrono::nanoseconds::rep>::min()};
    duration1_in.microseconds = std::chrono::microseconds{0};
    EXPECT_EQ(s::serialize(duration1_in, buffer, sizeof(buffer)), sizeof(test::Duration1));
    s::deserialize(buffer, sizeof(buffer), duration1_out);
    EXPECT_EQ(duration1_in.seconds, duration1_out.seconds);
    EXPECT_EQ(duration1_in.milliseconds, duration1_out.milliseconds);
    EXPECT_EQ(duration1_in.nanoseconds, duration1_out.nanoseconds);
    EXPECT_EQ(duration1_in.microseconds, duration1_out.microseconds);

    std::vector<int> vector3in{11, 12, 13};
    std::vector<int> vector3out;
    EXPECT_EQ(s::serialize(vector3in, buffer, sizeof(buffer)), sizeof(uint32_t) + sizeof(uint16_t) + 3 * sizeof(int));
    s::deserialize(buffer, sizeof(buffer), vector3out);
    EXPECT_EQ(vector3in, vector3out);

    {  //  Test buffer corrupted - subsize set to overflow
        buffer[subsize_index_first_byte] = max_buffer_element_value;
        buffer[subsize_index_second_byte] = max_buffer_element_value;
        const auto result = s::deserialize(buffer, sizeof(buffer), vector3out);
        EXPECT_TRUE(result.getOutOfBounds());
    }

    const std::string string4in{"31323334"};
    std::string string4out;
    EXPECT_EQ(s::serialize(string4in, buffer, sizeof(buffer)),
              sizeof(uint32_t) + sizeof(uint16_t) + string4in.size() + 1);
    s::deserialize(buffer, sizeof(buffer), string4out);
    EXPECT_EQ(string4in, string4out);

    {  //  Test buffer corrupted - subsize set to zero
        buffer[subsize_index_first_byte] = 0x00;
        buffer[subsize_index_second_byte] = 0x00;
        const auto result = s::deserialize(buffer, sizeof(buffer), string4out);
        EXPECT_TRUE(result.getInvalidFormat());
    }

    {  //  Test buffer corrupted - subsize set to overflow
        buffer[subsize_index_first_byte] = max_buffer_element_value;
        buffer[subsize_index_second_byte] = max_buffer_element_value;
        const auto result = s::deserialize(buffer, sizeof(buffer), string4out);
        EXPECT_TRUE(result.getOutOfBounds());
    }

    {  //  Test int array
        const std::array<int, 4> array_of_4_in{{41, 42, 43, 44}};
        std::array<int, 4> array_of_4_out;
        EXPECT_EQ(s::serialize(array_of_4_in, buffer, sizeof(buffer)), 4 * sizeof(int));
        s::deserialize(buffer, sizeof(buffer), array_of_4_out);
        EXPECT_EQ(array_of_4_in, array_of_4_out);
    }

    int a5in[5] = {51, 52, 53, 54, 55};
    int a5out[5];
    EXPECT_EQ(s::serialize(a5in, buffer, sizeof(buffer)), 5 * sizeof(int));
    s::deserialize(buffer, sizeof(buffer), a5out);
    EXPECT_EQ(a5in[0], a5out[0]);
    EXPECT_EQ(a5in[4], a5out[4]);

    test::S1w<std::string> s1wsin = {"qwerty"};
    test::S1w<std::string> s1wsout;
    EXPECT_EQ(s::serialize(s1wsin, buffer, sizeof(buffer)), sizeof(uint32_t) + sizeof(uint16_t) + s1wsin.f1.size() + 1);
    s::deserialize(buffer, sizeof(buffer), s1wsout);
    EXPECT_EQ(s1wsin.f1, s1wsout.f1);

    test::S1w<std::vector<int>> s1wvin = {{1, 2, 3}};
    test::S1w<std::vector<int>> s1wvout;
    EXPECT_EQ(s::serialize(s1wvin, buffer, sizeof(buffer)), sizeof(uint32_t) + sizeof(uint16_t) + sizeof(int) * 3);
    s::deserialize(buffer, sizeof(buffer), s1wvout);
    EXPECT_EQ(s1wvin.f1, s1wvout.f1);

    test::S1wvA s1wvain = {{1, 2, 3}};
    test::S1wvA s1wvaout;
    EXPECT_EQ(s::serialize(s1wvain, buffer, sizeof(buffer)), sizeof(uint32_t) + sizeof(uint16_t) + sizeof(int) * 3);
    s::deserialize(buffer, sizeof(buffer), s1wvaout);
    EXPECT_EQ(s1wvain.f1, s1wvaout.f1);

    test::S1wa4 s1wain = {{{1, 2, 3, 4}}};
    test::S1wa4 s1waout;
    EXPECT_EQ(s::serialize(s1wain, buffer, sizeof(buffer)), sizeof(int) * 4);
    s::deserialize(buffer, sizeof(buffer), s1waout);
    EXPECT_EQ(s1wain.f1, s1waout.f1);

    test::S1wc1 s1wcin1 = {{1, 2, 3}};
    test::S1wc1 s1wcout1;
    EXPECT_EQ(s::serialize(s1wcin1, buffer, sizeof(buffer)), sizeof(uint32_t) + sizeof(uint16_t) + sizeof(int) * 3);
    s::deserialize(buffer, sizeof(buffer), s1wcout1);
    EXPECT_TRUE(is_equal(s1wcin1.f1, s1wcout1.f1));

    test::S1wc2 s1wcin2 = {{1, 2, 3}};
    test::S1wc2 s1wcout2;
    EXPECT_EQ(s::serialize(s1wcin2, buffer, sizeof(buffer)), sizeof(uint32_t) + sizeof(uint16_t) + sizeof(int) * 3);
    s::deserialize(buffer, sizeof(buffer), s1wcout2);
    EXPECT_TRUE(is_equal(s1wcin2.f1, s1wcout2.f1));

    {  //  Testing resizable container:
        test::S1wc3 s1wcin3 = {{1, 2, 3}};
        test::S1wc3 s1wcout3;
        EXPECT_EQ(s::serialize(s1wcin3, buffer, sizeof(buffer)), sizeof(uint32_t) + sizeof(uint16_t) + sizeof(int) * 3);
        s::deserialize(buffer, sizeof(buffer), s1wcout3);
        EXPECT_TRUE(is_equal(s1wcin3.f1, s1wcout3.f1));

        {  //  Test buffer corrupted - number of elements exceeds container's max_size()
            const size_t max_size{1};
            test::S1wc3 s1wcout3_with_maxsize{resizeable_container<int>(max_size)};
            const auto result = s::deserialize(buffer, sizeof(buffer), s1wcout3_with_maxsize);
            EXPECT_FALSE(s1wcout3_with_maxsize.f1.get_overflow());
            EXPECT_TRUE(result.getInvalidFormat());
        }

        {  //  Test buffer corrupted - subsize set to overflow
            buffer[subsize_index_first_byte] = max_buffer_element_value;
            buffer[subsize_index_second_byte] = max_buffer_element_value;
            const auto result = s::deserialize(buffer, sizeof(buffer), s1wcout3);
            EXPECT_TRUE(result.getOutOfBounds());
        }

        {  //  Test buffer corrupted - offset set to zero
            std::memset(&buffer[number_of_elements_index_start], 0x00, number_of_elements_max_index_size);
            const auto result = s::deserialize(buffer, sizeof(buffer), s1wcout3);
            EXPECT_TRUE(result.getZeroOffset());
        }

        {  //  Test buffer corrupted - offset set to overflow
            std::memset(
                &buffer[number_of_elements_index_start], max_buffer_element_value, number_of_elements_max_index_size);
            const auto result = s::deserialize(buffer, sizeof(buffer), s1wcout3);
            EXPECT_TRUE(result.getOutOfBounds());
        }
    }

    test::S1wc4 s1wcin4 = {{1, 2, 3}};
    test::S1wc4 s1wcout4;
    EXPECT_EQ(s::serialize(s1wcin4, buffer, sizeof(buffer)), sizeof(uint32_t) + sizeof(uint16_t) + sizeof(int) * 3);
    s::deserialize(buffer, sizeof(buffer), s1wcout4);
    EXPECT_TRUE(is_equal(s1wcin4.f1, s1wcout4.f1));

    test::S1wc5 s1wcin5 = {{1, 2, 3}};
    test::S1wc5 s1wcout5;
    EXPECT_EQ(s::serialize(s1wcin5, buffer, sizeof(buffer)), sizeof(uint32_t) + sizeof(uint16_t) + sizeof(int) * 3);
    s::deserialize(buffer, sizeof(buffer), s1wcout5);
    EXPECT_TRUE(is_equal(s1wcin5.f1, s1wcout5.f1));

    //  Test operation under condition that output initially has non-zero size:
    s1wcout5.f1.push_back({});
    s::deserialize(buffer, sizeof(buffer), s1wcout5);
    EXPECT_TRUE(is_equal(s1wcin5.f1, s1wcout5.f1));

    const test::VectorOfArrays3Ints vector_of_arrays_3_ints_in{{{1, 2, 3}, {7, 8, 9}, {4, 5, 6}}};
    test::VectorOfArrays3Ints vector_of_arrays_3_ints_out{};
    EXPECT_EQ(s::serialize(vector_of_arrays_3_ints_in, buffer, sizeof(buffer)),
              sizeof(uint32_t) + sizeof(uint16_t) + 9 * sizeof(int32_t));
    s::deserialize(buffer, sizeof(buffer), vector_of_arrays_3_ints_out);
    EXPECT_TRUE(is_equal(vector_of_arrays_3_ints_in.data, vector_of_arrays_3_ints_out.data));

    test::S1wc6 s1wcin6 = {{1, 2, 3}};
    test::S1wc6 s1wcout6;
    EXPECT_EQ(s::serialize(s1wcin6, buffer, sizeof(buffer)), sizeof(uint32_t) + sizeof(uint16_t) + sizeof(int) * 3);
    s::deserialize(buffer, sizeof(buffer), s1wcout6);
    EXPECT_TRUE(is_equal(s1wcin6.f1, s1wcout6.f1));
}

// TODO: [TicketOld-30390]  better tests for SERIALIZE-LAYOUT, SERIALIZE-SERIALIZER;
// more requirements and tests for corner cases (borderline sizes)
// and allocation error handling

using timestamp_t = std::chrono::steady_clock::time_point;

MEMCPY_SERIALIZABLE(score::common::visitor::payload_tags::unsigned_le, timestamp_t)

TEST(serializer_visitor, custom)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the serialization and deserialization for steady clock time_point.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    using s = ::score::common::visitor::serializer_t<real_alloc_t>;
    char buffer[1024];

    timestamp_t ttin = timestamp_t::clock::now();
    timestamp_t ttout;
    EXPECT_EQ(s::serialize(ttin, buffer, sizeof(buffer)), sizeof(ttin));
    s::deserialize(buffer, sizeof(buffer), ttout);
    EXPECT_EQ(ttin, ttout);
}

struct CustomFloat
{
    float value{0.0F};
};

bool operator==(const CustomFloat& lhs, const CustomFloat& rhs)
{
    return std::fabs(lhs.value - rhs.value) < 0.001F;
}

template <typename T>
struct is_custom_float : std::is_same<T, CustomFloat>
{
};

MEMCPY_SERIALIZABLE_IF(score::common::visitor::payload_tags::ieee754_float_le, T, is_custom_float<T>::value)

TEST(serializer_visitor, serialize_unit)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the serialization and deserialization for a struct type.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    using s = ::score::common::visitor::serializer_t<real_alloc_t>;
    char buffer[1024];

    CustomFloat vin{42.0F};
    CustomFloat vout;
    EXPECT_EQ(s::serialize(vin, buffer, sizeof(buffer)), sizeof(vin));
    s::deserialize(buffer, sizeof(buffer), vout);
    EXPECT_EQ(vin, vout);
}

struct PotentiallyTooBigSubstructure
{
    std::vector<char> dummyVector;
};

STRUCT_VISITABLE(PotentiallyTooBigSubstructure, dummyVector)

inline bool operator==(const PotentiallyTooBigSubstructure& lhs, const PotentiallyTooBigSubstructure& rhs) noexcept
{
    return (lhs.dummyVector == rhs.dummyVector);
}

struct PotentiallyTooBigStructure
{
    std::array<uint8_t, 1024> staticPart;
    std::vector<PotentiallyTooBigSubstructure> dynamicPart;
    std::string potentiallyTooBigString;
};

STRUCT_VISITABLE(PotentiallyTooBigStructure, staticPart, dynamicPart, potentiallyTooBigString)

inline bool operator==(const PotentiallyTooBigStructure& lhs, const PotentiallyTooBigStructure& rhs) noexcept
{
    return (lhs.staticPart == rhs.staticPart) && (lhs.dynamicPart == rhs.dynamicPart) &&
           (lhs.potentiallyTooBigString == rhs.potentiallyTooBigString);
}

class serializer_visitor_overflows : public ::testing::Test
{
  public:
    using result_type = std::pair<score::common::visitor::deserialization_result_t, bool>;

    template <typename ALLOC_TYPE = real_alloc_t, typename CAST_INPUT_TO_TYPE = char*>
    result_type ThereAndBackWithErrorCheck(const PotentiallyTooBigStructure& input_data,
                                           std::size_t sizeIn,
                                           std::size_t sizeOut)
    {
        using s = ::score::common::visitor::serializer_t<ALLOC_TYPE>;
        std::vector<char> buffer(sizeIn);

        s::serialize(input_data, buffer.data(), static_cast<uint32_t>(buffer.size()));

        PotentiallyTooBigStructure replicated_data{};
        replicated_data.potentiallyTooBigString = "";
        buffer.resize(sizeOut);

        auto result = s::deserialize(
            reinterpret_cast<CAST_INPUT_TO_TYPE>(buffer.data()), static_cast<uint32_t>(buffer.size()), replicated_data);

        return std::make_pair(result, input_data == replicated_data);
    }

    serializer_visitor_overflows() : normalStructure{}, structureWithHugeDynamicPart{}, structureWithALongString{}
    {
        normalStructure.dynamicPart.push_back(PotentiallyTooBigSubstructure{std::vector<char>(100)});

        structureWithHugeDynamicPart.dynamicPart.resize(200);
        for (auto&& x : structureWithHugeDynamicPart.dynamicPart)
        {
            x.dummyVector.push_back('a');
        }
        structureWithHugeDynamicPart.dynamicPart.at(20).dummyVector.resize(2000);

        structureWithALongString.dynamicPart.resize(0);
        structureWithALongString.potentiallyTooBigString.resize(1024);
        for (size_t i = 0; i < 1024; i++)
        {
            structureWithALongString.potentiallyTooBigString.at(i) = static_cast<char>(i % 128);
        }
    }

    void SetUp() override {}

    void TearDown() override {}

    ~serializer_visitor_overflows() {}

    PotentiallyTooBigStructure normalStructure;
    PotentiallyTooBigStructure structureWithHugeDynamicPart;
    PotentiallyTooBigStructure structureWithALongString;
};

TEST_F(serializer_visitor_overflows, basic__no_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The serialization and deserialization for a normal struct shall success when providing the "
                   "serialized and the deserialized buffers with the same sizes.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    result_type result = ThereAndBackWithErrorCheck(normalStructure, 2048, 2048);
    EXPECT_EQ(result.first.operator bool(), true);
    EXPECT_EQ(result.second, true);
}

TEST_F(serializer_visitor_overflows, basic__serializer_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The serialization and deserialization for a normal struct shall overflow and reach zero offset "
                   "when deserialize more data than the serialized one.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    result_type result = ThereAndBackWithErrorCheck(normalStructure, 100, 2048);
    EXPECT_EQ(result.first.getZeroOffset(), true);
    EXPECT_EQ(result.second, false);
}

TEST_F(serializer_visitor_overflows, basic__derserializer_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The serialization and deserialization for a normal struct shall overflow for reaching out of "
                   "bounds when deserialize less data than the serialized one.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    result_type result = ThereAndBackWithErrorCheck(normalStructure, 2048, 100);
    EXPECT_EQ(result.first.getOutOfBounds(), true);
    EXPECT_EQ(result.second, false);
}

TEST_F(serializer_visitor_overflows, basic_deserializer_overflow_const)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The serialization and deserialization for a normal struct shall overflow for reaching out of "
                   "bounds when deserialize less data than the serialized one - const type.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    constexpr auto size_in = 2048UL;
    constexpr auto size_out = 100UL;
    result_type result =
        ThereAndBackWithErrorCheck<real_alloc_t, const std::uint8_t*>(normalStructure, size_in, size_out);
    EXPECT_EQ(result.first.getOutOfBounds(), true);
    EXPECT_EQ(result.second, false);
}

TEST_F(serializer_visitor_overflows, dynamic_part__no_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "The serialization and deserialization for a struct with a huge dynamic part shall success when providing the "
        "serialized and the deserialized buffers with the same sizes when allocate a dynamic part.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    structureWithHugeDynamicPart.dynamicPart.resize(100);
    result_type result = ThereAndBackWithErrorCheck(structureWithHugeDynamicPart, 4096, 4096);
    EXPECT_EQ(result.first.operator bool(), true);
    EXPECT_EQ(result.second, true);
}

TEST_F(serializer_visitor_overflows, dynamic_part__serializer_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "The serialization and deserialization for a struct with a huge dynamic part shall overflow when providing the "
        "serialized and the deserialized buffers with the same sizes but without allocating a dynamic part.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    constexpr auto size_in_out = 4096UL;
    result_type result = ThereAndBackWithErrorCheck(structureWithHugeDynamicPart, size_in_out, size_in_out);
    EXPECT_EQ(result.first.getZeroOffset(), true);
    EXPECT_EQ(result.second, false);
}

TEST_F(serializer_visitor_overflows, dynamic_part_serializer_overflow_too_small_subsize)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Logging library shall provide an annotation mechanism for data structures to support automatic "
                   "serialization/deserialization and handle subsize overflows returning the ZeroOffset status.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    constexpr auto size_in_out = 4096UL;
    result_type result =
        ThereAndBackWithErrorCheck<subsize_too_small_alloc_t>(structureWithHugeDynamicPart, size_in_out, size_in_out);
    EXPECT_EQ(result.first.getZeroOffset(), true);
    EXPECT_EQ(result.second, false);
}

TEST_F(serializer_visitor_overflows, dynamic_part__deserilizer_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The serialization and deserialization for a struct with a huge dynamic part shall overflow for "
                   "reaching out of bounds when deserialize less data than the serialized one.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    result_type result = ThereAndBackWithErrorCheck(structureWithHugeDynamicPart, 8192, 4096);
    EXPECT_EQ(result.first.getOutOfBounds(), true);
    EXPECT_EQ(result.second, false);
}

TEST_F(serializer_visitor_overflows, string__no_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The serialization and deserialization for a string data shall success when providing the "
                   "serialized and the deserialized buffers with the same sizes.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    result_type result = ThereAndBackWithErrorCheck(structureWithALongString, 4096, 4096);
    EXPECT_EQ(result.first.operator bool(), true);
    EXPECT_EQ(result.second, true);
}

TEST_F(serializer_visitor_overflows, string__serialization_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The serialization and deserialization for a string data shall overflow and reach zero offset when "
                   "deserialize more data than the serialized one.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    result_type result = ThereAndBackWithErrorCheck(structureWithALongString, 2048, 4096);
    EXPECT_EQ(result.first.getZeroOffset(), true);
    EXPECT_EQ(result.second, false);
}

TEST_F(serializer_visitor_overflows, string_deserialization_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861827, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The serialization and deserialization for a string data shall overflow for reaching out of bounds "
                   "when deserialize less data than the serialized one.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    result_type result = ThereAndBackWithErrorCheck(structureWithALongString, 4096, 2048);
    EXPECT_EQ(result.first.getOutOfBounds(), true);
    EXPECT_EQ(result.second, false);
}

TEST_F(serializer_visitor_overflows, test_logger_type_info_copy_size_overflow)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test the inability of logger_type_info API to copy data bigger than the size.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    constexpr auto cmpr = std::numeric_limits<char>::is_signed ? 0x7f : 0xff;
    constexpr auto array_size = 64UL;
    std::array<char, array_size> buffer;

    constexpr auto max_buffer_element_value = GetElementsMaxSize(buffer);

    std::memset(buffer.data(), max_buffer_element_value, buffer.size());

    //  Execute copy operation that will try to put serialized number of characters and name of the type
    //  'StructOneSigned' into a buffer
    ::score::common::visitor::logger_type_info<StructOneSigned>().copy(buffer.data(), sizeof(std::int16_t));
    // The datatype uint16 Overflowed (0x7f)
    EXPECT_EQ(buffer[GetNumberOfElementsIndex(0)], cmpr);
    EXPECT_EQ(buffer[GetNumberOfElementsIndex(1)], cmpr);
    EXPECT_EQ(buffer[GetNumberOfElementsIndex(2)], cmpr);
    EXPECT_EQ(buffer[GetNumberOfElementsIndex(3)], cmpr);
}

TEST_F(serializer_visitor_overflows, test_logger_type_info_copy_size_not_fit)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test the inability of logger_type_info API to copy data that does not fit size.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    constexpr auto array_size = 64UL;
    std::array<char, array_size> buffer;

    constexpr auto max_buffer_element_value = GetElementsMaxSize(buffer);

    std::memset(buffer.data(), max_buffer_element_value, buffer.size());

    //  Execute copy operation that will try to put serialized number of characters and name of the type
    //  'StructOneSigned' into a buffer
    ::score::common::visitor::logger_type_info<StructOneSigned>().copy(buffer.data(), sizeof(std::uint32_t));
    EXPECT_EQ(buffer[GetNumberOfElementsIndex(0)], 0x00);
    EXPECT_EQ(buffer[GetNumberOfElementsIndex(1)], 0x00);
    EXPECT_EQ(buffer[GetNumberOfElementsIndex(2)], 0x00);
    EXPECT_EQ(buffer[GetNumberOfElementsIndex(3)], 0x00);
}

TEST_F(serializer_visitor_overflows, test_logger_type_info_copy_size_fits)
{
    RecordProperty("ParentRequirement", "SCR-1633893, SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test the ability of logger_type_info to copy data.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    constexpr auto array_size = 64UL;
    std::array<char, array_size> buffer{0};

    const auto type_info_inst = ::score::common::visitor::logger_type_info<StructOneSigned>();
    const std::size_t type_info_size = type_info_inst.size();
    type_info_inst.copy(buffer.data(), type_info_size + 1);
    // text starts at 3-rd byte
    EXPECT_STREQ(&buffer[4], "test::StructOneSigned");
}

TEST(visit_optional_pack_desc_test, optional_pack_desc_should_return_correct_field_name)
{
    RecordProperty("Description",
                   "Test optional_pack_desc's field_name function for providing name to optional variable");
    using namespace ::score::common::visitor;
    serialized_visitor<double> visitor_local{};
    EXPECT_EQ(decltype(visit(visitor_local, score::cpp::optional<double>(1.0)))::pack_desc::field_name(0U), "has_value");
    EXPECT_EQ(decltype(visit(visitor_local, score::cpp::optional<double>(1.0)))::pack_desc::field_name(1U), "data");
    EXPECT_EQ(decltype(visit(visitor_local, score::cpp::optional<double>(1.0)))::pack_desc::field_name(2U), "");
    EXPECT_EQ(decltype(visit(visitor_local, score::cpp::optional<double>(1.0)))::pack_desc::field_name(4U), "");
}

TEST(logging_serializer_test, serialize_int_data_with_big_miss_match_size)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verify the inability of serialize integer data by providing a size bigger than"
                   "the original data size.");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::tuple<int, int> tuple_instance{1, 2};
    std::uint8_t buffer[1024];

    score::common::visitor::logging_serializer serialize_obj;
    auto offset = serialize_obj.serialize(tuple_instance, buffer, std::numeric_limits<uint64_t>::max());
    EXPECT_EQ(offset, 0);
}

TEST(logging_serializer_test, deserialize_int_data_with_big_miss_match_size)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verify the inability of deserialize integer data by providing a size bigger than"
                   "the original data size.");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::tuple<int, int> tuple_instance_in{1, 2};
    std::tuple<int, int> tuple_instance_out;
    std::uint8_t buffer[1024];

    score::common::visitor::logging_serializer serialize_obj;
    serialize_obj.serialize(tuple_instance_in, buffer, std::numeric_limits<uint64_t>::max());

    auto deserialization_result =
        serialize_obj.deserialize(buffer, std::numeric_limits<uint64_t>::max(), tuple_instance_out);
    EXPECT_EQ(deserialization_result.getOutOfBounds(), true);
    EXPECT_EQ(deserialization_result.getInvalidFormat(), false);
    EXPECT_EQ(deserialization_result.getZeroOffset(), false);
}

TEST(logging_serializer_test, deserialize_byte_data_with_miss_match_size)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verify the inability of deserialize byte data by providing a size bigger than"
                   "the original data size.");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    test::StructOneSigned struct_one_signed_out;
    char serialized_buffer[1024];

    score::common::visitor::logging_serializer serialize_obj;

    auto deserialization_result =
        serialize_obj.deserialize(serialized_buffer, std::numeric_limits<uint64_t>::max(), struct_one_signed_out);
    EXPECT_EQ(deserialization_result.getOutOfBounds(), true);
    EXPECT_EQ(deserialization_result.getInvalidFormat(), false);
    EXPECT_EQ(deserialization_result.getZeroOffset(), false);
}

class VectorWrapper
{
  public:
    void clear()
    {
        vector_of_int.clear();
    }
    std::vector<std::int32_t> vector_of_int{1, 2, 3, 4, 5};
};

TEST(clear_functionality_test, test_that_clear_function_can_clear_vector_of_int32)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the inability of clearing vector of int32.");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    VectorWrapper vector_wrapper_instance{};
    score::common::visitor::detail::clear(vector_wrapper_instance);
}

}  // namespace test
