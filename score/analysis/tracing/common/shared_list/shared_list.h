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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_SHARED_LIST_H
#define SCORE_ANALYSIS_TRACING_COMMON_SHARED_LIST_H
#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_interface.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/error_code/error_code.h"
#include "score/memory/shared/atomic_indirector.h"
#include <cstdint>
#include <list>
#include <type_traits>

namespace score
{
namespace analysis
{
namespace tracing
{

// Forward declaration for type trait
class SharedMemoryChunk;

namespace shared
{

/**
 * @brief A custom doubly linked list implementation that uses flexible memory allocator.
 *
 * @tparam T The type of the elements stored in the list.
 */
template <typename T, template <class> class AtomicIndirectorType = memory::shared::AtomicIndirectorReal>
class alignas(std::max_align_t) List
{
  private:
    /**
     * @brief Represents a node in the doubly linked list.
     */
    class alignas(std::max_align_t) Node
    {
      public:
        //  coverity[autosar_cpp14_m11_0_1_violation] the whole data type is declared and encapsulated private
        T data;  ///< Data stored in the node.
        //  coverity[autosar_cpp14_m11_0_1_violation] the whole data type is declared and encapsulated private
        std::ptrdiff_t prev;  ///< Offset to the previous node.
        //  coverity[autosar_cpp14_m11_0_1_violation] the whole data type is declared and encapsulated private
        std::ptrdiff_t next;  ///< Offset to the next node.
        /**
         * @brief Constructs a Node with the given value.
         *
         * @param value The value to store in the node.
         */
        explicit Node(const T& value) : data(value), prev(0), next(0) {}
    };

    /**
     * @brief Resolves an offset to a pointer to a Node.
     *
     * @param offset The offset of the node.
     * @return Pointer to the Node at the given offset, or nullptr if the offset is zero.
     */
    Node* ResolveOffset(std::ptrdiff_t offset) const
    {
        // Usage of reinterpret cast is intended here and as per our use case, type safety is not violated
        // As the memory is accessible only within the usage of this library which ensures that this memory
        // would be always translated in the same way.
        // Also pointer arithmetic is needed here to do the offset operation
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): Tolerated see above
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic): Tolerated see above
        return offset == 0
                   ? nullptr
                   //  coverity[autosar_cpp14_a5_2_4_violation]
                   //  coverity[autosar_cpp14_a5_2_3_violation]
                   //  coverity[autosar_cpp14_m5_0_15_violation]
                   : reinterpret_cast<Node*>(reinterpret_cast<std::uint8_t*>(const_cast<List<T>*>(this)) + offset);
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic): Tolerated see above
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast): Tolerated see above
    }

