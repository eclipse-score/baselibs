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
#include <score/private/memory/voidify.hpp>

#include <score/assert.hpp>
#include <score/utility.hpp>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <type_traits>

namespace score::cpp
{

namespace detail
{
namespace inplace_vector
{

template <typename T, std::size_t MaxSize, bool = std::is_trivially_destructible_v<T>>
class destructor_base
{
    static_assert(std::is_trivially_destructible_v<T>);

public:
    destructor_base() noexcept : size_{0}
    {
        // activate the union member. `start_lifetime_as_array` does not exist in C++17
        // With C++26 trivial union paper wg21.link/p3074 + wg21.link/p3726 this isn't needed
        // https://en.cppreference.com/w/cpp/memory/start_lifetime_as.html says
        // "`new (void_ptr) unsigned char[size]` or ... works as an untyped version of
        // `std::start_lifetime_as`, but it does not keep the object representation."
        score::cpp::ignore = ::new (score::cpp::detail::voidify(array_)) std::uint8_t[sizeof(T) * MaxSize];
    }

    void set_size(const std::size_t n) { size_ = n; }
    std::size_t size() const { return size_; }
    T* data() { return array_; }
    const T* data() const { return array_; }

private:
    union
    {
        // NOLINTNEXTLINE(readability-identifier-naming) keep `_` to make clear it is a member variable
        T array_[MaxSize];
    };
    std::size_t size_;
};

template <typename T, std::size_t MaxSize>
class destructor_base<T, MaxSize, false>
{
    static_assert(!std::is_trivially_destructible_v<T>);

public:
    destructor_base() noexcept : size_{0}
    {
        score::cpp::ignore = ::new (score::cpp::detail::voidify(array_)) std::uint8_t[sizeof(T) * MaxSize];
    }
    destructor_base(const destructor_base&) = default;
    destructor_base& operator=(const destructor_base&) = default;
    destructor_base(destructor_base&&) = default;
    destructor_base& operator=(destructor_base&&) = default;
    ~destructor_base() { score::cpp::ignore = std::destroy_n(data(), size()); }

    void set_size(const std::size_t n) { size_ = n; }
    std::size_t size() const { return size_; }
    T* data() { return array_; }
    const T* data() const { return array_; }

private:
    union
    {
        // NOLINTNEXTLINE(readability-identifier-naming) keep `_` to make clear it is a member variable
        T array_[MaxSize];
    };
    std::size_t size_;
};

} // namespace inplace_vector
} // namespace detail

/// \brief Compile-time fixed capacity vector
///
/// Can be used as replacement for std::vector when maximum size of vector is known at compile time.
///
/// Implements https://wg21.link/p0843. \todo document the differences
///
/// \tparam T Specifies the value type of a single element of a row.
/// \tparam MaxSize Specifies maximum size of the internal array.
template <typename T, std::size_t MaxSize>
class inplace_vector : private detail::inplace_vector::destructor_base<T, MaxSize>
{
    using base_t = detail::inplace_vector::destructor_base<T, MaxSize>;

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
    inplace_vector() noexcept : base_t{} {}

    /// \brief Constructs the container with count copies of elements with default value.
    ///
    /// \param count The number of copies of value.
    ///
    /// \pre count <= MaxSize
    explicit inplace_vector(const size_type count) : base_t{}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(count <= MaxSize);
        append(count);
    }

