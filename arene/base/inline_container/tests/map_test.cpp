// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/map.hpp"

#include <gtest/gtest.h>

#include "arene/base/compare.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/endian.hpp"
#include "arene/base/inline_container/tests/map_test_helpers.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/iterator.hpp"
#include "arene/base/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/reverse.hpp"
#include "arene/base/stdlib_choice/sort.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "arene/base/type_traits.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/has_broken_less_than.hpp"

namespace {

template <typename Map, typename... Args>
using try_construct_result = decltype(Map::try_construct(std::declval<Args>()...));

using ::arene::base::byte_swap;
using ::arene::base::compare_three_way;
using ::arene::base::inline_map;
using ::arene::base::is_bidirectional_iterator_v;
using ::arene::base::next;
using ::arene::base::strong_ordering;
using ::map_test::test_string;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicitly tracking global instances
struct counted {
  static std::uint32_t instances;

  counted() noexcept { ++instances; }
  counted(counted const&) noexcept { ++instances; }
  counted(counted&&) noexcept { ++instances; }
  ~counted() { --instances; }

  auto operator=(counted const&) -> counted& = default;
  auto operator=(counted&&) noexcept -> counted& { return *this; }
};
std::uint32_t counted::instances = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

/// @test A default-constructed `inline_map, std::uint32_t, std::uint32_t, 10>` is empty
TEST(InlineMap, DefaultConstructedMapIsEmpty) {
  inline_map<std::uint32_t, std::uint32_t, 10> const map;
  ASSERT_TRUE(map.empty());
}

/// @test Given a default-constructed `inline_map, std::uint32_t, std::uint64_t, 10>`, a value can be added and
/// retrieved using `operator[]`, and the map is no longer empty
TEST(InlineMap, CanUseIndexOperatorToInsert) {
  inline_map<std::uint32_t, std::uint64_t, 10> map;

  constexpr std::uint32_t key = 42;
  constexpr std::uint32_t value = 99;

  map[key] = value;
  ::testing::StaticAssertTypeEq<decltype(map[key]), std::uint64_t&>();

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map[key], value);
}

/// @test Given a default-constructed `inline_map`, after invoking `operator[]` with two different keys, the map is not
/// empty, and `operator[]` can be used to retrieve the values associated with those keys
TEST(InlineMap, CanUseIndexOperatorToInsertMultipleElements) {
  inline_map<std::uint32_t, std::uint32_t, 10> map;

  constexpr std::uint32_t key1 = 42;
  constexpr std::uint32_t value1 = 99;

  map[key1] = value1;

  constexpr std::uint32_t key2 = 123;
  constexpr std::uint32_t value2 = 456;

  map[key2] = value2;

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map[key1], value1);
  ASSERT_EQ(map[key2], value2);
}

/// @test Given a default-constructed `inline_map`, after invoking `operator[]` with multiple different keys,
/// `operator[]` can be used to retrieve the values associated with each of those keys
TEST(InlineMap, CanUseIndexOperatorToInsertAndQueryElements) {
  constexpr std::uint32_t element_count = 100;
  inline_map<std::uint32_t, std::uint32_t, element_count> map;

  for (std::uint32_t key = 0; key < element_count; ++key) {
    map[key] = key * 2;
  }
  for (std::uint32_t key = 0; key != element_count; ++key) {
    ASSERT_EQ(map[key], key * 2);
  }
}

/// @test Given a default-constructed `inline_map`, after invoking `operator[]` with multiple different keys in a
/// non-linear order, `operator[]` can be used to retrieve the values associated with each of those keys
TEST(InlineMap, CanUseIndexOperatorToInsertAndQueryElementsInNonlinearOrder) {
  constexpr std::uint32_t element_count = 100;
  inline_map<std::uint32_t, std::uint32_t, element_count> map;

  for (std::uint32_t key = 0; key < element_count; ++key) {
    map[byte_swap(key * 0x10203040)] = key * 2;
  }
  for (std::uint32_t key = 0; key != element_count; ++key) {
    ASSERT_EQ(map[byte_swap(key * 0x10203040)], key * 2);
  }
}

/// @test `contains` returns false for a key not present in the map
TEST(InlineMap, ContainsReturnsFalseIfElementNotPresent) {
  // NOLINTNEXTLINE(misc-const-correctness)
  inline_map<std::uint32_t, std::uint32_t, 10> map;
  ASSERT_FALSE(map.contains(42));
}

/// @test `contains` returns true after `operator[]` has been invoked to insert an element with the specified key.
TEST(InlineMap, ContainsReturnsTrueIfElementPresent) {
  inline_map<std::uint32_t, std::uint32_t, 10> map;
  constexpr std::uint32_t key = 42;
  constexpr std::uint32_t value = 99;
  map[key] = value;
  ASSERT_TRUE(map.contains(key));
}

/// @test `contains` returns false after `operator[]` has been invoked to insert an element with a different key.
TEST(InlineMap, ContainsReturnsFalseIfOtherElementNotPresent) {
  inline_map<std::uint32_t, std::uint32_t, 10> map;
  constexpr std::uint32_t key = 42;
  constexpr std::uint32_t value = 99;
  map[key] = value;
  ASSERT_FALSE(map.contains(key + 1));
}

/// @test `contains` returns false for a key not present in a `const` map
TEST(InlineMap, ContainsReturnsFalseIfElementNotPresentForConstMap) {
  inline_map<std::uint32_t, std::uint32_t, 10> const map;
  ASSERT_FALSE(map.contains(42));
}

/// @test `contains` returns true if invoked on a `const` reference to an `inline_map` after `operator[]` has been
/// invoked to insert an element with the specified key.
TEST(InlineMap, ContainsReturnsTrueIfElementPresentForConstMap) {
  inline_map<std::uint32_t, std::uint32_t, 10> map;
  constexpr std::uint32_t key = 42;
  constexpr std::uint32_t value = 99;
  map[key] = value;
  auto const& const_map = map;
  ASSERT_TRUE(const_map.contains(key));
}

/// @test `contains` returns false if invoked on a `const` reference to an `inline_map` after `operator[]` has been
/// invoked to insert an element with a different key.
TEST(InlineMap, ContainsReturnsFalseIfOtherElementNotPresentForConstMap) {
  inline_map<std::uint32_t, std::uint32_t, 10> map;
  constexpr std::uint32_t key = 42;
  constexpr std::uint32_t value = 99;
  map[key] = value;
  auto const& const_map = map;
  ASSERT_FALSE(const_map.contains(key + 1));
}

/// @test The `contains` member function is declared `noexcept` if the map comparator is `noexcept`
TEST(InlineMap, ContainsIsNoexceptIfComparatorIs) {
  using imap = inline_map<std::uint32_t, std::uint32_t, 10>;
  static_assert(noexcept(std::declval<imap&>().contains(42)), "Must be noexcept");
}

/// @test The `contains` member function is not declared `noexcept` if the map comparator is not `noexcept`
TEST(InlineMap, ContainsIsNotNoexceptIfComparatorIsNot) {
  struct throwing_less {
    auto operator<(throwing_less const&) const -> bool { return false; }
  };
  using imap = inline_map<throwing_less, std::uint32_t, 10>;
  static_assert(!noexcept(std::declval<imap&>().contains(throwing_less{})), "Must not be noexcept");
}

/// @test The size of a default constructed `inline_map` is zero.
TEST(InlineMap, SizeIsInitiallyZero) {
  inline_map<std::uint32_t, std::uint32_t, 10> const map;
  ASSERT_EQ(map.size(), 0);
}

/// @test Given a default-constructed `inline_map`, after inserting an element, the size is one.
TEST(InlineMap, SizeIsOneWithOneElement) {
  inline_map<std::uint32_t, std::uint32_t, 10> map;
  constexpr std::uint32_t key = 42;
  constexpr std::uint32_t value = 99;
  map[key] = value;
  ASSERT_EQ(map.size(), 1);
}

/// @test Given a map with two elements, if one of the elements is removed by invoking `erase` with the key for that
/// element, then the size is reduced to 1, `contains` returns false when given the removed key, but returns true when
/// given the not-erased key
TEST(InlineMap, EraseRemovesElement) {
  inline_map<std::uint32_t, std::uint32_t, 10> map;
  constexpr std::uint32_t key1 = 42;
  constexpr std::uint32_t value1 = 99;
  constexpr std::uint32_t key2 = 123;
  constexpr std::uint32_t value2 = 456;

  map[key1] = value1;
  map[key2] = value2;
  std::ignore = map.erase(key1);
  ASSERT_EQ(map.size(), 1);
  ASSERT_FALSE(map.contains(key1));
  ASSERT_TRUE(map.contains(key2));
}

/// @test Given a map containing elements, if `erase` is invoked with a key that does not correspond to any of the
/// elements, then the map is unchanged.
TEST(InlineMap, EraseDoesNothingForMissingElement) {
  inline_map<std::uint32_t, std::uint32_t, 10> map;
  constexpr std::uint32_t key1 = 42;
  constexpr std::uint32_t value1 = 99;
  constexpr std::uint32_t key2 = 123;
  constexpr std::uint32_t value2 = 456;

  map[key1] = value1;
  map[key2] = value2;
  std::ignore = map.erase(key1 + key2);
  ASSERT_EQ(map.size(), 2);
  ASSERT_TRUE(map.contains(key1));
  ASSERT_TRUE(map.contains(key2));
}

/// @test Given a map with two elements, if one of the elements is removed by invoking `erase` with an iterator
/// referring to that element, as returned by `find`, then the size is reduced to 1, `contains` returns false when given
/// the removed key, but returns true when given the not-erased key
TEST(InlineMap, EraseRemovesElementByIterator) {
  inline_map<std::uint32_t, std::uint32_t, 10> map;
  constexpr std::uint32_t key1 = 42;
  constexpr std::uint32_t value1 = 99;
  constexpr std::uint32_t key2 = 123;
  constexpr std::uint32_t value2 = 456;

  map[key1] = value1;
  map[key2] = value2;
  auto const pos = map.erase(map.find(key1));
  ASSERT_EQ(map.size(), 1);
  ASSERT_FALSE(map.contains(key1));
  ASSERT_TRUE(map.contains(key2));
  ASSERT_EQ(pos, map.find(key2));
}

/// @test Given a map with two elements, if one of the elements is removed by invoking `erase` with a `const_iterator`
/// referring to that element, as returned by `find`, then the size is reduced to 1, `contains` returns false when given
/// the removed key, but returns true when given the not-erased key
TEST(InlineMap, EraseRemovesElementByConstIterator) {
  using imap = inline_map<std::uint32_t, std::uint32_t, 10>;
  imap map;
  constexpr std::uint32_t key1 = 42;
  constexpr std::uint32_t value1 = 99;
  constexpr std::uint32_t key2 = 123;
  constexpr std::uint32_t value2 = 456;

  map[key1] = value1;
  map[key2] = value2;

  imap::const_iterator const itr = map.find(key1);
  auto const pos = map.erase(itr);
  ASSERT_EQ(map.size(), 1);
  ASSERT_FALSE(map.contains(key1));
  ASSERT_TRUE(map.contains(key2));
  ASSERT_EQ(pos, map.find(key2));
}

/// @test After erasing an element from an `inline_map` using `erase`, a new element can be added
TEST(InlineMap, AfterEraseCanInsertNewElement) {
  inline_map<int, std::uint32_t, 10> map;
  constexpr int key1 = 42;
  constexpr std::uint32_t value1 = 99;
  constexpr int key2 = 123;
  constexpr std::uint32_t value2 = 456;
  constexpr int key3 = -123;
  constexpr std::uint32_t value3 = 782;

  map[key1] = value1;
  map[key2] = value2;
  map.erase(key1);
  map[key3] = value3;
  ASSERT_EQ(map.size(), 2);
  ASSERT_FALSE(map.contains(key1));
  ASSERT_TRUE(map.contains(key3));
  ASSERT_TRUE(map.contains(key2));
  ASSERT_EQ(map[key2], value2);
  ASSERT_EQ(map[key3], value3);
}

namespace {
struct some_key {
  auto operator==(some_key const&) const noexcept -> bool { return true; }
  auto operator<(some_key const&) const noexcept -> bool { return false; }
};
struct some_value {};

}  // namespace

/// @test `inline_map` has the required type aliases for an associative container.
TEST(InlineMap, TypeAliases) {
  struct some_comparator {
    auto operator()(some_key, some_key) const -> bool;
  };
  constexpr std::uint32_t capacity = 123;
  using imap = inline_map<some_key, some_value, capacity>;
  using imap2 = inline_map<some_key, some_value, capacity, some_comparator>;
  ::testing::StaticAssertTypeEq<imap::key_type, some_key>();
  ::testing::StaticAssertTypeEq<imap::mapped_type, some_value>();
  ::testing::StaticAssertTypeEq<imap::value_type, std::pair<some_key const, some_value>>();
  ::testing::StaticAssertTypeEq<imap::pointer, std::pair<some_key const, some_value>*>();
  ::testing::StaticAssertTypeEq<imap::const_pointer, std::pair<some_key const, some_value> const*>();
  ::testing::StaticAssertTypeEq<imap::reference, std::pair<some_key const, some_value>&>();
  ::testing::StaticAssertTypeEq<imap::const_reference, std::pair<some_key const, some_value> const&>();
  ::testing::StaticAssertTypeEq<imap::size_type, std::size_t>();
  ::testing::StaticAssertTypeEq<imap::difference_type, imap::iterator::difference_type>();
  ::testing::StaticAssertTypeEq<imap::key_compare, compare_three_way>();
  ::testing::StaticAssertTypeEq<imap2::key_compare, some_comparator>();
}

/// @test `inline_map` iterators have the correct type aliases and operations to be bidirectional iterators over the
/// value type of the map.
TEST(InlineMap, Iterators) {
  constexpr std::uint32_t capacity = 123;

  using imap = inline_map<some_key, some_value, capacity>;

  static_assert(is_bidirectional_iterator_v<imap::iterator>, "Must be bidirectional iterator");
  static_assert(is_bidirectional_iterator_v<imap::const_iterator>, "Must be bidirectional iterator");
  ::testing::
      StaticAssertTypeEq<std::iterator_traits<imap::iterator>::iterator_category, std::bidirectional_iterator_tag>();
  static_assert(
      std::is_same<std::iterator_traits<imap::const_iterator>::iterator_category, std::bidirectional_iterator_tag>::
          value,
      "Iterators are bidirectional"
  );
  static_assert(!std::is_same<imap::iterator, imap::const_iterator>::value, "Iterators are not const iterators");
  ::testing::StaticAssertTypeEq<imap::iterator::value_type, imap::value_type>();
  ::testing::StaticAssertTypeEq<imap::iterator::difference_type, std::ptrdiff_t>();
  ::testing::StaticAssertTypeEq<imap::iterator::pointer, imap::pointer>();
  ::testing::StaticAssertTypeEq<imap::iterator::reference, imap::reference>();
  ::testing::StaticAssertTypeEq<imap::const_iterator::value_type, imap::value_type>();
  ::testing::StaticAssertTypeEq<imap::const_iterator::difference_type, imap::difference_type>();
  ::testing::StaticAssertTypeEq<imap::const_iterator::pointer, imap::const_pointer>();
  ::testing::StaticAssertTypeEq<imap::const_iterator::reference, imap::const_reference>();

  ::testing::StaticAssertTypeEq<decltype(*std::declval<imap::iterator&>()), imap::reference>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<imap::iterator&>().operator->()), imap::pointer>();
  ::testing::StaticAssertTypeEq<decltype(++std::declval<imap::iterator&>()), imap::iterator&>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<imap::iterator&>()++), imap::iterator>();
  ::testing::StaticAssertTypeEq<decltype(--std::declval<imap::iterator&>()), imap::iterator&>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<imap::iterator&>()--), imap::iterator>();
  static_assert(arene::base::is_equality_comparable_v<imap::iterator>, "equality comparable");
  static_assert(arene::base::is_inequality_comparable_v<imap::iterator>, "inequality comparable");
}

/// @test Given a default-constructed map, the iterators returned from `begin` and `end` compare equal.
TEST(InlineMap, ForAnEmptyMapBeginEqualsEnd) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<some_key, some_value, capacity>;

  ::testing::StaticAssertTypeEq<decltype(std::declval<imap&>().begin()), imap::iterator>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<imap&>().end()), imap::iterator>();

  imap map;
  ASSERT_EQ(map.begin(), map.end());
}

/// @test Given an `inline_map` with one element, the iterators returned from `begin` and `end` do not compare equal.
TEST(InlineMap, ForANonEmptyMapBeginIsNotEnd) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<some_key, some_value, capacity>;

  imap map;
  map[some_key{}] = some_value{};
  ASSERT_NE(map.begin(), map.end());
}

/// @test Given an `inline_map` with one element, derefencing the iterator returned from `begin` returns a reference to
/// the `value_type` element, the `second` field of which is referenced by the return from `operator[]` given the key
/// for that element
TEST(InlineMap, ForANonEmptyMapDereferencingBeginGivesFirstElement) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  ASSERT_EQ(&map[key], &(*map.begin()).second);
  ASSERT_EQ(&map[key], &map.begin()->second);
}

/// @test Given an `inline_map` with one element, dereferencing the iterator returned from `begin`, and dereferencing a
/// `const` copy of that iterator yield the same non-`const` references.
TEST(InlineMap, DereferencingWorksForConstQualifiedIterators) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  auto itr = map.begin();
  auto const citr = itr;
  int& ref1 = (*itr).second;
  int& ref2 = itr->second;
  int& ref3 = (*citr).second;
  int& ref4 = citr->second;
  ASSERT_EQ(&ref1, &ref2);
  ASSERT_EQ(&ref1, &ref3);
  ASSERT_EQ(&ref1, &ref4);
}

