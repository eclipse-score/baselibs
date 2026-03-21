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
/// - Tree links use score::shm::NullableOffsetPtr for shared-memory relocatability.
///
/// The tree is AVL-balanced to provide O(log n) insert/find/erase in the worst case.
template <typename Key,
          typename T,
          typename Compare = std::less<Key>,
          typename Allocator = PolymorphicAllocator<std::pair<const Key, T>>>
class Map
{
    struct Node;

  public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const key_type, mapped_type>;
    using key_compare = Compare;
    using allocator_type = Allocator;
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
                node_ = owner_->MaxNode(owner_->root_.get());
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
                node_ = owner_->MaxNode(owner_->root_.get());
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
        for (InputIt it = first; it != last; ++it)
        {
            const auto inserted = map.Insert(*it);
            if (!inserted.has_value())
            {
                return score::Result<Map>{score::unexpect, inserted.error()};
            }
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
          size_{other.size_}
    {
        other.root_ = NullableOffsetPtr<Node>{nullptr};
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
            size_ = other.size_;

            other.root_ = NullableOffsetPtr<Node>{nullptr};
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
        return iterator{MinNode(root_.get()), this};
    }

    [[nodiscard]] const_iterator begin() const noexcept
    {
        return const_iterator{MinNode(root_.get()), this};
    }

    [[nodiscard]] const_iterator cbegin() const noexcept
    {
        return const_iterator{MinNode(root_.get()), this};
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

    /// @brief Like std::map::emplace(). Returns kOutOfMemory on allocation failure.
    template <typename... Args>
    score::Result<std::pair<iterator, bool>> Emplace(Args&&... args) noexcept
    {
        value_type value{std::forward<Args>(args)...};
        return Insert(std::move(value));
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
        Node* previous = nullptr;

        while (current != nullptr)
        {
            Node* const parent = current->parent.get();
            Node* const left = current->left.get();
            Node* const right = current->right.get();

            if (previous == parent)
            {
                if (left != nullptr)
                {
                    previous = current;
                    current = left;
                    continue;
                }
                if (right != nullptr)
                {
                    previous = current;
                    current = right;
                    continue;
                }
            }
            else if (previous == left)
            {
                if (right != nullptr)
                {
                    previous = current;
                    current = right;
                    continue;
                }
            }

            DestroyNode(current);
            previous = current;
            current = parent;
        }

        root_ = NullableOffsetPtr<Node>{nullptr};
        size_ = 0U;
    }

    void swap(Map& other) noexcept
    {
        using std::swap;
        swap(allocator_, other.allocator_);
        swap(compare_, other.compare_);
        swap(root_, other.root_);
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
        for (const auto& value : *this)
        {
            auto inserted = clone.Insert(value);
            if (!inserted.has_value())
            {
                return score::Result<Map>{score::unexpect, inserted.error()};
            }
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
    struct Node
    {
        NullableOffsetPtr<Node> left{nullptr};
        NullableOffsetPtr<Node> right{nullptr};
        NullableOffsetPtr<Node> parent{nullptr};
        value_type value;
        std::uint32_t height{1U};

        Node(Node* parent_node, const value_type& value_in) noexcept : parent{parent_node}, value{value_in} {}

        Node(Node* parent_node, value_type&& value_in) noexcept : parent{parent_node}, value{std::move(value_in)} {}
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

    template <typename ValueArg>
    score::Result<std::pair<iterator, bool>> InsertImpl(ValueArg&& value) noexcept
    {
        const key_type& key = value.first;

        if (root_.get() == nullptr)
        {
            auto allocated = AllocateNode(nullptr, std::forward<ValueArg>(value));
            if (!allocated.has_value())
            {
                return score::Result<std::pair<iterator, bool>>{score::unexpect, allocated.error()};
            }

            root_ = allocated.value();
            ++size_;
            return std::pair<iterator, bool>{iterator{root_.get(), this}, true};
        }

        Node* parent = nullptr;
        Node* current = root_.get();
        bool insert_left = false;

        while (current != nullptr)
        {
            parent = current;
            if (IsLess(key, current->value.first))
            {
                insert_left = true;
                current = current->left.get();
            }
            else if (IsLess(current->value.first, key))
            {
                insert_left = false;
                current = current->right.get();
            }
            else
            {
                return std::pair<iterator, bool>{iterator{current, this}, false};
            }
        }

        auto allocated = AllocateNode(parent, std::forward<ValueArg>(value));
        if (!allocated.has_value())
        {
            return score::Result<std::pair<iterator, bool>>{score::unexpect, allocated.error()};
        }

        Node* const inserted = allocated.value();
        if (insert_left)
        {
            parent->left = inserted;
        }
        else
        {
            parent->right = inserted;
        }

        ++size_;
        RebalanceFrom(parent);
        return std::pair<iterator, bool>{iterator{inserted, this}, true};
    }

    template <typename NodePtr>
    NodePtr FindNode(NodePtr root, const key_type& key) const noexcept
    {
        NodePtr current = root;
        while (current != nullptr)
        {
            if (IsLess(key, current->value.first))
            {
                current = current->left.get();
            }
            else if (IsLess(current->value.first, key))
            {
                current = current->right.get();
            }
            else
            {
                return current;
            }
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

    void RebalanceFrom(Node* node) noexcept
    {
        Node* current = node;
        while (current != nullptr)
        {
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

            current = current->parent.get();
        }
    }

    void EraseNode(Node* target) noexcept
    {
        Node* rebalance_start = nullptr;

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
            RebalanceFrom(rebalance_start);
        }
    }

    allocator_type allocator_;
    key_compare compare_;
    NullableOffsetPtr<Node> root_{nullptr};
    size_type size_{0U};
};

}  // namespace score::shm

#endif  // SCORE_SHM_MAP_H
