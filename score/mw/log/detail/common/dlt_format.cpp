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
#include "score/mw/log/detail/common/dlt_format.h"
#include "score/bitmanipulation/bit_manipulation.h"
#include "score/mw/log/detail/common/helper_functions.h"

#include "score/assert.hpp"
#include "score/span.hpp"
#include <score/optional.hpp>

#include <type_traits>

/// For specification of the DLT protocol, please see:
/// https://www.autosar.org/fileadmin/user_upload/standards/foundation/1-0/AUTOSAR_PRS_DiagnosticLogAndTraceProtocol.pdf
///
/// As described in chapter 5.1 the general format of an DLT message looks as follows:
/// +-----------------+-----------------+---------+
/// | Standard Header | Extended Header | Payload |
/// +-----------------+-----------------+---------+
///
/// The `Standard Header` and `Extended Header` are for now no concern in this implementation. They will be filled by
/// the `DataRouter` application. For now we focus in this document on the `Payload` part.
///
/// The payload section can be filled in two ways: Non-Verbose (chapter 5.1.2.1) or Verbose (5.1.2.2).
/// This part of our code only implements the Verbose-Mode.
///
/// The verbose mode is further split into argument sections (PRS_Dlt_00459).
/// +-----------------+-----------------+-----------------------------------------------------+
/// | Standard Header | Extended Header |                       Payload                       |
/// |                 |                 +--------------------------+--------------------------+
/// |                 |                 |        Argument 1        |        Argument 2        |
/// |                 |                 +-----------+--------------+-----------+--------------+
/// |                 |                 | Type Info | Data Payload | Type Info | Data Payload |
/// +-----------------+-----------------+-----------+--------------+-----------+--------------+
///
/// For now also this handling of argument numbers is not handled within this class. This class rather interprets each
/// call to Log() as another argument that is added towards the payload. For this it takes case that the Type Info and
/// Data Payload are filled correctly. Any recorder using this formatter, will for now take care of the argument
/// handling.

namespace
{

// \Requirement PRS_Dlt_00626, PRS_Dlt_00354
enum class TypeLength : std::uint32_t
{
    kNotDefined = 0x00,
    k8Bit = 0x01,
    k16Bit = 0x02,
    k32Bit = 0x03,
    k64Bit = 0x04,
    k128Bi = 0x05,
};

// \Requirement PRS_Dlt_00627, PRS_Dlt_00182, PRS_Dlt_00366
enum class StringEncoding : std::uint32_t
{
    kASCII = 0x00,
    kUTF8 = 0x01,
};

// \Requirement PRS_Dlt_00783
enum class IntegerRepresentation : std::uint32_t
{
    kBase10 = 0x00U,
    kBase8 = 0x01U,
    kBase16 = 0x02U,
    kBase2 = 0x03U
};

class TypeInfo
{
  public:
    explicit TypeInfo(const std::uint32_t type)
    {
        std::ignore = score::platform::SetBit(underlying_type_, static_cast<std::size_t>(type));
    }

    // \Requirement PRS_Dlt_00625
    constexpr static std::uint32_t TYPE_BOOL_BIT = 4U;
    constexpr static std::uint32_t TYPE_SIGNED_BIT = 5U;
    constexpr static std::uint32_t TYPE_UNSIGNED_BIT = 6U;
    constexpr static std::uint32_t TYPE_FLOAT_BIT = 7U;
    constexpr static std::uint32_t TYPE_STRING_BIT = 9U;
    constexpr static std::uint32_t TYPE_RAW_BIT = 10U;

    // \Requirement PRS_Dlt_00354
    score::cpp::optional<TypeInfo> Set(const TypeLength length)
    {
        static_assert(std::is_same<std::underlying_type<TypeLength>::type, std::uint32_t>::value,
                      "Mismatching underlying type. Cast not valid.");
        underlying_type_ |= static_cast<std::uint32_t>(length);
        return *this;
    }

