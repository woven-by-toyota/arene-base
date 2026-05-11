// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/array/array.hpp"

#include <gtest/gtest.h>

#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/exchange.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_const.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_void.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/has_broken_less_than.hpp"

namespace {

struct user_type {};

/// @test `array` is default-constructible if the stored element type is default constructible, and the default
/// constructor is `noexcept` if the element type's default constructor is `noexcept`
TEST(Array, ArrayIsDefaultConstructibleIfTIsDefaultConstructible) {
  static_assert(
      std::is_nothrow_default_constructible<arene::base::array<int, 5>>::value,
      "Must be nothrow default constructible"
  );
  struct nothrow_dctor {};
  static_assert(
      std::is_nothrow_default_constructible<arene::base::array<nothrow_dctor, 5>>::value,
      "Must be nothrow default constructible"
  );
  struct throw_dctor {
    throw_dctor() noexcept(false) { static_cast<void>(0); }
  };
  static_assert(
      !std::is_nothrow_default_constructible<arene::base::array<throw_dctor, 5>>::value,
      "Must not be nothrow default constructible"
  );
  static_assert(
      std::is_default_constructible<arene::base::array<throw_dctor, 5>>::value,
      "Must be default constructible"
  );

  struct no_dctor {
    explicit no_dctor(int) {}
  };

  static_assert(
      !std::is_default_constructible<arene::base::array<no_dctor, 5>>::value,
      "Must not be default constructible"
  );
}

/// @test `array` supports aggregate initialization for both built-in and user-defined types. The array elements are
/// initialized from the corrsponding aggregate initializers.
TEST(Array, ArraySupportsAggregateInitialization) {
  arene::base::array<int, 5> array1 = {1, 2, 3, 4, 5};

  ASSERT_EQ(array1[0], 1);
  ASSERT_EQ(array1[1], 2);
  ASSERT_EQ(array1[2], 3);
  ASSERT_EQ(array1[3], 4);
  ASSERT_EQ(array1[4], 5);

  arene::base::array<int, 5> const array2{10, 20};

  ASSERT_EQ(array2[0], 10);
  ASSERT_EQ(array2[1], 20);
  ASSERT_EQ(array2[2], 0);
  ASSERT_EQ(array2[3], 0);
  ASSERT_EQ(array2[4], 0);

  constexpr arene::base::array<int, 3> array3{-100, -200, -300};

  static_assert(array3[0] == -100, "Element access is correct");
  static_assert(array3[1] == -200, "Element access is correct");
  static_assert(array3[2] == -300, "Element access is correct");

  struct aggregate_type {
    int i = 42;
  };

  constexpr arene::base::array<aggregate_type, 3> array4{};

  static_assert(array4[0].i == 42, "Element access is correct");
  static_assert(array4[1].i == 42, "Element access is correct");
  static_assert(array4[2].i == 42, "Element access is correct");

  constexpr arene::base::array<aggregate_type, 3> array5{{{3}, {99}, {123}}};

  static_assert(array5[0].i == 3, "Element access is correct");
  static_assert(array5[1].i == 99, "Element access is correct");
  static_assert(array5[2].i == 123, "Element access is correct");
}

/// @test `data` returns a pointer to the element type, which is equal to the address of the first element, as accessed
/// via the index operator. Incrementing the pointer addresses each element in turn.
TEST(Array, ElementAddresses) {
  using arr_t = arene::base::array<user_type, 5>;

  arr_t array{};

  ::testing::StaticAssertTypeEq<decltype(array.data()), user_type*>();
  user_type* start = array.data();
  ::testing::StaticAssertTypeEq<decltype(array[0]), user_type&>();
  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_EQ(&array[0], start);
  ASSERT_EQ(&array[1], arene::base::next(start));
  ASSERT_EQ(&array[2], arene::base::next(start, 2));
  ASSERT_EQ(&array[3], arene::base::next(start, 3));
  ASSERT_EQ(&array[4], arene::base::next(start, 4));

  user_type& elem = array[3];
  ASSERT_EQ(&elem, arene::base::next(start, 3));

  arr_t const& const_array = array;
  ASSERT_EQ(array.data(), const_array.data());
}

template <typename ArrayT>
using use_at = decltype(std::declval<ArrayT>().at(0));

template <typename ArrayT>
constexpr bool has_at_v = arene::base::substitution_succeeds<use_at, ArrayT>;

/// @test Given `arene::base::array<T,N>`, then `arene::base::array<T,N>::at(std::size_t)` participates in overload
/// resolution IFF `arene::base::detail::are_exceptions_enabled_v` is `true`.
TEST(Array, AtIsSFINAERemovedIfExceptionsAreDisabled) {
  using non_zero_array_t = arene::base::array<user_type, 5>;
  STATIC_ASSERT_EQ(has_at_v<non_zero_array_t&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<non_zero_array_t const&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<non_zero_array_t&&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<non_zero_array_t const&&>, arene::base::detail::are_exceptions_enabled_v);
  using zero_array_t = arene::base::array<user_type, 0>;
  STATIC_ASSERT_EQ(has_at_v<zero_array_t&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<zero_array_t const&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<zero_array_t&&>, arene::base::detail::are_exceptions_enabled_v);
  STATIC_ASSERT_EQ(has_at_v<zero_array_t const&&>, arene::base::detail::are_exceptions_enabled_v);
}

/// @test `data` returns a `const` pointer to the element type on a `const` `array`, which is equal to the address of
/// the first element, as accessed via the index operator. Incrementing the pointer addresses each element in turn.
TEST(Array, ElementAddressesForConstArrays) {
  using arr_t = arene::base::array<user_type, 5>;

  arr_t const const_array{};

  ::testing::StaticAssertTypeEq<decltype(const_array.data()), user_type const*>();
  user_type const* cstart = const_array.data();
  ::testing::StaticAssertTypeEq<decltype(const_array[0]), user_type const&>();

  ASSERT_EQ(&const_array[1], arene::base::next(cstart));
  ASSERT_EQ(&const_array[2], arene::base::next(cstart, 2));
  ASSERT_EQ(&const_array[3], arene::base::next(cstart, 3));
  ASSERT_EQ(&const_array[4], arene::base::next(cstart, 4));

  user_type const& celem = const_array[3];
  ASSERT_EQ(&celem, arene::base::next(cstart, 3));
}

/// @test `array` has the type aliases required for a sequence container
TEST(Array, MemberTypes) {
  using arr_t = arene::base::array<user_type, 42>;

  ::testing::StaticAssertTypeEq<arr_t::value_type, user_type>();
  ::testing::StaticAssertTypeEq<arr_t::pointer, user_type*>();
  ::testing::StaticAssertTypeEq<arr_t::const_pointer, user_type const*>();
  ::testing::StaticAssertTypeEq<arr_t::reference, user_type&>();
  ::testing::StaticAssertTypeEq<arr_t::const_reference, user_type const&>();
  ::testing::StaticAssertTypeEq<arr_t::size_type, std::size_t>();
  ::testing::StaticAssertTypeEq<arr_t::difference_type, std::ptrdiff_t>();
  STATIC_ASSERT_TRUE(arene::base::is_random_access_iterator_v<arr_t::iterator>);
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::reference, arr_t::reference>();
  STATIC_ASSERT_TRUE(arene::base::is_random_access_iterator_v<arr_t::const_iterator>);
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::const_iterator>::reference, arr_t::const_reference>();
  STATIC_ASSERT_TRUE(arene::base::is_random_access_iterator_v<arr_t::reverse_iterator>);
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::reverse_iterator>::reference, arr_t::reference>();
  STATIC_ASSERT_TRUE(arene::base::is_random_access_iterator_v<arr_t::const_reverse_iterator>);
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::const_reverse_iterator>::reference, arr_t::const_reference>(
  );
}

/// @test Invoking `size` or `max_size` on an `array` yields the number of elements in the array; `empty` returns
/// `false` for arrays with non-zero size. `size`, `max_size` and `empty` are all `noexcept`.
TEST(Array, Size) {
  constexpr auto size1 = 42;
  arene::base::array<int, size1> const array1{};

  ASSERT_EQ(array1.size(), size1);
  ::testing::StaticAssertTypeEq<decltype(array1.size()), std::size_t>();
  static_assert(noexcept(array1.size()), "size must be noexcept");

  ::testing::StaticAssertTypeEq<decltype(array1.max_size()), std::size_t>();
  static_assert(noexcept(array1.max_size()), "max_size must be noexcept");

  ASSERT_FALSE(array1.empty());
  ::testing::StaticAssertTypeEq<decltype(array1.empty()), bool>();
  static_assert(noexcept(array1.empty()), "empty must be noexcept");

  auto const& const_array1 = array1;

  ASSERT_EQ(const_array1.size(), size1);
  ::testing::StaticAssertTypeEq<decltype(const_array1.size()), std::size_t>();
  static_assert(noexcept(const_array1.size()), "size must be noexcept");

  ASSERT_FALSE(const_array1.empty());
  ::testing::StaticAssertTypeEq<decltype(const_array1.empty()), bool>();
  static_assert(noexcept(const_array1.empty()), "empty must be noexcept");

  struct custom_type {
    int a = 0;
    int b = 0;
    std::int64_t d = 0;
    bool z = false;
  };
  constexpr auto size2 = 90;
  constexpr arene::base::array<custom_type, size2> array2{};

  static_assert(array2.size() == size2, "size must be right");
  static_assert(array2.max_size() == size2, "max_size must be right");
  static_assert(!array2.empty(), "not empty");
}

/// @test `front` returns a reference to the first element in an `array`. `back` returns a reference to the last element
/// in an `array`. If `front` or `back` is invoked via a `const` reference, then the returned reference is also `const`.
/// Both `front` and `back` are declared `noexcept`.
TEST(Array, FrontAndBack) {
  constexpr auto size = 123;
  using arr_t = arene::base::array<user_type, size>;

  arr_t array{};

  ::testing::StaticAssertTypeEq<decltype(array.front()), user_type&>();
  static_assert(noexcept(array.front()), "noexcept");
  ::testing::StaticAssertTypeEq<decltype(array.back()), user_type&>();
  static_assert(noexcept(array.back()), "noexcept");

  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_EQ(&array.front(), &array[0]);
  ASSERT_EQ(&array.back(), &array[size - 1]);

  auto const& const_array = array;

  ::testing::StaticAssertTypeEq<decltype(const_array.front()), user_type const&>();
  static_assert(noexcept(const_array.front()), "noexcept");
  ::testing::StaticAssertTypeEq<decltype(const_array.front()), user_type const&>();
  static_assert(noexcept(const_array.front()), "noexcept");

  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_EQ(&const_array.front(), &array[0]);
  ASSERT_EQ(&const_array.back(), &array[size - 1]);

  constexpr arr_t barray{};
  // NOLINTNEXTLINE(readability-container-data-pointer)
  static_assert(&barray.front() == &barray[0], "Usable in constexpr");
  static_assert(&barray.back() == &barray[size - 1], "Usable in constexpr");
}

namespace {
constexpr auto fill_val = -42;
constexpr auto get_filled_array() -> arene::base::array<int, 5> {
  arene::base::array<int, 5> array{};
  array.fill(fill_val);
  return array;
}
}  // namespace

/// @test Invoking `fill` on an array assigns the specified value to every element of the array.
TEST(Array, Fill) {
  constexpr auto size1 = 33;
  arene::base::array<int, size1> array{};
  for (auto& element : array) {
    ASSERT_EQ(element, 0);
  }
  constexpr auto val = -123;
  array.fill(val);
  for (auto& element : array) {
    ASSERT_EQ(element, val);
  }

  static_assert(noexcept(array.fill(val)), "Noexcept fill");
  static_assert(
      !noexcept(std::declval<arene::base::array<testing::configurable_value<int, testing::throws_on::everything>, 5>&>()
                    .fill(1)),
      "Not Noexcept fill"
  );

  constexpr auto filled_array = get_filled_array();

  for (auto const& element : filled_array) {
    ASSERT_EQ(element, fill_val);
  }
}

/// @test `begin` and `end` return `iterator`s, `cbegin` and `cend` return `const_iterator`s, as do `begin` and `end`
/// when invoked on a `const` `array`. The iterators are random access iterators, and iterating through from `begin` to
/// `end` visits each element in turn.
TEST(Array, Iteration) {
  struct custom_type {
    int idx = fill_val;
  };
  constexpr auto size = 123;
  using arr_t = arene::base::array<custom_type, size>;

  // NOLINTNEXTLINE(misc-const-correctness)
  arr_t array{};

  ::testing::StaticAssertTypeEq<decltype(array.begin()), arr_t::iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.end()), arr_t::iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.cbegin()), arr_t::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.cend()), arr_t::const_iterator>();
  ::testing::
      StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::iterator_category, std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::value_type, custom_type>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::reference, custom_type&>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::pointer, custom_type*>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::difference_type, std::ptrdiff_t>();

  ::testing::StaticAssertTypeEq<decltype(*array.begin()), custom_type&>();

  arr_t const& k_a = array;
  ::testing::StaticAssertTypeEq<decltype(k_a.begin()), arr_t::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(k_a.end()), arr_t::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(k_a.cbegin()), arr_t::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(k_a.cend()), arr_t::const_iterator>();

  ASSERT_EQ(k_a.begin(), k_a.cbegin());
  ASSERT_EQ(k_a.begin(), array.cbegin());
  ASSERT_EQ(k_a.end(), k_a.cend());
  ASSERT_EQ(k_a.end(), array.cend());

  ::testing::StaticAssertTypeEq<
      std::iterator_traits<arr_t::const_iterator>::iterator_category,
      std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::const_iterator>::value_type, custom_type>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::const_iterator>::reference, custom_type const&>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::const_iterator>::pointer, custom_type const*>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::const_iterator>::difference_type, std::ptrdiff_t>();

  static_assert(arene::base::is_random_access_iterator_v<arr_t::iterator>, "Must be a random access iterator");
  static_assert(arene::base::is_random_access_iterator_v<arr_t::const_iterator>, "Must be a random access iterator");

  ::testing::StaticAssertTypeEq<decltype(*k_a.begin()), custom_type const&>();

  constexpr arr_t k_array2{};

  // NOLINTNEXTLINE(readability-container-data-pointer)
  static_assert(&*k_array2.begin() == &k_array2[0], "iterator");
  static_assert(&*(k_array2.begin() + 1) == &k_array2[1], "iterator");
}

