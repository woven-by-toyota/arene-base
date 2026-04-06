// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/algorithm/equal.hpp"
#include "arene/base/algorithm/find.hpp"
#include "arene/base/inline_container/map.hpp"
#include "arene/base/inline_container/tests/map_test_helpers.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/reverse.hpp"
#include "arene/base/stdlib_choice/sort.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"

namespace {

using ::arene::base::inline_map;
using ::map_test::test_string;

/// @test If `at` is invoked with a key that is not present in the map, then a `std::out_of_range` exception is thrown
TEST(InlineMap, IfElementNotPresentAtThrows) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  imap map{{1U, "hello"}};
  ASSERT_THROW(static_cast<void>(map.at(2)), std::out_of_range);
}

/// @test If `at` is invoked on a `const` `inline_map` with a key that is not present in the map, then a
/// `std::out_of_range` exception is thrown
TEST(InlineMap, IfElementNotPresentAtThrowsForConstMap) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  imap const map{{1U, "hello"}};
  ASSERT_THROW(static_cast<void>(map.at(2)), std::out_of_range);
}

/// @test If `at` is invoked on a non-`const` map with a key that is present in the map, then a reference to the
/// corresponding mapped type is returned.
TEST(InlineMap, IfElementPresentAtReturnsReferenceToValue) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  std::uint32_t const key = 42;
  imap map{{key, "hello"}};
  ASSERT_EQ(&map.at(key), &map.begin()->second);
}

/// @test If `at` is invoked on a `const` map with a key that is present in the map, then a reference to the
/// corresponding mapped type is returned.
TEST(InlineMap, IfElementPresentAtReturnsReferenceToValueForConstMap) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  std::uint32_t const key = 42;
  imap const map{{key, "hello"}};
  ASSERT_EQ(&map.at(key), &map.begin()->second);
}

/// @test For a map with multiple elements, starting with the iterator returned from `begin`, incrementing the iterator
/// a number of times equal to the number of elements in the map makes the iterator refer to each element once, and ends
/// with the iterator equal to that returned from `end`
TEST(InlineMap, IncrementingIteratorGoesThroughAllElements) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;

  constexpr std::uint32_t count = 15;
  constexpr int value = -123;

  for (imap::key_type idx = 0; idx < count; ++idx) {
    map[idx] = value;
  }

  arene::base::inline_vector<std::uint32_t, capacity> keys;

  auto itr = map.begin();
  for (imap::key_type idx = 0; idx < count; ++idx) {
    ASSERT_NE(itr, map.end());
    ASSERT_EQ(arene::base::find(keys.begin(), keys.end(), itr->first), keys.end());
    keys.push_back(itr->first);
    ASSERT_TRUE(map.contains(itr->first));
    ASSERT_EQ(&itr->second, &map[itr->first]);
    ++itr;
  }
  ASSERT_EQ(itr, map.end());
  ASSERT_EQ(keys.size(), count);
  std::sort(keys.begin(), keys.end());
  for (imap::key_type idx = 0; idx < count; ++idx) {
    ASSERT_EQ(keys[idx], idx);
  }
}

/// @test For a map with multiple elements, starting with the iterator returned from `end`, decrementing the iterator
/// a number of times equal to the number of elements in the map makes the iterator refer to each element once, and ends
/// with the iterator equal to that returned from `begin`
TEST(InlineMap, DecrementingIteratorGoesThroughAllElements) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  arene::base::inline_vector<std::pair<std::uint32_t, int>, capacity> const original_elements{
      {10U, 1},
      {1U, 2},
      {-1U, 3},
      {2U, 4}
  };
  arene::base::inline_vector<std::pair<std::uint32_t, int>, capacity> sorted_reversed_original_elements{
      original_elements
  };
  std::sort(
      sorted_reversed_original_elements.begin(),
      sorted_reversed_original_elements.end(),
      [](auto const& lhs_entry, auto const& rhs_entry) { return lhs_entry.first < rhs_entry.first; }
  );
  std::reverse(sorted_reversed_original_elements.begin(), sorted_reversed_original_elements.end());

  imap map{};
  for (auto const& original_element : original_elements) {
    map.emplace(original_element);
  }
  arene::base::inline_vector<std::pair<std::uint32_t, int>, capacity> iterated_elements;
  auto itr = map.end();
  for (std::size_t count = 0; count < map.size(); ++count) {
    iterated_elements.push_back(*(--itr));
  }
  EXPECT_TRUE(arene::base::equal(
      iterated_elements.begin(),
      iterated_elements.end(),
      sorted_reversed_original_elements.begin(),
      sorted_reversed_original_elements.end()
  ));
}

