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
/// \brief Score.Futurecpp.MultiSpan component
///
/// See \ref score::cpp::multi_span for more details.
///

#ifndef SCORE_LANGUAGE_FUTURECPP_MULTI_SPAN_HPP
#define SCORE_LANGUAGE_FUTURECPP_MULTI_SPAN_HPP

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>

#include <score/assert.hpp>

namespace score::cpp
{

template <std::size_t Rank>
class offset;
template <std::size_t Rank>
class bounds;

/// \cond detail
namespace detail
{

constexpr bool all_positive() { return true; }

template <typename T1, typename... T>
constexpr bool all_positive(const T1 v, const T... ts)
{
    return (v > 0) && all_positive(ts...);
}

template <std::size_t Rank, typename T = std::ptrdiff_t>
class coordinate_facade
{
    static_assert(Rank > 0, "Rank must be greater than 0!");

public:
    using size_type = std::size_t;
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using storage_type = std::array<value_type, Rank>;

    ///
    /// \brief Access operator (const version)
    ///
    /// \param pos index of the array
    /// \return A reference to the \f$i^{th}\f$ element of *this
    ///
    constexpr const_reference operator[](size_type pos) const { return data_[pos]; }

protected:
    constexpr coordinate_facade() : data_() {}
    constexpr coordinate_facade(const storage_type& data) : data_(data) {}
    template <typename... Types>
    constexpr coordinate_facade(value_type v, Types... ts) : data_{v, ts...}
    {
    }

    constexpr reference operator[](size_type pos) { return data_[pos]; }

private:
    storage_type data_;
};

template <std::size_t Rank, typename T>
constexpr bool operator==(const detail::coordinate_facade<Rank, T>& lhs, const detail::coordinate_facade<Rank, T>& rhs)
{
    bool ret = true;
    for (std::size_t i = 0; i < Rank; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            ret = false;
        }
    }
    return ret;
}

template <std::size_t Rank, typename T>
constexpr bool operator!=(const detail::coordinate_facade<Rank, T>& lhs, const detail::coordinate_facade<Rank, T>& rhs)
{
    return !operator==(lhs, rhs);
}

///
/// \brief bounds iterator
///
/// \note This iterator does not follow completely the random access
///       requirements. Therefore the reference is actually a value type
///       instead of a reference type. The reason behind is that it can be used
///       with reverse_iterator.
///
template <typename T, std::size_t Rank>
class bounds_iterator
{
public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;
    using pointer = T*;
    using reference = T;

    constexpr bounds_iterator() : bnd_(), idx_() {}

    constexpr explicit bounds_iterator(const bounds<Rank>& bnd, const offset<Rank>& idx = offset<Rank>())
        : bnd_(bnd), idx_(idx)
    {
    }

    constexpr reference operator*() { return idx_; }

    constexpr pointer operator->() { return &idx_; }

    constexpr bounds_iterator& operator++()
    {
        for (std::size_t i = Rank; i-- > 0;)
        {
            if (++idx_[i] < static_cast<typename offset<Rank>::value_type>(bnd_[i]))
            {
                return *this;
            }
            else
            {
                idx_[i] = 0;
            }
        }
        for (std::size_t i = 0; i < (Rank - 1); ++i)
        {
            idx_[i] = static_cast<typename offset<Rank>::value_type>(bnd_[i]) - 1;
        }
        idx_[Rank - 1] = static_cast<typename offset<Rank>::value_type>(bnd_[Rank - 1]);
        return *this;
    }

    constexpr bounds_iterator operator++(int)
    {
        bounds_iterator ret(*this);
        ++(*this);
        return ret;
    }

    constexpr bounds_iterator& operator--()
    {
        for (std::size_t i = Rank; i-- > 0;)
        {
            if (--idx_[i] >= 0)
            {
                return *this;
            }
            else
            {
                idx_[i] = static_cast<typename offset<Rank>::value_type>(bnd_[i]) - 1;
            }
        }
        for (std::size_t i = 0; i < (Rank - 1); ++i)
        {
            idx_[i] = 0;
        }
        idx_[Rank - 1] = -1;
        return *this;
    }

    constexpr bounds_iterator operator--(int)
    {
        bounds_iterator ret(*this);
        --(*this);
        return ret;
    }

    constexpr bounds_iterator& operator+=(difference_type n)
    {
        const offset<Rank> stride = make_stride(bnd_);
        difference_type lin_idx = linearize(idx_) + n;

        if (lin_idx >= 0)
        {
            if (lin_idx < static_cast<difference_type>(bnd_.size()))
            {
                for (std::size_t i = 0; i < Rank; ++i)
                {
                    idx_[i] = lin_idx / stride[i];
                    lin_idx = lin_idx % stride[i];
                }
            }
            else
            {
                for (std::size_t i = 0; i < (Rank - 1); ++i)
                {
                    idx_[i] = static_cast<typename offset<Rank>::value_type>(bnd_[i]) - 1;
                }
                idx_[Rank - 1] = static_cast<typename offset<Rank>::value_type>(bnd_[Rank - 1]);
            }
        }
        else
        {
            for (std::size_t i = 0; i < (Rank - 1); ++i)
            {
                idx_[i] = 0;
            }
            idx_[Rank - 1] = -1;
        }
        return *this;
    }

