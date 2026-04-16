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
/*!        \file
 *        \brief  json ops
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/json_ops.h"
#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "score/blank.hpp"
#include "score/span.hpp"

#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
namespace
{

/*!
 /// \brief            Split a StringView into two at the specified pivot point
 /// \details         The pivot element will be contained in the second view.
 /// \param[in]       view                  The StringView to split
 /// \param[in]       pivot                  The index on which to split.
 /// \return          A pair of string views.
 *
 /// \context         ANY
 /// \pre             -
 /// \threadsafe      TRUE
 /// \reentrant       TRUE
 */
inline auto SplitStringView(std::string_view const view, std::size_t const pivot) noexcept
    -> std::pair<std::string_view, std::string_view>
{
    return {view.substr(0, pivot), view.substr(pivot)};
}

}  // namespace

JsonOps::JsonOps(JsonData& json_data) noexcept : data_(json_data) {}

auto JsonOps::Take() noexcept -> char
{
    return this->TryTake().value();
}

auto JsonOps::TryTake() noexcept -> Result<char>
{
    std::istream& stream = this->GetStream();
    std::int64_t c = stream.get();
    Result<char> result{MakeErrorResult<char>(JsonErrc::kStreamFailure, "JsonOps::TryTake: EOF.")};

    if (c != std::char_traits<char>::eof())
    {
        result = Result<char>{std::char_traits<char>::to_char_type(static_cast<std::int32_t>(c))};
    }

    return result;
}

auto JsonOps::Move() noexcept -> bool
{
    std::istream& stream = this->GetStream();
    stream.seekg(1, std::ios::cur);
    return !stream.fail();
}

auto JsonOps::Tell() const noexcept -> Result<std::uint64_t>
{
    std::istream& stream = const_cast<std::istream&>(this->GetStream());
    std::streampos pos = stream.tellg();
    Result<std::uint64_t> result{static_cast<std::uint64_t>(pos)};
    if (pos == std::streampos(-1))
    {
        result =
            MakeErrorResult<std::uint64_t>(JsonErrc::kStreamFailure, "JsonOps::Tell: Could not get stream position.");
    }

    return result;
}

/*!
 /// \internal
 * - Peek at the next character
 * - If it matches:
 *   -  call get() to consume it, return true.
 * - Otherwise:
 *   - Return false.
 * \endinternal
 */
auto JsonOps::Skip(const char character) noexcept -> bool
{
    std::istream& stream = this->GetStream();
    std::int64_t peek_val = stream.peek();
    bool result{false};
    if (peek_val != std::char_traits<char>::eof() && peek_val == std::char_traits<char>::to_int_type(character))
    {
        stream.get();
        result = !stream.fail();
    }

    return result;
}

/*!
 * \internal
 * - Check that the string contains what is expected.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto JsonOps::CheckString(std::string_view const string, std::string_view const error_msg) noexcept
    -> vajson::ResultBlank
{
    return ReadString(string).and_then([&error_msg](bool val) noexcept {
        vajson::ResultBlank result{MakeErrorResult<vajson::Blank>(JsonErrc::kInvalidJson, error_msg)};
        if (val)
        {
            result.emplace(vajson::Blank{});
        }
        return result;
    });
}

/*!
 * \internal
 * - If the given string is not empty:
 *   - Compare every character of the string with the next characters.
 *   - If they match, return an empty Result.
 * - Otherwise:
 *   - Return an error.
 * \endinternal
 */
auto JsonOps::ReadString(std::string_view const string) noexcept -> Result<bool>
{
    AssertCondition(!string.empty(), "JsonOps::ReadString: Cannot check for empty string");

    std::size_t const total_size{string.size()};
    bool is_same{true};
    std::string_view to_be_read{string};
    return this
        ->Read(total_size,
               [&is_same, &to_be_read](std::string_view view) noexcept {
                   std::string_view to_be_compared{};

                   std::tie(to_be_compared, to_be_read) = SplitStringView(to_be_read, view.size());
                   if (view != to_be_compared)
                   {

                       is_same = false;
                   }
               })
        .and_then([this, &is_same, &total_size](std::size_t read_size) noexcept {
            return this->RewindIf((!is_same) || (total_size != read_size), read_size);
        })
        .transform([&is_same](vajson::Blank) noexcept {
            return is_same;
        });
}

/*!
 * \internal
 * - If the condition holds
 *  - Seek back the number of bytes passed from the current position.
 * \endinternal
 */
auto JsonOps::RewindIf(const bool condition, std::size_t const num) noexcept -> vajson::ResultBlank
{
    vajson::ResultBlank result{};
    if (condition)
    {
        std::istream& stream = this->GetStream();
        stream.seekg(-1 * static_cast<std::int64_t>(num), std::ios::cur);

        if (stream.fail())
        {
            result =
                MakeErrorResult<vajson::Blank>(JsonErrc::kStreamFailure, "JsonOps::RewindIf: Could not seek back.");
        }
    }

    return result;
}

/*!
 * \internal
 * - Skip all white space characters.
 * \endinternal
 */
