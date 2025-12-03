///
/// \file
/// \copyright Copyright (C) 2017-2018, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/type_traits.hpp>
#include <score/type_traits.hpp> // test include guard

#include <array>
#include <cstdint>
#include <forward_list>
#include <functional>
#include <list>
#include <vector>

#include <gtest/gtest.h>

static_assert(std::is_same<score::cpp::remove_cvref_t<int>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<int&>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<int&&>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<const int>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<const int&>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<const int&&>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<int[2]>, int[2]>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<int (&)[2]>, int[2]>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<const int[2]>, int[2]>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<const int (&)[2]>, int[2]>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<volatile int>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<volatile int&>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<volatile int&&>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<volatile const int>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<volatile const int&>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<volatile const int&&>, int>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<volatile int[2]>, int[2]>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<volatile int (&)[2]>, int[2]>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<volatile const int[2]>, int[2]>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<volatile const int (&)[2]>, int[2]>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<int(int)>, int(int)>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<int(const int)>, int(int)>::value, "Failed");
static_assert(std::is_same<score::cpp::remove_cvref_t<int(volatile const int)>, int(int)>::value, "Failed");

namespace
{

struct test_functor
{
    test_functor(std::int32_t& value) : test_member(value) {}
    void operator()(std::int32_t value);

    std::int32_t& test_member;
};

struct test_functor_2
{
    void operator()(int, double) {}
    void operator()(int, float) {}
};

struct test_functor_3
{
    template <typename T>
    void operator()(T&&)
    {
    }
};

struct arithmetic_type
{
    arithmetic_type operator+(const arithmetic_type&) { return {}; }
    arithmetic_type operator-(const arithmetic_type&) { return {}; }
    arithmetic_type operator*(const arithmetic_type&) { return {}; }
    arithmetic_type operator/(const arithmetic_type&) { return {}; }
};

struct non_arithmetic_type
{
};

struct comparable_type
{
    bool operator<(const comparable_type&) const { return false; }
};

struct non_comparable_type
{
};

struct non_copy_constructible_hash_type
{
    non_copy_constructible_hash_type(const non_copy_constructible_hash_type&) = delete;

    template <typename T>
    std::size_t operator()(const T&) const;
};

struct non_destructible_hash_type
{
    ~non_destructible_hash_type() = delete;

    template <typename T>
    std::size_t operator()(const T&) const;
};

struct hash_type_with_invalid_overload_1
{
    template <typename T>
    void operator()(T&);
    template <typename T>
    std::size_t operator()(T&) const;
    template <typename T>
    std::size_t operator()(const T&);
    template <typename T>
    std::size_t operator()(const T&) const;
};

struct hash_type_with_invalid_overload_2
{
    template <typename T>
    std::size_t operator()(T&);
    template <typename T>
    void operator()(T&) const;
    template <typename T>
    std::size_t operator()(const T&);
    template <typename T>
    std::size_t operator()(const T&) const;
};

struct hash_type_with_invalid_overload_3
{
    template <typename T>
    std::size_t operator()(T&);
    template <typename T>
    std::size_t operator()(T&) const;
    template <typename T>
    void operator()(const T&);
    template <typename T>
    std::size_t operator()(const T&) const;
};

struct hash_type_with_invalid_overload_4
{
    template <typename T>
    std::size_t operator()(T&);
    template <typename T>
    std::size_t operator()(T&) const;
    template <typename T>
    std::size_t operator()(const T&);
    template <typename T>
    void operator()(const T&) const;
};

struct valid_hash_type
{
    template <typename T>
    std::size_t operator()(const T&) const;
};

struct non_iterable_with_void_begin_end_type
{
    void begin() {}
    void end() {}

    void cbegin() const {}
    void cend() const {}
};

template <typename T>
struct iterable_type
{
    using iterator = T*;
    using const_iterator = T const*;

    iterator begin() { return std::begin(array_); }
    iterator end() { return std::end(array_); }

    const_iterator cbegin() const { return std::cbegin(array_); }
    const_iterator cend() const { return std::cend(array_); }

private:
    T array_[4]{};
};

///
/// \brief This 'simple_iterator' is based on the concept of 'Simple Input/Output Iterator', yet with an addition of the
/// '!=' not-equal comparison operator
///
/// \see https://en.cppreference.com/w/cpp/iterator/input_or_output_iterator.html
///
struct simple_iterator
{
    using difference_type = std::ptrdiff_t;

