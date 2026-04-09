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
 *        \brief  Keeps track of the nesting depth inside the JSON document.
 *
 *      \details  Provides methods to check and modify the current nesting depth.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_DEPTH_COUNTER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_DEPTH_COUNTER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstdint>
#include <stack>
#include <string>
#include <utility>

#include "score/json/internal/parser/vajson/vajson_impl/reader/parser_state.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include "score/result/result.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief           A counter to track the nesting depth of the JSON parser
class DepthCounter final
{
    /// \brief            Stores the stack items
    /// \details         Intentionally a string to make use of SSO.
    std::stack<char, std::string> stack_{};
    /// \brief           Stores the element count of arrays and objects on the stack
    std::stack<std::size_t> counter_{};
    /// \brief           Flag to indicate if a comma must appear before the next value
    bool comma_expected_{false};
    /// \brief           Flag to indicate if all elements are closed
    bool is_finished_{false};

  public:
    /// \brief           CTOR
    /// \details         Cannot be defaulted because then it gets deleted by the compiler.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    DepthCounter() noexcept {}

    /// \brief           Move constructor
    /// \param[in]       that
    ///                  The other DepthCounter to move from.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    DepthCounter(DepthCounter&& that) noexcept : stack_{std::move(that.stack_)}, counter_{std::move(that.counter_)} {}

    /// \brief           Default copy constructor

    DepthCounter(const DepthCounter&) noexcept = default;

    /// \brief           Move assignment
    /// \return          Reference to the moved to object.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto operator=(DepthCounter&&) & noexcept -> DepthCounter& = default;

    /// \brief           Default copy assignment
    /// \return          Reference to the copied object.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto operator=(const DepthCounter&) & noexcept(false) -> DepthCounter& = default;

    /// \brief           Default destructor
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    ~DepthCounter() noexcept = default;

    /// \brief           Checks if an EOF is correct
    /// \details         An EOF is correct when there are no more elements on the stack, i.e. no arrays, objects, or
    /// keys
    ///                  are left open, or there is only a single value, in case the document consists of only a single
    ///                  value.
    /// \return          The empty Result if the stack is empty, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if no EOF was encountered.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If there are no elements in the stack or there is only a single value:
    ///   - Return an empty Result.
    /// - Otherwise return kInvalidJson.
    /// \endinternal
    auto CheckEndOfFile() const noexcept -> ParserResult
    {
        ParserResult result{ParserState::kFinished};
        if (!is_finished_ && IsEmpty())
        {
            result = MakeErrorResult<ParserState>(score::json::vajson::JsonErrc::kInvalidJson,
                                                  "DepthCounter::CheckEndOfFile: Empty document.");
        }
        else if (!is_finished_ && !IsEmpty() && stack_.top() == '[')
        {
            result = MakeErrorResult<ParserState>(score::json::vajson::JsonErrc::kInvalidJson,
                                                  "DepthCounter::CheckEndOfFile: Expected closing brackets.");
        }
        else if (!is_finished_ && !IsEmpty() && stack_.top() != '[')
        {
            result = MakeErrorResult<ParserState>(score::json::vajson::JsonErrc::kInvalidJson,
                                                  "DepthCounter::CheckEndOfFile: Expected closing braces.");
        }
        else
        {
            // already assigned
        }
        return result;
    }

    /// \brief           Adds an Array to the stack
    /// \return          The empty Result if an array can be added, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson,
    ///                  if a key was expected
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto AddArray() noexcept -> ResultBlank
    {
        return this->AddElement('[');
    }

    /// \brief           Adds an Object to the stack
    /// \return          The empty Result if an object can be added, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if a key was expected
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto AddObject() noexcept -> ResultBlank
    {
        return this->AddElement('{');
    }

    /// \brief           Adds a single key to the stack
    /// \return          The empty Result if a key can be added, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if a value was expected
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If the last element is an object:
    ///   - Add a key to the stack.
    ///   - Return an empty Result.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    auto AddKey() noexcept -> ResultBlank
    {
        ResultBlank result{};
        if (this->is_finished_)
        {

            result = MakeErrorResult<score::Blank>(JsonErrc::kInvalidJson,
                                                   "DepthCounter::AddKey: Multiple top level elements.");
        }
        else if (this->comma_expected_)
        {

            result = MakeErrorResult<score::Blank>(JsonErrc::kInvalidJson, "DepthCounter::AddKey: Missing comma.");
        }
        else if (this->CheckLastElement('{'))
        {

            this->stack_.push('k');
        }
        else
        {

            result = MakeErrorResult<score::Blank>(JsonErrc::kInvalidJson, "DepthCounter::AddKey: Expected a value.");
        }
        return result;
    }

    /// \brief           Adds a single value to the stack
    /// \return          The empty Result if a value can be added, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if a key was expected
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if a value is already present on the stack
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If the stack contains elements:
    ///   - If the last element is an array:
    ///     - Increase the element count for the array.
    ///   - Else if the last element is a key:
    ///     - Pop the key, because the pair is complete.
    ///     - Increase the count, because if a value follows a key, the enclosing element must be an object.
    ///   - Otherwise:
    ///     - Return an error.
    /// - Otherwise:
    ///   - Return an empty Result, because a single value is valid JSON.
    /// \endinternal
    auto AddValue() noexcept -> Result<score::Blank>
    {
        Result<score::Blank> result{score::Blank{}};

        if (this->is_finished_)
        {

            result = MakeErrorResult<score::Blank>(JsonErrc::kInvalidJson,
                                                   "DepthCounter::AddValue: Multiple top level elements.");
        }
        else if (this->comma_expected_)
        {

            result = MakeErrorResult<score::Blank>(JsonErrc::kInvalidJson, "DepthCounter::AddValue: Missing comma.");
        }
        else if (!this->IsEmpty())
        {
            this->comma_expected_ = true;

            // We now know that the stack is not empty, so there's no need to use CheckLastElement.

            const char last{this->stack_.top()};
            switch (std::char_traits<char>::to_int_type(last))
            {
                case std::char_traits<char>::to_int_type('['):

                    ++this->counter_.top();
                    break;
                case std::char_traits<char>::to_int_type('k'):

                    this->stack_.pop();

                    ++this->counter_.top();
                    break;
                default:

                    result = MakeErrorResult<score::Blank>(JsonErrc::kInvalidJson,
                                                           "DepthCounter::AddValue: Expected a key.");
                    break;
            }
        }
        else
        {

            this->is_finished_ = true;
        }

        return result;
    }