/// @test `rbegin` and `rend` return `reverse_iterator`s, `crbegin` and `crend` return `const_reverse_iterator`s, as do
/// `rbegin` and `rend` when invoked on a `const` `array`. The iterators are random access iterators, and iterating
/// through from `rbegin` to `rend` visits each element in turn from the last element of the array to the first.
TEST(Array, ReverseIteration) {
  struct custom_type {
    int idx = fill_val;
  };
  constexpr auto size = 123;
  using arr_t = arene::base::array<custom_type, size>;

  arr_t array{};

  ::testing::StaticAssertTypeEq<decltype(array.rbegin()), arr_t::reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.rend()), arr_t::reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.crbegin()), arr_t::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.crend()), arr_t::const_reverse_iterator>();
  ::testing::
      StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::iterator_category, std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::value_type, custom_type>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::reference, custom_type&>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::pointer, custom_type*>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::iterator>::difference_type, std::ptrdiff_t>();

  ::testing::StaticAssertTypeEq<decltype(*array.begin()), custom_type&>();

  auto rit = array.rbegin();
  ASSERT_EQ(&*rit, &array[size - 1]);
  ASSERT_EQ(&++rit, &rit);
  ASSERT_EQ(&*rit, &array[size - 2]);

  auto crit = array.crbegin();
  ASSERT_EQ(&*crit, &array[size - 1]);
  ASSERT_EQ(&++crit, &crit);
  ASSERT_EQ(&*crit, &array[size - 2]);

  arr_t const& k_a = array;
  ::testing::StaticAssertTypeEq<decltype(k_a.rbegin()), arr_t::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(k_a.rend()), arr_t::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(k_a.crbegin()), arr_t::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(k_a.crend()), arr_t::const_reverse_iterator>();

  static_assert(arene::base::is_random_access_iterator_v<arr_t::reverse_iterator>, "Must be a random access iterator");
  static_assert(
      arene::base::is_random_access_iterator_v<arr_t::const_reverse_iterator>,
      "Must be a random access iterator"
  );

  crit = k_a.rbegin();
  ASSERT_EQ(&*crit, &array[size - 1]);
  ++crit;
  ASSERT_EQ(&*crit, &array[size - 2]);

  ::testing::StaticAssertTypeEq<
      std::iterator_traits<arr_t::const_iterator>::iterator_category,
      std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::const_iterator>::value_type, custom_type>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::const_iterator>::reference, custom_type const&>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::const_iterator>::pointer, custom_type const*>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<arr_t::const_iterator>::difference_type, std::ptrdiff_t>();

  ::testing::StaticAssertTypeEq<decltype(*k_a.begin()), custom_type const&>();

  constexpr arr_t k_array2{};

  // NOLINTNEXTLINE(readability-container-data-pointer)
  static_assert(&*k_array2.begin() == &k_array2[0], "iterator");
  static_assert(&*(k_array2.begin() + 1) == &k_array2[1], "iterator");
}

