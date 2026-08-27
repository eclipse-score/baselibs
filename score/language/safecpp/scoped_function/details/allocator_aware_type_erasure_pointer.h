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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_ALLOCATOR_AWARE_TYPE_ERASURE_POINTER_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_ALLOCATOR_AWARE_TYPE_ERASURE_POINTER_H

#include "score/language/safecpp/scoped_function/details/allocator_aware_erased_type.h"
#include "score/language/safecpp/scoped_function/details/allocator_wrapper.h"

#include <score/memory.hpp>

#include <cstddef>
#include <memory>
#include <type_traits>

namespace score::safecpp::details
{

class AllocatorAwareTypeErasurePointerBase
{
  protected:
    constexpr AllocatorAwareTypeErasurePointerBase() noexcept = default;
    constexpr AllocatorAwareTypeErasurePointerBase(const AllocatorAwareTypeErasurePointerBase&) = default;
    constexpr AllocatorAwareTypeErasurePointerBase(AllocatorAwareTypeErasurePointerBase&&) noexcept = default;
    constexpr AllocatorAwareTypeErasurePointerBase& operator=(const AllocatorAwareTypeErasurePointerBase&) = default;
    constexpr AllocatorAwareTypeErasurePointerBase& operator=(AllocatorAwareTypeErasurePointerBase&&) noexcept =
        default;

  public:
    ~AllocatorAwareTypeErasurePointerBase() noexcept = default;
};

template <class Interface, class Allocator = std::allocator<std::byte>>
class AllocatorAwareTypeErasurePointer : public AllocatorAwareTypeErasurePointerBase
{
    static_assert(std::is_base_of_v<AllocatorAwareErasedType<Interface>, Interface>,
                  "Template parameter ErasedType must inherit from AllocatorAwareErasedType<Interface>");

  public:
    using element_type = std::remove_reference_t<Interface>;
    using const_element_type = std::add_const_t<element_type>;
    using pointer = std::add_pointer_t<element_type>;
    using const_pointer = std::add_pointer_t<const_element_type>;

    AllocatorAwareTypeErasurePointer() : AllocatorAwareTypeErasurePointer{Allocator{}} {}
    ~AllocatorAwareTypeErasurePointer() noexcept = default;

    // Suppress "AUTOSAR C++14 A12-1-5" rule finding. This rule states:"Common class initialization for non-constant
    // members shall be done by a delegating constructor.".
    // Rationale: This constructor should be considered as a common delegating constructor. Also no easy way to fix this
    // due to the usage of the AllocatorWrapper.
    // coverity[autosar_cpp14_a12_1_5_violation]
    explicit AllocatorAwareTypeErasurePointer(const Allocator& allocator)
        : AllocatorAwareTypeErasurePointerBase{},
          allocator_{allocator},
          erasure_pointer_{nullptr, TypeErasurePointerDeleter{allocator_.GetPolymorphicAllocator()}}
    {
    }

    template <class Implementation,
              std::enable_if_t<
                  !std::is_base_of_v<AllocatorAwareTypeErasurePointerBase, score::cpp::remove_cvref_t<Implementation>>,
                  bool> = true>
    AllocatorAwareTypeErasurePointer(std::allocator_arg_t,
                                     const Allocator& allocator,
                                     TypeErasurePointer<Implementation> impl)
        : AllocatorAwareTypeErasurePointerBase{},
          allocator_{allocator},
          // Intentionally not done with move constructor since this would lead to wrong deleter
          erasure_pointer_{impl.release(),
                           TypeErasurePointerDeleter{allocator_.template GetPolymorphicAllocator<Implementation>()}}
    {
    }

    // Suppress ""AUTOSAR C++14 A12-8-1" rule finding.
    // This rule states:"Move and copy constructors shall move and respectively copy base classes and data members of a
    // class, without any side effects.".
    // Rationale: This constructor has side effects due to complex moving logic of allocator and erasure_pointer_, which
    // are not relevant to users.
    // coverity[autosar_cpp14_a12_8_1_violation]
    AllocatorAwareTypeErasurePointer(const AllocatorAwareTypeErasurePointer& other)
        : AllocatorAwareTypeErasurePointer{std::allocator_arg, other.get_allocator(), other}
    {
    }

