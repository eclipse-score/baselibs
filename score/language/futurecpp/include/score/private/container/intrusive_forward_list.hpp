/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_CONTAINER_INTRUSIVE_FORWARD_LIST_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_CONTAINER_INTRUSIVE_FORWARD_LIST_HPP

#include <type_traits>
#include <score/assert.hpp>

namespace score::cpp
{
namespace detail
{

/// \brief Base class for elements stored in `intrusive_forward_list`.
///
/// When inherited the class adds the necessary bookkeeping information to the elements of an `intrusive_forward_list`.
/// \see intrusive_forward_list for an example.
struct intrusive_forward_list_node
{
public:
    /// \brief Initialize an empty node.
    constexpr intrusive_forward_list_node() noexcept : next_{nullptr} {}
    /// \{
    /// \brief Copy or move from other.
    ///
    /// Copy- or move-constructing an intrusive_forward_list_node does not transfer the bookkeeping information.
    /// Given `auto element = list.front()`, `element` is a not owned by `list`. Adding or removing nodes must be done
    /// via the `intrusive_forward_list` API.
    // NOLINTNEXTLINE(bugprone-unhandled-self-assignment)
    constexpr intrusive_forward_list_node& operator=(const intrusive_forward_list_node&) noexcept { return *this; }
    constexpr intrusive_forward_list_node(const intrusive_forward_list_node&) noexcept : next_{nullptr} {}
    constexpr intrusive_forward_list_node& operator=(intrusive_forward_list_node&&) noexcept { return *this; }
    constexpr intrusive_forward_list_node(intrusive_forward_list_node&&) noexcept : next_{nullptr} {}
    /// \}

    /// \brief Returns whether this node is currently used in an `intrusive_forward_list`.
    ///
    /// \return true if node is used in `intrusive_forward_list`, otherwise false.
    constexpr bool is_linked() const noexcept { return next_ != nullptr; }

protected:
    /// \brief Destroy the node.
    ///
    /// \note `protected` to avoid destruction through pointer to base.
    ~intrusive_forward_list_node() noexcept = default;

private:
    template <typename>
    friend class intrusive_forward_list;

    /// \brief Initialize from node.
    explicit constexpr intrusive_forward_list_node(intrusive_forward_list_node* const next) noexcept : next_{next} {}

    /// \brief Value used to denote the end of the list.
    ///
    /// The property of the sentinel is
    ///   - unique address
    ///   - address is different from `nulltpr`
    static constexpr intrusive_forward_list_node* end_of_list() noexcept
    {
        // sentinel should never be modified. cast const away to allow sanitizer detecting this case. the alternative
        // would be to have a non-const global variable. but this disallows detecting the error case with sanitizers.
        return const_cast<intrusive_forward_list_node*>(&sentinel); // NOLINT(cppcoreguidelines-pro-type-const-cast)
    }

    /// \brief Pointer to the next node in the intrusive forward list.
    intrusive_forward_list_node* next_;

    /// \brief Object for taking the address to denote the end of the list.
    ///
    /// Use `end_of_list()` function.
    static const intrusive_forward_list_node sentinel;
};

/// \brief An intrusive forward list
///
/// An intrusive container does not own its elements. Instead, its elements are stored outside the list. It is important
/// that the elements outlive the list object. The bookkeeping information is stored in the elements itself. To use an
/// element with `intrusive_forward_list` one needs to inherit from `intrusive_forward_list_node`. This adds the
/// necessary bookkepping information.
///
/// \code{.cpp}
/// struct MyClass : intrusive_forward_list_node {};
/// intrusive_forward_list<MyClass> l{};
/// \endcode
///
/// \tparam T Type of the elements. Must inherit from `intrusive_forward_list_node`.
///
/// Implements https://wg21.link/p0406
template <typename T>
class intrusive_forward_list
{
    static_assert(std::is_base_of<intrusive_forward_list_node, T>::value,
                  "T does not inherit from intrusive_forward_list_node");

public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;

    /// \brief Initializes an empty container.
    ///
    /// Complexity: O(1)
    constexpr intrusive_forward_list() noexcept
        : before_begin_{intrusive_forward_list_node::end_of_list()}, last_{&before_begin_} {};

