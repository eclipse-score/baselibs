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

#include "score/json/json_writer.h"
#include "score/language/safecpp/safe_math/safe_math.h"
#include "score/json/i_json_writer.h"
#include "score/json/internal/model/error.h"
#include "score/json/internal/writer/json_serialize/json_serialize.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
// std::locale provides functionalities (e.g. facets) to customize parts of iostream implementation. In this case it is
// used for serializing integers efficiently. An implementation without std::locale might require (massive) amounts of
// code duplication. Furthermore, this header is allowed to be used for character conversion purposes if and it must be
// ensured that libcatalog is not linked using the target toolchain, which is the case for libjson.
// Reference: broken_link_c/issue/4600528
// NOLINTNEXTLINE(score-banned-include): see rationale above
#include <locale>
#include <sstream>
#include <string_view>
#include <type_traits>

namespace
{

template <typename T>
score::ResultBlank ToFileInternal(const T& json_data,
                                const std::string_view& file_path,
                                score::filesystem::IFileFactory& file_factory)
{
    const std::string file_path_string{file_path.data(), file_path.size()};
    const auto file = file_factory.Open(file_path_string, std::ios::out | std::ios::trunc);
    if (!file.has_value())
    {
        auto error = score::json::MakeError(score::json::Error::kInvalidFilePath, "Failed to open file");
        return score::ResultBlank{score::unexpect, error};
    }

    score::json::JsonSerialize serializer{**file};
    return serializer << json_data;
}

template <class U>
// Coverity thinks this function is unused, but it is used in for calculating kIntBufLen.
// coverity[autosar_cpp14_a0_1_3_violation]
constexpr std::size_t max_dec_digits() noexcept
{
    U v = std::numeric_limits<U>::max();
    std::size_t n = 1U;
    while (v >= 10U)
    {
        v /= 10U;
        // There is no known type that could represent enough digits to overflow std::size_t
        // coverity[autosar_cpp14_a4_7_1_violation]
        ++n;
    }
    return n;
}

template <typename T>
inline constexpr std::size_t kIntBufLen = max_dec_digits<std::make_unsigned_t<std::remove_cv_t<T>>>() + 1U;

// Rationale: noexcept safe magnitude conversion; uses safe_math assertions;
// COVERITY: autosar_cpp14_a15_5_3_violation, uncaught_exception
// policy requires terminate on failure; no exceptions thrown.
template <typename T>
// coverity[autosar_cpp14_a15_5_3_violation]
// coverity[uncaught_exception]
inline auto abs_magnitude_unsigned(T val) noexcept
{
    using U = std::make_unsigned_t<std::remove_cv_t<T>>;
    static_assert(std::is_integral_v<T>, "integral only");
    static_assert(std::is_integral_v<U>, "U must be integral");
    static_assert(std::numeric_limits<U>::digits >= std::numeric_limits<std::remove_cv_t<T>>::digits,
                  "U must represent full magnitude of T");

    // Coverity doesn't know constexpr if statements
    // coverity[autosar_cpp14_a7_1_8_violation]
    if constexpr (std::is_signed_v<T>)
    {
        const auto abs_val = score::safe_math::Abs(val);
        const auto cast_res = score::safe_math::Cast<U>(abs_val);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(cast_res.has_value(), "Safe cast failed in abs_magnitude_unsigned (signed)");
        return cast_res.value();
    }
    else
    {
        const auto cast_res = score::safe_math::Cast<U>(val);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(cast_res.has_value(), "Safe cast failed in abs_magnitude_unsigned (unsigned)");
        return cast_res.value();
    }
}

template <typename T>
[[nodiscard]] std::string_view integer_to_chars(std::array<char, kIntBufLen<T>>& buffer, const T val) noexcept
{
    using U = std::make_unsigned_t<std::remove_cv_t<T>>;
    static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "integral only");
    static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>, "bool not supported");
    static_assert(std::numeric_limits<U>::digits >= std::numeric_limits<std::remove_cv_t<T>>::digits,
                  "U must represent full magnitude of T");

    const bool is_negative = (std::is_signed_v<T> && (val < static_cast<T>(0)));
    U x = abs_magnitude_unsigned(val);

    // no range checks on it because the function declaration ensures enough buffer space for the given type
    auto it = buffer.end();
    do
    {
        // Rationale: Converting unsigned digit (x % 10U) to signed int8_t is safe because
        // the modulo operation guarantees the value is in range [0, 9], which fits in both
        // uint8_t and int8_t. The signed type is needed for consistent char arithmetic with '0'.
        // coverity[autosar_cpp14_m5_0_9_violation]
        const auto digit = static_cast<std::int8_t>(x % static_cast<U>(10U));
        it = std::prev(it);
        // coverity[autosar_cpp14_m5_0_9_violation]
        *it = static_cast<char>(static_cast<std::int8_t>('0') + digit);
        x /= 10U;
    } while (x > static_cast<U>(0U));
    if (is_negative)
    {
        it = std::prev(it);
        *it = '-';
    }

    return std::string_view(&*it, static_cast<std::size_t>(std::distance(it, buffer.end())));
}

// This specialization of std::num_put<char> ignores parameter widths (see std::setw) as this feature is neither useful
// nor used for serializing JSON.
class OptimizedNumPut : public std::num_put<char>
{
  public:
    // Coverity thinks this function is unused, wheras it is used for std::locale
    // coverity[autosar_cpp14_a0_1_3_violation]
    using std::num_put<char>::num_put;

