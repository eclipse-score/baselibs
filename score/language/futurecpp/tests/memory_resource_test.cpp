/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

#include <score/memory_resource.hpp>
#include <score/memory_resource.hpp> // check include guard

#include <score/string.hpp>
#include <score/string.hpp> // check include guard

#include <score/vector.hpp>
#include <score/vector.hpp> // check include guard

#include <score/unordered_map.hpp>
#include <score/unordered_map.hpp> // check include guard

#include <score/assert_support.hpp>
#include <score/bit.hpp>
#include <score/memory.hpp>
#include <score/size.hpp>

#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <new>
#include <vector>

#include <gtest/gtest.h>

namespace
{

using score::cpp::literals::operator""_UZ;

/** A mock memory resource that tracks all active allocations.
 */
class test_memory_resource : public score::cpp::pmr::memory_resource
{
public:
    struct alloc_info
    {
        bool operator==(const alloc_info& other) const
        {
            return (bytes == other.bytes) && (alignment == other.alignment) && (is_freed == other.is_freed);
        }
        std::size_t bytes;
        std::size_t alignment;
        bool is_freed;
    };

    std::map<void*, alloc_info> allocations;

    test_memory_resource(memory_resource* const upstream = score::cpp::pmr::new_delete_resource()) : upstream_{upstream} {}

    ~test_memory_resource()
    {
        for (const auto& ai : allocations)
        {
            SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(ai.second.is_freed);
        }
    }

    test_memory_resource(test_memory_resource&&) = delete;
    test_memory_resource(const test_memory_resource&) = delete;
    test_memory_resource& operator=(test_memory_resource&&) = delete;
    test_memory_resource& operator=(const test_memory_resource&) = delete;

protected:
    void* do_allocate(const std::size_t bytes, const std::size_t alignment) override
    {
        void* const p = upstream_->allocate(bytes, alignment);
        std::map<void*, alloc_info>::iterator it{};
        bool insertion_took_place{};
        std::tie(it, insertion_took_place) = allocations.insert(std::make_pair(p, alloc_info{bytes, alignment, true}));
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(insertion_took_place || it->second.is_freed);
        it->second.is_freed = false;
        return p;
    }

    void do_deallocate(void* const p, const std::size_t bytes, const std::size_t alignment) override
    {
        auto it = allocations.find(p);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(it != allocations.end());
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(bytes == it->second.bytes);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(alignment == it->second.alignment);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(!it->second.is_freed);
        it->second.is_freed = true;
        upstream_->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const memory_resource& other) const noexcept override { return this == &other; }

    memory_resource* upstream_;
};

static std::function<void*(std::size_t, std::size_t)> mock_allocate;
static std::function<void(void*, std::size_t, std::size_t)> mock_deallocate;

/** A mock allocator.
 */
template <typename T>
class test_allocator
{
public:
    using value_type = T;

public:
    int field = 0;

    test_allocator() noexcept = default;
    ~test_allocator() = default;
    test_allocator(const test_allocator&) = default;
    test_allocator& operator=(const test_allocator&) = default;

    template <typename U>
    test_allocator(const test_allocator<U>&)
    {
    }

    /** Allocates a storage suitable for n objects of type T.
     */
    T* allocate(std::size_t n) { return static_cast<T*>(mock_allocate(n, alignof(T))); }

    /** Deallocates storage pointed to by p.
     * @pre p must be a value returned by a previous call to allocate()
     *      that has not yet been deallocated.
     */
    void deallocate(T* p, std::size_t n) { return mock_deallocate(p, n, alignof(T)); }

