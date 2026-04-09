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
 *        \brief  A parser that works through composition rather than inheritance.
 *
 *      \details  Provides pre-defined parsers for JSON elements.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_COMPOSITION_PARSER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_COMPOSITION_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <type_traits>
#include <utility>

#include "score/json/internal/parser/vajson/vajson_impl/reader/parser_state.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader_fwd.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief           Returns bool if the function returns any Result
/// \tparam          F
///                  Type of function.
/// \tparam          T
///                  Type of argument.
template <typename F, typename T>
using ReturnsResult = score::IsResult<std::invoke_result_t<F, const T&>>;

/// \brief           Returns bool if the function returns ResultBlank
/// \tparam          F
///                  Type of function.
/// \tparam          T
///                  Type of argument.
template <typename F, typename T>
using ReturnsResultVoid = std::is_same<ResultBlank, std::invoke_result_t<F, const T&>>;

/// \brief           Returns bool if the function returns void
/// \tparam          F
///                  Type of function.
/// \tparam          T
///                  Type of argument.
template <typename F, typename T>
using ReturnsVoid = std::is_void<std::invoke_result_t<F, const T&>>;

/// \brief            SFINAE for callable returning Result<...>
/// \tparam          F                  Type of callable.
/// \tparam          T                  Type of argument.
/// \tparam          Out                  Type of return value.
template <typename F, typename T, typename Out>
using CallableReturnsResult = std::enable_if_t<ReturnsResult<F, T>::value, Out>;

/// \brief            SFINAE for callable returning Result<...>
/// \tparam          F                  Type of callable.
/// \tparam          T                  Type of argument.
/// \tparam          Out                  Type of return value.
template <typename F, typename T, typename Out>
using CallableReturnsNoResult = std::enable_if_t<!ReturnsResult<F, T>::value, Out>;

/// \brief           Returns bool in case the function returns any Result type
/// \tparam          F
///                  Type of function.
/// \tparam          T
///                  Type of argument.
template <typename F, typename T>
using ArrayReturnsResult = score::IsResult<std::invoke_result_t<F, std::size_t, const T&>>;

/// \brief           Returns bool in case the function returns a Result<score::Blank>
/// \tparam          F
///                  Type of function.
/// \tparam          T
///                  Type of argument.
template <typename F, typename T>
using ArrayReturnsResultVoid = std::is_same<Result<score::Blank>, std::invoke_result_t<F(std::size_t, const T&)>>;

/// \brief           Returns bool in case the function returns void
/// \tparam          F
///                  Type of function.
/// \tparam          T
///                  Type of argument.
template <typename F, typename T>
using ArrayReturnsVoid = std::is_void<std::invoke_result_t<F(std::size_t, const T&)>>;

/// \brief            SFINAE for callable returning Result<...>
/// \tparam          F                  Type of callable.
/// \tparam          T                  Type of argument.
/// \tparam          Out                  Type of return value.
template <typename F, typename T, typename Out>
using ArrayCallableReturnsResult = std::enable_if_t<ArrayReturnsResult<F, T>::value, Out>;

/// \brief            SFINAE for callable returning Result<...>
/// \tparam          F                  Type of callable.
/// \tparam          T                  Type of argument.
/// \tparam          Out                  Type of return value.
template <typename F, typename T, typename Out>
using ArrayCallableReturnsNoResult = std::enable_if_t<!ArrayReturnsResult<F, T>::value, Out>;

/// \brief           A parser that works through composition rather than inheritance
/// \details         Provides pre-defined parsers for JSON elements.

/// \trace           DSGN-JSON-Reader-Composition-Parser
template <typename Mixin>

class CompositionParser : public Mixin
{
    /// \brief           Shorthand for the ParserResult
    using R = ParserResult;

  public:
    /// \brief           Constructs a CompositionParser
    /// \param[in]       doc
    ///                  JSON document to parse.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    /// \spec
    ///    requires true;
    /// \endspec

    explicit CompositionParser(JsonData& doc) noexcept : Mixin(doc), doc_(doc) {}
    /// \brief Delete  copy constructor
    CompositionParser(const CompositionParser&) = delete;
    /// \brief Delete copy assignment constructor
    CompositionParser& operator=(const CompositionParser&) = delete;
    /// \brief Delete move constructor
    CompositionParser(CompositionParser&&) = delete;
    /// \brief Delete move assignment constructor
    CompositionParser& operator=(CompositionParser&&) = delete;

