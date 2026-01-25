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
#include "score/utils/base64.h"
#include <score/utility.hpp>
#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace score::utils
{

namespace
{

constexpr std::string_view base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
constexpr std::size_t kEncodeInputBufferSize{3U};
constexpr std::size_t kEncodeOutputBufferSize{4U};
constexpr std::size_t kDecodeInputBufferSize{4U};
constexpr std::size_t kDecodeOutputBufferSize{3U};
using EncodeInputBuffer = std::array<std::uint8_t, kEncodeInputBufferSize>;
using EncodeOutputBuffer = std::array<std::uint8_t, kEncodeOutputBufferSize>;
using DecodeInputBuffer = std::array<std::uint8_t, kDecodeInputBufferSize>;
using DecodeOutputBuffer = std::array<std::uint8_t, kDecodeOutputBufferSize>;

// Suppress "AUTOSAR C++14 A0-1-2", The rule states: "The value returned by a function having a non-void return
// type that is not an overloaded operator shall be used." This is false positive. array assigning returns void.
// coverity[autosar_cpp14_a0_1_2_violation : FALSE]
void PerformBase64EncodeStep(const EncodeInputBuffer& inputBuffer, EncodeOutputBuffer& outputBuffer) noexcept
{
    outputBuffer[0] = static_cast<std::uint8_t>((inputBuffer[0] & 0xFCU) >> 2U);
    outputBuffer[1] = static_cast<std::uint8_t>(static_cast<std::uint8_t>(((inputBuffer[0] & 0x03U) << 4U)) +
                                                static_cast<std::uint8_t>(((inputBuffer[1] & 0xF0U) >> 4U)));
    outputBuffer[2] = static_cast<std::uint8_t>(static_cast<std::uint8_t>(((inputBuffer[1] & 0x0FU) << 2U)) +
                                                static_cast<std::uint8_t>(((inputBuffer[2] & 0xC0U) >> 6U)));
    outputBuffer[3] = inputBuffer[2] & 0x3FU;
}

// Suppress "AUTOSAR C++14 A0-1-2", The rule states: "The value returned by a function having a non-void return
// type that is not an overloaded operator shall be used." This is false positive. array assigning returns void.
// coverity[autosar_cpp14_a0_1_2_violation : FALSE]
void PerformBase64DecodeStep(const DecodeInputBuffer& inputBuffer, DecodeOutputBuffer& outputBuffer) noexcept
{
    outputBuffer[0] = static_cast<std::uint8_t>(static_cast<std::uint8_t>((inputBuffer[0] << 2U)) +
                                                static_cast<std::uint8_t>(((inputBuffer[1] & 0x30U) >> 4U)));
    outputBuffer[1] = static_cast<std::uint8_t>(static_cast<std::uint8_t>(((inputBuffer[1] & 0xFU) << 4U)) +
                                                static_cast<std::uint8_t>(((inputBuffer[2] & 0x3CU) >> 2U)));
    // the c++17 standard in 6.9.1 basic.fundamental defines unsigned interger overflow as well defined
    // and 21.2.4 defines size_t to be an unsigned integer type
    // coverity[autosar_cpp14_a4_7_1_violation]
    outputBuffer[2] = static_cast<std::uint8_t>(static_cast<std::uint8_t>(((inputBuffer[2] & 0x3U) << 6U)) +
                                                static_cast<std::uint8_t>(inputBuffer[3]));
}
}  // namespace

std::string EncodeBase64(const std::vector<std::uint8_t>& buffer)
{
    std::string ret;
    EncodeInputBuffer inputBuffer{};
    EncodeOutputBuffer outputBuffer{};
    // the c++17 standard in 6.9.1 basic.fundamental defines unsigned interger overflow as well defined
    // and 21.2.4 defines size_t to be an unsigned integer type
    // the statement below is an optimization. Overflow in its integer expression does not lead to data loss
    // coverity[autosar_cpp14_a4_7_1_violation]
    ret.reserve((((buffer.size() + 2U) / kEncodeInputBufferSize) * kEncodeOutputBufferSize));
    std::size_t ind = 0U;
    for (const auto it : buffer)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) bounds already checked
        inputBuffer[ind] = it;
        ind++;
        if (ind == kEncodeInputBufferSize)
        {
            PerformBase64EncodeStep(inputBuffer, outputBuffer);
            score::cpp::ignore = std::transform(std::begin(outputBuffer),
                                         std::end(outputBuffer),
                                         std::back_inserter(ret),
                                         [](EncodeOutputBuffer::value_type v) {
                                             return base64_chars[v];
                                         });
            ind = 0U;
        }
    }
    if (ind != 0U)
    {
        const auto ind_diff =
            static_cast<std::iterator_traits<EncodeInputBuffer::const_iterator>::difference_type>(ind);
        std::fill(std::next(std::begin(inputBuffer), ind_diff), std::end(inputBuffer), 0U);
        PerformBase64EncodeStep(inputBuffer, outputBuffer);
        score::cpp::ignore = std::transform(std::begin(outputBuffer),
                                     std::next(std::begin(outputBuffer), ind_diff + 1),
                                     std::back_inserter(ret),
                                     [](EncodeOutputBuffer::value_type v) {
                                         return base64_chars[v];
                                     });
        score::cpp::ignore =
            std::fill_n(std::back_inserter(ret), kEncodeInputBufferSize - static_cast<std::size_t>(ind_diff), '=');
    }
    return ret;
}