    friend inline bool operator==(const test_allocator&, const test_allocator&) { return true; }
};

bool is_aligned(void* ptr, std::size_t alignment)
{
    return score::cpp::pmr::detail::is_aligned(score::cpp::bit_cast<std::uintptr_t>(ptr), alignment);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487566
TEST(PmrTest, PolymorphicMemoryResource_Allocate_Deallocate)
{
    test_memory_resource mr;
    void* p1 = mr.allocate(10, 8);
    EXPECT_NE(p1, nullptr);
    void* p2 = mr.allocate(33, 16);
    EXPECT_NE(p2, nullptr);
    void* p3 = mr.allocate(555, 32);
    EXPECT_NE(p3, nullptr);

    ASSERT_NE(mr.allocations.find(p1), mr.allocations.end());
    const test_memory_resource::alloc_info& ai1 = mr.allocations.find(p1)->second;
    EXPECT_EQ(ai1.bytes, 10);
    EXPECT_EQ(ai1.alignment, 8);
    EXPECT_FALSE(ai1.is_freed);

    ASSERT_NE(mr.allocations.find(p2), mr.allocations.end());
    const test_memory_resource::alloc_info& ai2 = mr.allocations.find(p2)->second;
    EXPECT_EQ(ai2.bytes, 33);
    EXPECT_EQ(ai2.alignment, 16);
    EXPECT_FALSE(ai2.is_freed);

    ASSERT_NE(mr.allocations.find(p3), mr.allocations.end());
    const test_memory_resource::alloc_info& ai3 = mr.allocations.find(p3)->second;
    EXPECT_EQ(ai3.bytes, 555);
    EXPECT_EQ(ai3.alignment, 32);
    EXPECT_FALSE(ai3.is_freed);

    mr.deallocate(p1, 10, 8);
    EXPECT_TRUE(ai1.is_freed);
    EXPECT_FALSE(ai2.is_freed);
    EXPECT_FALSE(ai3.is_freed);

    mr.deallocate(p3, 555, 32);
    EXPECT_TRUE(ai1.is_freed);
    EXPECT_FALSE(ai2.is_freed);
    EXPECT_TRUE(ai3.is_freed);

    mr.deallocate(p2, 33, 16);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487566
TEST(PmrTest, PolymorphicMemoryResource_Equality)
{
    test_memory_resource mr1;
    test_memory_resource mr2;

    EXPECT_TRUE(mr1 == mr1);
    EXPECT_TRUE(mr2 == mr2);
    EXPECT_FALSE(mr1 == mr2);

    EXPECT_FALSE(mr1 != mr1);
    EXPECT_FALSE(mr2 != mr2);
    EXPECT_TRUE(mr1 != mr2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_Construction)
{
    test_memory_resource mr;
    score::cpp::pmr::polymorphic_allocator<int> pma(&mr);

    EXPECT_EQ(pma.resource(), &mr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_CopyConstruction)
{
    test_memory_resource mr;
    score::cpp::pmr::polymorphic_allocator<int> pma(&mr);
    score::cpp::pmr::polymorphic_allocator<int> pma_copy(pma);

    EXPECT_EQ(pma_copy.resource(), &mr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_ConversionConstruction)
{
    test_memory_resource mr;
    score::cpp::pmr::polymorphic_allocator<int> pma(&mr);
    score::cpp::pmr::polymorphic_allocator<double> pma_copy(pma);

    EXPECT_EQ(pma_copy.resource(), &mr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_Equality)
{
    test_memory_resource mr;
    test_memory_resource mr2;
    score::cpp::pmr::polymorphic_allocator<int> pma1(&mr);
    score::cpp::pmr::polymorphic_allocator<double> pma2(&mr);
    score::cpp::pmr::polymorphic_allocator<int> pma3(&mr2);

    EXPECT_TRUE(pma1 == pma1);
    EXPECT_TRUE(pma1 == pma2);
    EXPECT_FALSE(pma1 == pma3);
    EXPECT_FALSE(pma2 == pma3);

    EXPECT_FALSE(pma1 != pma1);
    EXPECT_FALSE(pma1 != pma2);
    EXPECT_TRUE(pma1 != pma3);
    EXPECT_TRUE(pma2 != pma3);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_ConstructWithLValue)
{
    using container = score::cpp::pmr::vector<int>;
    using allocator_type = score::cpp::pmr::polymorphic_allocator<container>;
    using allocator_traits = std::allocator_traits<allocator_type>;

    test_memory_resource mr{};
    allocator_type pma{&mr};
    container* p{allocator_traits::allocate(pma, 1)};
    const container original{1, 23, score::cpp::pmr::new_delete_resource()};
    allocator_traits::construct(pma, p, original);
    EXPECT_EQ(p->get_allocator(), pma);
    EXPECT_EQ(p->at(0), 23);
    allocator_traits::destroy(pma, p);
    allocator_traits::deallocate(pma, p, 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_ConstructWithRValue)
{
    using container = score::cpp::pmr::vector<int>;
    using allocator_type = score::cpp::pmr::polymorphic_allocator<container>;
    using allocator_traits = std::allocator_traits<allocator_type>;

    test_memory_resource mr{};
    allocator_type pma{&mr};
    container* p{allocator_traits::allocate(pma, 1)};
    container original{1, 23, score::cpp::pmr::new_delete_resource()};
    allocator_traits::construct(pma, p, std::move(original));
    EXPECT_EQ(p->get_allocator(), pma);
    EXPECT_EQ(p->at(0), 23);
    allocator_traits::destroy(pma, p);
    allocator_traits::deallocate(pma, p, 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_ConstructUnorderedMap)
{
    using container = score::cpp::pmr::unordered_map<int, score::cpp::pmr::string>;
    using allocator_type = score::cpp::pmr::polymorphic_allocator<container>;
    using allocator_traits = std::allocator_traits<allocator_type>;

    test_memory_resource mr{};
    allocator_type pma{&mr};
    container* p{allocator_traits::allocate(pma, 1)};
    container original{score::cpp::pmr::new_delete_resource()};
    original[42] = "foo";
    allocator_traits::construct(pma, p, original);
    EXPECT_EQ(p->get_allocator(), pma);
    EXPECT_EQ(p->at(42).get_allocator(), pma);
    EXPECT_EQ(p->at(42), "foo");
    allocator_traits::destroy(pma, p);
    allocator_traits::deallocate(pma, p, 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_WhenDefaultConstructed_UsesDefaultResource)
{
    EXPECT_EQ(score::cpp::pmr::polymorphic_allocator<>{}.resource(), score::cpp::pmr::get_default_resource());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_DoesNotPropagateOnCopyConstruction)
{
    test_memory_resource mr{};
    score::cpp::pmr::vector<int> original{&mr};
    score::cpp::pmr::vector<int> copy{original};
    EXPECT_NE(copy.get_allocator().resource(), &mr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_DoesNotPropagateOnCopyAssignment)
{
    test_memory_resource mr{};
    score::cpp::pmr::vector<int> original{&mr};
    score::cpp::pmr::vector<int> copy;
    copy = original;
    EXPECT_NE(copy.get_allocator().resource(), &mr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_DoesNotPropagateOnMoveAssignment)
{
    test_memory_resource mr{};
    score::cpp::pmr::vector<int> original{&mr};
    score::cpp::pmr::vector<int> copy;
    copy = std::move(original);
    EXPECT_NE(copy.get_allocator().resource(), &mr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_PropagatesOnMoveConstruction)
{
    test_memory_resource mr{};
    score::cpp::pmr::vector<int> original{&mr};
    score::cpp::pmr::vector<int> copy{std::move(original)};
    EXPECT_EQ(copy.get_allocator().resource(), &mr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338042
TEST(PmrTest, PolymorphicAllocator_AllocateDeallocate)
{
    struct alignas(16) test_type
    {
        char c[32];
    };
    test_memory_resource mr;
    score::cpp::pmr::polymorphic_allocator<test_type> pma(&mr);

    test_type* p1 = pma.allocate(1);
    EXPECT_NE(p1, nullptr);
    test_type* p2 = pma.allocate(5);
    EXPECT_NE(p2, nullptr);
    test_type* p3 = pma.allocate(33);
    EXPECT_NE(p3, nullptr);

    ASSERT_NE(mr.allocations.find(p1), mr.allocations.end());
    const test_memory_resource::alloc_info& ai1 = mr.allocations.find(p1)->second;
    EXPECT_EQ(ai1.bytes, 32);
    EXPECT_EQ(ai1.alignment, 16);
    EXPECT_FALSE(ai1.is_freed);

    ASSERT_NE(mr.allocations.find(p2), mr.allocations.end());
    const test_memory_resource::alloc_info& ai2 = mr.allocations.find(p2)->second;
    EXPECT_EQ(ai2.bytes, 32 * 5);
    EXPECT_EQ(ai2.alignment, 16);
    EXPECT_FALSE(ai2.is_freed);

    ASSERT_NE(mr.allocations.find(p3), mr.allocations.end());
    const test_memory_resource::alloc_info& ai3 = mr.allocations.find(p3)->second;
    EXPECT_EQ(ai3.bytes, 32 * 33);
    EXPECT_EQ(ai3.alignment, 16);
    EXPECT_FALSE(ai3.is_freed);

    pma.deallocate(p1, 1);
    EXPECT_TRUE(ai1.is_freed);
    EXPECT_FALSE(ai2.is_freed);
    EXPECT_FALSE(ai3.is_freed);

    pma.deallocate(p3, 33);
    EXPECT_TRUE(ai1.is_freed);
    EXPECT_FALSE(ai2.is_freed);
    EXPECT_TRUE(ai3.is_freed);

    pma.deallocate(p2, 5);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18437843
TEST(PmrTest, ResourceApaptor_Construction)
{
    score::cpp::pmr::resource_adaptor<test_allocator<char>> ra{};

    bool was_hit = false;
    mock_allocate = [&was_hit](std::size_t, std::size_t) -> void* {
        was_hit = true;
        return nullptr;
    };

    EXPECT_FALSE(ra.get_allocator().allocate(1));
    EXPECT_TRUE(was_hit);
    mock_allocate = nullptr;
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18437843
TEST(PmrTest, ResourceApaptor_CopyConstruction)
{
    score::cpp::pmr::resource_adaptor<test_allocator<char>> ra_orig{};
    score::cpp::pmr::resource_adaptor<test_allocator<char>> ra{ra_orig};

    bool was_hit = false;
    mock_allocate = [&was_hit](std::size_t, std::size_t) -> void* {
        was_hit = true;
        return nullptr;
    };

    EXPECT_FALSE(ra.get_allocator().allocate(1));
    EXPECT_TRUE(was_hit);
    mock_allocate = nullptr;
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18437843
TEST(PmrTest, ResourceApaptor_MoveConstruction)
{
    score::cpp::pmr::resource_adaptor<test_allocator<char>> ra_orig{};
    score::cpp::pmr::resource_adaptor<test_allocator<char>> ra{std::move(ra_orig)};

    bool was_hit = false;
    mock_allocate = [&was_hit](std::size_t, std::size_t) -> void* {
        was_hit = true;
        return nullptr;
    };

    EXPECT_FALSE(ra.get_allocator().allocate(1));
    EXPECT_TRUE(was_hit);
    mock_allocate = nullptr;
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18437843
TEST(PmrTest, ResourceApaptor_ConstructionFromAllocator)
{
    test_memory_resource mr;
    score::cpp::pmr::polymorphic_allocator<char> pma{&mr};
    score::cpp::pmr::resource_adaptor<score::cpp::pmr::polymorphic_allocator<char>> ra{pma};

    EXPECT_EQ(*ra.get_allocator().resource(), mr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18437843
TEST(PmrTest, ResourceApaptor_MoveConstructionFromAllocator)
{
    test_memory_resource mr;
    score::cpp::pmr::polymorphic_allocator<char> pma{&mr};
    score::cpp::pmr::resource_adaptor<score::cpp::pmr::polymorphic_allocator<char>> ra{std::move(pma)};

    EXPECT_EQ(*ra.get_allocator().resource(), mr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18437843
TEST(PmrTest, ResourceApaptor_CopyAssignment)
{
    test_allocator<score::cpp::pmr::detail::resource_adaptor_base_type> at;
    at.field = 42;
    score::cpp::pmr::resource_adaptor<test_allocator<score::cpp::pmr::detail::resource_adaptor_base_type>> ra1{};
    score::cpp::pmr::resource_adaptor<test_allocator<score::cpp::pmr::detail::resource_adaptor_base_type>> ra2{at};

    EXPECT_EQ(ra1.get_allocator().field, 0);
    ra1 = ra2;
    EXPECT_EQ(ra1.get_allocator().field, 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18437843
TEST(PmrTest, ResourceApaptor_AllocateDeallocate)
{
    score::cpp::pmr::resource_adaptor<test_allocator<char>> ra{};

    constexpr std::size_t low_alignment = 8;
    static_assert(low_alignment < alignof(std::max_align_t),
                  "Alignment must be below maximum natural alignment for this test.");
    bool was_called = false;
    mock_allocate = [&was_called](std::size_t size, std::size_t align) {
        EXPECT_EQ(size, 512 / sizeof(decltype(ra)::allocator_type::value_type));
        EXPECT_EQ(align, alignof(decltype(ra)::allocator_type::value_type));
        was_called = true;
        void* ret;
        std::uintptr_t iret = 42;
        std::memcpy(&ret, &iret, sizeof(ret));
        return ret;
    };

    EXPECT_FALSE(was_called);
    void* res = ra.allocate(512, low_alignment);
    EXPECT_TRUE(was_called);
    std::uintptr_t ires;
    std::memcpy(&ires, &res, sizeof(ires));
    EXPECT_EQ(ires, 42);

    mock_allocate = nullptr;
    was_called = false;

    mock_deallocate = [&was_called](void* ptr, std::size_t size, std::size_t align) {
        std::uintptr_t iptr;
        std::memcpy(&iptr, &ptr, sizeof(ires));
        EXPECT_EQ(iptr, 42);
        EXPECT_EQ(size, 512 / sizeof(decltype(ra)::allocator_type::value_type));
        EXPECT_EQ(align, alignof(decltype(ra)::allocator_type::value_type));
        was_called = true;
    };

    EXPECT_FALSE(was_called);
    ra.deallocate(res, 512, low_alignment);
    EXPECT_TRUE(was_called);

    mock_deallocate = nullptr;
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18437843
TEST(PmrTest, ResourceApaptor_AllocateZeroBytesDoesNotUnderflow)
{
    score::cpp::pmr::resource_adaptor<test_allocator<char>> ra{};

    bool was_called = false;
    size_t allocated_bytes{};
    mock_allocate = [&was_called, &allocated_bytes](std::size_t bytes, std::size_t) {
        was_called = true;
        allocated_bytes = bytes;
        void* ret;
        std::uintptr_t iret = 42;
        std::memcpy(&ret, &iret, sizeof(ret));
        return ret;
    };

    EXPECT_FALSE(was_called);
    void* res = ra.allocate(0, 1);
    EXPECT_TRUE(was_called);
    EXPECT_EQ(allocated_bytes, 1UL);
    std::uintptr_t ires;
    std::memcpy(&ires, &res, sizeof(ires));
    EXPECT_EQ(ires, 42);

    mock_allocate = nullptr;
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18437843
TEST(PmrTest, ResourceApaptor_AllocateDeallocateExtendedAlignment)
{
    score::cpp::pmr::resource_adaptor<test_allocator<std::max_align_t>> ra{};

    constexpr std::size_t high_alignment = 64;
    static_assert(high_alignment > alignof(std::max_align_t),
                  "Alignment must be above maximum natural alignment for this test.");

    bool was_called = false;
    void* orig_ptr = nullptr;
    mock_allocate = [&was_called, &orig_ptr](std::size_t size, std::size_t align) {
        EXPECT_EQ(size, 13);
        EXPECT_EQ(align, alignof(decltype(ra)::allocator_type::value_type));
        was_called = true;
        orig_ptr = new decltype(ra)::allocator_type::value_type[size];
        return orig_ptr;
    };

    EXPECT_FALSE(was_called);
    void* res = ra.allocate(128, high_alignment);
    EXPECT_TRUE(was_called);
    EXPECT_NE(res, nullptr);
    EXPECT_TRUE(is_aligned(res, high_alignment));

    mock_allocate = nullptr;
    was_called = false;

    mock_deallocate = [&was_called, orig_ptr](void* ptr, std::size_t size, std::size_t align) {
        EXPECT_EQ(ptr, orig_ptr);
        EXPECT_EQ(size, 13);
        EXPECT_EQ(align, alignof(decltype(ra)::allocator_type::value_type));
        was_called = true;
    };

    EXPECT_FALSE(was_called);
    ra.deallocate(res, 128, high_alignment);
    EXPECT_TRUE(was_called);

    mock_deallocate = nullptr;
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18437843
TEST(PmrTest, ResourceAdaptor_Equality)
{
    test_memory_resource mr;
    score::cpp::pmr::polymorphic_allocator<char> pma{&mr};
    score::cpp::pmr::resource_adaptor<score::cpp::pmr::polymorphic_allocator<char>> ra1{std::move(pma)};

    test_allocator<score::cpp::pmr::detail::resource_adaptor_base_type> ta;
    score::cpp::pmr::resource_adaptor<test_allocator<score::cpp::pmr::detail::resource_adaptor_base_type>> ra2{ta};

    EXPECT_EQ(ra2, ra2);
    EXPECT_NE(ra2, ra1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338043
TEST(PmrTest, new_delete_resource_allocate_forwards_to_global_new)
{
    score::cpp::pmr::memory_resource* ndr = score::cpp::pmr::new_delete_resource();
    const size_t requested_size = 112;
    void* pt = ndr->allocate(requested_size);
    EXPECT_NE(pt, nullptr);
    std::memset(pt, 42, requested_size);
    ndr->deallocate(pt, requested_size);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338043
TEST(PmrTest, new_delete_resource_extended_alignment)
{
    score::cpp::pmr::memory_resource* ndr = score::cpp::pmr::new_delete_resource();
    ASSERT_GE(64, alignof(std::max_align_t));
    void* pt = ndr->allocate(112, 64);
    EXPECT_NE(pt, nullptr);
    // check for the extended alignment mechanism established through
    // score::cpp::pmr::detail::establish_extended_alignment()
    void* unaligned_ptr = score::cpp::pmr::detail::retrieve_unaligned_pointer(pt);
    EXPECT_GE(static_cast<char*>(pt) - static_cast<char*>(unaligned_ptr), 0);
    EXPECT_LE(static_cast<char*>(pt) - static_cast<char*>(unaligned_ptr), 64);
    ndr->deallocate(pt, 112, 64);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338043
TEST(PmrTest, new_delete_resource_compares_equal_only_to_itself)
{
    score::cpp::pmr::memory_resource* nmr1 = score::cpp::pmr::new_delete_resource();
    score::cpp::pmr::memory_resource* nmr2 = score::cpp::pmr::new_delete_resource();
    EXPECT_EQ(*nmr1, *nmr2);
    test_memory_resource test_resource;
    EXPECT_NE(*nmr1, test_resource);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338043
TEST(PmrTest, null_memory_resource_allocate_throws_bad_alloc)
{
    score::cpp::pmr::memory_resource* nmr = score::cpp::pmr::null_memory_resource();
    ASSERT_THROW(nmr->allocate(1), std::bad_alloc);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338043
TEST(PmrTest, null_memory_resource_deallocate_does_nothing)
{
    // we cannot really test this reliably, so we just throw a bunch of values
    // at it that are likely to cause memory corruption in a faulty implementation
    score::cpp::pmr::memory_resource* nmr = score::cpp::pmr::null_memory_resource();
    int i = 42;
    nmr->deallocate(&i, std::numeric_limits<size_t>::max());
    EXPECT_EQ(i, 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338043
TEST(PmrTest, null_memory_resource_deallocate_contract_violated_when_nullptr)
{
    score::cpp::pmr::memory_resource* nmr = score::cpp::pmr::null_memory_resource();
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(nmr->deallocate(nullptr, 23U));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338043
TEST(PmrTest, null_memory_resource_compares_equal_only_to_itself)
{
    score::cpp::pmr::memory_resource* nmr1 = score::cpp::pmr::null_memory_resource();
    score::cpp::pmr::memory_resource* nmr2 = score::cpp::pmr::null_memory_resource();
    EXPECT_EQ(*nmr1, *nmr2);
    test_memory_resource test_resource;
    EXPECT_NE(*nmr1, test_resource);
}

score::cpp::pmr::unique_ptr<int> deleted_during_global_destruction;

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338043
TEST(PmrTest, new_delete_resource_is_never_destroyed)
{
    deleted_during_global_destruction = score::cpp::pmr::make_unique<int>(score::cpp::pmr::new_delete_resource());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338043
TEST(PmrTest, default_resource)
{
    // The default resource is initially the new_delete_resource.
    EXPECT_EQ(score::cpp::pmr::get_default_resource(), score::cpp::pmr::new_delete_resource());

    // set_default_resource changes the default resource and returns the previous default resource.
    EXPECT_EQ(score::cpp::pmr::set_default_resource(score::cpp::pmr::null_memory_resource()), score::cpp::pmr::new_delete_resource());
    EXPECT_EQ(score::cpp::pmr::get_default_resource(), score::cpp::pmr::null_memory_resource());

    // Given nullptr set_default_resource resets to new_delete_resource.
    score::cpp::pmr::set_default_resource(nullptr);
    EXPECT_EQ(score::cpp::pmr::get_default_resource(), score::cpp::pmr::new_delete_resource());

    // Note that we have reset the default memory resource to the initial value, ensuring test hermeticity.
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18439170
TEST(chunk_size_and_alignment, WhenConstructAndSizeIsNotMultipleOf64ThenPanic)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::pmr::detail::chunk_size_and_alignment{63U, 1U}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18439170
TEST(chunk_size_and_alignment, WhenConstructAndAlignmentIsNotPowerOf2ThenPanic)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::pmr::detail::chunk_size_and_alignment{64U, 3U}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18439170
TEST(chunk_size_and_alignment, WhenConstructThenGetSizeAndGetAlignmentReturnPassedValues)
{
    constexpr std::size_t max_size{score::cpp::align_down(std::numeric_limits<std::size_t>::max(), 64_UZ)};
    constexpr std::size_t max_alignment{1_UZ << 63U};
    score::cpp::pmr::detail::chunk_size_and_alignment unit{max_size, max_alignment};

    EXPECT_EQ(unit.get_size(), max_size);
    EXPECT_EQ(unit.get_alignment(), max_alignment);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST(monotonic_buffer_resource, WhenConstructAndNoUpstreamResourceThenUseDefaultResource)
{
    score::cpp::pmr::monotonic_buffer_resource unit{};
    EXPECT_EQ(unit.upstream_resource(), score::cpp::pmr::get_default_resource());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST(monotonic_buffer_resource, WhenConstructAndUpstreamResourceIsNullptrThenPanic)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::pmr::monotonic_buffer_resource{nullptr});
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::pmr::monotonic_buffer_resource{1U, nullptr}));
    std::uint8_t buffer[1U];
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::pmr::monotonic_buffer_resource{std::begin(buffer), sizeof(buffer), nullptr}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST(monotonic_buffer_resource, WhenConstructAndInitialSizeIsZeroThenPanic)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::pmr::monotonic_buffer_resource{0_UZ});
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::pmr::monotonic_buffer_resource{0U, score::cpp::pmr::get_default_resource()}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST(monotonic_buffer_resource, WhenConstructAndBufferSizePairInconsistentThenPanic)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::pmr::monotonic_buffer_resource{nullptr, 1U}));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::pmr::monotonic_buffer_resource{nullptr, 1U, score::cpp::pmr::get_default_resource()}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST(monotonic_buffer_resource, SameObjectsAreEqual)
{
    score::cpp::pmr::monotonic_buffer_resource unit{};
    EXPECT_TRUE(unit == unit);
    EXPECT_FALSE(unit != unit);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST(monotonic_buffer_resource, DifferentObjectsAreUnequal)
{
    score::cpp::pmr::monotonic_buffer_resource unit{};
    score::cpp::pmr::monotonic_buffer_resource other{};
    EXPECT_FALSE(unit == other);
    EXPECT_TRUE(unit != other);
}

struct monotonic_buffer_resource_test : testing::Test
{
    test_memory_resource upstream{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(monotonic_buffer_resource_test, WhenConstructWithoutInitialSizeArgumentThenUseImplementationDefinedValue)
{
    score::cpp::pmr::monotonic_buffer_resource unit{&upstream};
    void* const p{unit.allocate(1U, 1U)};
    EXPECT_EQ(upstream.allocations.size(), 1U);
    const test_memory_resource::alloc_info allocation{upstream.allocations[p]};
    EXPECT_EQ(allocation.alignment, 8U);
    EXPECT_EQ(allocation.bytes, score::cpp::pmr::detail::chunk_list::internal_size(4096U));
    EXPECT_FALSE(allocation.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(monotonic_buffer_resource_test, WhenConstructAndInitialSizeIsSmallerThanDefaultThenUseDefault)
{
    score::cpp::pmr::monotonic_buffer_resource unit{1_UZ, &upstream};
    void* const p{unit.allocate(1U, 1U)};
    const test_memory_resource::alloc_info allocation{upstream.allocations.at(p)};
    EXPECT_EQ(allocation.alignment, 8U);
    EXPECT_EQ(allocation.bytes, score::cpp::pmr::detail::chunk_list::internal_size(4096U));
    EXPECT_FALSE(allocation.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(monotonic_buffer_resource_test, DeallocateIsANoOp)
{
    score::cpp::pmr::monotonic_buffer_resource unit{&upstream};
    void* const p{unit.allocate(42U, 8U)};
    auto before = upstream.allocations;
    unit.deallocate(p, 42U, 8U);
    auto after = upstream.allocations;
    EXPECT_EQ(before, after);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(monotonic_buffer_resource_test, WhenConstructedWithEmptyBufferSizePairThenAllocateReturnsMemoryFromUpstream)
{
    score::cpp::pmr::monotonic_buffer_resource unit{nullptr, 0U, &upstream};
    void* const p{unit.allocate(1U, 1U)};
    const test_memory_resource::alloc_info allocation{upstream.allocations.at(p)};
    EXPECT_EQ(allocation.alignment, 8U);
    EXPECT_EQ(allocation.bytes, score::cpp::pmr::detail::chunk_list::internal_size(4096U));
    EXPECT_FALSE(allocation.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(monotonic_buffer_resource_test,
       WhenConstructedWithEmptyBufferSizePairAndReleaseThenAllocateReturnsMemoryFromUpstream)
{
    score::cpp::pmr::monotonic_buffer_resource unit{nullptr, 0U, &upstream};
    unit.release();
    void* const p{unit.allocate(1U, 1U)};
    const test_memory_resource::alloc_info allocation{upstream.allocations.at(p)};
    EXPECT_EQ(allocation.alignment, 8U);
    EXPECT_EQ(allocation.bytes, score::cpp::pmr::detail::chunk_list::internal_size(4096U));
    EXPECT_FALSE(allocation.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(monotonic_buffer_resource_test,
       WhenConstructedWithNonNullBufferButSizeIsZeroThenAllocateReturnsMemoryFromUpstream)
{
    std::uint8_t buffer;
    score::cpp::pmr::monotonic_buffer_resource unit{&buffer, 0U, &upstream};
    void* const p{unit.allocate(1U, 1U)};
    const test_memory_resource::alloc_info allocation{upstream.allocations.at(p)};
    EXPECT_EQ(allocation.alignment, 8U);
    EXPECT_EQ(allocation.bytes, score::cpp::pmr::detail::chunk_list::internal_size(4096U));
    EXPECT_FALSE(allocation.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(monotonic_buffer_resource_test,
       WhenConstructedWithNonNullBufferButSizeIs0AndReleaseThenAllocateReturnsMemoryFromUpstream)
{
    std::uint8_t buffer;
    score::cpp::pmr::monotonic_buffer_resource unit{&buffer, 0U, &upstream};
    unit.release();
    void* const p{unit.allocate(1U, 1U)};
    const test_memory_resource::alloc_info allocation{upstream.allocations.at(p)};
    EXPECT_EQ(allocation.alignment, 8U);
    EXPECT_EQ(allocation.bytes, score::cpp::pmr::detail::chunk_list::internal_size(4096U));
    EXPECT_FALSE(allocation.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(monotonic_buffer_resource_test, WhenConstructedWithInitialSizeAndReleaseAndAllocateThenInitialSizeIsUsed)
{
    const std::size_t initial_size{12345U};
    ASSERT_NE(score::cpp::pmr::detail::chunk_list::internal_size(initial_size),
              score::cpp::pmr::detail::chunk_list::internal_size(2U * initial_size));
    score::cpp::pmr::monotonic_buffer_resource unit{initial_size, &upstream};
    unit.release();
    void* const p{unit.allocate(1U, 1U)};
    const test_memory_resource::alloc_info allocation1{upstream.allocations.at(p)};
    EXPECT_EQ(allocation1.alignment, score::cpp::pmr::detail::chunk_list::internal_alignment(1U));
    EXPECT_EQ(allocation1.bytes, score::cpp::pmr::detail::chunk_list::internal_size(initial_size));
    EXPECT_FALSE(allocation1.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(monotonic_buffer_resource_test, WhenAlignMemoryBufferThenCurrentBufferIsShifted)
{
    const std::size_t initial_size{12345U};
    score::cpp::pmr::monotonic_buffer_resource unit{initial_size, &upstream};
    void* const not_aligned_buffer{unit.allocate(1U, 1U)}; // move current_buffer_ 1 byte to make it not aligned
    void* const buffer_to_use{unit.allocate(8U, 8U)};
    void* const buffer_for_next_usage{unit.allocate(1U, 1U)};
    EXPECT_EQ(buffer_to_use, reinterpret_cast<std::uint8_t*>(not_aligned_buffer) + 8);
    EXPECT_EQ(buffer_for_next_usage, reinterpret_cast<std::uint8_t*>(buffer_to_use) + 8);
}

struct fresh_with_initial_size : monotonic_buffer_resource_test
{
    test_memory_resource upstream{};
    const std::size_t initial_size{12345U};
    score::cpp::pmr::monotonic_buffer_resource unit{initial_size, &upstream};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(fresh_with_initial_size, WhenAllocateThenAllocateInitialSizeFromUpstream)
{
    void* const p1{unit.allocate(24U, 8U)};
    EXPECT_EQ(upstream.allocations.size(), 1U);
    const test_memory_resource::alloc_info allocation1{upstream.allocations[p1]};
    EXPECT_EQ(allocation1.alignment, 8U);
    EXPECT_EQ(allocation1.bytes, score::cpp::pmr::detail::chunk_list::internal_size(initial_size));
    EXPECT_FALSE(allocation1.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(fresh_with_initial_size, WhenAllocateAndSizeIsGreaterThanCurrentBufferSizeThenNextBufferIsEvenBigger)
{
    const std::size_t more_than_initial{2U * initial_size};
    ASSERT_NE(score::cpp::pmr::detail::chunk_list::internal_size(2U * more_than_initial),
              score::cpp::pmr::detail::chunk_list::internal_size(2U * initial_size));
    void* const p1{unit.allocate(more_than_initial, 8U)};
    EXPECT_EQ(upstream.allocations.size(), 1U);
    const test_memory_resource::alloc_info allocation1{upstream.allocations.at(p1)};
    EXPECT_EQ(allocation1.alignment, 8U);
    EXPECT_EQ(allocation1.bytes, score::cpp::pmr::detail::chunk_list::internal_size(more_than_initial));
    EXPECT_FALSE(allocation1.is_freed);

    void* const p2{unit.allocate(1U, 8U)};
    EXPECT_EQ(upstream.allocations.size(), 2U);
    const test_memory_resource::alloc_info allocation2{upstream.allocations.at(p2)};
    EXPECT_EQ(allocation2.alignment, 8U);
    EXPECT_EQ(allocation2.bytes, score::cpp::pmr::detail::chunk_list::internal_size(2U * more_than_initial));
    EXPECT_FALSE(allocation2.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(fresh_with_initial_size, WhenAllocateAndAlignmentIsNotPowerOfTwoThenPanic)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(unit.allocate(42U, 23U));
}

struct given_initial_size_and_buffer_has_been_allocated : fresh_with_initial_size
{
    void SetUp() override
    {
        fresh_with_initial_size::SetUp();
        p1 = unit.allocate(already_allocated, 8U);
    }
    const std::size_t already_allocated{16U};
    void* p1{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(given_initial_size_and_buffer_has_been_allocated, WhenAllocateAndItFitsThenUseBuffer)
{
    void* const p2{unit.allocate(initial_size - already_allocated, 8U)};
    EXPECT_EQ(upstream.allocations.size(), 1U);
    EXPECT_EQ(p2, reinterpret_cast<std::uint8_t*>(p1) + already_allocated);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(given_initial_size_and_buffer_has_been_allocated, WhenReleaseThenReturnMemoryToUpstream)
{
    unit.release();
    EXPECT_EQ(upstream.allocations.size(), 1U);
    EXPECT_TRUE(upstream.allocations[p1].is_freed);
}

struct given_initial_size_and_buffer_is_depleted : given_initial_size_and_buffer_has_been_allocated
{
    void SetUp() override
    {
        given_initial_size_and_buffer_has_been_allocated::SetUp();
        p2 = unit.allocate(initial_size - already_allocated, 8U);
        ASSERT_EQ(upstream.allocations.size(), 1U);
    }
    void* p2{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(given_initial_size_and_buffer_is_depleted, WhenAllocateThenReplenishByAllocatingDoubleFromUpstream)
{
    void* const p3{unit.allocate(1U, 1U)};
    EXPECT_EQ(upstream.allocations.size(), 2U);
    const test_memory_resource::alloc_info allocation2{upstream.allocations[p3]};
    EXPECT_EQ(allocation2.alignment, score::cpp::pmr::detail::chunk_list::internal_alignment(1U));
    EXPECT_EQ(allocation2.bytes, score::cpp::pmr::detail::chunk_list::internal_size(2U * initial_size));
    EXPECT_FALSE(allocation2.is_freed);
}

struct freshly_allocated_with_buffer_size_pair : monotonic_buffer_resource_test
{
    freshly_allocated_with_buffer_size_pair()
        : buffer(12345U)
        , buffer_start{buffer.data()}
        , buffer_size{buffer.size()}
        , unit{buffer_start, buffer_size, &upstream}
    {
    }

    std::vector<std::uint8_t> buffer;
    std::uint8_t* buffer_start;
    std::size_t buffer_size;
    score::cpp::pmr::monotonic_buffer_resource unit;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(freshly_allocated_with_buffer_size_pair, WhenAllocateAndItFitsThenUseBuffer)
{
    void* const p1{unit.allocate(35U, 8U)};
    EXPECT_GE(p1, buffer_start);
    EXPECT_LT(p1, buffer_start + 8U);
    EXPECT_TRUE(is_aligned(p1, 8U));
    EXPECT_TRUE(upstream.allocations.empty());
}

struct given_buffer_size_pair_and_original_buffer_is_depleted : freshly_allocated_with_buffer_size_pair
{
    void SetUp() override
    {
        freshly_allocated_with_buffer_size_pair::SetUp();
        p1 = unit.allocate(12345U, 1U);
    }
    void* p1{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(given_buffer_size_pair_and_original_buffer_is_depleted, WhenAllocateThenReplenishByAllocatingDoubleFromUpstream)
{
    void* const p2{unit.allocate(1U, 1U)};
    EXPECT_EQ(upstream.allocations.size(), 1U);
    const test_memory_resource::alloc_info allocation1{upstream.allocations[p2]};
    EXPECT_EQ(allocation1.bytes, score::cpp::pmr::detail::chunk_list::internal_size(2U * buffer_size));
    EXPECT_FALSE(allocation1.is_freed);
}

struct given_initial_size_and_released : given_initial_size_and_buffer_has_been_allocated
{
    void SetUp() override
    {
        given_initial_size_and_buffer_has_been_allocated::SetUp();
        unit.release();
    }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(given_initial_size_and_released, WhenAllocateThenReplenishInitialSizeFromUpstream)
{
    void* const p2{unit.allocate(1U, 1U)};
    const test_memory_resource::alloc_info allocation2{upstream.allocations[p2]};
    EXPECT_EQ(allocation2.bytes, score::cpp::pmr::detail::chunk_list::internal_size(initial_size));
    EXPECT_FALSE(allocation2.is_freed);
}

struct given_buffer_size_pair_and_replenished : given_buffer_size_pair_and_original_buffer_is_depleted
{
    void SetUp() override
    {
        given_buffer_size_pair_and_original_buffer_is_depleted::SetUp();
        p2 = unit.allocate(1U, 1U);
    }
    void* p2{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(given_buffer_size_pair_and_replenished, WhenReleaseThenReturnMemoryToUpstream)
{
    unit.release();
    EXPECT_EQ(upstream.allocations.size(), 1U);
    EXPECT_TRUE(upstream.allocations[p2].is_freed);
}

struct given_buffer_size_pair_and_released : given_buffer_size_pair_and_replenished
{
    void SetUp() override
    {
        given_buffer_size_pair_and_replenished::SetUp();
        unit.release();
    }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18219774
TEST_F(given_buffer_size_pair_and_released, WhenAllocateThenUseBuffer)
{
    auto before = upstream.allocations;
    void* const p3{unit.allocate(35U, 8U)};
    auto after = upstream.allocations;
    EXPECT_GE(p3, buffer_start);
    EXPECT_LT(p3, buffer_start + 8U);
    EXPECT_TRUE(is_aligned(p3, 8U));
    EXPECT_EQ(before, after);
}

struct unsynchronized_pool_resource_test : testing::Test
{
    test_memory_resource upstream{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(unsynchronized_pool_resource_test, UpstreamIsConstructorArgument)
{
    EXPECT_EQ(score::cpp::pmr::unsynchronized_pool_resource{&upstream}.upstream_resource(), &upstream);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(unsynchronized_pool_resource_test, WhenConstructAndUpstreamIsNullptrThenPanic)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::pmr::unsynchronized_pool_resource{nullptr}));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::pmr::unsynchronized_pool_resource{score::cpp::pmr::pool_options{}, nullptr}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(unsynchronized_pool_resource_test, WhenConstructAndUpstreamIsNotProvidedThenUseDefault)
{
    EXPECT_EQ(score::cpp::pmr::unsynchronized_pool_resource{}.upstream_resource(), score::cpp::pmr::get_default_resource());
    EXPECT_EQ(score::cpp::pmr::unsynchronized_pool_resource{score::cpp::pmr::pool_options{}}.upstream_resource(),
              score::cpp::pmr::get_default_resource());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(unsynchronized_pool_resource_test, WhenConstructAndOptionsAreNotProvidedThenUseDefault)
{
    using score::cpp::pmr::pool_options;

    const pool_options options1{score::cpp::pmr::unsynchronized_pool_resource{}.options()};

    EXPECT_EQ(options1.largest_required_pool_block, 8192U);
    EXPECT_EQ(options1.max_blocks_per_chunk, 8192U);

    const pool_options options2{score::cpp::pmr::unsynchronized_pool_resource{score::cpp::pmr::get_default_resource()}.options()};

    EXPECT_EQ(options2.largest_required_pool_block, 8192U);
    EXPECT_EQ(options2.max_blocks_per_chunk, 8192U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(unsynchronized_pool_resource_test,
       WhenConstructWithOptionsThenLargestRequiredPoolBlockIsRoundedUpToNextSupportedPoolSize)
{
    using score::cpp::pmr::pool_options;

    pool_options options{};
    options.largest_required_pool_block = 1U;
    options.max_blocks_per_chunk = 2U;
    const pool_options options1{score::cpp::pmr::unsynchronized_pool_resource{options}.options()};

    EXPECT_EQ(options1.largest_required_pool_block, 8U);
    EXPECT_EQ(options1.max_blocks_per_chunk, 2U);

    const pool_options options2{
        score::cpp::pmr::unsynchronized_pool_resource{options, score::cpp::pmr::get_default_resource()}.options()};

    EXPECT_EQ(options2.largest_required_pool_block, 8U);
    EXPECT_EQ(options2.max_blocks_per_chunk, 2U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(unsynchronized_pool_resource_test, WhenConstructAndMaxBlocksPerChunkIsZeroThenUseDefault)
{
    using score::cpp::pmr::pool_options;

    pool_options options{};
    options.largest_required_pool_block = 8U;
    options.max_blocks_per_chunk = 0U;
    const pool_options options1{score::cpp::pmr::unsynchronized_pool_resource{options}.options()};

    EXPECT_EQ(options1.largest_required_pool_block, 8U);
    EXPECT_EQ(options1.max_blocks_per_chunk, 8192U);

    const pool_options options2{
        score::cpp::pmr::unsynchronized_pool_resource{options, score::cpp::pmr::get_default_resource()}.options()};

    EXPECT_EQ(options2.largest_required_pool_block, 8U);
    EXPECT_EQ(options2.max_blocks_per_chunk, 8192U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(unsynchronized_pool_resource_test, WhenConstructAndLargestRequiredPoolBlockIsZeroThenUseDefault)
{
    using score::cpp::pmr::pool_options;

    pool_options options{};
    options.largest_required_pool_block = 0U;
    options.max_blocks_per_chunk = 1U;
    const pool_options options1{score::cpp::pmr::unsynchronized_pool_resource{options}.options()};

    EXPECT_EQ(options1.largest_required_pool_block, 8192U);
    EXPECT_EQ(options1.max_blocks_per_chunk, 1U);

    const pool_options options2{
        score::cpp::pmr::unsynchronized_pool_resource{options, score::cpp::pmr::get_default_resource()}.options()};

    EXPECT_EQ(options2.largest_required_pool_block, 8192U);
    EXPECT_EQ(options2.max_blocks_per_chunk, 1U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(unsynchronized_pool_resource_test, WhenConstructAndLargestRequiredPoolBlockIsTooLargeThenUseMaximum)
{
    using score::cpp::pmr::pool_options;

    pool_options options{};
    options.largest_required_pool_block = std::numeric_limits<std::size_t>::max();
    options.max_blocks_per_chunk = 1U;
    const pool_options options1{score::cpp::pmr::unsynchronized_pool_resource{options}.options()};

    EXPECT_EQ(options1.largest_required_pool_block, 1U << 22U);
    EXPECT_EQ(options1.max_blocks_per_chunk, 1U);

    const pool_options options2{
        score::cpp::pmr::unsynchronized_pool_resource{options, score::cpp::pmr::get_default_resource()}.options()};

    EXPECT_EQ(options2.largest_required_pool_block, 1U << 22U);
    EXPECT_EQ(options2.max_blocks_per_chunk, 1U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(unsynchronized_pool_resource_test, SameObjectsAreEqual)
{
    score::cpp::pmr::unsynchronized_pool_resource unit{};
    EXPECT_TRUE(unit == unit);
    EXPECT_FALSE(unit != unit);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(unsynchronized_pool_resource_test, DifferentObjectsAreUnequal)
{
    score::cpp::pmr::unsynchronized_pool_resource unit{};
    score::cpp::pmr::unsynchronized_pool_resource other{};
    EXPECT_FALSE(unit == other);
    EXPECT_TRUE(unit != other);
}

struct freshly_constructed : unsynchronized_pool_resource_test
{
    std::size_t max_blocks_per_chunk{200U};
    std::size_t largest_required_pool_block{42U};
    score::cpp::pmr::unsynchronized_pool_resource unit{{max_blocks_per_chunk, largest_required_pool_block}, &upstream};
    const std::size_t bigblock_size{unit.options().largest_required_pool_block + 1U};
    const std::size_t min_chunk_size{4096U};
    const std::size_t initial_48_byte_blocks_per_chunk{min_chunk_size / 48U};
    const std::size_t initial_chunk_size_for_48_byte_blocks{
        score::cpp::pmr::detail::chunk_list::internal_size(48U * initial_48_byte_blocks_per_chunk)};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(freshly_constructed, WhenAllocateAndSizeIsGreaterThanLargestRequiredPoolBlockThenAllocateUpstream)
{
    void* const p{unit.allocate(bigblock_size, 8U)};
    EXPECT_EQ(upstream.allocations.size(), 1U);
    const test_memory_resource::alloc_info allocation{upstream.allocations[p]};
    EXPECT_EQ(allocation.bytes, score::cpp::pmr::detail::chunk_list::internal_size(bigblock_size));
    EXPECT_EQ(allocation.alignment, 8U);
    EXPECT_FALSE(allocation.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(freshly_constructed, WhenAllocateAndFitsInPoolThenPoolIsAllocated)
{
    void* const p{unit.allocate(42U, 16U)};
    EXPECT_EQ(upstream.allocations.size(), 2U);
    const test_memory_resource::alloc_info allocation{upstream.allocations[p]};
    EXPECT_EQ(allocation.bytes, initial_chunk_size_for_48_byte_blocks);
    EXPECT_EQ(allocation.alignment, 16U);
    EXPECT_FALSE(allocation.is_freed);
    // The second allocation is the internal pool array, which we will not check in detail here.
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(freshly_constructed, WhenAllocateThenAlignmentIsRespected)
{
    EXPECT_TRUE(is_aligned(unit.allocate(24U, 16U), 16U));
    EXPECT_TRUE(is_aligned(unit.allocate(24U, 32U), 32U));

    EXPECT_TRUE(is_aligned(unit.allocate(48U, 32U), 32U));
    EXPECT_TRUE(is_aligned(unit.allocate(48U, 64U), 64U));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
struct big_block_allocated : freshly_constructed
{
    void SetUp() override
    {
        freshly_constructed::SetUp();
        p1 = unit.allocate(bigblock_size, 8U);
    }

    void* p1{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(big_block_allocated, WhenDeallocateAndSizeIsGreaterThanLargestRequiredPoolBlockThenDeallocateUpstream)
{
    unit.deallocate(p1, bigblock_size, 8U);
    EXPECT_EQ(upstream.allocations.size(), 1U);
    const test_memory_resource::alloc_info allocation{upstream.allocations[p1]};
    EXPECT_TRUE(allocation.is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
struct two_big_blocks_allocated : big_block_allocated
{
    void SetUp() override
    {
        big_block_allocated::SetUp();
        p2 = unit.allocate(49U, 8U);
        SCORE_LANGUAGE_FUTURECPP_ASSERT(p2 != p1);
        SCORE_LANGUAGE_FUTURECPP_ASSERT(upstream.allocations.size() == 2U);
        const test_memory_resource::alloc_info allocation{upstream.allocations[p2]};
        SCORE_LANGUAGE_FUTURECPP_ASSERT(allocation.bytes == score::cpp::pmr::detail::chunk_list::internal_size(49U));
        SCORE_LANGUAGE_FUTURECPP_ASSERT(allocation.alignment == 8U);
        SCORE_LANGUAGE_FUTURECPP_ASSERT(!allocation.is_freed);
    }

    void* p2{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(two_big_blocks_allocated, WhenDeallocateFirstThenDeallocateUpstream)
{
    unit.deallocate(p1, 49U, 8U);
    EXPECT_EQ(upstream.allocations.size(), 2U);
    EXPECT_TRUE(upstream.allocations[p1].is_freed);
    EXPECT_FALSE(upstream.allocations[p2].is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(two_big_blocks_allocated, WhenDeallocateSecondThenDeallocateUpstream)
{
    unit.deallocate(p2, 49U, 8U);
    EXPECT_EQ(upstream.allocations.size(), 2U);
    EXPECT_FALSE(upstream.allocations[p1].is_freed);
    EXPECT_TRUE(upstream.allocations[p2].is_freed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(two_big_blocks_allocated, WhenReleaseThenAllMemoryIsFreed)
{
    unit.release();
    for (auto& allocation : upstream.allocations)
    {
        EXPECT_TRUE(allocation.second.is_freed);
    }
}

struct pool_has_been_allocated : freshly_constructed
{
    void SetUp() override
    {
        freshly_constructed::SetUp();
        p1 = unit.allocate(42U, 8U);
    }

    void* p1{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(pool_has_been_allocated, WhenAllocateAndFitsInPoolThenPoolIsUsed)
{
    void* const p{unit.allocate(42U, 8U)};
    EXPECT_EQ(upstream.allocations.size(), 2U) << "There must be no new upstream allocation.";
    EXPECT_EQ(p, reinterpret_cast<std::uint8_t*>(p1) + 48U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(pool_has_been_allocated, WhenDeallocateAndFitsInPoolThenAllocationIsReturnedToFreeList)
{
    unit.deallocate(p1, 42U, 8U);
    EXPECT_EQ(upstream.allocations.size(), 2U);
    for (const auto& allocation : upstream.allocations)
    {
        EXPECT_FALSE(allocation.second.is_freed) << "There must be no upstream deallocation.";
    }
}

struct block_has_been_returned_to_free_list : pool_has_been_allocated
{
    void SetUp() override
    {
        pool_has_been_allocated::SetUp();
        unit.deallocate(p1, 42U, 8U);
    }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(block_has_been_returned_to_free_list, WhenAllocateAndFitsInPoolThenFreeListIsUsed)
{
    void* const p{unit.allocate(42U, 8U)};
    EXPECT_EQ(upstream.allocations.size(), 2U) << "There must be no new upstream allocation.";
    EXPECT_EQ(p, p1);
}

struct pool_has_been_depleted : pool_has_been_allocated
{
    void SetUp() override
    {
        pool_has_been_allocated::SetUp();
        // One block already has been allocated. We allocate the remaining `initial_48_b_blocks_per_chunk - 1` blocks.
        for (auto i = 0U; i < initial_48_byte_blocks_per_chunk - 1U; ++i)
        {
            score::cpp::ignore = unit.allocate(42U, 8U);
        }
        SCORE_LANGUAGE_FUTURECPP_ASSERT(score::cpp::ssize(upstream.allocations) == 2);
    }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(pool_has_been_depleted, WhenAllocateThenALargerChunkIsAllocated)
{
    void* const p{unit.allocate(42U, 16U)};
    EXPECT_EQ(upstream.allocations.size(), 3U);
    const test_memory_resource::alloc_info allocation{upstream.allocations[p]};
    EXPECT_EQ(allocation.bytes, score::cpp::pmr::detail::chunk_list::internal_size(48U * 170U));
    EXPECT_EQ(allocation.alignment, 16U);
    EXPECT_FALSE(allocation.is_freed);
}

struct two_pool_chunks_have_been_allocated : pool_has_been_depleted
{
    void SetUp() override
    {
        pool_has_been_depleted::SetUp();
        p2 = unit.allocate(42U, 8U);
        SCORE_LANGUAGE_FUTURECPP_ASSERT(score::cpp::ssize(upstream.allocations) == 3);
    }
    void* p2{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(two_pool_chunks_have_been_allocated, WhenReleaseThenAllMemoryIsFreed)
{
    unit.release();
    for (auto& allocation : upstream.allocations)
    {
        EXPECT_TRUE(allocation.second.is_freed);
    }
}

struct pools_have_been_released : two_pool_chunks_have_been_allocated
{
    void SetUp() override
    {
        two_pool_chunks_have_been_allocated::SetUp();
        unit.release();
    }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487572
TEST_F(pools_have_been_released, WhenAllocateAndFitsInPoolThenLastPoolSizeIsForgottenAndChunkWithInitialSizeIsAllocated)
{
    void* const p{unit.allocate(42U, 16U)};
    // We don't check the allocation count: The two new allocations (pools vector and 48 Byte pool) may or may not have
    // the same addresses as the old allocations depending on what malloc/new returns. Thus upstream.allocations.size()
    // may be anything between 3 and 5. This difference has actually been observed when testing with clang and gcc.
    const test_memory_resource::alloc_info allocation{upstream.allocations[p]};
    EXPECT_EQ(allocation.bytes, initial_chunk_size_for_48_byte_blocks);
    EXPECT_EQ(allocation.alignment, 16U);
    EXPECT_FALSE(allocation.is_freed);
}

std::ptrdiff_t get_pool_index_spec(const std::size_t bytes, const std::size_t alignment)
{
    const auto it = std::find_if(std::begin(score::cpp::pmr::detail::unsynchronized_pool_resource_utils::block_sizes),
                                 std::end(score::cpp::pmr::detail::unsynchronized_pool_resource_utils::block_sizes),
                                 [bytes, alignment](const std::size_t size) {
                                     return (size >= bytes) && score::cpp::pmr::detail::is_aligned(size, alignment);
                                 });
    return std::distance(std::begin(score::cpp::pmr::detail::unsynchronized_pool_resource_utils::block_sizes), it);
}

struct block_sizes : ::testing::TestWithParam<std::tuple<std::size_t, std::size_t>>
{
    std::size_t get_block_size() const { return std::get<0>(GetParam()); }
    std::size_t get_alignment() const { return 1UL << std::get<1>(GetParam()); }
};

INSTANTIATE_TEST_SUITE_P(block_sizes_small,
                         block_sizes,
                         testing::Combine(testing::Range(1UL, 1025UL), testing::Range(0UL, 11UL)));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18443680
TEST_P(block_sizes, Spec)
{
    EXPECT_EQ(score::cpp::pmr::detail::unsynchronized_pool_resource_utils::get_pool_index(get_block_size(), get_alignment()),
              get_pool_index_spec(get_block_size(), get_alignment()));
}

struct block_sizes_logarithmic : ::testing::TestWithParam<std::tuple<std::size_t, std::size_t>>
{
    std::size_t get_block_size() const { return 1UL << std::get<0>(GetParam()); }
    std::size_t get_alignment() const { return 1UL << std::get<1>(GetParam()); }
};

INSTANTIATE_TEST_SUITE_P(block_sizes_logarithmic,
                         block_sizes_logarithmic,
                         testing::Combine(testing::Range(0UL, 64UL), testing::Range(0UL, 64UL)));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18443680
TEST_P(block_sizes_logarithmic, Spec)
{
    EXPECT_EQ(score::cpp::pmr::detail::unsynchronized_pool_resource_utils::get_pool_index(get_block_size(), get_alignment()),
              get_pool_index_spec(get_block_size(), get_alignment()));
}

} // anonymous namespace
