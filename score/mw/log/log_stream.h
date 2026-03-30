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
#ifndef SCORE_MW_LOG_LOG_STREAM_H
#define SCORE_MW_LOG_LOG_STREAM_H

// Be careful what you include here. Each additional header will be included in logging.h and thus exposed to the user.
// We need to try to keep the includes low to reduce the compile footprint of using this library.
#include "score/result/error.h"
#include "score/mw/log/detail/logging_identifier.h"
#include "score/mw/log/log_common.h"
#include "score/mw/log/log_level.h"
#include "score/mw/log/log_types.h"
#include "score/mw/log/recorder.h"
#include "score/mw/log/slot_handle.h"

#include "score/optional.hpp"
#include <string_view>

#include <chrono>
#include <type_traits>

namespace score
{
namespace mw
{
namespace log
{
/*
Deviation from Rule A0-1-3:
- Every function defined in an anonymous namespace, or static function with internal linkage,
    or private member function shall be used.
Justification:
- The function is used.
*/
template <typename T>
// coverity[autosar_cpp14_a0_1_3_violation : FALSE] see above
constexpr static bool IsCharPtrType() noexcept
{
    using PointerType = std::remove_reference_t<T>;
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
    if constexpr (std::is_pointer_v<PointerType>)
    {
        using ValueType = std::remove_pointer_t<PointerType>;
        return std::is_same_v<LogString::CharType, std::remove_const_t<ValueType>>;
    }
    return false;
}

template <typename T>
// coverity[autosar_cpp14_a0_1_3_violation : FALSE] see above
constexpr static bool IsCharArrayType() noexcept
{
    using ArrayType = std::remove_cv_t<std::remove_reference_t<T>>;
    return std::is_array_v<ArrayType> && std::is_same_v<LogString::CharType, std::remove_extent_t<ArrayType>>;
}

template <typename T>
// coverity[autosar_cpp14_a0_1_3_violation : FALSE] see above
constexpr static bool IsLogRawBufferType() noexcept
{
    return std::is_same_v<LogRawBuffer, std::remove_cv_t<std::remove_reference_t<T>>>;
}

namespace detail
{
class LogStreamFactory;

template <typename... Types>
struct TypesHolder
{
};

template <typename T, typename... SupportedTypes>
// coverity[autosar_cpp14_a0_1_3_violation : FALSE] see above
constexpr static bool LogStreamSupports(TypesHolder<SupportedTypes...> /*unused*/) noexcept
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
    if constexpr (not(IsCharPtrType<T>()))
    {
        using RequestedType = std::remove_cv_t<std::remove_reference_t<T>>;
        /*
        Deviation from Rule A5-2-6:
        - The operands of a logical && or || shall be parenthesized if the operands contain binary operators.
        Justification:
        - logical operator on pack parameter
        */
        // coverity[autosar_cpp14_a5_2_6_violation]
        return (std::is_convertible_v<RequestedType, SupportedTypes> || ...);
    }
    else
    {
        return false;
    }
}
}  // namespace detail

template <typename T>
// coverity[autosar_cpp14_a0_1_3_violation : FALSE] see above
constexpr static bool LogStreamSupports() noexcept
{
    using SupportedTypes = detail::TypesHolder<bool,
                                               float,
                                               double,
                                               std::int8_t,
                                               std::int16_t,
                                               std::int32_t,
                                               std::int64_t,
                                               std::uint8_t,
                                               std::uint16_t,
                                               std::uint32_t,
                                               std::uint64_t,
                                               LogBin8,
                                               LogBin16,
                                               LogBin32,
                                               LogBin64,
                                               LogHex8,
                                               LogHex16,
                                               LogHex32,
                                               LogHex64,
                                               LogString,
                                               LogRawBuffer,
                                               LogSlog2Message>;
    return detail::LogStreamSupports<T>(SupportedTypes{});
}

// \brief User-Facing RAII class that manages a LogStream and can be used to log data. The log message will only be
// flushed upon destruction of this type. It is not possible to reuse one LogStream for multiple different log
// messages. Logging is a best effort operation, if it is not possible to log a message due to some reason, this class
// will not forward the respective messages.
//
// \details This class only supports the logging of the following basic data types:
//          * bool
//          * float
//          * double
//          * std::int8_t
//          * std::int16_t
//          * std::int32_t
//          * std::int64_t
//          * std::uint8_t
//          * std::uint16_t
//          * std::uint32_t
//          * std::uint64_t
//          * mw::log::LogBin8
//          * mw::log::LogBin16
//          * mw::log::LogBin32
//          * mw::log::LogBin64
//          * mw::log::LogHex8
//          * mw::log::LogHex16
//          * mw::log::LogHex32
//          * mw::log::LogHex64
//          * mw::log::LogString
//          * mw::log::LogRawBuffer
//          * std::string (via implicit conversion to mw::log::LogString)
//          * std::string_view (via implicit conversion to mw::log::LogString)
//          * score::cpp::pmr::string (via implicit conversion to mw::log::LogString)
//          * ara::core::StringView (via implicit conversion to mw::log::LogString)
//          * std::array<[const] char> (via implicit conversion to mw::log::LogString)
//          * pointer to const char (deprecated)
//          * C-style array of char
//          * custom data struct LogSlog2Message, see struct description
//
//          If a user wants to log a custom data type, he needs to extend the possibility as follows in the context of
//          his data type:
//
//          score::mw::LogStream& operator<<(score::mw::LogStream& stream, const YourAwesomeType& type)
//          {
//             stream << YourAwesomeType.getSomething(); // custom logic on how to represent your type as Log-Message
//             return stream;
//          }
/// \public
class LogStream final
{
  public:
    /// \brief Stream operator which enables logging of supported data types.
    /// \public
    /// \note Only the types mentioned in class description are supported by default.
    /// \details Similar to std::cout it is not safe to access this stream from multiple threads!
    template <typename T>
    std::enable_if_t<LogStreamSupports<T>(), LogStream&> operator<<(const T& value) & noexcept
    {
        // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
        // not be implicitly converted to a different underlying type"
        // False positive, right hand value is the same type.
        // Suppress "AUTOSAR C++14 M5-0-11" rule findings, This rule states: "The plain char type shall
        // only be used for the storage and use of character values"
        // False positive, single char is not supported by operator<<
        // coverity[autosar_cpp14_m5_0_3_violation]
        // coverity[autosar_cpp14_m5_0_11_violation]
        return Log(value);
    }

