///
/// \file
/// \copyright Copyright (C) 2016-2018, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.CircularBuffer component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_CIRCULAR_BUFFER_HPP
#define SCORE_LANGUAGE_FUTURECPP_CIRCULAR_BUFFER_HPP

#include <score/private/container/circular_buffer_iterator.hpp> // IWYU pragma: export
#include <score/private/memory/uninitialized_move.hpp>
#include <score/private/utility/ignore.hpp>
#include <score/assert.hpp>
#include <score/utility.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

namespace score::cpp
{

/// \brief Circular buffer can for example be used to generate a history of a certain number of elements and
/// automatically discard the oldest elements.
///
/// \tparam T Specifies the value type of a single element of a row.
/// \tparam max_size Specifies maximum size of the internal data storage.
template <typename T, std::size_t MaxSize>
class circular_buffer
{
public:
    /// \brief The type of the array elements. This is a requirement of STL compliant types.
    using value_type = T;

    /// \brief The type to hold a size information. This is a requirement of STL compliant types.
    using size_type = std::size_t;

    /// \brief The type that can be used to identify distance between iterators
    using difference_type = std::ptrdiff_t;

    /// \brief The type of a non-const iterator. This is a requirement of STL compliant types.
    using iterator = detail::circular_buffer_iterator<circular_buffer<T, MaxSize>>;

    /// \brief The type of a const iterator. This is a requirement of STL compliant types.
    using const_iterator = detail::circular_buffer_iterator<circular_buffer<T, MaxSize>, true>;

    /// \brief The type of a non-const reverse iterator. This is a requirement of STL compliant types.
    using reverse_iterator = std::reverse_iterator<iterator>;

    /// \brief The type of a const reverse iterator. This is a requirement of STL compliant types.
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /// \brief Reference of the type in the buffer. This is a requirement of STL compliant types.
    using reference = T&;

    /// \brief Const reference of the type in the buffer. This is a requirement of STL compliant types.
    using const_reference = const T&;

    /// \brief Pointer of the type in the buffer. This is a requirement of STL compliant types.
    using pointer = T*;

    /// \brief Const pointer of the type in the buffer. This is a requirement of STL compliant types.
    using const_pointer = const T*;

    /// \brief Type of the container. This is a requirement of STL compliant types.
    using container = circular_buffer<T, MaxSize>;

    /// \brief Default constructor. Constructs an empty container.
    circular_buffer() noexcept : tail_{0U}, head_{0U}, size_{0U} {};

    /// \brief Copy construction from circular_buffer.
    circular_buffer(const circular_buffer& other) noexcept(std::is_nothrow_copy_constructible<T>::value)
        : tail_{other.tail_}, head_{other.head_}, size_{other.size_}
    {
        try
        {
            score::cpp::ignore = std::uninitialized_copy(std::cbegin(other), std::cend(other), std::begin(*this));
        }
        catch (...)
        {
            tail_ = 0U;
            head_ = 0U;
            size_ = 0U;
        }
    }

    /// \brief Move construction from circular_buffer.
    circular_buffer(circular_buffer&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
        : tail_{other.tail_}, head_{other.head_}, size_{other.size_}
    {
        try
        {
            score::cpp::ignore = score::cpp::uninitialized_move(std::begin(other), std::end(other), std::begin(*this));
        }
        catch (...)
        {
            tail_ = 0U;
            head_ = 0U;
            size_ = 0U;
        }
    }

    /// \brief Copy assignment from circular_buffer, implemented as copy-construct -> move assign
    circular_buffer& operator=(const circular_buffer& rhs)
    {
        if (this != &rhs)
        {
            try
            {
                clear();
                head_ = rhs.head_;
                tail_ = rhs.tail_;
                size_ = rhs.size_;
                score::cpp::ignore = std::uninitialized_copy(std::begin(rhs), std::end(rhs), std::begin(*this));
            }
            catch (...)
            {
                head_ = 0;
                tail_ = 0;
                size_ = 0;
            }
        }
        return *this;
    }

    /// \brief Move assignment from circular_buffer. Basic exception guarantee
    circular_buffer& operator=(circular_buffer&& rhs)
    {
        if (this != &rhs)
        {
            try
            {
                clear();
                head_ = rhs.head_;
                tail_ = rhs.tail_;
                size_ = rhs.size_;
                score::cpp::ignore = score::cpp::uninitialized_move(std::begin(rhs), std::end(rhs), std::begin(*this));
            }
            catch (...)
            {
                head_ = 0;
                tail_ = 0;
                size_ = 0;
            }
        }
        return *this;
    }

    /// \brief Clears all elements from the container.
    ~circular_buffer() { clear(); }

    /// \brief Removes all elements from the container.
    void clear()
    {
        const size_type number_of_elements_to_remove{size()};
        for (size_type i{0U}; i < number_of_elements_to_remove; ++i)
        {
            pop_front();
        }
    }

    /// \brief Checks if container contains elements.
    ///
    /// \return True, if the container does not contain elements - false otherwise.
    bool empty() const { return 0U == size(); }

    /// \brief Returns true if container is full.
    ///
    /// \return True if container is full - otherwise false.
    bool full() const { return capacity() == size(); }

    /// \brief Returns the number elements in the container.
    ///
    /// \return The number of elements in the container.
    size_type size() const { return size_; }

    /// \brief Returns an iterator to the beginning
    ///
    /// \return Iterator to the first element in the row.
    /// \{
    iterator begin() { return iterator{*this, true}; }
    const_iterator begin() const { return const_iterator{*this, true}; }
    const_iterator cbegin() const { return std::cbegin(*this); }
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

    /// \brief Returns an iterator to the reverse-beginning of the container
    ///
    /// \return Iterator to the last element of the vector.
    /// \{
    reverse_iterator rbegin() { return reverse_iterator{end()}; }
    const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
    const_reverse_iterator crbegin() const { return rbegin(); }
    /// \}

    /// \brief Returns an iterator to the reverse-end of the container
    ///
    /// Attempting to access it, results in undefined behavior.
    ///
    /// \return Iterator one before the first element of the container.
    /// \{
    reverse_iterator rend() { return reverse_iterator{begin()}; }
    const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }
    const_reverse_iterator crend() const { return rend(); }
    /// \}