    /// \brief Constructs the container with count copies of elements with value value.
    ///
    /// \param count The number of copies of value.
    /// \param value The value to initialize elements of the container with.
    ///
    /// \pre count <= MaxSize
    explicit inplace_vector(const size_type count, const_reference value) : base_t{}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(count <= MaxSize);
        append(count, value);
    }

    /// \brief Constructs the container with the provided iterators
    ///
    /// \param begin an iterator pointing to the start of the source container.
    /// \param end an iterator pointing to the end of the source container.
    ///
    /// \pre the distance between the input iterators shall be in a range of [0, MaxSize]
    template <typename InputIterator,
              typename = decltype(*std::declval<InputIterator&>(), ++std::declval<InputIterator&>())>
    explicit inplace_vector(InputIterator begin, InputIterator end)
        : inplace_vector{begin, end, typename std::iterator_traits<InputIterator>::iterator_category{}}
    {
    }

    /// \brief Constructor with initializer list.
    ///
    /// \param initializer_list The initializer_list to construct the inplace_vector from
    ///
    /// \pre initializer_list.size() <= MaxSize
    inplace_vector(const std::initializer_list<T> initializer_list)
        : inplace_vector(initializer_list.begin(), initializer_list.end())
    {
    }

    /// \brief Copy construction from inplace_vector.
    inplace_vector(const inplace_vector& other) noexcept((MaxSize == 0) || std::is_nothrow_copy_constructible<T>::value)
        : base_t{}
    {
        try
        {
            score::cpp::ignore = std::uninitialized_copy_n(other.data(), other.size(), this->data());
            this->set_size(other.size());
        }
        catch (...)
        {
            // do nothing because `uninitialized_copy_n` destroys all already copied elements
        }
    }

    /// \brief Copy assignment from inplace_vector.
    inplace_vector& operator=(const inplace_vector& other) noexcept((MaxSize == 0) ||
                                                                    (std::is_nothrow_copy_assignable<T>::value &&
                                                                     std::is_nothrow_copy_constructible<T>::value))
    {
        if (this != &other)
        {
            try
            {
                score::cpp::ignore = std::destroy_n(this->data(), this->size());
                score::cpp::ignore = std::uninitialized_copy_n(other.data(), other.size(), this->data());
                this->set_size(other.size());
            }
            catch (...)
            {
                // do nothing because `uninitialized_copy_n` destroys all already copied elements
            }
        }
        return *this;
    }

    /// \brief Move construction from inplace_vector.
    inplace_vector(inplace_vector&& other) noexcept((MaxSize == 0) || std::is_nothrow_move_constructible<T>::value)
        : base_t{}
    {
        try
        {
            score::cpp::ignore = std::uninitialized_move_n(other.data(), other.size(), this->data());
            this->set_size(other.size());
        }
        catch (...)
        {
            // do nothing because `uninitialized_move_n` destroys all already copied elements
        }
        score::cpp::ignore = std::destroy_n(other.data(), other.size());
        other.set_size(0U);
    }

    /// \brief Move assignment from inplace_vector.
    inplace_vector& operator=(inplace_vector&& other) noexcept((MaxSize == 0) ||
                                                               (std::is_nothrow_move_assignable<T>::value &&
                                                                std::is_nothrow_move_constructible<T>::value))
    {
        if (this != &other)
        {
            try
            {
                score::cpp::ignore = std::destroy_n(this->data(), this->size());
                score::cpp::ignore = std::uninitialized_move_n(other.data(), other.size(), this->data());
                this->set_size(other.size());
            }
            catch (...)
            {
                // do nothing because `uninitialized_move_n` destroys all already copied elements
            }
            score::cpp::ignore = std::destroy_n(other.data(), other.size());
            other.set_size(0U);
        }
        return *this;
    }

    /// \brief Destruct inplace_vector
    ///
    /// The destructor is trivial if `T` is trivial destructible
    ~inplace_vector() = default;

    /// \brief Mimics std::vector<>::clear()
    void clear() { shrink(0U); }

    /// \brief Mimics std::vector<>::assign(size_type count, const T& value).
    ///
    /// \param count Number of elements to assign.
    /// \param value Value to assign to the elements.
    ///
    /// \pre count <= MaxSize
    void assign(const size_type count, const_reference value)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(count <= MaxSize);
        clear();
        append(count, value);
    }

    /// \brief Mimics std::vector<>::resize(size_type count).
    ///
    /// \param count New size of the vector.
    ///
    /// \pre count <= MaxSize
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
    ///
    /// \param count New size of the vector.
    /// \param value Value to initialize new elements with.
    ///
    /// \pre count <= MaxSize
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
    ///
    /// \return True if vector is empty - otherwise false.
    bool empty() const { return (0U == size()); }

    /// \brief Returns true if vector is full.
    ///
    /// \return True if vector is full - otherwise false.
    bool full() const { return (MaxSize == size()); }

    /// \brief Mimics std::vector<>::operator[]()
    ///
    /// \pre n < size()
    ///
    /// \return Reference to the n-th element.
    /// \{
    reference operator[](const size_type n)
    {
        static_assert(MaxSize < static_cast<size_type>(std::numeric_limits<difference_type>::max()), "Overflow");
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(n < size());
        // The following static_cast is valid because of static_assert above.
        return *(std::begin(*this) + static_cast<difference_type>(n));
    }
    const_reference operator[](const size_type n) const
    {
        static_assert(MaxSize < static_cast<size_type>(std::numeric_limits<difference_type>::max()), "Overflow");
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(n < size());
        // The following static_cast is valid because of static_assert above.
        return *(std::cbegin(*this) + static_cast<difference_type>(n));
    }
    /// \}

    /// \brief Returns the number elements in the vector
    ///
    /// \return The number of elements in the container.
    size_type size() const { return base_t::size(); }

    /// \brief Returns a pointer to the first element of the internal array.
    ///
    /// \return Pointer to the first element of the internal array.
    /// \{
    T* data() { return base_t::data(); }
    const T* data() const { return base_t::data(); }
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
    /// \param where Position in the vector where the new element is inserted.
    /// \param value Value to be copied to the inserted elements.
    ///
    /// \pre std::cbegin(*this) <= where <= std::cend(*this)
    ///
    /// \return An iterator that points to the the newly inserted element.
    iterator insert(const const_iterator where, const_reference value) { return insert_impl(where, value); }

    /// \brief Inserts new rvalue element in vector at specified position
    ///
    /// \param where Position in the vector where the new element is inserted.
    /// \param value Value to be moved to the inserted elements.
    ///
    /// \pre std::cbegin(*this) <= where <= std::cend(*this)
    ///
    /// \return An iterator that points to the the newly inserted element.
    iterator insert(const const_iterator where, value_type&& value) { return insert_impl(where, std::move(value)); }

    /// \brief Inserts elements from range [first, last) before a specific position
    ///
    /// \param where Position in the vector before which the new elements are inserted.
    /// \param first The start of the range of elements to insert
    /// \param last The end of the range of elements to insert
    ///
    /// \pre std::cbegin(*this) <= where <= std::cend(*this)
    /// \pre the distance between the input iterators shall be in the range of [0, MaxSize - current_size]
    ///
    /// \return An iterator pointing to the first element inserted, or where if first==last.
    template <typename InputIterator>
    iterator insert(const const_iterator where, InputIterator first, InputIterator last)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(std::cbegin(*this) <= where);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(where <= std::cend(*this));
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(std::distance(first, last) >= 0);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION((static_cast<std::size_t>(std::distance(first, last)) + size()) <= MaxSize);

        const auto old_end = end();
        const auto new_end = std::uninitialized_copy(first, last, std::end(*this));
        const difference_type num_new_elements{new_end - old_end};
        base_t::set_size(size() + static_cast<std::size_t>(num_new_elements));
        const iterator it{const_iterator_cast(where)};
        score::cpp::ignore = std::rotate(it, old_end, new_end);

        return it;
    }

    /// \brief Removes the element at where.
    ///
    /// \param where Iterator pointing to a single element to be removed from the vector.
    ///
    /// \pre std::cbegin(*this) <= where < std::cend(*this)
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
    /// \param first Iterator pointing to the first element to be removed from the vector.
    /// \param last Iterator pointing to the last element to be removed from the vector.
    ///
    /// \pre first <= last
    /// \pre std::cbegin(*this) <= first
    /// \pre last <= std::cend(*this)
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
    /// \param value Value to be copied to the new element.
    ///
    /// \pre size() < MaxSize
    void push_back(const_reference value) { score::cpp::ignore = emplace_back(value); }

    /// \brief Mimics std::vector<>::push_back() for rvalues
    ///
    /// \param value Value to be moved to the new element.
    ///
    /// \pre size() < MaxSize
    void push_back(value_type&& value) { score::cpp::ignore = emplace_back(std::move(value)); }

    /// \brief Mimics std::vector<>::emplace_back()
    ///
    /// \param arguments Arguments forwarded to the constructor of T.
    ///
    /// \pre size() < MaxSize
    ///
    /// \return Reference to the inserted element.
    template <typename... Ts>
    reference emplace_back(Ts&&... arguments)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(size() < MaxSize);
        T* ptr{::new (data() + size()) T{std::forward<Ts>(arguments)...}};
        base_t::set_size(size() + 1U);
        SCORE_LANGUAGE_FUTURECPP_ASSERT(size() <= MaxSize);
        return *ptr;
    }

    /// \brief Mimics std::vector<>::pop_back()
    ///
    /// \pre !empty()
    void pop_back()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        back().~T();
        base_t::set_size(size() - 1U);
        SCORE_LANGUAGE_FUTURECPP_ASSERT(size() < MaxSize);
    }

    /// \brief Mimics std::vector<>::back()
    ///
    /// \pre !empty()
    ///
    /// \return Reference to the last element.
    /// \{
    reference back()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return (*this)[size() - 1U];
    }
    const_reference back() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return (*this)[size() - 1U];
    }
    /// \}

    /// \brief Mimics std::vector<>::front()
    ///
    /// \pre !empty()
    ///
    /// \return Reference to the first element.
    /// \{
    reference front()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return (*this)[0U];
    }
    const_reference front() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return (*this)[0U];
    }
    /// \}