    /// \brief Deprecated stream operator which enables logging of C-style string via character pointer.
    /// \note The pointed-to character sequence must be null-terminated!
    /// \details Similar to std::cout it is not safe to access this stream from multiple threads!
    template <typename T>
    [[deprecated(
        "SPP_DEPRECATION(performance): "
        "Logging a plain character pointer is discouraged for performance reasons since that requires a length "
        "determination of the pointed-to character sequence at runtime and results in a sequential memory scan. "
        "Instead, since almost all string-like types are implicitly nothrow-convertible to `mw::log::LogString`, "
        "these should be logged as-is and not via their underlying character pointer! If that's not possible, a "
        "custom overload of `operator<<` for the particular user-defined type should be provided.")]] std::
        enable_if_t<IsCharPtrType<T>(), LogStream&>
        operator<<(T char_ptr) &
    {
        if (char_ptr != nullptr)
        {
            const LogString value{char_ptr, LogString::TraitsType::length(char_ptr)};
            return Log(value);
        }
        return *this;
    }

    /// \brief Stream operator which enables logging of a string literal (-> character array).
    /// \public
    /// \details Similar to std::cout it is not safe to access this stream from multiple threads!
    /*
    Deviation from Rule A13-2-2:
    - A binary arithmetic operator and a bitwise operator shall return a “prvalue”."
    Justification:
    - The code with '<<' is not a left shift operator but an overload for logging the respective types.
        code analysis tools tend to assume otherwise hence a false positive.
    */
    template <std::size_t n>
    // NOLINTBEGIN(modernize-avoid-c-arrays): required for logging string literals without array-to-pointer decay
    // coverity[autosar_cpp14_a13_2_2_violation : FALSE] see above
    LogStream& operator<<(const LogString::CharType (&array)[n]) noexcept
    // NOLINTEND(modernize-avoid-c-arrays): see above
    {
        const LogString value{std::forward<decltype(array)>(array)};
        return Log(value);
    }