    // \Requirement PRS_Dlt_00183, PRS_Dlt_00367
    score::cpp::optional<TypeInfo> Set(const StringEncoding encoding) noexcept
    {
        const auto is_type_string_bit_set = score::platform::CheckBit(underlying_type_, TYPE_STRING_BIT);
        const auto is_trace_info_bit_set = score::platform::CheckBit(underlying_type_, TRACE_INFO_BIT);
        //  LCOV_EXCL_START : can't achieve the other conditions because can't control the "underlying_type_" value.
        if ((is_type_string_bit_set || is_trace_info_bit_set) == false)
        {
            return {};
            //  LCOV_EXCL_STOP
        }

        static_assert(std::is_same<std::underlying_type<StringEncoding>::type, std::uint32_t>::value,
                      "Mismatching underlying type. Cast not valid.");
        underlying_type_ |= (static_cast<std::uint32_t>(encoding) << STRING_ENCODING_START);
        return *this;
    }

    // \Requirement PRS_Dlt_00782, PRS_Dlt_00783
    score::cpp::optional<TypeInfo> Set(const score::mw::log::detail::IntegerRepresentation encoding) noexcept
    {
        const auto is_type_unsigned_bit_set = score::platform::CheckBit(underlying_type_, TYPE_UNSIGNED_BIT);
        const auto is_type_signed_bit_set = score::platform::CheckBit(underlying_type_, TYPE_SIGNED_BIT);
        //  LCOV_EXCL_START : can't achieve the other conditions because can't control the "underlying_type_" value.
        if ((is_type_unsigned_bit_set || is_type_signed_bit_set) == false)
        {
            return {};
            //  LCOV_EXCL_STOP
        }

        // Make sure the enum values match the values from the standard requirement PRS_Dlt_00783.
        static_assert(static_cast<std::uint8_t>(IntegerRepresentation::kBase10) == 0U,
                      "Value shall match PRS_Dlt_00783");
        static_assert(static_cast<std::uint8_t>(IntegerRepresentation::kBase8) == 1U,
                      "Value shall match PRS_Dlt_00783");
        static_assert(static_cast<std::uint8_t>(IntegerRepresentation::kBase16) == 2U,
                      "Value shall match PRS_Dlt_00783");
        static_assert(static_cast<std::uint8_t>(IntegerRepresentation::kBase2) == 3U,
                      "Value shall match PRS_Dlt_00783");

        static_assert(
            std::is_same<std::underlying_type<score::mw::log::detail::IntegerRepresentation>::type, std::uint8_t>::value,
            "Mismatching underlying type. Cast not valid.");
        underlying_type_ |= (static_cast<std::uint32_t>(encoding) << INTEGER_ENCODING_START);
        return *this;
    }

  private:
    // \Requirement PRS_Dlt_00135
    std::uint32_t underlying_type_{};

