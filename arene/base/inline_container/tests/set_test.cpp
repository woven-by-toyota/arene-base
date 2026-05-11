// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/set.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/copy.hpp"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/stdlib_choice/reverse.hpp"
#include "arene/base/stdlib_choice/sort.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

using ::arene::base::compare_three_way;
using ::arene::base::inline_set;
using ::arene::base::make_strong_ordering;
using ::arene::base::next;
using ::arene::base::nullopt;
using ::arene::base::strong_ordering;

// A capacity large enough for most tests
constexpr std::size_t standard_capacity = 20;

// A set with a large enough capacity for most tests
template <typename T>
using standard_capacity_set = inline_set<T, standard_capacity>;

// A set with a fixed type and capacity to be used in most tests
using standard_set = standard_capacity_set<std::int32_t>;

// A comparator whose copy ctor and assignment are not noexcept
struct throwing_copy_ca_comparator {
  throwing_copy_ca_comparator() = default;
  // NOLINTBEGIN(hicpp-use-equals-default) testing throwing ctor specification
  throwing_copy_ca_comparator(throwing_copy_ca_comparator const&) noexcept(false) {}
  auto operator=(throwing_copy_ca_comparator const&) noexcept(false) -> throwing_copy_ca_comparator& { return *this; }
  // NOLINTEND(hicpp-use-equals-default)

  throwing_copy_ca_comparator(throwing_copy_ca_comparator&&) noexcept(false) = default;
  auto operator=(throwing_copy_ca_comparator&&) noexcept(false) -> throwing_copy_ca_comparator& = default;
  ~throwing_copy_ca_comparator() = default;

  auto operator()(std::int32_t, std::int32_t) const noexcept -> bool { return false; }
};

// A comparator whose move ctor and assignment are not noexcept
struct throwing_move_ca_comparator {
  throwing_move_ca_comparator() = default;
  throwing_move_ca_comparator(throwing_move_ca_comparator&&) noexcept(false) {}
  auto operator=(throwing_move_ca_comparator&&) noexcept(false) -> throwing_move_ca_comparator& { return *this; }

  throwing_move_ca_comparator(throwing_move_ca_comparator const&) = default;
  auto operator=(throwing_move_ca_comparator const&) noexcept(false) -> throwing_move_ca_comparator& = default;
  ~throwing_move_ca_comparator() = default;

  auto operator()(std::int32_t, std::int32_t) const noexcept -> bool { return false; }
};

// An element whose copy ctor and assignment are not noexcept
struct throwing_copy_ca_element {
  throwing_copy_ca_element() = default;
  // NOLINTBEGIN(hicpp-use-equals-default) testing throwing ctor specification
  throwing_copy_ca_element(throwing_copy_ca_element const&) noexcept(false) {}
  auto operator=(throwing_copy_ca_element const&) noexcept(false) -> throwing_copy_ca_element& { return *this; }
  // NOLINTEND(hicpp-use-equals-default)

  throwing_copy_ca_element(throwing_copy_ca_element&&) = default;
  auto operator=(throwing_copy_ca_element&&) -> throwing_copy_ca_element& = default;
  ~throwing_copy_ca_element() = default;

  auto operator<(throwing_copy_ca_element const&) const noexcept -> bool { return false; }
};

// An element whose move ctor and assignment are not noexcept
struct throwing_move_ca_element {
  throwing_move_ca_element() = default;
  throwing_move_ca_element(throwing_move_ca_element&&) noexcept(false) {}
  auto operator=(throwing_move_ca_element&&) noexcept(false) -> throwing_move_ca_element& { return *this; }

  // Must make copy explicitly throwing as well or else moves turn into copies to avoid the throw
  // NOLINTBEGIN(hicpp-use-equals-default) testing throwing ctor specification
  throwing_move_ca_element(throwing_move_ca_element const&) noexcept(false) {}
  auto operator=(throwing_move_ca_element const&) noexcept(false) -> throwing_move_ca_element& { return *this; }
  // NOLINTEND(hicpp-use-equals-default)
  ~throwing_move_ca_element() = default;

  auto operator<(throwing_move_ca_element const&) const noexcept -> bool { return false; }
};

// An element which is move-only and implicitly convertible to/from int
struct move_only {
  std::int32_t val;
  explicit move_only(std::int32_t that_val)
      : val(that_val) {}
  ~move_only() = default;
  move_only(move_only const&) = delete;
  auto operator=(move_only const&) -> move_only& = delete;
  move_only(move_only&&) = default;
  auto operator=(move_only&&) -> move_only& = delete;

  auto operator<(move_only const& other) const noexcept -> bool { return val < other.val; }
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator decltype(val)() const { return val; }
};

// A move-only comparator that always returns false
struct move_only_cmp {
  ~move_only_cmp() = default;
  move_only_cmp(move_only_cmp&& other) = default;
  auto operator=(move_only_cmp&& other) -> move_only_cmp& = default;

  move_only_cmp(move_only_cmp const&) = delete;
  auto operator=(move_only_cmp const&) -> move_only_cmp& = delete;

  template <typename T>
  auto operator()(T const&, T const&) const noexcept -> bool {
    return false;
  }
};

// An element which is neither copyable nor movable
struct non_movable_non_copyable {
  std::int32_t val;
  std::int32_t payload;

  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  explicit non_movable_non_copyable(std::int32_t value, std::int32_t pld = 0)
      : val(value),
        payload(pld) {}
  ~non_movable_non_copyable() = default;
  non_movable_non_copyable(non_movable_non_copyable&& other) noexcept = delete;
  auto operator=(non_movable_non_copyable&& other) -> non_movable_non_copyable& = delete;
  non_movable_non_copyable(non_movable_non_copyable const&) = delete;
  auto operator=(non_movable_non_copyable const&) -> non_movable_non_copyable& = delete;

  auto operator<(non_movable_non_copyable const& other) const noexcept -> bool { return val < other.val; }
};

// A non-movable, non-copyable comparator which always returns false
struct non_movable_non_copyable_cmp {
  ~non_movable_non_copyable_cmp() = default;
  non_movable_non_copyable_cmp(non_movable_non_copyable_cmp&& other) noexcept = delete;
  auto operator=(non_movable_non_copyable_cmp&& other) -> non_movable_non_copyable_cmp& = delete;
  non_movable_non_copyable_cmp(non_movable_non_copyable_cmp const&) = delete;
  auto operator=(non_movable_non_copyable_cmp const&) -> non_movable_non_copyable_cmp& = delete;

  template <typename T>
  auto operator()(T const&, T const&) const noexcept -> bool {
    return false;
  }
};

template <typename T, std::size_t Size, typename Iterator>
constexpr auto copy_to_array(Iterator begin, Iterator end) noexcept -> arene::base::array<T, Size> {
  ARENE_INVARIANT(arene::base::distance(begin, end) == Size);

  arene::base::array<T, Size> output{};
  arene::base::copy(begin, end, output.begin());
  return output;
}

/// @test A default-constructed `inline_set` is empty
TEST(InlineSet, DefaultConstructedSetIsEmpty) {
  standard_set const set;

  ASSERT_TRUE(set.empty());
}

/// @test `empty` is `noexcept`
TEST(InlineSet, EmptyIsNoExcept) {
  standard_set const set;
  static_assert(noexcept(set.empty()), "Must be noexcept");
}

/// @test A default-constructed `const` `inline_set` is empty
TEST(InlineSet, CanCallEmptyOnConstSet) {
  standard_set const set;

  ASSERT_TRUE(set.empty());
}

/// @test An `inline_set` constructed with a non-empty initializer list is not empty
TEST(InlineSet, CanConstructWithAnInitializerList) {
  standard_set const set{1, 2, 3};

  ASSERT_FALSE(set.empty());
}

/// @test The size of a default-constructed `inline_set` is zero
TEST(InlineSet, TheSizeOfADefaultConstructedSetIsZero) {
  standard_set const set;
  ASSERT_EQ(set.size(), 0);
}

/// @test `size` is `noexcept`
TEST(InlineSet, SizeIsNoexcept) {
  standard_set const set;
  static_assert(noexcept(set.size()), "Must be noexcept");
}

/// @test Given an `inline_set` constructed from an initializer list, the value returned by `size` is the same as the
/// size of the initializer list
TEST(InlineSet, SizeMatchesInitListSize) {
  standard_set const set1{1, 2, 3};
  ASSERT_EQ(set1.size(), 3);
  standard_set const set2{1, 2, 3, 4, 5};
  ASSERT_EQ(set2.size(), 5);
}

/// @test Invoking `contains` for any value on a default-constructed `inline_set` returns `false`.
TEST(InlineSet, DefaultConstructedSetContainsNothing) {
  standard_set const set;

  ASSERT_FALSE(set.contains(1));
  ASSERT_FALSE(set.contains(42));
  ASSERT_FALSE(set.contains(-123));
  ASSERT_FALSE(set.contains(123456));
  ASSERT_FALSE(set.contains(-12345678));
}

/// @test Given an `inline_set` constructed from an initializer list, invoking `contains` for any value in that
/// initializer list returns `true`
TEST(InlineSet, ValuesInInitListAreInSet) {
  std::int32_t const val1 = 10;
  std::int32_t const val2 = 42;
  std::int32_t const val3 = -123;
  standard_set const set1{val1, val2, val3};
  ASSERT_TRUE(set1.contains(val1));
  ASSERT_TRUE(set1.contains(val2));
  ASSERT_TRUE(set1.contains(val3));
}

/// @test Given an `inline_set` constructed from an initializer list, invoking `contains` for any value not in that
/// initializer list returns `false`
TEST(InlineSet, ValuesNotInInitListAreNotInSet) {
  std::int32_t const val1 = 10;
  std::int32_t const val2 = 42;
  std::int32_t const val3 = -123;
  standard_set const set1{val1, val2, val3};
  ASSERT_FALSE(set1.contains(-val1));
  ASSERT_FALSE(set1.contains(-val2));
  ASSERT_FALSE(set1.contains(-val3));
}

/// @test `contains` is `noexcept`
TEST(InlineSet, ContainsIsNoexcept) {
  standard_set const set;
  static_assert(noexcept(set.contains(42)), "Must be noexcept");
}

// A non-primitive type large enough to definitely not be pass-by-value
struct user_type {
  auto operator<(user_type const&) const -> bool { return false; }
  arene::base::array<std::int32_t, 8> padding;
};

/// @test `inline_set` has the appropriate type aliases for an associative container
TEST(InlineSet, TypeAliases) {
  using set_t = standard_capacity_set<user_type>;

  ::testing::StaticAssertTypeEq<set_t::value_type, user_type>();
  ::testing::StaticAssertTypeEq<set_t::key_type, user_type>();
  ::testing::StaticAssertTypeEq<set_t::pointer, user_type*>();
  ::testing::StaticAssertTypeEq<set_t::const_pointer, user_type const*>();
  ::testing::StaticAssertTypeEq<set_t::reference, user_type&>();
  ::testing::StaticAssertTypeEq<set_t::const_reference, user_type const&>();
  ::testing::StaticAssertTypeEq<set_t::size_type, std::size_t>();
  ::testing::StaticAssertTypeEq<set_t::difference_type, std::ptrdiff_t>();
}

/// @test `inline_set::iterator` has the appropriate type aliases for a bidirectional iterator
TEST(InlineSetIterator, TypeAliases) {
  using set_t = standard_capacity_set<user_type>;

  ::testing::
      StaticAssertTypeEq<std::iterator_traits<set_t::iterator>::iterator_category, std::bidirectional_iterator_tag>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<set_t::iterator>::value_type, user_type>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<set_t::iterator>::pointer, user_type const*>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<set_t::iterator>::reference, user_type const&>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<set_t::iterator>::difference_type, std::ptrdiff_t>();

  ::testing::StaticAssertTypeEq<
      std::iterator_traits<set_t::const_iterator>::iterator_category,
      std::bidirectional_iterator_tag>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<set_t::const_iterator>::value_type, user_type>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<set_t::const_iterator>::pointer, user_type const*>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<set_t::const_iterator>::reference, user_type const&>();
  ::testing::StaticAssertTypeEq<std::iterator_traits<set_t::const_iterator>::difference_type, std::ptrdiff_t>();
}

/// @test `inline_set::iterator` has the appropriate operations for a bidirectional iterator
TEST(InlineSetIterator, IteratorOperationTypes) {
  using set_t = standard_set;
  ::testing::StaticAssertTypeEq<decltype(*std::declval<set_t::iterator&>()), set_t::const_reference>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<set_t::iterator&>().operator->()), set_t::const_pointer>();
  ::testing::StaticAssertTypeEq<decltype(++std::declval<set_t::iterator&>()), set_t::iterator&>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<set_t::iterator&>()++), set_t::iterator>();
  ::testing::StaticAssertTypeEq<decltype(--std::declval<set_t::iterator&>()), set_t::iterator&>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<set_t::iterator&>()--), set_t::iterator>();
  static_assert(arene::base::is_less_than_comparable_v<set_t::iterator>, "Less-than comparable");
  static_assert(arene::base::is_less_than_or_equal_comparable_v<set_t::iterator>, "Less-than-or-equal comparable");
  static_assert(arene::base::is_greater_than_comparable_v<set_t::iterator>, "Greater-than comparable");
  static_assert(
      arene::base::is_greater_than_or_equal_comparable_v<set_t::iterator>,
      "Greater-than-or-equal comparable"
  );
  static_assert(arene::base::is_equality_comparable_v<set_t::iterator>, "equality comparable");
  static_assert(arene::base::is_inequality_comparable_v<set_t::iterator>, "inequality comparable");
}

/// @test `begin` returns an `iterator`
TEST(InlineSet, BeginReturnsAnIterator) {
  using set_t = standard_set;
  ::testing::StaticAssertTypeEq<decltype(std::declval<set_t>().begin()), set_t::iterator>();
}

