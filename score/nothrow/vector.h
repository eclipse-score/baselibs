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
#ifndef SCORE_NOTHROW_VECTOR_H
#define SCORE_NOTHROW_VECTOR_H

#include "score/nothrow/container_error.h"
#include "score/nothrow/memory_resource.h"
#include "score/nothrow/pointer_box.h"

#include "score/result/result.h"

#include <cstddef>
#include <cstdlib>
#include <limits>
#include <new>
#include <algorithm>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace score::nothrow
{

/// @brief Nothrow counterpart to `std::vector`.
///
/// Deviations from `std::vector`:
/// - All member functions are noexcept.
/// - Precondition failures abort (`at`, `front`, `back`, `pop_back`, `erase`).
/// - Potential allocation failures are reported via `score::Result*` with
///   `ContainerErrorCode::kOutOfMemory` (`Create*`, `PushBack*`, `EmplaceBack*`,
///   `Insert*`, `Reserve*`, `Resize*`, `ShrinkToFit*`, `Clone*`).
/// - Not copyable. Use Clone() for explicit deep copies.
/// - Storage pointer type is injected via PointerPolicy::Ptr (default: score::nothrow::OffsetBox).
///
/// @tparam T Element type.
/// @tparam Allocator Allocator type, defaults to score::nothrow::PolymorphicAllocator<T>.
template <typename T, typename Allocator = PolymorphicAllocator<T>, typename PointerPolicy = OffsetBoxPolicy>
class Vector
{
  public:
    using value_type = T;
    using allocator_type = Allocator;
    using pointer_policy = PointerPolicy;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    /// @brief Allocates storage for @p initial_capacity elements without constructing any.
    /// @return kOutOfMemory if allocation fails.
    static score::Result<Vector> CreateWithCapacity(const size_type initial_capacity,
                                                    allocator_type allocator = allocator_type{}) noexcept
    {
        if (initial_capacity == 0U)
        {
            return Vector{allocator, nullptr, 0U, 0U, false};
        }

        if (initial_capacity > (std::numeric_limits<size_type>::max() / sizeof(value_type)))
        {
            return score::Result<Vector>{score::unexpect,
                                         MakeError(ContainerErrorCode::kOutOfMemory, "Vector capacity overflow")};
        }

        pointer const allocated_storage = allocator.allocate(initial_capacity);
        if (allocated_storage == nullptr)
        {
            return score::Result<Vector>{score::unexpect,
                                         MakeError(ContainerErrorCode::kOutOfMemory, "Vector allocation failed")};
        }

        return Vector{allocator, allocated_storage, 0U, initial_capacity, false};
    }

    /// @brief Like CreateWithCapacity(), but aborts on allocation failure.
    static Vector CreateWithCapacityOrAbort(const size_type initial_capacity,
                                            allocator_type allocator = allocator_type{}) noexcept
    {
        score::Result<Vector> created = CreateWithCapacity(initial_capacity, allocator);
        if (!created.has_value())
        {
            std::abort();
        }
        return std::move(created).value();
    }

    /// @brief Creates a vector backed by externally owned storage.
    ///
    /// Deviation from typical `std::vector` ownership model: this vector does not
    /// own `buffer`, never reallocates it, and reports growth past capacity as
    /// `kOutOfMemory`.
    static Vector CreateWithBuffer(void* buffer, const size_type bytes) noexcept
    {
        const size_type element_capacity = bytes / sizeof(value_type);
        return Vector{allocator_type{}, static_cast<pointer>(buffer), 0U, element_capacity, true};
    }

    /// @brief Returns true if created with `CreateWithBuffer`.
    [[nodiscard]] bool has_fixed_capacity() const noexcept
    {
        return fixed_capacity_;
    }

    /// @brief Like std::vector(count). Creates a vector with @p count default-constructed elements.
    /// @return kOutOfMemory if allocation fails.
    static score::Result<Vector> Create(const size_type count,
                                        allocator_type allocator = allocator_type{}) noexcept
    {
        score::Result<Vector> created = CreateWithCapacity(count, std::move(allocator));
        if (!created.has_value())
        {
            return created;
        }

        Vector vector = std::move(created).value();
        if constexpr (std::is_default_constructible_v<value_type>)
        {
            pointer const raw_data = vector.data();
            for (size_type index = 0U; index < count; ++index)
            {
                new (raw_data + index) value_type{};  // NOLINT(score-no-dynamic-raw-memory)
            }
            vector.size_ = count;
        }
        else
        {
            if (count != 0U)
            {
                std::abort();
            }
        }
        return vector;
    }

    /// @brief Like std::vector(count, value). Creates a vector with @p count copies of @p value.
    /// @return kOutOfMemory if allocation fails.
    static score::Result<Vector> Create(const size_type count,
                                        const value_type& value,
                                        allocator_type allocator = allocator_type{}) noexcept
    {
        score::Result<Vector> created = CreateWithCapacity(count, std::move(allocator));
        if (!created.has_value())
        {
            return created;
        }

        Vector vector = std::move(created).value();
        pointer const raw_data = vector.data();
        for (size_type index = 0U; index < count; ++index)
        {
            new (raw_data + index) value_type(value);  // NOLINT(score-no-dynamic-raw-memory)
        }
        vector.size_ = count;
        return vector;
    }

    /// @brief Creates a vector from an iterator range [first, last).
    /// @return kOutOfMemory if allocation fails.
    template <typename InputIt>
    static score::Result<Vector> Create(InputIt first,
                                        InputIt last,
                                        allocator_type allocator = allocator_type{}) noexcept
    {
        const auto count = static_cast<size_type>(std::distance(first, last));
        score::Result<Vector> created = CreateWithCapacity(count, std::move(allocator));
        if (!created.has_value())
        {
            return created;
        }

        Vector vector = std::move(created).value();
        pointer const raw_data = vector.data();
        size_type index = 0U;
        for (InputIt it = first; it != last; ++it, ++index)
        {
            new (raw_data + index) value_type(*it);  // NOLINT(score-no-dynamic-raw-memory)
        }
        vector.size_ = count;
        return vector;
    }

    /// @brief Creates a vector from an initializer list.
    /// @return kOutOfMemory if allocation fails.
    static score::Result<Vector> Create(std::initializer_list<value_type> init,
                                        allocator_type allocator = allocator_type{}) noexcept
    {
        return Create(init.begin(), init.end(), std::move(allocator));
    }

    /// @brief Like Create(count), but aborts on allocation failure.
    static Vector CreateOrAbort(const size_type count,
                                allocator_type allocator = allocator_type{}) noexcept
    {
        score::Result<Vector> created = Create(count, std::move(allocator));
        if (!created.has_value())
        {
            std::abort();
        }
        return std::move(created).value();
    }

    /// @brief Like Create(count, value), but aborts on allocation failure.
    static Vector CreateOrAbort(const size_type count,
                                const value_type& value,
                                allocator_type allocator = allocator_type{}) noexcept
    {
        score::Result<Vector> created = Create(count, value, std::move(allocator));
        if (!created.has_value())
        {
            std::abort();
        }
        return std::move(created).value();
    }

    /// @brief Like Create(first, last), but aborts on allocation failure.
    template <typename InputIt>
    static Vector CreateOrAbort(InputIt first,
                                InputIt last,
                                allocator_type allocator = allocator_type{}) noexcept
    {
        score::Result<Vector> created = Create(first, last, std::move(allocator));
        if (!created.has_value())
        {
            std::abort();
        }
        return std::move(created).value();
    }

    /// @brief Like Create(initializer_list), but aborts on allocation failure.
    static Vector CreateOrAbort(std::initializer_list<value_type> init,
                                allocator_type allocator = allocator_type{}) noexcept
    {
        score::Result<Vector> created = Create(init, std::move(allocator));
        if (!created.has_value())
        {
            std::abort();
        }
        return std::move(created).value();
    }

    Vector(const Vector&) = delete;
    Vector& operator=(const Vector&) = delete;

    Vector(Vector&& other) noexcept
        : allocator_{other.allocator_},
          data_{other.data_},
          size_{other.size_},
          capacity_{other.capacity_},
          fixed_capacity_{other.fixed_capacity_}
    {
        other.data_ = ptr_box<value_type>{nullptr};
        other.size_ = 0U;
        other.capacity_ = 0U;
        other.fixed_capacity_ = false;
    }

    Vector& operator=(Vector&& other) noexcept
    {
        if (this != &other)
        {
            DestroyElements();
            DeallocateStorage();

            allocator_ = other.allocator_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            fixed_capacity_ = other.fixed_capacity_;

            other.data_ = ptr_box<value_type>{nullptr};
            other.size_ = 0U;
            other.capacity_ = 0U;
            other.fixed_capacity_ = false;
        }
        return *this;
    }

    ~Vector() noexcept
    {
        DestroyElements();
        DeallocateStorage();
    }

    [[nodiscard]] size_type size() const noexcept
    {
        return size_;
    }

    [[nodiscard]] size_type capacity() const noexcept
    {
        return capacity_;
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return size_ == 0U;
    }

    [[nodiscard]] pointer begin() noexcept
    {
        return data();
    }

    [[nodiscard]] const_pointer begin() const noexcept
    {
        return data();
    }

    [[nodiscard]] const_pointer cbegin() const noexcept
    {
        return data();
    }

    [[nodiscard]] pointer end() noexcept
    {
        return data() + size_;
    }

    [[nodiscard]] const_pointer end() const noexcept
    {
        return data() + size_;
    }

    [[nodiscard]] const_pointer cend() const noexcept
    {
        return data() + size_;
    }

    reference operator[](const size_type index) noexcept
    {
        return data()[index];
    }

    const_reference operator[](const size_type index) const noexcept
    {
        return data()[index];
    }

    pointer data() noexcept
    {
        return data_.get();
    }

    const_pointer data() const noexcept
    {
        return data_.get();
    }

    /// @brief Equivalent to `std::vector::front()`.
    /// @note Aborts when empty (standard API has undefined behavior).
    reference front() noexcept
    {
        if (empty())
        {
            std::abort();
        }
        return data()[0U];
    }

    const_reference front() const noexcept
    {
        if (empty())
        {
            std::abort();
        }
        return data()[0U];
    }

    /// @brief Equivalent to `std::vector::back()`.
    /// @note Aborts when empty (standard API has undefined behavior).
    reference back() noexcept
    {
        if (empty())
        {
            std::abort();
        }
        return data()[size_ - 1U];
    }

    const_reference back() const noexcept
    {
        if (empty())
        {
            std::abort();
        }
        return data()[size_ - 1U];
    }

    /// @brief Equivalent to `std::vector::at()`.
    /// @note Aborts if index >= size(); `std::vector::at()` throws.
    reference at(const size_type index) noexcept
    {
        if (index >= size_)
        {
            std::abort();
        }
        return data()[index];
    }

    const_reference at(const size_type index) const noexcept
    {
        if (index >= size_)
        {
            std::abort();
        }
        return data()[index];
    }

    /// @brief Equivalent to `std::vector::push_back()`.
    /// @return `kOutOfMemory` if growth allocation fails.
    score::ResultBlank PushBack(const value_type& value) noexcept
    {
        if (size_ == capacity_)
        {
            const score::ResultBlank resized = Reserve(GrowCapacity(capacity_));
            if (!resized.has_value())
            {
                return resized;
            }
        }

        pointer const raw_data = data();
        new (raw_data + size_) value_type(value);  // NOLINT(score-no-dynamic-raw-memory)
        ++size_;
        return score::ResultBlank{};
    }

    /// @brief Move overload of `PushBack`.
    /// @return `kOutOfMemory` if growth allocation fails.
    score::ResultBlank PushBack(value_type&& value) noexcept
    {
        if (size_ == capacity_)
        {
            const score::ResultBlank resized = Reserve(GrowCapacity(capacity_));
            if (!resized.has_value())
            {
                return resized;
            }
        }

        pointer const raw_data = data();
        new (raw_data + size_) value_type(std::move(value));  // NOLINT(score-no-dynamic-raw-memory)
        ++size_;
        return score::ResultBlank{};
    }

    /// @brief Like PushBack(), but aborts on allocation failure.
    void PushBackOrAbort(const value_type& value) noexcept
    {
        const score::ResultBlank result = PushBack(value);
        if (!result.has_value())
        {
            std::abort();
        }
    }

    void PushBackOrAbort(value_type&& value) noexcept
    {
        const score::ResultBlank result = PushBack(std::move(value));
        if (!result.has_value())
        {
            std::abort();
        }
    }

    /// @brief Equivalent to `std::vector::emplace_back()`.
    /// @return `kOutOfMemory` if growth allocation fails.
    template <typename... Args>
    score::ResultBlank EmplaceBack(Args&&... args) noexcept
    {
        if (size_ == capacity_)
        {
            const score::ResultBlank resized = Reserve(GrowCapacity(capacity_));
            if (!resized.has_value())
            {
                return resized;
            }
        }

        pointer const raw_data = data();
        new (raw_data + size_) value_type(std::forward<Args>(args)...);  // NOLINT(score-no-dynamic-raw-memory)
        ++size_;
        return score::ResultBlank{};
    }

    /// @brief Like EmplaceBack(), but aborts on allocation failure.
    template <typename... Args>
    void EmplaceBackOrAbort(Args&&... args) noexcept
    {
        const score::ResultBlank result = EmplaceBack(std::forward<Args>(args)...);
        if (!result.has_value())
        {
            std::abort();
        }
    }

    /// @brief Equivalent to `std::vector::pop_back()`.
    /// @note Aborts when empty (standard API has undefined behavior).
    void pop_back() noexcept
    {
        if (empty())
        {
            std::abort();
        }

        --size_;
        data()[size_].~value_type();
    }

    /// @brief Equivalent to `std::vector::reserve()`.
    /// @return `kOutOfMemory` if allocation fails.
    score::ResultBlank Reserve(const size_type new_capacity) noexcept
    {
        if (new_capacity <= capacity_)
        {
            return score::ResultBlank{};
        }

        if (fixed_capacity_)
        {
            return score::ResultBlank{score::unexpect,
                                      MakeError(ContainerErrorCode::kOutOfMemory, "Vector has fixed capacity")};
        }

        if (new_capacity > (std::numeric_limits<size_type>::max() / sizeof(value_type)))
        {
            return score::ResultBlank{score::unexpect,
                                      MakeError(ContainerErrorCode::kOutOfMemory, "Vector capacity overflow")};
        }

        pointer const new_storage = allocator_.allocate(new_capacity);
        if (new_storage == nullptr)
        {
            return score::ResultBlank{score::unexpect,
                                      MakeError(ContainerErrorCode::kOutOfMemory, "Vector allocation failed")};
        }

        pointer const old_storage = data();
        for (size_type index = 0U; index < size_; ++index)
        {
            new (new_storage + index) value_type(std::move(old_storage[index]));  // NOLINT(score-no-dynamic-raw-memory)
            old_storage[index].~value_type();
        }

        if (capacity_ != 0U)
        {
            allocator_.deallocate(old_storage, capacity_);
        }

        data_ = ptr_box<value_type>{new_storage};
        capacity_ = new_capacity;
        return score::ResultBlank{};
    }

    /// @brief Like Reserve(), but aborts on allocation failure.
    void ReserveOrAbort(const size_type new_capacity) noexcept
    {
        const score::ResultBlank result = Reserve(new_capacity);
        if (!result.has_value())
        {
            std::abort();
        }
    }

    /// @brief Equivalent to `std::vector::resize(count)`.
    /// @return `kOutOfMemory` if growth allocation fails.
    /// @note Aborts when growth is requested for non-default-constructible `T`.
    score::ResultBlank Resize(const size_type new_size) noexcept
    {
        if (new_size > size_)
        {
            if constexpr (std::is_default_constructible_v<value_type>)
            {
                const score::ResultBlank reserved = Reserve(new_size);
                if (!reserved.has_value())
                {
                    return reserved;
                }

                pointer const raw_data = data();
                for (size_type index = size_; index < new_size; ++index)
                {
                    new (raw_data + index) value_type{};  // NOLINT(score-no-dynamic-raw-memory)
                }
            }
            else
            {
                std::abort();
            }
        }
        else
        {
            pointer const raw_data = data();
            for (size_type index = new_size; index < size_; ++index)
            {
                raw_data[index].~value_type();
            }
        }

        size_ = new_size;
        return score::ResultBlank{};
    }

    /// @brief Equivalent to `std::vector::resize(count, value)`.
    /// @return `kOutOfMemory` if growth allocation fails.
    score::ResultBlank Resize(const size_type new_size, const value_type& value) noexcept
    {
        if (new_size > size_)
        {
            const score::ResultBlank reserved = Reserve(new_size);
            if (!reserved.has_value())
            {
                return reserved;
            }

            pointer const raw_data = data();
            for (size_type index = size_; index < new_size; ++index)
            {
                new (raw_data + index) value_type(value);  // NOLINT(score-no-dynamic-raw-memory)
            }
        }
        else
        {
            pointer const raw_data = data();
            for (size_type index = new_size; index < size_; ++index)
            {
                raw_data[index].~value_type();
            }
        }

        size_ = new_size;
        return score::ResultBlank{};
    }

    /// @brief Like Resize(), but aborts on allocation failure.
    void ResizeOrAbort(const size_type new_size) noexcept
    {
        const score::ResultBlank result = Resize(new_size);
        if (!result.has_value())
        {
            std::abort();
        }
    }

    void ResizeOrAbort(const size_type new_size, const value_type& value) noexcept
    {
        const score::ResultBlank result = Resize(new_size, value);
        if (!result.has_value())
        {
            std::abort();
        }
    }

    /// @brief Equivalent to `std::vector::shrink_to_fit()`.
    /// @return `kOutOfMemory` if reallocation fails.
    /// @note Unlike standard API, this is a guaranteed shrink when possible.
    score::ResultBlank ShrinkToFit() noexcept
    {
        if (fixed_capacity_ || (size_ == capacity_))
        {
            return score::ResultBlank{};
        }

        if (size_ == 0U)
        {
            DeallocateStorage();
            return score::ResultBlank{};
        }

        pointer const new_storage = allocator_.allocate(size_);
        if (new_storage == nullptr)
        {
            return score::ResultBlank{score::unexpect,
                                      MakeError(ContainerErrorCode::kOutOfMemory, "Vector allocation failed")};
        }

        pointer const old_storage = data();
        for (size_type index = 0U; index < size_; ++index)
        {
            new (new_storage + index) value_type(std::move(old_storage[index]));  // NOLINT(score-no-dynamic-raw-memory)
            old_storage[index].~value_type();
        }

        allocator_.deallocate(old_storage, capacity_);
        data_ = ptr_box<value_type>{new_storage};
        capacity_ = size_;
        return score::ResultBlank{};
    }

    /// @brief Like ShrinkToFit(), but aborts on allocation failure.
    void ShrinkToFitOrAbort() noexcept
    {
        const score::ResultBlank result = ShrinkToFit();
        if (!result.has_value())
        {
            std::abort();
        }
    }

    /// @brief Equivalent to inserting before position index.
    /// @return `kOutOfMemory` on growth failure.
    /// @note Aborts if index > size().
    score::ResultBlank Insert(const size_type index, const value_type& value) noexcept
    {
        if (index > size_)
        {
            std::abort();
        }

        if (size_ == capacity_)
        {
            const score::ResultBlank resized = Reserve(GrowCapacity(capacity_));
            if (!resized.has_value())
            {
                return resized;
            }
        }

        pointer const raw_data = data();
        for (size_type pos = size_; pos > index; --pos)
        {
            new (raw_data + pos) value_type(std::move(raw_data[pos - 1U]));  // NOLINT(score-no-dynamic-raw-memory)
            raw_data[pos - 1U].~value_type();
        }
        new (raw_data + index) value_type(value);  // NOLINT(score-no-dynamic-raw-memory)
        ++size_;
        return score::ResultBlank{};
    }

    /// @brief Move overload of `Insert`.
    /// @return `kOutOfMemory` on growth failure.
    /// @note Aborts if index > size().
    score::ResultBlank Insert(const size_type index, value_type&& value) noexcept
    {
        if (index > size_)
        {
            std::abort();
        }

        if (size_ == capacity_)
        {
            const score::ResultBlank resized = Reserve(GrowCapacity(capacity_));
            if (!resized.has_value())
            {
                return resized;
            }
        }

        pointer const raw_data = data();
        for (size_type pos = size_; pos > index; --pos)
        {
            new (raw_data + pos) value_type(std::move(raw_data[pos - 1U]));  // NOLINT(score-no-dynamic-raw-memory)
            raw_data[pos - 1U].~value_type();
        }
        new (raw_data + index) value_type(std::move(value));  // NOLINT(score-no-dynamic-raw-memory)
        ++size_;
        return score::ResultBlank{};
    }

    /// @brief Like Insert(), but aborts on failure.
    void InsertOrAbort(const size_type index, const value_type& value) noexcept
    {
        const score::ResultBlank result = Insert(index, value);
        if (!result.has_value())
        {
            std::abort();
        }
    }

    void InsertOrAbort(const size_type index, value_type&& value) noexcept
    {
        const score::ResultBlank result = Insert(index, std::move(value));
        if (!result.has_value())
        {
            std::abort();
        }
    }

    /// @brief Equivalent to erase-at-index helper.
    /// @note Aborts if index >= size().
    void erase(const size_type index) noexcept
    {
        if (index >= size_)
        {
            std::abort();
        }

        pointer const raw_data = data();
        raw_data[index].~value_type();
        for (size_type pos = index; pos + 1U < size_; ++pos)
        {
            new (raw_data + pos) value_type(std::move(raw_data[pos + 1U]));  // NOLINT(score-no-dynamic-raw-memory)
            raw_data[pos + 1U].~value_type();
        }
        --size_;
    }

    void clear() noexcept
    {
        DestroyElements();
    }

    void swap(Vector& other) noexcept
    {
        using std::swap;
        swap(allocator_, other.allocator_);
        swap(data_, other.data_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
        swap(fixed_capacity_, other.fixed_capacity_);
    }

    /// @brief Deep-copy helper (standard copy constructor equivalent).
    /// @return `kOutOfMemory` if allocation fails.
    score::Result<Vector> Clone() const noexcept
    {
        score::Result<Vector> created = CreateWithCapacity(capacity_, allocator_);
        if (!created.has_value())
        {
            return created;
        }

        Vector clone = std::move(created).value();
        for (size_type i = 0U; i < size_; ++i)
        {
            const score::ResultBlank push_result = clone.PushBack(data()[i]);
            if (!push_result.has_value())
            {
                return score::Result<Vector>{score::unexpect, push_result.error()};
            }
        }

        return clone;
    }

    /// @brief Like Clone(), but aborts on allocation failure.
    Vector CloneOrAbort() const noexcept
    {
        score::Result<Vector> clone = Clone();
        if (!clone.has_value())
        {
            std::abort();
        }
        return std::move(clone).value();
    }

  private:
    template <typename U>
    using ptr_box = typename pointer_policy::template Ptr<U>;

    Vector(const allocator_type& allocator, pointer const storage, const size_type used_size, const size_type total_capacity, const bool fixed_capacity) noexcept
        : allocator_{allocator}, data_{storage}, size_{used_size}, capacity_{total_capacity}, fixed_capacity_{fixed_capacity}
    {
    }

    static size_type GrowCapacity(const size_type current) noexcept
    {
        if (current == 0U)
        {
            return 1U;
        }
        constexpr size_type kMaxCapacity = std::numeric_limits<size_type>::max() / sizeof(value_type);
        if (current > (kMaxCapacity / 2U))
        {
            return kMaxCapacity;
        }
        return current * 2U;
    }

    void DestroyElements() noexcept
    {
        pointer const raw_data = data();
        for (size_type index = 0U; index < size_; ++index)
        {
            raw_data[index].~value_type();
        }
        size_ = 0U;
    }

    void DeallocateStorage() noexcept
    {
        if ((capacity_ != 0U) && !fixed_capacity_)
        {
            allocator_.deallocate(data(), capacity_);
        }
        data_ = ptr_box<value_type>{nullptr};
        capacity_ = 0U;
    }

    allocator_type allocator_;
    ptr_box<value_type> data_{nullptr};
    size_type size_{0U};
    size_type capacity_{0U};
    bool fixed_capacity_{false};
};

template <typename T,
          typename Allocator = PolymorphicAllocator<T>,
          typename PointerPolicy = OffsetBoxPolicy>
using VectorBase = Vector<T, Allocator, PointerPolicy>;

}  // namespace score::nothrow

#endif  // SCORE_NOTHROW_VECTOR_H
