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

#include "score/hash/code/core/i_hash_calculator.h"
#include "score/hash/code/common/error.h"

#include "score/result/result.h"
#include "score/mw/log/logging.h"

#include <score/utility.hpp>

#include <iostream>

namespace score
{
namespace hash
{

namespace
{
/// @brief Size of data read from stream.
constexpr std::size_t kDataChunkSize{4096U};

/// @brief Unlimited reading.
constexpr std::int64_t kReadEndlessFromStream{-1};

using UpdateMethodPtr = ResultBlank (IHashCalculator::*)(const score::cpp::span<const std::uint8_t>&);

/// @brief Auxiliary class for parsing Streams.
/// @details Made a separated class to avoid keeping state on the interface level.
class StreamConsumer
{
    std::array<char, kDataChunkSize> read_buffer;
    ssize_t block_size;                // Sized as error is indicated with negative value
    std::int64_t total_size;           // Total size of data read from stream so far
    IHashCalculator& updater;          // Reference to the instance of the hash calculator
    const std::int64_t expected_size;  // Expected size of data to read from stream

    bool HasReadEnough() const noexcept
    {
        return ((expected_size != kReadEndlessFromStream) && (total_size >= expected_size));
    }

    std::streamsize CalculateNumberBytesToRead() const noexcept
    {
        if (expected_size == kReadEndlessFromStream)
        {
            return static_cast<std::streamsize>(kDataChunkSize);
        }

        const auto remaining = expected_size - total_size;
        const auto min_remaining = std::min(remaining, static_cast<std::int64_t>(kDataChunkSize));
        return static_cast<std::streamsize>(min_remaining);
    }

    void ReadBlock(std::istream& input)
    {
        auto number_bytes_to_read = CalculateNumberBytesToRead();
        score::cpp::ignore = input.read(read_buffer.data(), number_bytes_to_read);
        block_size = input.gcount();
    }

    void accumulate()
    {
        // HasReadEnough and CalculateNumberBytesToRead handles overflow scenarios of total_size
        //  coverity[autosar_cpp14_a4_7_1_violation] See above
        total_size += block_size;
        block_size = 0;
    }

    const score::cpp::span<const std::uint8_t> ConvertToSpan() const noexcept
    {
        // Suppress "UNUSED C++14 A5-2-4" rule finding: "Reinterpret_cast shall not be used":
        // The `Update` method requires a span of bytes (uint8_t) as input.
        // The data read from the stream is stored in an array of chars (for compatibility with the std::istream::read
        // method). Thus here we cast the array of chars to an array of uint8_t, but this shall not cause any problem,
        // as the data types are compatible in size.

        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) See above
        // coverity[autosar_cpp14_a5_2_4_violation] See above
        auto casted_buffer_data = reinterpret_cast<const std::uint8_t*>(read_buffer.data());
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        return {casted_buffer_data, static_cast<score::cpp::span<const std::uint8_t>::size_type>(block_size)};
    }

  public:
    StreamConsumer(IHashCalculator& updater_instance, const std::int64_t expected_size_in_bytes) noexcept
        : read_buffer{}, block_size{}, total_size{}, updater{updater_instance}, expected_size{expected_size_in_bytes}
    {
    }

    ResultBlank Consume(std::istream& input)
    {
        // GCOVR is unable to analyze line below. But all decision are taken in for while declaration.
        while (input.good() == true)  // LCOV_EXCL_BR_LINE rationale above
        {
            if (HasReadEnough() == true)
            {
                return {};
            }

            ReadBlock(input);
            if ((block_size == 0) && (input.eof() == true))
            {
                return {};
            }

            const auto data = ConvertToSpan();
            auto update_result = updater.Update(data);
            if (update_result.has_value() == false)
            {
                mw::log::LogError() << "IHashCalculator::Input data might be invalid " << __func__;
                return MakeUnexpected<score::Blank>(update_result.error());
            }

            accumulate();
        }

        if ((input.eof() == false) || (input.bad() == true))
        {
            mw::log::LogError() << "IHashCalculator::" << __func__ << " unable to read to end, digest incomplete";
            return MakeUnexpected(ErrorCode::kCouldNotUpdateDigestFromStream);
        }

        return {};
    }
};

}  // namespace

ResultBlank IHashCalculator::UpdateFromStream(std::istream& input)
{
    return UpdateFromStream(input, kReadEndlessFromStream);
}

ResultBlank IHashCalculator::UpdateFromStream(std::istream& input, const std::int64_t max_read)
{
    if (input.good() == false)
    {
        mw::log::LogError() << "IHashCalculator::Stream error state flag set " << __func__;
        return MakeUnexpected(ErrorCode::kStreamError);
    }

    StreamConsumer consumer{*this, max_read};
    return consumer.Consume(input);
}

}  // namespace hash
}  // namespace score