/// @test For a map with multiple elements, starting with the iterator returned from `begin`, incrementing the iterator
/// a number of times equal to the number of elements in the map makes the iterator refer to each element once, in the
/// sorted order of the keys
TEST(InlineMap, IncrementingIteratorGoesThroughAllElementsInSortedOrder) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  arene::base::inline_vector<std::pair<std::uint32_t, int>, capacity> const original_elements{
      {10U, 1},
      {1U, 2},
      {-1U, 3},
      {2U, 4}
  };
  arene::base::inline_vector<std::pair<std::uint32_t, int>, capacity> sorted_original_elements{original_elements};
  std::sort(
      sorted_original_elements.begin(),
      sorted_original_elements.end(),
      [](auto const& lhs_entry, auto const& rhs_entry) { return lhs_entry.first < rhs_entry.first; }
  );

  imap map{};
  for (auto const& original_element : original_elements) {
    map.emplace(original_element);
  }
  arene::base::inline_vector<std::pair<std::uint32_t, int>, capacity> iterated_elements;
  auto itr = map.begin();
  for (std::size_t count = 0; count < map.size(); ++count) {
    iterated_elements.push_back(*(itr++));
  }
  EXPECT_TRUE(arene::base::equal(
      iterated_elements.begin(),
      iterated_elements.end(),
      sorted_original_elements.begin(),
      sorted_original_elements.end()
  ));
}

/// @test The iterator returned from `erase` is an iterator to the first element with a key that is greater than the key
/// of the erased element, or an iterator equal to the iterator returned from `end` if there is no such element.
TEST(InlineMap, EraseReturnsIteratorToNextElement) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;

  imap::key_type const start_key = 1;
  imap::key_type const end_key = 10;
  arene::base::inline_vector<imap::key_type, capacity> keys;
  for (imap::key_type key = start_key; key != end_key; ++key) {
    keys.push_back(key);
  }
  for (auto const key : keys) {
    map[key] = static_cast<imap::mapped_type>(key + end_key);
  }

  ::testing::StaticAssertTypeEq<decltype(map.erase(std::declval<imap::key_type>())), imap::iterator>();
  {
    SCOPED_TRACE("value not in container");
    auto const original_size = map.size();
    auto const result = map.erase(end_key + 1);
    EXPECT_EQ(result, map.end());
    EXPECT_EQ(map.size(), original_size);
    for (auto const key : keys) {
      EXPECT_TRUE(map.contains(key));
    }
  }
  {
    SCOPED_TRACE("values in container");
    imap::size_type current_size = map.size();
    for (auto const key : arene::base::inline_vector<imap::key_type, capacity>{
             keys.begin(),
             keys.begin() + static_cast<std::ptrdiff_t>(keys.size()) / 2
         }) {
      auto const result = map.erase(key);
      ASSERT_NE(result, map.end());
      // Since we know this is an ordered container of contiguous keys, the position after the erase is the next key.
      EXPECT_EQ(result->first, key + 1);
      EXPECT_EQ(map.size(), --current_size);
    }
  }
}

/// @test If a custom comparator is specified, then the order of elements when iterating is in the sorted order
/// determined by the provided comparator
TEST(InlineMap, CanSpecifyComparator) {
  using counter = map_test::lt_comparable_counted<std::uint32_t>;
  constexpr std::uint32_t capacity = 20;

  using imap = inline_map<counter, int, capacity, map_test::reverse_less_than>;

  imap map;

  constexpr std::size_t count = 15;

  arene::base::inline_vector<std::uint32_t, count> const keys = {10, 6, 3, 9, 14, 0, 4, 7, 8, 2, 11, 12, 1, 13, 5};
  ASSERT_EQ(keys.size(), count);

  for (std::size_t idx = 0; idx < count; ++idx) {
    map[counter{keys[idx]}] = static_cast<int>(idx);
  }

  for (std::size_t idx = 0; idx < count; ++idx) {
    ASSERT_EQ(map[counter{keys[idx]}], static_cast<int>(idx));
  }

  auto itr = map.begin();
  for (std::size_t idx = 0; idx < count; ++idx) {
    ASSERT_EQ(itr->first.val, (count - 1 - idx));
    ASSERT_EQ(itr->second, arene::base::find(keys.begin(), keys.end(), itr->first.val) - keys.begin());
    ++itr;
  }
}