/// @test For `array`s of built-in types, both the `swap` member function and `swap` non-member function are declared
/// `noexcept`, and swap the contents of two arrays
TEST(Array, Swap) {
  using arr_t = arene::base::array<int, 5>;

  arr_t array1{1, 2, 3, 4, 5};
  arr_t array2{10, 20, 30, 40, 50};

  array1.swap(array2);

  ASSERT_EQ(array1[0], 10);
  ASSERT_EQ(array1[1], 20);
  ASSERT_EQ(array1[2], 30);
  ASSERT_EQ(array1[3], 40);
  ASSERT_EQ(array1[4], 50);

  ASSERT_EQ(array2[0], 1);
  ASSERT_EQ(array2[1], 2);
  ASSERT_EQ(array2[2], 3);
  ASSERT_EQ(array2[3], 4);
  ASSERT_EQ(array2[4], 5);

  static_assert(noexcept(array1.swap(array2)), "Nothrow swappable");
  static_assert(noexcept(swap(array1, array2)), "Nothrow swappable");

  swap(array1, array2);

  ASSERT_EQ(array1[0], 1);
  ASSERT_EQ(array1[1], 2);
  ASSERT_EQ(array1[2], 3);
  ASSERT_EQ(array1[3], 4);
  ASSERT_EQ(array1[4], 5);

  ASSERT_EQ(array2[0], 10);
  ASSERT_EQ(array2[1], 20);
  ASSERT_EQ(array2[2], 30);
  ASSERT_EQ(array2[3], 40);
  ASSERT_EQ(array2[4], 50);
}

namespace {
struct swappable_t {
  int value;

  friend constexpr void swap(swappable_t& lhs, swappable_t& rhs) {
    auto temp = rhs.value;
    rhs.value = lhs.value;
    lhs.value = temp + 100;
  }
};

struct nothrow_swappable_t {
  int value;

  friend constexpr void swap(nothrow_swappable_t& lhs, nothrow_swappable_t& rhs) noexcept {
    auto temp = rhs.value;
    rhs.value = lhs.value;
    lhs.value = temp + 100;
  }
};

constexpr auto check_constexpr_swap() -> arene::base::array<swappable_t, 5> {
  arene::base::array<swappable_t, 5> array1{};
  arene::base::array<swappable_t, 5> array2{{{1}}};
  array1.swap(array2);
  return array1;
}
}  // namespace

/// @test For `array`s of user-defined types, the `swap` member function and `swap` free function exchange the elements
/// between the `array`s, and are not `noexcept` if the element type is not nothrow-swappable
TEST(Array, SwapForUserTypes) {
  using arr_t = arene::base::array<swappable_t, 5>;

  arr_t array1{{{1}, {2}, {3}, {4}, {5}}};
  arr_t array2{{{10}, {20}, {30}, {40}, {50}}};

  array1.swap(array2);

  ASSERT_EQ(array1[0].value, 110);
  ASSERT_EQ(array1[1].value, 120);
  ASSERT_EQ(array1[2].value, 130);
  ASSERT_EQ(array1[3].value, 140);
  ASSERT_EQ(array1[4].value, 150);

  ASSERT_EQ(array2[0].value, 1);
  ASSERT_EQ(array2[1].value, 2);
  ASSERT_EQ(array2[2].value, 3);
  ASSERT_EQ(array2[3].value, 4);
  ASSERT_EQ(array2[4].value, 5);

  static_assert(arene::base::is_swappable_v<swappable_t>, "Must be swappable");
  static_assert(!arene::base::is_nothrow_swappable_v<swappable_t>, "Not nothrow swappable");
  static_assert(!noexcept(array1.swap(array2)), "Not Nothrow swappable");
  static_assert(!noexcept(swap(array1, array2)), "Not Nothrow swappable");

  swap(array1, array2);

  ASSERT_EQ(array1[0].value, 101);
  ASSERT_EQ(array1[1].value, 102);
  ASSERT_EQ(array1[2].value, 103);
  ASSERT_EQ(array1[3].value, 104);
  ASSERT_EQ(array1[4].value, 105);

  ASSERT_EQ(array2[0].value, 110);
  ASSERT_EQ(array2[1].value, 120);
  ASSERT_EQ(array2[2].value, 130);
  ASSERT_EQ(array2[3].value, 140);
  ASSERT_EQ(array2[4].value, 150);

  constexpr auto swapped = check_constexpr_swap();
  static_assert(swapped[0].value == 101, "Check element value");
}

/// @test For `array`s of user-defined types, the `swap` member function and `swap` free function exchange the elements
/// between the `array`s, and are `noexcept` if the element type is nothrow-swappable
TEST(Array, SwapForNothrowSwappableUserTypes) {
  using arr_t = arene::base::array<nothrow_swappable_t, 5>;

  arr_t array1{{{1}, {2}, {3}, {4}, {5}}};
  arr_t array2{{{10}, {20}, {30}, {40}, {50}}};

  array1.swap(array2);

  ASSERT_EQ(array1[0].value, 110);
  ASSERT_EQ(array1[1].value, 120);
  ASSERT_EQ(array1[2].value, 130);
  ASSERT_EQ(array1[3].value, 140);
  ASSERT_EQ(array1[4].value, 150);

  ASSERT_EQ(array2[0].value, 1);
  ASSERT_EQ(array2[1].value, 2);
  ASSERT_EQ(array2[2].value, 3);
  ASSERT_EQ(array2[3].value, 4);
  ASSERT_EQ(array2[4].value, 5);

  static_assert(arene::base::is_swappable_v<nothrow_swappable_t>, "Must be swappable");
  static_assert(arene::base::is_nothrow_swappable_v<nothrow_swappable_t>, "Nothrow swappable");
  static_assert(noexcept(array1.swap(array2)), "Nothrow swappable");
  static_assert(noexcept(swap(array1, array2)), "Nothrow swappable");

  swap(array1, array2);

  ASSERT_EQ(array1[0].value, 101);
  ASSERT_EQ(array1[1].value, 102);
  ASSERT_EQ(array1[2].value, 103);
  ASSERT_EQ(array1[3].value, 104);
  ASSERT_EQ(array1[4].value, 105);

  ASSERT_EQ(array2[0].value, 110);
  ASSERT_EQ(array2[1].value, 120);
  ASSERT_EQ(array2[2].value, 130);
  ASSERT_EQ(array2[3].value, 140);
  ASSERT_EQ(array2[4].value, 150);
}

using zero_size_element_types = ::testing::Types<int, user_type>;

template <typename ElementType>
class ZeroSizedArrayTests : public ::testing::Test {};

TYPED_TEST_SUITE(ZeroSizedArrayTests, zero_size_element_types, );

