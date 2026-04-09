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
 *        \brief  Contains a parser for stateful JSON files.
 *
 *      \details  Provides methods to parse JSON elements and allows for chaining.
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_JSON_PARSER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_JSON_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "amsr/json/reader/parser.h"
#include "amsr/json/reader_fwd.h"
#include "amsr/json/util/types.h"
#include <functional>
#include <string>
#include <utility>

namespace amsr
{
namespace json
{

/// \brief           A parser that can be used to parse stateful JSON files

/// \trace           DSGN-JSON-Reader-Method-Based-Parser
class JsonParser final
{
  public:
    /// \brief           Constructs the parser

    /// \param[in]       data
    ///                  The JSON data.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    explicit JsonParser(JsonData& data) noexcept;

    /// \brief           Gets the current status of the parser
    /// \details         Does not change the internal state of the parser and thus may be called multiple times.

    /// \return          The empty Result, or the error that occurred.
    /// \error           amsr::json::JsonErrc::kUserValidationFailed
    ///                  if checking for a specific type but the next token is not of that type
    /// \error           amsr::json::JsonErrc::kUserValidationFailed
    ///                  if checking for a specific name of a key or string but the next key or string has a different
    ///                  name
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred:
    ///   - Return an empty Result.
    /// - Otherwise:
    ///   - Return the error.
    /// \endinternal
    auto Validate() const noexcept -> ResultBlank
    {
        return this->result_;
    }

    /// \brief           Returns a reference to the JSON file

    /// \return          A reference to the JSON file.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    auto GetJsonDocument() noexcept -> JsonData&
    {
        return this->data_.get();
    }

    /// \brief           Returns a reference to the JSON file

    /// \return          A reference to the JSON file.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    auto GetJsonDocument() const noexcept -> const JsonData&
    {
        return this->data_.get();
    }

    /// \brief           Checks if the next token is a key and executes the given callable
    /// \details         The callable must take the name of the key as an std::string_view and return void or
    ///                  ResultBlank. The provided StringView is only valid until any other method or parser operating
    ///                  on the same document is called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the key could be parsed successfully:
    ///   - Execute the given callable with the key.
    /// - If an error occurred, store it as the state of the parser.
    /// \endinternal
    template <typename Fn>
    auto Key(const Fn& fn) noexcept -> JsonParser&
    {

        return this->IfValid([this, &fn]() noexcept {
            return this->parser_.Key(fn).Drop();
        });
    }

    /// \brief           Checks if the next token is a specific key

    /// \param[in]       key
    ///                  to compare with.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the key could be parsed successfully:
    ///   - Compare the parsed key with the given one.
    /// - If the keys do not match or an error occurred, store it as the state of the parser.
    /// \endinternal
    auto Key(std::string_view key) noexcept -> JsonParser&
    {
        return this->IfValid([this, &key]() noexcept {
            return this->parser_.Key(key);
        });
    }

    /*!
     /// \copydoc         Key(std::string_view)
     /// \spec
     *    requires true;
     /// \endspec
     */
    // auto Key(CStringView key) noexcept -> JsonParser& { return this->Key(std::string_view(key.c_str(), key.size()));
    // }

    /// \copydoc         Key(std::string_view)
    /// \spec
    ///    requires true;
    /// \endspec
    auto Key(const std::string& key) noexcept -> JsonParser&
    {
        return this->Key(std::string_view{key});
    }

    /// \brief           Checks if the next token is the start of an object

    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    auto StartObject() noexcept -> JsonParser&;

    /// \brief           Checks if the next token is the end of an object

    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    auto EndObject() noexcept -> JsonParser&;

    /// \brief           Checks if the next token is the start of an array

    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    auto StartArray() noexcept -> JsonParser&;

    /// \brief           Checks if the next token is the end of an array

    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    auto EndArray() noexcept -> JsonParser&;

    /// \brief           Checks if the next token is a bool and executes the given callable
    /// \details         The callable must take the bool and return void or ResultBlank.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the bool could be parsed successfully:
    ///   - Execute the given callable with the bool.
    /// - If an error occurred, store it as the state of the parser.
    /// \endinternal
    template <typename Fn>
    auto Bool(const Fn& fn) noexcept -> JsonParser&
    {
        return this->IfValid([this, &fn]() noexcept {
            return this->parser_.Bool(fn).Drop();
        });
    }

    /// \brief           Checks if the next token is a string and executes the given callable
    /// \details         The callable must take the string as an std::string_view and return void or ResultBlank. The
    ///                  provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the string could be parsed successfully:
    ///   - Execute the given callable with the string.
    /// - If an error occurred, store it as the state of the parser.
    /// \endinternal
    template <typename Fn>
    auto String(const Fn& fn) noexcept -> JsonParser&
    {
        return this->IfValid([this, &fn]() noexcept {
            return this->parser_.String(fn).Drop();
        });
    }

