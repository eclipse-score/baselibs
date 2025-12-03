///
/// \file
/// \copyright Copyright (C) 2015-2020, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#ifndef SCORE_LANGUAGE_FUTURECPP_SPAN_HPP
#define SCORE_LANGUAGE_FUTURECPP_SPAN_HPP

#include <score/private/iterator/at.hpp>       // IWYU pragma: export
#include <score/private/iterator/data.hpp>     // IWYU pragma: export
#include <score/private/iterator/iterator.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp>     // IWYU pragma: export
#include <score/private/type_traits/is_span.hpp>
#include <score/private/type_traits/is_std_array.hpp>
#include <score/private/type_traits/type_identity.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <score/assert.hpp>
#include <score/optional.hpp>
#include <score/size.hpp>
#include <score/type_traits.hpp>

namespace score::cpp
{

namespace detail
{

template <typename T, std::size_t Extent>
class base_span
{
public:
    constexpr base_span(T* const data, const std::size_t size) : data_{data} { SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(Extent == size); }
    constexpr std::size_t size() const noexcept { return Extent; }
    constexpr T* data() const noexcept { return data_; }

private:
    T* data_;
};

template <typename T>
class base_span<T, dynamic_extent>
{
public:
    constexpr base_span(T* const data, const std::size_t size) noexcept : data_{data}, size_{size} {}
    constexpr std::size_t size() const noexcept { return size_; }
    constexpr T* data() const noexcept { return data_; }

private:
    T* data_;
    std::size_t size_;
};

} // namespace detail

/// \brief Provides a 1d-view of a pointer + size. The benefit is that a one-dimensional span does not decay to T*
/// (thus looses the size information).
///
/// If used in combination with a two-dimensional span it represents a row.
///
/// It behaves the same as std::array<T, size_>. Thus you should directly use the container from the C++ standard
/// library. But it is still better than a plain C-Array. If you still want to use it than have a look at the
/// documentation for a two-dimensional span as both similar except the 1d vs 2d view.
///
/// \tparam T Specifies the value type of a single element of a row.
template <typename T, std::size_t Extent>
class span
{
    /// @brief Used to obtain the iterator type of the type Range.
    template <typename Range>
    using iterator_t = decltype(std::begin(std::declval<Range&>()));

    template <typename Iterator, typename R = decltype(*std::declval<Iterator&>()), typename = R&>
    using iter_reference_helper = R;

    /// @brief Computes the reference type of Iterator.
    template <typename Iterator>
    using iter_reference_t = iter_reference_helper<Iterator>;

public:
    /// \brief The (cv-qualified) type of the array elements.
    using element_type = T;

    /// \brief The (non cv-qualified) type of the array elements.
    using value_type = std::remove_cv_t<T>;

    /// \brief The type to hold a size information
    using size_type = std::size_t;

    /// @brief Unit of increment between two valid pointers into the span of T
    using difference_type = std::ptrdiff_t;

    /// @brief Type of pointer to element
    using pointer = element_type*;

    /// @brief Type of pointer to const to the element
    using const_pointer = const element_type*;

    /// @brief Type of reference to the element
    using reference = element_type&;

    /// @brief Type of reference to const to the element
    using const_reference = const element_type&;

    /// \brief The type of a non-const iterator
    using iterator = score::cpp::detail::iterator<span<T>, false>;

    /// \brief The type of a const iterator
    using const_iterator = score::cpp::detail::iterator<span<T>, true>;

    /// \brief The type of a non-const reverse iterator. This is a requirement of STL compliant types.
    using reverse_iterator = std::reverse_iterator<iterator>;

    /// \brief The type of a const reverse iterator. This is a requirement of STL compliant types.
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /// \brief The number of elements in the sequence, or score::cpp::dynamic_extent if dynamic
    static constexpr size_type extent{Extent};

    /// \brief Constructs an empty span whose `data() == nullptr` and `size() == 0`.
    template <std::size_t Size = 0U, typename = std::enable_if_t<(Extent == dynamic_extent) || (Extent == Size)>>
    span() noexcept : base_{nullptr, 0U}
    {
    }