    constexpr bounds_iterator& operator-=(difference_type n) { return *this += -n; }

    constexpr value_type operator[](difference_type n) const { return *(*this + n); }

    constexpr void swap(bounds_iterator& rhs)
    {
        std::swap(bnd_, rhs.bnd_);
        std::swap(idx_, rhs.idx_);
    }

private:
    friend constexpr typename bounds_iterator<T, Rank>::difference_type operator-(const bounds_iterator<T, Rank>& lhs,
                                                                                  const bounds_iterator<T, Rank>& rhs)
    {
        return lhs.linearize(lhs.idx_) - rhs.linearize(rhs.idx_);
    }
    friend constexpr bool operator==(const bounds_iterator<T, Rank>& lhs, const bounds_iterator<T, Rank>& rhs)
    {
        return lhs.idx_ == rhs.idx_;
    }
    friend constexpr bool operator<(const bounds_iterator<T, Rank>& lhs, const bounds_iterator<T, Rank>& rhs)
    {
        for (std::size_t i = 0; i < Rank; ++i)
        {
            if (lhs.idx_[i] < rhs.idx_[i])
            {
                return true;
            }
        }
        return false;
    }

    constexpr difference_type linearize(const offset<Rank>& idx) const
    {
        difference_type res = 0;
        const offset<Rank> stride = make_stride(bnd_);

        for (std::size_t i = 0; i < Rank; ++i)
        {
            res += idx[i] * stride[i];
        }
        return res;
    }

    bounds<Rank> bnd_;
    offset<Rank> idx_;
};

template <typename T, std::size_t Rank>
constexpr bounds_iterator<T, Rank> operator+(const bounds_iterator<T, Rank>& lhs,
                                             typename bounds_iterator<T, Rank>::difference_type rhs)
{
    bounds_iterator<T, Rank> ret(lhs);
    return ret += rhs;
}

template <typename T, std::size_t Rank>
constexpr bounds_iterator<T, Rank> operator-(const bounds_iterator<T, Rank>& lhs,
                                             typename bounds_iterator<T, Rank>::difference_type rhs)
{
    bounds_iterator<T, Rank> ret(lhs);
    return ret -= rhs;
}

template <typename T, std::size_t Rank>
constexpr bool operator!=(const bounds_iterator<T, Rank>& lhs, const bounds_iterator<T, Rank>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, std::size_t Rank>
constexpr bool operator<=(const bounds_iterator<T, Rank>& lhs, const bounds_iterator<T, Rank>& rhs)
{
    return !(rhs < lhs);
}

template <typename T, std::size_t Rank>
constexpr bool operator>(const bounds_iterator<T, Rank>& lhs, const bounds_iterator<T, Rank>& rhs)
{
    return rhs < lhs;
}

template <typename T, std::size_t Rank>
constexpr bool operator>=(const bounds_iterator<T, Rank>& lhs, const bounds_iterator<T, Rank>& rhs)
{
    return !(rhs > lhs);
}

} // namespace detail
/// \endcond

///
/// \brief Offset used with all \ref multi_span or strided_multi_span
///
/// Use \ref make_offset to create an offset with a specific rank.
///
/// \tparam Rank the rank of the coordinate_facade
///
template <std::size_t Rank>
class offset : public detail::coordinate_facade<Rank>
{
    using base_type = detail::coordinate_facade<Rank>;

public:
    using typename base_type::const_reference; ///< const reference type
    using typename base_type::reference;       ///< reference type
    using typename base_type::size_type;       ///< size type
    using typename base_type::storage_type;    ///< storage type
    using typename base_type::value_type;      ///< value type

    /// rank of the \ref offset
    static constexpr size_type rank = Rank;

    /// Default constructor; zero-initializes each element
    constexpr offset() : base_type() {}
    ///
    /// \brief For all \f$i\in\left[0,Rank\right)\f$ initializes the \f$i^{th}\f$
    ///        element of \c *this with \c <tt>*(il.begin() + i)</tt>.
    ///
    /// \param v first parameter
    /// \param ts remaining parameters
    ///
    template <typename... T>
    constexpr offset(value_type v, T... ts) : base_type(v, ts...)
    {
    }

    ///
    /// \brief \ref offset based on storage data
    ///
    /// \param data data to construct an \ref offset
    ///
    constexpr offset(const storage_type& data) : base_type(data) {}