    // \Requirement PRS_Dlt_00625
    // constexpr static auto VARIABLE_INFO_BIT = 11U; not supported in our implementation
    // constexpr static auto FIXED_POINT_BIT = 12U; not supported in our implementation
    constexpr static std::size_t TRACE_INFO_BIT{13UL};
    constexpr static auto STRING_ENCODING_START = 15U;
    // \Requirement PRS_Dlt_00782
    constexpr static auto INTEGER_ENCODING_START = 15U;
};

template <typename T>
std::size_t SizeOf(const T& t)
{
    /*
    Deviation from Rule M6-4-1:
    - An if ( condition ) construct shall be followed by a compound statement.
    - The else keyword shall be followed by either a compound statement, or another if statement.
    Deviation from Rule A7-1-8:
    - A non-type specifier shall be placed before a type specifier in a declaration.
    Justification:
    - Each 'if' and 'else if' is followed by a compound statement as required.
      The use of 'if constexpr' is intentional for compile-time branching. It's used to prevent
      the warning autosar_cpp14_a2_10_4_violation [The identifier name of a non-member object
      with static storage duration or static function shall not be reused within a namespace.]
    - constexpr if statements were introduced in c++17, the autosar guidelines are for the use of c++14.
      Coverity checkers may lack support for c++17, hence the false positive.
    */
    // coverity[autosar_cpp14_m6_4_1_violation : FALSE]
    // coverity[autosar_cpp14_a7_1_8_violation : FALSE]
    if constexpr (std::is_same_v<T, std::string_view>)
    {
        return t.size();
    }
    // coverity[autosar_cpp14_m6_4_1_violation : FALSE]
    // coverity[autosar_cpp14_a7_1_8_violation : FALSE]
    else if constexpr (std::is_same_v<T, score::mw::log::LogRawBuffer>)
    {
        return static_cast<std::size_t>(t.size());
    }
    else
    {
        std::ignore = t;  // Mark 't' as used to avoid unused parameter coverity warning.
        return sizeof(T);
    }
}

template <typename... T>
bool WillMessageFit(score::mw::log::detail::VerbosePayload& payload, T... message_parts)
{
    std::size_t size{};
    size = helper::Sum(SizeOf(message_parts)...);

    return !payload.WillOverflow(size);
}

using ByteView = score::cpp::span<const score::mw::log::detail::Byte>;

template <typename T>
ByteView ToByteView(const T& t)
{
    /*
    Deviation from Rule M6-4-1:
    - An if ( condition ) construct shall be followed by a compound statement.
    - The else keyword shall be followed by either a compound statement, or another if statement.
    Deviation from Rule A7-1-8:
    - A non-type specifier shall be placed before a type specifier in a declaration.
    Justification:
    - Each 'if' and 'else if' is followed by a compound statement as required.
      The use of 'if constexpr' is intentional for compile-time branching. It's used to prevent
      the warning autosar_cpp14_a2_10_4_violation [The identifier name of a non-member object
      with static storage duration or static function shall not be reused within a namespace. ]
    - constexpr if statements were introduced in c++17, the autosar guidelines are for the use of c++14.
      Coverity checkers may lack support for c++17, hence the false positive.
    */
    // coverity[autosar_cpp14_m6_4_1_violation : FALSE]
    // coverity[autosar_cpp14_a7_1_8_violation : FALSE]
    if constexpr (std::is_same_v<T, std::string_view>)
    {
        return ByteView{t.data(), static_cast<ByteView::size_type>(t.size())};
    }
    // coverity[autosar_cpp14_m6_4_1_violation : FALSE]
    // coverity[autosar_cpp14_a7_1_8_violation : FALSE]
    else if constexpr (std::is_same_v<T, score::mw::log::LogRawBuffer>)
    {
        return ByteView{t.data(), t.size()};
    }
    else
    {
        // Yes, this is bad, but its what we want. We want to store the current plain memory
        // byte by byte in our buffer. Thus, this reinterpret cast is the only correct way to-do this.
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) justified above
        // Deviation from Rule M4-5-1:
        // - Expressions with type bool shall not be used as operands to built-in operators other than
        // - the assignment operator =, the logical operators &&, ||, !, the equality operators == and !=, the unary &
        // operator,
        // - and the conditional operator.
        // Justification:
        // - It's complaining because we convert &bool to const score::mw::log::detail::Byte*
        // coverity[autosar_cpp14_m4_5_1_violation]
        // coverity[autosar_cpp14_a5_2_4_violation]
        return ByteView{reinterpret_cast<const score::mw::log::detail::Byte*>(&t), sizeof(t)};
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast) justified above
    }
}

template <class F, class... Args>
void DoFor(F function, Args... args)
{
    (function(ToByteView(args)), ...);
}

template <typename... T>
score::mw::log::detail::AddArgumentResult Store(score::mw::log::detail::VerbosePayload& payload, T... data_for_payload)
{
    if (WillMessageFit(payload, data_for_payload...) == true)
    {
        DoFor(
            [&payload](const ByteView byte_view) {
                payload.Put(byte_view.data(), static_cast<std::size_t>(byte_view.size()));
            },
            data_for_payload...);
        return score::mw::log::detail::AddArgumentResult::Added;
    }
    return score::mw::log::detail::AddArgumentResult::NotAdded;
}