/// @test Given a non-empty `inline_set`, derefencing the iterator returned from `begin` returns a `const` reference to
/// the first element
TEST(InlineSet, DereferencingBeginReturnsReferenceToElement) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  set_t const set{key1};
  auto itr = set.begin();
  ::testing::StaticAssertTypeEq<decltype(*itr), std::int32_t const&>();
  ASSERT_EQ(*itr, key1);
}

/// @test Iterating through the elements of an `inline_set` using the iterator returned from `begin` visits the elements
/// in sorted order
TEST(InlineSet, IterationIsInSortedOrder) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  std::int32_t const key2 = 19;
  std::int32_t const key3 = -123;
  std::int32_t const key4 = 123;
  set_t const set{key1, key2, key3, key4};
  auto itr = set.begin();
  ASSERT_EQ(*itr, key3);
  ++itr;
  ASSERT_EQ(*itr, key2);
  ++itr;
  ASSERT_EQ(*itr, key1);
  ++itr;
  ASSERT_EQ(*itr, key4);
  ++itr;
}

/// @test Invoking `insert` adds a value to an `inline_set`, and returns a pair holding an iterator that refers to the
/// new value, along with a boolean that is `true`. The size of the `inline_set` is increased by one, and `contains`
/// returns `true` when invoked with the new value.
TEST(InlineSet, CanInsertAValue) {
  using set_t = standard_set;

  set_t set{1, 2};

  std::int32_t const val = 42;
  ::testing::StaticAssertTypeEq<decltype(set.insert(val)), std::pair<set_t::iterator, bool>>();

  auto res = set.insert(val);
  ASSERT_TRUE(res.second);
  ASSERT_EQ(*res.first, val);
  ASSERT_EQ(res.first, next(set.begin(), 2));
  ASSERT_EQ(set.size(), 3);
  ASSERT_TRUE(set.contains(val));
}

/// @test Invoking `insert` with a value already in the set returns a pair holding an iterator that refers to the
/// existing element with the same value, along with a boolean that is `false`. The size and contents of the set remain
/// unchanged.
TEST(InlineSet, IfValueAlreadyExistsInsertReturnsIteratorToExisting) {
  using set_t = standard_set;

  set_t set{1, 2};

  std::int32_t const val = *set.begin();

  auto res = set.insert(val);
  ASSERT_FALSE(res.second);
  ASSERT_EQ(*res.first, val);
  ASSERT_EQ(res.first, set.begin());
  ASSERT_EQ(set.size(), 2);
  ASSERT_TRUE(set.contains(val));
}

/// @test After invoking `insert` to add a value to an `inline_set`, the newly inserted value appears in the correct
/// position according to the sort order, when iterating through the set.
TEST(InlineSet, ValueInsertedInSortedPosition) {
  using set_t = standard_set;

  set_t set{10, 20, 30, 40, 50, 60, 70};

  std::int32_t const val = 42;

  auto res = set.insert(val);
  ASSERT_TRUE(res.second);
  ASSERT_EQ(*res.first, val);
  ASSERT_EQ(res.first, next(set.begin(), 4));
  ASSERT_EQ(set.size(), 8);
  ASSERT_TRUE(set.contains(val));

  arene::base::array<std::int32_t, 8> const expected{10, 20, 30, 40, 42, 50, 60, 70};

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
}

/// @test An `inline_set` with an element type that is not default-constructible can be constructed with an initializer
/// list, and the membership of elements can be queried using `contains`.
TEST(InlineSet, HandleNonDefaultConstructibleElements) {
  struct non_default_ctor {
    std::int32_t val;
    explicit non_default_ctor(std::int32_t value)
        : val(value) {}

    auto operator<(non_default_ctor const& other) const noexcept -> bool { return val < other.val; }
  };

  using set_t = standard_capacity_set<non_default_ctor>;

  set_t const set{non_default_ctor{1}, non_default_ctor{2}, non_default_ctor{3}};

  ASSERT_EQ(set.size(), 3);
  ASSERT_TRUE(set.contains(non_default_ctor{1}));
  ASSERT_FALSE(set.contains(non_default_ctor{42}));
}

/// @test An `inline_set` with an element type that is move-only can be default-constructed, and elements can be
/// inserted by invoking `insert` with an rvalue. `contains` can be invoked with lvalue or rvalue arguments to check if
/// an element is in the set.
TEST(InlineSet, CanInsertMoveOnlyElements) {
  using set_t = standard_capacity_set<move_only>;

  set_t set;
  ::testing::StaticAssertTypeEq<decltype(set.insert(move_only{1})), std::pair<set_t::iterator, bool>>();
  auto res = set.insert(move_only{1});
  ASSERT_EQ(res.first, set.begin());
  ASSERT_TRUE(res.second);
  set.insert(move_only{2});
  set.insert(move_only{3});
  res = set.insert(move_only{1});
  ASSERT_EQ(res.first, set.begin());
  ASSERT_FALSE(res.second);

  ASSERT_EQ(set.size(), 3);
  move_only const first_item_to_check{1};
  ASSERT_TRUE(set.contains(first_item_to_check));
  ASSERT_TRUE(set.contains(move_only{2}));
  ASSERT_TRUE(set.contains(move_only{3}));
  ASSERT_FALSE(set.contains(move_only{42}));
}

/// @test An `inline_set` with an element type that is not movable can be default-constructed, and elements can be
/// inserted by invoking `emplace` with appropriate constructor arguments. `contains` can be invoked with lvalue or
/// rvalue arguments to check if an element is in the set.
TEST(InlineSet, CanEmplaceNonMovableElements) {
  using set_t = standard_capacity_set<non_movable_non_copyable>;

  set_t set;
  ::testing::StaticAssertTypeEq<decltype(set.emplace(1)), std::pair<set_t::iterator, bool>>();
  auto const res = set.emplace(1);
  ASSERT_EQ(res.first, set.begin());
  ASSERT_TRUE(res.second);
  set.emplace(2);
  set.emplace(3);

  ASSERT_EQ(set.size(), 3);
  non_movable_non_copyable const first_item_to_check{1};
  ASSERT_TRUE(set.contains(first_item_to_check));
  ASSERT_TRUE(set.contains(non_movable_non_copyable{2}));
  ASSERT_TRUE(set.contains(non_movable_non_copyable{3}));
  ASSERT_FALSE(set.contains(non_movable_non_copyable{42}));
}

/// @test Invoking `emplace` with a value already in the set returns a pair holding an iterator that refers to the
/// existing element with the same value, along with a boolean that is `false`. The size and contents of the set remain
/// unchanged.
TEST(InlineSet, EmplaceReturnsFalseIfValueAlreadyPresent) {
  using set_t = standard_capacity_set<non_movable_non_copyable>;

  set_t set;
  auto res = set.emplace(1);
  ASSERT_EQ(res.first, set.begin());
  ASSERT_TRUE(res.second);
  res = set.emplace(1);
  ASSERT_EQ(res.first, set.begin());
  ASSERT_FALSE(res.second);

  ASSERT_EQ(set.size(), 1);
  ASSERT_TRUE(set.contains(non_movable_non_copyable{1}));
  ASSERT_FALSE(set.contains(non_movable_non_copyable{42}));
}

/// @test `emplace` can be invoked with multiple arguments, which are passed to the constructor of the element being
/// inserted into the `inline_set`.
TEST(InlineSet, CanEmplaceWithAdditionalArguments) {
  using set_t = standard_capacity_set<non_movable_non_copyable>;

  set_t set;
  std::int32_t const key = 1;
  std::int32_t const payload = 42;

  auto res = set.emplace(key, payload);
  ::testing::StaticAssertTypeEq<decltype(set.emplace(key, payload)), std::pair<set_t::iterator, bool>>();
  ASSERT_EQ(res.first, set.begin());
  ASSERT_EQ(res.first->val, key);
  ASSERT_EQ(res.first->payload, payload);
  ASSERT_TRUE(res.second);
  res = set.emplace(key);
  ASSERT_EQ(res.first, set.begin());
  ASSERT_EQ(res.first->val, key);
  ASSERT_EQ(res.first->payload, payload);
  ASSERT_FALSE(res.second);

  ASSERT_EQ(set.size(), 1);
  ASSERT_TRUE(set.contains(non_movable_non_copyable{key}));
}

/// @test Invoking `emplace` with a value already in the set returns a pair holding an iterator that refers to the
/// existing element with the same value, along with a boolean that is `false`, even when the `inline_set` has a size
/// equal to its capacity. The size and contents of the set remain unchanged.
TEST(InlineSet, EmplaceWithExistingElementIntoFullSetReturnsIteratorAndFalse) {
  constexpr std::int32_t capacity = 3;
  using set_t = inline_set<std::int32_t, capacity>;

  set_t set{1, 2, 3};

  auto res = set.emplace(2);
  ASSERT_FALSE(res.second);
  ASSERT_EQ(res.first, next(set.begin()));
  ASSERT_EQ(set.size(), 3);
  auto itr = set.begin();
  ASSERT_EQ(*itr, 1);
  ++itr;
  ASSERT_EQ(*itr, 2);
  ++itr;
  ASSERT_EQ(*itr, 3);
}

class counted {
  std::int32_t key_;
  std::int32_t* count_ = nullptr;

 public:
  counted(std::int32_t key, std::int32_t* count)
      : key_(key),
        count_(count) {
    ++*count_;
  }
  counted(counted const& other)
      : key_(other.key_),
        count_(other.count_) {
    ++*count_;
  }
  counted(counted&& other) noexcept
      : key_(other.key_),
        count_(other.count_) {
    ++*count_;
  }
  auto operator=(counted const& other) -> counted& = delete;
  auto operator=(counted&& other) -> counted& = delete;
  ~counted() { --*count_; }

  auto operator<(counted const& other) const noexcept -> bool { return key_ < other.key_; }
};

/// @test If `emplace` is invoked with arguments that lead to the construction of an element that is already present in
/// the set, then the newly constructed element is destroyed.
TEST(InlineSet, NewElementDestroyedWhenEmplacingIfAlreadyPresent) {
  using set_t = standard_capacity_set<counted>;

  std::int32_t count = 0;

  set_t set{counted{1, &count}};
  ASSERT_EQ(count, 1);
  ASSERT_FALSE(set.emplace(1, &count).second);
  ASSERT_EQ(set.size(), 1);
  ASSERT_EQ(count, 1);
  ASSERT_TRUE(set.emplace(2, &count).second);
  ASSERT_EQ(count, 2);
  ASSERT_EQ(set.size(), 2);
}

/// @test Invoking `begin` on a `const` reference to an `inline_set` returns an iterator which is equal to that returned
/// by invoking `begin` on a non-`const` reference to the same set.
TEST(InlineSet, CanGetBeginFromConstSet) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  // NOLINTNEXTLINE(misc-const-correctness) Explicitly testing const vs non-const behavior
  set_t set{key1};
  set_t const& const_set = set;
  auto itr = set.begin();
  auto citr = const_set.begin();
  ::testing::StaticAssertTypeEq<decltype(const_set.begin()), set_t::const_iterator>();
  static_assert(noexcept(const_set.begin()), "begin must be noexcept");
  ASSERT_EQ(citr, itr);
  ASSERT_EQ(&*citr, &*itr);
}

/// @test Invoking `cbegin` on an `inline_set` returns an iterator which is equal to that returned by invoking `begin`
/// on the same set.
TEST(InlineSet, CanGetCBeginFromSet) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  // NOLINTNEXTLINE(misc-const-correctness) Explicitly testing const vs non-const behavior
  set_t set{key1};
  set_t const& const_set = set;
  auto begin = set.begin();
  auto itr = set.cbegin();
  auto citr = const_set.cbegin();
  ::testing::StaticAssertTypeEq<decltype(set.cbegin()), set_t::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_set.cbegin()), set_t::const_iterator>();
  static_assert(noexcept(set.cbegin()), "begin must be noexcept");
  static_assert(noexcept(const_set.cbegin()), "begin must be noexcept");
  ASSERT_EQ(citr, itr);
  ASSERT_EQ(citr, begin);
  ASSERT_EQ(&*citr, &*begin);
}

/// @test The iterator returned from `begin` is equal to the iterator returned from `end` when they are both invoked on
/// an empty `inline_set`.
TEST(InlineSet, EndEqualsBeginForEmptySet) {
  using set_t = standard_set;

  set_t const set;
  ASSERT_EQ(set.begin(), set.end());
  ::testing::StaticAssertTypeEq<decltype(set.end()), set_t::iterator>();
  static_assert(noexcept(set.end()), "begin must be noexcept");
}

/// @test Invoking `end` on a `const` reference to an `inline_set` returns an iterator which is equal to that returned
/// by invoking `end` on a non-`const` reference to the same set.
TEST(InlineSet, CanGetEndFromConstSet) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  // NOLINTNEXTLINE(misc-const-correctness) Explicitly testing const vs non-const behavior
  set_t set{key1};
  set_t const& const_set = set;
  auto itr = set.end();
  auto citr = const_set.end();
  ::testing::StaticAssertTypeEq<decltype(const_set.end()), set_t::const_iterator>();
  static_assert(noexcept(const_set.end()), "end must be noexcept");
  ASSERT_EQ(citr, itr);
  ASSERT_EQ(itr, next(set.begin(), static_cast<std::ptrdiff_t>(set.size())));
}

/// @test Invoking `cend` on an `inline_set` returns an iterator which is equal to that returned by invoking `end`
/// on the same set.
TEST(InlineSet, CanGetCEndFromSet) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  // NOLINTNEXTLINE(misc-const-correctness) Explicitly testing const vs non-const behavior
  set_t set{key1};
  set_t const& const_set = set;
  auto end = set.end();
  auto itr = set.cend();
  auto citr = const_set.cend();
  ::testing::StaticAssertTypeEq<decltype(set.cend()), set_t::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_set.cend()), set_t::const_iterator>();
  static_assert(noexcept(set.cend()), "end must be noexcept");
  static_assert(noexcept(const_set.cend()), "end must be noexcept");
  ASSERT_EQ(citr, itr);
  ASSERT_EQ(citr, end);
  ASSERT_EQ(itr, next(set.begin(), static_cast<std::ptrdiff_t>(set.size())));
}

