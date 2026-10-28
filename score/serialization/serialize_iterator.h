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

#ifndef SCORE_SERIALIZATION_SERIALIZE_ITERATOR_H
#define SCORE_SERIALIZATION_SERIALIZE_ITERATOR_H

#include "score/serialization/details/type_traits.h"
#include "score/serialization/error/serialization_error.h"

#include "score/result/result.h"

#include <score/assert.hpp>

#include <iterator>
#include <limits>
#include <type_traits>

namespace score
{
namespace serialization
{

namespace helper
{

/// @brief Appends value via emplace_back and moves the iterator to end(). Caller must ensure capacity is available.
template <typename Container>
bool EmplaceBackInto(Container& container,
                     typename Container::iterator& iterator,
                     const typename Container::value_type& value)
{
    // Suppress "AUTOSAR C++14 A0-1-2", The rule states: "The value returned by a function having a non-void return type
    // that is not an overloaded operator shall be used." This is false positive. API returns void.
    // coverity[autosar_cpp14_a0_1_2_violation : FALSE]
    container.emplace_back(value);
    iterator = container.end();  // we cannot be sure that the iterator stays valid (most certainly it did not)
    return true;
}

/// @brief We use free function overloads, in order to change the store routine based on the container. If we would want
/// to-do this inside the iterator (as one normally would expect) we would need to specialize the iterator which could
/// cause some more code duplication. Thus, extracting this functionality for overloading is the more elegant solution
template <typename Container,
          std::enable_if_t<details::supports_emplace_back<Container>::value &&
                               (details::has_bounded_capacity<Container>::value == false),
                           bool> = true>
bool StoreIn(Container& container, typename Container::iterator& iterator, const typename Container::value_type& value)
{
    // if emplace back does not work, we will get an exception and thus terminate. maybe we can come up with a more
    // robust solution once we switched to PMR.
    return EmplaceBackInto(container, iterator, value);
}

/// @brief Overload for bounded containers (e.g. score::cpp::static_vector) whose emplace_back has a size() < capacity()
/// precondition. We check capacity ourselves and report an out-of-bounds write instead of terminating.
template <typename Container,
          std::enable_if_t<details::supports_emplace_back<Container>::value &&
                               details::has_bounded_capacity<Container>::value,
                           bool> = true>
bool StoreIn(Container& container, typename Container::iterator& iterator, const typename Container::value_type& value)
{
    if (container.size() >= container.capacity())
    {
        return false;
    }
    return EmplaceBackInto(container, iterator, value);
}

template <typename Container, std::enable_if_t<details::supports_emplace_back<Container>::value == false, bool> = true>
bool StoreIn(Container& container, typename Container::iterator& iterator, const typename Container::value_type& value)
{
    // ensure no out of bounds access
    if (container.cend() != iterator)
    {
        *iterator = value;
        iterator = std::next(iterator);
        return true;
    }
    else
    {
        return false;
    }
}

}  // namespace helper

/// @brief SerializeIterator enables a SerializationStrategy to write data into a provide buffer (container). Similar to
/// std::back_inserter. The difference is, that this iterator supports all common sequence container and also view on
/// such (like score::cpp::span).
///
/// @tparam Container The container this iterator should be associated with
template <class Container>
class SerializeIterator
{
  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = typename Container::value_type;
    using pointer = typename Container::value_type*;
    using reference = typename Container::value_type&;

    explicit SerializeIterator(Container& container)
        : container_(container),
          iterator_{std::make_shared<typename Container::iterator>(container_.begin())},
          result_{std::make_shared<Result<std::size_t>>()},
          number_of_bytes_written_{std::make_shared<std::size_t>(0U)}
    {
        static_assert(details::is_supported_sequence_container<Container>::value,
                      "Provided Container is not supported.");
    }

    /// @brief Inserts an value into the associated container. The iterator will move automatically to the next element,
    /// to ensure that no element is overwritten (similar to std::back_inserter).
    /// @param value The value to place into the container
    SerializeIterator<Container>& operator=(const typename Container::value_type& value) &
    {
        const auto result = helper::StoreIn(container_, *iterator_, value);
        if (result == false)
        {
            *result_ = MakeUnexpected(SerializationError::kOutOfBoundsWrite);
        }
        else
        {
            ++(*number_of_bytes_written_);
        }
        return *this;
    }

    /// @brief Inserts a result (mostly negative) into the associated container. This way it is possible to indicate
    /// that the serialization failed.
    SerializeIterator<Container>& operator=(const Result<std::size_t>& result) &
    {
        *result_ = result;
        return *this;
    }

    /// @brief No-Op (as for std::back_insert_iterator)
    SerializeIterator<Container>& operator*()
    {
        return *this;
    }

    /// @brief No-Op (as for std::back_insert_iterator)
    SerializeIterator<Container>& operator++()
    {
        return *this;
    }