/// @test Derefencing an iterator is `noexcept`
TEST(InlineMap, DereferencingIsNoExcept) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  auto itr = map.begin();
  auto const citr = itr;
  static_assert(noexcept(*itr), "Must be noexcept");
  static_assert(noexcept(itr->second), "Must be noexcept");
  static_assert(noexcept(*citr), "Must be noexcept");
  static_assert(noexcept(citr->second), "Must be noexcept");
}

/// @test If there is only one element in a map, incrementing the iterator returned from `begin` with pre-increment
/// yields an iterator equal to that retuend from `end`
TEST(InlineMap, ForAMapWithOneElementPreIncrementBeginGivesEnd) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  auto itr = map.begin();
  auto& itr2 = ++itr;
  ASSERT_NE(itr, map.begin());
  ASSERT_EQ(itr, map.end());
  ASSERT_EQ(&itr, &itr2);
}

/// @test Pre-increment on an iterator is `noexcept`
TEST(InlineMap, IteratorPreincrementIsNoexcept) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  auto itr = map.begin();
  static_assert(noexcept(++itr), "Must be noexcept");
}

/// @test Post-increment on an iterator is `noexcept`
TEST(InlineMap, IteratorPostincrementIsNoexcept) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  auto itr = map.begin();
  static_assert(noexcept(itr++), "Must be noexcept");
}

/// @test Pre-decrement on an iterator is `noexcept`
TEST(InlineMap, IteratorPredecrementIsNoexcept) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  auto itr = map.begin();
  static_assert(noexcept(--itr), "Must be noexcept");
}

/// @test Post-decrement on an iterator is `noexcept`
TEST(InlineMap, IteratorPostdecrementIsNoexcept) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  auto itr = map.begin();
  static_assert(noexcept(itr--), "Must be noexcept");
}

/// @test If there is only one element in a map, incrementing the iterator returned from `begin` with post-increment
/// yields an iterator equal to that retuend from `end`
TEST(InlineMap, ForAMapWithOneElementPostIncrementBeginGivesEnd) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  auto itr = map.begin();
  auto itr2 = itr++;
  ASSERT_NE(itr, map.begin());
  ASSERT_EQ(itr, map.end());
  ASSERT_EQ(itr2, map.begin());
}

/// @test For a map with one element, decrementing the iterator returned from `end` using pre-decrement yields an
/// iterator equal to that returned from `begin`
TEST(InlineMap, ForAMapWithOneElementPreDecrementEndGivesBegin) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  auto itr = map.end();
  auto& itr2 = --itr;
  ASSERT_NE(itr, map.end());
  ASSERT_EQ(itr, map.begin());
  ASSERT_EQ(&itr, &itr2);
}

/// @test For a map with one element, decrementing the iterator returned from `end` using post-decrement yields an
/// iterator equal to that returned from `begin`
TEST(InlineMap, ForAMapWithOneElementPostDecrementEndGivesBegin) {
  constexpr std::uint32_t capacity = 10;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  constexpr std::uint32_t key = 99;
  constexpr int value = -123;
  map[key] = value;
  auto itr = map.end();
  auto itr2 = itr--;
  ASSERT_NE(itr, map.end());
  ASSERT_EQ(itr, map.begin());
  ASSERT_EQ(itr2, map.end());
}

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicitly counting global interactions
struct eq_lt_comparable_counted {
  std::uint32_t val;

  static std::uint32_t lt_count;
  static std::uint32_t eq_count;

  static void reset() {
    lt_count = 0;
    eq_count = 0;
  }

  friend auto operator<(eq_lt_comparable_counted const& lhs, eq_lt_comparable_counted const& rhs) noexcept -> bool {
    ++lt_count;
    return lhs.val < rhs.val;
  }
  friend auto operator==(eq_lt_comparable_counted const& lhs, eq_lt_comparable_counted const& rhs) noexcept -> bool {
    ++eq_count;
    return lhs.val == rhs.val;
  }
};
std::uint32_t eq_lt_comparable_counted::lt_count = 0;
std::uint32_t eq_lt_comparable_counted::eq_count = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

/// @test The `contains` member function uses a binary search to check the existence of an element with the specified
/// key, and therefore invokes the comparison operation an appropriate number of times to reflect the logarithmic nature
/// of such a search.
TEST(InlineMap, ContainsUsesBinarySearch) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<eq_lt_comparable_counted, int, capacity>;

  imap map;

  constexpr std::uint32_t count = 15;
  constexpr int value = -123;

  for (std::uint32_t idx = 0; idx < count; ++idx) {
    map[eq_lt_comparable_counted{idx}] = value;
  }
  eq_lt_comparable_counted::reset();
  ASSERT_TRUE(map.contains(eq_lt_comparable_counted{5}));
  ASSERT_EQ(eq_lt_comparable_counted::lt_count, 2);
  ASSERT_EQ(eq_lt_comparable_counted::eq_count, 3);

  eq_lt_comparable_counted::reset();
  ASSERT_TRUE(map.contains(eq_lt_comparable_counted{12}));
  ASSERT_EQ(eq_lt_comparable_counted::lt_count, 3);
  ASSERT_EQ(eq_lt_comparable_counted::eq_count, 4);

  eq_lt_comparable_counted::reset();
  ASSERT_FALSE(map.contains(eq_lt_comparable_counted{15}));
  ASSERT_EQ(eq_lt_comparable_counted::lt_count, 4);
  ASSERT_EQ(eq_lt_comparable_counted::eq_count, 4);
}

/// @test The `erase` member function uses a binary search to check the existence of an element with the specified
/// key, and therefore invokes the comparison operation an appropriate number of times to reflect the logarithmic nature
/// of such a search.
TEST(InlineMap, EraseUsesBinarySearch) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<eq_lt_comparable_counted, int, capacity>;

  imap map;

  constexpr std::uint32_t count = 15;
  constexpr int value = -123;

  for (std::uint32_t idx = 0; idx < count; ++idx) {
    map[eq_lt_comparable_counted{idx}] = value;
  }
  eq_lt_comparable_counted::reset();
  map.erase(eq_lt_comparable_counted{15});
  ASSERT_EQ(map.size(), count);
  ASSERT_EQ(eq_lt_comparable_counted::lt_count, 4);
  ASSERT_EQ(eq_lt_comparable_counted::eq_count, 4);

  eq_lt_comparable_counted::reset();
  map.erase(eq_lt_comparable_counted{5});
  ASSERT_EQ(map.size(), count - 1);
  ASSERT_EQ(eq_lt_comparable_counted::lt_count, 2);
  ASSERT_EQ(eq_lt_comparable_counted::eq_count, 3);

  eq_lt_comparable_counted::reset();
  map.erase(eq_lt_comparable_counted{12});
  ASSERT_EQ(map.size(), count - 2);
  ASSERT_EQ(eq_lt_comparable_counted::lt_count, 1);
  ASSERT_EQ(eq_lt_comparable_counted::eq_count, 2);
}

/// @test `lower_bound` returns an iterator referring to the element with a given key, or the first element with a key
/// greater than the specified key, if there is no element with the given key, or an iterator equal to the iterator
/// returned by `end` if there is no such element.
TEST(InlineMap, LowerBound) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;

  constexpr imap::key_type count = 15;
  constexpr int value = -123;

  for (imap::key_type idx = 0; idx < count; ++idx) {
    map[idx * 2] = value;
  }

  for (imap::key_type idx = 0; idx < count; ++idx) {
    ASSERT_EQ(map.lower_bound(idx * 2), next(map.begin(), idx));
    ASSERT_EQ(map.lower_bound(idx * 2 + 1), next(map.begin(), idx + 1));
  }

  ASSERT_EQ(map.lower_bound(count * 2), map.end());
  ASSERT_EQ(map.lower_bound(count * 4), map.end());
}

/// @test `begin` and `end` on a `const` reference to an `inline_map` return `const_iterator`s that are equal to those
/// returned from `cbegin` and `begin` or `cend` and `end`, respectively, from a non-`const` reference to the same map.
TEST(InlineMap, ConstIteration) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  ::testing::StaticAssertTypeEq<decltype(std::declval<imap const&>().begin()), imap::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<imap&>().cbegin()), imap::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<imap const&>().end()), imap::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<imap&>().cend()), imap::const_iterator>();

  imap map;
  map[0] = "hello";
  map[1] = "goodbye";
  map[2] = "world";
  auto const& const_map = map;

  ASSERT_EQ(map.begin(), const_map.begin());
  ASSERT_EQ(map.cbegin(), const_map.begin());
  ASSERT_EQ(map.end(), const_map.end());
  ASSERT_EQ(map.cend(), const_map.end());
}

/// @test Insertion of values using `operator[]` and lookup using `contains` work when the key only supports less-than
/// comparison, when using the default map comparator
TEST(InlineMap, CanUseKeyWithJustLessThan) {
  constexpr std::uint32_t capacity = 20;
  using lt_comparable_counted = map_test::lt_comparable_counted<std::uint32_t>;
  using imap = inline_map<lt_comparable_counted, int, capacity>;

  imap map;

  constexpr std::uint32_t count = 15;
  constexpr int value = -123;

  for (std::uint32_t idx = 0; idx < count; ++idx) {
    map[lt_comparable_counted{idx}] = value;
  }
  lt_comparable_counted::reset();
  ASSERT_TRUE(map.contains(lt_comparable_counted{5}));
  ASSERT_EQ(lt_comparable_counted::lt_count, 5);

  lt_comparable_counted::reset();
  ASSERT_TRUE(map.contains(lt_comparable_counted{12}));
  ASSERT_EQ(lt_comparable_counted::lt_count, 6);

  lt_comparable_counted::reset();
  ASSERT_FALSE(map.contains(lt_comparable_counted{15}));
  ASSERT_EQ(lt_comparable_counted::lt_count, 4);

  lt_comparable_counted::reset();
  map.erase(lt_comparable_counted{12});
  ASSERT_EQ(lt_comparable_counted::lt_count, 6);
  ASSERT_EQ(map.size(), count - 1);
  ASSERT_FALSE(map.contains(lt_comparable_counted{12}));
}

struct three_way_comparable {
  std::uint32_t val;

  static auto three_way_compare(three_way_comparable const& lhs, three_way_comparable const& rhs) noexcept
      -> strong_ordering {
    return compare_three_way{}(lhs.val, rhs.val);
  }
};

/// @test Insertion of values using `operator[]` and lookup using `contains` work when the key only supports three-way
/// comparison, when using the default map comparator
TEST(InlineMap, CanUseThreeWayComparison) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<three_way_comparable, int, capacity>;

  imap map;

  constexpr std::uint32_t count = 15;
  constexpr int value = -123;

  for (std::uint32_t idx = 0; idx < count; ++idx) {
    map[three_way_comparable{idx}] = value;
  }
  ASSERT_TRUE(map.contains(three_way_comparable{5}));

  ASSERT_TRUE(map.contains(three_way_comparable{12}));

  ASSERT_FALSE(map.contains(three_way_comparable{15}));

  map.erase(three_way_comparable{12});
  ASSERT_EQ(map.size(), count - 1);
  ASSERT_FALSE(map.contains(three_way_comparable{12}));
}

/// @test An `inline_map` can be constructed from an initializer list holding key/value pairs. The values can then be
/// retrieved using `operator[]` passing the corresponding key
TEST(InlineMap, CanConstructMapFromInitializerList) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 123;
  std::uint32_t const key3 = 5;

  test_string const value1 = "hello";
  test_string const value2 = "world";
  test_string const value3 = "goodbye";

  imap map{{key1, value1}, {key2, value2}, {key3, value3}};

  ASSERT_EQ(map.size(), 3);
  ASSERT_EQ(map[key1], value1);
  ASSERT_EQ(map[key2], value2);
  ASSERT_EQ(map[key3], value3);
}

struct move_only_value {
  arene::base::optional<std::uint32_t> val{};

  move_only_value() noexcept = default;
  explicit move_only_value(std::uint32_t value) noexcept
      : val(value) {}
  move_only_value(move_only_value const&) = delete;
  move_only_value(move_only_value&& other) noexcept
      : val(std::move(other.val)) {
    other.val.reset();  // explicitly reset so we can test which instances have been moved from
  }
  auto operator=(move_only_value const&) -> move_only_value& = delete;
  auto operator=(move_only_value&& other) noexcept -> move_only_value& {
    val = std::move(other.val);
    other.val.reset();  // explicitly reset so we can test which instances have been moved from
    return *this;
  }
  ~move_only_value() = default;
};

/// @test The overload of `try_construct` taking an initializer list is not considered for overload resolution if the
/// mapped type is move-only.
TEST(InlineMap, CannotConstructMapWithMoveOnlyElementsFromInitializerListWithFactory) {
  constexpr std::uint32_t capacity = 20;

  using imap = inline_map<std::uint32_t, move_only_value, capacity>;

  static_assert(
      !arene::base::substitution_succeeds<try_construct_result, imap, std::initializer_list<imap::value_type>>,
      "Cannot construct from init list if value type not copy constructible"
  );
}

/// @test The `try_construct` overload taking an initializer list is `noexcept` if the comparator is
/// nothrow-default-constructible, and the key type and mapped type are both nothrow-copy-constructible.
TEST(InlineMap, InitListFactoryIsNoexceptIfComparatorNothrowDefaultConstructibleAndValueNothrowCopyConstructible) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, std::uint32_t, capacity>;

  static_assert(
      noexcept(imap::try_construct(std::declval<std::initializer_list<imap::value_type>>())),
      "Must be noexcept"
  );
}

/// @test The `try_construct` overload taking an initializer list is not `noexcept` if the mapped type is not
/// nothrow-copy-constructible.
TEST(InlineMap, InitListFactoryIsNotNoexceptIfValueIsNotNothrowCopyConstructible) {
  using value_type = testing::configurable_value<std::int32_t, testing::throws_on::copy_construct>;

  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, value_type, capacity>;

  static_assert(
      !noexcept(imap::try_construct(std::declval<std::initializer_list<imap::value_type>>())),
      "Must not be noexcept"
  );
}

namespace {
struct compare_with_throwing_default_construct {
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  compare_with_throwing_default_construct() noexcept(false) {}

  template <typename T>
  auto operator()(T const& lhs, T const& rhs) const noexcept -> strong_ordering {
    return compare_three_way{}(lhs, rhs);
  }
};
}  // namespace

/// @test The `try_construct` overload taking an initializer list is not `noexcept` if the comparator is not nothrow
/// default-constructible
TEST(InlineMap, InitListFactoryIsNotNoexceptIfComparatorIsNotNothrowDefaultConstructible) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, std::uint32_t, capacity, compare_with_throwing_default_construct>;

  static_assert(
      !noexcept(imap::try_construct(std::declval<std::initializer_list<imap::value_type>>())),
      "Must not be noexcept"
  );
}

namespace {
struct compare_with_throwing_compare {
  template <typename T>
  auto operator()(T const& lhs, T const& rhs) const noexcept(false) -> strong_ordering {
    return compare_three_way{}(lhs, rhs);
  }
};
}  // namespace

/// @test The `try_construct` overload taking an initializer list is not `noexcept` if the comparator is not nothrow
/// invocable
TEST(InlineMap, InitListFactoryIsNotNoexceptIfComparatorIsNotNothrow) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, std::uint32_t, capacity, compare_with_throwing_compare>;

  static_assert(
      !noexcept(imap::try_construct(std::declval<std::initializer_list<imap::value_type>>())),
      "Must not be noexcept"
  );
}

/// @test The `try_construct` overload with no arguments is `noexcept` if the comparator is nothrow default
/// constructible
TEST(InlineMap, NoArgsFactoryIsNoexceptIfComparatorNothrowDefaultConstructible) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  using imap2 = inline_map<std::uint32_t, std::uint32_t, capacity>;

  static_assert(noexcept(imap::try_construct()), "Must be noexcept");
  static_assert(noexcept(imap2::try_construct()), "Must be noexcept");
}

/// @test The `try_construct` overload with no arguments is not `noexcept` if the comparator is not nothrow default
/// constructible
TEST(InlineMap, NoArgsFactoryIsNotNoexceptIfComparatorIsNotNothrowDefaultConstructible) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, std::uint32_t, capacity, compare_with_throwing_default_construct>;

  static_assert(!noexcept(imap::try_construct()), "Must not be noexcept");
}

/// @test Invoking `try_construct` with no arguments returns an `optional` holding an empty `inline_map`.
TEST(InlineMap, CanDefaultConstructMapWithFactory) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  static_assert(arene::base::substitution_succeeds<try_construct_result, imap>, "Can default construct");

  auto result = imap::try_construct();

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap>>();

  ASSERT_TRUE(result.has_value());
  auto& map = *result;

  ASSERT_EQ(map.size(), 0);
}

/// @test Invoking `try_construct` with an initializer list of key/value pairs returns an `optional` holding an
/// `inline_map` with the specified keys and values, such that the values can then be retrieved using `operator[]`
/// passing the corresponding key
TEST(InlineMap, CanConstructMapFromInitializerListWithFactory) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 123;
  std::uint32_t const key3 = 5;

  test_string const value1 = "hello";
  test_string const value2 = "world";
  test_string const value3 = "goodbye";

  static_assert(
      arene::base::substitution_succeeds<try_construct_result, imap, std::initializer_list<imap::value_type>>,
      "Can construct from init list"
  );

  auto result = imap::try_construct({{key1, value1}, {key2, value2}, {key3, value3}});

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap>>();

  ASSERT_TRUE(result.has_value());
  auto& map = *result;

  ASSERT_EQ(map.size(), 3);
  ASSERT_EQ(map[key1], value1);
  ASSERT_EQ(map[key2], value2);
  ASSERT_EQ(map[key3], value3);
}