    using base_type::operator[];
    ///
    /// \brief Access operator (non-const version)
    ///
    /// \param pos index of the array
    /// \return A reference to the \f$i^{th}\f$ element of *this
    ///
    constexpr reference operator[](size_type pos) { return base_type::operator[](pos); }
    ///
    /// \brief Negation operator
    ///
    /// \return A copy of \c *this with each element negated.
    ///
    constexpr offset operator-() const
    {
        offset ret(*this);
        for (size_type i = 0; i < rank; ++i)
        {
            ret[i] = -(*this)[i];
        }
        return ret;
    }
    ///
    /// \brief For all \f$i\in\left[0,Rank\right)\f$ adds the \f$i^{th}\f$
    ///        element of \c rhs to the \f$i^{th}\f$ element of \c *this and
    ///        stores the sum in the \f$i^{th}\f$ element of \c *this.
    ///
    /// \param rhs right-hand side \ref offset
    /// \return \c *this
    ///
    constexpr offset& operator+=(const offset& rhs)
    {
        for (size_type i = 0; i < rank; ++i)
        {
            (*this)[i] += rhs[i];
        }
        return *this;
    }
    ///
    /// \brief For all \f$i\in\left[0,Rank\right)\f$ subtracts the \f$i^{th}\f$
    ///        element of \c rhs to the \f$i^{th}\f$ element of \c *this and
    ///        stores the difference in the \f$i^{th}\f$ element of \c *this.
    ///
    /// \param rhs right-hand side \ref offset
    /// \return \c *this
    ///
    constexpr offset& operator-=(const offset& rhs)
    {
        for (size_type i = 0; i < rank; ++i)
        {
            (*this)[i] -= rhs[i];
        }
        return *this;
    }
    ///
    /// \brief \c <tt>++(*this)[0]</tt>.
    ///
    /// \note This function can solely be used if \f$Rank=1\f$.
    /// \return \c *this
    ///
    constexpr offset& operator++()
    {
        static_assert(Rank == 1, "This operator can only be used with rank == 1.");
        ++(*this)[0];
        return *this;
    }
    ///
    /// \brief \c <tt>(*this)[0]++</tt>.
    ///
    /// \note This function can solely be used if \f$Rank=1\f$.
    /// \return copy of \c *this
    ///
    constexpr offset operator++(int)
    {
        static_assert(Rank == 1, "This operator can only be used with rank == 1.");
        ++(*this);
        return *this;
    }
    ///
    /// \brief \c <tt>--(*this)[0]</tt>.
    ///
    /// \note This function can solely be used if \f$Rank=1\f$.
    /// \return \c *this
    ///
    constexpr offset& operator--()
    {
        static_assert(Rank == 1, "This operator can only be used with rank == 1.");
        --(*this)[0];
        return *this;
    }
    ///
    /// \brief \c <tt>(*this)[0]--</tt>.
    ///
    /// \note This function can solely be used if \f$Rank=1\f$.
    /// \return copy of \c *this
    ///
    constexpr offset operator--(int)
    {
        static_assert(Rank == 1, "This operator can only be used with rank == 1.");
        --(*this);
        return *this;
    }
    ///
    /// \brief For all \f$i\in\left[0,Rank\right)\f$ multiplies the \f$i^{th}\f$
    ///        element of \c *this by \c v and stores the product in the
    ///        \f$i^{th}\f$ element of \c *this.
    ///
    /// \param v value
    /// \return \c *this
    ///
    constexpr offset& operator*=(value_type v)
    {
        for (size_type i = 0; i < rank; ++i)
        {
            (*this)[i] *= v;
        }
        return *this;
    }
    ///
    /// \brief For all \f$i\in\left[0,Rank\right)\f$ divides the \f$i^{th}\f$
    ///        element of \c *this by \c v and stores the product in the
    ///        \f$i^{th}\f$ element of \c *this.
    ///
    /// \param v value
    /// \return \c *this
    ///
    constexpr offset& operator/=(value_type v)
    {
        for (size_type i = 0; i < rank; ++i)
        {
            (*this)[i] /= v;
        }
        return *this;
    }
};
///
/// \brief Addition of two \ref offset
///
/// \tparam Rank the rank of the \ref offset
///
/// \param lhs left-hand side \ref offset
/// \param rhs right-hand side \ref offset
/// \return \c <tt>offset<Rank>{lhs} += rhs</tt>
///
template <std::size_t Rank>
constexpr offset<Rank> operator+(const offset<Rank>& lhs, const offset<Rank>& rhs)
{
    offset<Rank> ret(lhs);
    ret += rhs;
    return ret;
}
///
/// \brief Subtraction of two \ref offset
///
/// \tparam Rank the rank of the \ref offset
///
/// \param lhs left-hand side \ref offset
/// \param rhs right-hand side \ref offset
/// \return \c <tt>offset<Rank>{lhs} -= rhs</tt>
///
template <std::size_t Rank>
constexpr offset<Rank> operator-(const offset<Rank>& lhs, const offset<Rank>& rhs)
{
    offset<Rank> ret(lhs);
    ret -= rhs;
    return ret;
}
///
/// \brief Multiplies an \ref offset with a value
///
/// \tparam Rank the rank of the \ref offset
///
/// \param lhs left-hand side \ref offset
/// \param v value
/// \return \c <tt>offset<Rank>{lhs} *= v</tt>
///
template <std::size_t Rank>
constexpr offset<Rank> operator*(const offset<Rank>& lhs, ptrdiff_t v)
{
    offset<Rank> ret(lhs);
    ret *= v;
    return ret;
}
///
/// \brief Multiplies a value with an \ref offset
///
/// \tparam Rank the rank of the \ref offset
///
/// \param v value
/// \param rhs right-hand side \ref offset
/// \return \c <tt>offset<Rank>{rhs} *= v</tt>
///
template <std::size_t Rank>
constexpr offset<Rank> operator*(ptrdiff_t v, const offset<Rank>& rhs)
{
    return operator*(rhs, v);
}
///
/// \brief Divides an \ref offset by a value
///
/// \tparam Rank the rank of the \ref offset
///
/// \param lhs left-hand side \ref offset
/// \param v value
/// \return \c <tt>offset<Rank>{lhs} /= v</tt>
///
template <std::size_t Rank>
constexpr offset<Rank> operator/(const offset<Rank>& lhs, ptrdiff_t v)
{
    offset<Rank> ret(lhs);
    ret /= v;
    return ret;
}

///
/// \brief \ref bounds used with all \ref multi_span or strided_multi_span
///
/// Use \ref make_bounds to create an \ref bounds with a specific rank.
///
/// \note \f$i\in\left[0,Rank\right)>0\f$

/// \tparam Rank the rank of the coordinate_facade
///
template <std::size_t Rank>
class bounds : public detail::coordinate_facade<Rank, std::size_t>
{
    using base_type = detail::coordinate_facade<Rank, std::size_t>;

public:
    /// size type
    using typename base_type::size_type;
    /// value type
    using typename base_type::value_type;
    /// storage type
    using typename base_type::storage_type;
    /// iterator type
    using iterator = detail::bounds_iterator<offset<Rank>, Rank>;
    /// const iterator type
    using const_iterator = detail::bounds_iterator<const offset<Rank>, Rank>;
    /// reverse iterator type
    using reverse_iterator = std::reverse_iterator<detail::bounds_iterator<offset<Rank>, Rank>>;
    /// const reverse iterator type
    using const_reverse_iterator = std::reverse_iterator<detail::bounds_iterator<const offset<Rank>, Rank>>;