auto JsonOps::SkipWhitespace() noexcept -> bool
{
    constexpr static StringView kWhitespaceChars{" \n\r\t"sv};
    std::istream& stream = this->GetStream();
    bool done{false};
    bool result{false};

    while (!done)
    {
        std::int64_t ch = stream.peek();

        if (ch == std::char_traits<char>::eof())
        {
            result = false;
            done = true;
        }
        else
        {
            char c = std::char_traits<char>::to_char_type(static_cast<std::int32_t>(ch));

            const bool is_whitespace =
                std::any_of(kWhitespaceChars.cbegin(), kWhitespaceChars.cend(), [c](char wc) noexcept {
                    return c == wc;
                });

            if (!is_whitespace)
            {
                result = true;
                done = true;
            }
            else
            {
                stream.get();  // consume the whitespace character
            }
        }
    }

    return result;
}

/*!
 * \internal
 * - As long as there are new elements in the stream and the number of bytes to read is not zero:
 *   - Trigger the given action on the returned span.
 * - Otherwise:
 *   - Break.
 * \endinternal
 */
auto JsonOps::Read(std::uint64_t const num_to_read,
                   const score::cpp::move_only_function<void(std::string_view), 40>& callback) noexcept
    -> Result<std::uint64_t>
{
    std::istream& stream = this->GetStream();
    std::vector<char> buffer(num_to_read);

    stream.read(buffer.data(), static_cast<std::streamsize>(num_to_read));
    std::uint64_t bytes_read = static_cast<std::uint64_t>(stream.gcount());

    if (bytes_read > 0)
    {
        std::string_view view{buffer.data(), bytes_read};
        callback(view);
    }

    return Result<std::uint64_t>{bytes_read};
}

/*!
 * \internal
 * - Read the requested amount of characters from the stream.
 * - If the number of read characters is as requested:
 *   - Execute the callback.
 * - Otherwise:
 *   - Buffer the characters.
 * - If the requested number of characters could not be read, return an error.
 * - If the callback has not been executed and there was no error, execute the callback.
 * \endinternal
 */
auto JsonOps::ReadExactly(std::uint64_t const num_to_read,
                          const score::cpp::move_only_function<void(std::string_view)>& callback) noexcept
    -> Result<vajson::Blank>
{

    String& buffer{this->GetJsonDocument().GetClearedStringBuffer()};

    bool callback_executed{false};
    Result<vajson::Blank> result =
        Drop(Read(num_to_read, [&buffer, &callback, &num_to_read, &callback_executed](std::string_view view) noexcept {
                 if (view.size() == num_to_read)
                 {
                     callback(view);
                     callback_executed = true;
                 }
                 else
                 {
                     static_cast<void>(buffer.append(view));
                 }
             }).and_then([&num_to_read](const std::uint64_t& read) noexcept {
            vajson::ResultBlank result{
                MakeErrorResult<vajson::Blank>(JsonErrc::kInvalidJson, "JsonOps::ReadExactly: Unexpected EOF.")};
            if (num_to_read == read)
            {
                result.emplace(vajson::Blank{});
            }
            return result;
        }));

    if (result && (!callback_executed))
    {
        const StringView buf_view{buffer};
        callback(buf_view);
    }

    return result;
}

/*!
 * \internal
 * - As long as there are new elements in the stream and the delimiter is not found:
 *   - Trigger the given action on the returned span.
 * - Otherwise:
 *   - Break.
 * \endinternal
 */
auto JsonOps::ReadUntil(std::string_view const delimiter,
                        const score::cpp::move_only_function<void(std::string_view)>& callback) noexcept
    -> Result<OptChar>
{
    std::istream& stream = this->GetStream();
    std::string buffer;
    bool done{false};
    Result<OptChar> result{OptChar{-1}};

    while (!done)
    {
        std::int64_t const ch{stream.peek()};

        if (ch == std::char_traits<char>::eof())
        {
            if (!buffer.empty())
            {
                std::string_view view{buffer.data(), buffer.size()};
                callback(view);
            }
            result = Result<OptChar>{OptChar{-1}};
            done = true;
        }
        else
        {
            const char character{std::char_traits<char>::to_char_type(static_cast<std::int32_t>(ch))};

            // Check if this character matches any in the delimiter
            const bool is_delimiter =
                std::any_of(delimiter.cbegin(), delimiter.cend(), [character](char delim) noexcept {
                    return character == delim;
                });

            if (is_delimiter)
            {
                if (!buffer.empty())
                {
                    std::string_view const view{buffer.data(), buffer.size()};
                    callback(view);
                }
                result = Result<OptChar>{OptChar{ch}};
                done = true;
            }
            else
            {
                buffer.push_back(character);
                static_cast<void>(stream.get());  // consume the character
            }
        }
    }

    return result;
}

auto JsonOps::GetJsonDocument() & noexcept -> JsonData&
{

    return this->data_.get();
}

auto JsonOps::GetJsonDocument() const& noexcept -> const JsonData&
{

    return this->data_.get();
}

auto JsonOps::GetStream() & noexcept -> std::istream&
{

    return this->GetJsonDocument().GetStream();
}

auto JsonOps::GetStream() const& noexcept -> const std::istream&
{

    return this->GetJsonDocument().GetStream();
}

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score