    // Suppress ""AUTOSAR C++14 A12-6-1" rule finding.
    // This rule states:"All class data members that are initialized by
    // the constructor shall be initialized using member initializers.".
    // Rationale: Initialization of the erasure_pointer_ member cannot be completed in the initializer list, so it is
    // defined in the constructor body depending on the "other" object.
    // Suppress ""AUTOSAR C++14 A12-8-1" rule finding.
    // This rule states:"Move and copy constructors shall move and respectively copy base classes and data members of a
    // class, without any side effects.".
    // Rationale: This constructor has side effects due to complex moving logic of allocator and erasure_pointer_, which
    // are not relevant to users.
    template <class OtherAllocator>
    // coverity[autosar_cpp14_a12_6_1_violation]
    // coverity[autosar_cpp14_a12_8_1_violation]
    AllocatorAwareTypeErasurePointer(std::allocator_arg_t,
                                     const Allocator& allocator,
                                     const AllocatorAwareTypeErasurePointer<Interface, OtherAllocator>& other)
        : AllocatorAwareTypeErasurePointer{allocator}
    {
        if (other.get() != nullptr)
        {
            erasure_pointer_ = other->Copy(allocator_.GetPolymorphicAllocator());
        }
    }

    // Suppress ""AUTOSAR C++14 A6-2-1 rule finding.
    // This rule states:"Move and copy assignment operators shall either move or respectively copy base classes and data
    // members of a class, without any side effects.".
    // Rationale: This operator has side effects due to complex copying logic of allocator and erasure_pointer_, which
    // are not relevant to users.
    // coverity[autosar_cpp14_a6_2_1_violation]
    AllocatorAwareTypeErasurePointer& operator=(const AllocatorAwareTypeErasurePointer& other)
    {
        if (this != &other)
        {
            erasure_pointer_.reset();

            constexpr bool allocator_propagates =
                std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value;
            // coverity[autosar_cpp14_m6_4_1_violation: FALSE]: "if constexpr" is a valid statement since C++17.
            // coverity[autosar_cpp14_a7_1_8_violation: FALSE]: "if constexpr" is a valid statement since C++17.
            if constexpr (allocator_propagates)
            {
                const bool allocator_different = get_allocator() != other.get_allocator();
                if (allocator_different)
                {
                    allocator_ = other.allocator_;
                }
            }

            if (other.erasure_pointer_ != nullptr)
            {
                erasure_pointer_ = other.erasure_pointer_->Copy(allocator_.GetPolymorphicAllocator());
            }
        }

        return *this;
    }

    // Suppress ""AUTOSAR C++14 A12-8-1" rule finding.
    // This rule states:"Move and copy constructors shall move and respectively copy base classes and data members of a
    // class, without any side effects.".
    // Rationale: This constructor has side effects due to complex moving logic of allocator and erasure_pointer_, which
    // are not relevant to users.
    // coverity[autosar_cpp14_a12_8_1_violation]
    AllocatorAwareTypeErasurePointer(AllocatorAwareTypeErasurePointer&& other) noexcept
        : AllocatorAwareTypeErasurePointer{std::allocator_arg, other.get_allocator(), std::move(other)}
    {
    }

    // Suppress ""AUTOSAR C++14 A12-6-1" rule finding.
    // This rule states:"All class data members that are initialized by
    // the constructor shall be initialized using member initializers.".
    // Rationale: Initialization of the erasure_pointer_ member cannot be completed in the initializer list, so it is
    // defined in the constructor body depending on the "other" object.
    // Suppress ""AUTOSAR C++14 A12-8-1" rule finding.
    // This rule states:"Move and copy constructors shall move and respectively copy base classes and data members of a
    // class, without any side effects.".
    // Rationale: This constructor has side effects due to complex moving logic of allocator and erasure_pointer_, which
    // are not relevant to users.
    // coverity[autosar_cpp14_a12_6_1_violation]
    // coverity[autosar_cpp14_a12_8_1_violation]
    AllocatorAwareTypeErasurePointer(std::allocator_arg_t,
                                     const Allocator& allocator,
                                     AllocatorAwareTypeErasurePointer<Interface, Allocator>&& other) noexcept
        : AllocatorAwareTypeErasurePointer{allocator}
    {
        if (other.get_allocator() != allocator)
        {
            if (other.get() != nullptr)
            {
                erasure_pointer_ = other->Move(allocator_.GetPolymorphicAllocator());
            }
        }
        else
        {
            erasure_pointer_ = std::move(other.erasure_pointer_);
        }
    }