    /// rank of the \ref bounds
    static constexpr size_type rank = Rank;

    /// Default constructor; zero-initializes each element
    constexpr bounds() : base_type() {}
    ///
    /// \brief For all \f$i\in\left[0,Rank\right)\f$ initializes the \f$i^{th}\f$
    ///        element of \c *this with \c <tt>*(il.begin() + i)</tt>.
    ///
    /// \param v first parameter
    /// \param ts remaining parameters
    ///
    template <typename... T>
    constexpr bounds(value_type v, T... ts) : base_type(v, ts...)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(v > 0, "value of v needs to be positive.");
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(detail::all_positive(ts...), "ts... need to be positive.");
    }
    ///
    /// \brief \ref bounds based on storage data
    ///
    /// \param data data to construct a \ref bounds
    ///
    constexpr bounds(const storage_type& data) : base_type(data) {}

    using base_type::operator[];
    ///
    /// \brief Size of \ref bounds
    ///
    /// \return The product of all elements of \c *this.
    ///
    constexpr size_type size() const
    {
        size_type ret = 1;
        for (size_type i = 0; i < rank; ++i)
        {
            ret *= (*this)[i];
        }
        return ret;
    }
    ///
    /// \brief Check if an \ref offset is within \ref bounds
    ///
    /// \return \c true, if \c <tt>0 <= idx[i]</tt> and \c <tt>idx[i] < (*this)[i]</tt>
    ///         \f$\forall i\in\left[0,Rank\right)\f$, otherwise \c false.
    ///
    constexpr bool contains(const offset<Rank>& idx) const
    {
        for (size_type i = 0; i < rank; ++i)
        {
            if ((idx[i] < 0) || (static_cast<value_type>(idx[i]) >= (*this)[i]))
            {
                return false;
            }
        }
        return true;
    }

    ///
    /// \brief Begin \ref iterator
    ///
    /// \return A \ref iterator referring to the first element of the space defined
    ///         by \c *this such that \c <tt>*begin() == offset<Rank>{}</tt>
    ///         if \c <tt>size() != 0</tt>, otherwise \c <tt>begin() == end()</tt> otherwise.
    ///
    constexpr iterator begin() { return iterator(*this); }
    ///
    /// \brief Begin \ref const_iterator
    ///
    /// \return A \ref const_iterator referring to the first element of the space defined
    ///         by \c *this such that \c <tt>*begin() == offset<Rank>{}</tt>
    ///         if \c <tt>size() != 0</tt>, otherwise \c <tt>begin() == end()</tt> otherwise.
    ///
    constexpr const_iterator begin() const { return const_iterator(*this); }
    ///
    /// \brief Begin \ref const_iterator
    ///
    /// \return A \ref const_iterator referring to the first element of the space defined
    ///         by \c *this such that \c <tt>*begin() == offset<Rank>{}</tt>
    ///         if \c <tt>size() != 0</tt>, otherwise \c <tt>begin() == end()</tt> otherwise.
    ///
    constexpr const_iterator cbegin() const { return const_iterator(*this); }
    ///
    /// \brief Begin \ref reverse_iterator
    ///
    /// \return A \ref reverse_iterator
    ///
    constexpr reverse_iterator rbegin() { return reverse_iterator(end()); }
    ///
    /// \brief Begin \ref const_reverse_iterator
    ///
    /// \return A \ref const_reverse_iterator
    ///
    constexpr const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    ///
    /// \brief Begin \ref const_reverse_iterator
    ///
    /// \return A \ref const_reverse_iterator
    ///
    constexpr const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    ///
    /// \brief End \ref iterator
    ///
    /// \return A \ref iterator which is the past-the-end \ref iterator for the
    ///         space defined by \c *this.
    ///
    constexpr iterator end() { return begin() + size(); }
    ///
    /// \brief End \ref const_iterator
    ///
    /// \return A \ref const_iterator which is the past-the-end \ref const_iterator
    ///          for the space defined by \c *this.
    ///
    constexpr const_iterator end() const
    {
        return begin() + static_cast<typename const_iterator::difference_type>(size());
    }
    ///
    /// \brief End \ref const_iterator
    ///
    /// \return A \ref const_iterator which is the past-the-end \ref const_iterator
    ///          for the space defined by \c *this.
    ///
    constexpr const_iterator cend() const { return cbegin() + size(); }
    ///
    /// \brief End \ref reverse_iterator
    ///
    /// \return A \ref reverse_iterator
    ///
    constexpr reverse_iterator rend() { return reverse_iterator(begin()); }
    ///
    /// \brief End \ref const_reverse_iterator
    ///
    /// \return A \ref const_reverse_iterator
    ///
    constexpr const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    ///
    /// \brief End \ref const_reverse_iterator
    ///
    /// \return A \ref const_reverse_iterator
    ///
    constexpr const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }
};