    /// \brief           Parses the following key value and executes the given callable
    /// \details         The callable must take the name of the key as an std::string_view and return void. The
    ///                  provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no key comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the key could be parsed successfully:
    ///   - Execute the callable with the key.
    ///   - Return an empty Result.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Fn>
    auto Key(Fn fn) noexcept -> CallableReturnsNoResult<Fn, std::string_view, R>
    {
        static_assert(ReturnsVoid<Fn, std::string_view>::value, "Must return void");
        return this->Key([&fn](std::string_view s) noexcept {
            std::forward<Fn>(fn)(s);
            return ResultBlank{};
        });
    }

    /// \brief           Parses the following key value and executes the given callable
    /// \details         The callable must take the name of the key as an std::string_view and return ResultBlank.
    ///                  The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no key comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the key could be parsed successfully:
    ///   - Execute the callable with the key.
    ///   - If the callable succeeds, return kRunning.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Fn>
    auto Key(Fn fn) noexcept -> CallableReturnsResult<Fn, std::string_view, R>;

    /// \brief           Parses the following key value and checks if it is a specific key

    /// \param[in]       key
    ///                  to compare with.
    /// \return          kRunning if the keys are the same.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if the names of the keys are not equal or no key comes next
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the key could be parsed successfully:
    ///   - Compare the parsed key with the given one.
    ///   - If the keys match, return an empty Result.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    auto Key(std::string_view key) noexcept -> R
    {
        return this->Key([&key](std::string_view str_view) noexcept {
            return MakeResult(key == str_view, {JsonErrc::kUserValidationFailed, "Incorrect key received"});
        });
    }

    /// \brief           Parses the following bool value and executes the given callable
    /// \details         The callable must take the bool and return void.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no bool comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the bool could be parsed successfully:
    ///   - Execute the callable with the bool.
    ///   - Return an empty Result.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Fn>
    auto Bool(Fn fn) noexcept -> CallableReturnsNoResult<Fn, bool, R>
    {
        static_assert(ReturnsVoid<Fn, bool>::value, "Must return void");
        return this->Bool([&fn](bool b) noexcept {
            std::forward<Fn>(fn)(b);
            return ResultBlank{};
        });
    }

    /// \brief           Parses the following bool value and executes the given callable
    /// \details         The callable must take the bool and return ResultBlank.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no bool comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the bool could be parsed successfully:
    ///   - Execute the callable with the bool.
    ///   - If the callable succeeds, return kRunning.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Fn>
    auto Bool(Fn fn) noexcept -> CallableReturnsResult<Fn, bool, R>;

    /// \brief           Parses the following number value and executes the given callable
    /// \details         The callable must take the number of type T and return void.

    /// \tparam          T
    ///                  Type of number.
    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no number comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the number could be parsed successfully:
    ///   - Execute the callable with the number.
    ///   - Return an empty Result.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename T, typename Fn>
    auto Number(Fn fn) noexcept -> CallableReturnsNoResult<Fn, T, R>
    {
        static_assert(ReturnsVoid<Fn, T>::value, "Must return void");
        return this->Number<T>([&fn](T n) noexcept {
            std::forward<Fn>(fn)(n);
            return ResultBlank{};
        });
    }

    /// \brief           Parses the following number value and executes the given callable
    /// \details         The callable must take the number of type T and return ResultBlank.

    /// \tparam          T
    ///                  Type of number.
    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no number comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the number could be parsed successfully:
    ///   - Execute the callable with the number.
    ///   - If the callable succeeds, return kRunning.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename T, typename Fn>
    auto Number(Fn fn) noexcept -> CallableReturnsResult<Fn, T, R>;

    /// \brief           Parses the following string value and executes the given callable
    /// \details         The callable must take the string as a std::string_view and return void. The provided
    ///                  StringView is only valid until any other method or parser operating on the same document is
    ///                  called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no string comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the string could be parsed successfully:
    ///   - Execute the callable with the string.
    ///   - Return an empty Result.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Fn>
    auto String(Fn fn) noexcept -> CallableReturnsNoResult<Fn, std::string_view, R>
    {
        static_assert(ReturnsVoid<Fn, std::string_view>::value, "Must return void");
        return this->String([&fn](std::string_view s) noexcept {
            std::forward<Fn>(fn)(s);
            return ResultBlank{};
        });
    }

    /// \brief           Parses the following string value and executes the given callable
    /// \details         The callable must take the string as an std::string_view and return ResultBlank. The
    ///                  provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no string comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the string could be parsed successfully:
    ///   - Execute the callable with the string.
    ///   - If the callable succeeds, return kRunning.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Fn>
    auto String(Fn fn) noexcept -> CallableReturnsResult<Fn, std::string_view, R>;

    /// \brief           Parses the following string value and checks if it is a specific string