/// @test Invoking `try_construct` with an `inline_map` with a larger capacity and more elements than the capacity of
/// the destination type returns an empty `optional`.
TEST(InlineMap, TryConstructFromOverSizedSourceReturnsEmpty) {
  constexpr std::uint32_t capacity1 = 10;
  using imap1 = inline_map<std::uint32_t, test_string, capacity1>;

  constexpr std::uint32_t capacity2 = 5;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  test_string const value1 = "hello";

  imap1 source;

  for (std::uint32_t key = 0; key < capacity1; ++key) {
    source[key] = value1;
  }

  auto result = imap2::try_construct(source);

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap2>>();

  ASSERT_FALSE(result.has_value());
}

/// @test Invoking `try_construct` with an `inline_map` with a larger capacity, but a size that is less than or equal to
/// the capacity of the destination type returns an `optional` holding an `inline_map` with copies of the keys and
/// values from the source, such that the values can then be retrieved using `operator[]` passing the corresponding key
TEST(InlineMap, TryConstructFromOKSizedSourceWithLargeCapacityIsOK) {
  constexpr std::uint32_t capacity1 = 10;
  using imap1 = inline_map<std::uint32_t, test_string, capacity1>;

  constexpr std::uint32_t capacity2 = 3;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  test_string const value1 = "hello";

  imap1 source;

  for (std::uint32_t key = 0; key < capacity2; ++key) {
    source[key] = value1;
  }

  auto result = imap2::try_construct(source);

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap2>>();

  ASSERT_TRUE(result.has_value());

  for (std::uint32_t key = 0; key < capacity2; ++key) {
    ASSERT_TRUE(result->contains(key));
    ASSERT_EQ((*result)[key], value1);
  }
}

/// @test Invoking `try_construct` with an rvalue `inline_map` with a larger capacity and more elements than the
/// capacity of the destination type returns an empty `optional`.
TEST(InlineMap, TryMoveConstructFromOverSizedSourceReturnsEmpty) {
  constexpr std::uint32_t capacity1 = 10;
  using imap1 = inline_map<std::uint32_t, test_string, capacity1>;

  constexpr std::uint32_t capacity2 = 5;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  test_string const value1 = "hello";

  imap1 source;

  for (std::uint32_t key = 0; key < capacity1; ++key) {
    source[key] = value1;
  }

  auto result = imap2::try_construct(std::move(source));

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap2>>();

  ASSERT_FALSE(result.has_value());
}

/// @test Invoking `try_construct` with an rvalue `inline_map` with a larger capacity, but a size that is less than or
/// equal to the capacity of the destination type returns an `optional` holding an `inline_map` with copies of the keys
/// and values from the source, such that the values can then be retrieved using `operator[]` passing the corresponding
/// key
TEST(InlineMap, TryMoveConstructFromOKSizedSourceWithLargeCapacityIsOK) {
  constexpr std::uint32_t capacity1 = 10;
  using imap1 = inline_map<std::uint32_t, test_string, capacity1>;

  constexpr std::uint32_t capacity2 = 3;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  test_string const value1 = "hello";

  imap1 source;

  for (std::uint32_t key = 0; key < capacity2; ++key) {
    source[key] = value1;
  }

  auto result = imap2::try_construct(std::move(source));

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap2>>();

  ASSERT_TRUE(result.has_value());

  for (std::uint32_t key = 0; key < capacity2; ++key) {
    ASSERT_TRUE(result->contains(key));
    ASSERT_EQ((*result)[key], value1);
  }
}

/// @test Invoking `try_construct` with an initializer list of key/value pairs with a larger size than the capacity of
/// the destination map returns an empty `optional`.
TEST(InlineMap, ConstructingFromOverlyLargeInitListWithFactoryReturnsEmptyOptional) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 123;
  std::uint32_t const key3 = 5;

  test_string const value1 = "hello";
  test_string const value2 = "world";
  test_string const value3 = "goodbye";

  auto result = imap::try_construct({{key1, value1}, {key2, value2}, {key3, value3}});

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap>>();

  ASSERT_FALSE(result.has_value());
}

/// @test Invoking `try_construct` with an initializer list of key/value pairs where the size of the initializer list
/// exceeds the capacity of the destination map, but the number of distinct keys is less than or equal to the capacity
/// returns an `optional` holding an `inline_map` with the specified keys and values, such that the values can then be
/// retrieved using `operator[]` passing the corresponding key, and where the value returned for duplicate keys is the
/// first value for that key from the initializer list
TEST(InlineMap, ConstructingFromOverlyLargeInitListWithFactoryWithDuplicatesIsOK) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 123;

  test_string const value1 = "hello";
  test_string const value2 = "world";
  test_string const value3 = "goodbye";

  auto result = imap::try_construct({{key1, value1}, {key1, value3}, {key2, value2}});

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap>>();

  ASSERT_TRUE(result.has_value());
  auto& map = *result;

  ASSERT_EQ(map.size(), 2);
  ASSERT_EQ(map[key1], value1);
  ASSERT_EQ(map[key2], value2);
}

namespace {
struct non_default_constructible_comparator : counted {
  std::uint32_t tag;
  explicit non_default_constructible_comparator(std::uint32_t that_tag) noexcept
      : tag(that_tag) {}

  template <typename T>
  auto operator()(T const& lhs, T const& rhs) const noexcept -> bool {
    return lhs < rhs;
  }
};

struct throwing_copy_comparator {
  explicit throwing_copy_comparator(int) noexcept {}

  ~throwing_copy_comparator() = default;

  // NOLINTNEXTLINE(hicpp-use-equals-default, modernize-use-equals-default)
  throwing_copy_comparator(throwing_copy_comparator const&) noexcept(false) {}
  auto operator=(throwing_copy_comparator const&) -> throwing_copy_comparator& = default;
  throwing_copy_comparator(throwing_copy_comparator&&) = default;
  auto operator=(throwing_copy_comparator&&) -> throwing_copy_comparator& = default;

  auto operator()(std::uint32_t lhs, std::uint32_t rhs) const noexcept -> bool { return lhs < rhs; }
};

}  // namespace

/// @test The overload of `try_construct` taking an initializer list and a comparator is not `noexcept` if the
/// comparator is not nothrow-copy-constructible
TEST(InlineMap, InitListAndComparatorFactoryIsNotNoexceptIfComparatorIsNotNothrowCopyConstructible) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, std::uint32_t, capacity, throwing_copy_comparator>;

  static_assert(
      !noexcept(imap::try_construct(
          std::declval<std::initializer_list<imap::value_type>>(),
          std::declval<throwing_copy_comparator>()
      )),
      "Must not be noexcept"
  );
}

/// @test The overload of `try_construct` taking an initializer list and a comparator is `noexcept` if the
/// comparator is nothrow-copy-constructible and invoking the comparator is also `noexcept`
TEST(InlineMap, InitListAndComparatorFactoryIsNoexceptIfComparatorIsNothrowConstructibleAndComparing) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, std::uint32_t, capacity, non_default_constructible_comparator>;

  static_assert(
      noexcept(imap::try_construct(
          std::declval<std::initializer_list<imap::value_type>>(),
          std::declval<non_default_constructible_comparator>()
      )),
      "Must be noexcept"
  );
}

/// @test The overload of `try_construct` taking a comparator is not `noexcept` if the comparator is not
/// nothrow-copy-constructible
TEST(InlineMap, ComparatorFactoryIsNotNoexceptIfComparatorIsNotNothrowCopyConstructible) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, std::uint32_t, capacity, throwing_copy_comparator>;

  static_assert(!noexcept(imap::try_construct(std::declval<throwing_copy_comparator>())), "Must not be noexcept");
}

/// @test The overload of `try_construct` taking a comparator is `noexcept` if the comparator is
/// nothrow-copy-constructible and invoking the comparator is also `noexcept`
TEST(InlineMap, ComparatorFactoryIsNoexceptIfComparatorIsNothrowCopyConstructible) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, non_default_constructible_comparator>;

  static_assert(
      noexcept(imap::try_construct(std::declval<non_default_constructible_comparator>())),
      "Must be noexcept"
  );
}

/// @test Invoking `try_construct` with a comparator returns an `optional` holding an `inline_map` where the comparator
/// is a copy of that supplied to `try_construct`.
TEST(InlineMap, ComparatorFactoryConstructsMap) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, std::uint32_t, capacity, non_default_constructible_comparator>;

  constexpr std::uint32_t tag_value = 987654321;
  auto result = imap::try_construct(non_default_constructible_comparator(tag_value));
  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap>>();
  ASSERT_TRUE(result);
  ASSERT_EQ(result->key_comp().tag, tag_value);
}

namespace {
struct non_default_constructible_compare_with_throwing_compare {
  explicit non_default_constructible_compare_with_throwing_compare(int) noexcept {}

  template <typename T>
  auto operator()(T const& lhs, T const& rhs) const noexcept(false) -> strong_ordering {
    return compare_three_way{}(lhs, rhs);
  }
};
}  // namespace

/// @test The overload of `try_construct` taking an initializer list and a comparator is not `noexcept` if invoking the
/// comparator is not `noexcept`
TEST(InlineMap, InitListAndComparatorFactoryIsNotNoexceptIfComparatorIsNotNothrow) {
  constexpr std::uint32_t capacity = 20;
  using imap =
      inline_map<std::uint32_t, std::uint32_t, capacity, non_default_constructible_compare_with_throwing_compare>;

  static_assert(
      !noexcept(imap::try_construct(
          std::declval<std::initializer_list<imap::value_type>>(),
          std::declval<non_default_constructible_compare_with_throwing_compare>()
      )),
      "Must not be noexcept"
  );
}

/// @test Invoking `try_construct` with an initializer list and a comparator returns an `optional` holding an
/// `inline_map` where the comparator is a copy of that supplied to `try_construct`, and the map contains the values
/// from the initializer list associated with the corresponding keys.
TEST(InlineMap, CanConstructMapFromInitializerListAndComparatorWithFactory) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, non_default_constructible_comparator>;

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 123;
  std::uint32_t const key3 = 5;

  test_string const value1 = "hello";
  test_string const value2 = "world";
  test_string const value3 = "goodbye";

  std::uint32_t const tag_value = 12345;

  static_assert(
      arene::base::substitution_succeeds<
          try_construct_result,
          imap,
          std::initializer_list<imap::value_type>,
          non_default_constructible_comparator>,
      "Can construct from init list and comparator"
  );

  auto result = imap::try_construct(
      {{key1, value1}, {key2, value2}, {key3, value3}},
      non_default_constructible_comparator{tag_value}
  );

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap>>();

  ASSERT_TRUE(result.has_value());
  auto& map = *result;

  ASSERT_EQ(map.size(), 3);
  ASSERT_EQ(map[key1], value1);
  ASSERT_EQ(map[key2], value2);
  ASSERT_EQ(map[key3], value3);

  ASSERT_EQ(map.key_comp().tag, tag_value);
}

/// @test Invoking `try_construct` with an initializer list and a comparator where the number of elements in the
/// initializer list exceeds the capacity of the destination map returns an empty `optional`.
TEST(InlineMap, ConstructingFromOverlyLargeInitListAndComparatorWithFactoryReturnsEmptyOptional) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity, non_default_constructible_comparator>;

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 123;
  std::uint32_t const key3 = 5;

  test_string const value1 = "hello";
  test_string const value2 = "world";
  test_string const value3 = "goodbye";

  std::uint32_t const tag_value = 12345;

  auto result = imap::try_construct(
      {{key1, value1}, {key2, value2}, {key3, value3}},
      non_default_constructible_comparator{tag_value}
  );

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap>>();

  ASSERT_FALSE(result.has_value());
}

/// @test Invoking `try_construct` with an initializer list of key/value pairs and a comparator where the size of the
/// initializer list exceeds the capacity of the destination map, but the number of distinct keys is less than or equal
/// to the capacity returns an `optional` holding an `inline_map` with the specified keys and values, such that the
/// values can then be retrieved using `operator[]` passing the corresponding key, and where the value returned for
/// duplicate keys is the first value for that key from the initializer list, and the comparator in the map is a copy of
/// the supplied comparator
TEST(InlineMap, ConstructingFromOverlyLargeInitListAndComparatorWithFactoryWithDuplicatesIsOK) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity, non_default_constructible_comparator>;

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 123;

  test_string const value1 = "hello";
  test_string const value2 = "world";
  test_string const value3 = "goodbye";

  std::uint32_t const tag_value = 12345;

  auto result = imap::try_construct(
      {{key1, value1}, {key1, value3}, {key2, value2}},
      non_default_constructible_comparator{tag_value}
  );

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap>>();

  ASSERT_TRUE(result.has_value());
  auto& map = *result;

  ASSERT_EQ(map.size(), 2);
  ASSERT_EQ(map[key1], value1);
  ASSERT_EQ(map[key2], value2);
}

/// @test Constructing an `inline_map` with an initializer list of key/value pairs and a comparator initializes the map
/// with the specified keys and values, such that the values can then be retrieved using `operator[]` passing the
/// corresponding key, and where the value returned for duplicate keys is the first value for that key from the
/// initializer list, and the comparator in the map is a copy of the supplied comparator
TEST(InlineMap, CanConstructMapFromInitializerListAndComparator) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, non_default_constructible_comparator>;

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 123;
  std::uint32_t const key3 = 5;

  test_string const value1 = "hello";
  test_string const value2 = "world";
  test_string const value3 = "goodbye";

  std::uint32_t const tag_value = 12345;

  imap map{{{key1, value1}, {key2, value2}, {key3, value3}}, non_default_constructible_comparator{tag_value}};
  ASSERT_EQ(map.size(), 3);
  ASSERT_EQ(map[key1], value1);
  ASSERT_EQ(map[key2], value2);
  ASSERT_EQ(map[key3], value3);

  ASSERT_EQ(map.key_comp().tag, tag_value);
  ASSERT_EQ(counted::instances, 1);
}

/// @test Constructing an `inline_map` from an initializer list where the mapped type is not default-constructible
/// initializes the map with the specified keys and values, such that the iterating through the map yields the
/// corresponding keys and values in sorted order of the keys
TEST(InlineMap, CanConstructFromInitListWithNonDefaultConstructibleType) {
  struct non_default_constructible {
    std::uint32_t val;

    explicit non_default_constructible(std::uint32_t newval)
        : val(newval) {}
  };

  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, non_default_constructible, capacity>;

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 123;
  std::uint32_t const key3 = 5;

  std::uint32_t const value1 = 99;
  std::uint32_t const value2 = 75;
  std::uint32_t const value3 = 1000;

  imap map{
      {key1, non_default_constructible{value1}},
      {key2, non_default_constructible{value2}},
      {key3, non_default_constructible{value3}}
  };

  ASSERT_EQ(map.size(), 3);
  auto itr = map.begin();
  ASSERT_EQ(itr->first, key3);
  ASSERT_EQ(itr->second.val, value3);
  ++itr;
  ASSERT_EQ(itr->first, key1);
  ASSERT_EQ(itr->second.val, value1);
  ++itr;
  ASSERT_EQ(itr->first, key2);
  ASSERT_EQ(itr->second.val, value2);
}

/// @test If `find` is invoked with a key that is not present in the map, then it returns an iterator equal to that
/// returned from `end`
TEST(InlineMap, IfElementNotPresentFindReturnsEnd) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  imap map{{1U, "hello"}};
  ASSERT_EQ(map.find(2), map.end());
}

/// @test `find` is `noexcept` if and only if invoking the comparator is `noexcept`
TEST(InlineMap, FindIsNoexceptIfComparisonIs) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  static_assert(noexcept(std::declval<imap&>().find(42)), "Must be noexcept");
  static_assert(noexcept(std::declval<imap const&>().find(42)), "Must be noexcept");

  struct throwing_comparator {
    auto operator()(std::uint32_t, std::uint32_t) const -> bool;
  };

  using imap2 = inline_map<std::uint32_t, test_string, capacity, throwing_comparator>;

  static_assert(!noexcept(std::declval<imap2&>().find(42)), "Must not be noexcept");
  static_assert(!noexcept(std::declval<imap2 const&>().find(42)), "Must not be noexcept");
}

/// @test If `find` is invoked with a key that is present in the map, then it returns an iterator referring to that
/// element
TEST(InlineMap, IfElementPresentFindReturnsIteratorToValue) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  std::uint32_t const key = 42;
  imap map{{key - 1, "first"}, {key, "hello"}, {key + 1, "last"}};
  ASSERT_EQ(map.find(key), next(map.begin()));
}

/// @test If `find` is invoked on a `const` map with a key that is not present in the map, then it returns an iterator
/// equal to that returned from `end`
TEST(InlineMap, IfElementNotPresentFindReturnsEndForConstMap) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  imap const map{{1U, "hello"}};
  ASSERT_EQ(map.find(2), map.end());
}

/// @test If `find` is invoked on a `const` `inline_map` with a key that is present in the map, then it returns an
/// iterator referring to that element
TEST(InlineMap, IfElementPresentFindReturnsIteratorToValueForConstMap) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  std::uint32_t const key = 42;
  imap const map{{key - 1, "first"}, {key, "hello"}, {key + 1, "last"}};
  ASSERT_EQ(map.find(key), next(map.begin()));
}

/// @test The stored comparator in the map is used directly for key comparisons in `operator[]` rather than a copy being
/// used, such that the comparator's state may be modified by such invocations
TEST(InlineMap, CanUseStatefulComparator) {
  struct counting_comparator {
    auto operator()(std::uint32_t lhs, std::uint32_t rhs) const noexcept -> bool {
      ++count;
      return lhs < rhs;
    }
    mutable std::uint32_t count = 0;
  };

  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, counting_comparator>;

  imap map;
  ::testing::StaticAssertTypeEq<decltype(map.key_comp()), counting_comparator>();
  ASSERT_EQ(map.key_comp().count, 0);
  map[1] = "hello";
  ASSERT_EQ(map.key_comp().count, 0);
  map[2] = "goodbye";
  ASSERT_EQ(map.key_comp().count, 1);
}