///
/// \brief Helper function to create \ref bounds
///
/// \param b1 value
/// \return \ref bounds with rank 1
///
constexpr bounds<1> make_bounds(const bounds<1>::value_type& b1)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b1 > 0, "value of b1 needs to be positive.");
    return bounds<1>(b1);
}
///
/// \brief Helper function to create \ref bounds
///
/// \param b1 value
/// \param b2 value
/// \return \ref bounds with rank 2
///
constexpr bounds<2> make_bounds(const bounds<2>::value_type& b1, const bounds<2>::value_type& b2)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b1 > 0, "value of b1 needs to be positive.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b2 > 0, "value of b2 needs to be positive.");

    using bounds_type = bounds<2>;
    return bounds_type{b1, b2};
}
///
/// \brief Helper function to create \ref bounds
///
/// \param b1 value
/// \param b2 value
/// \param b3 value
/// \return \ref bounds with rank 3
///
constexpr bounds<3>
make_bounds(const bounds<3>::value_type& b1, const bounds<3>::value_type& b2, const bounds<3>::value_type& b3)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b1 > 0, "value of b1 needs to be positive.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b2 > 0, "value of b2 needs to be positive.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b3 > 0, "value of b3 needs to be positive.");
    using bounds_type = bounds<3>;
    return bounds_type{b1, b2, b3};
}
///
/// \brief Helper function to create \ref bounds
///
/// \param b1 value
/// \param b2 value
/// \param b3 value
/// \param b4 value
/// \return \ref bounds with rank 4
///
constexpr bounds<4> make_bounds(const bounds<4>::value_type& b1,
                                const bounds<4>::value_type& b2,
                                const bounds<4>::value_type& b3,
                                const bounds<4>::value_type& b4)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b1 > 0, "value of b1 needs to be positive.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b2 > 0, "value of b2 needs to be positive.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b3 > 0, "value of b3 needs to be positive.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b4 > 0, "value of b4 needs to be positive.");
    using bounds_type = bounds<4>;
    return bounds_type{b1, b2, b3, b4};
}
///
/// \brief Helper function to create \ref bounds
///
/// \param b1 value
/// \param b2 value
/// \param b3 value
/// \param b4 value
/// \param b5 value
/// \return \ref bounds with rank 5
///
constexpr bounds<5> make_bounds(const bounds<5>::value_type& b1,
                                const bounds<5>::value_type& b2,
                                const bounds<5>::value_type& b3,
                                const bounds<5>::value_type& b4,
                                const bounds<5>::value_type& b5)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b1 > 0, "value of b1 needs to be positive.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b2 > 0, "value of b2 needs to be positive.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b3 > 0, "value of b3 needs to be positive.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b4 > 0, "value of b4 needs to be positive.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(b5 > 0, "value of b5 needs to be positive.");
    using bounds_type = bounds<5>;
    return bounds_type{b1, b2, b3, b4, b5};
}
///
/// \brief Helper function to create \ref offset
///
/// \param o1 value
/// \return \ref offset with rank 1
///
constexpr offset<1> make_offset(const offset<1>::value_type& o1) { return offset<1>(o1); }
///
/// \brief Helper function to create \ref offset
///
/// \param o1 value
/// \param o2 value
/// \return \ref offset with rank 2
///
constexpr offset<2> make_offset(const offset<2>::value_type& o1, const offset<2>::value_type& o2)
{
    using offset_type = offset<2>;
    return offset_type{o1, o2};
}
///
/// \brief Helper function to create \ref offset
///
/// \param o1 value
/// \param o2 value
/// \param o3 value
/// \return \ref offset with rank 3
///
constexpr offset<3>
make_offset(const offset<3>::value_type& o1, const offset<3>::value_type& o2, const offset<3>::value_type& o3)
{
    using offset_type = offset<3>;
    return offset_type{o1, o2, o3};
}
///
/// \brief Helper function to create \ref offset
///
/// \param o1 value
/// \param o2 value
/// \param o3 value
/// \param o4 value
/// \return \ref offset with rank 4
///
constexpr offset<4> make_offset(const offset<4>::value_type& o1,
                                const offset<4>::value_type& o2,
                                const offset<4>::value_type& o3,
                                const offset<4>::value_type& o4)
{
    using offset_type = offset<4>;
    return offset_type{o1, o2, o3, o4};
}
///
/// \brief Helper function to create \ref offset
///
/// \param o1 value
/// \param o2 value
/// \param o3 value
/// \param o4 value
/// \param o5 value
/// \return \ref offset with rank 5
///
constexpr offset<5> make_offset(const offset<5>::value_type& o1,
                                const offset<5>::value_type& o2,
                                const offset<5>::value_type& o3,
                                const offset<5>::value_type& o4,
                                const offset<5>::value_type& o5)
{
    using offset_type = offset<5>;
    return offset_type{o1, o2, o3, o4, o5};
}
///
/// \brief Helper function to create stride for \ref multi_span or strided_multi_span
///
/// \tparam Rank rank of the stride
///
/// \param bnd \ref bounds
/// \return \ref offset which contains the stride
///
template <std::size_t Rank>
constexpr offset<Rank> make_stride(const bounds<Rank>& bnd)
{
    offset<Rank> stride;
    stride[Rank - 1] = 1;
    for (std::size_t i = Rank - 1; i-- > 0;)
    {
        stride[i] = stride[i + 1] * static_cast<typename offset<Rank>::value_type>(bnd[i + 1]);
    }
    return stride;
}