/// @test Invoking `erase` with the value of an element in the `inline_set` removes that element from the set, so that
/// the size is descreased by one, and invoking `contains` on that `inline_set` with that value returns `false`. The
/// return value from `erase` is equal to 1.
TEST(InlineSet, CanEraseElementByKey) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  std::int32_t const key2 = 19;
  std::int32_t const key3 = 123;
  std::int32_t const key4 = -123;
  std::int32_t const key5 = -2371632;
  set_t set{key1, key2, key3, key4, key5};

  auto const res = set.erase(key4);
  ::testing::StaticAssertTypeEq<decltype(set.erase(key4)), std::size_t>();
  static_assert(noexcept(set.erase(key4)), "Must be noexcept");
  ASSERT_EQ(res, 1);
  ASSERT_EQ(set.size(), 4);
  ASSERT_FALSE(set.contains(key4));
  ASSERT_TRUE(set.contains(key1));
  ASSERT_TRUE(set.contains(key2));
  ASSERT_TRUE(set.contains(key3));
  ASSERT_TRUE(set.contains(key5));
}

/// @test Invoking `erase` with the value of an element that is not in the `inline_set` returns 0, and leaves the
/// `inline_set` instance unchanged, with the same size and values.
TEST(InlineSet, EraseElementByKeyDoesNothingIfKeyNotPresent) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  std::int32_t const key2 = 19;
  std::int32_t const key3 = 123;
  std::int32_t const key4 = -123;
  std::int32_t const key5 = -2371632;
  std::int32_t const missing = 11;
  set_t set{key1, key2, key3, key4, key5};

  auto const res = set.erase(missing);
  ASSERT_EQ(res, 0);
  ASSERT_EQ(set.size(), 5);
  ASSERT_TRUE(set.contains(key4));
  ASSERT_TRUE(set.contains(key1));
  ASSERT_TRUE(set.contains(key2));
  ASSERT_TRUE(set.contains(key3));
  ASSERT_TRUE(set.contains(key5));
}

/// @test After erasing a value with `erase`, a new element can be inserted via `insert`
TEST(InlineSet, AfterErasingElementCanInsertNewOne) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  std::int32_t const key2 = 19;
  std::int32_t const key3 = 123;
  std::int32_t const key4 = -123;
  std::int32_t const key5 = -2371632;
  std::int32_t const key6 = 11;
  set_t set{key1, key2, key3, key4, key5};

  set.erase(key4);
  set.insert(key6);
  ASSERT_EQ(set.size(), 5);
  ASSERT_FALSE(set.contains(key4));
  ASSERT_TRUE(set.contains(key1));
  ASSERT_TRUE(set.contains(key2));
  ASSERT_TRUE(set.contains(key3));
  ASSERT_TRUE(set.contains(key5));
  ASSERT_TRUE(set.contains(key6));
}

/// @test Invoking `erase` with an iterator referring to an element in the `inline_set` removes that element from the
/// set, so that the size is descreased by one, and invoking `contains` on that `inline_set` with that value returns
/// `false`. The return value from `erase` is equal to an iterator with the same offset from the iterator returned by
/// `begin` as the iterator passed to `erase`.
TEST(InlineSet, CanEraseElementByIterator) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  std::int32_t const key2 = 19;
  std::int32_t const key3 = 123;
  std::int32_t const key4 = -123;
  std::int32_t const key5 = -2371632;
  set_t set{key1, key2, key3, key4, key5};

  auto pos = set.erase(next(set.begin()));
  ::testing::StaticAssertTypeEq<decltype(set.erase(set.begin())), set_t::iterator>();
  static_assert(noexcept(set.erase(set.begin())), "Must be noexcept");
  ASSERT_EQ(pos, next(set.begin()));
  ASSERT_EQ(*pos, key2);
  ASSERT_EQ(set.size(), 4);
  ASSERT_FALSE(set.contains(key4));
  ASSERT_TRUE(set.contains(key1));
  ASSERT_TRUE(set.contains(key2));
  ASSERT_TRUE(set.contains(key3));
  ASSERT_TRUE(set.contains(key5));
}

/// @test Invoking `find` with the value of an element in an `inline_set` returns an iterator referring to that element.
TEST(InlineSet, CanFindElement) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  std::int32_t const key2 = 19;
  std::int32_t const key3 = 123;
  std::int32_t const key4 = -123;
  std::int32_t const key5 = -2371632;
  set_t const set{key1, key2, key3, key4, key5};

  ::testing::StaticAssertTypeEq<decltype(set.find(key1)), set_t::iterator>();
  static_assert(noexcept(set.find(key1)), "Must be noexcept");
  auto pos = set.find(key1);
  ASSERT_EQ(pos, next(set.begin(), 3));
  ASSERT_EQ(*pos, key1);
}

/// @test Invoking `find` with the value of an element not in an `inline_set` returns an iterator equal to that returned
/// from `end` on that set.
TEST(InlineSet, FindReturnsEndIfElementNotPresent) {
  using set_t = standard_set;

  std::int32_t const missing = 11;
  set_t const set{42, 19, 123, -123, -2371632};
  ASSERT_EQ(set.find(missing), set.end());
}

/// @test Invoking `erase` with an iterator pair referring to a range of elements in the `inline_set` removes those
/// elements from the set, so that the size is decreased by the number of elements in the range, and invoking
/// `contains` on that `inline_set` with those values returns `false`. The return value from `erase` is equal to an
/// iterator with the same offset from the iterator returned by `begin` as the iterator passed to `erase` denoting the
/// start of the range.
TEST(InlineSet, CanEraseAnIteratorRange) {
  constexpr std::int32_t capacity = 50;
  using set_t = inline_set<std::int32_t, capacity>;

  constexpr std::int32_t min = -10;
  constexpr std::int32_t max = 10;
  constexpr std::int32_t erase_first = -5;
  constexpr std::int32_t erase_last = 3;
  constexpr auto expected_size = static_cast<std::size_t>((max - min) - (erase_last - erase_first));

  set_t set;
  arene::base::array<std::int32_t, expected_size> expected{};
  auto expected_it = expected.begin();
  for (std::int32_t element = min; element < max; ++element) {
    set.insert(element);
    if (element < erase_first || element >= erase_last) {
      *expected_it = element;
      ++expected_it;
    }
  }

  ::testing::StaticAssertTypeEq<decltype(set.erase(set.find(erase_first), set.find(erase_last))), set_t::iterator>();
  static_assert(noexcept(set.erase(set.begin(), set.begin())), "Must be noexcept");
  auto const res = set.erase(set.find(erase_first), set.find(erase_last));

  ASSERT_EQ(res, set.find(erase_last));

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
}

/// @test Invoking `erase` with an iterator pair referring to a range of elements in the `inline_set` destroys the
/// elements in that range.
TEST(InlineSet, ErasingElementsDestroysElements) {
  using set_t = standard_capacity_set<counted>;

  std::int32_t const min = -10;
  std::int32_t const max = 10;

  std::int32_t count = 0;

  set_t set;
  for (std::int32_t i = min; i < max; ++i) {
    set.emplace(i, &count);
  }

  std::int32_t const erase_first = -5;
  std::int32_t const erase_last = 3;

  auto res = set.erase(set.find(counted(erase_first, &count)), set.find(counted(erase_last, &count)));

  ASSERT_EQ(res, set.find(counted(erase_last, &count)));
  ASSERT_EQ(set.size(), (max - min) - (erase_last - erase_first));
  ASSERT_EQ(static_cast<std::size_t>(count), set.size());
}

/// @test Invoking `count` with a value that is not present in an `inline_set` returns zero.
TEST(InlineSet, CountReturnsZeroForMissingElements) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  std::int32_t const key2 = 19;
  std::int32_t const key3 = 123;
  std::int32_t const key4 = -123;
  std::int32_t const key5 = -2371632;
  std::int32_t const missing = 11;
  set_t const set{key1, key2, key3, key4, key5};

  ::testing::StaticAssertTypeEq<decltype(set.count(missing)), std::size_t>();
  static_assert(noexcept(set.count(missing)), "Noexcept");

  ASSERT_EQ(set.count(missing), 0);
}

/// @test Invoking `count` with a value that is present in an `inline_set` returns one.
TEST(InlineSet, CountReturnsOneForPresentElements) {
  using set_t = standard_set;

  std::int32_t const key1 = 42;
  std::int32_t const key2 = 19;
  std::int32_t const key3 = 123;
  std::int32_t const key4 = -123;
  std::int32_t const key5 = -2371632;
  set_t const set{key1, key2, key3, key4, key5};

  ASSERT_EQ(set.count(key1), 1);
  ASSERT_EQ(set.count(key2), 1);
  ASSERT_EQ(set.count(key3), 1);
  ASSERT_EQ(set.count(key4), 1);
  ASSERT_EQ(set.count(key5), 1);
}