    /// \brief           Checks if the next token is the expected string

    /// \param[in]       string
    ///                  to expect.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the string could be parsed successfully:
    ///   - Compare the parsed string with the given one.
    /// - If the strings do not match or an error occurred, store it as the state of the parser.
    /// \endinternal
    auto String(std::string_view string) noexcept -> JsonParser&
    {
        return this->IfValid([this, &string]() noexcept {
            return Drop(this->parser_.String(string));
        });
    }

    /// \copydoc         String(std::string_view)
    auto String(const std::string& string) noexcept -> JsonParser&
    {
        return this->String(std::string_view{string});
    }

    /// \brief           Checks if the next token is a number and executes the given callable
    /// \details         The callable must take the number of template type Num and return void or ResultBlank.

    /// \tparam          Num
    ///                  Type of number.
    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the number could be parsed successfully:
    ///   - Execute the given callable with the number.
    /// - If an error occurred, store it as the state of the parser.
    /// \endinternal
    template <typename Num, typename Fn>
    auto Number(const Fn& fn) noexcept -> JsonParser&
    {
        return this->IfValid([this, &fn]() noexcept {
            return this->parser_.Number<Num>(fn).Drop();
        });
    }

    /// \brief           Checks if the next token is binary content and executes the given callable
    /// \details         The callable must take the binary content as score::cpp::span<char const> and return
    /// ResultBlank. The
    ///                  provided Span is only valid until any other method or parser operating on the same document is
    ///                  called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the binary content could be parsed successfully:
    ///   - Execute the given callable with the string.
    /// - If an error occurred, store it as the state of the parser.
    /// \endinternal
    template <typename Fn>
    auto Binary(const Fn& fn) noexcept -> JsonParser&
    {
        return this->IfValid([this, &fn]() noexcept {
            return this->parser_.Binary(fn).Drop();
        });
    }

    /// \brief           Checks if the next token is an array and executes the given callable on every element
    /// \details         The callable must take the current array index as a std::size_t and return void or ResultBlank.
    ///                  The callable is expected to have consumed all tokens representing the element if it returns a
    ///                  successful Result or has return type void. If the callable is unable to consume all tokens it
    ///                  must return an error Result.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the array element could be parsed successfully:
    ///   - Execute the given callable with the array index.
    ///   - Repeat for all elements of the array.
    /// - If an error occurred, store it as the state of the parser.
    /// \endinternal
    template <typename Fn>
    auto Array(const Fn& fn) noexcept -> JsonParser&
    {
        return this->IfValid([this, &fn]() noexcept {
            return this->parser_.Array(fn).Drop();
        });
    }

    /// \brief           Checks if the next token is an array of strings and executes the given callable on every
    /// element \details         The callable must take the current array index as a std::size_t, the current string as
    /// an
    ///                  std::string_view, and return void or ResultBlank. The provided StringView is only valid
    ///                  until any other method or parser operating on the same document is called.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the array element could be parsed successfully:
    ///   - Execute the given callable with the array index and the parsed string.
    ///   - Repeat for all elements of the array.
    /// - If an error occurred, store it as the state of the parser.
    /// \endinternal
    template <typename Fn>
    auto StringArray(const Fn& fn) noexcept -> JsonParser&
    {
        return this->IfValid([this, &fn]() noexcept {
            return this->parser_.StringArray(fn).Drop();
        });
    }

    /// \brief           Checks if the next token is an array of numbers and executes the given callable on every
    /// element \details         The callable must take the current array index as a std::size_t, the current number of
    /// template
    ///                  type Num, and return void or ResultBlank.

    /// \tparam          Num
    ///                  Type of number.
    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the array element could be parsed successfully:
    ///   - Execute the given callable with the array index and the parsed number.
    ///   - Repeat for all elements of the array.
    /// - If an error occurred, store it as the state of the parser.
    /// \endinternal
    template <typename Num, typename Fn>
    auto NumberArray(const Fn& fn) noexcept -> JsonParser&
    {
        return this->IfValid([this, &fn]() noexcept {
            return this->parser_.NumberArray<Num>(fn).Drop();
        });
    }

    /// \brief           Checks if the next token is an array of bools and executes the given callable on every element
    /// \details         The callable must take the current array index as a std::size_t, the current bool value, and
    ///                  return void or ResultBlank.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the array element could be parsed successfully:
    ///   - Execute the given callable with the array index and the parsed bool.
    ///   - Repeat for all elements of the array.
    /// - If an error occurred, store it as the state of the parser.
    /// \endinternal
    template <typename Fn>
    auto BoolArray(const Fn& fn) noexcept -> JsonParser&
    {
        return this->IfValid([this, &fn]() noexcept {
            return this->parser_.BoolArray(fn).Drop();
        });
    }

