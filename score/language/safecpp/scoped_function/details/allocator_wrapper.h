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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_ALLOCATOR_WRAPPER_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_ALLOCATOR_WRAPPER_H

#include <score/memory.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace score::safecpp::details
{

template <class Allocator>
class AllocatorWrapper
{
  private:
    class AllocatorContainer
    {
      public:
        explicit AllocatorContainer(const Allocator& allocator) : allocator_{allocator}, resource_adaptor_{allocator_}
        {
        }

        [[nodiscard]] std::reference_wrapper<Allocator> Ref() noexcept
        {
            return std::ref(allocator_);
        }

        template <class T = typename Allocator::value_type>
        [[nodiscard]] score::cpp::pmr::polymorphic_allocator<T> GetPolymorphicAllocator() noexcept
        {
            return score::cpp::pmr::polymorphic_allocator<T>{&(resource_adaptor_)};
        }

      private:
        Allocator allocator_;
        score::cpp::pmr::resource_adaptor<Allocator> resource_adaptor_;
    };

  public:
    explicit AllocatorWrapper(const Allocator& allocator)
        : allocator_container_memory_{},
          // Usage of placement new is intended here and safe as the allocator container is constructed
          // in the pre-allocated memory provided by the allocator wrapper
          // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Tolerated see above
          allocator_container_{::new (&allocator_container_memory_) AllocatorContainer{allocator}}
    {
    }

    // Suppress "AUTOSAR C++14 A12-8-1" rule finding. This rule states:"Move and copy constructors shall move and
    // respectively copy base classes and data members of a class, without any side effects.".
    // Rationale: Tolerated, the copy constructor copies memory allocator (AllocatorContainer), but this copy recreated
    // in the pre-allocated memory
    // coverity[autosar_cpp14_a12_8_1_violation]
    AllocatorWrapper(const AllocatorWrapper& other)
        : allocator_container_memory_{},
          // Usage of placement new is intended here and safe as the allocator container is constructed
          // in the pre-allocated memory provided by the allocator wrapper
          // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Tolerated see above
          allocator_container_{::new (&allocator_container_memory_) AllocatorContainer{*other}}
    {
    }

    AllocatorWrapper& operator=(const AllocatorWrapper& other)
    {
        if (this != &other)
        {
            allocator_container_->~AllocatorContainer();
            // Usage of placement new is intended here and safe as the allocator container is constructed
            // in the pre-allocated memory provided by the allocator wrapper
            // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Tolerated see above
            allocator_container_ = ::new (&allocator_container_memory_) AllocatorContainer{*other};
        }
        return *this;
    }

    // coverity[autosar_cpp14_a12_8_1_violation] : Tolerated, see above for copy constructor
    AllocatorWrapper(AllocatorWrapper&& other) noexcept
        // Suppress "AUTOSAR C++14 A12-8-4" rule finding. This rule states:"Move constructor shall not initialize its
        // class members and base classes using copy semantics."
        // Rationale: No make sense to move array of bytes, so new object is created.
        // coverity[autosar_cpp14_a12_8_4_violation]
        : allocator_container_memory_{},
          // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Tolerated, see copy constructor
          allocator_container_{::new (&allocator_container_memory_) AllocatorContainer{*other}}
    {
    }

    AllocatorWrapper& operator=(AllocatorWrapper&& other) noexcept
    {
        if (this != &other)
        {
            allocator_container_->~AllocatorContainer();
            // Usage of placement new is intended here and safe as the allocator container is constructed
            // in the pre-allocated memory provided by the allocator wrapper
            // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Tolerated see above
            allocator_container_ = ::new (&allocator_container_memory_) AllocatorContainer{*other};
        }
        return *this;
    }

    ~AllocatorWrapper()
    {
        allocator_container_->~AllocatorContainer();
    }

    [[nodiscard]] std::add_lvalue_reference_t<Allocator> operator*() const
    {
        return allocator_container_->Ref();
    }

    [[nodiscard]] Allocator* operator->() const noexcept
    {
        return std::addressof(allocator_container_->Ref().get());
    }

    template <class T = typename Allocator::value_type>
    [[nodiscard]] score::cpp::pmr::polymorphic_allocator<T> GetPolymorphicAllocator() const noexcept
    {
        return allocator_container_->template GetPolymorphicAllocator<T>();
    }

    // Suppress "AUTOSAR C++14 A13-5-5" rule finding. This rule states:"Comparison operators shall be non-member
    // functions with identical parameter types and noexcept."
    // Suppress "AUTOSAR C++14 A3-3-1" rule finding. This rule states:"Objects or functions with external linkage
    // (including members of named namespaces) shall be declared in a header file."
    // Rationale: This function is declared in a header file
    // coverity[autosar_cpp14_a13_5_5_violation: FALSE]: This is a friend function, not a member function
    // coverity[autosar_cpp14_a3_3_1_violation]
    [[nodiscard]] friend bool operator==(const AllocatorWrapper& lhs, const AllocatorWrapper& rhs)
    {
        return *lhs == *rhs;
    }

  private:
    alignas(AllocatorContainer) std::array<std::byte, sizeof(AllocatorContainer)> allocator_container_memory_;
    AllocatorContainer* allocator_container_;
};

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_ALLOCATOR_WRAPPER_H