score::mw::log::detail::AddArgumentResult TryStore(score::mw::log::detail::VerbosePayload& payload,
                                                 const TypeInfo& type_info,
                                                 const std::uint64_t max_string_len_incl_null,
                                                 const std::string_view data) noexcept
{
    // LCOV_EXCL_START : see below
    // Defensive programming, this local function never called with max_string_len_incl_null == 0
    const std::uint64_t max_string_len = (max_string_len_incl_null > 0ULL) ? max_string_len_incl_null - 1ULL : 0ULL;
    // LCOV_EXCL_STOP

    const std::size_t data_size = data.size();

    // compute the cropped length as a size_t.
    const std::size_t cropped_length_size = (data_size > max_string_len) ? max_string_len : data_size;

    // ensure that cropped_length_size fits into a std::uint16_t.
    // This check makes explicit that we are not losing data when casting.
    const auto length_cropped = helper::ClampTo<std::uint16_t>(cropped_length_size);

    // Since the payload should include a terminating null,
    // calculate length_incl_null in a safe manner.
    // Check that adding 1 won't overflow the uint16_t.
    const auto length_incl_null = helper::ClampAddNullTerminator(length_cropped);

    // Based on above clamp opteration, length_cropped <= data.size() is always true.
    // And the following operation does not expect null terminator for string_view
    // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage)  See above
    const std::string_view data_cropped{data.data(), length_cropped};
    return Store(payload, type_info, length_incl_null, data_cropped, '\0');
}