    int operator*() { return {}; }
    simple_iterator& operator++() { return *this; }
    void operator++(int) { ++*this; }
    bool operator!=(simple_iterator&) { return {}; }
};

///
/// \brief Non swappable iterator(almost) type
///
/// \see https://en.cppreference.com/w/cpp/algorithm/swap.html
///
struct simple_non_swappable_iterator
{
    using difference_type = std::ptrdiff_t;
    simple_non_swappable_iterator() = default;
    simple_non_swappable_iterator(const simple_non_swappable_iterator&) = default;
    simple_non_swappable_iterator(simple_non_swappable_iterator&&) = default;
    simple_non_swappable_iterator& operator=(const simple_non_swappable_iterator&) = default;
    // enough to render the type non-swappable
    simple_non_swappable_iterator& operator=(simple_non_swappable_iterator&&) = delete;

    int operator*() { return {}; }
    simple_non_swappable_iterator& operator++() { return *this; }
    void operator++(int) { ++*this; }
    bool operator!=(simple_non_swappable_iterator&) { return {}; }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_addition, when_available)
{
    constexpr bool operator_addition_available{score::cpp::has_operator_addition<arithmetic_type>::value};
    static_assert(operator_addition_available, "'arithmetic_type' does not have operator+");
    static_assert(score::cpp::has_operator_addition_v<arithmetic_type>, "'arithmetic_type' does not have operator+");
    EXPECT_TRUE(operator_addition_available);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_addition, when_not_available)
{
    constexpr bool operator_addition_available{score::cpp::has_operator_addition<non_arithmetic_type>::value};
    static_assert(!operator_addition_available, "'non_arithmetic_type' has unexpected operator+");
    static_assert(!score::cpp::has_operator_addition_v<non_arithmetic_type>, "'non_arithmetic_type' has unexpected operator+");
    EXPECT_FALSE(operator_addition_available);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_subtraction, when_available)
{
    constexpr bool operator_subtraction_available{score::cpp::has_operator_subtraction<arithmetic_type>::value};
    static_assert(operator_subtraction_available, "'arithmetic_type' does not have operator-");
    static_assert(score::cpp::has_operator_subtraction_v<arithmetic_type>, "'arithmetic_type' does not have operator-");
    EXPECT_TRUE(operator_subtraction_available);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_subtraction, when_not_available)
{
    constexpr bool operator_subtraction_available{score::cpp::has_operator_subtraction<non_arithmetic_type>::value};
    static_assert(!operator_subtraction_available, "'non_arithmetic_type' has unexpected operator-");
    static_assert(!score::cpp::has_operator_subtraction_v<non_arithmetic_type>,
                  "'non_arithmetic_type' has unexpected operator-");
    EXPECT_FALSE(operator_subtraction_available);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_multiplication, when_available)
{
    constexpr bool operator_multiplication_available{score::cpp::has_operator_multiplication<arithmetic_type>::value};
    static_assert(operator_multiplication_available, "'arithmetic_type' does not have operator*");
    static_assert(score::cpp::has_operator_multiplication_v<arithmetic_type>, "'arithmetic_type' does not have operator*");
    EXPECT_TRUE(operator_multiplication_available);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_multiplication, when_not_available)
{
    constexpr bool operator_multiplication_available{score::cpp::has_operator_multiplication<non_arithmetic_type>::value};
    static_assert(!operator_multiplication_available, "'non_arithmetic_type' has unexpected operator*");
    static_assert(!score::cpp::has_operator_multiplication_v<non_arithmetic_type>,
                  "'non_arithmetic_type' has unexpected operator*");
    EXPECT_FALSE(operator_multiplication_available);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_division, when_available)
{
    constexpr bool operator_division_available{score::cpp::has_operator_division<arithmetic_type>::value};
    static_assert(operator_division_available, "'arithmetic_type' does not have operator/");
    static_assert(score::cpp::has_operator_division_v<arithmetic_type>, "'arithmetic_type' does not have operator/");
    EXPECT_TRUE(operator_division_available);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_division, when_not_available)
{
    constexpr bool operator_division_available{score::cpp::has_operator_division<non_arithmetic_type>::value};
    static_assert(!operator_division_available, "'non_arithmetic_type' has unexpected operator/");
    static_assert(!score::cpp::has_operator_division_v<non_arithmetic_type>, "'non_arithmetic_type' has unexpected operator/");
    EXPECT_FALSE(operator_division_available);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_less, when_available)
{
    constexpr bool operator_less_available{score::cpp::has_operator_less<comparable_type>::value};
    static_assert(operator_less_available, "'comparable_type' does not have operator<");
    static_assert(score::cpp::has_operator_less_v<comparable_type>, "'comparable_type' does not have operator<");
    EXPECT_TRUE(operator_less_available);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_less, when_not_available)
{
    constexpr bool operator_less_available{score::cpp::has_operator_less<non_comparable_type>::value};
    static_assert(!operator_less_available, "'non_comparable_type' has unexpected operator<");
    static_assert(!score::cpp::has_operator_less_v<non_comparable_type>, "'non_comparable_type' has unexpected operator<");
    EXPECT_FALSE(operator_less_available);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770147
TEST(type_identity, when_type_argument_is_exposed_via_nested_typedef_type)
{
    static_assert(std::is_same<score::cpp::type_identity_t<int>, int>::value,
                  "Error: Type identity of fundamental types not working.");
    static_assert(!std::is_same<score::cpp::type_identity_t<int>, long>::value,
                  "Error: Type identity changes fundamental type - implicit conversion.");
    static_assert(!std::is_same<score::cpp::type_identity_t<int>, unsigned>::value,
                  "Error: Type identity of fundamental types not working - signed/unsigned.");
    static_assert(std::is_same<score::cpp::type_identity_t<float>, float>::value,
                  "Error: Type identity of fundamental types not working.");
    struct dummy_t
    {
    };
    static_assert(std::is_same<score::cpp::type_identity_t<dummy_t>, dummy_t>::value,
                  "Error: Type identity of user-defined types not working.");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770147
TEST(type_identity, when_type_argument_does_not_decay)
{
    static_assert(std::is_same<score::cpp::type_identity_t<int[]>, int[]>::value,
                  "Error: Type identity does not preserve array type.");
    static_assert(!std::is_same<score::cpp::type_identity_t<int[]>, int*>::value,
                  "Error: Type identity performs array decay.");
    static_assert(std::is_same<score::cpp::type_identity_t<int const&>, int const&>::value,
                  "Error: Type identity does not preserve c/v qualified reference type.");
    static_assert(!std::is_same<score::cpp::type_identity_t<int const&>, int>::value,
                  "Error: Type identity performs reference type decay.");
}

template <typename T, typename U>
constexpr bool arguments_have_same_type(T, U)
{
    return std::is_same<T, U>::value;
}

template <typename T, typename U>
constexpr bool both_arguments_deduced_independently(T l, U r)
{
    return arguments_have_same_type(l, r);
}

template <typename T>
constexpr bool second_argument_ignored_for_deduction(T l, score::cpp::type_identity_t<T> r)
{
    return arguments_have_same_type(l, r);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770147
TEST(type_identity, when_it_disables_argument_type_deduction)
{
    static_assert(both_arguments_deduced_independently(42, 4),
                  "Error: Broken base test case - both_arguments_deduced_independently should deduce argument types.");
    static_assert(!both_arguments_deduced_independently(42, 4.0),
                  "Error: Broken base test case - both_arguments_deduced_independently should deduce argument types "
                  "independently.");
    static_assert(second_argument_ignored_for_deduction(42, 4),
                  "Error: Broken base test case - second_argument_ignored_for_deduction should deduce argument types.");
    static_assert(second_argument_ignored_for_deduction(42, 4.0),
                  "Error: Type identity does not shield template argument deduction.");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17822902
TEST(void_t, when_used_for_template_specialization_purpose)
{
    static_assert(score::cpp::is_iterable<std::vector<double>>::value, "Error: type is not iterable but should be");
    static_assert(!score::cpp::is_iterable<double>::value, "Error: type is iterable but shouldn't");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17822902
TEST(void_t, when_standardly_used)
{
    static_assert(std::is_same<score::cpp::void_t<double>, void>::value, "Error type isn't promoted to void");
}

template <class T>
using copy_assign_t = decltype(std::declval<T&>() = std::declval<const T&>());

struct test_is_detected_struct
{
};

struct test_is_detected_not_copy_assign
{
    void operator=(const test_is_detected_not_copy_assign&) = delete;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17823050
TEST(is_detected, when_detect_for_function)
{
    static_assert(score::cpp::is_detected<copy_assign_t, test_is_detected_struct>::value,
                  "Error: type is not copy assignable but should be");
    static_assert(!score::cpp::is_detected<copy_assign_t, test_is_detected_not_copy_assign>::value,
                  "Error: type is copy assignable but should not be");
}

template <typename T>
using has_field = decltype(T::my_field);

struct test_is_detected_field_struct
{
    constexpr static int my_field{1};
};

enum class test_is_detected_enum_class
{
    my_field,
    my_second_field
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17823050
TEST(is_detected, when_detect_for_fields)
{
    static_assert(score::cpp::is_detected<has_field, test_is_detected_field_struct>::value,
                  "Error: type does not have the field");
    static_assert(score::cpp::is_detected<has_field, test_is_detected_enum_class>::value,
                  "Error: type does not have the field");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17765422
TEST(has_size, when_container_has_size_method)
{
    static_assert(score::cpp::has_size<std::vector<double>>::value, "Container does not have a size method");
    static_assert(score::cpp::has_size_v<std::vector<double>>, "Container does not have a size method");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770042
TEST(is_not_equal_comparable, when_used_with_notequal_and_non_notequal_comparable_types)
{
    static_assert(score::cpp::is_not_equal_comparable<int>::value,
                  "Error: type isn't not-equal comparable type but should be");
    static_assert(score::cpp::is_not_equal_comparable<int*>::value,
                  "Error: type isn't not-equal comparable type but should be");
    static_assert(score::cpp::is_not_equal_comparable_v<typename std::vector<int>::iterator>,
                  "Error: type isn't not-equal comparable type but should be");
    static_assert(
        !score::cpp::is_not_equal_comparable_v<decltype(std::declval<non_iterable_with_void_begin_end_type>().begin())>,
        "Error: type should not be an a not-equal comparable type");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_pre_increment, when_used_with_incrementable_and_non_incrementable_types)
{
    static_assert(!score::cpp::has_operator_pre_increment<decltype(
                      std::declval<non_iterable_with_void_begin_end_type>().begin())>::value,
                  "Error: type should not be incrementable");
    static_assert(score::cpp::has_operator_pre_increment<decltype(std::declval<std::array<float, 2UL>>().begin())>::value,
                  "Error: type isn't incrementable type but should be");
    static_assert(score::cpp::has_operator_pre_increment_v<iterable_type<double>::iterator>,
                  "Error: type isn't incrementable type but should be");
    static_assert(score::cpp::has_operator_pre_increment<int>::value, "Error: type isn't incrementable type but should be");
    static_assert(score::cpp::has_operator_pre_increment<char*>::value, "Error: type isn't incrementable type but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17769132
TEST(has_operator_indirection, when_used_with_dereferenceable_and_non_dereferenceable_types)
{
    static_assert(
        !score::cpp::has_operator_indirection<decltype(std::declval<non_iterable_with_void_begin_end_type>().begin())>::value,
        "Error: type should not be dereferenceable type");
    static_assert(score::cpp::has_operator_indirection<int*>::value, "Error: type isn't a dereferenceable type but should be");
    static_assert(score::cpp::has_operator_indirection<std::vector<double>::iterator>::value,
                  "Error: type isn't a dereferenceable type but should be");
    static_assert(score::cpp::has_operator_indirection_v<iterable_type<int>::iterator>,
                  "Error: type isn't a dereferenceable type but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_legacy_input_iterator, when_used_with_non_iterator_type)
{
    static_assert(!score::cpp::detail::is_legacy_input_iterator<decltype(
                      std::declval<non_iterable_with_void_begin_end_type>().begin())>::value,
                  "Error: type should not be an a legacy iterator type");
    static_assert(!score::cpp::detail::is_legacy_input_iterator<int>::value,
                  "Error: type should not be a legacy iterator type");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_legacy_input_iterator, when_used_with_container_iterator)
{
    static_assert(score::cpp::detail::is_legacy_input_iterator<decltype(std::declval<std::vector<int>>().begin())>::value,
                  "Error: type is not a legacy input iterator but should be");
    static_assert(score::cpp::detail::is_legacy_input_iterator<decltype(std::declval<std::array<int, 3UL>>().end())>::value,
                  "Error: type is not a legacy input iterator but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_legacy_input_iterator, when_used_with_simple_iterator)
{
    static_assert(score::cpp::detail::is_legacy_input_iterator<simple_iterator>::value,
                  "Error: type is not a legacy input iterator but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_legacy_input_iterator, when_used_with_iterable_begin_end_type)
{
    static_assert(
        score::cpp::detail::is_legacy_input_iterator<decltype(std::declval<iterable_type<std::size_t>>().end())>::value,
        "Error: type is not a legacy input iterator but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_legacy_input_iterator, when_used_with_builtin_c_array_supports_std_cbegin_std_cend)
{
    static_assert(score::cpp::detail::is_legacy_input_iterator_v<decltype(std::cbegin(std::declval<int[4]>()))>,
                  "Error: type is not legacy input iterator but should be");
    // int[] shouldn't be considered a legacy iterator
    static_assert(!score::cpp::detail::is_legacy_input_iterator_v<int[]>, "Error: type should not be a legacy input iterator");
    // However with its decayed version, it is considered legacy iterator, since it's decayed into 'int*'
    static_assert(score::cpp::detail::is_legacy_input_iterator_v<std::decay_t<int[]>>,
                  "Error: type is not a legacy input iterator but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_legacy_input_iterator, when_used_with_pointer)
{
    // So as with the 'int*', that's also behaves as an iterator like
    static_assert(score::cpp::detail::is_legacy_input_iterator_v<int*>,
                  "Error: type is not a legacy input iterator but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_legacy_input_iterator, when_used_with_non_swappable_type)
{
    static_assert(!score::cpp::detail::is_legacy_input_iterator_v<simple_non_swappable_iterator>,
                  "Error: type should not be a legacy input iterator");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_iterable, when_used_with_builtin_c_array)
{
    static_assert(score::cpp::is_iterable_v<int[5]>, "Error: type is not iterable but should be");
    static_assert(score::cpp::is_iterable_v<const int[5]>, "Error: type is not iterable but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_iterable, when_iterable)
{
    static_assert(score::cpp::is_iterable<std::vector<double>>::value, "Error: type is not iterable but should be");
    static_assert(score::cpp::is_iterable_v<std::vector<double>>, "Error: type is not iterable but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_iterable, when_not_iterable)
{
    static_assert(!score::cpp::is_iterable<double>::value, "Error: type is iterable but shouldn't");
    static_assert(!score::cpp::is_iterable_v<double>, "Error: type is iterable but shouldn't");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_iterable, when_used_with_non_iterable_begin_end_type)
{
    static_assert(!score::cpp::is_iterable<non_iterable_with_void_begin_end_type>::value,
                  "Error: type should not be iterable");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770107
TEST(is_iterable, when_used_with_iterable_begin_end_type)
{
    static_assert(score::cpp::is_iterable<iterable_type<int>>::value, "Error: type is not iterable but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770112
TEST(conjunction, when_empty)
{
    //
    static_assert(score::cpp::conjunction_v<>, "Failure");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770112
TEST(conjunction, when_single_argument)
{
    static_assert(!score::cpp::conjunction_v<std::integral_constant<bool, false>>, "Failure");
    static_assert(score::cpp::conjunction_v<std::integral_constant<bool, true>>, "Failure");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770112
TEST(conjunction, when_double_argument)
{
    static_assert(!score::cpp::conjunction_v<std::false_type, std::false_type>, "Failure");
    static_assert(!score::cpp::conjunction_v<std::false_type, std::true_type>, "Failure");
    static_assert(!score::cpp::conjunction_v<std::true_type, std::false_type>, "Failure");
    static_assert(score::cpp::conjunction_v<std::true_type, std::true_type>, "Failure");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770119
TEST(is_expected, when_expected)
{
    static_assert(score::cpp::is_expected<score::cpp::expected<int, int>>::value, "Error: type is not expected but should be");
    static_assert(score::cpp::is_expected_v<score::cpp::expected<int, int>>, "Error: type is not expected but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770119
TEST(is_expected, when_not_expected)
{
    static_assert(!score::cpp::is_expected<double>::value, "Error: type is expected but shouldn't");
    static_assert(!score::cpp::is_expected_v<double>, "Error: type is expected but shouldn't");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770126
TEST(is_optional, when_optional)
{
    static_assert(score::cpp::is_optional<score::cpp::optional<int>>::value, "Error: type is not optional but should be");
    static_assert(score::cpp::is_optional_v<score::cpp::optional<int>>, "Error: type is not optional but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770126
TEST(is_optional, when_not_optional)
{
    static_assert(!score::cpp::is_optional<double>::value, "Error: type is optional but shouldn't");
    static_assert(!score::cpp::is_optional_v<double>, "Error: type is optional but shouldn't");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#35266710
TEST(is_hash_for, when_hash_for)
{
    static_assert(score::cpp::is_hash_for_v<std::hash<int>, int>,
                  "Error: first type is not hash for second type but should be");
    static_assert(score::cpp::is_hash_for_v<valid_hash_type, int>,
                  "Error: first type is not hash for second type but should be");
    static_assert(score::cpp::is_hash_for_v<std::hash<int>, const int>,
                  "Error: first type is not hash for second type but should be");
    static_assert(score::cpp::is_hash_for_v<valid_hash_type, const int>,
                  "Error: first type is not hash for second type but should be");
    static_assert(score::cpp::is_hash_for_v<std::hash<int>, int&>,
                  "Error: first type is not hash for second type but should be");
    static_assert(score::cpp::is_hash_for_v<valid_hash_type, int&>,
                  "Error: first type is not hash for second type but should be");
    static_assert(score::cpp::is_hash_for_v<std::hash<int>, const int&>,
                  "Error: first type is not hash for second type but should be");
    static_assert(score::cpp::is_hash_for_v<valid_hash_type, const int&>,
                  "Error: first type is not hash for second type but should be");
    static_assert(score::cpp::is_hash_for_v<std::hash<int>, int&&>,
                  "Error: first type is not hash for second type but should be");
    static_assert(score::cpp::is_hash_for_v<valid_hash_type, int&&>,
                  "Error: first type is not hash for second type but should be");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#35266710
TEST(is_hash_for, when_not_hash_for)
{
    static_assert(!score::cpp::is_hash_for_v<non_copy_constructible_hash_type, int>,
                  "Error: first type is hash for second type but shouldn't be");
    static_assert(!score::cpp::is_hash_for_v<non_destructible_hash_type, int>,
                  "Error: first type is hash for second type but shouldn't be");
    static_assert(!score::cpp::is_hash_for_v<std::size_t (&)(const int&), int>,
                  "Error: first type is hash for second type but shouldn't be");
    static_assert(!score::cpp::is_hash_for_v<hash_type_with_invalid_overload_1, int>,
                  "Error: first type is hash for second type but shouldn't be");
    static_assert(!score::cpp::is_hash_for_v<hash_type_with_invalid_overload_2, int>,
                  "Error: first type is hash for second type but shouldn't be");
    static_assert(!score::cpp::is_hash_for_v<hash_type_with_invalid_overload_3, int>,
                  "Error: first type is hash for second type but shouldn't be");
    static_assert(!score::cpp::is_hash_for_v<hash_type_with_invalid_overload_4, int>,
                  "Error: first type is hash for second type but shouldn't be");
}

// NOTRACING
TEST(type_traits, input_iterator_category)
{
    using it = std::istream_iterator<std::int32_t>;

    static_assert(score::cpp::detail::is_input_iterator<it>::value, "failed");
    static_assert(!score::cpp::detail::is_forward_iterator<it>::value, "failed");
    static_assert(!score::cpp::detail::is_bidirectional_iterator<it>::value, "failed");
    static_assert(!score::cpp::detail::is_random_access_iterator<it>::value, "failed");
}

// NOTRACING
TEST(type_traits, forward_iterator_category)
{
    using it = std::forward_list<std::int32_t>::iterator;

    static_assert(score::cpp::detail::is_input_iterator<it>::value, "failed");
    static_assert(score::cpp::detail::is_forward_iterator<it>::value, "failed");
    static_assert(!score::cpp::detail::is_bidirectional_iterator<it>::value, "failed");
    static_assert(!score::cpp::detail::is_random_access_iterator<it>::value, "failed");
}

// NOTRACING
TEST(type_traits, bidirectional_iterator_category)
{
    using it = std::list<std::int32_t>::iterator;

    static_assert(score::cpp::detail::is_input_iterator<it>::value, "failed");
    static_assert(score::cpp::detail::is_forward_iterator<it>::value, "failed");
    static_assert(score::cpp::detail::is_bidirectional_iterator<it>::value, "failed");
    static_assert(!score::cpp::detail::is_random_access_iterator<it>::value, "failed");
}

// NOTRACING
TEST(type_traits, random_access_iterator_category)
{
    using it = std::vector<std::int32_t>::iterator;

    static_assert(score::cpp::detail::is_input_iterator<it>::value, "failed");
    static_assert(score::cpp::detail::is_forward_iterator<it>::value, "failed");
    static_assert(score::cpp::detail::is_bidirectional_iterator<it>::value, "failed");
    static_assert(score::cpp::detail::is_random_access_iterator<it>::value, "failed");
}

} // namespace