/// @test The comparator supplied to `inline_map` does not have to be default-constructible, provided an instance is
/// supplied to the constructor. After constructing a map with such a comparator, values can be inserted and queried in
/// the map using `operator[]` with lookups returning the corresponding mapped values for the supplied keys.
TEST(InlineMap, CanUseNonDefaultConstructibleComparator) {
  struct counting_comparator {
    auto operator()(std::uint32_t lhs, std::uint32_t rhs) const noexcept -> bool {
      ++count;
      return lhs < rhs;
    }

    explicit counting_comparator(std::uint32_t initial_count)
        : count(initial_count) {}

    mutable std::uint32_t count = 0;
  };

  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, counting_comparator>;

  constexpr std::uint32_t initial = 42;
  imap map{counting_comparator{initial}};
  ::testing::StaticAssertTypeEq<decltype(map.key_comp()), counting_comparator>();
  ASSERT_EQ(map.key_comp().count, initial);
  map[1] = "hello";
  ASSERT_EQ(map.key_comp().count, initial);
  map[2] = "goodbye";
  ASSERT_EQ(map.key_comp().count, initial + 1);
  ASSERT_EQ(map[1], "hello");
  ASSERT_EQ(map[2], "goodbye");
}

/// @test The comparator supplied to `inline_map` can be declared `final` as well as being non-default-constructible.
/// After constructing a map with such a comparator, values can be inserted and queried in the map using `operator[]`
/// and `at`, with lookups returning the corresponding mapped values for the supplied keys. The stored comparator is
/// used for comparisons
TEST(InlineMap, CanUseFinalComparator) {
  struct counting_comparator final {
    auto operator()(std::uint32_t lhs, std::uint32_t rhs) const noexcept -> bool {
      ++count;
      return lhs < rhs;
    }

    explicit counting_comparator(std::uint32_t initial_count)
        : count(initial_count) {}

    mutable std::uint32_t count = 0;
  };

  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, counting_comparator>;

  constexpr std::uint32_t initial = 42;
  imap map{counting_comparator{initial}};
  ::testing::StaticAssertTypeEq<decltype(map.key_comp()), counting_comparator>();
  ASSERT_EQ(map.key_comp().count, initial);
  map[1] = "hello";
  ASSERT_EQ(map.key_comp().count, initial);
  map[2] = "goodbye";
  ASSERT_EQ(map.key_comp().count, initial + 1);
}

/// @test The comparator supplied to `inline_map` can be declared `final` when it is default-constructible.  After
/// constructing a map with such a comparator, values can be inserted and queried in the map using `operator[]` and
/// `at`, with lookups returning the corresponding mapped values for the supplied keys. The stored comparator is used
/// for comparisons
TEST(InlineMap, CanUseFinalDefaultConstructibleComparator) {
  struct counting_comparator final {
    auto operator()(std::uint32_t lhs, std::uint32_t rhs) const noexcept -> bool {
      ++count;
      return lhs < rhs;
    }

    mutable std::uint32_t count = 0;
  };

  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, counting_comparator>;

  imap map;
  ::testing::StaticAssertTypeEq<decltype(map.key_comp()), counting_comparator>();
  ASSERT_EQ(map.key_comp().count, 0);
  map[1] = "hello";
  ASSERT_EQ(map.key_comp().count, 0);
  map[2] = "goodbye";
  ASSERT_EQ(map.key_comp().count, 1);
}

namespace {
auto compare_unsigneds(std::uint32_t lhs, std::uint32_t rhs) -> bool { return lhs > rhs; }
}  // namespace

/// @test A function pointer can be used for the comparator. The order of keys returned from iteration reflects the
/// function supplied to the `inline_map` constructor
TEST(InlineMap, CanUseFunctionPointerComparator) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, bool (*)(std::uint32_t, std::uint32_t)>;

  imap map{compare_unsigneds};
  ::testing::StaticAssertTypeEq<decltype(map.key_comp()), bool (*)(std::uint32_t, std::uint32_t)>();
  map[1] = "hello";
  map[2] = "goodbye";
  ASSERT_TRUE(map.contains(2));
  ASSERT_FALSE(map.contains(3));
  ASSERT_EQ(map.begin()->first, 2);
}

struct nothrow_comparator {
  auto operator()(std::uint32_t, std::uint32_t) const -> bool;
};

/// @test The Default constructor for an `inline_map` is `noexcept` if and only if the comparator is
/// nothrow-default-constructible
TEST(InlineMap, DefaultConstructorIsNoExceptIfComparatorConstructorIs) {
  struct throwing_comparator {
    // NOLINTNEXTLINE(hicpp-use-equals-default)
    throwing_comparator() noexcept(false) {}
    auto operator()(std::uint32_t, std::uint32_t) const -> bool;
  };
  static_assert(std::is_nothrow_default_constructible<inline_map<std::uint32_t, int, 2>>::value, "Must be noexcept");
  static_assert(
      std::is_nothrow_default_constructible<inline_map<std::uint32_t, int, 2, nothrow_comparator>>::value,
      "Must be noexcept"
  );
  static_assert(
      !std::is_nothrow_default_constructible<inline_map<std::uint32_t, int, 2, throwing_comparator>>::value,
      "Must be noexcept"
  );
}

struct throwing_comparator {
  throwing_comparator() = default;
  // NOLINTNEXTLINE(hicpp-use-equals-default) explicitly throwing
  throwing_comparator(throwing_comparator const&) noexcept(false) {}
  // NOLINTNEXTLINE(hicpp-use-equals-default) explicitly throwing
  throwing_comparator(throwing_comparator&&) noexcept(false) {}
  // NOLINTNEXTLINE(hicpp-use-equals-default) explicitly throwing
  auto operator=(throwing_comparator const&) noexcept(false) -> throwing_comparator& { return *this; }
  auto operator=(throwing_comparator&&) -> throwing_comparator& = default;
  ~throwing_comparator() = default;
  auto operator()(std::uint32_t, std::uint32_t) const -> bool;
};

/// @test The copy constructor for an `inline_map` is `noexcept` if and only if the comparator is
/// nothrow-copy-constructible
TEST(InlineMap, CopyConstructorIsNoExceptIfComparatorConstructorIs) {
  static_assert(std::is_nothrow_copy_constructible<inline_map<std::uint32_t, int, 2>>::value, "Must be noexcept");
  static_assert(
      std::is_nothrow_copy_constructible<inline_map<std::uint32_t, int, 2, nothrow_comparator>>::value,
      "Must be noexcept"
  );
  static_assert(
      !std::is_nothrow_copy_constructible<inline_map<std::uint32_t, int, 2, throwing_comparator>>::value,
      "Must not be noexcept"
  );
}

/// @test The `try_construct` overload for copying an existing `inline_map` is `noexcept` if and only if the comparator
/// is nothrow-copy-constructible
TEST(InlineMap, CopyConstructorFactoryIsNoExceptIfComparatorConstructorIs) {
  static_assert(
      noexcept(inline_map<std::uint32_t, int, 2>::try_construct(std::declval<inline_map<std::uint32_t, int, 2> const&>()
      )),
      "Must be noexcept"
  );
  static_assert(
      noexcept(inline_map<std::uint32_t, int, 2, nothrow_comparator>::try_construct(
          std::declval<inline_map<std::uint32_t, int, 2, nothrow_comparator> const&>()
      )),
      "Must be noexcept"
  );
  static_assert(
      !noexcept(inline_map<std::uint32_t, int, 2, throwing_comparator>::try_construct(
          std::declval<inline_map<std::uint32_t, int, 2, throwing_comparator> const&>()
      )),
      "Must not be noexcept"
  );
}

/// @test The `try_construct` overload for copying an existing `inline_map` with a different capacity is `noexcept` if
/// and only if the comparator is nothrow-copy-constructible
TEST(InlineMap, ConstructorDifferentSizeFactoryIsNoExceptIfComparatorConstructorIs) {
  static_assert(
      noexcept(inline_map<std::uint32_t, int, 2>::try_construct(std::declval<inline_map<std::uint32_t, int, 3> const&>()
      )),
      "Must be noexcept"
  );
  static_assert(
      noexcept(inline_map<std::uint32_t, int, 2, nothrow_comparator>::try_construct(
          std::declval<inline_map<std::uint32_t, int, 3, nothrow_comparator> const&>()
      )),
      "Must be noexcept"
  );
  static_assert(
      !noexcept(inline_map<std::uint32_t, int, 2, throwing_comparator>::try_construct(
          std::declval<inline_map<std::uint32_t, int, 3, throwing_comparator> const&>()
      )),
      "Must not be noexcept"
  );
}

/// @test The `try_construct` overload for copying an existing `inline_map` with a different capacity is not `noexcept`
/// if the key and value types are not nothrow-copy-constructible
TEST(InlineMap, ConstructorDifferentSizeFactoryIsNotNoexceptIfKeyAndValueConstructorsAreNot) {
  using value_type = testing::configurable_value<std::int32_t, testing::throws_on::copy_construct>;

  static_assert(
      !noexcept(inline_map<std::uint32_t, value_type, 2>::try_construct(
          std::declval<inline_map<std::uint32_t, value_type, 3> const&>()
      )),
      "Must not be noexcept"
  );
  static_assert(
      !noexcept(inline_map<value_type, int, 2>::try_construct(std::declval<inline_map<value_type, int, 3> const&>())),
      "Must not be noexcept"
  );
}

/// @test The `try_construct` overload taking an rvalue `inline_map` is `noexcept`
/// if and only if the key and value types are nothrow-copy-constructible
TEST(InlineMap, MoveConstructorFactoryIsNoExceptIfComparatorConstructorIs) {
  static_assert(
      noexcept(inline_map<std::uint32_t, int, 2>::try_construct(std::declval<inline_map<std::uint32_t, int, 2>&&>())),
      "Must be noexcept"
  );
  static_assert(
      noexcept(inline_map<std::uint32_t, int, 2, nothrow_comparator>::try_construct(
          std::declval<inline_map<std::uint32_t, int, 2, nothrow_comparator>&&>()
      )),
      "Must be noexcept"
  );
  static_assert(
      !noexcept(inline_map<std::uint32_t, int, 2, throwing_comparator>::try_construct(
          std::declval<inline_map<std::uint32_t, int, 2, throwing_comparator>&&>()
      )),
      "Must not be noexcept"
  );
}

/// @test The `try_construct` overload taking an rvalue `inline_map` with a different capacity is `noexcept`
/// if and only if the copy constructors for the key and value types are nothrow-copy-constructible
TEST(InlineMap, MoveConstructorDifferentSizeFactoryIsNoExceptIfComparatorConstructorIs) {
  static_assert(
      noexcept(inline_map<std::uint32_t, int, 2>::try_construct(std::declval<inline_map<std::uint32_t, int, 3>&&>())),
      "Must be noexcept"
  );
  static_assert(
      noexcept(inline_map<std::uint32_t, int, 2, nothrow_comparator>::try_construct(
          std::declval<inline_map<std::uint32_t, int, 3, nothrow_comparator>&&>()
      )),
      "Must be noexcept"
  );
  static_assert(
      !noexcept(inline_map<std::uint32_t, int, 2, throwing_comparator>::try_construct(
          std::declval<inline_map<std::uint32_t, int, 3, throwing_comparator>&&>()
      )),
      "Must not be noexcept"
  );
}

/// @test The `try_construct` overload taking an rvalue `inline_map` with a different capacity is not `noexcept`
/// if the key and value types are not nothrow-move-constructible
TEST(InlineMap, MoveConstructorDifferentSizeFactoryIsNotExceptIfKeyAndValueConstructorsAreNot) {
  using throwing_move = testing::configurable_value<std::int32_t, testing::throws_on::move_construct>;
  using throwing_copy = testing::configurable_value<std::int32_t, testing::throws_on::copy_construct>;

  static_assert(
      !noexcept(inline_map<std::uint32_t, throwing_move, 2>::try_construct(
          std::declval<inline_map<std::uint32_t, throwing_move, 3>&&>()
      )),
      "Must not be noexcept"
  );
  static_assert(
      noexcept(inline_map<throwing_move, int, 2>::try_construct(std::declval<inline_map<throwing_move, int, 3>&&>())),
      "Must be noexcept: keys are copied not moved"
  );
  static_assert(
      !noexcept(inline_map<throwing_copy, int, 2>::try_construct(std::declval<inline_map<throwing_copy, int, 3>&&>())),
      "Must not be noexcept"
  );
}

/// @test The constructor that takes a comparator is `noexcept` if and only if the comparator is
/// nothrow-copy-constructible
TEST(InlineMap, ConstructorWithComparatorIsNoExceptIfComparatorConstructorIs) {
  static_assert(noexcept(inline_map<std::uint32_t, int, 2>{compare_three_way{}}), "Must be noexcept");
  static_assert(
      noexcept(inline_map<std::uint32_t, int, 2, nothrow_comparator>{nothrow_comparator{}}),
      "Must be noexcept"
  );
  static_assert(
      !noexcept(inline_map<std::uint32_t, int, 2, throwing_comparator>{throwing_comparator{}}),
      "Must be noexcept"
  );
}

/// @test Invoking `size` on a `const` reference to an `inline_map` returns the number of elements in the map
TEST(InlineMap, CanGetSizeOfConstMap) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  imap map;
  imap const& const_map = map;

  ASSERT_EQ(const_map.size(), 0);
  map[1] = "hello";
  ASSERT_EQ(const_map.size(), 1);
  map[2] = "goodbye";
  ASSERT_EQ(const_map.size(), 2);
}

/// @test The `size` member function is `noexcept`
TEST(InlineMap, SizeIsNoexcept) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  static_assert(noexcept(imap{}.size()), "Must be noexcept");
}

/// @test Invoking `empty` on a `const` reference to an `inline_map` returns true if and only if there are no
/// elements in the map.
TEST(InlineMap, EmptyWorksForConstMap) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  imap map;
  imap const& const_map = map;

  ASSERT_TRUE(const_map.empty());
  map[1] = "hello";
  ASSERT_FALSE(const_map.empty());
}

/// @test The `empty` member function is `noexcept`
TEST(InlineMap, EmptyIsNoexcept) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  static_assert(noexcept(imap{}.empty()), "Must be noexcept");
}

/// @test The `lower_bound` member function is `noexcept` if invoking the comparator is `noexcept`
TEST(InlineMap, LowerBoundIsNoexceptIfComparisonIs) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  static_assert(noexcept(std::declval<imap&>().lower_bound(32)), "Must be noexcept");
  static_assert(noexcept(std::declval<imap const&>().lower_bound(32)), "Must be noexcept");
}

/// @test The `lower_bound` member function is not `noexcept` if invoking the comparator can throw
TEST(InlineMap, LowerBoundIsNotNoexceptIfComparisonThrows) {
  struct compare {
    auto operator()(std::uint32_t, std::uint32_t) const -> bool { return false; }
  };
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, compare>;
  static_assert(!noexcept(std::declval<imap&>().lower_bound(32)), "Must be noexcept");
  static_assert(!noexcept(std::declval<imap const&>().lower_bound(32)), "Must be noexcept");
}

/// @test `begin` is `noexcept`
TEST(InlineMap, BeginIsNoexcept) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  static_assert(noexcept(std::declval<imap&>().begin()), "Must be noexcept");
  static_assert(noexcept(std::declval<imap const&>().begin()), "Must be noexcept");
  static_assert(noexcept(std::declval<imap&>().cbegin()), "Must be noexcept");
  static_assert(noexcept(std::declval<imap const&>().cbegin()), "Must be noexcept");
}

/// @test `end` is `noexcept`
TEST(InlineMap, EndIsNoexcept) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  static_assert(noexcept(std::declval<imap&>().end()), "Must be noexcept");
  static_assert(noexcept(std::declval<imap const&>().end()), "Must be noexcept");
  static_assert(noexcept(std::declval<imap&>().cend()), "Must be noexcept");
  static_assert(noexcept(std::declval<imap const&>().cend()), "Must be noexcept");
}

/// @test Invoking `insert` with an instance of the map's `value_type` increases the size of the map by one, and returns
/// a pair holding an iterator and a boolean where the boolean is true and the iterator is equal to that returned from
/// `find` when given the key from the `value_type`, and such that the mapped value referenced via this iterator is
/// equal to the mapped value of the source.
TEST(InlineMap, CanInsert) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  imap map{};

  imap::value_type const map_element{42U, "hello"};
  auto res = map.insert(map_element);

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(map_element.first));
  ASSERT_TRUE(res.second);
  ASSERT_EQ(res.first->second, map_element.second);
}

/// @test Invoking `insert` with an instance of the map's `value_type` where the key is already present leaves the map
/// unchanged, and returns a pair holding an iterator and a boolean where the boolean is false and the iterator is equal
/// to that returned from `find` when given the key from the `value_type`, and such that the mapped value referenced via
/// this iterator is equal to the original mapped value for that key.
TEST(InlineMap, InsertDoesNothingIfValuePresent) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  imap::value_type const map_element{42U, "hello"};
  test_string const original = "goodbye";
  imap map{{map_element.first, original}};

  auto res = map.insert(map_element);

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(map_element.first));
  ASSERT_FALSE(res.second);
  ASSERT_EQ(res.first->second, original);
}

/// @test Invoking `insert` with an rvalue instance of the map's `value_type`, where the mapped type is move-only,
/// increases the size of the map by one, and returns a pair holding an iterator and a boolean where the boolean is true
/// and the iterator is equal to that returned from `find` when given the key from the `value_type`, and such that the
/// mapped value referenced via this iterator is equal to the mapped value of the source.
TEST(InlineMap, CanInsertMoveOnlyValues) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, move_only_value, capacity>;
  imap map{};

  std::uint32_t const key = 42;
  imap::value_type map_element{key, move_only_value(123)};
  auto const value = *map_element.second.val;
  auto res = map.insert(std::move(map_element));
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  ASSERT_EQ(map_element.second.val, arene::base::nullopt);
  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(key));
  ASSERT_TRUE(res.second);
  ASSERT_EQ(*res.first->second.val, value);
}

