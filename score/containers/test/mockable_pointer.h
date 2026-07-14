/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_CONTAINERS_TEST_MOCKABLE_POINTER_H
#define SCORE_LIB_CONTAINERS_TEST_MOCKABLE_POINTER_H

#include "score/containers/test/pointer_spy_mock.h"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace score::containers::test
{

/// \brief A fancy-pointer wrapper around a raw T* that supports optional spy-based call recording.
///
/// By default, MockablePointer behaves identically to a raw pointer. All arithmetic, compare and dereference
/// operations are forwarded to the underlying raw pointer.
///
/// When a PointerSpyMock is registered via SetMock(), each operation additionally invokes the
/// corresponding spy method (for EXPECT_CALL verification), but the real pointer behavior is
/// always preserved. This enables "spy" testing: verify that specific pointer operations are
/// called (e.g. during NonRelocatableVector::data()) without having to mock the entire lifecycle.
///
/// Usage: See example/usage in MockableAllocator, which uses MockablePointer as its pointer type.
///
/// Or use the RAII guard MockablePointerMockGuard for exception-safe mock lifetime management.
template <typename T>
class MockablePointer
{
  public:
    // Iterator traits (required for std::advance, std::next, std::iterator_traits)
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using element_type = T;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using pointer = T*;

    /// \brief Register a spy mock. While set, all pointer operations will record calls to the spy.
    static void SetMock(PointerSpyMock<T>* mock) noexcept
    {
        mock_ = mock;
    }

    /// \brief Clear the spy mock. Operations revert to pure pointer behavior (no recording).
    static void ClearMock() noexcept
    {
        mock_ = nullptr;
    }

    // --- Constructors ---

    MockablePointer() noexcept : ptr_{nullptr} {}

    // NOLINTNEXTLINE(google-explicit-constructor) - intentional implicit conversion from nullptr
    MockablePointer(std::nullptr_t) noexcept : ptr_{nullptr} {}

    explicit MockablePointer(T* ptr) noexcept : ptr_{ptr} {}

    // --- Assignment ---

    MockablePointer& operator=(std::nullptr_t) noexcept
    {
        ptr_ = nullptr;
        return *this;
    }

    // --- Raw pointer access ---

    /// \brief Get the underlying raw pointer.
    T* get() const noexcept
    {
        return ptr_;
    }

    // --- Dereference operations ---

    reference operator*() const
    {
        if (mock_ != nullptr)
        {
            mock_->Dereference();
        }
        return *ptr_;
    }

    pointer operator->() const
    {
        if (mock_ != nullptr)
        {
            mock_->Arrow(ptr_);
        }
        return ptr_;
    }

    reference operator[](difference_type idx) const
    {
        if (mock_ != nullptr)
        {
            mock_->Index(idx);
        }
        return ptr_[idx];
    }

    // --- Arithmetic operations ---

    MockablePointer& operator+=(difference_type offset)
    {
        if (mock_ != nullptr)
        {
            mock_->PlusAssign(offset);
        }
        ptr_ += offset;
        return *this;
    }

    MockablePointer& operator-=(difference_type offset)
    {
        if (mock_ != nullptr)
        {
            mock_->MinusAssign(offset);
        }
        ptr_ -= offset;
        return *this;
    }

    MockablePointer& operator++()
    {
        if (mock_ != nullptr)
        {
            mock_->PreIncrement();
        }
        ++ptr_;
        return *this;
    }

    MockablePointer operator++(int)
    {
        if (mock_ != nullptr)
        {
            mock_->PostIncrement();
        }
        MockablePointer tmp = *this;
        ++ptr_;
        return tmp;
    }

    MockablePointer& operator--()
    {
        if (mock_ != nullptr)
        {
            mock_->PreDecrement();
        }
        --ptr_;
        return *this;
    }

    MockablePointer operator--(int)
    {
        if (mock_ != nullptr)
        {
            mock_->PostDecrement();
        }
        MockablePointer tmp = *this;
        --ptr_;
        return tmp;
    }

    // --- Comparison operators ---

    bool operator==(const MockablePointer& other) const noexcept
    {
        return ptr_ == other.ptr_;
    }

    bool operator!=(const MockablePointer& other) const noexcept
    {
        return ptr_ != other.ptr_;
    }

    bool operator==(std::nullptr_t) const noexcept
    {
        return ptr_ == nullptr;
    }

    bool operator!=(std::nullptr_t) const noexcept
    {
        return ptr_ != nullptr;
    }

    friend bool operator==(std::nullptr_t, const MockablePointer& rhs) noexcept
    {
        return rhs.ptr_ == nullptr;
    }

    friend bool operator!=(std::nullptr_t, const MockablePointer& rhs) noexcept
    {
        return rhs.ptr_ != nullptr;
    }

    // --- Difference ---

    friend difference_type operator-(const MockablePointer& lhs, const MockablePointer& rhs) noexcept
    {
        return lhs.ptr_ - rhs.ptr_;
    }

    // --- pointer_traits support ---

    static MockablePointer pointer_to(element_type& e) noexcept
    {
        return MockablePointer(&e);
    }

  private:
    T* ptr_;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) - intentional static for test spy pattern
    static inline PointerSpyMock<T>* mock_ = nullptr;
};



}  // namespace score::containers::test

#endif  // SCORE_LIB_CONTAINERS_TEST_MOCKABLE_POINTER_H