    /// @brief No-Op (as for std::back_insert_iterator)
    SerializeIterator<Container> operator++(int)
    {
        return *this;
    }

    Result<std::size_t> GetResult() const
    {
        if (not result_->has_value())
        {
            return *result_;
        }

        return GetNumberOfBytesWritten();
    }

  private:
    // Suppress "AUTOSAR C++14 A0-1-3" rule findings. This rule states: "Every function defined in an anonymous
    // namespace, or static function with internal linkage, or private member function shall be used." False-positive,
    // method is used in GetResult().
    // coverity[autosar_cpp14_a0_1_3_violation : FALSE]
    std::size_t GetNumberOfBytesWritten() const
    {
        // Counts only elements stored through this iterator, excluding any content the container held beforehand.
        return *number_of_bytes_written_;
    }

    Container& container_;
    std::shared_ptr<typename Container::iterator> iterator_;
    std::shared_ptr<Result<std::size_t>> result_;
    std::shared_ptr<std::size_t> number_of_bytes_written_;
};

/// @brief Creates a SerializeIterator for a given Container
///
/// @tparam Container must be one of the supported container types (a sequence container)
template <typename Container>
auto SerializeInto(Container& container) -> SerializeIterator<Container>
{
    static_assert(details::is_supported_sequence_container<Container>::value, "Provided Container is not supported.");

    return SerializeIterator<Container>(container);
}

/// @brief DSerializeIterator acts nearly same as any LegacyForwardIterator, but in difference to standard iterators, it
/// will never go past `end()` of a container. This way we can ensure at a central location that no out-of-bound reads
/// are happening without bothering with it in every of our deserialization logics.
///
/// @tparam Container The container this iterator should be associated with
template <class Container>
class DeSerializeIterator
{
  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = typename Container::value_type;
    using pointer = typename Container::value_type*;
    using reference = typename Container::value_type&;

    explicit DeSerializeIterator(const Container& container)
        : container_(container), iterator_{container_.cbegin()}, result_{std::make_shared<Result<std::size_t>>()}
    {
        static_assert(details::is_supported_sequence_container<Container>::value,
                      "Provided Container is not supported.");
    }

    /// @brief Access underling value
    const typename Container::value_type& operator*()
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(iterator_ != container_.cend());
        return *iterator_;
    }

    /// @brief Move iterator to next position (but not past end())
    DeSerializeIterator<Container>& operator++()
    {
        if (iterator_ != container_.cend())
        {
            iterator_++;
        }
        else
        {
            *result_ = MakeUnexpected(SerializationError::kOutOfBoundsWrite);
        }
        return *this;
    }

    /// @brief Move iterator to next position (but not past end())
    DeSerializeIterator<Container> operator++(int)
    {
        DeSerializeIterator<Container> previous{*this};
        if (iterator_ != container_.cend())
        {
            ++iterator_;
        }
        return previous;
    }

    /// @brief Indicate how much the iterator was moved and if an out-of-bound access happend
    Result<std::size_t> GetResult() const
    {
        if (not result_->has_value())
        {
            return *result_;
        }

        return GetNumberOfBytesWritten();
    }

    // Suppress "AUTOSAR C++14 A3-3-1" rule finding. This rule states:"Objects or functions with external linkage
    // (including members of named namespaces) shall be declared in a header file."
    // Rationale: This function is declared in a header file
    // coverity[autosar_cpp14_a3_3_1_violation]
    friend bool operator==(const DeSerializeIterator& left, const DeSerializeIterator& right) noexcept
    {
        return left.iterator_ == right.iterator_;
    };

    // necessary for stl algorithms like std::advance
    // Suppress "AUTOSAR C++14 A3-3-1" rule violation. The rule states "Objects or functions with external linkage
    // (including members of named namespaces) shall be declared in a header file." This is necessary for stl algorithms
    // like std::advance hench suppressing.
    // coverity[autosar_cpp14_a3_3_1_violation]
    friend bool operator!=(const DeSerializeIterator& left, const DeSerializeIterator& right) noexcept
    {
        return !(left == right);
    };

    DeSerializeIterator End() const
    {
        DeSerializeIterator end{container_};
        constexpr difference_type max_difference_type = std::numeric_limits<difference_type>::max();
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(static_cast<difference_type>(container_.size()) < max_difference_type);
        std::advance(end, static_cast<difference_type>(container_.size()));
        return end;
    }

  private:
    // coverity[autosar_cpp14_a0_1_3_violation : FALSE]
    std::size_t GetNumberOfBytesWritten() const
    {
        const auto distance = std::distance(container_.cbegin(), iterator_);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(distance >=
                                            0);  // we start at begin and go on from there, it can not be negative.
        return static_cast<std::size_t>(distance);
    }

    const Container& container_;
    typename Container::const_iterator iterator_;
    std::shared_ptr<Result<std::size_t>> result_;
};

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_SERIALIZE_ITERATOR_H