std::vector<std::uint8_t> DecodeBase64(const std::string& encoded_string)
{
    std::vector<std::uint8_t> ret;
    DecodeInputBuffer inputBuffer{};
    DecodeOutputBuffer outputBuffer{};

    // the c++17 standard in 6.9.1 basic.fundamental defines unsigned interger overflow as well defined
    // and 21.2.4 defines size_t to be an unsigned integer type
    // the statement below is an optimization. Overflow in its integer expression does not lead to data loss
    // coverity[autosar_cpp14_a4_7_1_violation]
    ret.reserve((encoded_string.size() * kDecodeOutputBufferSize + 1U) / kDecodeInputBufferSize);
    std::size_t ind = 0U;

    // GCOVR is unable to analyze line below. But all decisions are covered with unit test and all possible outcome of
    // a for loop are tested.
    for (const auto it : encoded_string)
    {
        if (auto found = base64_chars.find(it); found != std::string::npos)
        {
            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index) bounds already checked
            // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
            // not lead to data loss.".
            // Rationale: casting from char to std:unit8_t won't cause data loss.
            // coverity[autosar_cpp14_a4_7_1_violation]
            inputBuffer[ind] = static_cast<std::uint8_t>(found);
            // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
        }
        else
        {
            break;
        }
        // The increment (++) and decrement (--) operators shall not be mixed with other operators in an expression.
        ++ind;  // AUTOSAR C++14 M5-2-10
        if (ind == kDecodeInputBufferSize)
        {
            PerformBase64DecodeStep(inputBuffer, outputBuffer);
            score::cpp::ignore = std::copy(std::begin(outputBuffer), std::end(outputBuffer), std::back_inserter(ret));

            // when index is 4, reset it to 0 to avoid buffer overflow with inputBuffer
            ind = 0U;
        }
    }
    if (ind != 0U)
    {
        const auto ind_diff =
            static_cast<std::iterator_traits<DecodeOutputBuffer::const_iterator>::difference_type>(ind);
        std::fill(std::next(std::begin(inputBuffer), ind_diff), std::end(inputBuffer), 0xFFU);
        PerformBase64DecodeStep(inputBuffer, outputBuffer);
        score::cpp::ignore = std::copy(
            std::begin(outputBuffer), std::next(std::begin(outputBuffer), ind_diff - 1), std::back_inserter(ret));
    }
    return ret;
}

}  // namespace score::utils