/// @test Invoking `erase` with a pair of iterators referring to the map removes the elements in that range, so the size
/// is descreased by the length of the range, and the keys in the range are no longer present in the map.
TEST(InlineMap, EraseRange) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;

  constexpr std::size_t count = 15;

  arene::base::inline_vector<std::uint32_t, count> const keys = {10, 6, 3, 9, 14, 0, 4, 7, 8, 2, 11, 12, 1, 13, 5};
  ASSERT_EQ(keys.size(), count);

  for (std::size_t idx = 0; idx < static_cast<std::size_t>(count); ++idx) {
    map[keys[idx]] = static_cast<int>(idx);
  }

  std::uint32_t const start = 4;
  std::uint32_t const end = 8;
  auto res = map.erase(map.find(start), map.find(end));

  ASSERT_EQ(res, map.find(end));
  ASSERT_EQ(map.size(), count - (end - start));

  for (std::uint32_t idx = 0; idx < count; ++idx) {
    auto pos = map.find(idx);
    if ((idx >= start) && (idx < end)) {
      ASSERT_EQ(pos, map.end());
    } else {
      ASSERT_EQ(pos->first, idx);
      ASSERT_EQ(pos->second, arene::base::find(keys.begin(), keys.end(), pos->first) - keys.begin());
    }
  }
}

/// @test Iterating through an `inline_map` using the iterators returned from `rbegin` and `rend` visits each of the
/// element in the reverse of the sorted order
TEST(InlineMap, ReverseIterationGoesThroughAllElementsInSortedOrder) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;

  constexpr std::size_t count = 15;

  arene::base::inline_vector<std::uint32_t, count> const keys = {10, 6, 3, 9, 14, 0, 4, 7, 8, 2, 11, 12, 1, 13, 5};
  ASSERT_EQ(keys.size(), count);

  for (std::size_t idx = 0; idx < count; ++idx) {
    map[keys[idx]] = static_cast<int>(idx);
  }

  auto itr = map.rbegin();
  ::testing::StaticAssertTypeEq<decltype(itr), imap::reverse_iterator>();
  ::testing::StaticAssertTypeEq<imap::reverse_iterator, ::arene::base::reverse_iterator<imap::iterator>>();
  ASSERT_EQ(itr.base(), map.end());
  for (std::size_t idx = 0; idx < count; ++idx) {
    ASSERT_EQ(itr->first, (count - 1 - idx));
    ASSERT_EQ(itr->second, arene::base::find(keys.begin(), keys.end(), itr->first) - keys.begin());
    ++itr;
  }
}

/// @test `rend` returns a reverse iterator with a `base` that is the result of calling `begin` on the `inline_map`.
TEST(InlineMap, REndReturnsReverseIteratorToBegin) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, int, capacity>;

  imap map;

  constexpr std::size_t count = 15;

  arene::base::inline_vector<std::uint32_t, count> const keys = {10, 6, 3, 9, 14, 0, 4, 7, 8, 2, 11, 12, 1, 13, 5};
  ASSERT_EQ(keys.size(), count);

  for (std::size_t idx = 0; idx < count; ++idx) {
    map[keys[idx]] = static_cast<int>(idx);
  }

  auto itr = map.rend();
  ::testing::StaticAssertTypeEq<decltype(itr), imap::reverse_iterator>();
  ::testing::StaticAssertTypeEq<imap::reverse_iterator, ::arene::base::reverse_iterator<imap::iterator>>();

  ASSERT_EQ(itr.base(), map.begin());
}

using map_test::const_and_non_const_transparent_imaps;
using map_test::InlineMapTransparentComparisonEquivalencyTest;

TYPED_TEST_SUITE(InlineMapTransparentComparisonEquivalencyTest, const_and_non_const_transparent_imaps, );

/// @test `at` returns the same result when supplied a value that is comparable to the key type as it does when
/// supplied a key that has the same comparison results
TYPED_TEST(InlineMapTransparentComparisonEquivalencyTest, At) {
  ::testing::StaticAssertTypeEq<
      decltype(this->values.at(std::declval<typename TestFixture::comparable_not_convertible_to_key>())),
      decltype(this->values.at(std::declval<typename TestFixture::imap::key_type>()))>();
  ASSERT_THROW(
      std::ignore =
          this->values.at(typename TestFixture::comparable_not_convertible_to_key{this->key_less_than_container}),
      std::out_of_range
  );
  ASSERT_THROW(
      std::ignore =
          this->values.at(typename TestFixture::comparable_not_convertible_to_key{this->key_greater_than_container}),
      std::out_of_range
  );
  ASSERT_THROW(
      std::ignore =
          this->values.at(typename TestFixture::comparable_not_convertible_to_key{this->key_gap_in_container}),
      std::out_of_range
  );
  EXPECT_EQ(
      this->values.at(typename TestFixture::comparable_not_convertible_to_key{this->key_in_container}),
      this->values.at(this->key_in_container)
  );
}

}  // namespace