    /// \brief Adds the element to the end of the container.
    ///
    /// \param value The element that is added to the container.
    /// \{
    void push_back(const_reference value) { emplace_back(value); }
    void push_back(value_type&& value) { emplace_back(std::move(value)); }
    /// \}

    /// \brief constructs a new element in place at the end of the buffer
    ///
    /// It will remove the element at front() if the buffer is full
    ///
    /// \param arguments The arguments from which the element is constructed
    template <typename... Ts>
    void emplace_back(Ts&&... arguments)
    {
        if (full())
        {
            pop_front();
        }
        score::cpp::ignore = ::new (base() + tail_) T{std::forward<Ts>(arguments)...};
        ++tail_;
        tail_ %= capacity();
        ++size_;
    }

    /// \brief Removes the first element in the container.
    void pop_front()
    {
        if (!empty())
        {
            front().~T();
            ++head_;
            head_ %= capacity();
            --size_;
        }
    }

    /// \brief Removes the last element in the container.
    void pop_back()
    {
        if (!empty())
        {
            back().~T();

            if (tail_ == 0U)
            {
                tail_ = capacity() - 1U;
            }
            else
            {
                --tail_;
            }
            --size_;
        }
    }

    /// \brief Removes the elements at pos
    ///
    /// \param pos A valid iterator in the range [begin(), end())
    /// \returns An iterator pointing at the element that was immediately following pos prior to erasure, or end() if no
    /// such element exists.
    iterator erase(iterator pos) { return erase(pos, pos + 1); }

    /// \brief Removes the elements in the valid range [first, last)
    ///
    /// \param first A valid iterator in the range [begin(), end()]
    /// \param last A valid iterator in the range [begin(), end()]
    /// \returns An iterator pointing at the element that was pointed by last prior to any erasure, or end() if no such
    /// element exists.
    iterator erase(iterator first, iterator last)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(first.base() == this);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(last.base() == this);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(begin() <= first);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(last <= end());
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(first <= last);

        const difference_type diff{last - first};

        if ((first - begin()) <= (end() - last))
        {
            score::cpp::ignore = std::move_backward(begin(), first, last);
            for (difference_type i{0}; i < diff; ++i)
            {
                pop_front();
            }
        }
        else
        {
            score::cpp::ignore = std::move(last, end(), first);
            for (difference_type i{0}; i < diff; ++i)
            {
                pop_back();
            }
        }

        return last - diff;
    }

    /// \brief Returns the last element in the container.
    ///
    /// \return Reference to the last element in the container.
    const_reference back() const
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(!empty());
        return *(std::cend(*this) - 1);
    }

    /// \brief Returns the first element in the container.
    ///
    /// \return Reference to the first element in the container.
    const_reference front() const
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(!empty());
        return *std::cbegin(*this);
    }

    /// \brief Random access to an element in the container.
    ///
    /// It also performs a bounds check and considers the wrapped memory layout of the circular buffer.
    ///
    /// \param n Index in the container.
    /// \return Reference to the n'th element in the container.
    /// \{
    reference operator[](const size_type n)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(n < size());
        const size_type wrapped_index{(head_ + n) % capacity()};
        return *(base() + wrapped_index);
    }
    const_reference operator[](const size_type n) const
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(n < size());
        const size_type wrapped_index{(head_ + n) % capacity()};
        return *(base() + wrapped_index);
    }
    /// \}

    /// \brief Returns the maximum size of the circular buffer.
    ///
    /// \return Maximum size of the circular buffer.
    static constexpr size_type capacity() { return MaxSize; }

    /// \brief Returns the maximum size of the circular buffer.
    ///
    /// \return Maximum size of the circular buffer.
    static constexpr size_type max_size() { return MaxSize; }

private:
    /// \brief Returns a pointer to the begin of the internal array.
    /// \{
    T* base() { return reinterpret_cast<T*>(array_); }
    const T* base() const { return reinterpret_cast<const T*>(array_); }
    /// \}

    /// \brief Internal data array
    typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type array_[MaxSize];

    /// \brief Index of the last valid data in the circular buffer.
    size_type tail_;

    /// \brief Index of the first valid data in the circular buffer.
    size_type head_;

    /// \brief Amount of valid elements in the circular buffer.
    size_type size_;
};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_CIRCULAR_BUFFER_HPP
