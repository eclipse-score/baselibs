/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.InplaceVector component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_INPLACE_VECTOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_INPLACE_VECTOR_HPP

#include <score/private/iterator/iterator.hpp> // IWYU pragma: export

#include <score/private/memory/uninitialized_move.hpp>
#include <score/private/memory/uninitialized_value_construct.hpp>
#include <score/assert.hpp>
#include <score/utility.hpp>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <type_traits>

namespace score::cpp
{

/// \brief Compile-time fixed capacity vector
///
/// Can be used as replacement for std::vector when maximum size of vector is known at compile time.
///
/// Implements https://wg21.link/p0843. \todo document the differences
///
/// \tparam T Specifies the value type of a single element of a row.
/// \tparam MaxSize Specifies maximum size of the internal array.
template <typename T, std::size_t MaxSize>
class inplace_vector
{
public:
    /// \brief The type of the array elements.
    using value_type = T;

    /// \brief The type to hold a size information
    using size_type = std::size_t;

    /// \brief The type that can be used to identify distance between iterators
    using difference_type = std::ptrdiff_t;

    /// \brief The type of a non-const iterator
    using iterator = score::cpp::detail::iterator<inplace_vector<T, MaxSize>>;

    using reverse_iterator = std::reverse_iterator<iterator>;

    /// \brief The type of a const iterator
    using const_iterator = score::cpp::detail::iterator<inplace_vector<T, MaxSize>, true>;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /// \brief The value type as non-const reference.
    using reference = T&;

    /// \brief The value type a const reference.
    using const_reference = const T&;

    /// \brief The value type as non-const pointer.
    using pointer = T*;

    /// \brief The value type a const pointer.
    using const_pointer = const T*;

    /// \brief Default constructor. Constructs an empty container.
    inplace_vector() noexcept : size_{0U} {}

    /// \brief Constructs the container with count copies of elements with default value.
    ///
    /// \pre count <= MaxSize
    ///
    /// \param count The number of copies of value.
    explicit inplace_vector(const size_type count) : size_{0U}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(count <= MaxSize);
        append(count);
    }