    // \brief LogStream is move-construction only
    LogStream(const LogStream&) noexcept = delete;
    LogStream(LogStream&&) noexcept;
    LogStream& operator=(const LogStream&) noexcept = delete;
    LogStream& operator=(LogStream&&) noexcept = delete;
    ~LogStream() noexcept;

    /// \brief Flushes the current buffer and prepares a new one.
    /// \public
    /// \details Calling mw::log::LogStream::Flush is only necessary if the mw::log::LogStream
    /// object is going to be re-used within the same scope. Otherwise, if the
    /// object goes out of scope (e.g. end of function block) then the flushing operation will
    /// be done internally by the destructor. It is important to note that the mw::log::-
    /// LogStream::Flush command does not empty the buffer, but it forwards the buffer’s
    /// current contents to the Logging framework.
    void Flush() noexcept;

  private:
    // We can't make the ctor public, the ctor intended to be private to avoid instance instantiation by the user
    // but it is needed internally by LogStreamFactory
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend score::mw::log::detail::LogStreamFactory;

    LogStream(Recorder&, Recorder&, const LogLevel, const std::string_view) noexcept;

    /// \brief Internal methods that perform logging of supported data types.
    LogStream& Log(const bool) noexcept;
    LogStream& Log(const float) noexcept;
    LogStream& Log(const double) noexcept;
    LogStream& Log(const LogString) noexcept;
    LogStream& Log(const std::int8_t) noexcept;
    LogStream& Log(const std::int16_t) noexcept;
    LogStream& Log(const std::int32_t) noexcept;
    LogStream& Log(const std::int64_t) noexcept;
    LogStream& Log(const std::uint8_t) noexcept;
    LogStream& Log(const std::uint16_t) noexcept;
    LogStream& Log(const std::uint32_t) noexcept;
    LogStream& Log(const std::uint64_t) noexcept;
    LogStream& Log(const LogBin8& value) noexcept;
    LogStream& Log(const LogBin16& value) noexcept;
    LogStream& Log(const LogBin32& value) noexcept;
    LogStream& Log(const LogBin64& value) noexcept;
    LogStream& Log(const LogHex8& value) noexcept;
    LogStream& Log(const LogHex16& value) noexcept;
    LogStream& Log(const LogHex32& value) noexcept;
    LogStream& Log(const LogHex64& value) noexcept;
    LogStream& Log(const LogSlog2Message& value) noexcept;

    /// \note Log method for `LogRawBuffer` must be templated so that the overload for `LogString` always
    ///       has higher precedence during overload resolution in case the parameter type is string-like.
    template <typename RawBufferType>
    std::enable_if_t<IsLogRawBufferType<RawBufferType>(), LogStream&> Log(const RawBufferType&) noexcept;

    /// \brief Internal method which forwards the logging operation to `LogStream`'s provided recorder(s).
    template <typename T>
    LogStream& LogWithRecorder(const T value) noexcept;

    /*
    Deviation from Rule A8-2-1:
    - When declaring function templates, the trailing return type syntax shall be used if the return type depends on the
    type of parameters.
    Justification:
    - the return type of the template function does not depend on the type of parameters.
    */
    template <typename ReturnValue, typename... ArgsOfFunction, typename... ArgsPassed>
    // Checker overly strict, pointer of object not null and only existing functions called (ensure by typeset)
    // NOLINTBEGIN(score-no-pointer-to-member): See above
    // coverity[autosar_cpp14_a8_2_1_violation] see above
    ReturnValue CallOnRecorder(ReturnValue (Recorder::*arbitrary_function)(ArgsOfFunction...) noexcept,
                               ArgsPassed&&... args) noexcept;
    // NOLINTEND(score-no-pointer-to-member): See above

