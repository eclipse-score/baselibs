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
#ifndef SCORE_SHM_MAP_H
#define SCORE_SHM_MAP_H

#include "score/shm/container_error.h"
#include "score/shm/memory_resource.h"
#include "score/shm/offset_ptr.h"

#include "score/result/result.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace score::shm
{

/// @brief Shared-memory-safe counterpart to std::map.
///
/// Follows std::map semantics with these deviations:
/// - All member functions are noexcept.
/// - Bounds-checking member at() aborts on precondition violation instead of throwing.
/// - Members that may allocate (Create from ranges/lists, Insert, Emplace, GetOrInsertDefault, Clone)
///   return score::Result with kOutOfMemory on failure. Each has an OrAbort convenience variant.
/// - Not copyable. Use Clone() for explicit deep copies.
/// - Tree links are injected via PointerPolicy::NullablePtr (default: score::shm::NullableOffsetPtr).
///
/// The tree is AVL-balanced to provide O(log n) insert/find/erase in the worst case.
template <typename Key,
          typename T,
          typename Compare = std::less<Key>,
          typename Allocator = PolymorphicAllocator<std::pair<const Key, T>>,
          typename PointerPolicy = ShmPointerPolicy>
class Map
{
    struct Node;

  public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const key_type, mapped_type>;
    using key_compare = Compare;
    using allocator_type = Allocator;
    using pointer_policy = PointerPolicy;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    class const_iterator;

    class iterator
    {
        friend class Map;
        friend class const_iterator;

      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Map::value_type;
        using difference_type = Map::difference_type;
        using pointer = value_type*;
        using reference = value_type&;

        iterator() noexcept = default;

        reference operator*() const noexcept
        {
            if (node_ == nullptr)
            {
                std::abort();
            }
            return node_->value;
        }

        pointer operator->() const noexcept
        {
            if (node_ == nullptr)
            {
                std::abort();
            }
            return &node_->value;
        }

        iterator& operator++() noexcept
        {
            if ((owner_ == nullptr) || (node_ == nullptr))
            {
                std::abort();
            }

            node_ = owner_->NextNode(node_);
            return *this;
        }

        iterator operator++(int) noexcept
        {
            iterator previous{*this};
            ++(*this);
            return previous;
        }

        iterator& operator--() noexcept
        {
            if (owner_ == nullptr)
            {
                std::abort();
            }

            if (node_ == nullptr)
            {
                node_ = owner_->rightmost_.get();
                if (node_ == nullptr)
                {
                    std::abort();
                }
                return *this;
            }

            node_ = owner_->PreviousNode(node_);
            if (node_ == nullptr)
            {
                std::abort();
            }
            return *this;
        }

        iterator operator--(int) noexcept
        {
            iterator previous{*this};
            --(*this);
            return previous;
        }

        bool operator==(const iterator& other) const noexcept
        {
            return (node_ == other.node_) && (owner_ == other.owner_);
        }

        bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

      private:
        iterator(Node* node, Map* owner) noexcept : node_{node}, owner_{owner} {}

        Node* node_{nullptr};
        Map* owner_{nullptr};
    };

    class const_iterator
    {
        friend class Map;

      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = const Map::value_type;
        using difference_type = Map::difference_type;
        using pointer = const value_type*;
        using reference = const value_type&;

        const_iterator() noexcept = default;

        const_iterator(const iterator& other) noexcept : node_{other.node_}, owner_{other.owner_} {}

        reference operator*() const noexcept
        {
            if (node_ == nullptr)
            {
                std::abort();
            }
            return node_->value;
        }

        pointer operator->() const noexcept
        {
            if (node_ == nullptr)
            {
                std::abort();
            }
            return &node_->value;
        }

        const_iterator& operator++() noexcept
        {
            if ((owner_ == nullptr) || (node_ == nullptr))
            {
                std::abort();
            }

            node_ = owner_->NextNode(node_);
            return *this;
        }

        const_iterator operator++(int) noexcept
        {
            const_iterator previous{*this};
            ++(*this);
            return previous;
        }

        const_iterator& operator--() noexcept
        {
            if (owner_ == nullptr)
            {
                std::abort();
            }

            if (node_ == nullptr)
            {
                node_ = owner_->rightmost_.get();
                if (node_ == nullptr)
                {
                    std::abort();
                }
                return *this;
            }

            node_ = owner_->PreviousNode(node_);
            if (node_ == nullptr)
            {
                std::abort();
            }
            return *this;
        }

        const_iterator operator--(int) noexcept
        {
            const_iterator previous{*this};
            --(*this);
            return previous;
        }

        bool operator==(const const_iterator& other) const noexcept
        {
            return (node_ == other.node_) && (owner_ == other.owner_);
        }

        bool operator!=(const const_iterator& other) const noexcept
        {
            return !(*this == other);
        }

      private:
        const_iterator(const Node* node, const Map* owner) noexcept : node_{node}, owner_{owner} {}

        const Node* node_{nullptr};
        const Map* owner_{nullptr};
    };

    /// @brief Creates an empty map.
    static score::Result<Map> Create(key_compare compare = key_compare{},
                                     allocator_type allocator = allocator_type{}) noexcept
    {
        return Map{std::move(allocator), std::move(compare)};
    }

    /// @brief Creates a map from iterator range [first, last).
    template <typename InputIt>
    static score::Result<Map> Create(InputIt first,
                                     InputIt last,
                                     key_compare compare = key_compare{},
                                     allocator_type allocator = allocator_type{}) noexcept
    {
        Map map{std::move(allocator), std::move(compare)};
        const_iterator hint = map.cend();
        for (InputIt it = first; it != last; ++it)
        {
            const auto inserted = map.Insert(hint, *it);
            if (!inserted.has_value())
            {
                return score::Result<Map>{score::unexpect, inserted.error()};
            }
            hint = inserted.value().first;
        }
        return map;
    }

    /// @brief Creates a map from an initializer list.
    static score::Result<Map> Create(std::initializer_list<value_type> init,
                                     key_compare compare = key_compare{},
                                     allocator_type allocator = allocator_type{}) noexcept
    {
        return Create(init.begin(), init.end(), std::move(compare), std::move(allocator));
    }

    /// @brief Like Create(), but aborts on allocation failure.
    static Map CreateOrAbort(key_compare compare = key_compare{},
                             allocator_type allocator = allocator_type{}) noexcept
    {
        score::Result<Map> created = Create(std::move(compare), std::move(allocator));
        if (!created.has_value())
        {
            std::abort();
        }
        return std::move(created).value();
    }

    /// @brief Like Create(first, last), but aborts on allocation failure.
    template <typename InputIt>
    static Map CreateOrAbort(InputIt first,
                             InputIt last,
                             key_compare compare = key_compare{},
                             allocator_type allocator = allocator_type{}) noexcept
    {
        score::Result<Map> created = Create(first, last, std::move(compare), std::move(allocator));
        if (!created.has_value())
        {
            std::abort();
        }
        return std::move(created).value();
    }

    /// @brief Like Create(initializer_list), but aborts on allocation failure.
    static Map CreateOrAbort(std::initializer_list<value_type> init,
                             key_compare compare = key_compare{},
                             allocator_type allocator = allocator_type{}) noexcept
    {
        score::Result<Map> created = Create(init, std::move(compare), std::move(allocator));
        if (!created.has_value())
        {
            std::abort();
        }
        return std::move(created).value();
    }

    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    Map(Map&& other) noexcept
        : allocator_{std::move(other.allocator_)},
          compare_{std::move(other.compare_)},
          root_{other.root_},
          leftmost_{other.leftmost_},
          rightmost_{other.rightmost_},
          size_{other.size_}
    {
        other.root_ = nullable_ptr<Node>{nullptr};
        other.leftmost_ = nullable_ptr<Node>{nullptr};
        other.rightmost_ = nullable_ptr<Node>{nullptr};
        other.size_ = 0U;
    }

    Map& operator=(Map&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            allocator_ = std::move(other.allocator_);
            compare_ = std::move(other.compare_);
            root_ = other.root_;
            leftmost_ = other.leftmost_;
            rightmost_ = other.rightmost_;
            size_ = other.size_;

            other.root_ = nullable_ptr<Node>{nullptr};
            other.leftmost_ = nullable_ptr<Node>{nullptr};
            other.rightmost_ = nullable_ptr<Node>{nullptr};
            other.size_ = 0U;
        }
        return *this;
    }

    ~Map() noexcept
    {
        clear();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return size_ == 0U;
    }

    [[nodiscard]] size_type size() const noexcept
    {
        return size_;
    }

    [[nodiscard]] allocator_type get_allocator() const noexcept
    {
        return allocator_;
    }

    [[nodiscard]] iterator begin() noexcept
    {
        return iterator{leftmost_.get(), this};
    }

    [[nodiscard]] const_iterator begin() const noexcept
    {
        return const_iterator{leftmost_.get(), this};
    }

    [[nodiscard]] const_iterator cbegin() const noexcept
    {
        return const_iterator{leftmost_.get(), this};
    }

    [[nodiscard]] iterator end() noexcept
    {
        return iterator{nullptr, this};
    }

    [[nodiscard]] const_iterator end() const noexcept
    {
        return const_iterator{nullptr, this};
    }

    [[nodiscard]] const_iterator cend() const noexcept
    {
        return const_iterator{nullptr, this};
    }

    [[nodiscard]] iterator find(const key_type& key) noexcept
    {
        return iterator{FindNode(root_.get(), key), this};
    }

    [[nodiscard]] const_iterator find(const key_type& key) const noexcept
    {
        return const_iterator{FindNode(root_.get(), key), this};
    }

    [[nodiscard]] bool contains(const key_type& key) const noexcept
    {
        return FindNode(root_.get(), key) != nullptr;
    }

    /// @brief Like std::map::at(). Aborts if key does not exist.
    mapped_type& at(const key_type& key) noexcept
    {
        Node* const node = FindNode(root_.get(), key);
        if (node == nullptr)
        {
            std::abort();
        }
        return node->value.second;
    }

    /// @brief Like std::map::at(). Aborts if key does not exist.
    const mapped_type& at(const key_type& key) const noexcept
    {
        const Node* const node = FindNode(root_.get(), key);
        if (node == nullptr)
        {
            std::abort();
        }
        return node->value.second;
    }

    /// @brief Inserts @p value if key is not present.
    /// @return Pair of iterator and insertion flag. Returns kOutOfMemory on allocation failure.
    score::Result<std::pair<iterator, bool>> Insert(const value_type& value) noexcept
    {
        return InsertImpl(value);
    }

    /// @brief Move overload of Insert().
    score::Result<std::pair<iterator, bool>> Insert(value_type&& value) noexcept
    {
        return InsertImpl(std::move(value));
    }

    /// @brief Hint-aware Insert(). Falls back to regular lookup if hint is not usable.
    score::Result<std::pair<iterator, bool>> Insert(const_iterator hint, const value_type& value) noexcept
    {
        return InsertImpl(value, &hint);
    }

    /// @brief Hint-aware move Insert(). Falls back to regular lookup if hint is not usable.
    score::Result<std::pair<iterator, bool>> Insert(const_iterator hint, value_type&& value) noexcept
    {
        return InsertImpl(std::move(value), &hint);
    }

    /// @brief Like Insert(), but aborts on allocation failure.
    std::pair<iterator, bool> InsertOrAbort(const value_type& value) noexcept
    {
        auto inserted = Insert(value);
        if (!inserted.has_value())
        {
            std::abort();
        }
        return std::move(inserted).value();
    }

    /// @brief Move overload of InsertOrAbort().
    std::pair<iterator, bool> InsertOrAbort(value_type&& value) noexcept
    {
        auto inserted = Insert(std::move(value));
        if (!inserted.has_value())
        {
            std::abort();
        }
        return std::move(inserted).value();
    }

    /// @brief Like hint-aware Insert(), but aborts on allocation failure.
    std::pair<iterator, bool> InsertOrAbort(const_iterator hint, const value_type& value) noexcept
    {
        auto inserted = Insert(hint, value);
        if (!inserted.has_value())
        {
            std::abort();
        }
        return std::move(inserted).value();
    }

    /// @brief Like hint-aware move Insert(), but aborts on allocation failure.
    std::pair<iterator, bool> InsertOrAbort(const_iterator hint, value_type&& value) noexcept
    {
        auto inserted = Insert(hint, std::move(value));
        if (!inserted.has_value())
        {
            std::abort();
        }
        return std::move(inserted).value();
    }

    /// @brief Constructs a value_type in-place in the node. Returns kOutOfMemory on allocation failure.
    ///
    /// Unlike Insert, the value is constructed directly in node storage without
    /// intermediate copies or moves. If the key already exists, the pre-constructed
    /// node is destroyed and the existing entry is returned.
    score::Result<std::pair<iterator, bool>> Emplace(const key_type& key, const mapped_type& value) noexcept
    {
        return EmplaceKeyValueImpl(key, value);
    }

    score::Result<std::pair<iterator, bool>> Emplace(const key_type& key, mapped_type&& value) noexcept
    {
        return EmplaceKeyValueImpl(key, std::move(value));
    }

    score::Result<std::pair<iterator, bool>> Emplace(key_type&& key, const mapped_type& value) noexcept
    {
        return EmplaceKeyValueImpl(std::move(key), value);
    }

    score::Result<std::pair<iterator, bool>> Emplace(key_type&& key, mapped_type&& value) noexcept
    {
        return EmplaceKeyValueImpl(std::move(key), std::move(value));
    }

    /// @brief Hint-aware key/value emplace. Falls back to regular lookup if hint is not usable.
    score::Result<std::pair<iterator, bool>> Emplace(const_iterator hint,
                                                     const key_type& key,
                                                     const mapped_type& value) noexcept
    {
        return EmplaceKeyValueImpl(key, value, &hint);
    }

    /// @brief Hint-aware key/value emplace. Falls back to regular lookup if hint is not usable.
    score::Result<std::pair<iterator, bool>> Emplace(const_iterator hint,
                                                     const key_type& key,
                                                     mapped_type&& value) noexcept
    {
        return EmplaceKeyValueImpl(key, std::move(value), &hint);
    }

    /// @brief Hint-aware key/value emplace. Falls back to regular lookup if hint is not usable.
    score::Result<std::pair<iterator, bool>> Emplace(const_iterator hint,
                                                     key_type&& key,
                                                     const mapped_type& value) noexcept
    {
        return EmplaceKeyValueImpl(std::move(key), value, &hint);
    }

    /// @brief Hint-aware key/value emplace. Falls back to regular lookup if hint is not usable.
    score::Result<std::pair<iterator, bool>> Emplace(const_iterator hint,
                                                     key_type&& key,
                                                     mapped_type&& value) noexcept
    {
        return EmplaceKeyValueImpl(std::move(key), std::move(value), &hint);
    }

    template <typename... Args>
    score::Result<std::pair<iterator, bool>> Emplace(Args&&... args) noexcept
    {
        return EmplaceConstructedNodeImpl(nullptr, std::forward<Args>(args)...);
    }

    /// @brief Hint-aware in-place construction. Falls back to regular lookup if hint is not usable.
    template <typename... Args>
    score::Result<std::pair<iterator, bool>> Emplace(const_iterator hint, Args&&... args) noexcept
    {
        return EmplaceConstructedNodeImpl(&hint, std::forward<Args>(args)...);
    }

    /// @brief Like Emplace(), but aborts on allocation failure.
    template <typename... Args>
    std::pair<iterator, bool> EmplaceOrAbort(Args&&... args) noexcept
    {
        auto inserted = Emplace(std::forward<Args>(args)...);
        if (!inserted.has_value())
        {
            std::abort();
        }
        return std::move(inserted).value();
    }

    /// @brief Operator[] equivalent with explicit error propagation.
    ///
    /// Returns a reference to the mapped value for @p key. If key is absent, inserts
    /// a new element with default-constructed mapped value.
    ///
    /// @return kOutOfMemory if insertion allocation fails.
    score::Result<std::reference_wrapper<mapped_type>> GetOrInsertDefault(const key_type& key) noexcept
    {
        Node* existing = FindNode(root_.get(), key);
        if (existing != nullptr)
        {
            return std::ref(existing->value.second);
        }

        if constexpr (std::is_default_constructible_v<mapped_type>)
        {
            value_type value{key, mapped_type{}};
            auto inserted = Insert(std::move(value));
            if (!inserted.has_value())
            {
                return score::Result<std::reference_wrapper<mapped_type>>{score::unexpect, inserted.error()};
            }

            return std::ref(inserted.value().first->second);
        }
        else
        {
            std::abort();
        }
    }

    /// @brief Like GetOrInsertDefault(), but aborts on allocation failure.
    mapped_type& GetOrInsertDefaultOrAbort(const key_type& key) noexcept
    {
        auto result = GetOrInsertDefault(key);
        if (!result.has_value())
        {
            std::abort();
        }
        return result.value().get();
    }

    /// @brief Like std::map::erase(key). Returns 1 if erased, 0 if not found.
    size_type erase(const key_type& key) noexcept
    {
        Node* const node = FindNode(root_.get(), key);
        if (node == nullptr)
        {
            return 0U;
        }

        EraseNode(node);
        return 1U;
    }

    void clear() noexcept
    {
        Node* current = root_.get();

        while (current != nullptr)
        {
            if (current->left.get() != nullptr)
            {
                current = current->left.get();
                continue;
            }
            if (current->right.get() != nullptr)
            {
                current = current->right.get();
                continue;
            }

            Node* const parent = current->parent.get();
            if (parent != nullptr)
            {
                if (parent->left.get() == current)
                {
                    parent->left = nullable_ptr<Node>{nullptr};
                }
                else
                {
                    parent->right = nullable_ptr<Node>{nullptr};
                }
            }

            DestroyNode(current);
            current = parent;
        }

        root_ = nullable_ptr<Node>{nullptr};
        leftmost_ = nullable_ptr<Node>{nullptr};
        rightmost_ = nullable_ptr<Node>{nullptr};
        size_ = 0U;
    }

    void swap(Map& other) noexcept
    {
        using std::swap;
        swap(allocator_, other.allocator_);
        swap(compare_, other.compare_);
        swap(root_, other.root_);
        swap(leftmost_, other.leftmost_);
        swap(rightmost_, other.rightmost_);
        swap(size_, other.size_);
    }

    /// @brief Deep-copies all elements while preserving key ordering.
    /// @return kOutOfMemory if node allocation fails.
    score::Result<Map> Clone() const noexcept
    {
        score::Result<Map> created = Create(compare_, allocator_);
        if (!created.has_value())
        {
            return created;
        }

        Map clone = std::move(created).value();
        const_iterator hint = clone.cend();
        for (const auto& value : *this)
        {
            auto inserted = clone.Insert(hint, value);
            if (!inserted.has_value())
            {
                return score::Result<Map>{score::unexpect, inserted.error()};
            }
            hint = inserted.value().first;
        }

        return clone;
    }

    /// @brief Like Clone(), but aborts on allocation failure.
    Map CloneOrAbort() const noexcept
    {
        score::Result<Map> clone = Clone();
        if (!clone.has_value())
        {
            std::abort();
        }
        return std::move(clone).value();
    }

  private:
    template <typename TNode>
    using nullable_ptr = typename pointer_policy::template NullablePtr<TNode>;

    struct Node
    {
        nullable_ptr<Node> left{nullptr};
        nullable_ptr<Node> right{nullptr};
        nullable_ptr<Node> parent{nullptr};
        value_type value;
        std::uint32_t height{1U};

        Node(Node* parent_node, const value_type& value_in) noexcept : parent{parent_node}, value{value_in} {}

        Node(Node* parent_node, value_type&& value_in) noexcept : parent{parent_node}, value{std::move(value_in)} {}

        template <typename... Args>
        Node(Node* parent_node, std::in_place_t, Args&&... args) noexcept
            : parent{parent_node}, value{std::forward<Args>(args)...}
        {
        }
    };

    struct InsertPosition
    {
        Node* parent{nullptr};
        Node* existing{nullptr};
        bool insert_left{false};
    };

    using node_allocator_type = typename std::allocator_traits<allocator_type>::template rebind_alloc<Node>;

    static constexpr bool kUsesDefaultAllocator =
        std::is_same<allocator_type, PolymorphicAllocator<value_type>>::value &&
        std::is_same<node_allocator_type, PolymorphicAllocator<Node>>::value;

    static_assert(kUsesDefaultAllocator || std::is_constructible<node_allocator_type, allocator_type>::value,
                  "Map allocator rebind type must be constructible from allocator_type.");

    explicit Map(allocator_type allocator, key_compare compare) noexcept
        : allocator_{std::move(allocator)}, compare_{std::move(compare)}
    {
    }

    static node_allocator_type CreateNodeAllocator(allocator_type& allocator) noexcept
    {
        if constexpr (kUsesDefaultAllocator)
        {
            return node_allocator_type{allocator.resource()};
        }
        else
        {
            return node_allocator_type{allocator};
        }
    }

    template <typename ValueArg>
    score::Result<Node*> AllocateNode(Node* parent, ValueArg&& value) noexcept
    {
        node_allocator_type node_allocator = CreateNodeAllocator(allocator_);
        Node* const node = std::allocator_traits<node_allocator_type>::allocate(node_allocator, 1U);
        if (node == nullptr)
        {
            return score::Result<Node*>{score::unexpect,
                                        MakeError(ContainerErrorCode::kOutOfMemory, "Map allocation failed")};
        }

        std::allocator_traits<node_allocator_type>::construct(node_allocator, node, parent, std::forward<ValueArg>(value));
        return node;
    }

    template <typename... Args>
    score::Result<Node*> AllocateNodeEmplace(Node* parent, Args&&... args) noexcept
    {
        node_allocator_type node_allocator = CreateNodeAllocator(allocator_);
        Node* const node = std::allocator_traits<node_allocator_type>::allocate(node_allocator, 1U);
        if (node == nullptr)
        {
            return score::Result<Node*>{score::unexpect,
                                        MakeError(ContainerErrorCode::kOutOfMemory, "Map allocation failed")};
        }

        std::allocator_traits<node_allocator_type>::construct(
            node_allocator, node, parent, std::in_place, std::forward<Args>(args)...);
        return node;
    }

    void DestroyNode(Node* node) noexcept
    {
        node_allocator_type node_allocator = CreateNodeAllocator(allocator_);
        std::allocator_traits<node_allocator_type>::destroy(node_allocator, node);
        std::allocator_traits<node_allocator_type>::deallocate(node_allocator, node, 1U);
    }

    bool IsLess(const key_type& lhs, const key_type& rhs) const noexcept
    {
        return compare_(lhs, rhs);
    }

    InsertPosition FindInsertPosition(const key_type& key) noexcept
    {
        InsertPosition position{};
        if (root_.get() == nullptr)
        {
            return position;
        }

        Node* const rightmost = rightmost_.get();
        if ((rightmost != nullptr) && IsLess(rightmost->value.first, key))
        {
            position.parent = rightmost;
            position.insert_left = false;
            return position;
        }

        Node* const leftmost = leftmost_.get();
        if ((leftmost != nullptr) && IsLess(key, leftmost->value.first))
        {
            position.parent = leftmost;
            position.insert_left = true;
            return position;
        }

        if ((rightmost != nullptr) && !IsLess(key, rightmost->value.first) && !IsLess(rightmost->value.first, key))
        {
            position.existing = rightmost;
            return position;
        }

        if ((leftmost != nullptr) && (leftmost != rightmost) && !IsLess(key, leftmost->value.first) &&
            !IsLess(leftmost->value.first, key))
        {
            position.existing = leftmost;
            return position;
        }

        Node* lower_bound = nullptr;
        Node* current = root_.get();
        while (current != nullptr)
        {
            position.parent = current;
            if (IsLess(current->value.first, key))
            {
                position.insert_left = false;
                current = current->right.get();
            }
            else
            {
                lower_bound = current;
                position.insert_left = true;
                current = current->left.get();
            }
        }

        if ((lower_bound != nullptr) && !IsLess(key, lower_bound->value.first))
        {
            position.existing = lower_bound;
        }

        return position;
    }

    InsertPosition FindInsertPositionWithHint(const key_type& key, const const_iterator& hint) noexcept
    {
        if (hint.owner_ != this)
        {
            return FindInsertPosition(key);
        }

        if (root_.get() == nullptr)
        {
            return InsertPosition{};
        }

        if (hint.node_ == nullptr)
        {
            if (rightmost_.get() == nullptr)
            {
                return InsertPosition{};
            }
            if (IsLess(rightmost_.get()->value.first, key))
            {
                InsertPosition position{};
                position.parent = rightmost_.get();
                position.insert_left = false;
                return position;
            }
            return FindInsertPosition(key);
        }

        Node* const hinted = const_cast<Node*>(hint.node_);
        if (IsLess(key, hinted->value.first))
        {
            Node* const predecessor = PreviousNode(hinted);
            const bool key_greater_than_predecessor = (predecessor == nullptr) || IsLess(predecessor->value.first, key);
            if (key_greater_than_predecessor)
            {
                if (hinted->left.get() == nullptr)
                {
                    InsertPosition position{};
                    position.parent = hinted;
                    position.insert_left = true;
                    return position;
                }
                return FindInsertPosition(key);
            }
            return FindInsertPosition(key);
        }

        if (IsLess(hinted->value.first, key))
        {
            Node* const successor = NextNode(hinted);
            const bool key_less_than_successor = (successor == nullptr) || IsLess(key, successor->value.first);
            if (key_less_than_successor)
            {
                if (hinted->right.get() == nullptr)
                {
                    InsertPosition position{};
                    position.parent = hinted;
                    position.insert_left = false;
                    return position;
                }
                return FindInsertPosition(key);
            }
            return FindInsertPosition(key);
        }

        InsertPosition position{};
        position.existing = hinted;
        return position;
    }

    iterator FinalizeInsertResult(const InsertPosition& position, Node* inserted) noexcept
    {
        if (size_ == 0U)
        {
            leftmost_ = inserted;
            rightmost_ = inserted;
        }
        else
        {
            if (position.insert_left && (position.parent == leftmost_.get()))
            {
                leftmost_ = inserted;
            }
            if (!position.insert_left && (position.parent == rightmost_.get()))
            {
                rightmost_ = inserted;
            }
        }

        ++size_;
        RebalanceFrom(position.parent, true);
        return iterator{inserted, this};
    }

    score::Result<std::pair<iterator, bool>> LinkAllocatedNode(const InsertPosition& position, Node* inserted) noexcept
    {
        inserted->parent = position.parent;
        if (position.parent == nullptr)
        {
            root_ = inserted;
        }
        else if (position.insert_left)
        {
            position.parent->left = inserted;
        }
        else
        {
            position.parent->right = inserted;
        }

        return std::pair<iterator, bool>{FinalizeInsertResult(position, inserted), true};
    }

    template <typename KeyArg, typename MappedArg>
    score::Result<std::pair<iterator, bool>> EmplaceKeyValueImpl(KeyArg&& key,
                                                                  MappedArg&& value,
                                                                  const const_iterator* hint = nullptr) noexcept
    {
        const key_type& lookup_key = key;
        const InsertPosition position = (hint == nullptr) ? FindInsertPosition(lookup_key)
                                                          : FindInsertPositionWithHint(lookup_key, *hint);
        if (position.existing != nullptr)
        {
            return std::pair<iterator, bool>{iterator{position.existing, this}, false};
        }

        auto allocated = AllocateNodeEmplace(position.parent, std::forward<KeyArg>(key), std::forward<MappedArg>(value));
        if (!allocated.has_value())
        {
            return score::Result<std::pair<iterator, bool>>{score::unexpect, allocated.error()};
        }

        return LinkAllocatedNode(position, allocated.value());
    }

    template <typename ValueArg>
    score::Result<std::pair<iterator, bool>> InsertImpl(ValueArg&& value,
                                                        const const_iterator* hint = nullptr) noexcept
    {
        const key_type& key = value.first;

        const InsertPosition position = (hint == nullptr) ? FindInsertPosition(key)
                                                          : FindInsertPositionWithHint(key, *hint);
        if (position.existing != nullptr)
        {
            return std::pair<iterator, bool>{iterator{position.existing, this}, false};
        }

        auto allocated = AllocateNode(position.parent, std::forward<ValueArg>(value));
        if (!allocated.has_value())
        {
            return score::Result<std::pair<iterator, bool>>{score::unexpect, allocated.error()};
        }

        return LinkAllocatedNode(position, allocated.value());
    }

    template <typename... Args>
    score::Result<std::pair<iterator, bool>> EmplaceConstructedNodeImpl(const const_iterator* hint,
                                                                         Args&&... args) noexcept
    {
        auto allocated = AllocateNodeEmplace(nullptr, std::forward<Args>(args)...);
        if (!allocated.has_value())
        {
            return score::Result<std::pair<iterator, bool>>{score::unexpect, allocated.error()};
        }

        Node* const node = allocated.value();
        const key_type& key = node->value.first;
        const InsertPosition position = (hint == nullptr) ? FindInsertPosition(key)
                                                          : FindInsertPositionWithHint(key, *hint);
        if (position.existing != nullptr)
        {
            DestroyNode(node);
            return std::pair<iterator, bool>{iterator{position.existing, this}, false};
        }

        node->parent = position.parent;
        if (position.parent == nullptr)
        {
            root_ = node;
        }
        else if (position.insert_left)
        {
            position.parent->left = node;
        }
        else
        {
            position.parent->right = node;
        }

        return std::pair<iterator, bool>{FinalizeInsertResult(position, node), true};
    }

    template <typename NodePtr>
    NodePtr LowerBoundNode(NodePtr root, const key_type& key) const noexcept
    {
        NodePtr current = root;
        NodePtr lower_bound = nullptr;
        while (current != nullptr)
        {
            if (IsLess(current->value.first, key))
            {
                current = current->right.get();
            }
            else
            {
                lower_bound = current;
                current = current->left.get();
            }
        }

        return lower_bound;
    }

    template <typename NodePtr>
    NodePtr FindNode(NodePtr root, const key_type& key) const noexcept
    {
        NodePtr lower_bound = LowerBoundNode(root, key);
        if ((lower_bound != nullptr) && !IsLess(key, lower_bound->value.first))
        {
            return lower_bound;
        }

        return nullptr;
    }

    template <typename NodePtr>
    static NodePtr MinNode(NodePtr node) noexcept
    {
        NodePtr current = node;
        while ((current != nullptr) && (current->left.get() != nullptr))
        {
            current = current->left.get();
        }
        return current;
    }

    template <typename NodePtr>
    static NodePtr MaxNode(NodePtr node) noexcept
    {
        NodePtr current = node;
        while ((current != nullptr) && (current->right.get() != nullptr))
        {
            current = current->right.get();
        }
        return current;
    }

    template <typename NodePtr>
    static NodePtr NextNode(NodePtr node) noexcept
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        if (node->right.get() != nullptr)
        {
            return MinNode(node->right.get());
        }

        NodePtr parent = node->parent.get();
        NodePtr current = node;
        while ((parent != nullptr) && (current == parent->right.get()))
        {
            current = parent;
            parent = parent->parent.get();
        }

        return parent;
    }

    template <typename NodePtr>
    static NodePtr PreviousNode(NodePtr node) noexcept
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        if (node->left.get() != nullptr)
        {
            return MaxNode(node->left.get());
        }

        NodePtr parent = node->parent.get();
        NodePtr current = node;
        while ((parent != nullptr) && (current == parent->left.get()))
        {
            current = parent;
            parent = parent->parent.get();
        }

        return parent;
    }

    static std::uint32_t Height(const Node* node) noexcept
    {
        return node == nullptr ? 0U : node->height;
    }

    static void UpdateHeight(Node* node) noexcept
    {
        if (node == nullptr)
        {
            return;
        }
        node->height = 1U + std::max(Height(node->left.get()), Height(node->right.get()));
    }

    static int BalanceFactor(const Node* node) noexcept
    {
        if (node == nullptr)
        {
            return 0;
        }
        return static_cast<int>(Height(node->left.get())) - static_cast<int>(Height(node->right.get()));
    }

    void ReplaceNode(Node* current, Node* replacement) noexcept
    {
        Node* const parent = current->parent.get();
        if (parent == nullptr)
        {
            root_ = replacement;
        }
        else if (parent->left.get() == current)
        {
            parent->left = replacement;
        }
        else
        {
            parent->right = replacement;
        }

        if (replacement != nullptr)
        {
            replacement->parent = parent;
        }
    }

    Node* RotateLeft(Node* node) noexcept
    {
        Node* const pivot = node->right.get();
        if (pivot == nullptr)
        {
            return node;
        }

        Node* const pivot_left = pivot->left.get();
        Node* const parent = node->parent.get();

        pivot->parent = parent;
        if (parent == nullptr)
        {
            root_ = pivot;
        }
        else if (parent->left.get() == node)
        {
            parent->left = pivot;
        }
        else
        {
            parent->right = pivot;
        }

        pivot->left = node;
        node->parent = pivot;
        node->right = pivot_left;
        if (pivot_left != nullptr)
        {
            pivot_left->parent = node;
        }

        UpdateHeight(node);
        UpdateHeight(pivot);
        return pivot;
    }

    Node* RotateRight(Node* node) noexcept
    {
        Node* const pivot = node->left.get();
        if (pivot == nullptr)
        {
            return node;
        }

        Node* const pivot_right = pivot->right.get();
        Node* const parent = node->parent.get();

        pivot->parent = parent;
        if (parent == nullptr)
        {
            root_ = pivot;
        }
        else if (parent->left.get() == node)
        {
            parent->left = pivot;
        }
        else
        {
            parent->right = pivot;
        }

        pivot->right = node;
        node->parent = pivot;
        node->left = pivot_right;
        if (pivot_right != nullptr)
        {
            pivot_right->parent = node;
        }

        UpdateHeight(node);
        UpdateHeight(pivot);
        return pivot;
    }

    void RebalanceFrom(Node* node, const bool stop_on_stable_height) noexcept
    {
        Node* current = node;
        while (current != nullptr)
        {
            const std::uint32_t previous_height = current->height;
            UpdateHeight(current);
            const int balance = BalanceFactor(current);

            if (balance > 1)
            {
                Node* const left = current->left.get();
                if (BalanceFactor(left) < 0)
                {
                    RotateLeft(left);
                }
                current = RotateRight(current);
            }
            else if (balance < -1)
            {
                Node* const right = current->right.get();
                if (BalanceFactor(right) > 0)
                {
                    RotateRight(right);
                }
                current = RotateLeft(current);
            }
            else if (stop_on_stable_height && (previous_height == current->height))
            {
                break;
            }

            current = current->parent.get();
        }
    }

    void EraseNode(Node* target) noexcept
    {
        Node* rebalance_start = nullptr;
        Node* next_leftmost = leftmost_.get();
        Node* next_rightmost = rightmost_.get();
        if (target == leftmost_.get())
        {
            next_leftmost = NextNode(target);
        }
        if (target == rightmost_.get())
        {
            next_rightmost = PreviousNode(target);
        }

        if (target->left.get() == nullptr)
        {
            rebalance_start = target->parent.get();
            ReplaceNode(target, target->right.get());
        }
        else if (target->right.get() == nullptr)
        {
            rebalance_start = target->parent.get();
            ReplaceNode(target, target->left.get());
        }
        else
        {
            Node* const successor = MinNode(target->right.get());
            Node* const successor_parent = successor->parent.get();

            if (successor_parent != target)
            {
                rebalance_start = successor_parent;
                ReplaceNode(successor, successor->right.get());
                successor->right = target->right.get();
                successor->right.get()->parent = successor;
            }
            else
            {
                rebalance_start = successor;
            }

            ReplaceNode(target, successor);
            successor->left = target->left.get();
            successor->left.get()->parent = successor;
            UpdateHeight(successor);
        }

        DestroyNode(target);
        --size_;

        if (rebalance_start != nullptr)
        {
            RebalanceFrom(rebalance_start, false);
        }

        if (root_.get() == nullptr)
        {
            leftmost_ = nullable_ptr<Node>{nullptr};
            rightmost_ = nullable_ptr<Node>{nullptr};
        }
        else
        {
            leftmost_ = next_leftmost;
            rightmost_ = next_rightmost;
        }
    }

    allocator_type allocator_;
    key_compare compare_;
    nullable_ptr<Node> root_{nullptr};
    nullable_ptr<Node> leftmost_{nullptr};
    nullable_ptr<Node> rightmost_{nullptr};
    size_type size_{0U};
};

template <typename Key,
          typename T,
          typename Compare = std::less<Key>,
          typename Allocator = PolymorphicAllocator<std::pair<const Key, T>>,
          typename PointerPolicy = ShmPointerPolicy>
using MapBase = Map<Key, T, Compare, Allocator, PointerPolicy>;

}  // namespace score::shm

#endif  // SCORE_SHM_MAP_H