/// @test Invoking `insert` with an rvalue instance of the map's `value_type`, where the mapped type is move-only, where
/// the key is already present leaves the map unchanged, and returns a pair holding an iterator and a boolean where the
/// boolean is false and the iterator is equal to that returned from `find` when given the key from the `value_type`,
/// and such that the mapped value referenced via this iterator is equal to the original mapped value for that key.
TEST(InlineMap, InsertDoesNothingIfValuePresentForMoveOnlyValues) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, move_only_value, capacity>;
  std::uint32_t const key = 42;
  imap::value_type map_element{key, move_only_value(123)};
  auto const value = map_element.second.val;
  auto original = move_only_value(456);
  auto const original_val = original.val;
  imap map;
  map[map_element.first] = std::move(original);

  auto res = map.insert(std::move(map_element));

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(key));
  ASSERT_FALSE(res.second);
  ASSERT_EQ(res.first->second.val, original_val);
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  ASSERT_EQ(map_element.first, key);
  ASSERT_EQ(map_element.second.val, value);
}

struct convertible_to_int {
  std::uint32_t val;
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  operator std::uint32_t() const noexcept { return val; }
};
struct user_defined_key {
  std::uint32_t val;
  explicit user_defined_key(std::uint32_t value) noexcept
      : val(value) {}

  auto operator<(user_defined_key const& rhs) const noexcept -> bool { return val < rhs.val; }
};

/// @test Invoking `emplace` with a value that can be used to construct the key, and a value that can be used to
/// construct the mapped type inserts an entry into the map with the corresponding key and value
TEST(InlineMap, CanEmplace) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<user_defined_key, move_only_value, capacity>;
  imap map{};

  std::uint32_t const key1 = 42;
  std::uint32_t const value1 = 123;

  auto res = map.emplace(convertible_to_int{key1}, convertible_to_int{value1});

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(user_defined_key{key1}));
  ASSERT_EQ(res.first->first.val, key1);
  ASSERT_EQ(*res.first->second.val, value1);
  ASSERT_TRUE(res.second);
}

/// @test Invoking `emplace` with a value that can be used to construct the key, and a value that can be used to
/// construct the mapped type leaves the map unchanged if the constructed key is already present in the map
TEST(InlineMap, EmplaceDoesNothingIfElementAlreadyPresent) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<user_defined_key, move_only_value, capacity>;
  imap map{};

  std::uint32_t const key1 = 42;
  std::uint32_t const value1 = 123;
  std::uint32_t const value2 = 456;

  map.insert({user_defined_key{key1}, move_only_value{value2}});

  auto res = map.emplace(convertible_to_int{key1}, convertible_to_int{value1});

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_FALSE(res.second);
  ASSERT_EQ(res.first, map.find(user_defined_key{key1}));
  ASSERT_EQ(res.first->first.val, key1);
  ASSERT_EQ(*res.first->second.val, value2);
}

/// @test Invoking `emplace` when the size of the map is already equal to the capacity, and the key value constructed
/// from the supplied arguments is already present in the map leaves the map unchanged and returns a pair holding an
/// iterator to the existing element with the specified key, and a boolean with the value false.
TEST(InlineMap, EmplaceDoesNotDieIfKeyIsPresent) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<user_defined_key, move_only_value, capacity>;
  imap map{};

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 99;
  std::uint32_t const value1 = 123;
  std::uint32_t const value2 = 456;
  std::uint32_t const value3 = 1;

  map.insert({user_defined_key{key1}, move_only_value{value1}});
  map.insert({user_defined_key{key2}, move_only_value{value2}});

  auto res = map.emplace(convertible_to_int{key1}, convertible_to_int{value3});

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 2);
  ASSERT_NE(map.find(user_defined_key{key1}), map.end());
  ASSERT_EQ(*map.find(user_defined_key{key1})->second.val, value1);
  ASSERT_NE(map.find(user_defined_key{key2}), map.end());
  ASSERT_EQ(*map.find(user_defined_key{key2})->second.val, value2);
  ASSERT_EQ(res.first, map.find(user_defined_key{key1}));
  ASSERT_FALSE(res.second);
}

/// @test If `emplace` is invoked with a key that is already present in the map, then any temporary instances of the
/// mapped type created for the emplacement are destroyed before the call returns.
TEST(InlineMap, EmplaceDoesNotLeaveActiveObjectsOnFailure) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, counted, capacity>;
  imap map{};

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 99;

  map[key1] = {};
  map[key2] = {};

  auto res = map.emplace(key1, counted{});
  ASSERT_FALSE(res.second);
  ASSERT_EQ(map.size(), 2);
  ASSERT_EQ(counted::instances, map.size());
}

/// @test The `max_size` member function returns the capacity of the map.
TEST(InlineMap, GetMaxSize) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, counted, capacity>;
  imap const map;
  ASSERT_EQ(map.max_size(), capacity);
}

/// @test The `max_size` member function is `noexcept`
TEST(InlineMap, GetMaxSizeIsNoexcept) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, counted, capacity>;
  static_assert(noexcept(imap{}.max_size()), "Must be noexcept");
}

/// @test The `max_size` member function returns the capacity of the map.
TEST(InlineMap, GetMaxSizeForDifferentCapacity) {
  constexpr std::uint32_t capacity = 22;
  using imap = inline_map<std::uint32_t, counted, capacity>;
  imap const map;
  ASSERT_EQ(map.max_size(), capacity);
}

/// @test Invoking `emplace` with a first argument of `std::piecewise_construct` and two tuples of values inserts a new
/// element in the map with a key constructed from the values in the first typle, and a mapped value constructed from
/// the values in the second tuple, such that the entry can be retrieved by invoking `find` with an
/// equivalently-constructed key
TEST(InlineMap, CanEmplaceWithComplexArgs) {
  struct key {
    std::uint32_t val;
    explicit key(std::uint32_t first, std::uint32_t second) noexcept
        : val(first + second) {}

    auto operator<(key const& rhs) const noexcept -> bool { return val < rhs.val; }
  };

  struct value {
    test_string val;
    explicit value(std::uint32_t first, test_string const& second)
        : val(arene::base::to_inline_string(first) + second) {}
  };

  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<key, value, capacity>;
  imap map{};

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 42;
  std::uint32_t const value1 = 123;
  test_string const value2 = "hello";

  auto res = map.emplace(std::piecewise_construct, std::make_tuple(key1, key2), std::make_tuple(value1, value2));

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(key{key1, key2}));
  ASSERT_EQ(res.first->first.val, key1 + key2);
  ASSERT_EQ(res.first->second.val, "123hello");
  ASSERT_TRUE(res.second);
}

/// @test Invoking `try_emplace` with a key that is not already present in the map, and constructor parameters for the
/// mapped value, inserts an element into the map with the corresponding key and the mapped value constructed from the
/// supplied parameters, and returns an iterator to that element and a boolean with the value `true`.
TEST(InlineMap, CanTryEmplace) {
  struct value {
    test_string val;
    explicit value(std::uint32_t first, test_string const& second)
        : val(arene::base::to_inline_string(first) + second) {}
  };

  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, value, capacity>;
  imap map{};

  std::uint32_t const key1 = 42;
  std::uint32_t const value1 = 123;
  test_string const value2 = "hello";

  auto res = map.try_emplace(key1, value1, value2);

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(key1));
  ASSERT_EQ(res.first->first, key1);
  ASSERT_EQ(res.first->second.val, "123hello");
  ASSERT_TRUE(res.second);
}

struct move_only_key {
  std::uint32_t val;
  explicit move_only_key(std::uint32_t key)
      : val(key) {}
  move_only_key(move_only_key&&) = default;
  move_only_key(move_only_key const&) = delete;
  auto operator=(move_only_key&&) -> move_only_key& = default;
  auto operator=(move_only_key const&) -> move_only_key& = delete;
  ~move_only_key() = default;

  auto operator<(move_only_key const& rhs) const -> bool { return val < rhs.val; }
};
struct composite_value {
  test_string val;
  explicit composite_value(std::uint32_t first, test_string const& second)
      : val(arene::base::to_inline_string(first) + second) {}
};

/// @test Invoking `try_emplace` on an `inline_map` with a move-only key type, with a rvalue key that is not already
/// present in the map, and constructor parameters for the mapped value, inserts an element into the map with the
/// corresponding key and the mapped value constructed from the supplied parameters, and returns an iterator to that
/// element and a boolean with the value `true`. Subsequently invoking `try_emplace` on an `inline_map` with a
/// move-only key type, with a rvalue key with the same value, and constructor parameters for the mapped value, leaves
/// the map unchanged and returns an iterator to the existing value and a boolean with the value `false`.
TEST(InlineMap, CanTryEmplaceWithMoveOnlyKey) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<move_only_key, composite_value, capacity>;
  imap map{};

  std::uint32_t const key1 = 42;
  std::uint32_t const value1 = 123;
  test_string const value2 = "hello";

  auto res = map.try_emplace(move_only_key{key1}, value1, value2);

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(move_only_key{key1}));
  ASSERT_EQ(res.first->first.val, key1);
  ASSERT_EQ(res.first->second.val, "123hello");
  ASSERT_TRUE(res.second);

  std::uint32_t const value3 = value1 + 1;
  test_string const value4 = value2 + " world";

  res = map.try_emplace(move_only_key{key1}, value3, value4);

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(move_only_key{key1}));
  ASSERT_EQ(res.first->first.val, key1);
  ASSERT_EQ(res.first->second.val, "123hello");
  ASSERT_FALSE(res.second);
}

/// @test Invoking `insert` on an `inline_map` with a move-only key type, with a rvalue key that is not already present
/// in the map, and a mapped value, inserts an element into the map with the corresponding key and mapped value, and
/// returns an iterator to that element and a boolean with the value `true`.
TEST(InlineMap, CanInsertWithMoveOnlyKey) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<move_only_key, std::uint32_t, capacity>;
  imap map{};

  std::uint32_t const key1 = 42;
  std::uint32_t const value1 = 123;

  auto res = map.insert(std::pair<move_only_key, std::uint32_t>{move_only_key{key1}, value1});

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(move_only_key{key1}));
  ASSERT_EQ(res.first->first.val, key1);
  ASSERT_EQ(res.first->second, value1);
  ASSERT_TRUE(res.second);
}

/// @test Invoking `insert_or_assign` with a key and value where the key is not already present in the map inserts the
/// value into the map, and returns an iterator to that element and a boolean with the value `true`.
TEST(InlineMap, InsertOrAssignInsertsIfNoElement) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  imap map{};

  std::uint32_t const key = 42;
  char const* const value = "hello";
  auto res = map.insert_or_assign(key, value);

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(key));
  ASSERT_TRUE(res.second);
  ASSERT_EQ(res.first->second, value);
}

/// @test Invoking `insert_or_assign` with a key and value where the key is already present in the map assigns the
/// new value to the corresponding element in the map, and returns an iterator to that element and a boolean with the
/// value `false`.
TEST(InlineMap, InsertOrAssignAssignsIfAlreadyThere) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  imap map{};

  std::uint32_t const key = 42;
  char const* const value = "hello";

  map[key] = "goodbye";

  auto res = map.insert_or_assign(key, value);

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(key));
  ASSERT_FALSE(res.second);
  ASSERT_EQ(res.first->second, value);
}

/// @test Invoking `insert_or_assign` on an `inline_map` with a move-only key type, with an rvalue key and value where
/// the key is not already present in the map inserts the value into the map, and returns an iterator to that element
/// and a boolean with the value `true`.
TEST(InlineMap, InsertOrAssignInsertsIfNoElementWithMoveOnlyKey) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<move_only_key, test_string, capacity>;
  imap map{};

  std::uint32_t const keyval = 42;
  char const* const value = "hello";
  auto res = map.insert_or_assign(move_only_key{keyval}, value);

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(move_only_key{keyval}));
  ASSERT_TRUE(res.second);
  ASSERT_EQ(res.first->second, value);
}

/// @test Invoking `insert_or_assign` on an `inline_map` with a move-only key type, with an rvalue key and value, where
/// there is already an equivalent key in the map, assigns the new value to the corresponding element in the map, and
/// returns an iterator to that element and a boolean with the value `false`.
TEST(InlineMap, InsertOrAssignAssignsIfAlreadyThereWithMoveOnlyKey) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<move_only_key, test_string, capacity>;
  imap map{};

  std::uint32_t const keyval = 42;
  char const* const value = "hello";

  map.emplace(move_only_key{keyval}, "goodbye");

  auto res = map.insert_or_assign(move_only_key{keyval}, value);

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(move_only_key{keyval}));
  ASSERT_FALSE(res.second);
  ASSERT_EQ(res.first->second, value);
}

/// @test Invoking `insert_or_assign` on an `inline_map` with a move-only mapped type, with a key and rvalue mapped
/// value, where the key is not already present in the map inserts a new element into the map with the mapped value
/// move-constructed from the supplied value, and returns an iterator to that element and a boolean with the value
/// `true`.
TEST(InlineMap, InsertOrAssignWorksForMoveOnlyValues) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, move_only_value, capacity>;
  imap map{};

  std::uint32_t const key = 42;
  std::int32_t const value = 123;

  map[key] = move_only_value(456);

  auto res = map.insert_or_assign(key, move_only_value(value));

  ASSERT_FALSE(map.empty());
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(res.first, map.find(key));
  ASSERT_FALSE(res.second);
  ASSERT_EQ(res.first->second.val, value);
}

/// @test Invoking `clear` on an `inline_map` with existing elements sets the size to zero, and destroys all contained
/// elements.
TEST(InlineMap, ClearRemovesAllElements) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, counted, capacity>;
  imap map{};

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 99;

  map[key1] = {};
  map[key2] = {};

  map.clear();
  ASSERT_TRUE(map.empty());
  ASSERT_EQ(counted::instances, 0);
  static_assert(noexcept(std::declval<imap&>().clear()), "Must be noexcept");
}

/// @test The overload of `erase` with a single iterator is `noexcept`
TEST(InlineMap, IteratorBasedEraseIsNoExcept) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;
  static_assert(noexcept(std::declval<imap&>().erase(std::declval<imap&>().begin())), "Must be noexcept");
  static_assert(noexcept(std::declval<imap&>().erase(std::declval<imap&>().cbegin())), "Must be noexcept");
  static_assert(
      noexcept(std::declval<imap&>().erase(std::declval<imap&>().begin(), std::declval<imap&>().end())),
      "Must be noexcept"
  );
}

/// @test The overload of `erase` with a key is `noexcept` if invoking the comparator is `noexcept`
TEST(InlineMap, EraseByKeyIsNoexceptIfComparatorIs) {
  using imap = inline_map<std::uint32_t, std::uint32_t, 10>;
  static_assert(noexcept(std::declval<imap&>().erase(42)), "Must be noexcept");
}

/// @test The overload of `erase` with a key is not `noexcept` if invoking the comparator is not `noexcept`
TEST(InlineMap, EraseByKeyIsNotNoexceptIfComparatorIsNot) {
  struct throwing_less {
    auto operator<(throwing_less const&) const -> bool { return false; }
  };
  using imap = inline_map<throwing_less, std::uint32_t, 10>;
  static_assert(!noexcept(std::declval<imap&>().erase(throwing_less{})), "Must not be noexcept");
}

/// @test `upper_bound` returns an iterator referring to the first element with a key that is greater than the given
/// key, or an iterator equal to the iterator returned by `end` if there is no such element.
TEST(InlineMap, UpperBound) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;

  constexpr imap::key_type count = 15;
  constexpr int value = -123;

  for (imap::key_type idx = 0; idx < count; ++idx) {
    map[idx * 2] = value;
  }

  for (imap::key_type idx = 0; idx < count; ++idx) {
    ASSERT_EQ(map.upper_bound(idx * 2), next(map.begin(), idx + 1));
    ASSERT_EQ(map.upper_bound(idx * 2 + 1), next(map.begin(), idx + 1));
  }

  ASSERT_EQ(map.upper_bound(count * 2), map.end());
  ASSERT_EQ(map.upper_bound(count * 4), map.end());
}

/// @test `upper_bound` on a `const` map returns a `const_iterator` referring to the first element with a key that is
/// greater than the given key, or an iterator equal to the iterator returned by `end` if there is no such element.
TEST(InlineMap, UpperBoundForConstMap) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  imap const& const_map = map;

  constexpr imap::key_type count = 15;
  constexpr int value = -123;

  for (imap::key_type idx = 0; idx < count; ++idx) {
    map[idx * 2] = value;
  }

  for (imap::key_type idx = 0; idx < count; ++idx) {
    ASSERT_EQ(const_map.upper_bound(idx * 2), next(const_map.begin(), idx + 1));
    ASSERT_EQ(const_map.upper_bound(idx * 2 + 1), next(const_map.begin(), idx + 1));
  }

  ASSERT_EQ(const_map.upper_bound(count * 2), const_map.end());
  ASSERT_EQ(const_map.upper_bound(count * 4), const_map.end());
}

/// @test `upper_bound` is `noexcept` if invoking the comparator is `noexcept`
TEST(InlineMap, UpperBoundIsNoexceptIfComparisonIs) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  static_assert(noexcept(std::declval<imap&>().upper_bound(32)), "Must be noexcept");
  static_assert(noexcept(std::declval<imap const&>().upper_bound(32)), "Must be noexcept");
}