/// @test Invoking `clear` on an inline set sets the size to zero.
TEST(InlineSet, CanClearTheSet) {
  using set_t = standard_set;

  set_t set{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  static_assert(noexcept(set.clear()), "Must be noexcept");
  set.clear();
  ASSERT_EQ(set.size(), 0);
  ASSERT_TRUE(set.empty());
}

/// @test Invoking `insert` with an iterator range inserts the values into the `inline_set`. They appear in correct
/// position in the sorted order visible by iterating over the elements, irrespective of the insertion order.
TEST(InlineSet, CanInsertIteratorRange) {
  using set_t = standard_set;

  std::initializer_list<std::int32_t> const original_values{1, 2, 3, 4, 5};
  std::initializer_list<std::int32_t> const insert_values{-123, 456, -789, 10, 54, -99};

  arene::base::array<std::int32_t, 11> expected{};
  arene::base::copy(original_values.begin(), original_values.end(), expected.begin());
  arene::base::copy(insert_values.begin(), insert_values.end(), arene::base::next(expected.begin(), 5));
  std::sort(expected.begin(), expected.end());

  set_t set{original_values};
  set.insert(insert_values.begin(), insert_values.end());

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
}

/// @test Invoking `insert` with an initializer list inserts the values into the `inline_set`. They appear in correct
/// position in the sorted order visible by iterating over the elements, irrespective of the insertion order.
TEST(InlineSet, CanInsertAnInitializerList) {
  using set_t = standard_set;

  std::initializer_list<std::int32_t> const original_values{1, 2, 3, 4, 5};
  std::initializer_list<std::int32_t> const insert_values{-123, 456, -789, 10, 54, -99};

  arene::base::array<std::int32_t, 11> expected{};
  arene::base::copy(original_values.begin(), original_values.end(), expected.begin());
  arene::base::copy(insert_values.begin(), insert_values.end(), arene::base::next(expected.begin(), 5));
  std::sort(expected.begin(), expected.end());

  set_t set{original_values};
  set.insert(insert_values);

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
}

/// @test Constructing an `inline_set` with an iterator range initializes it with the values in the range. They appear
/// in correct position in the sorted order visible by iterating over the elements, irrespective of the insertion order.
TEST(InlineSet, CanConstructWithIteratorRange) {
  arene::base::array<std::int32_t, 6> const values{-123, 456, -789, 10, 54, -99};

  using set_t = standard_set;

  set_t const set(values.begin(), values.end());

  arene::base::array<std::int32_t, 6> expected = values;
  std::sort(expected.begin(), expected.end());

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
}

/// @test The `inline_set` constructor that accepts an iterator range is not considered for overload resolution if the
/// iterators are output iterators.
TEST(InlineSet, CannotConstructFromOutputIterators) {
  using set_t = standard_set;

  using output_iterator = testing::demoted_iterator<std::int32_t*, std::output_iterator_tag>;
  static_assert(
      !std::is_constructible<set_t, output_iterator, output_iterator>::value,
      "Should not be constructible from output iterators"
  );
}

/// @test The `inline_set` constructor that accepts an iterator range is not considered for overload resolution if the
/// element type of the set cannot be constructed from the value type of the iterator range.
TEST(InlineSet, CannotConstructFromIncompatibleInputIterators) {
  struct unconvertible_type {};

  using set_t = standard_set;
  using input_iterator = testing::demoted_iterator<unconvertible_type*, std::input_iterator_tag>;
  static_assert(
      !std::is_constructible<set_t, input_iterator, input_iterator>::value,
      "Should not be constructible from incompatible input iterators"
  );
}

/// @test The `iterator` type of an `inline_set` is a bidrectional iterator.
TEST(InlineSet, SetIteratorIsBidirectional) {
  using set_t = standard_set;

  static_assert(arene::base::is_bidirectional_iterator_v<set_t::iterator>, "Must be bidirectional");
}

/// @test Specifying the comparator type for the `inline_set` template as a type with a function call operator that
/// returns a `bool` which is `true` to indicate if the left-hand argument should be sorted before the right-hand
/// argument and `false` otherwise, sorts the elements as per the order dictated by an instance of the provided
/// comparator type.
TEST(InlineSet, CanSpecifyComparator) {
  struct reverses {
    auto operator()(std::int32_t lhs, std::int32_t rhs) const noexcept -> bool { return rhs < lhs; }
  };

  using set_t = inline_set<std::int32_t, standard_capacity, reverses>;
  ::testing::StaticAssertTypeEq<set_t::key_compare, reverses>();
  ::testing::StaticAssertTypeEq<set_t::value_compare, reverses>();

  arene::base::array<std::int32_t, 5> const values{1, 2, 3, 4, 5};

  set_t const set{values.begin(), values.end()};

  auto expected = copy_to_array<std::int32_t, 5>(values.begin(), values.end());
  std::sort(expected.begin(), expected.end(), reverses{});

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
}

/// @test Specifying the comparator type for the `inline_set` template as a type with a function call operator that
/// returns a `strong_ordering` which indicates whether the left-hand argument is less than, equal to, or greater than
/// the right-hand argument in the sort order, sorts the elements as per the order dictated by an instance of the
/// provided comparator type.
TEST(InlineSet, CanSpecifyCustomThreeWayComparator) {
  struct comparator {
    auto operator()(std::int32_t lhs, std::int32_t rhs) const noexcept -> strong_ordering {
      std::int32_t delta = rhs - lhs;
      delta = (delta / 10) * 10;
      return make_strong_ordering(delta);
    }
  };

  using set_t = inline_set<std::int32_t, standard_capacity, comparator>;
  ::testing::StaticAssertTypeEq<set_t::key_compare, comparator>();
  ::testing::StaticAssertTypeEq<set_t::value_compare, comparator>();

  arene::base::array<std::int32_t, 11> const values{11, 23, 10, 9, 0, 27, 26, 49, 21, 42, 5};

  set_t const set{values.begin(), values.end()};

  arene::base::array<std::int32_t, 4> const expected{49, 23, 11, 0};

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
}

/// @test If an instance of the comparator type specified as a template argument for `inline_set` is passed to the
/// constructor, along with an iterator range, then a copy of that comparator is used for sorting the elements and
/// stored in the set.
TEST(InlineSet, ComparatorInstanceStoredInSet) {
  struct comparator {
    std::int32_t divisor = 1;

    explicit comparator(std::int32_t that_divisor)
        : divisor(that_divisor) {}

    auto operator()(std::int32_t lhs, std::int32_t rhs) const noexcept -> strong_ordering {
      std::int32_t delta = rhs - lhs;
      delta = (delta / divisor) * divisor;
      return make_strong_ordering(delta);
    }
  };

  using set_t = inline_set<std::int32_t, standard_capacity, comparator>;
  ::testing::StaticAssertTypeEq<set_t::key_compare, comparator>();
  ::testing::StaticAssertTypeEq<set_t::value_compare, comparator>();

  arene::base::array<std::int32_t, 11> const values{11, 23, 10, 9, 0, 27, 26, 49, 21, 42, 5};

  std::int32_t const divisor = 10;
  set_t const set{values.begin(), values.end(), comparator{divisor}};

  arene::base::array<std::int32_t, 4> const expected{49, 23, 11, 0};

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
  ASSERT_EQ(set.key_comp().divisor, divisor);
}

/// @test If an instance of the comparator type specified as a template argument for `inline_set` is passed to the
/// constructor, along with an initializer list, then a copy of that comparator is used for sorting the elements and
/// stored in the set.
TEST(InlineSet, ComparatorInstanceStoredInSetWhenInitializedWithInitList) {
  struct comparator {
    std::int32_t divisor = 1;

    explicit comparator(std::int32_t that_divisor)
        : divisor(that_divisor) {}

    auto operator()(std::int32_t lhs, std::int32_t rhs) const noexcept -> strong_ordering {
      std::int32_t delta = rhs - lhs;
      delta = (delta / divisor) * divisor;
      return make_strong_ordering(delta);
    }
  };

  using set_t = inline_set<std::int32_t, standard_capacity, comparator>;
  ::testing::StaticAssertTypeEq<set_t::key_compare, comparator>();
  ::testing::StaticAssertTypeEq<set_t::value_compare, comparator>();

  std::int32_t const divisor = 10;
  set_t const set{{11, 23, 10, 9, 0, 27, 26, 49, 21, 42, 5}, comparator{divisor}};

  arene::base::array<std::int32_t, 4> const expected{49, 23, 11, 0};

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
  ASSERT_EQ(set.key_comp().divisor, divisor);
}

/// @test `rbegin` and `rend` return `reverse_iterator`s which iterate through the elements of the set in the reverse
/// order to the iterators returned from `begin` and `end`.
TEST(InlineSet, ReverseIterationIsInReverseSortedOrder) {
  using set_t = standard_set;

  set_t const set{42, 19, -123, 123};
  ::testing::StaticAssertTypeEq<set_t::reverse_iterator, ::arene::base::reverse_iterator<set_t::iterator>>();
  ::testing::StaticAssertTypeEq<set_t::reverse_iterator, decltype(set.rbegin())>();
  auto reversed_original_elements = copy_to_array<std::int32_t, 4>(set.begin(), set.end());
  std::reverse(reversed_original_elements.begin(), reversed_original_elements.end());
  auto const reverse_itr_elements = copy_to_array<std::int32_t, 4>(set.rbegin(), set.rend());
  EXPECT_TRUE(arene::base::equal(
      reverse_itr_elements.begin(),
      reverse_itr_elements.end(),
      reversed_original_elements.begin(),
      reversed_original_elements.end()
  ));
}

/// @test `crbegin` and `crend` return `reverse_iterator`s which iterate through the elements of the set in the same
/// order as the iterators returned from `rbegin` and `rend`.
TEST(InlineSet, ConstReverseIteration) {
  using set_t = standard_set;

  set_t const set{42, 19, -123, 123};
  ::testing::StaticAssertTypeEq<set_t::const_reverse_iterator, ::arene::base::reverse_iterator<set_t::const_iterator>>(
  );
  ::testing::StaticAssertTypeEq<set_t::const_reverse_iterator, decltype(set.crbegin())>();
  ::testing::StaticAssertTypeEq<set_t::const_reverse_iterator, decltype(set.crend())>();
  auto const reverse_itr_values = copy_to_array<std::int32_t, 4>(set.crbegin(), set.crend());
  auto const reversed_original_values = copy_to_array<std::int32_t, 4>(set.rbegin(), set.rend());
  EXPECT_TRUE(arene::base::equal(
      reverse_itr_values.begin(),
      reverse_itr_values.end(),
      reversed_original_values.begin(),
      reversed_original_values.end()
  ));
}

/// @test If an `inline_set` is constructed with a comparator object, then a copy of that object can be retrieved via
/// the `key_comp` member function or `value_comp` member function.
TEST(InlineSet, CanGetStoredComparator) {
  struct comparator {
    std::int32_t divisor = 1;

    explicit comparator(std::int32_t that_divisor)
        : divisor(that_divisor) {}

    auto operator()(std::int32_t lhs, std::int32_t rhs) const noexcept -> strong_ordering {
      std::int32_t delta = rhs - lhs;
      delta = (delta / divisor) * divisor;
      return make_strong_ordering(delta);
    }
  };

  using set_t = inline_set<std::int32_t, standard_capacity, comparator>;

  constexpr std::int32_t divisor = 42;
  set_t const set{comparator{divisor}};

  ::testing::StaticAssertTypeEq<comparator, decltype(set.key_comp())>();
  static_assert(noexcept(set.key_comp()), "Must be noexcept");
  ::testing::StaticAssertTypeEq<comparator, decltype(set.value_comp())>();
  static_assert(noexcept(set.value_comp()), "Must be noexcept");
  ASSERT_EQ(set.key_comp().divisor, divisor);
  ASSERT_EQ(set.value_comp().divisor, divisor);
}

/// @test `max_size` returns the capacity of the `inline_set`.
TEST(InlineSet, MaxSizeIsCapacity) {
  std::int32_t const capacity1 = 50;
  using set1_t = inline_set<std::int32_t, capacity1>;
  std::int32_t const capacity2 = 10;
  using set2_t = inline_set<std::int32_t, capacity2>;

  set1_t const set1;
  set2_t const set2;
  ::testing::StaticAssertTypeEq<std::size_t, decltype(set1_t::max_size())>();
  static_assert(noexcept(set1_t::max_size()), "Noexcept");
  ASSERT_EQ(set1.max_size(), capacity1);
  ASSERT_EQ(set2.max_size(), capacity2);
}

/// @test `capacity` returns the capacity of the `inline_set`.
TEST(InlineSet, CapacityIsCapacity) {
  STATIC_ASSERT_EQ((inline_set<std::int32_t, 10>::capacity), 10);
  STATIC_ASSERT_EQ((inline_set<std::int32_t, 10>::capacity()), 10);
}

/// @test `lower_bound` returns an iterator referring to the element if the value passed is in the set.
TEST(InlineSet, LowerBoundForAnExistingElement) {
  using set_t = standard_set;

  std::int32_t const search_value = 2;
  set_t const set{1, search_value, 3, 4, 5};
  ::testing::StaticAssertTypeEq<set_t::iterator, decltype(set.lower_bound(search_value))>();
  static_assert(noexcept(set.lower_bound(search_value)), "Must be noexcept");

  auto pos = set.lower_bound(search_value);
  ASSERT_EQ(pos, next(set.begin()));
  ASSERT_EQ(*pos, search_value);
}

/// @test `lower_bound` returns an iterator referring to the first element that is greater than the supplied value if
/// the value is not in the set
TEST(InlineSet, LowerBoundForAMissingElement) {
  using set_t = standard_set;

  std::int32_t const search_value = 3;
  set_t const set{1, 2, 4, 5};
  auto const pos = set.lower_bound(search_value);
  ASSERT_EQ(pos, next(set.begin(), 2));
}

/// @test `upper_bound` returns an iterator referring to the element after the element with the specified value, if the
/// value passed is in the set.
TEST(InlineSet, UpperBoundForAnExistingElement) {
  using set_t = standard_set;

  std::int32_t const search_value = 2;
  std::int32_t const next_value = 13;
  set_t const set{1, search_value, next_value, 14, 15};
  ::testing::StaticAssertTypeEq<set_t::iterator, decltype(set.upper_bound(search_value))>();
  static_assert(noexcept(set.upper_bound(search_value)), "Must be noexcept");

  auto pos = set.upper_bound(search_value);
  ASSERT_EQ(pos, next(set.find(search_value)));
  ASSERT_EQ(*pos, next_value);
}

/// @test `upper_bound` returns an iterator referring to the first element that is greater than the supplied value if
/// the value is not in the set
TEST(InlineSet, UpperBoundForAMissingElement) {
  using set_t = standard_set;

  std::int32_t const search_value = 4;
  set_t const set{1, 2, 3, 5};
  auto const pos = set.upper_bound(search_value);
  ASSERT_EQ(pos, next(set.begin(), 3));
}

/// @test If `equal_range` is invoked with a value that is in the set, then it returns a pair of iterators, where the
/// first is an iterator that refers to the element with the specified value, and the second is the result of
/// incrementing that iterator by one.
TEST(InlineSet, EqualRangeForAnExistingElement) {
  using set_t = standard_set;

  std::int32_t const search_value = 2;
  set_t const set{1, search_value, 3, 4, 5};
  ::testing::StaticAssertTypeEq<std::pair<set_t::iterator, set_t::iterator>, decltype(set.equal_range(search_value))>();
  static_assert(noexcept(set.equal_range(search_value)), "Must be noexcept");

  auto pos = set.equal_range(search_value);
  ASSERT_EQ(pos.first, next(set.begin()));
  ASSERT_EQ(*pos.first, search_value);
  ASSERT_EQ(pos.second, next(set.begin(), 2));
}

/// @test If `equal_range` is invoked with a value that is not in the set, then it returns a pair of iterators, both of
/// which are equal to the iterator returned from `end` if there is no such value.
TEST(InlineSet, EqualRangeForAMissingElement) {
  using set_t = standard_set;

  std::int32_t const search_value = 3;
  set_t const set{1, 2, 4, 5};
  auto const pos = set.equal_range(search_value);
  ASSERT_EQ(pos.first, set.end());
  ASSERT_EQ(pos.second, set.end());
}

/// @test Move-construction of an `inline_set` leaves the source set empty.
TEST(InlineSet, MoveConstructionLeavesSourceEmpty) {
  using set_t = standard_set;

  set_t set{10, 20, 30, 40, 50, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 7>(set.begin(), set.end());

  set_t const set2(std::move(set));

  // NOLINTNEXTLINE(bugprone-use-after-move) Explicitly testing post-move constructor
  ASSERT_EQ(set.size(), 0);

  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test Move construction of an `inline_set` with a move-only element type transfers the values to the new set,
/// leaving the source set empty.
TEST(InlineSet, CanMoveSetOfMoveOnlyElements) {
  using set_t = standard_set;

  arene::base::array<std::int32_t, 8> const expected{3, 10, 14, 20, 42, 56, 78, 1203};

  set_t set;
  for (auto value : expected) {
    set.insert(move_only(value));
  }
  ASSERT_EQ(set.size(), expected.size());

  set_t const set2(std::move(set));

  // NOLINTNEXTLINE(bugprone-use-after-move) Explicitly testing post-move condition
  ASSERT_EQ(set.size(), 0);

  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));

  static_assert(std::is_move_constructible<set_t>::value, "Must be move-constructible");
  static_assert(std::is_nothrow_move_constructible<set_t>::value, "Must be move-constructible");
}

/// @test `inline_set` is not move-constructible if the element type is not move-constructible.
TEST(InlineSet, NotMoveConstructibleIfElementNotMoveConstructible) {
  using set_t = standard_capacity_set<non_movable_non_copyable>;
  static_assert(!std::is_move_constructible<set_t>::value, "Not move-constructible");
}

/// @test `inline_set` is not move-constructible if the comparator is not move-constructible.
TEST(InlineSet, NotMoveConstructibleIfComparatorNotMoveConstructible) {
  using set_t = inline_set<std::int32_t, standard_capacity, non_movable_non_copyable_cmp>;
  static_assert(!std::is_move_constructible<set_t>::value, "Not move-constructible");
}

/// @test `inline_set` is not move-assignable if the element type is not move-constructible.
TEST(InlineSet, NotMoveAssignableIfElementNotMoveConstructible) {
  using set_t = standard_capacity_set<non_movable_non_copyable>;
  static_assert(!std::is_move_assignable<set_t>::value, "Not move-assignable");
}

/// @test `inline_set` is not move-assignable if the comparator is not move-assignable.
TEST(InlineSet, NotMoveAssignableIfComparatorNotMoveAssignable) {
  using set_t = inline_set<std::int32_t, standard_capacity, non_movable_non_copyable_cmp>;
  static_assert(!std::is_move_assignable<set_t>::value, "Not move-assignable");
}

/// @test An `inline_set` with a move-only element type can be move-assigned to another instance, which moves all the
/// elements to the destination set and leaves the source set empty.
TEST(InlineSet, CanMoveAssignSetOfMoveOnlyElements) {
  using set_t = standard_set;

  arene::base::array<std::int32_t, 8> const expected{3, 10, 14, 20, 42, 56, 78, 1203};

  set_t set;
  for (auto element : expected) {
    set.insert(move_only(element));
  }
  ASSERT_EQ(set.size(), expected.size());

  set_t set2;
  // put in some values that shouldn't be there after the move
  set2.insert(move_only{999});
  set2.insert(move_only{99999});
  set2.insert(move_only{999999});

  set2 = std::move(set);

  // NOLINTNEXTLINE(bugprone-use-after-move) Explicitly testing post-move condition
  ASSERT_EQ(set.size(), 0);

  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));

  static_assert(std::is_move_assignable<set_t>::value, "Must be move-assignable");
  static_assert(std::is_nothrow_move_assignable<set_t>::value, "Must be noexcept");
}