    // Suppress ""AUTOSAR C++14 A12-6-1" rule finding.
    // This rule states:"All class data members that are initialized by
    // the constructor shall be initialized using member initializers.".
    // Rationale: Initialization of the erasure_pointer_ member cannot be completed in the initializer list, so it is
    // defined in the constructor body depending on the "other" object.
    // Suppress ""AUTOSAR C++14 A12-8-1" rule finding.
    // This rule states:"Move and copy constructors shall move and respectively copy base classes and data members of a
    // class, without any side effects.".
    // Rationale: This constructor has side effects due to complex moving logic of allocator and erasure_pointer_, which
    // are not relevant to users.
    template <class OtherAllocator>
    // coverity[autosar_cpp14_a12_6_1_violation]
    // coverity[autosar_cpp14_a12_8_1_violation]
    AllocatorAwareTypeErasurePointer(std::allocator_arg_t,
                                     const Allocator& allocator,
                                     AllocatorAwareTypeErasurePointer<Interface, OtherAllocator>&& other) noexcept
        : AllocatorAwareTypeErasurePointer{allocator}
    {
        if (other.get() != nullptr)
        {
            erasure_pointer_ = other->Move(allocator_.GetPolymorphicAllocator());
        }
    }

    // Suppress ""AUTOSAR C++14 A6-2-1" rule finding.
    // This rule states:"Move and copy assignment operators shall either move or respectively copy base classes and data
    // members of a class, without any side effects.".
    // Rationale: This operator has side effects due to complex moving logic of allocator and erasure_pointer_, which
    // are not relevant to users.
    // coverity[autosar_cpp14_a6_2_1_violation]
    AllocatorAwareTypeErasurePointer& operator=(AllocatorAwareTypeErasurePointer&& other) noexcept
    {
        if (this != &other)
        {
            erasure_pointer_.reset();

            const bool allocator_different = get_allocator() != other.get_allocator();
            constexpr bool allocator_propagates =
                std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value;

            if (allocator_different)
            {
                if (allocator_propagates)
                {
                    allocator_ = std::move(other.allocator_);
                }

                if (other.get() != nullptr)
                {
                    erasure_pointer_ = other->Move(allocator_.GetPolymorphicAllocator());
                }
            }
            else
            {
                erasure_pointer_ = std::move(other.erasure_pointer_);
            }
        }

        return *this;
    }

    Allocator get_allocator() const noexcept
    {
        return *allocator_;
    }

    const_pointer get() const noexcept
    {
        return static_cast<const_pointer>(erasure_pointer_.get());
    }

    pointer get() noexcept
    {
        return erasure_pointer_.get();
    }

    std::add_lvalue_reference_t<const_element_type> operator*() const
    {
        return erasure_pointer_.operator*();
    }

    std::add_lvalue_reference_t<element_type> operator*()
    {
        return erasure_pointer_.operator*();
    }

    const_pointer operator->() const noexcept
    {
        return static_cast<const_pointer>(erasure_pointer_.operator->());
    }

    pointer operator->() noexcept
    {
        return erasure_pointer_.operator->();
    }

    template <class InterfaceRhs, class AllocatorRhs>
    // Suppress "AUTOSAR C++14 A13-5-5" rule finding. This rule states:"Comparison operators shall be non-member
    // functions with identical parameter types and noexcept. Rationale: Non-identical parameter types are required for
    // the comparison with different allocator types.
    // Suppress "AUTOSAR C++14 A3-3-1" rule finding. This rule states:"Objects or functions with external linkage
    // (including members of named namespaces) shall be declared in a header file."
    // Rationale: This function is declared in a header file
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a3_3_1_violation]
    friend bool operator==(const AllocatorAwareTypeErasurePointer<Interface, Allocator>& lhs,
                           const AllocatorAwareTypeErasurePointer<InterfaceRhs, AllocatorRhs>& rhs) noexcept
    {
        return (lhs.get_allocator() == rhs.get_allocator()) && (lhs.get() == rhs.get());
    }