  protected:
    using std::num_put<char>::do_put;

    // Coverity thinks this function is unused, wheras it is used for std::locale
    // coverity[autosar_cpp14_a0_1_3_violation]
    iter_type do_put(iter_type out, std::ios_base& s, char_type fill, long v) const override
    {
        return OptimizedPutForInts(out, s, fill, v);
    }
    // Coverity thinks this function is unused, wheras it is used for std::locale
    // coverity[autosar_cpp14_a0_1_3_violation]
    iter_type do_put(iter_type out, std::ios_base& s, char_type fill, unsigned long v) const override
    {
        return OptimizedPutForInts(out, s, fill, v);
    }
    // Coverity thinks this function is unused, wheras it is used for std::locale
    // coverity[autosar_cpp14_a0_1_3_violation]
    // LCOV_EXCL_START see SCORE_LANGUAGE_FUTURECPP_UNREACHABLE_MESSAGE
    iter_type do_put(iter_type out, std::ios_base& s, char_type fill, long long v) const override
    {
        SCORE_LANGUAGE_FUTURECPP_UNREACHABLE_MESSAGE("This code is unreachable with tested toolchains and target platforms");
        return OptimizedPutForInts(out, s, fill, v);
    }
    // LCOV_EXCL_STOP
    // Coverity thinks this function is unused, wheras it is used for std::locale
    // coverity[autosar_cpp14_a0_1_3_violation]
    // LCOV_EXCL_START see SCORE_LANGUAGE_FUTURECPP_UNREACHABLE_MESSAGE
    iter_type do_put(iter_type out, std::ios_base& s, char_type fill, unsigned long long v) const override
    {
        SCORE_LANGUAGE_FUTURECPP_UNREACHABLE_MESSAGE("This code is unreachable with tested toolchains and target platforms");
        return OptimizedPutForInts(out, s, fill, v);
    }
    // LCOV_EXCL_STOP

  private:
    template <typename T>
    iter_type OptimizedPutForInts(iter_type out, std::ios_base&, char_type, T val) const
    {
        std::array<char, kIntBufLen<T>> buf{};
        const auto sv = integer_to_chars<T>(buf, val);
        return std::copy(sv.begin(), sv.end(), out);
    }
};

template <typename T>
score::ResultBlank ToFileInternalAtomic(const T& json_data,
                                      const std::string_view& file_path,
                                      score::filesystem::IFileFactory& file_factory,
                                      const score::filesystem::AtomicUpdateOwnershipFlags atomic_ownership)

{
    return file_factory.AtomicUpdate(std::string{file_path}, std::ios::out | std::ios::trunc, atomic_ownership)
        .transform_error([](auto err) noexcept {
            return score::json::MakeError(score::json::Error::kInvalidFilePath, err.UserMessage());
        })
        .and_then([&json_data](auto filestream) -> score::ResultBlank {
            score::json::JsonSerialize serializer{*filestream};
            auto serializer_result = serializer << json_data;
            return filestream->Close().and_then([serializer_result](auto) noexcept {
                return serializer_result;
            });
        });
}

template <typename T>
score::Result<std::string> ToBufferInternal(const T& json_data)
{
    // This line must be hit when the function is called. Since other parts of this function show line coverage,
    // this line must also be hit. Missing coverage is due to a bug in the coverage tool
    std::ostringstream string_stream{};  // LCOV_EXCL_LINE

    // NOLINTBEGIN(score-no-dynamic-raw-memory) See rationale below
    //  Rationale:  std::num_put is reference counted and std::locale does manage it.
    //  Explanation for Coverity Suppression for AUTOSAR A3-3-2: Static locale with custom facet requires runtime
    //  initialization. std::locale constructor with facet cannot be constexpr. Thread-safe due to function-local static
    //  initialization guarantee (C++11 §6.7 [stmt.dcl]/4). coverity[autosar_cpp14_a3_3_2_violation]
    const static std::locale loc(std::locale(), new OptimizedNumPut());
    // NOLINTEND(score-no-dynamic-raw-memory) See rationale above

    score::cpp::ignore = string_stream.imbue(loc);

    score::json::JsonSerialize serializer{string_stream};
    serializer << json_data;
    return string_stream.str();
}

}  // namespace

score::json::JsonWriter::JsonWriter(FileSyncMode file_sync_mode,
                                  const score::filesystem::AtomicUpdateOwnershipFlags ownership) noexcept
    : IJsonWriter{}, file_sync_mode_{file_sync_mode}, atomic_ownership_{ownership}
{
}

score::ResultBlank score::json::JsonWriter::ToFile(const score::json::Object& json_data,
                                               const std::string_view& file_path,
                                               std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::ResultBlank score::json::JsonWriter::ToFile(const score::json::List& json_data,
                                               const std::string_view& file_path,
                                               std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::ResultBlank score::json::JsonWriter::ToFile(const score::json::Any& json_data,
                                               const std::string_view& file_path,
                                               std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::Object& json_data)
{
    return ToBufferInternal(json_data);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::List& json_data)
{
    return ToBufferInternal(json_data);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::Any& json_data)
{
    return ToBufferInternal(json_data);
}