    /// \brief Constructs the container with count copies of elements with value value.
    ///
    /// \pre count <= MaxSize
    ///
    /// \param count The number of copies of value.
    /// \param value The value to initialize elements of the container with.
    explicit inplace_vector(const size_type count, const_reference value) : size_{0U}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(count <= MaxSize);
        append(count, value);
    }

    /// \brief Constructs the container with the provided iterators
    ///
    /// \param begin an iterator pointing to the start of the source container.
    /// \param end an iterator pointing to the end of the source container.
    /// \pre the distance between the input iterators shall be in a range of [0, MaxSize]
    template <typename InputIterator,
              typename = decltype(*std::declval<InputIterator&>(), ++std::declval<InputIterator&>())>
    explicit inplace_vector(InputIterator begin, InputIterator end) : size_{0U}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(std::distance(begin, end) >= 0);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(static_cast<std::size_t>(std::distance(begin, end)) <= MaxSize);
        try
        {
            const auto new_end = std::uninitialized_copy(begin, end, std::begin(*this));
            size_ = static_cast<std::size_t>(new_end - std::begin(*this));
        }
        catch (...)
        {
            // do nothing
        }
    }

    /// \brief Copy construction from inplace_vector.
    inplace_vector(const inplace_vector& other) noexcept : size_{other.size_}
    {
        try
        {
            score::cpp::ignore = std::uninitialized_copy_n(other.data(), size_, this->data());
        }
        catch (...)
        {
            size_ = 0U;
        }
    }

    /// \brief Move construction from inplace_vector.
    ///
    /// Constructs the container with the contents of other using move semantics. After the move, other is guaranteed to
    /// be empty().
    ///
    /// If an exception is thrown during the move,
    ///  - the objects already moved are destroyed
    ///  - other is guaranteed to be empty()
    ///  - *this is guaranteed to be empty()
    ///
    /// This gives basic exception guarantee.
    inplace_vector(inplace_vector&& other) noexcept : size_{other.size_}
    {
        try
        {
            score::cpp::ignore = score::cpp::uninitialized_move_n(other.data(), size_, this->data());
        }
        catch (...)
        {
            size_ = 0U;
        }
        other.clear();
    }

    /// \brief Constructor with initializer list.
    ///
    /// \pre initializer_list.size() <= MaxSize
    ///
    /// @param initializer_list The initializer_list to construct the inplace_vector from
    inplace_vector(const std::initializer_list<T> initializer_list)
        : inplace_vector(initializer_list.begin(), initializer_list.end())
    {
    }

    /// \brief Copy assignment from inplace_vector
    ///
    /// Gives basic exception guarantee
    ///
    /// \param other The vector to copy from
    inplace_vector& operator=(const inplace_vector& other)
    {
        if (this != &other)
        {
            try
            {
                clear();
                size_ = other.size_;
                score::cpp::ignore = std::uninitialized_copy_n(other.data(), size_, this->data());
            }
            catch (...)
            {
                size_ = 0U;
            }
        }
        return *this;
    }

    /// \brief move assignment operator
    ///
    ///
    /// Replaces the contents with those of other using move semantics, i.e., the data in other is moved from other into
    /// this container. After the move, other is guaranteed to be empty().
    ///
    /// If an exception is thrown during the move,
    ///  - the objects already moved are destroyed
    ///  - other is guaranteed to be empty()
    ///  - *this is guaranteed to be empty()
    ///
    /// This gives basic exception guarantee.
    ///
    /// \param other The vector to move from
    inplace_vector& operator=(inplace_vector&& other) noexcept((MaxSize == 0) ||
                                                               (std::is_nothrow_move_assignable<T>::value &&
                                                                std::is_nothrow_move_constructible<T>::value))
    {
        if (this != &other)
        {
            try
            {
                clear();
                size_ = other.size_;
                score::cpp::ignore = score::cpp::uninitialized_move_n(other.data(), size_, this->data());
            }
            catch (...)
            {
                size_ = 0U;
            }
            other.clear();
        }
        return *this;
    }

    /// \brief Clears the vector.
    ~inplace_vector() { clear(); }

    /// \brief Mimics std::vector<>::clear()
    void clear() { shrink(0U); }

    /// \brief Mimics std::vector<>::assign(size_type count, const T& value).
    void assign(const size_type count, const_reference value)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(count <= MaxSize);
        clear();
        append(count, value);
    }

    /// \brief Mimics std::vector<>::resize(size_type count).
    void resize(const size_type count)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(count <= MaxSize);
        if (count < size())
        {
            shrink(count);
        }
        else
        {
            append(count - size());
        }
    }

    /// \brief Mimics std::vector<>::resize(size_type count, const T& value).
    void resize(const size_type count, const_reference value)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(count <= MaxSize);
        if (count < size())
        {
            shrink(count);
        }
        else
        {
            append(count - size(), value);
        }
    }

    /// \brief Mimics std::vector<>::capacity().
    ///
    /// \return capacity, which is MaxSize.
    static constexpr std::size_t capacity() { return MaxSize; }

    /// \brief Mimics std::vector<>::max_size().
    ///
    /// \return maximum size, which is MaxSize.
    static constexpr std::size_t max_size() { return MaxSize; }

    /// \brief Mimics std::vector<>::empty()
    bool empty() const { return (0U == size()); }

    /// \brief Returns true if vector is full.
    ///
    /// \return True if vector is full - otherwise false.
    bool full() const { return (MaxSize == size()); }

    /// \brief Mimics std::vector<>::operator[]()
    ///
    /// \pre n < size()
    reference operator[](const size_type n)
    {
        static_assert(MaxSize < static_cast<size_type>(std::numeric_limits<difference_type>::max()), "Overflow");
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(n < size());
        // The following static_cast is valid because of static_assert above.
        return *(std::begin(*this) + static_cast<difference_type>(n));
    }

    /// \brief Mimics std::vector<>::operator[]()
    ///
    /// \pre n < size()
    const_reference operator[](const size_type n) const
    {
        static_assert(MaxSize < static_cast<size_type>(std::numeric_limits<difference_type>::max()), "Overflow");
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(n < size());
        // The following static_cast is valid because of static_assert above.
        return *(std::cbegin(*this) + static_cast<difference_type>(n));
    }

    /// \brief Returns the number elements in the vector
    ///
    /// \return The number of elements in the container.
    size_type size() const { return size_; }

    /// \brief Returns a pointer to the first element of the internal array.
    ///
    /// \return Pointer to the first element of the internal array.
    /// \{
    T* data() { return reinterpret_cast<T*>(array_); }
    const T* data() const { return reinterpret_cast<const T*>(array_); }
    /// \}

    /// \brief Returns an iterator to the beginning
    ///
    /// \return Iterator to the first element in the row.
    /// \{
    iterator begin() { return iterator{*this, true}; }
    const_iterator begin() const { return const_iterator{*this, true}; }
    const_iterator cbegin() const { return std::cbegin(*this); }
    /// \}

    /// \brief Returns an iterator to the reverse-beginning of the container
    ///
    /// \return Iterator to the last element of the vector.
    /// \{
    reverse_iterator rbegin() { return reverse_iterator{end()}; }
    const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
    const_reverse_iterator crbegin() const { return const_reverse_iterator{cend()}; }
    /// \}

    /// \brief Returns an iterator one past the end
    ///
    /// Attempting to access it, results in undefined behavior.
    ///
    /// \return Iterator one past the last element of the vector.
    /// \{
    iterator end() { return iterator{*this, false}; }
    const_iterator end() const { return const_iterator{*this, false}; }
    const_iterator cend() const { return std::cend(*this); }
    /// \}

    /// \brief Returns an iterator to the reverse-end of the container
    ///
    /// Attempting to access it, results in undefined behavior.
    ///
    /// \return Iterator one before the first element of the container.
    /// \{
    reverse_iterator rend() { return reverse_iterator{begin()}; }
    const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }
    const_reverse_iterator crend() const { return const_reverse_iterator{cbegin()}; }
    /// \}

    /// \brief Inserts new element in vector at specified position
    ///
    /// \pre std::cbegin(*this) <= where <= std::cend(*this)
    ///
    /// \param where Position in the vector where the new element is inserted.
    /// \param value Value to be copied to the inserted elements.
    ///
    /// \return An iterator that points to the the newly inserted element.
    iterator insert(const const_iterator where, const_reference value) { return insert_impl(where, value); }

    /// \brief Inserts new rvalue element in vector at specified position
    ///
    /// \pre std::cbegin(*this) <= where <= std::cend(*this)
    ///
    /// \param where Position in the vector where the new element is inserted.
    /// \param value Value to be moved to the inserted elements.
    ///
    /// \return An iterator that points to the the newly inserted element.
    iterator insert(const const_iterator where, value_type&& value) { return insert_impl(where, std::move(value)); }

    /// \brief Inserts elements from range [first, last) before a specific position
    ///
    /// \pre std::cbegin(*this) <= where <= std::cend(*this)
    /// \pre the distance between the input iterators shall be in the range of [0, MaxSize - current_size]
    ///
    /// \param where Position in the vector before which the new elements are inserted.
    /// \param first The start of the range of elements to insert
    /// \param last The end of the range of elements to insert
    ///
    /// \return An iterator pointing to the first element inserted, or where if first==last.
    template <typename InputIterator>
    iterator insert(const const_iterator where, InputIterator first, InputIterator last)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(std::cbegin(*this) <= where);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(where <= std::cend(*this));
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(std::distance(first, last) >= 0);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION((static_cast<std::size_t>(std::distance(first, last)) + size_) <= MaxSize);

        const auto old_end = end();
        const auto new_end = std::uninitialized_copy(first, last, std::end(*this));
        const difference_type num_new_elements{new_end - old_end};
        size_ += static_cast<std::size_t>(num_new_elements);
        const iterator it{const_iterator_cast(where)};
        score::cpp::ignore = std::rotate(it, old_end, new_end);

        return it;
    }

    /// \brief Removes the element at where.
    ///
    /// \pre std::cbegin(*this) <= where < std::cend(*this)
    ///
    /// \param where Iterator pointing to a single element to be removed from the vector.
    ///
    /// \return An iterator pointing to the new location of the element that followed the last element erased by the
    /// function call.
    iterator erase(const const_iterator where)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(std::cbegin(*this) <= where);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(where < std::cend(*this));
        const iterator it{const_iterator_cast(where)}; // same as where but non-constant iterator
        score::cpp::ignore = std::move(it + 1, std::end(*this), it);
        pop_back();
        return it;
    }

    /// \brief Removes the elements in the range [first, last).
    ///
    /// \pre first <= last
    /// \pre std::cbegin(*this) <= first
    /// \pre last <= std::cend(*this)
    ///
    /// \param first Iterator pointing to the first element to be removed from the vector.
    /// \param last Iterator pointing to the last element to be removed from the vector.
    ///
    /// \return Iterator pointing to the new location of the element that followed the last element erased by the
    /// function call.
    iterator erase(const const_iterator first, const const_iterator last)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(first <= last);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(std::cbegin(*this) <= first);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(last <= std::cend(*this));
        const iterator first_non_const{const_iterator_cast(first)}; // same as first but non-constant iterator
        const iterator last_non_const{const_iterator_cast(last)};   // same as last but non-constant iterator
        score::cpp::ignore = std::move(last_non_const, std::end(*this), first_non_const);
        const difference_type number_of_elements_to_erase{std::distance(first, last)};
        for (difference_type i{0}; i < number_of_elements_to_erase; ++i)
        {
            pop_back();
        }
        return first_non_const;
    }

    /// \brief Mimics std::vector<>::push_back()
    ///
    /// \pre size() < MaxSize
    void push_back(const_reference value) { emplace_back(value); }

    /// \brief Mimics std::vector<>::push_back() for rvalues
    ///
    /// \pre size() < MaxSize
    void push_back(value_type&& value) { emplace_back(std::move(value)); }

    /// \brief Mimics std::vector<>::emplace_back()
    ///
    /// \pre size() < MaxSize
    template <typename... Ts>
    void emplace_back(Ts&&... arguments)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(size() < MaxSize);
        score::cpp::ignore = ::new (data() + size()) T{std::forward<Ts>(arguments)...};
        ++size_;
        SCORE_LANGUAGE_FUTURECPP_ASSERT(size() <= MaxSize);
    }

    /// \brief Mimics std::vector<>::pop_back()
    ///
    /// \pre !empty()
    void pop_back()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        back().~T();
        --size_;
        SCORE_LANGUAGE_FUTURECPP_ASSERT(size() < MaxSize);
    }

    /// \brief Mimics std::vector<>::back()
    ///
    /// \pre !empty()
    reference back()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return (*this)[size() - 1U];
    }

    /// \brief Mimics std::vector<>::back()
    ///
    /// \pre !empty()
    const_reference back() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return (*this)[size() - 1U];
    }

    /// \brief Mimics std::vector<>::front()
    ///
    /// \pre !empty()
    reference front()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return (*this)[0U];
    }

    /// \brief Mimics std::vector<>::front()
    ///
    /// \pre !empty()
    const_reference front() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return (*this)[0U];
    }