    /// \brief           Checks if the next token is an object and executes the given callable on every key
    /// \details         The callable must take the name of the current key as a std::string_view and return void or
    ///                  ResultBlank. The provided StringView is only valid until any other method or parser operating
    ///                  on the same document is called. The callable is expected to have consumed all tokens
    ///                  representing the value related to the key if it returns a successful Result or has return type
    ///                  void. If the callable is unable to consume all tokens it must return an error Result.

    /// \tparam          Fn
    ///                  Type of callable.
    /// \param[in]       fn
    ///                  Callable.
    /// \param[in]       object_already_open
    ///                  Specify if the object has already been opened. Defaults to false.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If no error occurred before and the objects key could be parsed successfully:
    ///   - Execute the given callable with the objects key.
    ///   - Repeat for all keys of the object.
    /// - If an error occurred, store it as the state of the parser.
    /// \endinternal
    template <typename Fn>
    auto Object(const Fn& fn, bool object_already_open = false) noexcept -> JsonParser&
    {
        return this->IfValid([this, &fn, object_already_open]() noexcept {
            return this->parser_.Object(fn, object_already_open).Drop();
        });
    }

    /// \brief           Adds a custom error message to the ParserResult

    /// \param[in]       msg
    ///                  to add. Must live until the ParserResult is evaluated.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the error message has not been customized yet:
    ///   - Replace the SupportDataType of the erroneous parser result with the given one.
    /// \endinternal
    auto AddErrorInfo(const char* msg) & noexcept -> JsonParser&
    {
        if (ContainsStandardError())
        {
            result_ = MakeErrorResult<score::Blank>(static_cast<JsonErrc>(*result_.error()), msg);
            this->customized_ = true;
        }
        return *this;
    }

    /// \brief           Adds a custom error message to the ParserResult

    /// \tparam          Args
    ///                  Constructor arguments for the error code.
    /// \param[in]       args
    ///                  to construct the error code.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the error message has not been customized yet:
    ///   - Replace the erroneous parser result with the error constructed from the given arguments.
    /// \endinternal
    template <typename... Args>
    auto AddErrorInfo(Args... args) noexcept -> JsonParser&
    {
        if (ContainsStandardError())
        {

            this->result_ = ResultBlank(ErrorCode{args...});
            this->customized_ = true;
        }
        return *this;
    }

    /// \param[in]       errc
    ///                  The error code to add.
    /// \param[in]       msg
    ///                  The data msg to add. Must live until the ParserResult is evaluated.
    /// \return          The reference to itself.
    /// \copybrief       AddErrorInfo
    /// \spec
    ///    requires true;
    /// \endspec
    auto AddErrorInfo(JsonErrorDomain::Errc errc, CStr msg) & noexcept -> JsonParser&
    {
        if (ContainsStandardError())
        {

            this->result_ = MakeErrorResult<score::Blank>(errc, msg);
            this->customized_ = true;
        }
        return *this;
    }

  private:
    /// \brief           Executes the function and saves the Result if the previous actions were successful
    /// \tparam          Fn
    ///                  Type of function.
    /// \param[in]       fn
    ///                  Function to execute.
    /// \return          The reference to itself.
    /// \context         ANY
    /// \pre             Callable does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \internal
    /// - If no error occurred before:
    ///   - Execute the function and set the state of the parser from its return value.
    /// \endinternal
    template <typename Fn>
    auto IfValid(const Fn& fn) & noexcept -> JsonParser&
    {
        if (this->result_)
        {
            // If a callable that returns void changes this->result_ to an error, the return value of any pre-defined
            // parser call is still just a positive ResultBlank. Thus, even if the parser call returned successfully,
            // this->result_ must be checked again and set because it might contain an error.
            this->result_ = And(fn(), (this->result_));
        }
        return *this;
    }

    /// \brief           Checks the current error state of the parser
    /// \return          True if the state contains a standard error, otherwise false.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \spec
    ///    requires true;
    /// \endspec
    /// \internal
    /// - If the parser state contains an error and it has not been customized yet:
    ///   - Return true.
    /// - Otherwise:
    ///   - Return false.
    /// \endinternal
    auto ContainsStandardError() const noexcept -> bool
    {
        return !(this->customized_ || this->result_);
    }

    /// \brief           Internal parser
    internal::CompositionParser<internal::VirtualParser> parser_;

    /// \brief           JSON data
    std::reference_wrapper<JsonData> data_;

     /// \brief            State of the parser
     /// \details         Contains the first error that occurred.
    ResultBlank result_{};

    /// \brief           Flag if the error message has already been customized
    bool customized_{false};
};

}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_JSON_PARSER_H_