/// @test Zero-sized `array`s have a `size` and `max_size` of zero, `empty` returns `true`, they are
/// nothrow-default-constructible, return an unspecified value for `data`, and the begin/end pairs of iterators are
/// equal, as there are no elements, though they are still valid random access iterators.
/// @tparam TypeParam The element type of the array
TYPED_TEST(ZeroSizedArrayTests, ZeroSizedArray) {
  using arr_t = arene::base::array<TypeParam, 0>;

  static_assert(std::is_nothrow_default_constructible<arr_t>::value, "Nothrow default constructible");

  arr_t array{};

  ::testing::StaticAssertTypeEq<decltype(array.size()), std::size_t>();
  ::testing::StaticAssertTypeEq<decltype(array.max_size()), std::size_t>();
  ::testing::StaticAssertTypeEq<decltype(array.empty()), bool>();
  ASSERT_EQ(array.size(), 0);
  ASSERT_EQ(array.max_size(), 0);
  ASSERT_TRUE(array.empty());
  ASSERT_EQ(array.begin(), array.end());

  ::testing::StaticAssertTypeEq<decltype(array.data()), TypeParam*>();
  std::ignore = array.data();

  ::testing::StaticAssertTypeEq<decltype(array.begin()), typename arr_t::iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.end()), typename arr_t::iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.cbegin()), typename arr_t::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.cend()), typename arr_t::const_iterator>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<typename arr_t::iterator>::iterator_category,
      std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<typename std::iterator_traits<typename arr_t::iterator>::value_type, TypeParam>();
  ::testing::StaticAssertTypeEq<typename std::iterator_traits<typename arr_t::iterator>::reference, TypeParam&>();
  ::testing::StaticAssertTypeEq<typename std::iterator_traits<typename arr_t::iterator>::pointer, TypeParam*>();
  ::testing::
      StaticAssertTypeEq<typename std::iterator_traits<typename arr_t::iterator>::difference_type, std::ptrdiff_t>();

  ::testing::StaticAssertTypeEq<decltype(*array.begin()), TypeParam&>();
  ::testing::StaticAssertTypeEq<decltype(array.rbegin()), typename arr_t::reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.rend()), typename arr_t::reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.crbegin()), typename arr_t::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(array.crend()), typename arr_t::const_reverse_iterator>();

  ASSERT_EQ(array.rbegin(), array.rend());
  ASSERT_EQ(array.crbegin(), array.crend());

  auto const& const_array = array;
  ::testing::StaticAssertTypeEq<decltype(const_array.size()), std::size_t>();
  ::testing::StaticAssertTypeEq<decltype(const_array.max_size()), std::size_t>();
  ::testing::StaticAssertTypeEq<decltype(const_array.cbegin()), typename arr_t::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_array.cend()), typename arr_t::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_array.empty()), bool>();
  ASSERT_EQ(const_array.size(), 0);
  ASSERT_EQ(const_array.max_size(), 0);
  ASSERT_TRUE(const_array.empty());

  ::testing::StaticAssertTypeEq<decltype(const_array.begin()), typename arr_t::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_array.end()), typename arr_t::const_iterator>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<typename arr_t::const_iterator>::iterator_category,
      std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<typename std::iterator_traits<typename arr_t::const_iterator>::value_type, TypeParam>();
  ::testing::
      StaticAssertTypeEq<typename std::iterator_traits<typename arr_t::const_iterator>::reference, TypeParam const&>();
  ::testing::
      StaticAssertTypeEq<typename std::iterator_traits<typename arr_t::const_iterator>::pointer, TypeParam const*>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<typename arr_t::const_iterator>::difference_type,
      std::ptrdiff_t>();

  ::testing::StaticAssertTypeEq<decltype(*const_array.begin()), TypeParam const&>();

  ::testing::StaticAssertTypeEq<decltype(const_array.rbegin()), typename arr_t::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_array.rend()), typename arr_t::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_array.crbegin()), typename arr_t::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_array.crend()), typename arr_t::const_reverse_iterator>();

  ASSERT_EQ(const_array.rbegin(), const_array.rend());
  ASSERT_EQ(const_array.crbegin(), const_array.crend());

  ASSERT_EQ(const_array.begin(), const_array.end());
  ASSERT_EQ(const_array.begin(), array.cbegin());
  ASSERT_EQ(array.cbegin(), array.cend());
  ASSERT_EQ(const_array.cbegin(), const_array.cend());
  ::testing::StaticAssertTypeEq<decltype(const_array.data()), TypeParam const*>();
  std::ignore = const_array.data();

  constexpr arr_t constexpr_array{};

  // NOLINTNEXTLINE(readability-container-size-empty)
  static_assert(constexpr_array.size() == 0, "Size");
  static_assert(constexpr_array.max_size() == 0, "Max size");
  static_assert(constexpr_array.empty(), "Empty");
  static_assert(constexpr_array.begin() == constexpr_array.end(), "Empty");

  ::testing::StaticAssertTypeEq<typename arr_t::value_type, TypeParam>();
  ::testing::StaticAssertTypeEq<typename arr_t::pointer, TypeParam*>();
  ::testing::StaticAssertTypeEq<typename arr_t::const_pointer, TypeParam const*>();
  ::testing::StaticAssertTypeEq<typename arr_t::reference, TypeParam&>();
  ::testing::StaticAssertTypeEq<typename arr_t::const_reference, TypeParam const&>();
  ::testing::StaticAssertTypeEq<typename arr_t::size_type, std::size_t>();
  ::testing::StaticAssertTypeEq<typename arr_t::difference_type, std::ptrdiff_t>();
  static_assert(!std::is_void<typename arr_t::iterator>::value, "Type must be correct");
  static_assert(!std::is_void<typename arr_t::const_iterator>::value, "Type must be correct");
  ::testing::
      StaticAssertTypeEq<typename arr_t::reverse_iterator, ::arene::base::reverse_iterator<typename arr_t::iterator>>();
  ::testing::StaticAssertTypeEq<
      typename arr_t::const_reverse_iterator,
      ::arene::base::reverse_iterator<typename arr_t::const_iterator>>();

  static_assert(arene::base::is_nothrow_swappable_v<arr_t>, "Nothrow swap");

  ::testing::StaticAssertTypeEq<decltype(array.front()), TypeParam&>();
  ::testing::StaticAssertTypeEq<decltype(const_array.front()), TypeParam const&>();
  ::testing::StaticAssertTypeEq<decltype(array.back()), TypeParam&>();
  ::testing::StaticAssertTypeEq<decltype(const_array.back()), TypeParam const&>();
  ::testing::StaticAssertTypeEq<decltype(array.data()), TypeParam*>();
  ::testing::StaticAssertTypeEq<decltype(const_array.data()), TypeParam const*>();
  ::testing::StaticAssertTypeEq<decltype(array[0]), TypeParam&>();
  ::testing::StaticAssertTypeEq<decltype(const_array[0]), TypeParam const&>();

  swap(array, array);

  static_assert(arene::base::is_nothrow_swappable_v<arr_t>, "nothrow swap");
  static_assert(noexcept(std::declval<arr_t&>().fill(TypeParam{})), "nothrow fill");
  array.fill(TypeParam{});
}

/// @test It is possible to create an `array` with a non-swappable element type, and access the elements of that `array`
TEST(Array, CanCreateArrayOfNonSwappableTypes) {
  constexpr auto val = 42;
  struct non_swappable {
    int value = val;
    non_swappable() = default;
    ~non_swappable() = default;

    non_swappable(non_swappable const&) = delete;
    auto operator=(non_swappable const&) -> non_swappable& = delete;
    non_swappable(non_swappable&&) = delete;
    auto operator=(non_swappable&&) -> non_swappable& = delete;
  };

  constexpr auto size = 5;
  using arr_t = arene::base::array<non_swappable, size>;

  static_assert(!arene::base::is_swappable_v<arr_t>, "Not swappable");
  arr_t arr{};
  ASSERT_EQ(arr[0].value, val);
}

/// @test Zero-sized `array`s of the same type are always equal
TEST(ArrayComparison, EmptyArraysAreAlwaysEqual) {
  // NOLINTNEXTLINE(readability-container-size-empty) explicitly testing empty container comparison
  STATIC_ASSERT_EQ((arene::base::array<int, 0>{}), (arene::base::array<int, 0>{}));
  // NOLINTNEXTLINE(readability-container-size-empty) explicitly testing empty container comparison
  STATIC_ASSERT_FALSE(arene::base::array<int, 0>{} != arene::base::array<int, 0>{});
}

/// @test Zero-sized `array`s of the same type are always equal, and the relational operators reflect that
TEST(ArrayComparison, EmptyArraysRelationallyCompareEqualOnly) {
  STATIC_ASSERT_FALSE(arene::base::array<int, 0>{} < arene::base::array<int, 0>{});
  STATIC_ASSERT_TRUE(arene::base::array<int, 0>{} <= arene::base::array<int, 0>{});
  STATIC_ASSERT_FALSE(arene::base::array<int, 0>{} > arene::base::array<int, 0>{});
  STATIC_ASSERT_TRUE(arene::base::array<int, 0>{} >= arene::base::array<int, 0>{});
}