    Recorder& recorder_;
    Recorder& fallback_recorder_;
    score::cpp::optional<SlotHandle> slot_;

    detail::LoggingIdentifier context_id_;
    LogLevel log_level_;
    static constexpr std::string_view kDefaultContextInStream{"DFLT"};
};

/// \brief Stream operator overload which enables logging to a `LogStream` rvalue.
/// \public
template <typename T>
std::enable_if_t<not(IsCharArrayType<T>()), LogStream&> operator<<(LogStream&& out, T&& value) noexcept
{
    /*
    Deviation from Rule A4-5-1
    - Expressions with type enum or enum class shall not be used as operands to built-in and overloaded operators other
      than the subscript operator [ ], the assignment operator =, the equality operators == and ! =, the unary &
      operator, and the relational operators <, <=, >, >=.
    Justification:
    - This is a false positive, as << is the stream operator
    */
    // coverity[autosar_cpp14_a4_5_1_violation]
    return (out << std::forward<T>(value));
}

/// @brief Writes chrono duration parameter as text into message.
/// \public
/*
Deviation from Rule A13-2-2:
- A binary arithmetic operator and a bitwise operator shall return a “prvalue”."
Justification:
- The code with '<<' is not a left shift operator but an overload for logging the respective types.
    code analysis tools tend to assume otherwise hence a false positive.

Deviation from Rule A13-3-1:
- A function that contains “forwarding reference” as its argument shall not be overloaded
Justification:
- different parameter types against the other with "forwarding reference" argument
*/
template <typename Rep, typename Period>
// coverity[autosar_cpp14_a13_2_2_violation : FALSE] see above
// coverity[autosar_cpp14_a13_3_1_violation] see above
LogStream& operator<<(LogStream& out, const std::chrono::duration<Rep, Period>& value) noexcept
{
    return (out << value.count() << DurationUnitSuffix<Period>());
}

/// \brief Support for signed long long (transforms into int64_t on 64 bit platforms)
/// \public
template <
    typename T = void,
    typename std::enable_if<((!std::is_same<long long, int64_t>::value) && (sizeof(long long) == sizeof(int64_t))),
                            T>::type* = nullptr>
// coverity[autosar_cpp14_a13_2_2_violation : FALSE] see above
inline LogStream& operator<<(LogStream& out, long long value) noexcept
{
    return (out << static_cast<int64_t>(value));
}

// Non-standard extensions
// To use these utilities please import the namespace, like this for example:
// `using ara::log::score_ext::operator<<;`
namespace score_ext
{
template <typename EnumerationT, std::enable_if_t<std::is_enum<EnumerationT>::value, bool> = true>
/// \public
// coverity[autosar_cpp14_a13_2_2_violation : FALSE] see above
LogStream& operator<<(LogStream& out, EnumerationT enumvalue)
{
    /*
    Deviation from Rule A5-2-2:
    - Traditional C-style casts shall not be used
    Deviation from Rule M5-0-4:
    - An implicit integral conversion shall not change the signedness of the underlying type
    Justification:
    - c-style casts is required for template
    - cast to enum underlying type, no signedness change
    */
    // coverity[autosar_cpp14_a5_2_2_violation]
    // coverity[autosar_cpp14_m5_0_4_violation]
    return (out << +typename std::underlying_type<EnumerationT>::type(enumvalue));
}
}  // namespace score_ext

/// \brief Global overload of a LogStream operator to enable translation from error to human readable representation)
LogStream& operator<<(LogStream& log_stream, const score::result::Error& error) noexcept;

/// \brief Global overload of a LogStream operator to enable translation from error to human readable representation)
LogStream&& operator<<(LogStream&& log_stream, const score::result::Error& error) noexcept;

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_LOG_STREAM_H