private:
    /// \brief Construct from "single-pass" iterators
    template <typename InputIterator>
    inplace_vector(InputIterator begin, const InputIterator end, const std::input_iterator_tag)
    {
        try
        {
            for (; begin != end; ++begin)
            {
                emplace_back(*begin);
            }
        }
        catch (...)
        {
            // roll back all already copied elements
            clear();
        }
    }

    /// \brief Construct from "multi-pass" iterators
    template <typename InputIterator>
    inplace_vector(const InputIterator begin, const InputIterator end, const std::forward_iterator_tag)
    {
        const auto length = std::distance(begin, end);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(length >= 0);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(static_cast<std::size_t>(length) <= MaxSize);
        try
        {
            const auto new_end = std::uninitialized_copy(begin, end, std::begin(*this));
            base_t::set_size(static_cast<std::size_t>(new_end - std::begin(*this)));
        }
        catch (...)
        {
            // do nothing because `uninitialized_copy` destroys all already copied elements
        }
    }

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
        score::cpp::ignore = emplace_back(std::forward<U>(value));
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
        base_t::set_size(new_size);
    }

    /// \brief Appends `n` default-inserted `T`s.
    void append(const size_type n)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((size() + n) <= MaxSize);
        score::cpp::ignore = std::uninitialized_value_construct_n(data() + size(), n);
        base_t::set_size(size() + n);
    }

    /// \brief Appends `n` copies of `value`.
    void append(const size_type n, const_reference value)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((size() + n) <= MaxSize);
        score::cpp::ignore = std::uninitialized_fill_n(data() + size(), n, value);
        base_t::set_size(size() + n);
    }
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