/// @test Non-zero-sized `array`s of the same type are equal if their elements are equal
TEST(ArrayComparison, ArraysWithEqualElementsCompareEqual) {
  STATIC_ASSERT_EQ((arene::base::array<int, 1>{1}), (arene::base::array<int, 1>{1}));
  STATIC_ASSERT_FALSE(arene::base::array<int, 1>{1} != arene::base::array<int, 1>{1});
  STATIC_ASSERT_EQ((arene::base::array<int, 2>{1, 2}), (arene::base::array<int, 2>{1, 2}));
  STATIC_ASSERT_FALSE(arene::base::array<int, 2>{1, 2} != arene::base::array<int, 2>{1, 2});
  constexpr arene::base::array<int, 10> big_arr{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  STATIC_ASSERT_EQ(big_arr, big_arr);
  STATIC_ASSERT_FALSE(big_arr != big_arr);
}

/// @test Non-zero-sized `array`s of the same type are not equal if any of their elements are not equal
TEST(ArrayComparison, ArraysWithAnyUnequalElementsCompareUnequal) {
  STATIC_ASSERT_NE((arene::base::array<int, 1>{1}), (arene::base::array<int, 1>{2}));
  STATIC_ASSERT_FALSE(arene::base::array<int, 1>{1} == arene::base::array<int, 1>{2});
  STATIC_ASSERT_NE((arene::base::array<int, 2>{2, 1}), (arene::base::array<int, 2>{1, 2}));
  STATIC_ASSERT_FALSE(arene::base::array<int, 2>{2, 1} == arene::base::array<int, 2>{1, 2});
  constexpr arene::base::array<int, 10> big_arr{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  for (std::size_t idx = 0; idx < big_arr.size(); ++idx) {
    auto permuted_arr = big_arr;
    ++permuted_arr[idx];
    ASSERT_NE(big_arr, permuted_arr);
  }
}

/// @test When non-zero-sized `array`s of the same type are compared using the relational operators, the result is
/// consistent with a lexicographical ordering of the elements.
TEST(ArrayComparison, ArraysWithDifferentElementsRelationallyCompareLexicographically) {
  STATIC_ASSERT_LT((arene::base::array<int, 1>{1}), (arene::base::array<int, 1>{2}));
  STATIC_ASSERT_LT((arene::base::array<int, 3>{1, 2, 3}), (arene::base::array<int, 3>{1, 3, 1}));

  STATIC_ASSERT_LE((arene::base::array<int, 1>{1}), (arene::base::array<int, 1>{2}));
  STATIC_ASSERT_LE((arene::base::array<int, 1>{1}), (arene::base::array<int, 1>{1}));
  STATIC_ASSERT_LE((arene::base::array<int, 3>{1, 2, 3}), (arene::base::array<int, 3>{1, 3, 1}));
  STATIC_ASSERT_LE((arene::base::array<int, 3>{1, 2, 3}), (arene::base::array<int, 3>{1, 2, 3}));

  STATIC_ASSERT_GT((arene::base::array<int, 1>{2}), (arene::base::array<int, 1>{1}));
  STATIC_ASSERT_GT((arene::base::array<int, 3>{1, 3, 1}), (arene::base::array<int, 3>{1, 2, 3}));

  STATIC_ASSERT_GE((arene::base::array<int, 1>{2}), (arene::base::array<int, 1>{1}));
  STATIC_ASSERT_GE((arene::base::array<int, 1>{1}), (arene::base::array<int, 1>{1}));
  STATIC_ASSERT_GE((arene::base::array<int, 3>{1, 3, 1}), (arene::base::array<int, 3>{1, 2, 3}));
  STATIC_ASSERT_GE((arene::base::array<int, 3>{1, 2, 3}), (arene::base::array<int, 3>{1, 2, 3}));
}

/// @test `array`s with different sizes are not equal
TEST(ArrayComparison, ArraysWithDifferentSizesCompareUnequal) {
  // NOLINTNEXTLINE(readability-container-size-empty) explicitly testing empty container comparison
  STATIC_ASSERT_FALSE(arene::base::array<int, 0>{} == arene::base::array<int, 1>{1});
  // NOLINTNEXTLINE(readability-container-size-empty) explicitly testing empty container comparison
  STATIC_ASSERT_TRUE(arene::base::array<int, 0>{} != arene::base::array<int, 1>{1});

  STATIC_ASSERT_FALSE(arene::base::array<int, 2>{1, 2} == arene::base::array<int, 1>{1});
  STATIC_ASSERT_TRUE(arene::base::array<int, 2>{1, 2} != arene::base::array<int, 1>{1});
}

/// @test When non-zero-sized `array`s of different sizes, where the elements of the smaller array are equal to the
/// corresponding elements of the larger array, the smaller array is "less than" the larger one.
TEST(ArrayComparison, ArraysWithDifferentSizesWhichArePrefixesRelationallyCompareLess) {
  STATIC_ASSERT_LT((arene::base::array<int, 0>{}), (arene::base::array<int, 1>{1}));
  STATIC_ASSERT_LT((arene::base::array<int, 1>{1}), (arene::base::array<int, 2>{1, 1}));
  STATIC_ASSERT_LE((arene::base::array<int, 0>{}), (arene::base::array<int, 1>{1}));
  STATIC_ASSERT_LE((arene::base::array<int, 1>{1}), (arene::base::array<int, 2>{1, 1}));

  STATIC_ASSERT_GT((arene::base::array<int, 1>{1}), (arene::base::array<int, 0>{}));
  STATIC_ASSERT_GT((arene::base::array<int, 2>{1, 1}), (arene::base::array<int, 1>{1}));
  STATIC_ASSERT_GE((arene::base::array<int, 1>{1}), (arene::base::array<int, 0>{}));
  STATIC_ASSERT_GE((arene::base::array<int, 2>{1, 1}), (arene::base::array<int, 1>{1}));
}

/// @test When non-zero-sized `array`s of the different sizes are compared using the relational operators, the result is
/// consistent with a lexicographical ordering of the elements.
TEST(ArrayComparison, ArraysWithDifferentSizesWhichAreNotPrefixesRelationallyCompareNormally) {
  STATIC_ASSERT_LT((arene::base::array<int, 2>{1, 1}), (arene::base::array<int, 1>{2}));
  STATIC_ASSERT_LE((arene::base::array<int, 2>{1, 1}), (arene::base::array<int, 1>{2}));

  STATIC_ASSERT_GT((arene::base::array<int, 2>{2, 1}), (arene::base::array<int, 1>{1}));
  STATIC_ASSERT_GE((arene::base::array<int, 2>{2, 1}), (arene::base::array<int, 1>{1}));
}

/// @test An `array` holding a type which only provides equality comparisons is equality-comparable
TEST(ArrayComparison, ArraysWithNonRelationallyComparableElementsStillCanBeComparedForEquality) {
  struct only_equality {
    // NOLINTNEXTLINE(hicpp-explicit-conversions) want implicit conversion
    constexpr only_equality(int val)
        : value(val) {}
    int value;

    constexpr auto operator==(only_equality const& rhs) const -> bool { return value == rhs.value; }
    constexpr auto operator!=(only_equality const& rhs) const -> bool { return !(*this == rhs); }
  };

  arene::base::array<only_equality, 0> empty{};

  ASSERT_EQ(empty.begin(), empty.end());

  arene::base::array<only_equality, 0> const& const_empty = empty;

  ASSERT_EQ(const_empty.begin(), const_empty.end());

  // NOLINTNEXTLINE(readability-container-size-empty) explicitly testing empty container comparison
  STATIC_ASSERT_EQ((arene::base::array<only_equality, 0>{}), (arene::base::array<only_equality, 0>{}));
  // NOLINTNEXTLINE(readability-container-size-empty) explicitly testing empty container comparison
  STATIC_ASSERT_FALSE(arene::base::array<only_equality, 0>{} != arene::base::array<only_equality, 0>{});

  STATIC_ASSERT_EQ((arene::base::array<only_equality, 3>{1, 2, 3}), (arene::base::array<only_equality, 3>{1, 2, 3}));
  STATIC_ASSERT_FALSE(
      (arene::base::array<only_equality, 3>{1, 2, 3}) != (arene::base::array<only_equality, 3>{1, 2, 3})
  );
}

/// @test An `array` holding a type which declares both equality and ordering but only defines the former still works.
TEST(ArrayComparison, ArraysWithDeclaredButBrokenLessThanCanBeComparedForEquality) {
  constexpr arene::base::array<testing::has_broken_less_than, 2> two{1, 2};

  STATIC_ASSERT_EQ(two, two);
  STATIC_ASSERT_FALSE(two != two);

  constexpr arene::base::array<testing::has_broken_less_than, 3> three{1, 2, 3};

  STATIC_ASSERT_EQ(three, three);
  STATIC_ASSERT_NE(three, two);
  STATIC_ASSERT_NE(two, three);
  STATIC_ASSERT_FALSE(three != three);

  // Using ordering assertions like STATIC_ASSERT_LT would cause a compilation error.
}

///  @test `to_array` can be used to construct an `array` from a built-in array, deducing the size and element type from
///  the supplied built-in array. The elements of the constructed `array` are copy-constructed from the elements of the
///  source array.
TEST(Array, ToArray) {
  constexpr auto size = 5;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  constexpr std::int32_t source[size] = {1, 22, 333, 4444, 55555};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::int32_t source2[size] = {1, 22, 333, 4444, 55555};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::int32_t const(&source3)[size] = source2;

  constexpr auto dest = arene::base::to_array(source);
  auto dest2 = arene::base::to_array(source2);
  auto runtime_const_array = arene::base::to_array(source3);

  ::testing::StaticAssertTypeEq<decltype(dest), arene::base::array<std::int32_t, size> const>();
  ::testing::StaticAssertTypeEq<decltype(dest2), arene::base::array<std::int32_t, size>>();

  for (unsigned idx = 0; idx < size; ++idx) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(dest[idx], source[idx]);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(dest2[idx], source2[idx]);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(runtime_const_array[idx], source3[idx]);
  }
}

///  @test `to_array` can be used to construct an `array` from an rvalue built-in array, deducing the size and element
///  type from the supplied built-in array. The elements of the constructed `array` are move-constructed from the
///  corresponding elements from the source array.
TEST(Array, ToArrayForRValues) {
  constexpr auto val = -123;
  struct custom_type {
    int value = val;
    custom_type() noexcept = default;
    custom_type(custom_type const&) noexcept = default;
    auto operator=(custom_type const&) noexcept -> custom_type& = default;
    custom_type(custom_type&& other) noexcept
        : value(std::exchange(other.value, 0)) {}
    auto operator=(custom_type&&) noexcept -> custom_type& = delete;
    ~custom_type() = default;
  };

  constexpr auto size = 5;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  custom_type source[size]{};

  auto dest = arene::base::to_array(std::move(source));

  ::testing::StaticAssertTypeEq<decltype(dest), arene::base::array<custom_type, size>>();

  for (unsigned i = 0; i < size; ++i) {
    ASSERT_EQ(dest[i].value, val);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(source[i].value, 0);
  }
}

/// @test If a `std::index_sequence` is passed to the internal `detail::to_array` function in addition to the source
/// array, then the resulting array contains the elements specified by the index sequence values from the source array
/// as the initial elements, and the remaining elements are default-constructed
TEST(Array, ToArrayForSelectedIndices) {
  constexpr auto size = 5;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t source[size]{10, 20, 30, 40, 50};

  auto dest = arene::base::array_detail::to_array(source, std::index_sequence<2, 3, 0>{});

  ::testing::StaticAssertTypeEq<decltype(dest), arene::base::array<std::uint32_t, size>>();

  ASSERT_EQ(dest[0], 30);
  ASSERT_EQ(dest[1], 40);
  ASSERT_EQ(dest[2], 10);
  ASSERT_EQ(dest[3], 0);
  ASSERT_EQ(dest[4], 0);
}

/// @test If a `std::index_sequence` is passed to the internal `detail::to_array` function in addition to the source
/// array, then the resulting array contains elements move-constructed from the elements of the source array specified
/// by the index sequence values as the initial elements, and the remaining elements are default-constructed
TEST(Array, ToArrayForSelectedIndicesFromRValue) {
  constexpr auto size = 5;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t source[size]{10, 20, 30, 40, 50};

  // NOLINTNEXTLINE(hicpp-move-const-arg)
  auto const dest = arene::base::array_detail::to_array(std::move(source), std::index_sequence<2, 3, 0>{});

  ::testing::StaticAssertTypeEq<decltype(dest), arene::base::array<std::uint32_t, size> const>();

  ASSERT_EQ(dest[0], 30);
  ASSERT_EQ(dest[1], 40);
  ASSERT_EQ(dest[2], 10);
  ASSERT_EQ(dest[3], 0);
  ASSERT_EQ(dest[4], 0);
}

/// @test use of 'operator[]' does not prevent compilation if used in a constant expression
CONSTEXPR_TEST(Array, IndexOperatorConstexpr) {
  constexpr auto size = std::size_t{};
  auto const values = arene::base::array<std::size_t, size>{};

  for (auto const dim : arene::base::sequential_values<std::size_t, size>) {
    ARENE_IGNORE_START();
    ARENE_IGNORE_ALL("-Wunreachable-code", "even though this is unreachable, it should not prevent compilation");
    std::ignore = values[dim];
    ARENE_IGNORE_END();
  }
}

namespace {
template <typename From, typename To>
struct conversion_should_succeed;

template <typename FromT, std::size_t FromSize, typename To>
struct conversion_should_succeed<arene::base::array<FromT, FromSize>, To> {
  static constexpr bool value = std::is_same<FromT, typename To::value_type>::value &&
                                (FromSize == To::extent || To::extent == arene::base::dynamic_extent);
};

template <typename FromT, std::size_t FromSize, typename To>
struct conversion_should_succeed<arene::base::array<FromT, FromSize> const, To> {
  static constexpr bool value = std::is_same<FromT, typename To::value_type>::value &&
                                std::is_const<typename To::element_type>::value &&
                                (FromSize == To::extent || To::extent == arene::base::dynamic_extent);
};

template <typename From, typename To>
constexpr bool conversion_should_succeed_v = conversion_should_succeed<From, To>::value;

template <typename From, typename To>
struct imp_conv_1_to_1 {
  static constexpr bool should_succeed = conversion_should_succeed_v<From, To>;
  static constexpr bool does_succeed = std::is_convertible<From, To>::value;
  static constexpr bool value = (should_succeed == does_succeed);
  static_assert(should_succeed == does_succeed, "array->span implicit conversion test failed");
};

template <typename From, typename To>
struct imp_conv_n_to_1 {};

template <typename To>
struct imp_conv_n_to_1<arene::base::type_list<>, To> {
  static constexpr bool value = true;
};

template <typename NextFrom, typename To, typename... From>
struct imp_conv_n_to_1<arene::base::type_list<NextFrom, From...>, To> {
  static constexpr bool value =
      imp_conv_1_to_1<NextFrom, To>::value && imp_conv_n_to_1<arene::base::type_list<From...>, To>::value;
};

template <typename From, typename To>
struct implicit_conversion_test {};

template <typename... From>
struct implicit_conversion_test<arene::base::type_list<From...>, arene::base::type_list<>> {
  static constexpr bool value = true;
};

template <typename NextTo, typename... From, typename... To>
struct implicit_conversion_test<arene::base::type_list<From...>, arene::base::type_list<NextTo, To...>> {
  static constexpr bool value =
      imp_conv_n_to_1<arene::base::type_list<From...>, NextTo>::value &&
      implicit_conversion_test<arene::base::type_list<From...>, arene::base::type_list<To...>>::value;
};
}  // namespace

/// @test Instances of `array` are implicitly convertible to instances of `span` with the same element type, and either
/// an extent equal to the size of the array, or a dynamic extent.
TEST(Span, ImplicitlyConvertibleToSpan) {
  using arrays = arene::base::type_list<
      arene::base::array<int, 0>,
      arene::base::array<int, 5>,
      arene::base::array<int, 0> const,
      arene::base::array<int, 5> const,
      arene::base::array<unsigned, 0>,
      arene::base::array<unsigned, 5>,
      arene::base::array<unsigned, 0> const,
      arene::base::array<unsigned, 5> const>;

  using spans = arene::base::type_list<
      arene::base::span<int, 0>,
      arene::base::span<int, 5>,
      arene::base::span<int, 9>,
      arene::base::span<int, arene::base::dynamic_extent>,
      arene::base::span<unsigned, 0>,
      arene::base::span<unsigned, 5>,
      arene::base::span<unsigned, 9>,
      arene::base::span<unsigned, arene::base::dynamic_extent>,
      arene::base::span<int const, 0>,
      arene::base::span<int const, 5>,
      arene::base::span<int const, 9>,
      arene::base::span<int const, arene::base::dynamic_extent>>;

  constexpr bool value = implicit_conversion_test<arrays, spans>::value;
  ASSERT_TRUE(value);
}

template <typename T>
class ArrayTupleProtocolTest : public ::testing::Test {
 public:
  static constexpr std::size_t size = T();
};

template <typename T>
constexpr std::size_t ArrayTupleProtocolTest<T>::size;

using non_zero_array_sizes = ::testing::Types<
    std::integral_constant<std::size_t, 1U>,
    std::integral_constant<std::size_t, 2U>,
    std::integral_constant<std::size_t, 3U>,
    std::integral_constant<std::size_t, 4U>,
    std::integral_constant<std::size_t, 5U>>;

using array_sizes = ::arene::base::type_lists::
    concat_t<::testing::Types<std::integral_constant<std::size_t, 0U>>, non_zero_array_sizes>;

template <typename T>
class ArrayTupleSizeTest : public ArrayTupleProtocolTest<T> {};

TYPED_TEST_SUITE(ArrayTupleSizeTest, array_sizes, );

/// @test Given an `arene::base::array<T,N>`, when `std::tuple_size<arene::base::array<T,N>>` is queried, then the value
/// is `N`.
TYPED_TEST(ArrayTupleSizeTest, TupleSizeIsN) {
  STATIC_ASSERT_EQ((std::tuple_size<arene::base::array<int, TestFixture::size>>::value), TestFixture::size);
}

template <typename T>
class ArrayTupleElementTest : public ArrayTupleProtocolTest<T> {};

TYPED_TEST_SUITE(ArrayTupleElementTest, non_zero_array_sizes, );

/// @test Given an `arene::base::array<T,N>`, when `std::tuple_element<I,arene::base::array<T,N>>` is called with `I<N`,
/// then the return type is `value_type`.
TYPED_TEST(ArrayTupleElementTest, TupleElementIForItoNIsValueType) {
  using array_t = arene::base::array<int, TestFixture::size>;
  ::testing::StaticAssertTypeEq<std::tuple_element_t<0, array_t>, typename array_t::value_type>();
  ::testing::StaticAssertTypeEq<std::tuple_element_t<0, array_t const>, typename array_t::value_type const>();

  ::testing::StaticAssertTypeEq<std::tuple_element_t<TestFixture::size - 1U, array_t>, typename array_t::value_type>();
  ::testing::StaticAssertTypeEq<
      std::tuple_element_t<TestFixture::size - 1U, array_t const>,
      typename array_t::value_type const>();
}

using value_types = ::testing::Types<std::uint8_t, std::size_t, testing::configurable_value<int>>;

template <typename T>
class ArrayGetTest : public testing::Test {
 protected:
  using array_t = arene::base::array<T, 5>;
  template <std::size_t... Idxs>
  static constexpr auto make_array(std::index_sequence<Idxs...>) -> array_t {
    return array_t{static_cast<T>(Idxs)...};
  }

  static constexpr auto make_array() { return make_array(std::make_index_sequence<array_t::size - 1U>{}); }
};

TYPED_TEST_SUITE(ArrayGetTest, value_types, );

/// @test Given an instance `arene::base::array<T,N> arr`, when `get<I>(arr)` is called with `I<N`,
/// then the return type is `value_type` with cref qualification matching `arr`.
TYPED_TEST(ArrayGetTest, GetIForItoNIsValueType) {
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::get<0>(std::declval<typename TestFixture::array_t&>())),
      typename TestFixture::array_t::value_type&>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::get<0>(std::declval<typename TestFixture::array_t const&>())),
      typename TestFixture::array_t::value_type const&>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::get<0>(std::declval<typename TestFixture::array_t&&>())),
      typename TestFixture::array_t::value_type&&>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::get<0>(std::declval<typename TestFixture::array_t&&>())),
      typename TestFixture::array_t::value_type&&>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::get<0>(std::declval<typename TestFixture::array_t const&&>())),
      typename TestFixture::array_t::value_type const&&>();

  constexpr auto last_index = TestFixture::array_t::size - 1U;
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::get<last_index>(std::declval<typename TestFixture::array_t&>())),
      typename TestFixture::array_t::value_type&>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::get<last_index>(std::declval<typename TestFixture::array_t const&>())),
      typename TestFixture::array_t::value_type const&>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::get<last_index>(std::declval<typename TestFixture::array_t&&>())),
      typename TestFixture::array_t::value_type&&>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::get<last_index>(std::declval<typename TestFixture::array_t const&&>())),
      typename TestFixture::array_t::value_type const&&>();
}