    /// \param[in]       str
    ///                  to compare with.
    /// \return          kRunning if the strings are the same.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if the strings are not equal or no string comes next
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - Call the method String.
    ///   - If the parsed string is equal to passed string return an empty result.
    ///   - Otherwise return an error.
    /// \endinternal
    auto String(std::string_view str) noexcept -> R
    {
        return this->String([&str](std::string_view str_view) noexcept {
            return MakeResult(str == str_view, {JsonErrc::kUserValidationFailed, "Incorrect string received"});
        });
    }

    /// \brief           Parses the following array of numbers and executes the given callable on every element
    /// \details         The callable must take the current array index as a std::size_t, the current number of type T,
    /// and
    ///                  return void.

    /// \tparam          T
    ///                  Type of number.
    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajsonJsonErrc::kUserValidationFailed
    ///                  if no array of numbers comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - Parse the array of numbers and execute the callable with every number.
    /// \endinternal
    template <typename T, typename Fn>
    auto NumberArray(Fn fn) noexcept -> ArrayCallableReturnsNoResult<Fn, T, R>
    {
        static_assert(ArrayReturnsVoid<Fn, T>::value, "Must return void");
        return this->NumberArray<T>([&fn](std::size_t n, T number) noexcept {
            std::forward<Fn>(fn)(n, number);
            return ResultBlank{};
        });
    }

    /// \brief           Parses the following array of numbers and executes the given callable on every element
    /// \details         The callable must take the current array index as a std::size_t, the current number of type T,
    /// and
    ///                  return ResultBlank.

    /// \tparam          T
    ///                  Type of number.
    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajsonJsonErrc::kUserValidationFailed
    ///                  if no array of numbers comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - Parse the array of numbers and execute the callable with every number.
    /// \endinternal
    template <typename T, typename Fn>
    auto NumberArray(Fn fn) noexcept -> ArrayCallableReturnsResult<Fn, T, R>
    {
        static_assert(ArrayReturnsResultVoid<Fn, T>::value, "Must return ResultBlank");
        return this->Array([this, &fn](std::size_t n) noexcept {
            return this

                ->Number<T>(

                    [&fn, n](T number) noexcept {
                        return std::forward<Fn>(fn)(n, number);
                    })
                .Drop();
        });
    }

    /// \brief           Parses the following array of strings and executes the given callable on every element
    /// \details         The callable must take the current array index as a std::size_t, the current string as an
    ///                  std::string_view, and return void. The provided StringView is only valid until any other
    ///                  method or parser operating on the same document is called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no array of strings comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - Parse the array of strings and execute the callable with every string.
    /// \endinternal
    template <typename Fn>
    auto StringArray(Fn fn) noexcept -> ArrayCallableReturnsNoResult<Fn, std::string_view, R>
    {
        static_assert(ArrayReturnsVoid<Fn, std::string_view>::value, "Must return void");
        return this->StringArray([&fn](std::size_t n, std::string_view s) noexcept {
            std::forward<Fn>(fn)(n, s);
            return ResultBlank{};
        });
    }

    /// \brief           Parses the following array of strings and executes the given callable on every element
    /// \details         The callable must take the current array index as a std::size_t, the current string as an
    ///                  std::string_view, and return ResultBlank. The provided StringView is only valid until any
    ///                  other method or parser operating on the same document is called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no array of strings comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - Parse the array of strings and execute the callable with every string.
    /// \endinternal
    template <typename Fn>
    auto StringArray(Fn fn) noexcept -> ArrayCallableReturnsResult<Fn, std::string_view, R>
    {
        static_assert(ArrayReturnsResultVoid<Fn, std::string_view>::value, "Must return ResultBlank");
        return this->Array([this, &fn](std::size_t n) noexcept {
            return this

                ->String(

                    [&fn, n](std::string_view s) noexcept {
                        return std::forward<Fn>(fn)(n, s);
                    })
                .Drop();
        });
    }

    /// \brief           Parses the following array of bools and executes the given callable on every element
    /// \details         The callable must take the current array index as a std::size_t, the current bool value, and
    ///                  return void.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no array of bools comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - Parse the array of bools and execute the callable with every string.
    /// \endinternal
    template <typename Fn>
    auto BoolArray(Fn fn) noexcept -> ArrayCallableReturnsNoResult<Fn, bool, R>
    {
        static_assert(ArrayReturnsVoid<Fn, bool>::value, "Must return void");
        return this->BoolArray([&fn](std::size_t n, bool b) noexcept {
            std::forward<Fn>(fn)(n, b);
            return ResultBlank{};
        });
    }