private:
    /// \brief Removes (cast away) constness from iterator.
    iterator const_iterator_cast(const const_iterator& it) { return std::begin(*this) + (it - std::cbegin(*this)); }

    /// \brief Inserts new element in vector at specified position using forwarding references
    ///
    /// \pre std::cbegin(*this) <= where <= std::cend(*this)
    template <typename U>
    iterator insert_impl(const const_iterator where, U&& value)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(std::cbegin(*this) <= where);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(where <= std::cend(*this));
        emplace_back(std::forward<U>(value));
        const iterator it{const_iterator_cast(where)}; // same as where but non-constant iterator
        score::cpp::ignore = std::rotate(it, std::end(*this) - 1, std::end(*this));
        return it;
    }

    /// \brief Shrinks container so that `new_size` is the result of `size()` afterwards.
    void shrink(const size_type new_size)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(new_size <= size());
        for (size_type i{new_size}; i < size(); ++i)
        {
            (data() + i)->~T();
        }
        size_ = new_size;
    }

    /// \brief Appends `n` default-inserted `T`s.
    void append(const size_type n)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((size() + n) <= MaxSize);
        score::cpp::ignore = score::cpp::uninitialized_value_construct_n(data() + size(), n);
        size_ += n;
    }

    /// \brief Appends `n` copies of `value`.
    void append(const size_type n, const_reference value)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((size() + n) <= MaxSize);
        score::cpp::ignore = std::uninitialized_fill_n(data() + size(), n, value);
        size_ += n;
    }

    /// \brief Internal data array
    typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type array_[MaxSize];

    /// \brief Used size of data array
    std::size_t size_;
};