/// @test Given an instance `arene::base::array<T,N> arr`, when `get<I>(arr)` is called with `I<N`,
/// then it is noexcept.
TYPED_TEST(ArrayGetTest, GetIForItoNIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(arene::base::get<0>(std::declval<typename TestFixture::array_t&>())));
  STATIC_ASSERT_TRUE(noexcept(arene::base::get<0>(std::declval<typename TestFixture::array_t const&>())));
  STATIC_ASSERT_TRUE(noexcept(arene::base::get<0>(std::declval<typename TestFixture::array_t&&>())));
  STATIC_ASSERT_TRUE(noexcept(arene::base::get<0>(std::declval<typename TestFixture::array_t const&&>())));
  constexpr auto last_index = TestFixture::array_t::size - 1U;
  STATIC_ASSERT_TRUE(noexcept(arene::base::get<last_index>(std::declval<typename TestFixture::array_t&>())));
  STATIC_ASSERT_TRUE(noexcept(arene::base::get<last_index>(std::declval<typename TestFixture::array_t const&>())));
  STATIC_ASSERT_TRUE(noexcept(arene::base::get<last_index>(std::declval<typename TestFixture::array_t&&>())));
  STATIC_ASSERT_TRUE(noexcept(arene::base::get<last_index>(std::declval<typename TestFixture::array_t const&&>())));
}