/// @test Move construction of an `inline_set` can throw if the move construction of the element type can throw.
TEST(InlineSet, MoveConstructorCanThrowIfElementMoveConstructorThrows) {
  using set_t = standard_capacity_set<throwing_move_ca_element>;
  static_assert(std::is_move_constructible<set_t>::value, "Must be move constructible");
  static_assert(!std::is_nothrow_move_constructible<set_t>::value, "Must not be nothrow move constructible");
}

/// @test Move assignment of an `inline_set` can throw if the move construction of the element type can throw.
TEST(InlineSet, MoveAssignmentCanThrowIfElementMoveConstructorThrows) {
  using set_t = standard_capacity_set<throwing_move_ca_element>;
  static_assert(std::is_move_assignable<set_t>::value, "Must be move assignable");
  static_assert(!std::is_nothrow_move_assignable<set_t>::value, "Must not be nothrow move assignable");
}

/// @test Move construction of an `inline_set` can throw if the move construction of the comparator can throw.
TEST(InlineSet, MoveAssignmentCanThrowIfComparatorThrows) {
  using set_t = inline_set<std::int32_t, standard_capacity, throwing_move_ca_comparator>;
  static_assert(std::is_move_assignable<set_t>::value, "Must be move assignable");
  static_assert(!std::is_nothrow_move_assignable<set_t>::value, "Must not be nothrow move assignable");
}

/// @test The copy constructor of `inline_set` copies the values from the source set into the new set, and leaves the
/// source set unchanged, so the two sets have the same size and values.
TEST(InlineSet, CanCopyConstructLeavingSourceUnchanged) {
  using set_t = standard_set;
  using larger_set_t = inline_set<set_t::key_type, set_t::capacity + 1, set_t::key_compare>;

  set_t const set{30, 20, 30, 40, 80, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 6>(set.begin(), set.end());

  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization) explicitly testing copy behavior
  set_t const set2(set);
  static_assert(std::is_nothrow_copy_constructible<set_t>::value, "Must be noexcept");

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));

  larger_set_t const from_smaller{set};
  EXPECT_TRUE(arene::base::equal(from_smaller.begin(), from_smaller.end(), expected.begin(), expected.end()));
  set_t const from_larger{from_smaller};
  EXPECT_TRUE(arene::base::equal(from_larger.begin(), from_larger.end(), expected.begin(), expected.end()));
}