/// \brief Mimics std::vector operator==
template <typename T, std::size_t MaxSize>
inline bool operator==(const inplace_vector<T, MaxSize>& lhs, const inplace_vector<T, MaxSize>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/// \brief Mimics std::vector operator!=
template <typename T, std::size_t MaxSize>
inline bool operator!=(const inplace_vector<T, MaxSize>& lhs, const inplace_vector<T, MaxSize>& rhs)
{
    return !(lhs == rhs);
}

/// \brief Mimics std::vector operator<
template <typename T, std::size_t MaxSize>
inline bool operator<(const inplace_vector<T, MaxSize>& lhs, const inplace_vector<T, MaxSize>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/// \brief Mimics std::vector operator>
template <typename T, std::size_t MaxSize>
inline bool operator>(const inplace_vector<T, MaxSize>& lhs, const inplace_vector<T, MaxSize>& rhs)
{
    return rhs < lhs;
}

/// \brief Mimics std::vector operator<=
template <typename T, std::size_t MaxSize>
inline bool operator<=(const inplace_vector<T, MaxSize>& lhs, const inplace_vector<T, MaxSize>& rhs)
{
    return !(rhs < lhs);
}

/// \brief Mimics std::vector operator>=
template <typename T, std::size_t MaxSize>
inline bool operator>=(const inplace_vector<T, MaxSize>& lhs, const inplace_vector<T, MaxSize>& rhs)
{
    return !(lhs < rhs);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_INPLACE_VECTOR_HPP