template <typename IntConstI, typename ArrayT>
using use_get_at_i = decltype(arene::base::get<IntConstI::value>(std::declval<ArrayT>()));

template <std::size_t I, typename ArrayT>
constexpr bool has_get_at_i =
    ::arene::base::substitution_succeeds<use_get_at_i, std::integral_constant<std::size_t, I>, ArrayT>;

/// @test Given an instance `arene::base::array<T,N> arr`, when `get<I>(arr)` is called with `I>=N`,
/// then it does not participate in overload resolution.
TYPED_TEST(ArrayGetTest, GetIForIGENDoesNotParticipateInOverloadResolution) {
  constexpr auto size = TestFixture::array_t::size;
  STATIC_ASSERT_FALSE(has_get_at_i<size, typename TestFixture::array_t&>);
  STATIC_ASSERT_FALSE(has_get_at_i<size, typename TestFixture::array_t const&>);
  STATIC_ASSERT_FALSE(has_get_at_i<size, typename TestFixture::array_t&&>);
  STATIC_ASSERT_FALSE(has_get_at_i<size, typename TestFixture::array_t const&&>);
}

template <std::size_t I, typename ArrayT>
constexpr auto is_array_get_equal_to_index_lv(ArrayT arr) -> bool {
  return arene::base::get<I>(arr) == arr[I];
}

template <std::size_t I, typename ArrayT>
constexpr auto is_array_get_equal_to_index_clv(ArrayT const& arr) -> bool {
  return arene::base::get<I>(arr) == arr[I];
}

template <std::size_t I, typename ArrayT>
constexpr auto is_array_get_equal_to_index_rv(ArrayT arr) -> bool {
  // NOLINTNEXTLINE(bugprone-use-after-move) This is known safe and selects the correct overload
  return arene::base::get<I>(std::move(arr)) == arr[I];
}

template <std::size_t I, typename ArrayT>
constexpr auto is_array_get_equal_to_index_crv(ArrayT const arr) -> bool {
  // NOLINTNEXTLINE(bugprone-use-after-move) This is known safe and selects the correct overload
  return arene::base::get<I>(std::move(arr)) == arr[I];
}