    /**
     * @brief Calculates the offset of a given Node pointer.
     *
     * @param node_address Pointer to the node.
     * @return The offset of the node from the base address.
     */
    std::ptrdiff_t CalculateOffset(Node* node_address) const
    {
        // Usage of reinterpret cast is intended here and as per our use case, type safety is not violated
        // As the memory is accessible only within the usage of this library which ensures that this memory
        // would be always translated in the same way
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): Tolerated see above
        return node_address == nullptr ? 0
                                       //  coverity[autosar_cpp14_a5_2_4_violation]
                                       //  coverity[autosar_cpp14_m5_0_15_violation]
                                       : reinterpret_cast<std::uint8_t*>(node_address) -
                                             //  coverity[autosar_cpp14_a5_2_3_violation]
                                             //  coverity[autosar_cpp14_a5_2_4_violation]
                                             //  coverity[autosar_cpp14_m5_0_15_violation]
                                             reinterpret_cast<std::uint8_t*>(const_cast<List<T>*>(this));
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast): Tolerated see above
    }

    /**
     * @brief Allocates memory for a new Node and constructs it with the given value.
     *
     * @param value The value to store in the new node.
     * @return Pointer to the newly allocated Node, or nullptr if allocation fails.
     */
    score::Result<Node*> AllocateNewNode(const T& value)
    {
        auto node_address_result = flexible_allocator_->Allocate(sizeof(Node), alignof(Node));
        if ((node_address_result.has_value()) && (nullptr != node_address_result.value()))
        {
            // Usage of placement new is intended here and safe as we explicitly construct the node
            // in the pre-allocated memory provided by the flexible allocator
            // NOLINTBEGIN(score-no-dynamic-raw-memory): Tolerated see above
            //  coverity[autosar_cpp14_a18_5_10_violation]
            auto node = new (node_address_result.value()) Node(value);
            // NOLINTEND(score-no-dynamic-raw-memory): Tolerated see above
            return node;
        }
        return score::MakeUnexpected(ErrorCode::kNoSpaceLeftForAllocationRecoverable);
    }

    /**
     * @brief Deallocates the memory of a given Node.
     *
     * @param node Pointer to the Node to deallocate.
     */
    void DeallocateNode(Node* node)
    {
        // when ResolveOffset(head_offset_) returns nullptr initially, the loop won't execute, and
        // DeallocateNode won't be called so this condition always evaluate to true.
        if (node != nullptr)  // LCOV_EXCL_BR_LINE not testable see comment above.
        {
            if (flexible_allocator_)  // LCOV_EXCL_BR_LINE not testable see comment above.
            {
                score::cpp::ignore = flexible_allocator_->Deallocate(static_cast<void*>(node), sizeof(Node));
            }
        }
    }
    std::uint64_t canary_start_ = kCanaryStart;                       ///< Start canary.
    std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator_;  ///< Allocator for shared memory.
    std::atomic<std::ptrdiff_t> head_offset_;                         ///< Offset of the head node.
    std::atomic<std::ptrdiff_t> tail_offset_;                         ///< Offset of the tail node.
    std::atomic_size_t size_;
    std::uint64_t canary_end_ = kCanaryEnd;

  public:
    static constexpr std::uint64_t kCanaryStart = 0xDEADBEEFCAFEBABEULL;
    static constexpr std::uint64_t kCanaryEnd = 0xBABECAFEBEEFDEADULL;

    /**
     * @brief Validate list integrity by checking magic numbers
     * @return true if valid, false if corrupted
     */
    bool IsValid() const noexcept
    {
        if (canary_start_ != kCanaryStart)
        {
            return false;
        }
        if (canary_end_ != kCanaryEnd)
        {
            return false;
        }
        return true;
    }

    /**
     * @brief Custom Iterator class for the List.
     */
    class iterator
    {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        /**
         * @brief Constructs an iterator.
         *
         * @param parent_list Pointer to the parent List.
         * @param node Pointer to the current Node.
         */
        iterator(List* parent_list, Node* node) : parent_(parent_list), current_(node) {}

        /**
         * @brief Dereference operator to access the data of the current node.
         *
         * @note This operation has potential risk with functions like std::copy in case that there is no enough memory
         * for allocation
         * @return Reference to the data of the current node.
         */
        const T operator*()
        {
            if (current_ == nullptr)
            {
                // Allocate a new/empty node dynamically in the shared memory to avoid crashes
                auto result = parent_->AllocateNewNode(T{});
                if (result.has_value())
                {
                    current_ = result.value();
                }
                else
                {
                    // should not be real usecase
                    return T{};
                }
            }
            return current_->data;
        }

        /**
         * @brief Arrow operator to access the data of the current node.
         *
         * @return Pointer to the data of the current node.
         */
        pointer operator->()
        {
            // Suppress "AUTOSAR C++14 A9-3-1" rule finding. This rule states:"Member functions shall not return
            // non-const “raw” pointers or references to private or protected data owned by the class.".
            // Justification: As with a normal container, an underlying item should be accessible as a non-const
            // reference from outside by index. The result reference remains valid as long as the SharedList instance is
            // alive.
            // coverity[autosar_cpp14_a9_3_1_violation] see above
            return &(current_->data);
        }
        // Increment and Decrement
        iterator& operator++()
        {
            if (current_ != nullptr)
            {
                current_ = parent_->ResolveOffset(current_->next);
            }
            return *this;
        }
        iterator operator++(int)
        {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator& operator--()
        {
            if (current_)
            {
                current_ = parent_->ResolveOffset(current_->prev);
            }
            return *this;
        }
        iterator operator--(int)
        {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        // Define the non-member comparison operators
        // Adhere to the One Definition Rule (ODR) permits multiple definitions across translation units.
        // coverity[autosar_cpp14_a3_3_1_violation: FALSE]
        friend inline bool operator==(const iterator& lhs, const iterator& rhs) noexcept
        {
            return lhs.current_ == rhs.current_;
        }
        // Adhere to the One Definition Rule (ODR) permits multiple definitions across translation units.
        // coverity[autosar_cpp14_a3_3_1_violation: FALSE]
        friend inline bool operator!=(const iterator& lhs, const iterator& rhs) noexcept
        {
            return lhs.current_ != rhs.current_;
        }

      private:
        List* parent_;   ///< Pointer to the parent List.
        Node* current_;  ///< Pointer to the current Node.
    };

    /**
     * @brief Primary constructor for initializing the List.
     *
     * @param flexible_allocator Shared pointer reference to a flexible circular allocator.
     * @param head_offset Offset for the head of the list.
     * @param tail_offset Offset for the tail of the list.
     * @param size Initial size of the list.
     */
    explicit List(std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator,
                  std::ptrdiff_t head_offset,
                  std::ptrdiff_t tail_offset,
                  std::size_t size)
        : canary_start_(kCanaryStart),
          flexible_allocator_(flexible_allocator),
          head_offset_(head_offset),
          tail_offset_(tail_offset),
          size_(size),
          canary_end_(kCanaryEnd)
    {
    }

    /**
     * @brief Constructs an empty List with no allocator provided.
     *
     * @param flexible_allocator Shared pointer reference to a flexible circular allocator.
     */
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init) clangtidy false positive, delegating ctor not recognized
    List() : List(nullptr, 0, 0, 0U) {}

    /**
     * @brief Constructs an empty List with the specified allocator.
     *
     * @param flexible_allocator Shared pointer reference to a flexible circular allocator.
     */
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init) clangtidy false positive, delegating ctor not recognized
    explicit List(std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator) : List(flexible_allocator, 0, 0, 0U)
    {
    }

    /**
     * @brief Destroys the List and releases all resources.
     */
    ~List()
    {
        clear();
    }

    /**
     * @brief Deleted copy constructor.
     *
     * The copy constructor is deleted to prevent copying of the `List` object.
     */
    List(const List&) = delete;

    /**
     * @brief Deleted copy assignment operator.
     *
     * The copy assignment operator is deleted to prevent assignment between `List` objects.
     */
    List& operator=(const List&) = delete;

    /**
     * @brief Deleted move constructor.
     *
     * The move constructor is deleted to prevent moving of the `List` object.
     */
    List(List&&) = delete;

    /**
     * @brief Deleted move assignment operator.
     *
     * The move assignment operator is deleted to prevent move assignment between `List` objects.
     */
    List& operator=(List&&) = delete;

    /**
     * @brief Gets the size of the List.
     *
     * @return The number of elements in the List.
     */
    std::size_t size() const
    {
        return AtomicIndirectorType<std::size_t>::load(size_);
    }

    /**
     * @brief Checks if the List is empty.
     *
     * @return True if the List is empty, false otherwise.
     */
    bool empty() const
    {
        return AtomicIndirectorType<std::size_t>::load(size_) == 0U;
    }

    /**
     * @brief Clears all elements from the List.
     */
    void clear()
    {
        // Validate list integrity before attempting to clear
        // If list is corrupted, skip deallocation to prevent crash
        if (!IsValid())
        {
            // List is corrupted - just reset pointers without deallocating
            AtomicIndirectorType<std::ptrdiff_t>::store(tail_offset_, 0U);
            AtomicIndirectorType<std::ptrdiff_t>::store(head_offset_, 0U);
            AtomicIndirectorType<std::size_t>::store(size_, 0U);
            return;
        }

        while (ResolveOffset(AtomicIndirectorType<std::ptrdiff_t>::load(head_offset_)) != nullptr)
        {
            Node* temp = ResolveOffset(AtomicIndirectorType<std::ptrdiff_t>::load(head_offset_));
            AtomicIndirectorType<std::ptrdiff_t>::store(
                head_offset_, ResolveOffset(AtomicIndirectorType<std::ptrdiff_t>::load(head_offset_))->next);
            DeallocateNode(temp);
        }
        flexible_allocator_.reset();
        AtomicIndirectorType<std::ptrdiff_t>::store(tail_offset_, 0U);
        AtomicIndirectorType<std::ptrdiff_t>::store(head_offset_, 0U);
        AtomicIndirectorType<std::size_t>::store(size_, 0U);
    }

    /**
     * @brief Adds an element to the end of the List.
     *
     * @param value The value to add.
     * @return A result indicating success or failure.
     */
    score::Result<Blank> push_back(const T& value)
    {
        auto new_node = AllocateNewNode(value);
        if (!new_node.has_value())
        {
            return score::MakeUnexpected<Blank>(new_node.error());
        }
        if (nullptr == ResolveOffset(AtomicIndirectorType<std::ptrdiff_t>::load(tail_offset_)))
        {
            AtomicIndirectorType<std::ptrdiff_t>::store(tail_offset_, CalculateOffset(new_node.value()));

            AtomicIndirectorType<std::ptrdiff_t>::store(head_offset_,
                                                        AtomicIndirectorType<std::ptrdiff_t>::load(tail_offset_));
        }
        else
        {
            ResolveOffset(AtomicIndirectorType<std::ptrdiff_t>::load(tail_offset_))->next =
                CalculateOffset(new_node.value());
            new_node.value()->prev = AtomicIndirectorType<std::ptrdiff_t>::load(tail_offset_);
            AtomicIndirectorType<std::ptrdiff_t>::store(tail_offset_, CalculateOffset(new_node.value()));
        }
        std::ignore = AtomicIndirectorType<std::size_t>::fetch_add(size_, 1UL);
        return score::cpp::blank{};
    }

    /**
     * @brief Constructs and adds an element to the end of the List.
     *
     * @tparam Args The types of the arguments for constructing the element.
     * @param args The arguments to forward to the constructor of the element.
     * @return A result indicating success or failure.
     */
    template <typename... Args>
    score::Result<Blank> emplace_back(Args&&... args)
    {
        T value(std::forward<Args>(args)...);
        return push_back(value);
    }

    /**
     * @brief Accesses the element at a specific index.
     *
     * @param index The index of the element.
     * @return The element at the specified index, or an error if out of range.
     */
    score::Result<T> at(std::size_t index)
    {
        if (!IsValid())
        {
            return score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal);
        }

        if (index >= AtomicIndirectorType<std::size_t>::load(size_))
        {
            return score::MakeUnexpected(ErrorCode::kIndexOutOfBoundsInSharedListRecoverable);
        }

        Node* current = ResolveOffset(AtomicIndirectorType<std::ptrdiff_t>::load(head_offset_));

        if (!flexible_allocator_->IsInBounds(current, sizeof(Node)))
        {
            return score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal);
        }

        for (std::size_t i = 0U; i < index; ++i)
        {
            // No need to check against nullptr in the following line due to the check against the size
            // otherwise, it yields to unreachable code
            // coverity[autosar_cpp14_a5_3_2_violation] See above
            current = ResolveOffset(current->next);
        }

        // Additional safety check: verify current is valid before dereferencing
        if (current == nullptr)
        {
            return score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal);
        }

        if constexpr (std::is_same<T, SharedMemoryChunk>::value)
        {
            if (current->data.IsCorrupted())
            {
                return score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal);
            }
        }

        // coverity[autosar_cpp14_a5_3_2_violation] See above
        return current->data;
    }

    /**
     * @brief Gets an iterator to the beginning of the List.
     *
     * @return Iterator to the first element.
     */
    iterator begin()
    {
        return iterator(this, ResolveOffset(AtomicIndirectorType<std::ptrdiff_t>::load(head_offset_)));
    }

    /**
     * @brief Gets an iterator to the end of the List.
     *
     * @return Iterator to one past the last element.
     */
    iterator end()
    {
        return iterator(this, nullptr);
    }
};

}  // namespace shared
}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_SHARED_LIST_H
