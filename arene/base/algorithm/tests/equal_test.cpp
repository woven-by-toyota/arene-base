// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/equal.hpp"

#include <cstdlib>
#include <list>
#include <set>
#include <tuple>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/iterator.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/type_list.hpp"

namespace {
using ::arene::base::equal;

using ::arene::base::type_list;
namespace type_lists = ::arene::base::type_lists;

template <typename Tl>
using as_types = type_lists::apply_all_t<Tl, ::testing::Types>;

using not_random_access_types = type_list<std::list<int>, std::set<int>>;
using random_access_types = type_list<std::vector<int>>;
using all_containers = type_lists::concat_t<not_random_access_types, random_access_types>;

template <typename T>
struct EqualTest : public ::testing::Test {};

template <typename T>
class EqualAlwaysEqualSizeTest : public EqualTest<T> {};

TYPED_TEST_SUITE(EqualAlwaysEqualSizeTest, as_types<all_containers>, );

/// @test Two empty sequences are equal.
TYPED_TEST(EqualAlwaysEqualSizeTest, EmptySequencesAreEqual) {
  auto const sequence_1 = TypeParam{};
  auto const sequence_2 = TypeParam{};
  EXPECT_TRUE(equal(std::begin(sequence_1), std::end(sequence_1), std::begin(sequence_2)));
  EXPECT_TRUE(equal(std::begin(sequence_1), std::end(sequence_1), std::begin(sequence_2), [](auto const&, auto const&) {
    return true;
  }));
}

/// @test `equal` calls the comparison function with the ordered sequence pairs.
TYPED_TEST(EqualAlwaysEqualSizeTest, CallsCmpWithTheOrderedSequencePairs) {
  auto const sequence_1 = TypeParam{1, 2, 3, 4, 5, 6, 7};
  auto const sequence_2 = TypeParam{7, 6, 5, 4, 3, 2, 1};
  std::vector<typename TypeParam::value_type> sequence_1_elements;
  std::vector<typename TypeParam::value_type> sequence_2_elements;
  auto const observing_cmp = [&sequence_1_elements,
                              &sequence_2_elements](auto const& seq1_element, auto const& seq2_element) {
    sequence_1_elements.push_back(seq1_element);
    sequence_2_elements.push_back(seq2_element);
    return true;
  };
  std::ignore = equal(std::begin(sequence_1), std::end(sequence_1), std::begin(sequence_2), observing_cmp);
  EXPECT_THAT(sequence_1_elements, ::testing::ElementsAreArray(sequence_1));
  EXPECT_THAT(sequence_2_elements, ::testing::ElementsAreArray(sequence_2));
}

/// @test `equal` returns true if all elements in the sequences compare equivalent.
TYPED_TEST(EqualAlwaysEqualSizeTest, ReturnsTrueIfAllElementsInTheSequencesCompareEquivalent) {
  auto const sequence_1 = TypeParam{1, 2, 3, 4, 5};
  auto const sequence_2 = TypeParam{1, 2, 3, 4, 5};
  EXPECT_TRUE(equal(std::begin(sequence_1), std::end(sequence_1), std::begin(sequence_2)));

  auto const custom_compare_1 = TypeParam{-1, 1};
  auto const custom_compare_2 = TypeParam{1, -1};
  auto const are_abs_equivalent = [](auto const& lhs, auto const& rhs) { return std::abs(lhs) == std::abs(rhs); };
  EXPECT_TRUE(
      equal(std::begin(custom_compare_1), std::end(custom_compare_1), std::begin(custom_compare_2), are_abs_equivalent)
  );
}

/// @test `equal` returns false if any elements in the sequences compare not equivalent.
TYPED_TEST(EqualAlwaysEqualSizeTest, ReturnsFalseIfAnyElementsInTheSequencesCompareNotEquivalent) {
  auto const sequence_1 = TypeParam{1, 2, 3, 4, 5};
  for (std::size_t offset = 0; offset < sequence_1.size(); ++offset) {
    std::vector<int> sequence_2{std::begin(sequence_1), std::end(sequence_1)};
    ++sequence_2[offset];
    EXPECT_FALSE(equal(std::begin(sequence_1), std::end(sequence_1), std::begin(sequence_2)));
  }

  auto const always_false = [](auto const&, auto const&) { return false; };
  EXPECT_FALSE(equal(std::begin(sequence_1), std::end(sequence_1), std::begin(sequence_1), always_false));
}

template <typename T>
class EqualMaybeUnequalSizeTest : public EqualTest<T> {};

TYPED_TEST_SUITE(EqualMaybeUnequalSizeTest, as_types<all_containers>, );

/// @test Empty sequences are equal.
TYPED_TEST(EqualMaybeUnequalSizeTest, EmptySequencesAreEqual) {
  auto const sequence_1 = TypeParam{};
  auto const sequence_2 = TypeParam{};
  EXPECT_TRUE(equal(std::begin(sequence_1), std::end(sequence_1), std::begin(sequence_2), std::end(sequence_2)));
  EXPECT_TRUE(equal(
      std::begin(sequence_1),
      std::end(sequence_1),
      std::begin(sequence_2),
      std::end(sequence_2),
      [](auto const&, auto const&) { return true; }
  ));
}

/// @test `equal` calls the comparison function with the ordered sequence pairs up to the shortest
/// sequence.
TYPED_TEST(EqualMaybeUnequalSizeTest, CallsCmpWithTheOrderedSequencePairsUpToShortestSequence) {
  std::vector<typename TypeParam::value_type> lhs_elements_compared;
  std::vector<typename TypeParam::value_type> rhs_elements_compared;
  auto const always_true_observing_cmp = [&lhs_elements_compared,
                                          &rhs_elements_compared](auto const& lhs_element, auto const& rhs_element) {
    lhs_elements_compared.push_back(lhs_element);
    rhs_elements_compared.push_back(rhs_element);
    return true;
  };
  // We have to use IsSubsetOf in these comparison to allow optimizations for random access itrs that shortcut and never
  // compare elements. There is a separate to test that this optimization happens.
  {
    SCOPED_TRACE("Equal length inputs");
    auto const equal_length_1 = TypeParam{1, 2, 3, 4, 5, 6, 7};
    auto const equal_length_2 = TypeParam{7, 6, 5, 4, 3, 1, 2};
    lhs_elements_compared.clear();
    rhs_elements_compared.clear();
    std::ignore = equal(
        std::begin(equal_length_1),
        std::end(equal_length_1),
        std::begin(equal_length_2),
        std::end(equal_length_2),
        always_true_observing_cmp
    );
    EXPECT_THAT(lhs_elements_compared, ::testing::IsSubsetOf(equal_length_1));
    EXPECT_THAT(rhs_elements_compared, ::testing::IsSubsetOf(equal_length_2));
  }

  {
    SCOPED_TRACE("Unequal length inputs");
    auto const longer = TypeParam{1, 2, 3, 4, 5, 6, 7};
    auto const shorter = TypeParam{7, 6, 5, 4, 3};
    {
      SCOPED_TRACE("rhs is shorter than lhs");
      lhs_elements_compared.clear();
      rhs_elements_compared.clear();
      std::ignore = equal(
          std::begin(longer),
          std::end(longer),
          std::begin(shorter),
          std::end(shorter),
          always_true_observing_cmp
      );
      EXPECT_THAT(lhs_elements_compared, ::testing::IsSubsetOf(longer));
      EXPECT_THAT(rhs_elements_compared, ::testing::IsSubsetOf(shorter));
    }
    {
      SCOPED_TRACE("lhs is shorter than rhs");
      lhs_elements_compared.clear();
      rhs_elements_compared.clear();
      std::ignore = equal(
          std::begin(shorter),
          std::end(shorter),
          std::begin(longer),
          std::end(longer),
          always_true_observing_cmp
      );
      EXPECT_THAT(lhs_elements_compared, ::testing::IsSubsetOf(shorter));
      EXPECT_THAT(rhs_elements_compared, ::testing::IsSubsetOf(longer));
    }
  }
}

/// @test `equal` returns true if all elements in the sequences compare equivalent.
TYPED_TEST(EqualMaybeUnequalSizeTest, ReturnsTrueIfAllElementsInTheSequencesCompareEquivalent) {
  auto const sequence_1 = TypeParam{1, 2, 3, 4, 5};
  auto const sequence_2 = TypeParam{1, 2, 3, 4, 5};
  EXPECT_TRUE(equal(std::begin(sequence_1), std::end(sequence_1), std::begin(sequence_2), std::end(sequence_2)));

  auto const custom_compare_1 = TypeParam{-1, 1};
  auto const custom_compare_2 = TypeParam{1, -1};
  auto const are_abs_equivalent = [](auto const& lhs, auto const& rhs) { return std::abs(lhs) == std::abs(rhs); };
  EXPECT_TRUE(equal(
      std::begin(custom_compare_1),
      std::end(custom_compare_1),
      std::begin(custom_compare_2),
      std::end(custom_compare_2),
      are_abs_equivalent
  ));
}

/// @test `equal` returns false if the length of the sequences are not equal.
TYPED_TEST(EqualMaybeUnequalSizeTest, ReturnsFalseIfSequencesAreUnequalLength) {
  auto const longer = TypeParam{1, 2, 3, 4, 5, 6, 7};
  auto const shorter = TypeParam{
      std::begin(longer),
      ::arene::base::next(std::begin(longer), static_cast<std::ptrdiff_t>(longer.size() / 2))
  };
  EXPECT_FALSE(equal(std::begin(longer), std::end(longer), std::begin(shorter), std::end(shorter)));
  EXPECT_FALSE(equal(std::begin(shorter), std::end(shorter), std::begin(longer), std::end(longer)));

  auto const empty = TypeParam{};
  EXPECT_FALSE(equal(std::begin(longer), std::end(longer), std::begin(empty), std::end(empty)));
  EXPECT_FALSE(equal(std::begin(empty), std::end(empty), std::begin(longer), std::end(longer)));
}

/// @test `equal` returns `false` if any element in the sequences being compared do not compare equal
TYPED_TEST(EqualMaybeUnequalSizeTest, ReturnsFalseIfAnyElementsInTheSequencesCompareNotEquivalent) {
  auto const sequence_1 = TypeParam{1, 2, 3, 4, 5};
  for (std::size_t offset = 0; offset < sequence_1.size(); ++offset) {
    std::vector<int> sequence_2{std::begin(sequence_1), std::end(sequence_1)};
    ++sequence_2[offset];
    EXPECT_FALSE(equal(std::begin(sequence_1), std::end(sequence_1), std::begin(sequence_2), std::end(sequence_2)));
  }

  auto const always_false = [](auto const&, auto const&) { return false; };
  EXPECT_FALSE(
      equal(std::begin(sequence_1), std::end(sequence_1), std::begin(sequence_1), std::end(sequence_1), always_false)
  );
}

template <typename T>
class EqualMaybeUnequalSizeRandomAccessIteratorsTest : public EqualTest<T> {};

TYPED_TEST_SUITE(EqualMaybeUnequalSizeRandomAccessIteratorsTest, as_types<random_access_types>, );

/// @test `equal` does not call the comparison function if the random access iterators are unequal length.
TYPED_TEST(EqualMaybeUnequalSizeRandomAccessIteratorsTest, DoesNotCallCmpIfRandomAccessItrsAreUnequalLength) {
  auto const longer = TypeParam{1, 2, 3, 4, 5, 6, 7};
  auto const shorter = TypeParam{
      std::begin(longer),
      ::arene::base::next(std::begin(longer), static_cast<std::ptrdiff_t>(longer.size() / 2))
  };
  std::size_t call_count = 0;
  auto const call_counting_cmp = [&call_count](auto const&, auto const&) {
    ++call_count;
    return true;
  };
  std::ignore = equal(std::begin(longer), std::end(longer), std::begin(shorter), std::end(shorter), call_counting_cmp);
  EXPECT_EQ(call_count, 0);
  call_count = 0;
  std::ignore = equal(std::begin(shorter), std::end(shorter), std::begin(longer), std::end(longer), call_counting_cmp);
  EXPECT_EQ(call_count, 0);

  auto const empty = TypeParam{};
  call_count = 0;
  std::ignore = equal(std::begin(longer), std::end(longer), std::begin(empty), std::end(empty), call_counting_cmp);
  EXPECT_EQ(call_count, 0);
  call_count = 0;
  std::ignore = equal(std::begin(empty), std::end(empty), std::begin(longer), std::end(longer), call_counting_cmp);
  EXPECT_EQ(call_count, 0);
}

}  // namespace