    /// \brief           Parses the following array of bools and executes the given callable on every element
    /// \details         The callable must take the current array index as a std::size_t, the current bool value, and
    ///                  return ResultBlank.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no array of bools comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - Parse the array of bools and execute the callable with every string.
    /// \endinternal
    template <typename Fn>
    auto BoolArray(Fn fn) noexcept -> ArrayCallableReturnsResult<Fn, bool, R>
    {
        static_assert(ArrayReturnsResultVoid<Fn, bool>::value, "Must return ResultBlank");
        return this->Array([this, &fn](std::size_t n) noexcept {
            return this

                ->Bool(

                    [&fn, n](bool b) noexcept {
                        return std::forward<Fn>(fn)(n, b);
                    })
                .Drop();
        });
    }

    /// \brief           Parses the following binary content and executes the given callable
    /// \details         The callable must take the binary content as score::cpp::span<char const> and return
    /// ResultBlank. The
    ///                  provided Span is only valid until any other method or parser operating on the same document is
    ///                  called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no binary content comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the binary content could be parsed successfully:
    ///   - Execute the callable with the binary content.
    ///   - If the callable succeeds, return kRunning.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Fn>
    auto Binary(Fn fn) noexcept -> CallableReturnsResult<Fn, Bytes, R>;

    /// \brief           Parses the following binary content and executes the given callable
    /// \details         The callable must take the binary content as score::cpp::span<char const> and return void. The
    /// provided
    ///                  Span is only valid until any other method or parser operating on the same document is called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no binary content comes next
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the binary content could be parsed successfully:
    ///   - Execute the callable with the binary content.
    ///   - If the callable succeeds, return kRunning.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Fn>
    auto Binary(Fn fn) noexcept -> CallableReturnsNoResult<Fn, Bytes, R>
    {
        static_assert(ReturnsVoid<Fn, Bytes>::value, "Must return void");
        return this->Binary([&fn](Bytes view) noexcept {
            std::forward<Fn>(fn)(view);
            return ResultBlank{};
        });
    }

    /// \brief           Parses the following array and executes the given callable on every element
    /// \details         The callable must take the current array index as a std::size_t and return void. The callable
    /// must
    ///                  consume all tokens representing the element.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - Parse the array and execute the callable with every array element.
    /// \endinternal
    template <typename Fn>
    auto Array(Fn fn) noexcept -> CallableReturnsNoResult<Fn, std::size_t, R>
    {
        static_assert(ReturnsVoid<Fn, std::size_t>::value, "Must return void");
        return this->Array([&fn](std::size_t n) noexcept {
            std::forward<Fn>(fn)(n);
            return ResultBlank{};
        });
    }

    /// \brief           Parses the following array and executes the given callable on every element
    /// \details         The callable must take the current array index as a std::size_t and return ResultBlank. The
    ///                  callable is expected to have consumed all tokens representing the element if it returns a
    ///                  successful Result. If the callable is unable to consume all tokens it must return an error
    ///                  Result.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          kRunning on success, or an error.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - Parse the array and execute the callable with every array element.
    /// \endinternal
    template <typename Fn>
    auto Array(Fn fn) noexcept -> CallableReturnsResult<Fn, std::size_t, R>;

    /// \brief           Parses the following object and executes the given callable on every key
    /// \details         The callable must take the current object key as an std::string_view and return void. The
    ///                  provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called. The callable must consume all tokens representing the value related to the
    ///                  key.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \param[in]       object_already_open
    ///                  Specify if the object has already been opened. Defaults to false.
    /// \return          kRunning on success, or an error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - Parse the object and execute the callable with every key.
    /// \endinternal
    template <typename Fn>
    auto Object(Fn fn, bool object_already_open = false) noexcept -> CallableReturnsNoResult<Fn, std::string_view, R>
    {
        static_assert(ReturnsVoid<Fn, std::string_view>::value, "Must return void");
        return Object(
            [&fn](std::string_view key) noexcept {
                std::forward<Fn>(fn)(key);
                return ResultBlank{};
            },
            object_already_open);
    }

    /// \brief           Parses the following object and executes the given callable on every key
    /// \details         The callable must take the current key as an std::string_view and return ResultBlank. The
    ///                  provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called. The callable is expected to have consumed all tokens representing the value
    ///                  related to the key if it returns a successful Result. If the callable is unable to consume all
    ///                  tokens it must return an error Result.
    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \param[in]       object_already_open
    ///                  Specify if the object has already been opened. Defaults to false.
    /// \return          kRunning on success, or an error.
    /// \error           score::json::vajsonJsonErrc::kUserValidationFailed,
    ///                  if no object comes next
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE

    /// \internal
    /// - Parse the object and execute the callable with every key.
    /// \endinternal
    template <typename Fn>
    auto Object(Fn fn, bool object_already_open = false) noexcept -> CallableReturnsResult<Fn, std::string_view, R>;

  private:
    /// \brief           Reference to the JSON document to parse
    JsonData& doc_;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_COMPOSITION_PARSER_H_