/// @test Given an instance `arene::base::array<T,N> arr`, when `get<I>(arr)` is called with `I<N`,
/// then the return is equivalent to `arr[I]`.
TYPED_TEST(ArrayGetTest, GetIForItoNReturnsIthElementOfArray) {
  constexpr auto arr = TestFixture::make_array();
  constexpr auto last_index = TestFixture::array_t::size - 1U;
  STATIC_ASSERT_TRUE(is_array_get_equal_to_index_lv<0>(arr));
  STATIC_ASSERT_TRUE(is_array_get_equal_to_index_lv<last_index>(arr));
  STATIC_ASSERT_TRUE(is_array_get_equal_to_index_clv<0>(arr));
  STATIC_ASSERT_TRUE(is_array_get_equal_to_index_clv<last_index>(arr));
  STATIC_ASSERT_TRUE(is_array_get_equal_to_index_rv<0>(arr));
  STATIC_ASSERT_TRUE(is_array_get_equal_to_index_rv<last_index>(arr));
  STATIC_ASSERT_TRUE(is_array_get_equal_to_index_crv<0>(arr));
  STATIC_ASSERT_TRUE(is_array_get_equal_to_index_crv<last_index>(arr));
}

enum class comparison_flag { enabled, disabled, uncallable };

template <comparison_flag Equality, comparison_flag ThreeWay, comparison_flag FastInequality>
class ComparisonTester {
  int value_;

 public:
  explicit constexpr ComparisonTester(int value) noexcept
      : value_(value) {}

  constexpr auto get() const noexcept -> int { return value_; }

  template <
      bool Enable = (Equality != comparison_flag::disabled),
      arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  friend constexpr auto operator==(ComparisonTester const& lhs, ComparisonTester const& rhs) noexcept -> bool {
    if (Equality == comparison_flag::uncallable) {
      ARENE_INVARIANT_UNREACHABLE("Called operator== which should not happen");
    }

    return lhs.value_ == rhs.value_;
  }

  template <
      bool Enable = (ThreeWay != comparison_flag::disabled),
      arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  static constexpr auto three_way_compare(ComparisonTester const& lhs, ComparisonTester const& rhs) noexcept
      -> arene::base::strong_ordering {
    if (ThreeWay == comparison_flag::uncallable) {
      ARENE_INVARIANT_UNREACHABLE("Called three_way_compare which should not happen");
    }

    return arene::base::compare_three_way{}(lhs.get(), rhs.get());
  }

  template <
      bool Enable = (FastInequality != comparison_flag::disabled),
      arene::base::constraints<std::enable_if_t<Enable>> = nullptr>
  static constexpr auto fast_inequality_check(ComparisonTester const& lhs, ComparisonTester const& rhs) noexcept
      -> arene::base::inequality_heuristic {
    if (FastInequality == comparison_flag::uncallable) {
      ARENE_INVARIANT_UNREACHABLE("Called fast_inequality_check which should not happen");
    }

    if (lhs.get() != rhs.get()) {
      return arene::base::inequality_heuristic::definitely_not_equal;
    }

    return arene::base::inequality_heuristic::may_be_equal_or_not_equal;
  }
};

/// @test The equality and inequality operators of an `arene::base::array` do not call `three_way_compare` when an
/// underlying `operator==` is available
TEST(ArrayComparisonTest, EqualityDoesNotCallThreeWayCompare) {
  using eq_only = ComparisonTester<comparison_flag::enabled, comparison_flag::uncallable, comparison_flag::uncallable>;
  using array_type = arene::base::array<eq_only, 3U>;
  using zero_array_type = arene::base::array<eq_only, 0U>;

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<array_type>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<zero_array_type>);

  constexpr array_type first{eq_only{1}, eq_only{2}, eq_only{3}};
  STATIC_ASSERT_EQ(first, first);

  constexpr array_type second{eq_only{4}, eq_only{5}, eq_only{6}};
  STATIC_ASSERT_NE(first, second);

  constexpr zero_array_type zero{};
  STATIC_ASSERT_EQ(zero, zero);
  STATIC_ASSERT_NE(zero, first);
  STATIC_ASSERT_NE(first, zero);

  // If ordered comparisons *exist*, then ordering with a 0-size array can be computed without actually calling them.
  STATIC_ASSERT_LT(zero, first);
  STATIC_ASSERT_LE(zero, first);
  STATIC_ASSERT_FALSE(zero > first);
  STATIC_ASSERT_FALSE(zero >= first);

  STATIC_ASSERT_FALSE(first < zero);
  STATIC_ASSERT_FALSE(first <= zero);
  STATIC_ASSERT_GT(first, zero);
  STATIC_ASSERT_GE(first, zero);
}

/// @test The equality and inequality operators of an `arene::base::array` call `three_way_compare` when no underlying
/// `operator==` is available
TEST(ArrayComparisonTest, ThreeWayCompareWorksWithoutEquality) {
  using tw_only = ComparisonTester<comparison_flag::disabled, comparison_flag::enabled, comparison_flag::disabled>;
  using array_type = arene::base::array<tw_only, 3U>;
  using zero_array_type = arene::base::array<tw_only, 0U>;

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<array_type>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<array_type>);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<array_type>);

  constexpr array_type first{tw_only{1}, tw_only{2}, tw_only{3}};
  STATIC_ASSERT_EQ(first, first);
  STATIC_ASSERT_EQ(arene::base::compare_three_way{}(first, first), arene::base::strong_ordering::equal);

  constexpr array_type second{tw_only{4}, tw_only{5}, tw_only{6}};
  STATIC_ASSERT_NE(first, second);
  STATIC_ASSERT_LT(first, second);
  STATIC_ASSERT_EQ(arene::base::compare_three_way{}(first, second), arene::base::strong_ordering::less);

  // All ordered comparisons also work with zero-sized arrays on either size (or both).
  constexpr zero_array_type zero{};
  STATIC_ASSERT_EQ(zero, zero);
  STATIC_ASSERT_FALSE(zero < zero);
  STATIC_ASSERT_LE(zero, zero);
  STATIC_ASSERT_FALSE(zero > zero);
  STATIC_ASSERT_GE(zero, zero);

  STATIC_ASSERT_NE(zero, first);
  STATIC_ASSERT_LT(zero, first);
  STATIC_ASSERT_LE(zero, first);
  STATIC_ASSERT_FALSE(zero > first);
  STATIC_ASSERT_FALSE(zero >= first);

  STATIC_ASSERT_NE(first, zero);
  STATIC_ASSERT_FALSE(first < zero);
  STATIC_ASSERT_FALSE(first <= zero);
  STATIC_ASSERT_GT(first, zero);
  STATIC_ASSERT_GE(first, zero);
}

/// @test The `three_way_compare` of an `arene::base::array` with no underlying `operator==` uses an underlying
/// `fast_inequality_check` to short-circuit when one is available
TEST(ArrayComparisonTest, ThreeWayCompareCallsHeuristicIfAvailable) {
  using tw_crashes = ComparisonTester<comparison_flag::disabled, comparison_flag::uncallable, comparison_flag::enabled>;
  using array_type = arene::base::array<tw_crashes, 3U>;
  using shorter_array_type = arene::base::array<tw_crashes, 2U>;
  using zero_array_type = arene::base::array<tw_crashes, 0U>;

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<array_type>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<array_type>);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<array_type>);

  constexpr array_type longer{tw_crashes{1}, tw_crashes{2}, tw_crashes{3}};
  constexpr shorter_array_type shorter{tw_crashes{4}, tw_crashes{5}};

  // Asserting equality would crash, but if the lengths are different they're always unequal without checking values.
  STATIC_ASSERT_NE(shorter, longer);

  // If the inequality heuristic shows they're different, the synthesized inequality doesn't call three_way_compare.
  constexpr array_type different_contents{tw_crashes{6}, tw_crashes{7}, tw_crashes{8}};
  STATIC_ASSERT_NE(longer, different_contents);

  // three_way_compare never actually needs to be called for zero-sized arrays.
  constexpr zero_array_type zero{};
  STATIC_ASSERT_EQ(zero, zero);
  STATIC_ASSERT_FALSE(zero < zero);
  STATIC_ASSERT_LE(zero, zero);
  STATIC_ASSERT_FALSE(zero > zero);
  STATIC_ASSERT_GE(zero, zero);

  STATIC_ASSERT_NE(zero, shorter);
  STATIC_ASSERT_LT(zero, shorter);
  STATIC_ASSERT_LE(zero, shorter);
  STATIC_ASSERT_FALSE(zero > shorter);
  STATIC_ASSERT_FALSE(zero >= shorter);

  STATIC_ASSERT_NE(shorter, zero);
  STATIC_ASSERT_FALSE(shorter < zero);
  STATIC_ASSERT_FALSE(shorter <= zero);
  STATIC_ASSERT_GT(shorter, zero);
  STATIC_ASSERT_GE(shorter, zero);
}

}  // namespace