/// @test `upper_bound` is not `noexcept` if invoking the comparator is not `noexcept`
TEST(InlineMap, UpperBoundIsNotNoexceptIfComparisonThrows) {
  struct compare {
    auto operator()(std::uint32_t, std::uint32_t) const -> bool { return false; }
  };
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, compare>;
  static_assert(!noexcept(std::declval<imap&>().upper_bound(32)), "Must be noexcept");
  static_assert(!noexcept(std::declval<imap const&>().upper_bound(32)), "Must be noexcept");
}

/// @test Invoking `equal_range` with a key that is present in the map returns a pair of iterators, the first of which
/// refers to the element with the given key, and the second of which refers to the following element. Invoking
/// `equal_range` with a key that is not in the map returns a pair of iterators, both of which are equal, and which
/// refer to the first element with a key greater than the provided key, or are equal to the iterator returned from
/// `end` if there is no such element.
TEST(InlineMap, EqualRange) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;

  constexpr imap::key_type count = 15;
  constexpr int value = -123;

  for (imap::key_type idx = 0; idx < count; ++idx) {
    map[idx * 2] = value;
  }

  for (imap::key_type idx = 0; idx < count; ++idx) {
    auto present_range = map.equal_range(idx * 2);
    ::testing::StaticAssertTypeEq<decltype(present_range), std::pair<imap::iterator, imap::iterator>>();

    ASSERT_EQ(present_range.first, next(map.begin(), idx));
    ASSERT_EQ(present_range.second, next(map.begin(), idx + 1));
    auto missing_range = map.equal_range(idx * 2 + 1);
    ASSERT_EQ(missing_range.first, map.end());
    ASSERT_EQ(missing_range.second, map.end());
  }
  auto missing_range = map.equal_range(count * 3);
  ASSERT_EQ(missing_range.first, map.end());
  ASSERT_EQ(missing_range.second, map.end());
}

/// @test Invoking `equal_range` on a `const` map with a key that is present in the map returns a pair of iterators, the
/// first of which refers to the element with the given key, and the second of which refers to the following
/// element. Invoking `equal_range` with a key that is not in the map returns a pair of iterators, both of which are
/// equal, and which refer to the first element with a key greater than the provided key, or are equal to the iterator
/// returned from `end` if there is no such element.
TEST(InlineMap, EqualRangeForConstMap) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;
  imap const& const_map = map;

  constexpr imap::key_type count = 15;
  constexpr int value = -123;

  for (imap::key_type idx = 0; idx < count; ++idx) {
    map[idx * 2] = value;
  }

  for (imap::key_type idx = 0; idx < count; ++idx) {
    auto const present_range = const_map.equal_range(idx * 2);
    ::testing::StaticAssertTypeEq<decltype(present_range), std::pair<imap::const_iterator, imap::const_iterator> const>(
    );

    ASSERT_EQ(present_range.first, next(const_map.begin(), idx));
    ASSERT_EQ(present_range.second, next(const_map.begin(), idx + 1));
    auto const missing_range = const_map.equal_range(idx * 2 + 1);
    ASSERT_EQ(missing_range.first, const_map.end());
    ASSERT_EQ(missing_range.second, const_map.end());
  }
  auto const missing_range = const_map.equal_range(count * 3);
  ASSERT_EQ(missing_range.first, const_map.end());
  ASSERT_EQ(missing_range.second, const_map.end());
}

/// @test `equal_range` is `noexcept` if invoking the comparator is `noexcept`
TEST(InlineMap, EqualRangeIsNoexceptIfComparisonIs) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  static_assert(noexcept(std::declval<imap&>().equal_range(32)), "Must be noexcept");
  static_assert(noexcept(std::declval<imap const&>().equal_range(32)), "Must be noexcept");
}

/// @test `equal_range` is not `noexcept` if invoking the comparator is not `noexcept`
TEST(InlineMap, EqualRangeIsNotNoexceptIfComparisonThrows) {
  struct compare {
    auto operator()(std::uint32_t, std::uint32_t) const -> bool { return false; }
  };
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, test_string, capacity, compare>;
  static_assert(!noexcept(std::declval<imap&>().equal_range(32)), "Must be noexcept");
  static_assert(!noexcept(std::declval<imap const&>().equal_range(32)), "Must be noexcept");
}

/// @test Copy construction of an `inline_map` creates a second map with the same size and contents.
TEST(InlineMap, CanCopyConstructMaps) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  imap const map1{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  imap const map2{map1};

  ASSERT_EQ(map1.size(), map2.size());

  for (auto const& element : map1) {
    auto pos = map2.find(element.first);
    ASSERT_NE(pos, map2.end());
    ASSERT_EQ(arene::base::distance(map2.begin(), pos), arene::base::distance(map1.begin(), map1.find(element.first)));
    ASSERT_EQ(pos->first, element.first);
    ASSERT_EQ(pos->second, element.second);
  }
}

/// @test Invoking `try_construct` with an existing `inline_map` returns an `optional` holding an `inline_map` with the
/// same size and contents.
TEST(InlineMap, CanCopyConstructMapsViaFactory) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  imap const map1{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  auto result = imap::try_construct(map1);

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap>>();
  ASSERT_TRUE(result.has_value());

  imap& map2 = *result;

  ASSERT_EQ(map1.size(), map2.size());

  for (auto const& element : map1) {
    auto pos = map2.find(element.first);
    ASSERT_NE(pos, map2.end());
    ASSERT_EQ(arene::base::distance(map2.begin(), pos), arene::base::distance(map1.begin(), map1.find(element.first)));
    ASSERT_EQ(pos->first, element.first);
    ASSERT_EQ(pos->second, element.second);
  }
}

/// @test Invoking `try_construct` with an rvalue `inline_map` returns an `optional` holding an `inline_map` with the
/// same size and contents.
TEST(InlineMap, CanMoveConstructMapsViaFactory) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  imap const refmap{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  imap map1{refmap};
  auto result = imap::try_construct(std::move(map1));

  ::testing::StaticAssertTypeEq<decltype(result), arene::base::optional<imap>>();
  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_TRUE(map1.empty());

  imap& map2 = *result;

  ASSERT_EQ(refmap.size(), map2.size());

  for (auto const& element : refmap) {
    auto pos = map2.find(element.first);
    ASSERT_NE(pos, map2.end());
    ASSERT_EQ(
        arene::base::distance(map2.begin(), pos),
        arene::base::distance(refmap.begin(), refmap.find(element.first))
    );
    ASSERT_EQ(pos->first, element.first);
    ASSERT_EQ(pos->second, element.second);
  }
}

/// @test Constructing an `inline_map` from an existing `inline_map` with a different capacity initializes the map to
/// have the same size and contents as the source map.
TEST(InlineMap, CanCopyConstructMapsWithDifferentCapacities) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 5U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap const map1{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  imap2 const map2{map1};

  ASSERT_EQ(map1.size(), map2.size());

  for (auto const& element : map1) {
    auto pos = map2.find(element.first);
    ASSERT_NE(pos, map2.end());
    ASSERT_EQ(arene::base::distance(map2.begin(), pos), arene::base::distance(map1.begin(), map1.find(element.first)));
    ASSERT_EQ(pos->first, element.first);
    ASSERT_EQ(pos->second, element.second);
  }
}

/// @test Invoking `try_construct` with an existing `inline_map` of a different capacity returns an `optional` holding
/// an `inline_map` with the same size and contents.
TEST(InlineMap, CanCopyConstructMapsWithDifferentCapacitiesViaFactory) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 5U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap const map1{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  auto const result = imap2::try_construct(map1);

  ASSERT_TRUE(result.has_value());
  imap2 const& map2 = *result;

  ASSERT_EQ(map1.size(), map2.size());

  for (auto const& element : map1) {
    auto pos = map2.find(element.first);
    ASSERT_NE(pos, map2.end());
    ASSERT_EQ(arene::base::distance(map2.begin(), pos), arene::base::distance(map1.begin(), map1.find(element.first)));
    ASSERT_EQ(pos->first, element.first);
    ASSERT_EQ(pos->second, element.second);
  }
}

/// @test Invoking `try_construct` with an existing `inline_map` of a larger capacity and more elements than the
/// capacity of the destination type returns an empty `optional`
TEST(InlineMapDeathTests, CopyConstructingSmallerFromLargerViaFactoryIfSizeTooBigReturnsNullopt) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 3U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap const map1{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  auto const result = imap2::try_construct(map1);
  ASSERT_FALSE(result.has_value());
}

/// @test Invoking `try_construct` with an rvalue `inline_map` of a different capacity returns an `optional` holding an
/// `inline_map` with the same size and contents.
TEST(InlineMap, CanMoveConstructMapsWithDifferentCapacitiesViaFactory) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 5U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap const refmap{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  imap map1{refmap};
  auto const result = imap2::try_construct(std::move(map1));

  ASSERT_TRUE(result.has_value());
  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_TRUE(map1.empty());
  imap2 const& map2 = *result;

  ASSERT_EQ(refmap.size(), map2.size());

  for (auto const& element : refmap) {
    auto pos = map2.find(element.first);
    ASSERT_NE(pos, map2.end());
    ASSERT_EQ(
        arene::base::distance(map2.begin(), pos),
        arene::base::distance(refmap.begin(), refmap.find(element.first))
    );
    ASSERT_EQ(pos->first, element.first);
    ASSERT_EQ(pos->second, element.second);
  }
}

/// @test Invoking `try_construct` with an rvalue `inline_map` of a larger capacity and more elements than the
/// capacity of the destination type returns an empty `optional`
TEST(InlineMap, MoveConstructingSmallerFromLargerViaFactoryIfSizeTooBigReturnsNullopt) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 3U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap const refmap{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  imap map1{refmap};
  auto const result = imap2::try_construct(std::move(map1));
  ASSERT_FALSE(result.has_value());
  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_FALSE(map1.empty());
}

/// @test `inline_map` is not copy-constructible if the mapped type is not copy-constructible
TEST(InlineMap, CannotCopyMapWithMoveOnlyElements) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, move_only_value, capacity>;
  static_assert(!std::is_copy_constructible<imap>::value, "Not copyable");
}

/// @test `inline_map` is move-constructible if the mapped type is move-constructible but not copy-constructible; moving
/// the map moves the mapped values, and leaves the source map empty
TEST(InlineMap, CanMoveMapWithMoveOnlyelements) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, move_only_value, capacity>;
  static_assert(std::is_nothrow_move_constructible<imap>::value, "Should be movable");

  std::uint32_t const key1 = 42;
  std::int32_t const value1 = 11111;
  std::uint32_t const key2 = 99;
  std::int32_t const value2 = 22222;
  std::uint32_t const key3 = 103;
  std::int32_t const value3 = 33333;
  std::uint32_t const key4 = 2;
  std::int32_t const value4 = 44444;
  std::uint32_t const key5 = 12345;
  std::int32_t const value5 = 55555;
  imap map1;
  map1.emplace(key1, move_only_value(value1));
  map1.emplace(key2, move_only_value(value2));
  map1.emplace(key3, move_only_value(value3));
  map1.emplace(key4, move_only_value(value4));
  map1.emplace(key5, move_only_value(value5));
  imap const map2{std::move(map1)};
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved,clang-analyzer-cplusplus.Move)
  ASSERT_EQ(map1.size(), 0);
  ASSERT_EQ(map2.size(), 5);
  ASSERT_NE(map2.find(key1), map2.end());
  ASSERT_EQ(map2.find(key1)->second.val, value1);
  ASSERT_NE(map2.find(key2), map2.end());
  ASSERT_EQ(map2.find(key2)->second.val, value2);
  ASSERT_NE(map2.find(key3), map2.end());
  ASSERT_EQ(map2.find(key3)->second.val, value3);
  ASSERT_NE(map2.find(key4), map2.end());
  ASSERT_EQ(map2.find(key4)->second.val, value4);
  ASSERT_NE(map2.find(key5), map2.end());
  ASSERT_EQ(map2.find(key5)->second.val, value5);
}

/// @test Constructing an `inline_map` holding a mapped type that is move-constructible but not copy-constructible
/// from an rvalue map with the same data types and a different capacity  moves the mapped values, and leaves the source
/// map empty
TEST(InlineMap, CanMoveMapWithMoveOnlyElementsWithDifferentCapacity) {
  constexpr auto capacity = 10U;
  using imap1 = inline_map<std::uint32_t, move_only_value, capacity>;
  constexpr auto capacity2 = 5U;
  using imap2 = inline_map<std::uint32_t, move_only_value, capacity2>;

  std::uint32_t const key1 = 42;
  std::int32_t const value1 = 11111;
  std::uint32_t const key2 = 99;
  std::int32_t const value2 = 22222;
  std::uint32_t const key3 = 103;
  std::int32_t const value3 = 33333;
  std::uint32_t const key4 = 2;
  std::int32_t const value4 = 44444;
  std::uint32_t const key5 = 12345;
  std::int32_t const value5 = 55555;
  imap1 map1;
  map1.emplace(key1, move_only_value(value1));
  map1.emplace(key2, move_only_value(value2));
  map1.emplace(key3, move_only_value(value3));
  map1.emplace(key4, move_only_value(value4));
  map1.emplace(key5, move_only_value(value5));
  imap2 map2{std::move(map1)};
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved,clang-analyzer-cplusplus.Move)
  ASSERT_EQ(map1.size(), 0);
  ASSERT_EQ(map2.size(), 5);
  ASSERT_NE(map2.find(key1), map2.end());
  ASSERT_EQ(map2.find(key1)->second.val, value1);
  ASSERT_NE(map2.find(key2), map2.end());
  ASSERT_EQ(map2.find(key2)->second.val, value2);
  ASSERT_NE(map2.find(key3), map2.end());
  ASSERT_EQ(map2.find(key3)->second.val, value3);
  ASSERT_NE(map2.find(key4), map2.end());
  ASSERT_EQ(map2.find(key4)->second.val, value4);
  ASSERT_NE(map2.find(key5), map2.end());
  ASSERT_EQ(map2.find(key5)->second.val, value5);
}

/// @test Assigning to an `inline_map` from another `inline_map` leaves the destination map with the same size and set
/// of key-value pairs as the source map.
TEST(InlineMap, CanCopyAssignMaps) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  imap const map1{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  imap map2{{1U, "other"}, {2U, "foo"}};
  map2 = map1;

  ASSERT_EQ(map1.size(), map2.size());

  for (auto const& element : map1) {
    auto pos = map2.find(element.first);
    ASSERT_NE(pos, map2.end());
    ASSERT_EQ(arene::base::distance(map2.begin(), pos), arene::base::distance(map1.begin(), map1.find(element.first)));
    ASSERT_EQ(pos->first, element.first);
    ASSERT_EQ(pos->second, element.second);
  }
}