constexpr offset<1> make_stride(const bounds<1>&) { return make_offset(1); }

///
/// \brief Multi span base class for a Multi-dimensional view on arrays
///
/// Define \c <tt>VIEW_ACCESS(data, idx, stride, rank)</tt> as \c <tt>*(data + offset)</tt>
/// where
/// \f$\text{offset}=\sum_{i=0}^{Rank-1}\text{idx}_{i}\times\text{stride}_{i},\text{idx}_{i}=\text{idx}\left[i\right]\f$
/// , and \f$\text{stride}_{i}=\text{stride}\left[i\right]\f$.
///
/// For a detailed explanation on the usage, please go to \ref score::cpp::multi_span.
///
/// \tparam T value type
/// \tparam Rank rank of the \ref multi_span_base
///
/// \par Thread safety
/// - <em>Shared object:</em> No
/// - <em>Distinct object:</em> Yes
///
template <typename T, std::size_t Rank>
class multi_span_base
{
    static_assert(Rank > 0, "Rank must be greater than 0!");

public:
    /// offset type
    using offset_type = offset<Rank>;
    /// bounds type
    using bounds_type = score::cpp::bounds<Rank>;
    /// size type
    using size_type = std::size_t;
    /// value type
    using value_type = T;
    /// pointer type
    using pointer = T*;
    /// reference type
    using reference = T&;

    /// rank of the \ref multi_span_base
    static constexpr size_type rank = Rank;

    ///
    /// \brief Converting constructor from another multi_span_base
    ///
    /// \param rhs \ref multi_span_base
    ///
    /// The resulting span has size() == rhs.size() and data() == rhs.data().
    /// This overload only participates in overload resolution if U (*)[] is convertible to value_type (*)[]
    template <typename U, typename = std::enable_if_t<std::is_convertible<U (*)[], value_type (*)[]>::value>>
    constexpr multi_span_base(const multi_span_base<U, Rank>& rhs)
        : bnds_(rhs.bnds_), srd_(rhs.srd_), data_ptr_(rhs.data_ptr_)
    {
    }

    ///
    /// \brief Get the bounds of the \ref multi_span_base
    ///
    /// \return \ref bounds
    ///
    constexpr const bounds_type& bounds() const { return bnds_; }
    ///
    /// \brief Get the size of the \ref multi_span_base
    ///
    /// \return size
    ///
    constexpr size_type size() const { return bnds_.size(); }
    ///
    /// \brief Get the stride of the \ref multi_span_base
    ///
    /// \return stride
    ///
    constexpr const offset_type& stride() const { return srd_; }
    ///
    /// \brief Element access
    ///
    /// \note \c <tt>bounds().contains(idx) == true</tt>
    ///
    /// \return \c <tt>VIEW_ACCESS(data_ptr_, idx, stride(), Rank)</tt>
    ///
    constexpr reference operator[](const offset_type& idx) const
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(data_ptr_ != nullptr);
        pointer ptr = data_ptr_;
        for (size_type i = 0; i < rank; ++i)
        {
            ptr += idx[i] * srd_[i];
        }
        return *ptr;
    }
    ///
    /// \brief Element access
    ///
    /// \note \c <tt>bounds().contains(idx) == true</tt>
    ///
    /// \return \c <tt>VIEW_ACCESS(data_ptr_, idx, stride(), Rank)</tt>
    ///
    constexpr reference at(const offset_type& idx) const
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(bnds_.contains(idx));
        return operator[](idx);
    }
    ///
    /// \brief Get data
    ///
    /// \return raw data pointer
    ///
    constexpr pointer data() const { return data_ptr_; }