    /// \brief Provides a 1d-view of a pointer + size. The benefit is that a one-dimensional span does not decay to
    /// T* (thus looses the size information).
    ///
    /// \param array Pointer to the array
    /// \param size Number of elements in the one-dimensional span
    /// \{
    template <std::size_t Size = Extent, typename std::enable_if_t<Size != dynamic_extent, bool> = true>
    explicit span(T* const array, const size_type size) : base_{array, size}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((Extent == dynamic_extent) || (Extent == size));
    }
    template <std::size_t Size = Extent, typename std::enable_if_t<Size == dynamic_extent, bool> = true>
    span(T* const array, const size_type size) : base_{array, size}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((Extent == dynamic_extent) || (Extent == size));
    }
    /// \}

    /// \brief Provides a 1d-view of a C-array. The benefit is that a one-dimensional span does not decay to T*
    /// (thus looses the size information).
    ///
    /// \tparam Size Number of elements in the one-dimensional span
    /// \param array Pointer to the array
    template <std::size_t N, typename = std::enable_if_t<(Extent == dynamic_extent) || (Extent == N)>>
    span(score::cpp::type_identity_t<T> (&array)[N]) noexcept : base_{std::data(array), N}
    {
    }

    /// \brief Constructs a span that is a view over the supplied array
    ///
    /// The resulting span has size() == N and data() == std::data(arr).
    ///
    /// This overload participates in overload resolution only if elements of `arr` are convertible to this span's
    /// element_type.
    ///
    /// \tparam U Type of std::array::value_type
    /// \tparam N size of std::array.
    /// \param arr std::array to construct a view for
    /// \{
    template <typename U,
              std::size_t N,
              typename = std::enable_if_t<((Extent == dynamic_extent) || (Extent == N)) &&
                                          std::is_convertible_v<U (*)[], element_type (*)[]>>>
    span(std::array<U, N>& arr) noexcept : base_{std::data(arr), N}
    {
    }
    template <typename U,
              std::size_t N,
              typename = std::enable_if_t<((Extent == dynamic_extent) || (Extent == N)) &&
                                          std::is_convertible_v<const U (*)[], element_type (*)[]>>>
    span(const std::array<U, N>& arr) noexcept : base_{std::data(arr), N}
    {
    }
    /// \}

    /// \brief Constructs a span that is a view over \p range; the resulting span has size() == std::size(range) and
    /// data() == std::data(range).
    ///
    /// This overload participates in overload resolution only if Range is iterable, not a span or an array and elements
    /// of the range are convertible to this span's element_type.
    ///
    /// \tparam Range Type of \p range
    /// \param range Range to construct a view for
    /// \{
    template <typename Range,
              std::size_t Size = Extent,
              typename R = score::cpp::remove_cvref_t<Range>,
              typename U = std::remove_reference_t<iter_reference_t<iterator_t<Range>>>,
              typename std::enable_if_t<Size != dynamic_extent                              //
                                            && score::cpp::is_iterable<R>::value                   //
                                            && std::is_convertible<U (*)[], T (*)[]>::value //
                                            && (!is_span<R>::value)                         //
                                            && (!detail::is_std_array<R>::value)            //
                                            && (!std::is_array<R>::value)                   //
                                        ,
                                        bool> = true>
    explicit span(Range&& range) : base_{std::data(range), std::size(range)}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((Extent == dynamic_extent) || (Extent == std::size(range)));
    }
    template <typename Range,
              std::size_t Size = Extent,
              typename R = score::cpp::remove_cvref_t<Range>,
              typename U = std::remove_reference_t<iter_reference_t<iterator_t<Range>>>,
              typename std::enable_if_t<Size == dynamic_extent                              //
                                            && score::cpp::is_iterable<R>::value                   //
                                            && std::is_convertible<U (*)[], T (*)[]>::value //
                                            && (!is_span<R>::value)                         //
                                            && (!detail::is_std_array<R>::value)            //
                                            && (!std::is_array<R>::value)                   //
                                        ,
                                        bool> = true>
    span(Range&& range) : base_{std::data(range), std::size(range)}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((Extent == dynamic_extent) || (Extent == std::size(range)));
    }
    /// \}

    /// \brief Converting constructor from another span `other`
    ///
    /// The resulting span has `size() == other.size()` and `data() == other.data()`.
    ///
    /// This overload participates in overload resolution only if elements of the other span are convertible to this
    /// span's element type and the following size requirements are fulfilled
    /// `(Extent == dynamic_extent) || (N == dynamic_extent) || (Extent == N)`
    ///
    /// \pre (Extent == dynamic_extent) || (Extent == other.size())
    ///
    /// \tparam U element type of the other span
    /// \param other other span
    /// \{
    template <typename U,
              std::size_t N,
              typename std::enable_if_t<(Extent != dynamic_extent) && (N == dynamic_extent)                           //
                                            && ((Extent == dynamic_extent) || (N == dynamic_extent) || (Extent == N)) //
                                            && std::is_convertible_v<U (*)[], T (*)[]>                                //
                                        ,
                                        bool> = true>
    explicit span(const span<U, N>& other) : base_{other.data(), other.size()}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((Extent == dynamic_extent) || (Extent == other.size()));
    }
    template <typename U,
              std::size_t N,
              typename std::enable_if_t<!((Extent != dynamic_extent) && (N == dynamic_extent))                        //
                                            && ((Extent == dynamic_extent) || (N == dynamic_extent) || (Extent == N)) //
                                            && std::is_convertible_v<U (*)[], T (*)[]>                                //
                                        ,
                                        bool> = true>
    span(const span<U, N>& other) : base_{other.data(), other.size()}
    {
        // keep the precondition although it can never fail. it is essentially a side effect of our pre-C++20
        // explicit/non-explicit split while keeping the conditions as defined by the standard.
        // this ctor covers the following cases:
        //   1. "dynamic -> dynamic" => precondition will never fail because of first term
        //   2. "static -> static" => enable_if will fail because of `(Extent == N)`
        //   3. "static -> dynamic" => precondition will never fail because of first term
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((Extent == dynamic_extent) || (Extent == other.size()));
    }
    /// \}

    /// \brief Returns the number elements in the row - when combined with a two-dimensional span the number of
    /// columns.
    ///
    /// \return  Number of elements in the row.
    size_type size() const noexcept { return base_.size(); }

    /// \brief checks if the span is empty
    ///
    /// @return true if span is empty (size==0); false otherwise.
    bool empty() const noexcept { return size() == 0; }

    /// \brief Returns a pointer to the underlying array.
    ///
    /// \return Pointer to the first element of the row.
    pointer data() const noexcept { return base_.data(); }

    ///\{
    /// \brief Returns an iterator to the beginning
    ///
    /// \return Iterator to the first element in the row.
    iterator begin() const noexcept { return iterator{*this, true}; }
    const_iterator cbegin() const noexcept { return const_iterator{*this, true}; }
    ///\}

    ///\{
    /// \brief Returns an iterator one past the end
    ///
    /// Attempting to access it results in undefined behavior.
    ///
    /// \return Iterator one past the last element of the row.
    iterator end() const noexcept { return iterator{*this, false}; }
    const_iterator cend() const noexcept { return const_iterator{*this, false}; }
    ///\}

    /// \{
    /// \brief Returns an iterator to the reverse-beginning of the container
    ///
    /// \return Iterator to the last element of the span.
    reverse_iterator rbegin() const noexcept { return reverse_iterator{end()}; }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{cend()}; }
    /// \}

    /// \{
    /// \brief Returns an iterator to the reverse-end of the container
    ///
    /// Attempting to access it results in undefined behavior.
    ///
    /// \return Iterator one before the first element of the container.
    reverse_iterator rend() const noexcept { return reverse_iterator{begin()}; }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator{cbegin()}; }
    /// \}

    /// \brief Returns a reference to the `i`th element
    ///
    /// \pre i < size()
    ///
    /// \param i the index of the element to access
    /// \return ith element
    constexpr reference operator[](const size_type i) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(i < size());
        return *(data() + i);
    }

    /// \brief Returns a reference to the first element in the span
    ///
    /// \pre !empty()
    ///
    /// \return A reference to the first element.
    reference front() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(!empty());
        return *begin();
    }

    /// \brief Returns a reference to the last element in the span
    ///
    /// \pre !empty()
    ///
    /// \return A reference to the last element.
    reference back() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(!empty());
        return *(end() - 1);
    }

    /// \brief Obtains a span that is a view over the `count` elements of this span starting at offset `offset`. If
    /// `count` is dynamic_extent, the subspan contains `size() - offset` elements (i.e., all remaining elements).
    ///
    /// \param offset skipped elements at the front
    /// \param count number of elements in the resulting subspan, if `count` is  dynamic_extent, the resulting subspan
    /// contains all remaining elements.
    /// \pre `0 <= offset <= size()`
    /// \pre `count <= size() - offset`
    /// \return The requested subspan `r`, such that `r.data() == this->data() + offset`. If `count` is dynamic_extent,
    /// `r.size() == this->size() - offset`; otherwise `r.size() == count`.
    span subspan(const size_type offset, const size_type count = dynamic_extent) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(offset <= size());
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((count == dynamic_extent) || (count <= (size() - offset)));
        return span{data() + offset, (count != dynamic_extent) ? count : (size() - offset)};
    }

    /// \brief Obtains a span that is a view over the first `count` elements of this span.
    ///
    /// \param count number of elements in the resulting subspan
    /// \pre `0 <= count <= size()`
    /// \return A span `r` that is a view over the first `count` elements of `*this`, such that
    /// `r.data() == this->data() && r.size() == count`.
    span first(const size_type count) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(count <= size());
        return span{data(), count};
    }

    /// \brief Obtains a span that is a view over the last `count` elements of this span.
    ///
    /// \param count number of elements in the resulting subspan
    /// \pre `0 <= count <= size()`
    /// \return A span `r` that is a view over the last `count` elements of `*this`, such that
    /// `r.data() == this->data() + (this->size() - count) && r.size() == count`.
    span last(const size_type count) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(count <= size());
        return span{data() + (size() - count), count};
    }

private:
    detail::base_span<T, Extent> base_;
};

/// \brief Obtains a view to the object representation of the elements of the span<>.
template <typename T>
span<const std::uint8_t> as_bytes(const span<T> view) noexcept
{
    const std::size_t size_bytes{view.size() * sizeof(T)};
    return {reinterpret_cast<const std::uint8_t*>(view.data()), size_bytes};
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_SPAN_HPP