    /// \brief Moves elements from other.
    ///
    /// Complexity: O(n)
    ///
    /// Other is empty afterwards.
    constexpr intrusive_forward_list& operator=(intrusive_forward_list&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            if (!other.empty())
            {
                before_begin_.next_ = other.before_begin_.next_;
                last_ = other.last_;
                other.before_begin_.next_ = intrusive_forward_list_node::end_of_list();
                other.last_ = &other.before_begin_;
            }
        }

        return *this;
    };

    /// \brief Moves elements from other.
    ///
    /// Complexity: O(1)
    ///
    /// Other is empty afterwards.
    constexpr intrusive_forward_list(intrusive_forward_list&& other) noexcept : intrusive_forward_list{}
    {
        if (!other.empty())
        {
            before_begin_.next_ = other.before_begin_.next_;
            last_ = other.last_;
            other.before_begin_.next_ = intrusive_forward_list_node::end_of_list();
            other.last_ = &other.before_begin_;
        }
    };

    constexpr intrusive_forward_list& operator=(const intrusive_forward_list&) = delete;
    constexpr intrusive_forward_list(const intrusive_forward_list&) = delete;

    /// \brief Removes all elements from the container and destroys the container.
    ///
    /// Note the elements themselves are not destroyed.
    ///
    /// Complexity: O(n)
    ~intrusive_forward_list() noexcept { clear(); };

    /// \{
    /// \brief Returns a reference to the last element in the container.
    ///
    /// Complexity: O(1)
    ///
    /// \pre !empty()
    constexpr reference back()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(!empty());
        return *static_cast<T*>(last_);
    }
    constexpr const_reference back() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(!empty());
        return *static_cast<T*>(last_);
    }
    /// \}

    /// \{
    /// \brief Returns a reference to the first element in the container.
    ///
    /// Complexity: O(1)
    ///
    /// \pre !empty()
    constexpr reference front()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(!empty());
        return *static_cast<T*>(before_begin_.next_);
    }
    constexpr const_reference front() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(!empty());
        return *static_cast<T*>(before_begin_.next_);
    }
    /// \}

    /// \brief Appends the given element value to the end of the container.
    ///
    /// Complexity: O(1)
    ///
    /// \pre value is not element of the container
    constexpr void push_back(reference value)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(!value.is_linked());

        last_->next_ = &value;
        last_ = last_->next_;
        last_->next_ = intrusive_forward_list_node::end_of_list();
    }

    /// \brief Removes the first element of the container.
    ///
    /// Complexity: O(1)
    ///
    /// \pre !empty()
    constexpr void pop_front()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(!empty());

        intrusive_forward_list_node* const to_be_erased{before_begin_.next_};
        before_begin_.next_ = to_be_erased->next_;
        to_be_erased->next_ = nullptr;
        if (empty())
        {
            last_ = &before_begin_;
        }
    }

    /// \brief Erases all elements from the container.
    ///
    /// Complexity: O(n)
    constexpr void clear() noexcept
    {
        intrusive_forward_list_node* it{before_begin_.next_};
        while (it != intrusive_forward_list_node::end_of_list())
        {
            intrusive_forward_list_node* const tmp{it->next_};
            it->next_ = nullptr;
            it = tmp;
        }
        before_begin_.next_ = intrusive_forward_list_node::end_of_list();
        last_ = &before_begin_;
    }

    /// \brief Checks if the container has no elements.
    ///
    /// Complexity: O(1)
    ///
    /// \return true if the container is empty, false otherwise
    constexpr bool empty() const noexcept { return before_begin_.next_ == intrusive_forward_list_node::end_of_list(); }

    /// \brief Exchanges the contents of the container with those of other.
    ///
    /// Complexity: O(1)
    ///
    /// Does not invoke any move, copy, or swap operations on individual elements.
    constexpr void swap(intrusive_forward_list& other) noexcept
    {
        {
            intrusive_forward_list_node* const tmp{before_begin_.next_};
            before_begin_.next_ = other.before_begin_.next_;
            other.before_begin_.next_ = tmp;
        }
        {
            intrusive_forward_list_node* const tmp{last_};
            last_ = other.last_;
            other.last_ = tmp;
        }
        if (empty())
        {
            last_ = &before_begin_;
        }
        if (other.empty())
        {
            other.last_ = &other.before_begin_;
        }
    }

private:
    /// \brief Node pointing to the beginning of the list.
    intrusive_forward_list_node before_begin_;
    /// \brief Pointer to the last node in the list to allow fast container end access.
    intrusive_forward_list_node* last_;
};

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_CONTAINER_INTRUSIVE_FORWARD_LIST_HPP