protected:
    ///
    /// \brief Constructs an empty multi_span_base whose data() == nullptr and size() == 0.
    ///
    constexpr multi_span_base() = default;

    ///
    /// \brief Constructor
    ///
    /// \param bnds \ref bounds
    /// \param stride \ref offset representing the stride
    /// \param data raw data pointer
    ///
    constexpr multi_span_base(const bounds_type& bnds, const offset_type& stride, pointer const data)
        : bnds_(bnds), srd_(stride), data_ptr_(data)
    {
    }

private:
    template <typename U, std::size_t R>
    friend class multi_span_base;

    bounds_type bnds_{}; ///< \ref bounds of the \ref multi_span_base
    offset_type srd_{};  ///< \ref offset representing the stride of the \ref multi_span_base
    pointer data_ptr_{}; ///< raw data pointer of the \ref multi_span_base
};

///
/// \brief Multi span (\ref multi_span_base)
///
/// For a detailed explanation on the usage, please go to \ref score::cpp::multi_span.
///
/// \tparam T value type
/// \tparam Rank rank of the \ref multi_span_base
///
/// \par Thread safety
/// - <em>Shared object:</em> No
/// - <em>Distinct object:</em> Yes
///
template <typename T, std::size_t Rank>
class multi_span : public multi_span_base<T, Rank>
{
    using base_type = multi_span_base<T, Rank>;

public:
    /// offset type
    using typename base_type::offset_type;
    /// bounds type
    using typename base_type::bounds_type;
    /// size type
    using typename base_type::size_type;
    /// value type
    using typename base_type::value_type;
    /// pointer
    using typename base_type::pointer;

    ///
    /// \brief Constructs an empty multi_span_base whose data() == nullptr and size() == 0.
    ///
    constexpr multi_span() : base_type() {}

    ///
    /// \brief Converting constructor from another multi_span
    ///
    /// \param rhs \ref multi_span
    ///
    /// The resulting span has size() == rhs.size() and data() == rhs.data().
    /// This overload only participates in overload resolution if U (*)[] is convertible to value_type (*)[]
    template <typename U, typename = std::enable_if_t<std::is_convertible<U (*)[], value_type (*)[]>::value>>
    constexpr multi_span(const multi_span<U, Rank>& rhs) : base_type(rhs)
    {
    }

    ///
    /// \brief Constructor based on a data pointer
    ///
    /// \note No checks are performed on the data pointer. Please make sure,
    ///       that you do not exceed your memory area.
    ///
    /// \param bnds \ref bounds
    /// \param data data pointer
    ///
    constexpr multi_span(const bounds_type& bnds, pointer const data) : base_type(bnds, make_stride(bnds), data)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(data != nullptr);
    }
    ///
    /// \brief Constructor based on a fixed-size array
    ///
    /// \param bnds \ref bounds
    /// \param data a fixed-size array
    ///
    template <std::size_t N>
    constexpr multi_span(const bounds_type& bnds, value_type (&data)[N])
        : base_type(bnds, make_stride(bnds), static_cast<pointer>(data))
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(bnds.size() <= N);
    }
    ///
    /// \brief Constructor based on a mutable std::array
    ///
    /// \param bnds \ref bounds
    /// \param data a mutable std::array
    ///
    template <std::size_t N>
    constexpr multi_span(const bounds_type& bnds, std::array<value_type, N>& data)
        : base_type(bnds, make_stride(bnds), data.data())
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(bnds.size() <= N);
    }
    ///
    /// \brief Constructor based on a read-only std::array
    ///
    /// \param bnds \ref bounds
    /// \param data a read-only std::array
    ///
    template <std::size_t N>
    constexpr multi_span(const bounds_type& bnds,
                         const std::array<typename std::remove_const<value_type>::type, N>& data)
        : base_type(bnds, make_stride(bnds), data.data())
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(bnds.size() <= N);
    }

    using base_type::operator[];
    ///
    /// \brief Get a slice of the \ref multi_span
    ///
    /// \note \c <tt>bounds().contains(idx) == true</tt>
    /// \note Rank > 1
    ///
    /// \param slice slice
    ///
    /// \return A reduced \ref multi_span.
    ///
    constexpr multi_span<value_type, Rank - 1> operator[](size_type slice) const
    {
        static_assert(Rank > 1, "Rank must be greater than 1!");
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(slice < static_cast<typename bounds_type::value_type>(base_type::bounds()[0]));

        offset_type idx;
        idx[0] = static_cast<typename offset_type::value_type>(slice);

        score::cpp::bounds<Rank - 1> bnd;
        for (size_type i = 1; i < Rank; ++i)
        {
            bnd[i - 1] = base_type::bounds()[i];
        }

        return multi_span<value_type, Rank - 1>(bnd, &operator[](idx));
    }
};
/// \cond detail
template <typename T>
class multi_span<T, 1> : public multi_span_base<T, 1>
{
    using base_type = multi_span_base<T, 1>;

public:
    /// offset type
    using typename base_type::offset_type;
    /// bounds type
    using typename base_type::bounds_type;
    /// size type
    using typename base_type::size_type;
    /// value type
    using typename base_type::value_type;
    /// pointer
    using typename base_type::pointer;