    /// \brief           Pops an Object from the stack
    /// \return          The number of keys in the object if the end object call was valid, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  If the object cannot be closed.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If the last element is an object:
    ///   - Pop the Object from the stack.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    auto PopObject() noexcept -> Result<std::size_t>
    {
        const bool expected_key{this->CheckLastElement('{')};
        return MakeResult(expected_key,
                          MakeError(static_cast<score::json::vajson::ErrorCode>(JsonErrc::kInvalidJson),
                                    "DepthCounter::PopObject: Not in an object."))
            .transform([this](score::Blank) noexcept {
                this->comma_expected_ = true;
                return Pop();
            });
    }

    /// \brief           Pops an Array from the stack
    /// \return          The number of elements in the array if the end array call was valid, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if in an object or on toplevel.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If the last element is an array:
    ///   - Pop the Array from the stack.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    auto PopArray() noexcept -> Result<std::size_t>
    {
        const bool expected_key{this->CheckLastElement('[')};
        return MakeResult(expected_key,
                          MakeError(static_cast<ErrorCode>(JsonErrc::kInvalidJson),
                                    "DepthCounter::PopArray: Not in an array."))
            .transform([this](score::Blank) noexcept {
                this->comma_expected_ = true;
                return this->Pop();
            });
    }

    /// \brief           Adds a comma
    /// \return          True if a comma at the current position is valid, or false otherwise.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If a comma is expected:
    ///   - Reset the flag.
    /// - If the stack is empty:
    ///   - Return false.
    /// - If the previous element is not a key:
    ///   - Return true, because in this case we must be inside an array or object and empty elements are fine.
    /// - Otherwise:
    ///   - Return false.
    /// \endinternal
    auto AddComma() noexcept -> bool
    {
        // Found at least the expected comma, ignore any succeeding commas (i.e. empty elements).
        if (this->comma_expected_)
        {
            this->comma_expected_ = false;
        }

        return ((!this->IsEmpty()) && (this->stack_.top() != 'k'));
    }

  private:
    /// \brief           Checks if the stack is empty
    /// \return          True if empty.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto IsEmpty() const noexcept -> bool
    {
        return this->stack_.empty();
    }

    /// \brief           Pops an element from the stack
    /// \details         Calling on empty counter invokes undefined behaviour.
    /// \return          The value of the removed element.
    /// \context         ANY
    /// \pre             The stack contains at least one element.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \internal
    /// - Remove the element at the last position from the stack.
    /// - Remove the counter at the last position and return its value.
    /// - If the element was inside another element, increase the outer element's count.
    /// \endinternal
    auto Pop() noexcept -> std::size_t
    {

        std::size_t const count{this->counter_.top()};

        this->counter_.pop();

        this->stack_.pop();

        if (this->CheckLastElement('[') || this->CheckLastElement('{'))
        {

            ++this->counter_.top();
        }

        this->CheckIfFinished();

        return count;
    }

    /// \brief           Adds an element to the stack
    /// \param[in]       element
    ///                  The element to add.
    /// \return          The empty Result if the element could be added, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if a key was expected
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if a value is already present on the stack
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If the last element is not an object and not a value:
    ///   - If the last element is a key, pop it from the stack, because the pair is complete.
    ///   - Add the element to the stack.
    ///   - Add a new counter to the stack.
    ///   - Return an empty Result.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    auto AddElement(char element) noexcept -> ResultBlank
    {
        ResultBlank result{};

        if (this->is_finished_)
        {

            result = MakeErrorResult<score::Blank>(JsonErrc::kInvalidJson,
                                                   "DepthCounter::AddElement: Multiple top level elements.");
        }
        else if (this->CheckLastElement('{'))
        {

            result = MakeErrorResult<score::Blank>(JsonErrc::kInvalidJson, "DepthCounter::AddElement: Expected a key.");
        }
        else if (this->comma_expected_)
        {

            result =
                MakeErrorResult<score::Blank>(JsonErrc::kInvalidJson, "DepthCounter::AddElement: Expected a comma.");
        }
        else
        {
            if (this->CheckLastElement('k'))
            {

                this->stack_.pop();
            }

            this->stack_.push(element);

            this->counter_.push(0U);
        }

        return result;
    }

    /// \brief           Compares the given element to the last element on the stack if it is not empty
    /// \param[in]       item
    ///                  to check for.
    /// \return          True if the last element equals the given item.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto CheckLastElement(char item) const noexcept -> bool
    {

        return (!this->IsEmpty()) && (this->stack_.top() == item);
    }

    /// \brief           Sets the finished flag if the stack is empty
    void CheckIfFinished() noexcept
    {
        if (this->IsEmpty())
        {
            this->is_finished_ = true;
        }
    }
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_DEPTH_COUNTER_H_