/// @test The copy assignment operator of `inline_set` copies the values from the source set into the destination set,
/// and leaves the source set unchanged, so the two sets have the same size and values.
TEST(InlineSet, CanCopyAssignLeavingSourceUnchanged) {
  using set_t = standard_set;

  set_t const set{30, 20, 30, 40, 80, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 6>(set.begin(), set.end());

  set_t set2{1, 2, 3, 4};
  set2 = set;
  static_assert(std::is_nothrow_copy_assignable<set_t>::value, "Must be noexcept");

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test Invoking the copy assignment operator of `inline_set` with a set with a larger capacity, but a size that is
/// less than or equal to the capacity of the destination set copies the values from the source set into the destination
/// set, and leaves the source set unchanged, so the two sets have the same size and values.
TEST(InlineSet, CanCopyAssignFromLargerCapacitySetLeavingSourceUnchanged) {
  using set_t = standard_set;
  using larger_set_t = inline_set<set_t::key_type, set_t::capacity + 1, set_t::key_compare>;
  STATIC_ASSERT_TRUE(std::is_nothrow_assignable<set_t&, larger_set_t const&>::value);

  larger_set_t const larger_set{10, 20, 30, 40, 50, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 7>(larger_set.begin(), larger_set.end());

  set_t set{1, 2, 3, 4};
  set = larger_set;

  EXPECT_TRUE(arene::base::equal(larger_set.begin(), larger_set.end(), expected.begin(), expected.end()));
  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
}

/// @test Invoking the copy assignment operator of `inline_set` with a set with a smaller capacity than the destination
/// set copies the values from the source set into the destination set, and leaves the source set unchanged, so the two
/// sets have the same size and values.
TEST(InlineSet, CanCopyAssignFromSmallerCapacitySetLeavingSourceUnchanged) {
  using set_t = standard_set;
  using smaller_set_t = inline_set<set_t::key_type, set_t::capacity - 1, set_t::key_compare>;
  STATIC_ASSERT_TRUE(std::is_nothrow_assignable<set_t&, smaller_set_t const&>::value);

  smaller_set_t const smaller_set{30, 10, 30, 40, 80, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 6>(smaller_set.begin(), smaller_set.end());

  set_t set{1, 2, 3, 4};
  set = smaller_set;

  EXPECT_TRUE(arene::base::equal(smaller_set.begin(), smaller_set.end(), expected.begin(), expected.end()));
  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
}

/// @test The copy assignment operator of `inline_set` works even if the element type is not assignable; elements are
/// copy-constructed, so the two sets have the same size and values.
TEST(InlineSet, CanCopyAssignNonAssignableElementLeavingSourceUnchanged) {
  struct non_assignable {
    std::int32_t val;

    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    non_assignable(std::int32_t that_val) noexcept
        : val(that_val) {}

    non_assignable(non_assignable const&) = default;
    auto operator=(non_assignable const&) -> non_assignable& = delete;
    non_assignable(non_assignable&&) = default;
    auto operator=(non_assignable&&) -> non_assignable& = delete;
    ~non_assignable() = default;

    auto operator<(non_assignable const& other) const noexcept -> bool { return val < other.val; }
    // NOLINTNEXTLINE(hicpp-explicit-conversions) need implicit conversion for comparisons
    operator decltype(val)() const { return val; }
  };

  using set_t = standard_capacity_set<non_assignable>;

  set_t const set{30, 20, 30, 40, 80, 60, 70};

  arene::base::array<non_assignable, 6> const expected{20, 30, 40, 60, 70, 80};

  set_t set2{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  set2 = set;
  static_assert(std::is_nothrow_copy_assignable<set_t>::value, "Must be noexcept");

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test The `inline_set` copy-assignment operator can throw if the copy assignment of the comparator can throw
TEST(InlineSet, CopyAssignmentCanThrowIfComparatorThrows) {
  using set_t = inline_set<std::int32_t, standard_capacity, throwing_copy_ca_comparator>;
  static_assert(std::is_copy_assignable<set_t>::value, "Must be copy assignable");
  static_assert(!std::is_nothrow_copy_assignable<set_t>::value, "Must not be nothrow copy assignable");
}

/// @test The `inline_set` copy-assignment operator can throw if the copy constructor of the element type can throw
TEST(InlineSet, CopyAssignmentCanThrowIfElementCopyConstructorThrows) {
  using set_t = standard_capacity_set<throwing_copy_ca_element>;
  static_assert(std::is_copy_assignable<set_t>::value, "Must be copy assignable");
  static_assert(!std::is_nothrow_copy_assignable<set_t>::value, "Must not be nothrow copy assignable");
}

/// @test Move-construction of an `inline_set` from a set with a larger capacity, but a size less than or equal to the
/// capacity of the destination leaves the source set empty, and the destination has the same size and contents as the
/// source had originally.
TEST(InlineSet, MoveConstructionFromLargerCapacityLeavesSourceEmpty) {
  constexpr std::int32_t larger_capacity = 20;
  using set1_t = inline_set<std::int32_t, larger_capacity>;
  constexpr std::int32_t smaller_capacity = 10;
  using set2_t = inline_set<std::int32_t, smaller_capacity>;

  set1_t set{30, 20, 30, 40, 80, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 6>(set.begin(), set.end());

  set2_t const set2 = std::move(set);

  // NOLINTNEXTLINE(bugprone-use-after-move) Explicitly testing post-move condition
  ASSERT_EQ(set.size(), 0);

  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test Move-construction of an `inline_set` with a move-only element type from a set with a larger capacity, but a
/// size less than or equal to the capacity of the destination leaves the source set empty, and the destination has the
/// same size and contents as the source had originally.
TEST(InlineSet, CanMoveSetOfMoveOnlyElementsWithLargerCapacity) {
  constexpr std::int32_t larger_capacity = 20;
  using set1_t = inline_set<move_only, larger_capacity>;
  constexpr std::int32_t smaller_capacity = 10;
  using set2_t = inline_set<move_only, smaller_capacity>;

  arene::base::array<std::int32_t, 8> const expected{3, 10, 14, 20, 42, 56, 78, 1203};

  set1_t set;
  for (auto element : expected) {
    set.insert(move_only(element));
  }
  ASSERT_EQ(set.size(), expected.size());

  set2_t const set2(std::move(set));

  // NOLINTNEXTLINE(bugprone-use-after-move) explicitly testing post-move condition
  ASSERT_EQ(set.size(), 0);

  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test Move-construction of an `inline_set` from a set with a smaller capacity than the destination leaves the source
/// set empty, and the destination has the same size and contents as the source had originally.
TEST(InlineSet, MoveConstructionFromSmallerCapacityLeavesSourceEmpty) {
  constexpr std::int32_t smaller_capacity = 20;
  using set1_t = inline_set<std::int32_t, smaller_capacity>;
  constexpr std::int32_t larger_capacity = 30;
  using set2_t = inline_set<std::int32_t, larger_capacity>;

  set1_t set{10, 20, 30, 40, 50, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 7>(set.begin(), set.end());

  set2_t const set2 = std::move(set);
  static_assert(std::is_nothrow_constructible<set2_t, set1_t&&>::value, "Must not throw");

  // NOLINTNEXTLINE(bugprone-use-after-move) Explicitly testing post-move condition
  ASSERT_EQ(set.size(), 0);

  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test Move-construction of an `inline_set` from a set with a smaller capacity than the destination can throw if the
/// move constructor of the element type can throw.
TEST(InlineSet, MoveConstructorFromSmallerCanThrowIfElementMoveConstructorThrows) {
  constexpr std::int32_t capacity1 = 5;
  using set1_t = inline_set<throwing_move_ca_element, capacity1>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<throwing_move_ca_element, capacity2>;
  static_assert(std::is_constructible<set2_t, set1_t&&>::value, "Must be move constructible");
  static_assert(!std::is_nothrow_constructible<set2_t, set1_t&&>::value, "Must not be nothrow move constructible");
}

/// @test Move-construction of an `inline_set` from a set with a smaller capacity than the destination can throw if the
/// move constructor of the comparator can throw.
TEST(InlineSet, MoveConstructorFromSmallerCanThrowIfComparatorMoveConstructorThrows) {
  constexpr std::int32_t capacity1 = 5;
  using set1_t = inline_set<std::int32_t, capacity1, throwing_move_ca_comparator>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<std::int32_t, capacity2, throwing_move_ca_comparator>;
  static_assert(std::is_constructible<set2_t, set1_t&&>::value, "Must be move constructible");
  static_assert(!std::is_nothrow_constructible<set2_t, set1_t&&>::value, "Must not be nothrow move constructible");
}

/// @test `inline_set` is not move-constructible if the element type is not move-constructible
TEST(InlineSet, CannotMoveConstructIfElementNotMoveConstructible) {
  constexpr std::int32_t capacity1 = 20;
  using set1_t = inline_set<non_movable_non_copyable, capacity1>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<non_movable_non_copyable, capacity2>;

  static_assert(!std::is_constructible<set1_t, set2_t&&>::value, "Must not be constructible");
  static_assert(!std::is_constructible<set2_t, set1_t&&>::value, "Must not be constructible");
}

/// @test The copy constructor of `inline_set` from a set with a larger capacity, but a size less than or equal to the
/// capacity of the destination, copies the values from the source set into the new set, and leaves the source set
/// unchanged, so the two sets have the same size and values.
TEST(InlineSet, CopyConstructionFromLargerCapacityLeavesSourceUnchanged) {
  constexpr std::int32_t capacity1 = 20;
  using set1_t = inline_set<std::int32_t, capacity1>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<std::int32_t, capacity2>;

  set1_t const set{10, 20, 30, 40, 50, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 7>(set.begin(), set.end());

  set2_t const set2 = set;

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test `inline_set` is not constructible from a `const` source with a larger capacity if the element type is not
/// copy-constructible
TEST(InlineSet, CannotCopySetOfMoveOnlyElementsWithLargerCapacity) {
  constexpr std::int32_t capacity1 = 20;
  using set1_t = inline_set<move_only, capacity1>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<move_only, capacity2>;

  static_assert(!std::is_constructible<set2_t, set1_t const&>::value, "Should not be constructible");
}

/// @test The copy constructor of `inline_set` from a set with a smaller capacity than the destination, copies the
/// values from the source set into the new set, and leaves the source set unchanged, so the two sets have the same size
/// and values.
TEST(InlineSet, CopyConstructionFromSmallerCapacityLeavesSourceUnchanged) {
  constexpr std::int32_t capacity1 = 20;
  using set1_t = inline_set<std::int32_t, capacity1>;
  constexpr std::int32_t capacity2 = 30;
  using set2_t = inline_set<std::int32_t, capacity2>;

  set1_t const set{10, 20, 30, 40, 50, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 7>(set.begin(), set.end());

  set2_t const set2 = set;

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test Copy construction of an `inline_set` from a source set with a smaller capacity is `noexcept` if the element
/// copy constructor is `noexcept`.
TEST(InlineSet, CopyConstructionDoesNotThrowIfSourceHasFewerManyElements) {
  constexpr std::int32_t capacity1 = 20;
  using set1_t = inline_set<std::int32_t, capacity1>;
  constexpr std::int32_t capacity2 = 30;
  using set2_t = inline_set<std::int32_t, capacity2>;

  static_assert(std::is_nothrow_constructible<set2_t, set1_t const&>::value, "Must not throw");
}

/// @test `inline_set` is not constructible from a `const` source with a smaller capacity if the element type is not
/// copy-constructible
TEST(InlineSet, CannotCopySetOfMoveOnlyElementsWithSmallerCapacity) {
  constexpr std::int32_t capacity1 = 20;
  using set1_t = inline_set<move_only, capacity1>;
  constexpr std::int32_t capacity2 = 30;
  using set2_t = inline_set<move_only, capacity2>;

  static_assert(!std::is_constructible<set2_t, set1_t const&>::value, "Should not be constructible");
}

/// @test Copy construction of an `inline_set` from a source set with a smaller capacity is not `noexcept` if the
/// element copy constructor is not `noexcept`.
TEST(InlineSet, CopyConstructorFromSmallerCanThrowIfComparatorCopyConstructorThrows) {
  constexpr std::int32_t capacity1 = 5;
  using set1_t = inline_set<std::int32_t, capacity1, throwing_copy_ca_comparator>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<std::int32_t, capacity2, throwing_copy_ca_comparator>;
  static_assert(std::is_constructible<set2_t, set1_t const&>::value, "Must be copy constructible");
  static_assert(!std::is_nothrow_constructible<set2_t, set1_t const&>::value, "Must not be nothrow copy constructible");
}

/// @test Copy assignment of an `inline_set` from a source set with a smaller capacity is not `noexcept` if the
/// comparator copy constructor is not `noexcept`.
TEST(InlineSet, CopyAssignmentFromSmallerCapacityCanThrowIfComparatorThrows) {
  constexpr std::int32_t capacity1 = 5;
  using set1_t = inline_set<std::int32_t, capacity1, throwing_copy_ca_comparator>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<std::int32_t, capacity2, throwing_copy_ca_comparator>;
  static_assert(std::is_assignable<set2_t, set1_t const&>::value, "Must be copy assignable");
  static_assert(!std::is_nothrow_assignable<set2_t, set1_t const&>::value, "Must not be nothrow copy assignable");
}

/// @test Copy assignment of an `inline_set` from a source set with a smaller capacity is not `noexcept` if the
/// element copy constructor is not `noexcept`.
TEST(InlineSet, CopyAssignmentFromSmallerCapacityCanThrowIfElementCopyConstructorThrows) {
  constexpr std::int32_t capacity1 = 5;
  using set1_t = inline_set<throwing_copy_ca_element, capacity1>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<throwing_copy_ca_element, capacity2>;
  static_assert(std::is_assignable<set2_t, set1_t const&>::value, "Must be copy assignable");
  static_assert(!std::is_nothrow_assignable<set2_t, set1_t const&>::value, "Must not be nothrow copy assignable");
}

/// @test Invoking the copy assignment operator of `inline_set` with a set with a larger capacity than the destination
/// set but a smaller size than the destination capacity, copies the values from the source set into the destination
/// set, and leaves the source set unchanged, so the two sets have the same size and values.
TEST(InlineSet, CopyAssignmentFromLargerCapacityLeavesSourceUnchanged) {
  constexpr std::int32_t capacity1 = 20;
  using set1_t = inline_set<std::int32_t, capacity1>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<std::int32_t, capacity2>;

  set1_t const set{10, 20, 30, 40, 50, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 7>(set.begin(), set.end());

  set2_t set2;
  set2 = set;

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test Invoking the copy assignment operator of `inline_set` with a set with a smaller capacity than the destination
/// set copies the values from the source set into the destination set, and leaves the source set unchanged, so the two
/// sets have the same size and values.
TEST(InlineSet, CopyAssignmentFromSmallerCapacityLeavesSourceUnchanged) {
  constexpr std::int32_t capacity1 = 10;
  using set1_t = inline_set<std::int32_t, capacity1>;
  constexpr std::int32_t capacity2 = 20;
  using set2_t = inline_set<std::int32_t, capacity2>;

  set1_t const set{10, 20, 30, 40, 50, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 7>(set.begin(), set.end());

  set2_t set2;
  set2 = set;
  static_assert(noexcept(set2 = set), "Must not throw");

  EXPECT_TRUE(arene::base::equal(set.begin(), set.end(), expected.begin(), expected.end()));
  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test Move-assignment of an `inline_set` from a set with smaller capacity can throw if the comparator's move
/// assignment can throw
TEST(InlineSet, MoveAssignmentFromSmallerCapacityCanThrowIfComparatorThrows) {
  constexpr std::int32_t capacity1 = 5;
  using set1_t = inline_set<std::int32_t, capacity1, throwing_move_ca_comparator>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<std::int32_t, capacity2, throwing_move_ca_comparator>;
  static_assert(std::is_assignable<set2_t, set1_t&&>::value, "Must be move assignable");
  static_assert(!std::is_nothrow_assignable<set2_t, set1_t&&>::value, "Must not be nothrow move assignable");
}

/// @test Move-assignment of an `inline_set` from a set with smaller capacity can throw if the element type's move
/// constructor can throw
TEST(InlineSet, MoveAssignmentFromSmallerCapacityCanThrowIfElementMoveConstructorThrows) {
  constexpr std::int32_t capacity1 = 5;
  using set1_t = inline_set<throwing_move_ca_element, capacity1>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<throwing_move_ca_element, capacity2>;
  static_assert(std::is_assignable<set2_t, set1_t&&>::value, "Must be move assignable");
  static_assert(!std::is_nothrow_assignable<set2_t, set1_t&&>::value, "Must not be nothrow move assignable");
}

/// @test Move-assignment of an `inline_set` from a set with a larger capacity, but a size less than or equal to the
/// capacity of the destination leaves the source set empty, and the destination has the same size and contents as the
/// source had originally.
TEST(InlineSet, MoveAssignmentFromLargerCapacityLeavesSourceEmpty) {
  constexpr std::int32_t capacity1 = 20;
  using set1_t = inline_set<std::int32_t, capacity1>;
  constexpr std::int32_t capacity2 = 10;
  using set2_t = inline_set<std::int32_t, capacity2>;

  set1_t set{10, 20, 30, 40, 50, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 7>(set.begin(), set.end());

  set2_t set2;
  set2 = std::move(set);

  // NOLINTNEXTLINE(bugprone-use-after-move) explicitly asserting post-move condition.
  EXPECT_EQ(set.size(), 0);

  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test Move-assignment of an `inline_set` from a set with a smaller capacity than the destination leaves the source
/// set empty, and the destination has the same size and contents as the source had originally.
TEST(InlineSet, MoveAssignmentFromSmallerCapacityLeavesSourceEmpty) {
  constexpr std::int32_t capacity1 = 10;
  using set1_t = inline_set<std::int32_t, capacity1>;
  constexpr std::int32_t capacity2 = 20;
  using set2_t = inline_set<std::int32_t, capacity2>;

  set1_t set{10, 20, 30, 40, 50, 60, 70};

  auto const expected = copy_to_array<std::int32_t, 7>(set.begin(), set.end());

  set2_t set2;
  set2 = std::move(set);
  static_assert(noexcept(set2 = std::move(set)), "Must not throw");

  // NOLINTNEXTLINE(bugprone-use-after-move) explicitly asserting post-move condition.
  EXPECT_EQ(set.size(), 0);

  EXPECT_TRUE(arene::base::equal(set2.begin(), set2.end(), expected.begin(), expected.end()));
}

/// @test Invoking `try_construct` with an initializer list with more elements than the capacity of the destination type
/// returns an empty `optional`
TEST(InlineSetTryConstructFromInitializerList, ReturnsNulloptIfSizeGreaterThanCapacity) {
  using set_t = inline_set<std::int32_t, 10U>;
  EXPECT_EQ(set_t::try_construct({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}), nullopt);
  EXPECT_EQ(set_t::try_construct({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, compare_three_way{}), nullopt);
}

/// @test Invoking `try_construct` with an initializer list with a number of elements that is less than or equal to the
/// capacity of the destination type returns an `optional` containing an `inline_set` constructed from that initializaer
/// list
TEST(InlineSetTryConstructFromInitializerList, EquivalentToCtorIfSizeLessThanCapacity) {
  using set_t = inline_set<std::int32_t, 10U>;
  SCOPED_TRACE("Without explicit comparator");
  auto const maybe_set = set_t::try_construct({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  ASSERT_TRUE(maybe_set);

  set_t const expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  EXPECT_TRUE(arene::base::equal(maybe_set->begin(), maybe_set->end(), expected.begin(), expected.end()));
}

/// @test Invoking `try_construct` with an initializer list with a number of elements that is less than or equal to the
/// capacity of the destination type and a comparator returns an `optional` containing an `inline_set` constructed from
/// that initializer list and comparator
TEST(InlineSetTryConstructFromInitializerList, InitListAndComparatorEquivalentToCtorIfSizeLessThanCapacity) {
  using set_t = inline_set<std::int32_t, 10U>;
  SCOPED_TRACE("With explicit comparator");
  auto const maybe_set = set_t::try_construct({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, compare_three_way{});
  ASSERT_TRUE(maybe_set);

  set_t const expected{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, compare_three_way{}};
  EXPECT_TRUE(arene::base::equal(maybe_set->begin(), maybe_set->end(), expected.begin(), expected.end()));
}

template <typename T>
class InlineSetTryConstructFromIteratorsTest : public ::testing::Test {};

using iterable_containers = ::testing::Types<
    arene::base::array<std::int32_t, standard_capacity + 1U>,
    arene::base::array<std::int32_t, 3U * (standard_capacity + 1U)>>;

TYPED_TEST_SUITE(InlineSetTryConstructFromIteratorsTest, iterable_containers, );

/// @test Calling `try_construct` to construct an `inline_set` from an iterator range with more elements than the
/// specified capacity of the `inline_set` returns `nullopt`.
///
/// @tparam TypeParam The element type for the `inline_set`
TYPED_TEST(InlineSetTryConstructFromIteratorsTest, ReturnsNullOptIfNumberOfUniqueElementsExceedsCapacity) {
  using set_t = inline_set<std::int32_t, standard_capacity>;
  static_assert(TypeParam::size() > set_t::capacity(), "For this to fail, we need more capacity than the set");

  // Populate the input array with capacity() + 1U unique values (potentially several copies of each)
  TypeParam container{};
  for (std::size_t idx = 0U; idx < container.size(); ++idx) {
    container[idx] = static_cast<typename TypeParam::value_type>(idx % (set_t::capacity() + 1U));
  }

  EXPECT_EQ(set_t::try_construct(container.begin(), container.end()), nullopt);
  EXPECT_EQ(set_t::try_construct(container.begin(), container.end(), compare_three_way{}), nullopt);
}

/// @test Calling `try_construct` to construct an `inline_set` from an iterator range with a number of elements that is
/// the same as or less than the capacity of the destination set returns an `optional` holding an `inline_set`
/// with the same contents as if constructed with the arguments to `try_construct`.
///
/// @tparam TypeParam The element type for the `inline_set`
TYPED_TEST(InlineSetTryConstructFromIteratorsTest, EquivalentToCtorIfNumberOfUniqueElementsFitsInCapacity) {
  using set_t = inline_set<std::int32_t, standard_capacity>;

  // Populate the input array with capacity() unique values (potentially several copies of each)
  TypeParam container{};
  for (std::size_t idx = 0U; idx < container.size(); ++idx) {
    container[idx] = static_cast<typename TypeParam::value_type>(idx % (set_t::capacity()));
  }

  {
    SCOPED_TRACE("Without explicit comparator");
    auto const maybe_set = set_t::try_construct(container.begin(), container.end());
    ASSERT_TRUE(maybe_set);
    set_t const expected{container.begin(), container.end()};
    EXPECT_TRUE(arene::base::equal(maybe_set->begin(), maybe_set->end(), expected.begin(), expected.end()));
  }
  {
    SCOPED_TRACE("With explicit comparator");
    auto const maybe_set = set_t::try_construct(container.begin(), container.end(), compare_three_way{});
    ASSERT_TRUE(maybe_set);
    set_t const expected{container.begin(), container.end(), compare_three_way{}};
    EXPECT_TRUE(arene::base::equal(maybe_set->begin(), maybe_set->end(), expected.begin(), expected.end()));
  }
}

/// @test Calling `try_construct` to construct an `inline_set` from an `inline_set` with a larger capacity and with more
/// elements than the specified capacity of the `inline_set` returns `nullopt`.
///
/// @tparam TypeParam The element type for the `inline_set`
TEST(InlineSetTryConstructFromInlineSet, ReturnsNulloptIfSizeGreaterThanCapacity) {
  using set_t = inline_set<std::int32_t, 10U>;
  using bigger_set_t = inline_set<std::int32_t, 11U>;
  EXPECT_EQ(set_t::try_construct(bigger_set_t{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}), nullopt);
}

/// @test Calling `try_construct` to construct an `inline_set` from another `inline_set` with a number of elements that
/// is the same as or less than the capacity of the destination set returns an `optional` holding an `inline_set` with
/// the same contents as if constructed with the arguments to `try_construct`.
///
/// @tparam TypeParam The element type for the `inline_set`
TEST(InlineSetTryConstructFromInlineSet, EquivalentToCtorIfSizeFitsInCapacity) {
  using set_t = inline_set<std::int32_t, 10U>;
  using smaller_set_t = inline_set<std::int32_t, set_t::capacity() - 1U>;
  using bigger_set_t = inline_set<std::int32_t, set_t::capacity() + 1U>;
  {
    SCOPED_TRACE("Copy");
    {
      SCOPED_TRACE("From Lesser Capacity Set");
      auto const smaller_set = smaller_set_t{1, 2, 3, 4, 5, 6, 7, 8, 9};
      auto const maybe_set = set_t::try_construct(smaller_set);
      ASSERT_TRUE(maybe_set);
      EXPECT_TRUE(arene::base::equal(maybe_set->begin(), maybe_set->end(), smaller_set.begin(), smaller_set.end()));
    }
    {
      SCOPED_TRACE("From Equal Capacity Set");
      auto const equal_set = set_t{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      auto const maybe_set = set_t::try_construct(equal_set);
      ASSERT_TRUE(maybe_set);
      EXPECT_TRUE(arene::base::equal(maybe_set->begin(), maybe_set->end(), equal_set.begin(), equal_set.end()));
    }
    {
      SCOPED_TRACE("From Greater Capacity Set");
      auto const greater_set = bigger_set_t{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      auto const maybe_set = set_t::try_construct(greater_set);
      ASSERT_TRUE(maybe_set);
      EXPECT_TRUE(arene::base::equal(maybe_set->begin(), maybe_set->end(), greater_set.begin(), greater_set.end()));
    }
  }
  {
    SCOPED_TRACE("Move");
    {
      SCOPED_TRACE("From Lesser Capacity Set");
      auto const maybe_set = set_t::try_construct(smaller_set_t{1, 2, 3, 4, 5, 6, 7, 8, 9});
      ASSERT_TRUE(maybe_set);
      set_t const expected{1, 2, 3, 4, 5, 6, 7, 8, 9};
      EXPECT_TRUE(arene::base::equal(maybe_set->begin(), maybe_set->end(), expected.begin(), expected.end()));
    }
    {
      SCOPED_TRACE("From Equal Capacity Set");
      auto const maybe_set = set_t::try_construct(set_t{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
      ASSERT_TRUE(maybe_set);
      set_t const expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      EXPECT_TRUE(arene::base::equal(maybe_set->begin(), maybe_set->end(), expected.begin(), expected.end()));
    }
    {
      SCOPED_TRACE("From Greater Capacity Set");
      auto const maybe_set = set_t::try_construct(bigger_set_t{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
      ASSERT_TRUE(maybe_set);
      set_t const expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      EXPECT_TRUE(arene::base::equal(maybe_set->begin(), maybe_set->end(), expected.begin(), expected.end()));
    }
  }
}

/// if we allow explicit conversion to the key-type or not.
enum class explicit_conversion_to_key { allowed, not_allowed };

/// @brief A helper type which is comparable to a key, but not implicitly convertible to it.
/// @tparam KeyType the key type to use
template <typename KeyType>
class comparable_not_convertible_to_key_impl {
 public:
  // the value of the transparent key
  KeyType value;
  explicit_conversion_to_key conversion_allowed{explicit_conversion_to_key::not_allowed};

  // compares a transparent key and an actual key
  static constexpr auto
  three_way_compare(comparable_not_convertible_to_key_impl const& lhs, KeyType const& rhs) noexcept -> strong_ordering {
    return compare_three_way{}(lhs.value, rhs);
  }

  // _explicitly_ converts a transparent key to its key.
  explicit operator KeyType() const {
    if (conversion_allowed == explicit_conversion_to_key::allowed) {
      return value;
    }
    ARENE_INVARIANT_UNREACHABLE("Invalid conversions are not part of this test");
  }
};

///  @brief Helper fixture base class that initializes a set with values and provides some stock keys to use.
template <typename SetType>
class InlineSetTransparentComparisonEquivalencyBase {
 public:
  // The container type to use for equivalence testing
  using iset = SetType;
  // A key which is less than all keys in the container
  typename iset::key_type const key_less_than_container{0};
  // A key which is between keys in the container
  typename iset::key_type const key_gap_in_container{10};
  // A key which is in the container
  typename iset::key_type const key_in_container = key_gap_in_container + 1;
  // A key which is greater than all keys in the container
  typename iset::key_type const key_greater_than_container{std::numeric_limits<typename iset::key_type>::max()};
  // The container,
  iset values;

  InlineSetTransparentComparisonEquivalencyBase()
      : values([this]() {
          std::remove_const_t<iset> result;
          for (typename iset::key_type key = key_less_than_container + 1; key <= key_in_container + 1; ++key) {
            if (key != key_gap_in_container) {
              result.insert(key);
            }
          }
          return result;
        }()) {}
};

/// @brief specialization for string keys from a non-const set.
template <std::size_t Capacity, typename Compare>
class InlineSetTransparentComparisonEquivalencyBase<
    inline_set<arene::base::inline_string<standard_capacity>, Capacity, Compare>> {
 public:
  // The container type to use for equivalence testing
  using iset = inline_set<arene::base::inline_string<standard_capacity>, Capacity, Compare>;
  // A key which is less than all keys in the container
  typename iset::key_type const key_less_than_container;
  // A key which is between keys in the container
  typename iset::key_type const key_gap_in_container{"d"};
  // A key which is in the container
  typename iset::key_type const key_in_container{"e"};
  // A key which is greater than all keys in the container
  typename iset::key_type const key_greater_than_container{"z"};
  // The container,
  iset values{"a", "b", "c", "e", "f"};
};

/// @brief specialization for string keys from a const set.
template <std::size_t Capacity, typename Compare>
class InlineSetTransparentComparisonEquivalencyBase<
    inline_set<arene::base::inline_string<standard_capacity>, Capacity, Compare> const> {
 public:
  // The container type to use for equivalence testing
  using iset = inline_set<arene::base::inline_string<standard_capacity>, Capacity, Compare> const;
  // A key which is less than all keys in the container
  typename iset::key_type const key_less_than_container;
  // A key which is between keys in the container
  typename iset::key_type const key_gap_in_container{"d"};
  // A key which is in the container
  typename iset::key_type const key_in_container{"e"};
  // A key which is greater than all keys in the container
  typename iset::key_type const key_greater_than_container{"z"};
  // The container,
  iset values{"a", "b", "c", "e", "f"};
};

template <typename T>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance) safe, and required as you ultimately _must_ inherit from testing::Test.
class InlineSetTransparentComparisonEquivalencyTest
    : public InlineSetTransparentComparisonEquivalencyBase<T>
    , public ::testing::Test {
 public:
  using iset = typename InlineSetTransparentComparisonEquivalencyBase<T>::iset;
  using comparable_not_convertible_to_key = comparable_not_convertible_to_key_impl<typename iset::key_type>;
};

using const_and_non_const_transparent_isets = ::testing::Types<
    inline_set<std::uint32_t, 20>,
    inline_set<std::uint32_t, 20> const,
    inline_set<arene::base::inline_string<standard_capacity>, 20>,
    inline_set<arene::base::inline_string<standard_capacity>, 20> const>;

TYPED_TEST_SUITE(InlineSetTransparentComparisonEquivalencyTest, const_and_non_const_transparent_isets, );

/// @test Invoking `find` with a value that is comparable with but not convertible to the element type of an
/// `inline_set` yields the same result as invoking `find` with a value of the element type that has same sort ordering
///
/// @tparam TypeParam The element type for the `inline_set`
TYPED_TEST(InlineSetTransparentComparisonEquivalencyTest, Find) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.find(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.find(std::declval<typename TestFixture::iset::key_type>()))>();
  EXPECT_EQ(
      this->values.find(typename TestFixture::comparable_not_convertible_to_key{this->key_less_than_container}),
      this->values.find(this->key_less_than_container)
  );
  EXPECT_EQ(
      this->values.find(typename TestFixture::comparable_not_convertible_to_key{this->key_greater_than_container}),
      this->values.find(this->key_greater_than_container)
  );
  EXPECT_EQ(
      this->values.find(typename TestFixture::comparable_not_convertible_to_key{this->key_gap_in_container}),
      this->values.find(this->key_gap_in_container)
  );
  EXPECT_EQ(
      this->values.find(typename TestFixture::comparable_not_convertible_to_key{this->key_in_container}),
      this->values.find(this->key_in_container)
  );
}

/// @test Invoking `contains` with a value that is comparable with but not convertible to the element type of an
/// `inline_set` yields the same result as invoking `contains` with a value of the element type that has same sort
/// ordering
///
/// @tparam TypeParam The element type for the `inline_set`
TYPED_TEST(InlineSetTransparentComparisonEquivalencyTest, Contains) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.contains(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.contains(std::declval<typename TestFixture::iset::key_type>()))>();
  EXPECT_EQ(
      this->values.contains(typename TestFixture::comparable_not_convertible_to_key{this->key_less_than_container}),
      this->values.contains(this->key_less_than_container)
  );
  EXPECT_EQ(
      this->values.contains(typename TestFixture::comparable_not_convertible_to_key{this->key_greater_than_container}),
      this->values.contains(this->key_greater_than_container)
  );
  EXPECT_EQ(
      this->values.contains(typename TestFixture::comparable_not_convertible_to_key{this->key_gap_in_container}),
      this->values.contains(this->key_gap_in_container)
  );
  EXPECT_EQ(
      this->values.contains(typename TestFixture::comparable_not_convertible_to_key{this->key_in_container}),
      this->values.contains(this->key_in_container)
  );
}

/// @test Invoking `count` with a value that is comparable with but not convertible to the element type of an
/// `inline_set` yields the same result as invoking `count` with a value of the element type that has same sort ordering
///
/// @tparam TypeParam The element type for the `inline_set`
TYPED_TEST(InlineSetTransparentComparisonEquivalencyTest, Count) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.count(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.count(std::declval<typename TestFixture::iset::key_type>()))>();
  EXPECT_EQ(
      this->values.count(typename TestFixture::comparable_not_convertible_to_key{this->key_less_than_container}),
      this->values.count(this->key_less_than_container)
  );
  EXPECT_EQ(
      this->values.count(typename TestFixture::comparable_not_convertible_to_key{this->key_greater_than_container}),
      this->values.count(this->key_greater_than_container)
  );
  EXPECT_EQ(
      this->values.count(typename TestFixture::comparable_not_convertible_to_key{this->key_gap_in_container}),
      this->values.count(this->key_gap_in_container)
  );
  EXPECT_EQ(
      this->values.count(typename TestFixture::comparable_not_convertible_to_key{this->key_in_container}),
      this->values.count(this->key_in_container)
  );
}

/// @test Invoking `lower_bound` with a value that is comparable with but not convertible to the element type of an
/// `inline_set` yields the same result as invoking `lower_bound` with a value of the element type that has same sort
/// ordering
///
/// @tparam TypeParam The element type for the `inline_set`
TYPED_TEST(InlineSetTransparentComparisonEquivalencyTest, LowerBound) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.lower_bound(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.lower_bound(std::declval<typename TestFixture::iset::key_type>()))>();
  EXPECT_EQ(
      this->values.lower_bound(typename TestFixture::comparable_not_convertible_to_key{this->key_less_than_container}),
      this->values.lower_bound(this->key_less_than_container)
  );
  EXPECT_EQ(
      this->values.lower_bound(typename TestFixture::comparable_not_convertible_to_key{this->key_greater_than_container}
      ),
      this->values.lower_bound(this->key_greater_than_container)
  );
  EXPECT_EQ(
      this->values.lower_bound(typename TestFixture::comparable_not_convertible_to_key{this->key_gap_in_container}),
      this->values.lower_bound(this->key_gap_in_container)
  );
  EXPECT_EQ(
      this->values.lower_bound(typename TestFixture::comparable_not_convertible_to_key{this->key_in_container}),
      this->values.lower_bound(this->key_in_container)
  );
}

/// @test Invoking `upper_bound` with a value that is comparable with but not convertible to the element type of an
/// `inline_set` yields the same result as invoking `upper_bound` with a value of the element type that has same sort
/// ordering
///
/// @tparam TypeParam The element type for the `inline_set`
TYPED_TEST(InlineSetTransparentComparisonEquivalencyTest, UpperBound) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.upper_bound(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.upper_bound(std::declval<typename TestFixture::iset::key_type>()))>();
  EXPECT_EQ(
      this->values.upper_bound(typename TestFixture::comparable_not_convertible_to_key{this->key_less_than_container}),
      this->values.upper_bound(this->key_less_than_container)
  );
  EXPECT_EQ(
      this->values.upper_bound(typename TestFixture::comparable_not_convertible_to_key{this->key_greater_than_container}
      ),
      this->values.upper_bound(this->key_greater_than_container)
  );
  EXPECT_EQ(
      this->values.upper_bound(typename TestFixture::comparable_not_convertible_to_key{this->key_gap_in_container}),
      this->values.upper_bound(this->key_gap_in_container)
  );
  EXPECT_EQ(
      this->values.upper_bound(typename TestFixture::comparable_not_convertible_to_key{this->key_in_container}),
      this->values.upper_bound(this->key_in_container)
  );
}

/// @test Invoking `equal_range` with a value that is comparable with but not convertible to the element type of an
/// `inline_set` yields the same result as invoking `equal_range` with a value of the element type that has same sort
/// ordering
///
/// @tparam TypeParam The element type for the `inline_set`
TYPED_TEST(InlineSetTransparentComparisonEquivalencyTest, EqualRange) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.equal_range(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.equal_range(std::declval<typename TestFixture::iset::key_type>()))>();
  EXPECT_EQ(
      this->values.equal_range(typename TestFixture::comparable_not_convertible_to_key{this->key_less_than_container}),
      this->values.equal_range(this->key_less_than_container)
  );
  EXPECT_EQ(
      this->values.equal_range(typename TestFixture::comparable_not_convertible_to_key{this->key_greater_than_container}
      ),
      this->values.equal_range(this->key_greater_than_container)
  );
  EXPECT_EQ(
      this->values.equal_range(typename TestFixture::comparable_not_convertible_to_key{this->key_gap_in_container}),
      this->values.equal_range(this->key_gap_in_container)
  );
  EXPECT_EQ(
      this->values.equal_range(typename TestFixture::comparable_not_convertible_to_key{this->key_in_container}),
      this->values.equal_range(this->key_in_container)
  );
}

// This fixture needs to be declared (and instantiated) as a typed test because minitest doesn't have TEST_F.
template <typename T>
class InlineSetTransparentComparisonMutableEquivalencyTest : public InlineSetTransparentComparisonEquivalencyTest<T> {};

using non_const_transparent_isets =
    ::testing::Types<inline_set<arene::base::inline_string<standard_capacity>, 10, compare_three_way>>;

TYPED_TEST_SUITE(InlineSetTransparentComparisonMutableEquivalencyTest, non_const_transparent_isets, );

/// @test Invoking `insert` with a value that is comparable with and convertible to the element type of an
/// `inline_set` yields the same result as invoking `insert` with a value of the element type constructed from that
/// value
///
/// @tparam TypeParam The element type for the `inline_set`
TYPED_TEST(InlineSetTransparentComparisonMutableEquivalencyTest, Insert) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.insert(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.insert(std::declval<typename TestFixture::iset::key_type>()))>();
  {
    SCOPED_TRACE("key not in container");
    auto const original_size = this->values.size();
    std::size_t insert_count{0U};
    for (auto const& inserted_key :
         {this->key_less_than_container, this->key_greater_than_container, this->key_gap_in_container}) {
      auto const insert_result = this->values.insert(
          typename TestFixture::comparable_not_convertible_to_key{inserted_key, explicit_conversion_to_key::allowed}
      );
      EXPECT_TRUE(insert_result.second);
      EXPECT_EQ(*insert_result.first, inserted_key);
      EXPECT_EQ(this->values.size(), original_size + (++insert_count));
    }
  }

  {
    SCOPED_TRACE("key  in container");
    auto const original_size = this->values.size();
    auto const insert_result = this->values.insert(typename TestFixture::comparable_not_convertible_to_key{
        this->key_in_container,
        explicit_conversion_to_key::not_allowed
    });
    EXPECT_FALSE(insert_result.second);
    EXPECT_EQ(*insert_result.first, this->key_in_container);
    EXPECT_EQ(this->values.size(), original_size);
  }
}

/// @test Invoking `erase` with a value that is comparable with but not convertible to the element type of an
/// `inline_set` yields the same result as invoking `erase` with a value of the element type that has same sort ordering
///
/// @tparam TypeParam The element type for the `inline_set`
TYPED_TEST(InlineSetTransparentComparisonMutableEquivalencyTest, Erase) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.erase(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.erase(std::declval<typename TestFixture::iset::key_type>()))>();
  auto const original_size = this->values.size();
  {
    SCOPED_TRACE("key not in container");
    for (auto const& erased_key :
         {this->key_less_than_container, this->key_greater_than_container, this->key_gap_in_container}) {
      EXPECT_EQ(
          this->values.erase(typename TestFixture::comparable_not_convertible_to_key{erased_key}),
          this->values.erase(erased_key)
      );
      EXPECT_EQ(this->values.size(), original_size);
    }
  }

  {
    SCOPED_TRACE("key in container");
    EXPECT_EQ(this->values.erase(typename TestFixture::comparable_not_convertible_to_key{this->key_in_container}), 1);
    EXPECT_EQ(this->values.size(), original_size - 1);
  }
}

template <typename T>
class InlineSetTransparentComparisonSignedUnsignedIntegerInsertionTest : public ::testing::Test {
 public:
  using iset = inline_set<T, 10, compare_three_way>;
  iset set;
};

using unsigned_integer_types = arene::base::type_lists::remove_duplicates_t<
    ::testing::Types<unsigned char, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>>;

TYPED_TEST_SUITE(InlineSetTransparentComparisonSignedUnsignedIntegerInsertionTest, unsigned_integer_types, );

/// @test Invoking `insert` for an `inline_set` with an unsigned integral element type, with a positive signed
/// value that is in range as the element correctly inserts the value, however triggers a compilation error when
/// `-Wconversion` or `-Wsignedconversion` is enabled, so an explicit cast is needed.
/// `-Wconversion` will not trigger for implicit conversions if the compiler can prove the value is non-negative and
/// non-narrowing. It cannot do that for non-const values.
TYPED_TEST(
    InlineSetTransparentComparisonSignedUnsignedIntegerInsertionTest,
    InsertingASignedValueWhichIsNotStaticallyProvablyPositiveRequiresCasting
) {
  // NOLINTBEGIN(misc-const-correctness)
  char charval = 1;
  std::int8_t i8val = 2;
  std::int16_t i16val = 3;
  std::int32_t i32val = 4;
  std::int64_t i64val = 5;
  // NOLINTEND(misc-const-correctness)
  //  uncommenting these should be a compiler error when compiled with -Wconversion or -WsignedConversion
  //  this->set.insert(charval);
  //  this->set.insert(i8val);
  //  this->set.insert(i16val);
  //  this->set.insert(i32val);
  //  this->set.insert(i64val);
  //  this->set.insert(-1);
  using key_type = typename TestFixture::iset::value_type;
  EXPECT_TRUE(this->set.insert(static_cast<key_type>(charval)).second);
  EXPECT_TRUE(this->set.insert(static_cast<key_type>(i8val)).second);
  EXPECT_TRUE(this->set.insert(static_cast<key_type>(i16val)).second);
  EXPECT_TRUE(this->set.insert(static_cast<key_type>(i32val)).second);
  EXPECT_TRUE(this->set.insert(static_cast<key_type>(i64val)).second);
}

/// @test Invoking `insert` for an `inline_set` with an unsigned integral element type, with a positive signed
/// value that is in range as the element correctly inserts the value. This is OK, even with `-Wconversion` enabled, if
/// the argument is `const`.
/// `-Wconversion` will not trigger for implicit conversions if the compiler can prove the value is non-negative and
/// non-narrowing. It can do that for const locals, and literals.
TYPED_TEST(
    InlineSetTransparentComparisonSignedUnsignedIntegerInsertionTest,
    InsertingASignedValueWhichIsStaticallyProvablyPositiveDoesNotRequireCasting
) {
  char const charval{1};
  std::int8_t const i8val{2};
  std::int16_t const i16val{3};
  std::int32_t const i32val{4};
  std::int64_t const i64val{5};

  EXPECT_TRUE(this->set.insert(charval).second);
  EXPECT_TRUE(this->set.insert(i8val).second);
  EXPECT_TRUE(this->set.insert(i16val).second);
  EXPECT_TRUE(this->set.insert(i32val).second);
  EXPECT_TRUE(this->set.insert(i64val).second);

  EXPECT_TRUE(this->set.insert(6).second);
  EXPECT_TRUE(this->set.insert(7L).second);
  EXPECT_TRUE(this->set.insert(8LL).second);
}

template <typename KeyType>
class InlineSetComparisonTest : public ::testing::Test {
 public:
  template <std::size_t Capacity>
  using iset = inline_set<KeyType, Capacity>;
  using standard_iset = iset<3>;
  using large_iset = iset<20>;

  standard_iset const s_empty{};
  large_iset const l_empty{};
};

using comparison_key_types = ::testing::Types<std::uint32_t, std::int32_t>;

TYPED_TEST_SUITE(InlineSetComparisonTest, comparison_key_types, );

/// @test Sets with different sizes compare unequal, even if the initial values are identical, and with a mix of
/// capacities.
TYPED_TEST(InlineSetComparisonTest, SetsOfDifferentSizesCompareUnequal) {
  typename TestFixture::standard_iset const s_values{TypeParam{2}, TypeParam{1}};
  typename TestFixture::standard_iset const s_other_values{TypeParam{1}};

  typename TestFixture::large_iset const l_values{s_values};
  typename TestFixture::large_iset const l_other_values{s_other_values};

  EXPECT_NE(this->s_empty, s_values);
  EXPECT_NE(this->l_empty, l_values);
  EXPECT_NE(this->s_empty, l_values);
  EXPECT_NE(this->l_empty, s_values);
  EXPECT_NE(s_other_values, s_values);
  EXPECT_NE(l_other_values, l_values);
  EXPECT_NE(l_other_values, s_values);
  EXPECT_NE(s_other_values, l_values);

  EXPECT_FALSE(this->s_empty == s_values);
  EXPECT_FALSE(this->l_empty == l_values);
  EXPECT_FALSE(this->s_empty == l_values);
  EXPECT_FALSE(this->l_empty == s_values);
  EXPECT_FALSE(s_other_values == s_values);
  EXPECT_FALSE(l_other_values == l_values);
  EXPECT_FALSE(l_other_values == s_values);
  EXPECT_FALSE(s_other_values == l_values);
}

/// @test Sets with the same size compare unequal, if the values are different, and with a mix of capacities.
TYPED_TEST(InlineSetComparisonTest, SetsOfSameSizesButDifferentElementsCompareUnequal) {
  typename TestFixture::standard_iset const s_values{TypeParam{2}, TypeParam{1}};
  typename TestFixture::standard_iset const s_changed_key{TypeParam{3}, TypeParam{2}};

  typename TestFixture::large_iset const l_values{s_values};
  typename TestFixture::large_iset const l_changed_key{s_changed_key};

  EXPECT_NE(s_values, s_changed_key);
  EXPECT_NE(l_values, l_changed_key);
  EXPECT_NE(s_values, l_changed_key);
  EXPECT_NE(l_values, s_changed_key);

  EXPECT_FALSE(s_values == s_changed_key);
  EXPECT_FALSE(l_values == l_changed_key);
  EXPECT_FALSE(s_values == l_changed_key);
  EXPECT_FALSE(l_values == s_changed_key);
}

/// @test Sets with the same size and values compare equal, even with a mix of capacities.
TYPED_TEST(InlineSetComparisonTest, SetsOfSameElementsCompareEqual) {
  typename TestFixture::standard_iset const s_values{TypeParam{2}, TypeParam{1}};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization) explicitly want a copy here
  typename TestFixture::standard_iset const s_same_values{s_values};

  typename TestFixture::large_iset const l_values{s_values};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization) explicitly want a copy here
  typename TestFixture::large_iset const l_same_values{l_values};

  EXPECT_EQ(this->s_empty, this->s_empty);
  EXPECT_EQ(s_values, s_same_values);
  EXPECT_EQ(this->l_empty, this->l_empty);
  EXPECT_EQ(l_values, l_same_values);
  EXPECT_EQ(this->s_empty, this->l_empty);
  EXPECT_EQ(s_values, l_same_values);
  EXPECT_EQ(this->l_empty, this->s_empty);
  EXPECT_EQ(l_values, s_same_values);

  EXPECT_FALSE(this->s_empty != this->s_empty);
  EXPECT_FALSE(s_values != s_same_values);
  EXPECT_FALSE(this->l_empty != this->l_empty);
  EXPECT_FALSE(l_values != l_same_values);
  EXPECT_FALSE(this->s_empty != this->l_empty);
  EXPECT_FALSE(s_values != l_same_values);
  EXPECT_FALSE(this->l_empty != this->s_empty);
  EXPECT_FALSE(l_values != s_same_values);
}

/// @test Sets with different elements are lexicographically ordered based on the contents, ignoring the capacities.
TYPED_TEST(InlineSetComparisonTest, RelationalComparisonsAreLexicographicalOrderings) {
  typename TestFixture::standard_iset const s_values{TypeParam{20}, TypeParam{10}};
  typename TestFixture::standard_iset const s_lesser_value{TypeParam{20}, TypeParam{1}};
  typename TestFixture::standard_iset const s_greater_value{TypeParam{200}, TypeParam{10}};
  typename TestFixture::standard_iset s_more_elements{s_values};
  s_more_elements.insert(*(--s_more_elements.end()) + 1);
  typename TestFixture::standard_iset s_fewer_elements{s_values};
  s_fewer_elements.erase(--s_fewer_elements.end());

  typename TestFixture::large_iset const l_values{s_values};
  typename TestFixture::large_iset const l_same_values{s_values};
  typename TestFixture::large_iset const l_lesser_value{TypeParam{20}, TypeParam{1}};
  typename TestFixture::large_iset const l_greater_value{TypeParam{200}, TypeParam{10}};
  typename TestFixture::large_iset l_more_elements{l_values};
  l_more_elements.insert(*(--l_more_elements.end()) + 1);
  typename TestFixture::large_iset l_fewer_elements{l_values};
  l_fewer_elements.erase(--l_fewer_elements.end());

  ASSERT_LT(s_values, s_greater_value);
  ASSERT_LT(s_lesser_value, s_values);
  ASSERT_LT(s_values, s_more_elements);
  ASSERT_LT(s_fewer_elements, s_values);

  ASSERT_LE(s_values, s_greater_value);
  ASSERT_LE(s_lesser_value, s_values);
  ASSERT_LE(s_values, s_more_elements);
  ASSERT_LE(s_fewer_elements, s_values);
  ASSERT_LE(s_values, s_values);

  ASSERT_GT(s_greater_value, s_values);
  ASSERT_GT(s_values, s_lesser_value);
  ASSERT_GT(s_more_elements, s_values);
  ASSERT_GT(s_values, s_fewer_elements);

  ASSERT_GE(s_greater_value, s_values);
  ASSERT_GE(s_values, s_lesser_value);
  ASSERT_GE(s_more_elements, s_values);
  ASSERT_GE(s_values, s_fewer_elements);
  ASSERT_GE(s_values, s_values);

  ASSERT_LT(s_values, l_greater_value);
  ASSERT_LT(s_lesser_value, l_values);
  ASSERT_LT(s_values, l_more_elements);
  ASSERT_LT(s_fewer_elements, l_values);

  ASSERT_LE(s_values, l_greater_value);
  ASSERT_LE(s_lesser_value, l_values);
  ASSERT_LE(s_values, l_more_elements);
  ASSERT_LE(s_fewer_elements, l_values);
  ASSERT_LE(s_values, l_values);

  ASSERT_GT(s_greater_value, l_values);
  ASSERT_GT(s_values, l_lesser_value);
  ASSERT_GT(s_more_elements, l_values);
  ASSERT_GT(s_values, l_fewer_elements);

  ASSERT_GE(s_greater_value, l_values);
  ASSERT_GE(s_values, l_lesser_value);
  ASSERT_GE(s_more_elements, l_values);
  ASSERT_GE(s_values, l_fewer_elements);
  ASSERT_GE(s_values, l_values);
}

}  // namespace