    ///
    /// \brief Constructs an empty multi_span_base whose data() == nullptr and size() == 0.
    ///
    constexpr multi_span() : base_type() {}

    ///
    /// \brief Converting constructor from another multi_span
    ///
    /// \param rhs \ref multi_span
    ///
    /// The resulting span has size() == rhs.size() and data() == rhs.data().
    /// This overload only participates in overload resolution if U (*)[] is convertible to value_type (*)[]
    template <typename U, typename = std::enable_if_t<std::is_convertible<U (*)[], value_type (*)[]>::value>>
    constexpr multi_span(const multi_span<U, 1>& rhs) : base_type(rhs)
    {
    }
    ///
    /// \brief Constructor based on a data pointer
    ///
    /// \note No checks are performed on the data pointer. Please make sure,
    ///       that you do not exceed your memory area.
    ///
    /// \param bnds \ref bounds
    /// \param data data pointer
    ///
    constexpr multi_span(const bounds_type& bnds, pointer const data) : base_type(bnds, make_stride(bnds), data)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(data != nullptr);
    }
    ///
    /// \brief Constructor based on a fixed-size array
    ///
    /// \param bnds \ref bounds
    /// \param data a fixed-size array
    ///
    template <std::size_t N>
    multi_span(const bounds_type& bnds, value_type (&data)[N])
        : base_type(bnds, make_stride(bnds), static_cast<pointer>(data))
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(bnds.size() <= N);
    }
    ///
    /// \brief Constructor based on a mutable std::array
    ///
    /// \param bnds \ref bounds
    /// \param data a mutable std::array
    ///
    template <std::size_t N>
    multi_span(const bounds_type& bnds, std::array<value_type, N>& data)
        : base_type(bnds, make_stride(bnds), data.data())
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(bnds.size() <= N);
    }
    ///
    /// \brief Constructor based on a read-only std::array
    ///
    /// \param bnds \ref bounds
    /// \param data a read-only std::array
    ///
    template <std::size_t N>
    multi_span(const bounds_type& bnds, const std::array<typename std::remove_const<value_type>::type, N>& data)
        : base_type(bnds, make_stride(bnds), data.data())
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(bnds.size() <= N);
    }

    using base_type::operator[];
};
/// \endcond

///
/// \brief Construct an \ref multi_span based on a data pointer
///
/// \note No checks are performed on the data pointer. Please make sure,
///       that you do not exceed your memory area.
///
/// \param bnds \ref bounds
/// \param data data pointer
///
template <typename T, std::size_t Rank>
constexpr multi_span<T, Rank> as_multi_span(const bounds<Rank>& bnds, T* const data)
{
    return multi_span<T, Rank>(bnds, data);
}
///
/// \brief Construct an \ref multi_span based on a fixed-size array
///
/// \param bnds \ref bounds
/// \param data a fixed-size array
///
template <typename T, std::size_t Rank, std::size_t N>
constexpr multi_span<T, Rank> as_multi_span(const typename multi_span<T, Rank>::bounds_type& bnds, T (&data)[N])
{
    return multi_span<T, Rank>(bnds, data);
}
///
/// \brief Construct an 1D \ref multi_span based on a fixed-size array
///
/// \param data a fixed-size array
///
template <typename T, std::size_t N>
constexpr multi_span<T, 1> as_multi_span(T (&data)[N])
{
    return multi_span<T, 1>(make_bounds(N), data);
}
///
/// \brief Construct an \ref multi_span based on a mutable std::array
///
/// \param bnds \ref bounds
/// \param data a mutable std::array
///
template <typename T, std::size_t Rank, std::size_t N>
constexpr multi_span<T, Rank> as_multi_span(const bounds<Rank>& bnds, std::array<T, N>& data)
{
    return multi_span<T, Rank>(bnds, data);
}
///
/// \brief Construct an 1D \ref multi_span based on a mutable std::array
///
/// \param data a mutable std::array
///
template <typename T, std::size_t N>
constexpr multi_span<T, 1> as_multi_span(std::array<T, N>& data)
{
    return multi_span<T, 1>(make_bounds(N), data);
}
///
/// \brief Construct an \ref multi_span based on a read-only std::array
///
/// \param bnds \ref bounds
/// \param data a read-only std::array
///
template <typename T, std::size_t Rank, std::size_t N>
constexpr multi_span<const T, Rank> as_multi_span(const bounds<Rank>& bnds, const std::array<T, N>& data)
{
    return multi_span<const T, Rank>(bnds, data);
}
///
/// \brief Construct an 1D \ref multi_span based on a read-only std::array
///
/// \param data a read-only std::array
///
template <typename T, std::size_t N>
constexpr multi_span<const T, 1> as_multi_span(const std::array<T, N>& data)
{
    return multi_span<const T, 1>(make_bounds(N), data);
}

} // namespace score::cpp
#endif // SCORE_LANGUAGE_FUTURECPP_MULTI_SPAN_HPP