    template <class InterfaceRhs, class AllocatorRhs>
    // Suppress "AUTOSAR C++14 A13-5-5" rule finding. This rule states:"Comparison operators shall be non-member
    // functions with identical parameter types and noexcept. Rationale: Non-identical parameter types are required for
    // the comparison with nullptr_t.
    // Suppress "AUTOSAR C++14 A3-3-1" rule finding. This rule states:"Objects or functions with external linkage
    // (including members of named namespaces) shall be declared in a header file."
    // Rationale: This function is declared in a header file
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a3_3_1_violation]
    friend bool operator!=(const AllocatorAwareTypeErasurePointer<Interface, Allocator>& lhs,
                           const AllocatorAwareTypeErasurePointer<InterfaceRhs, AllocatorRhs>& rhs) noexcept
    {
        return (lhs.get_allocator() != rhs.get_allocator()) && (lhs.get() != rhs.get());
    }

    // Suppress "AUTOSAR C++14 A13-5-5" rule finding. This rule states:"Comparison operators shall be non-member
    // functions with identical parameter types and noexcept. Rationale: Non-identical parameter types are required for
    // the comparison with nullptr_t.
    // Suppress "AUTOSAR C++14 A3-3-1" rule finding. This rule states:"Objects or functions with external linkage
    // (including members of named namespaces) shall be declared in a header file."
    // Rationale: This function is declared in a header file
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a3_3_1_violation]
    friend bool operator==(const AllocatorAwareTypeErasurePointer<Interface, Allocator>& erasure,
                           const std::nullptr_t) noexcept
    {
        return erasure.erasure_pointer_ == nullptr;
    }

    // Suppress "AUTOSAR C++14 A13-5-5" rule finding. This rule states:"Comparison operators shall be non-member
    // functions with identical parameter types and noexcept. Rationale: Non-identical parameter types are required for
    // the comparison with nullptr_t.
    // Suppress "AUTOSAR C++14 A3-3-1" rule finding. This rule states:"Objects or functions with external linkage
    // (including members of named namespaces) shall be declared in a header file."
    // Rationale: This function is declared in a header file
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a3_3_1_violation]
    friend bool operator!=(const AllocatorAwareTypeErasurePointer<Interface, Allocator>& erasure,
                           const std::nullptr_t) noexcept
    {
        return erasure.erasure_pointer_ != nullptr;
    }

  private:
    AllocatorWrapper<Allocator> allocator_;
    TypeErasurePointer<element_type> erasure_pointer_;
};

template <class Interface, class Implementation, class Allocator, class... Args>
auto MakeAllocatorAwareTypeErasurePointer(const Allocator& allocator, Args&&... args)
    -> AllocatorAwareTypeErasurePointer<Interface, Allocator>
{
    static_assert(std::is_base_of_v<AllocatorAwareErasedType<Interface>, Interface>,
                  "Interface must inherit from AllocatorAwareErasedType<Interface>");
    static_assert(std::is_base_of_v<Interface, Implementation>, "Implementation must inherit from Interface");
    static_assert(std::is_constructible_v<Implementation, Args...>, "Implementation must be constructible with Args");

    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(
        allocator == Allocator{allocator},
        "Copies of the allocator must be able to deallocate previously allocated memory");

    score::cpp::pmr::resource_adaptor<Allocator> resource_adaptor{allocator};
    score::cpp::pmr::polymorphic_allocator<Implementation> polymorphic_allocator{&resource_adaptor};
    auto pointer = MakeTypeErasurePointer<Implementation>(polymorphic_allocator, std::forward<Args>(args)...);

    return AllocatorAwareTypeErasurePointer<Interface, Allocator>{std::allocator_arg, allocator, std::move(pointer)};
}

}  // namespace score::safecpp::details

namespace std
{

template <class Interface, class Alloc>
// Suppress "AUTOSAR C++14 A17-6-1" rule finding.
// This rule states:"Non-standard entities shall not be added to standard namespaces.".
// Rationale: This is a standard way to add a template specialization for a standard template.
// coverity[autosar_cpp14_a17_6_1_violation]
struct uses_allocator<score::safecpp::details::AllocatorAwareTypeErasurePointer<Interface, Alloc>, Alloc>
    : std::true_type
{
};

}  // namespace std

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_ALLOCATOR_AWARE_TYPE_ERASURE_POINTER_H