/// @test Assigning to an `inline_map` from another `inline_map` with the same data types and a different capacity
/// leaves the destination map with the same size and set of key-value pairs as the source map.
TEST(InlineMap, CanCopyAssignMapsWithDifferentCapacities) {
  constexpr auto capacity = 10U;
  using imap1 = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 5U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap1 const map1{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  imap2 map2{{1U, "other"}, {2U, "foo"}};
  map2 = map1;

  ASSERT_EQ(map1.size(), map2.size());

  for (auto const& element : map1) {
    auto pos = map2.find(element.first);
    ASSERT_NE(pos, map2.end());
    ASSERT_EQ(arene::base::distance(map2.begin(), pos), arene::base::distance(map1.begin(), map1.find(element.first)));
    ASSERT_EQ(pos->first, element.first);
    ASSERT_EQ(pos->second, element.second);
  }
}

/// @test Assigning to an `inline_map` with existing elements from a source map with smaller size reduces the size of
/// the destination to that of the source map, so the destination has the same size and key/value pairs as the source
/// map, where the source capacity is larger than the destination capacity
TEST(InlineMap, CopyAssignmentFromSmallerSizeOverLargerSizeTrimsTheEnd) {
  constexpr auto capacity = 10U;
  using imap1 = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 5U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap1 const map1{{42U, "hello"}};
  imap2 map2{{99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  map2 = map1;

  ASSERT_EQ(map1.size(), map2.size());

  for (auto const& element : map1) {
    auto pos = map2.find(element.first);
    ASSERT_NE(pos, map2.end());
    ASSERT_EQ(arene::base::distance(map2.begin(), pos), arene::base::distance(map1.begin(), map1.find(element.first)));
    ASSERT_EQ(pos->first, element.first);
    ASSERT_EQ(pos->second, element.second);
  }
}

/// @test Assigning to an `inline_map` with existing elements from a source map with no elements destroys all the
/// elements in the destination, so the maps are both empty
TEST(InlineMap, CopyAssignmentFromEmptyOverNonEmptyTrimsTheEnd) {
  constexpr auto capacity = 10U;
  using imap1 = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 3U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap1 const map1{};
  imap2 map2{{99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}};
  map2 = map1;

  ASSERT_EQ(map1.size(), map2.size());
  ASSERT_TRUE(map2.empty());
}

/// @test Assigning to an `inline_map` with existing elements from a source map with smaller size reduces the size of
/// the destination to that of the source map, so the destination has the same size and key/value pairs as the source
/// map, where the source capacity is smaller than the destination capacity
TEST(InlineMap, CopyAssignmentFromSmallerToLargerCapacityTrimsTheEnd) {
  constexpr auto capacity = 5U;
  using imap1 = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 10U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap1 const map1{{42U, "hello"}};
  imap2 map2{{99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  map2 = map1;

  ASSERT_EQ(map1.size(), map2.size());

  for (auto const& element : map1) {
    auto pos = map2.find(element.first);
    ASSERT_NE(pos, map2.end());
    ASSERT_EQ(arene::base::distance(map2.begin(), pos), arene::base::distance(map1.begin(), map1.find(element.first)));
    ASSERT_EQ(pos->first, element.first);
    ASSERT_EQ(pos->second, element.second);
  }
}

/// @test An `inline_map` with a move-only mapped type is not copy-assignable
TEST(InlineMap, CannotCopyAssignMapWithMoveOnlyElements) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, move_only_value, capacity>;
  static_assert(!std::is_copy_assignable<imap>::value, "Not copyable");
}

/// @test An `inline_map` with a move-only mapped type is move-assignable; assigning to an `inline_map` from an
/// rvalue moves the elements, so the destination has the same size, keys and values as the source did prior to the
/// move, and then clears all elements from the source.
TEST(InlineMap, CanMoveAssignMapWithMoveOnlyelements) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, move_only_value, capacity>;
  static_assert(std::is_nothrow_move_assignable<imap>::value, "Should be movable");

  std::uint32_t const key1 = 42;
  std::int32_t const value1 = 11111;
  std::uint32_t const key2 = 99;
  std::int32_t const value2 = 22222;
  std::uint32_t const key3 = 103;
  std::int32_t const value3 = 33333;
  std::uint32_t const key4 = 2;
  std::int32_t const value4 = 44444;
  std::uint32_t const key5 = 12345;
  std::int32_t const value5 = 55555;
  imap map1;
  map1.emplace(key1, move_only_value(value1));
  map1.emplace(key2, move_only_value(value2));
  map1.emplace(key3, move_only_value(value3));
  map1.emplace(key4, move_only_value(value4));
  map1.emplace(key5, move_only_value(value5));
  imap map2;
  map2 = std::move(map1);
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved,clang-analyzer-cplusplus.Move)
  ASSERT_EQ(map1.size(), 0);
  ASSERT_EQ(map2.size(), 5);
  ASSERT_NE(map2.find(key1), map2.end());
  ASSERT_EQ(map2.find(key1)->second.val, value1);
  ASSERT_NE(map2.find(key2), map2.end());
  ASSERT_EQ(map2.find(key2)->second.val, value2);
  ASSERT_NE(map2.find(key3), map2.end());
  ASSERT_EQ(map2.find(key3)->second.val, value3);
  ASSERT_NE(map2.find(key4), map2.end());
  ASSERT_EQ(map2.find(key4)->second.val, value4);
  ASSERT_NE(map2.find(key5), map2.end());
  ASSERT_EQ(map2.find(key5)->second.val, value5);
}

/// @test Assigning to an `inline_map` from an rvalue with a larger capacity moves the elements, so the destination has
/// the same size, keys and values as the source did prior to the move, and then clears all elements from the source.
TEST(InlineMap, CanMoveAssignMapWithMoveOnlyelementsAndDifferentCapacity) {
  constexpr auto capacity = 10U;
  using imap1 = inline_map<std::uint32_t, move_only_value, capacity>;
  constexpr auto capacity2 = 5U;
  using imap2 = inline_map<std::uint32_t, move_only_value, capacity2>;

  std::uint32_t const key1 = 42;
  std::int32_t const value1 = 11111;
  std::uint32_t const key2 = 99;
  std::int32_t const value2 = 22222;
  std::uint32_t const key3 = 103;
  std::int32_t const value3 = 33333;
  std::uint32_t const key4 = 2;
  std::int32_t const value4 = 44444;
  std::uint32_t const key5 = 12345;
  std::int32_t const value5 = 55555;
  imap1 map1;
  map1.emplace(key1, move_only_value(value1));
  map1.emplace(key2, move_only_value(value2));
  map1.emplace(key3, move_only_value(value3));
  map1.emplace(key4, move_only_value(value4));
  map1.emplace(key5, move_only_value(value5));
  imap2 map2;
  map2.emplace(102030U, move_only_value(66666));
  map2 = std::move(map1);
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved,clang-analyzer-cplusplus.Move)
  ASSERT_EQ(map1.size(), 0);
  ASSERT_EQ(map2.size(), 5);
  ASSERT_NE(map2.find(key1), map2.end());
  ASSERT_EQ(map2.find(key1)->second.val, value1);
  ASSERT_NE(map2.find(key2), map2.end());
  ASSERT_EQ(map2.find(key2)->second.val, value2);
  ASSERT_NE(map2.find(key3), map2.end());
  ASSERT_EQ(map2.find(key3)->second.val, value3);
  ASSERT_NE(map2.find(key4), map2.end());
  ASSERT_EQ(map2.find(key4)->second.val, value4);
  ASSERT_NE(map2.find(key5), map2.end());
  ASSERT_EQ(map2.find(key5)->second.val, value5);
}

/// @test Iterating through a `const` `inline_map` using the iterators returned from `rbegin` and `rend` or `crbegin`
/// and `crend` visits each of the element in the reverse of the sorted order
TEST(InlineMap, ReverseIterationAndConst) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  // NOLINTNEXTLINE(misc-const-correctness) explicitly attempting to test for non-cost vs. const behavior.
  imap map{{1U, 1}, {2U, 2}, {3U, 3}};
  imap const& const_map = map;

  ::testing::StaticAssertTypeEq<decltype(const_map.rbegin()), imap::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_map.rend()), imap::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<::arene::base::reverse_iterator<imap::const_iterator>, imap::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(map.crbegin()), imap::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(map.crend()), imap::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_map.crbegin()), imap::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<decltype(const_map.crend()), imap::const_reverse_iterator>();

  ASSERT_EQ(const_map.rbegin(), map.crbegin());
  ASSERT_EQ(const_map.rend(), map.crend());
  ASSERT_EQ(const_map.rbegin().base(), const_map.end());
  ASSERT_EQ(const_map.rend().base(), const_map.begin());
}

struct non_copyable_non_movable {
  non_copyable_non_movable() = default;
  non_copyable_non_movable(non_copyable_non_movable const&) = delete;
  non_copyable_non_movable(non_copyable_non_movable&&) = delete;
  auto operator=(non_copyable_non_movable const&) -> non_copyable_non_movable& = delete;
  auto operator=(non_copyable_non_movable&&) -> non_copyable_non_movable& = delete;
  ~non_copyable_non_movable() = default;

  auto operator<(non_copyable_non_movable const&) const noexcept -> bool { return false; }
  template <typename T>
  auto operator()(T const&, T const&) const noexcept -> bool {
    return false;
  }
};

struct move_only_cmp {
  move_only_cmp() = default;
  move_only_cmp(move_only_cmp const&) = delete;
  move_only_cmp(move_only_cmp&&) = default;
  auto operator=(move_only_cmp const&) -> move_only_cmp& = delete;
  auto operator=(move_only_cmp&&) -> move_only_cmp& = default;
  ~move_only_cmp() = default;

  template <typename T>
  auto operator()(T const&, T const&) const noexcept -> bool {
    return false;
  }
};

/// @test `inline_map` is not copy-constructible if the key is not copy-constructible
TEST(InlineMap, NotCopyConstructibleIfKeyIsNot) {
  using imap = inline_map<move_only_key, std::uint32_t, 3>;
  static_assert(!std::is_copy_constructible<imap>::value, "Not copy constructible");
}

/// @test `inline_map` is not copy-constructible if the mapped value is not copy-constructible
TEST(InlineMap, NotCopyConstructibleIfValueIsNot) {
  using imap = inline_map<std::uint32_t, move_only_value, 3>;
  static_assert(!std::is_copy_constructible<imap>::value, "Not copy constructible");
}

/// @test `inline_map` is not copy-constructible if the comparator is not copy-constructible
TEST(InlineMap, NotCopyConstructibleIfComparatorIsNot) {
  using imap = inline_map<std::uint32_t, std::uint32_t, 3, move_only_cmp>;
  static_assert(!std::is_copy_constructible<imap>::value, "Not copy constructible");
}

/// @test `inline_map` is not move-constructible if the key is not copy-constructible
TEST(InlineMap, NotMoveConstructibleIfKeyIsNot) {
  using imap = inline_map<non_copyable_non_movable, std::uint32_t, 3>;
  static_assert(!std::is_move_constructible<imap>::value, "Not move constructible");
}

/// @test `inline_map` is not move-constructible if the mapped value is not move-constructible
TEST(InlineMap, NotMoveConstructibleIfValueIsNot) {
  using imap = inline_map<std::uint32_t, non_copyable_non_movable, 3>;
  static_assert(!std::is_move_constructible<imap>::value, "Not move constructible");
}

/// @test `inline_map` is not move-constructible if the comparator is not copy-constructible
TEST(InlineMap, NotMoveConstructibleIfComparatorIsNot) {
  using imap = inline_map<std::uint32_t, std::uint32_t, 3, non_copyable_non_movable>;
  static_assert(!std::is_move_constructible<imap>::value, "Not move constructible");
}

/// @test `inline_map` is not copy-assignable if the key is not copy-assignable
TEST(InlineMap, NotCopyAssignableIfKeyIsNot) {
  using imap = inline_map<move_only_key, std::uint32_t, 3>;
  static_assert(!std::is_copy_assignable<imap>::value, "Not copy assignable");
}

/// @test `inline_map` is not copy-assignable if the mapped value is not copy-assignable
TEST(InlineMap, NotCopyAssignableIfValueIsNot) {
  using imap = inline_map<std::uint32_t, move_only_value, 3>;
  static_assert(!std::is_copy_assignable<imap>::value, "Not copy assignable");
}

/// @test `inline_map` is not copy-assignable if the comparator is not copy-assignable
TEST(InlineMap, NotCopyAssignableIfComparatorIsNot) {
  using imap = inline_map<std::uint32_t, std::uint32_t, 3, move_only_cmp>;
  static_assert(!std::is_copy_assignable<imap>::value, "Not copy assignable");
}

/// @test `inline_map` is not move-assignable if the key is not copy-assignable
TEST(InlineMap, NotMoveAssignableIfKeyIsNot) {
  using imap = inline_map<non_copyable_non_movable, std::uint32_t, 3>;
  static_assert(!std::is_move_assignable<imap>::value, "Not move assignable");
}

/// @test `inline_map` is not move-assignable if the mapped value is not move-assignable
TEST(InlineMap, NotMoveAssignableIfValueIsNot) {
  using imap = inline_map<std::uint32_t, non_copyable_non_movable, 3>;
  static_assert(!std::is_move_assignable<imap>::value, "Not move assignable");
}

/// @test `inline_map` is not move-assignable if the comparator is not move-assignable
TEST(InlineMap, NotMoveAssignableIfComparatorIsNot) {
  using imap = inline_map<std::uint32_t, std::uint32_t, 3, non_copyable_non_movable>;
  static_assert(!std::is_move_assignable<imap>::value, "Not move assignable");
}

using map_test::const_and_non_const_transparent_imaps;
using map_test::explicit_conversion_to_key;
using map_test::InlineMapTransparentComparisonEquivalencyTest;

TYPED_TEST_SUITE(InlineMapTransparentComparisonEquivalencyTest, const_and_non_const_transparent_imaps, );

/// @test `find` returns the same result when supplied a value that is comparable to the key type as it does when
/// supplied a key that has the same comparison results
TYPED_TEST(InlineMapTransparentComparisonEquivalencyTest, Find) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.find(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.find(std::declval<typename TestFixture::imap::key_type>()))>();
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

/// @test `contains` returns the same result when supplied a value that is comparable to the key type as it does when
/// supplied a key that has the same comparison results
TYPED_TEST(InlineMapTransparentComparisonEquivalencyTest, Contains) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.contains(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.contains(std::declval<typename TestFixture::imap::key_type>()))>();
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

/// @test `count` returns the same result when supplied a value that is comparable to the key type as it does when
/// supplied a key that has the same comparison results
TYPED_TEST(InlineMapTransparentComparisonEquivalencyTest, Count) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.count(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.count(std::declval<typename TestFixture::imap::key_type>()))>();
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

/// @test `lower_bound` returns the same result when supplied a value that is comparable to the key type as it does when
/// supplied a key that has the same comparison results
TYPED_TEST(InlineMapTransparentComparisonEquivalencyTest, LowerBound) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.lower_bound(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.lower_bound(std::declval<typename TestFixture::imap::key_type>()))>();
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

/// @test `upper_bound` returns the same result when supplied a value that is comparable to the key type as it does when
/// supplied a key that has the same comparison results
TYPED_TEST(InlineMapTransparentComparisonEquivalencyTest, UpperBound) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.upper_bound(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.upper_bound(std::declval<typename TestFixture::imap::key_type>()))>();
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

/// @test `equal_range` returns the same result when supplied a value that is comparable to the key type as it does when
/// supplied a key that has the same comparison results
TYPED_TEST(InlineMapTransparentComparisonEquivalencyTest, EqualRange) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.equal_range(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.equal_range(std::declval<typename TestFixture::imap::key_type>()))>();
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

using map_test::InlineMapTransparentComparisonMutatingEquivalencyTest;

/// @test `operator[]` returns the same result when supplied a value that is comparable to the key type as it does when
/// supplied a key that has the same comparison results
TEST_F(InlineMapTransparentComparisonMutatingEquivalencyTest, Index) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.operator[](std::declval<comparable_not_convertible_to_key>())),
      decltype(this->values.operator[](std::declval<imap::key_type>()))>();

  {
    SCOPED_TRACE("key not in container");
    for (auto const& inserted_key :
         {this->key_less_than_container, this->key_greater_than_container, this->key_gap_in_container}) {
      this->values[comparable_not_convertible_to_key{inserted_key, explicit_conversion_to_key::allowed}] =
          ++inserted_value;
      auto const expected_inserted_itr = this->values.find(inserted_key);
      ASSERT_NE(expected_inserted_itr, this->values.end());
      EXPECT_EQ(expected_inserted_itr->second, inserted_value);
    }
  }
  {
    SCOPED_TRACE("key  in container");
    auto const existing_itr = this->values.find(this->key_in_container);
    ASSERT_NE(existing_itr, this->values.end());
    auto const& existing_value = existing_itr->second;
    EXPECT_EQ(
        (this->values
             [comparable_not_convertible_to_key{this->key_in_container, explicit_conversion_to_key::not_allowed}]),
        existing_value
    );
  }
}

/// @test `insert_or_assign` returns the same result and has the same behaviour when supplied a value that is comparable
/// and convertible to the key type as it does when supplied a key that has the same comparison results
TEST_F(InlineMapTransparentComparisonMutatingEquivalencyTest, InsertOrAssign) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.insert_or_assign(
          std::declval<comparable_not_convertible_to_key>(),
          std::declval<imap::mapped_type>()
      )),
      decltype(this->values.insert_or_assign(std::declval<imap::key_type>(), std::declval<imap::mapped_type>()))>();
  {
    SCOPED_TRACE("key not in container");
    for (auto const& inserted_key :
         {this->key_less_than_container, this->key_greater_than_container, this->key_gap_in_container}) {
      auto const insert_result = this->values.insert_or_assign(
          comparable_not_convertible_to_key{inserted_key, explicit_conversion_to_key::allowed},
          ++inserted_value
      );
      EXPECT_TRUE(insert_result.second);
      EXPECT_EQ(insert_result.first->second, inserted_value);
    }
  }

  {
    SCOPED_TRACE("key  in container");
    auto const insert_result = this->values.insert_or_assign(
        comparable_not_convertible_to_key{this->key_in_container, explicit_conversion_to_key::not_allowed},
        ++inserted_value
    );
    EXPECT_FALSE(insert_result.second);
    EXPECT_EQ(insert_result.first->second, inserted_value);
  }
}

/// @test `try_emplace` returns the same result and has the same behaviour when supplied a value that is comparable
/// and convertible to the key type as it does when supplied a key that has the same comparison results
TEST_F(InlineMapTransparentComparisonMutatingEquivalencyTest, TryEmplace) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values
                   .try_emplace(std::declval<comparable_not_convertible_to_key>(), std::declval<imap::mapped_type>())),
      decltype(this->values.try_emplace(std::declval<imap::key_type>(), std::declval<imap::mapped_type>()))>();
  {
    SCOPED_TRACE("key not in container");
    for (auto const& inserted_key :
         {this->key_less_than_container, this->key_greater_than_container, this->key_gap_in_container}) {
      auto const insert_result = this->values.try_emplace(
          comparable_not_convertible_to_key{inserted_key, explicit_conversion_to_key::allowed},
          ++inserted_value
      );
      EXPECT_TRUE(insert_result.second);
      EXPECT_EQ(insert_result.first->second, inserted_value);
    }
  }

  {
    SCOPED_TRACE("key  in container");
    auto const existing_itr = this->values.find(this->key_in_container);
    ASSERT_NE(existing_itr, this->values.end());
    auto const& existing_value = existing_itr->second;
    auto const insert_result = this->values.try_emplace(
        comparable_not_convertible_to_key{this->key_in_container, explicit_conversion_to_key::not_allowed},
        ++inserted_value
    );
    EXPECT_FALSE(insert_result.second);
    EXPECT_EQ(insert_result.first->second, existing_value);
  }
}

/// @test `erase` returns the same result and has the same behaviour when supplied a value that is comparable
/// and convertible to the key type as it does when supplied a key that has the same comparison results
TEST_F(InlineMapTransparentComparisonMutatingEquivalencyTest, Erase) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.erase(std::declval<comparable_not_convertible_to_key>())),
      decltype(this->values.erase(std::declval<imap::key_type>()))>();
  auto const original_size = this->values.size();
  {
    SCOPED_TRACE("key not in container");
    for (auto const& erased_key :
         {this->key_less_than_container, this->key_greater_than_container, this->key_gap_in_container}) {
      EXPECT_EQ(
          this->values.erase(comparable_not_convertible_to_key{erased_key, explicit_conversion_to_key::not_allowed}),
          this->values.erase(erased_key)
      );
      EXPECT_EQ(values.size(), original_size);
    }
  }

  {
    SCOPED_TRACE("key in container");
    EXPECT_EQ(
        this->values.erase(
            comparable_not_convertible_to_key{this->key_in_container, explicit_conversion_to_key::not_allowed}
        ),
        this->values.lower_bound(this->key_in_container)
    );
    EXPECT_EQ(values.size(), original_size - 1);
  }
}