template <typename Resolution>
score::mw::log::detail::AddArgumentResult LogData(score::mw::log::detail::VerbosePayload& payload,
                                                const Resolution data,
                                                const score::mw::log::detail::IntegerRepresentation repr,
                                                const std::uint32_t type,
                                                TypeLength type_length) noexcept
{
    // \Requirement PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358
    TypeInfo type_info(type);
    //  LCOV_EXCL_START :we can't cover true case as type_length are constant in the calling funcion & can't control
    if ((type_info.Set(type_length).has_value() == false) || (type_info.Set(repr).has_value() == false))
    {
        return score::mw::log::detail::AddArgumentResult::NotAdded;
        //  LCOV_EXCL_STOP
    }

    // \Requirement PRS_Dlt_00370
    return Store(payload, type_info, data);
}
}  // namespace

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const bool data) noexcept
{
    // \Requirement PRS_Dlt_00139
    TypeInfo type_info(TypeInfo::TYPE_BOOL_BIT);
    //  LCOV_EXCL_START : we can't cover true case as TypeLength and TypeInfo are constant and harded code
    if (type_info.Set(TypeLength::k8Bit).has_value() == false)
    {
        return AddArgumentResult::NotAdded;
        //  LCOV_EXCL_STOP
    }

    // \Requirement PRS_Dlt_00422
    static_assert((sizeof(data) == 1UL) == true, "Bool is not of size one Byte");

    // \Requirement PRS_Dlt_00369, PRS_Dlt_00423
    return Store(payload, type_info, data);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::uint8_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k8Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::uint16_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k16Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::uint32_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k32Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::uint64_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k64Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::int8_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_SIGNED_BIT, TypeLength::k8Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::int16_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_SIGNED_BIT, TypeLength::k16Bit);
}
AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::int32_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_SIGNED_BIT, TypeLength::k32Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::int64_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_SIGNED_BIT, TypeLength::k64Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const LogHex8 data, const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k8Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogHex16 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k16Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogHex32 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k32Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogHex64 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k64Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const LogBin8 data, const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k8Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogBin16 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k16Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogBin32 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k32Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogBin64 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k64Bit);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const float data) noexcept
{
    // \Requirement PRS_Dlt_00390, PRS_Dlt_00145
    TypeInfo type_info(TypeInfo::TYPE_FLOAT_BIT);
    //  LCOV_EXCL_START : we can't cover true case as TypeLength and TypeInfo are constant and harded code
    if (type_info.Set(TypeLength::k32Bit).has_value() == false)
    {
        return AddArgumentResult::NotAdded;
        //  LCOV_EXCL_STOP
    }

    // \Requirement PRS_Dlt_00371
    return Store(payload, type_info, data);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const double data) noexcept
{
    // \Requirement PRS_Dlt_00386, PRS_Dlt_00356
    TypeInfo type_info(TypeInfo::TYPE_FLOAT_BIT);
    //  LCOV_EXCL_START : we can't cover true case as TypeLength and TypeInfo are constant and harded code
    if (type_info.Set(TypeLength::k64Bit).has_value() == false)
    {
        return AddArgumentResult::NotAdded;
        //  LCOV_EXCL_STOP
    }

    // \Requirement PRS_Dlt_00371
    return Store(payload, type_info, data);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const std::string_view data) noexcept
{
    // \Requirement PRS_Dlt_00420, PRS_Dlt_00155
    TypeInfo type_info(TypeInfo::TYPE_STRING_BIT);
    //  LCOV_EXCL_START : we can't cover true case as TypeLength and TypeInfo are constant and harded code
    if (type_info.Set(StringEncoding::kUTF8).has_value() == false)
    {
        return AddArgumentResult::NotAdded;
        //  LCOV_EXCL_STOP
    }

    // \Requirement PRS_Dlt_00156, PRS_Dlt_00373
    // The string payload shall be assembled as follows:
    //       _____________________________________________
    //      |16-bit Length including termination character|
    //      |_____________________________________________|
    //      |Encoded data string length < 2^16 bytes      |
    //      |_____________________________________________|
    //      | Zero terminator 1 byte                      |
    //      |_____________________________________________|
    // Note that in practice the string must be even shorter as the entire DLT message must fit in max 2^16 bytes
    // including the DLT headers.

    // Number of bytes needed for the header needed before the payload.
    constexpr std::uint64_t header_size = sizeof(type_info) + sizeof(std::uint16_t);

    if (payload.RemainingCapacity() <= header_size)
    {
        // No space left in buffer for payload.
        return AddArgumentResult::NotAdded;
    }

    // Now figure out how many bytes we can store for the string (including null terminator).
    const auto capacity_after_header = payload.RemainingCapacity() - header_size;

    // Clamp that capacity to what fits in a 16-bit length field.
    // This is our max "string length including null".
    const auto max_string_len_incl_null =
        std::min<std::uint64_t>(capacity_after_header, std::numeric_limits<std::uint16_t>::max());

    return TryStore(payload, type_info, max_string_len_incl_null, data);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const LogRawBuffer data) noexcept
{
    // \Requirement PRS_Dlt_00625
    const auto type_info = TypeInfo(TypeInfo::TYPE_RAW_BIT);

    // \Requirement PRS_Dlt_00160, PRS_Dlt_00374
    // The string payload shall be assembled as follows:
    //       _____________________________________________
    //      |16-bit Length                                |
    //      |_____________________________________________|
    //      |Data string length <= 2^16 bytes             |
    //      |_____________________________________________|
    // Note that in pratice the data must be even shorter as the entire DLT message must fit in max 2^16 bytes
    // including the DLT headers.

    // Number of bytes needed for the header needed before the payload.
    constexpr std::size_t header_size = sizeof(type_info) + sizeof(std::uint16_t);

    if (payload.RemainingCapacity() <= header_size)
    {
        // No space left in buffer for payload.
        return AddArgumentResult::NotAdded;
    }

    // Calculate how many bytes can remain for the raw buffer after the header.
    const auto capacity_after_header = payload.RemainingCapacity() - header_size;

    // Clamp that to fit in uint16_t.
    const auto max_length = helper::ClampTo<std::uint16_t>(capacity_after_header);

    // The user-supplied data might also be bigger than that.
    const auto data_size = static_cast<std::size_t>(data.size());

    // Crop the data size if needed.
    const std::uint16_t length_cropped =
        (data_size > static_cast<std::size_t>(max_length)) ? max_length : static_cast<std::uint16_t>(data_size);

    static_assert(sizeof(LogRawBuffer::size_type) >= sizeof(decltype(length_cropped)),
                  "Size type of LogRawBuffer is smaller than uint16_t, can't safely cast");
    const LogRawBuffer data_cropped{data.first(static_cast<LogRawBuffer::size_type>(length_cropped))};

    return Store(payload, type_info, length_cropped, data_cropped);
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