template <typename T>
class InlineMapTransparentComparisonSignedUnsignedIntegerInsertionTest : public ::testing::Test {
 public:
  using imap = inline_map<T, int, 10, compare_three_way>;
  imap map;
};

using unsigned_integer_types = arene::base::type_lists::remove_duplicates_t<
    ::testing::Types<unsigned char, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>>;

TYPED_TEST_SUITE(InlineMapTransparentComparisonSignedUnsignedIntegerInsertionTest, unsigned_integer_types, );

/// @test Invoking `insert_or_assign` for an `inline_map` with an unsigned integral key type, with a positive signed
/// value that is in range as the key correctly inserts the value, however triggers a compilation error when
/// `-Wconversion` or `-Wsignedconversion` is enabled, so an explicit cast is needed.
/// `-Wconversion` will not trigger for implicit conversions if the compiler can prove the value is non-negative and
/// non-narrowing. It cannot do that for non-const values.
TYPED_TEST(
    InlineMapTransparentComparisonSignedUnsignedIntegerInsertionTest,
    InsertOrAssignASignedValueWhichIsNotStaticallyProvablyPositiveRequiresCasting
) {
  // NOLINTBEGIN(misc-const-correctness)
  char charval = 1;
  std::int8_t i8val = 2;
  std::int16_t i16val = 3;
  std::int32_t i32val = 4;
  std::int64_t i64val = 5;
  // NOLINTEND(misc-const-correctness)
  // uncommenting these should be a compiler error when compiled with -Wconversion or -WsignedConversion
  // this->map.insert_or_assign(charval, 1);
  // this->map.insert_or_assign(i8val, 1);
  // this->map.insert_or_assign(i16val, 1);
  // this->map.insert_or_assign(i32val, 1);
  // this->map.insert_or_assign(i64val, 1);
  // this->map.insert_or_assign(-1, 1);
  using key_type = typename TestFixture::imap::key_type;
  EXPECT_TRUE(this->map.insert_or_assign(static_cast<key_type>(charval), 1).second);
  EXPECT_TRUE(this->map.insert_or_assign(static_cast<key_type>(i8val), 1).second);
  EXPECT_TRUE(this->map.insert_or_assign(static_cast<key_type>(i16val), 1).second);
  EXPECT_TRUE(this->map.insert_or_assign(static_cast<key_type>(i32val), 1).second);
  EXPECT_TRUE(this->map.insert_or_assign(static_cast<key_type>(i64val), 1).second);
}

/// @test Invoking `insert_or_assign` for an `inline_map` with an unsigned integral key type, with a positive signed
/// value that is in range as the key correctly inserts the value. This is OK, even with `-Wconversion` enabled, if the
/// argument is `const`.
/// `-Wconversion` will not trigger for implicit conversions if the compiler can prove the value is non-negative and
/// non-narrowing. It can do that for const locals, and literals.
TYPED_TEST(
    InlineMapTransparentComparisonSignedUnsignedIntegerInsertionTest,
    InsertOrAssignASignedValueWhichIsStaticallyProvablyPositiveDoesNotRequireCasting
) {
  char const charval{1};
  std::int8_t const i8val{2};
  std::int16_t const i16val{3};
  std::int32_t const i32val{4};
  std::int64_t const i64val{5};

  EXPECT_TRUE(this->map.insert_or_assign(charval, 1).second);
  EXPECT_TRUE(this->map.insert_or_assign(i8val, 1).second);
  EXPECT_TRUE(this->map.insert_or_assign(i16val, 1).second);
  EXPECT_TRUE(this->map.insert_or_assign(i32val, 1).second);
  EXPECT_TRUE(this->map.insert_or_assign(i64val, 1).second);

  EXPECT_TRUE(this->map.insert_or_assign(6, 1).second);
  EXPECT_TRUE(this->map.insert_or_assign(7L, 1).second);
  EXPECT_TRUE(this->map.insert_or_assign(8LL, 1).second);
}

/// @test Invoking `emplace` for an `inline_map` with an unsigned integral key type, with a positive signed
/// value that is in range as the key correctly inserts the value, however triggers a compilation error when
/// `-Wconversion` or `-Wsignedconversion` is enabled, so an explicit cast is needed.
/// `-Wconversion` will not trigger for implicit conversions if the compiler can prove the value is non-negative and
/// non-narrowing. It cannot do that for non-const values.
TYPED_TEST(
    InlineMapTransparentComparisonSignedUnsignedIntegerInsertionTest,
    EmplaceASignedValueWhichIsNotStaticallyProvablyPositiveRequiresCasting
) {
  // NOLINTBEGIN(misc-const-correctness)
  char charval = 1;
  std::int8_t i8val = 2;
  std::int16_t i16val = 3;
  std::int32_t i32val = 4;
  std::int64_t i64val = 5;
  // NOLINTEND(misc-const-correctness)
  // uncommenting these should be a compiler error when compiled with -Wconversion or -WsignedConversion
  // this->map.emplace(charval, 1);
  // this->map.emplace(i8val, 1);
  // this->map.emplace(i16val, 1);
  // this->map.emplace(i32val, 1);
  // this->map.emplace(i64val, 1);
  // this->map.emplace(-1, 1);
  using key_type = typename TestFixture::imap::key_type;
  EXPECT_TRUE(this->map.emplace(static_cast<key_type>(charval), 1).second);
  EXPECT_TRUE(this->map.emplace(static_cast<key_type>(i8val), 1).second);
  EXPECT_TRUE(this->map.emplace(static_cast<key_type>(i16val), 1).second);
  EXPECT_TRUE(this->map.emplace(static_cast<key_type>(i32val), 1).second);
  EXPECT_TRUE(this->map.emplace(static_cast<key_type>(i64val), 1).second);
}

/// @test Invoking `try_emplace` for an `inline_map` with an unsigned integral key type, with a positive signed
/// value that is in range as the key correctly inserts the value, however triggers a compilation error when
/// `-Wconversion` or `-Wsignedconversion` is enabled, so an explicit cast is needed.
/// `-Wconversion` will not trigger for implicit conversions if the compiler can prove the value is non-negative and
/// non-narrowing. It cannot do that for non-const values.
TYPED_TEST(
    InlineMapTransparentComparisonSignedUnsignedIntegerInsertionTest,
    TryEmplaceASignedValueWhichIsNotStaticallyProvablyPositiveRequiresCasting
) {
  // NOLINTBEGIN(misc-const-correctness)
  char charval = 1;
  std::int8_t i8val = 2;
  std::int16_t i16val = 3;
  std::int32_t i32val = 4;
  std::int64_t i64val = 5;
  // NOLINTEND(misc-const-correctness)
  // uncommenting these should be a compiler error when compiled with -Wconversion or -WsignedConversion
  // this->map.try_emplace(charval, 1);
  // this->map.try_emplace(i8val, 1);
  // this->map.try_emplace(i16val, 1);
  // this->map.try_emplace(i32val, 1);
  // this->map.try_emplace(i64val, 1);
  // this->map.try_emplace(-1, 1);
  using key_type = typename TestFixture::imap::key_type;
  EXPECT_TRUE(this->map.try_emplace(static_cast<key_type>(charval), 1).second);
  EXPECT_TRUE(this->map.try_emplace(static_cast<key_type>(i8val), 1).second);
  EXPECT_TRUE(this->map.try_emplace(static_cast<key_type>(i16val), 1).second);
  EXPECT_TRUE(this->map.try_emplace(static_cast<key_type>(i32val), 1).second);
  EXPECT_TRUE(this->map.try_emplace(static_cast<key_type>(i64val), 1).second);
}

/// @test Invoking `try_emplace` for an `inline_map` with an unsigned integral key type, with a positive signed
/// value that is in range as the key correctly inserts the value. This is OK, even with `-Wconversion` enabled, if the
/// argument is `const`.
/// `-Wconversion` will not trigger for implicit conversions if the compiler can prove the value is non-negative and
/// non-narrowing. It can do that for const locals, and literals.
TYPED_TEST(
    InlineMapTransparentComparisonSignedUnsignedIntegerInsertionTest,
    TryEmplaceASignedValueWhichIsStaticallyProvablyPositiveDoesNotRequireCasting
) {
  // Wconversion will not trigger for implicit conversions if the compiler can prove the value is non-negative and
  // non-narrowing. It can do that for const locals, and literals.
  char const charval{1};
  std::int8_t const i8val{2};
  std::int16_t const i16val{3};
  std::int32_t const i32val{4};
  std::int64_t const i64val{5};

  EXPECT_TRUE(this->map.try_emplace(charval, 1).second);
  EXPECT_TRUE(this->map.try_emplace(i8val, 1).second);
  EXPECT_TRUE(this->map.try_emplace(i16val, 1).second);
  EXPECT_TRUE(this->map.try_emplace(i32val, 1).second);
  EXPECT_TRUE(this->map.try_emplace(i64val, 1).second);

  EXPECT_TRUE(this->map.try_emplace(6, 1).second);
  EXPECT_TRUE(this->map.try_emplace(7L, 1).second);
  EXPECT_TRUE(this->map.try_emplace(8LL, 1).second);
}

template <typename MappedType>
class InlineMapComparisonTest : public ::testing::Test {
 public:
  template <std::size_t Capacity>
  using imap = inline_map<std::uint32_t, MappedType, Capacity>;
  using standard_imap = imap<3>;
  using large_imap = imap<20>;

  standard_imap const s_empty{};
  large_imap const l_empty{};
};

class only_equality {
 public:
  int value;

  constexpr auto operator==(only_equality const& rhs) const -> bool { return value == rhs.value; }
};

using three_way_and_not_three_way_types = ::testing::Types<int, only_equality, testing::has_broken_less_than>;

template <typename MappedType>
using InlineMapEqualityComparisonTest = InlineMapComparisonTest<MappedType>;

TYPED_TEST_SUITE(InlineMapEqualityComparisonTest, three_way_and_not_three_way_types, );

/// @test Maps with different sizes compare unequal, even if the initial values are identical, and with a mix of
/// capacities.
TYPED_TEST(InlineMapEqualityComparisonTest, MapsOfDifferentSizesCompareUnequal) {
  typename TestFixture::standard_imap const s_values{{1U, TypeParam{-1}}, {2U, TypeParam{-2}}};
  typename TestFixture::standard_imap const s_other_values{{1U, TypeParam{-1}}};

  typename TestFixture::large_imap const l_values{s_values};
  typename TestFixture::large_imap const l_other_values{s_other_values};

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

/// @test Maps with the same size compare unequal, if the values are different, and with a mix of capacities.
TYPED_TEST(InlineMapEqualityComparisonTest, MapsOfSameSizesButDifferentElementsCompareUnequal) {
  typename TestFixture::standard_imap const s_values{{1U, TypeParam{-1}}, {2U, TypeParam{-2}}};
  typename TestFixture::standard_imap const s_changed_key{{3U, TypeParam{-1}}, {2U, TypeParam{-2}}};
  typename TestFixture::standard_imap const s_changed_value{{1U, TypeParam{-1}}, {2U, TypeParam{-4}}};

  typename TestFixture::large_imap const l_values{s_values};
  typename TestFixture::large_imap const l_changed_key{s_changed_key};
  typename TestFixture::large_imap const l_changed_value{s_changed_value};

  EXPECT_NE(s_values, s_changed_key);
  EXPECT_NE(s_values, s_changed_value);
  EXPECT_NE(l_values, l_changed_key);
  EXPECT_NE(l_values, l_changed_value);
  EXPECT_NE(s_values, l_changed_key);
  EXPECT_NE(s_values, l_changed_value);
  EXPECT_NE(l_values, s_changed_key);
  EXPECT_NE(l_values, s_changed_value);

  EXPECT_FALSE(s_values == s_changed_key);
  EXPECT_FALSE(s_values == s_changed_value);
  EXPECT_FALSE(l_values == l_changed_key);
  EXPECT_FALSE(l_values == l_changed_value);
  EXPECT_FALSE(s_values == l_changed_key);
  EXPECT_FALSE(s_values == l_changed_value);
  EXPECT_FALSE(l_values == s_changed_key);
  EXPECT_FALSE(l_values == s_changed_value);
}

/// @test Maps with the same size and values compare equal, even with a mix of capacities.
TYPED_TEST(InlineMapEqualityComparisonTest, MapsOfSameElementsCompareEqual) {
  typename TestFixture::standard_imap const s_values{{1U, TypeParam{-1}}, {2U, TypeParam{-2}}};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization) explicitly want a copy here
  typename TestFixture::standard_imap const s_same_values{s_values};

  typename TestFixture::large_imap const l_values{s_values};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization) explicitly want a copy here
  typename TestFixture::large_imap const l_same_values{l_values};

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

template <typename MappedType>
using InlineMapRelationalComparisonTest = InlineMapComparisonTest<MappedType>;

using comparison_mapped_types = ::testing::Types<std::uint32_t, std::int32_t>;

TYPED_TEST_SUITE(InlineMapRelationalComparisonTest, comparison_mapped_types, );

/// @test Maps with different elements are lexicographically ordered based on the contents, ignoring the capacities.
TYPED_TEST(InlineMapRelationalComparisonTest, RelationalComparisonsAreLexicographicalOrderings) {
  auto const extra_element = std::make_pair(3U, TypeParam{10});
  typename TestFixture::standard_imap const s_values{{2U, TypeParam{20}}, {1U, TypeParam{10}}};
  typename TestFixture::standard_imap const s_lesser_value{{2U, TypeParam{20}}, {1U, TypeParam{1}}};
  typename TestFixture::standard_imap const s_greater_value{{2U, TypeParam{200}}, {1U, TypeParam{10}}};
  typename TestFixture::standard_imap const s_lesser_key{{2U, TypeParam{20}}, {0U, TypeParam{10}}};
  typename TestFixture::standard_imap const s_greater_key{{2U, TypeParam{20}}, {3U, TypeParam{10}}};
  typename TestFixture::standard_imap s_more_elements{s_values};
  s_more_elements.insert(extra_element);
  typename TestFixture::standard_imap s_fewer_elements{s_values};
  s_fewer_elements.erase(--s_fewer_elements.end());

  typename TestFixture::large_imap const l_values{s_values};
  typename TestFixture::large_imap const l_same_values{s_values};
  typename TestFixture::large_imap const l_lesser_value{{2U, TypeParam{20}}, {1U, TypeParam{1}}};
  typename TestFixture::large_imap const l_greater_value{{2U, TypeParam{200}}, {1U, TypeParam{10}}};
  typename TestFixture::large_imap const l_lesser_key{{2U, TypeParam{20}}, {0U, TypeParam{10}}};
  typename TestFixture::large_imap const l_greater_key{{2U, TypeParam{20}}, {3U, TypeParam{10}}};
  typename TestFixture::large_imap l_more_elements{l_values};
  l_more_elements.insert(extra_element);
  typename TestFixture::large_imap l_fewer_elements{l_values};
  l_fewer_elements.erase(--l_fewer_elements.end());

  ASSERT_LT(s_values, s_greater_key);
  ASSERT_LT(s_values, s_greater_key);
  ASSERT_LT(s_lesser_key, s_values);
  ASSERT_LT(s_lesser_value, s_values);
  ASSERT_LT(s_values, s_more_elements);
  ASSERT_LT(s_fewer_elements, s_values);

  ASSERT_LE(s_values, s_greater_key);
  ASSERT_LE(s_values, s_greater_value);
  ASSERT_LE(s_lesser_key, s_values);
  ASSERT_LE(s_lesser_value, s_values);
  ASSERT_LE(s_values, s_more_elements);
  ASSERT_LE(s_fewer_elements, s_values);
  ASSERT_LE(s_values, s_values);

  ASSERT_GT(s_greater_key, s_values);
  ASSERT_GT(s_greater_value, s_values);
  ASSERT_GT(s_values, s_lesser_key);
  ASSERT_GT(s_values, s_lesser_value);
  ASSERT_GT(s_more_elements, s_values);
  ASSERT_GT(s_values, s_fewer_elements);

  ASSERT_GE(s_greater_key, s_values);
  ASSERT_GE(s_greater_value, s_values);
  ASSERT_GE(s_values, s_lesser_key);
  ASSERT_GE(s_values, s_lesser_value);
  ASSERT_GE(s_more_elements, s_values);
  ASSERT_GE(s_values, s_fewer_elements);

  ASSERT_LT(s_values, l_greater_key);
  ASSERT_LT(s_values, l_greater_key);
  ASSERT_LT(s_lesser_key, l_values);
  ASSERT_LT(s_lesser_value, l_values);
  ASSERT_LT(s_values, l_more_elements);
  ASSERT_LT(s_fewer_elements, l_values);

  ASSERT_LE(s_values, l_greater_key);
  ASSERT_LE(s_values, l_greater_value);
  ASSERT_LE(s_lesser_key, l_values);
  ASSERT_LE(s_lesser_value, l_values);
  ASSERT_LE(s_values, l_more_elements);
  ASSERT_LE(s_fewer_elements, l_values);
  ASSERT_LE(s_values, l_values);

  ASSERT_GT(s_greater_key, l_values);
  ASSERT_GT(s_greater_value, l_values);
  ASSERT_GT(s_values, l_lesser_key);
  ASSERT_GT(s_values, l_lesser_value);
  ASSERT_GT(s_more_elements, l_values);
  ASSERT_GT(s_values, l_fewer_elements);

  ASSERT_GE(s_greater_key, l_values);
  ASSERT_GE(s_greater_value, l_values);
  ASSERT_GE(s_values, l_lesser_key);
  ASSERT_GE(s_values, l_lesser_value);
  ASSERT_GE(s_more_elements, l_values);
  ASSERT_GE(s_values, s_fewer_elements);
  ASSERT_GE(s_values, l_values);
}

}  // namespace
