// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/lexicographical_compare.hpp"

#include <initializer_list>
#include <list>
#include <set>
#include <tuple>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/compare.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list.hpp"

namespace {

using ::arene::base::lexicographical_compare;
using ::arene::base::lexicographical_compare_three_way;
using ::arene::base::strong_ordering;

namespace type_lists = ::arene::base::type_lists;
using ::arene::base::type_list;

template <typename Tl>
using as_types = type_lists::apply_all_t<Tl, ::testing::Types>;

using non_random_access_iterators = type_list<std::list<int>, std::set<int>>;
using non_constexpr_random_access_iterators = type_list<std::vector<int>>;
using constexpr_random_access_iterators = type_list<std::initializer_list<int>>;
using random_access_iterators =
    type_lists::concat_t<non_constexpr_random_access_iterators, constexpr_random_access_iterators>;

using all_iterators = type_lists::concat_t<non_random_access_iterators, random_access_iterators>;

template <typename T>
class LexicographicalComparisonsTest : public ::testing::Test {};

template <typename T>
class LexicographicalCompareTest : public LexicographicalComparisonsTest<T> {};

TYPED_TEST_SUITE(LexicographicalCompareTest, as_types<all_iterators>, );

/// @test Verify that an empty sequence is not lexicographically less than another empty sequence.
TYPED_TEST(LexicographicalCompareTest, AnEmptySequenceIsNotLexicographicallyLessThanAnotherEmptySequence) {
  TypeParam const empty_1{};
  TypeParam const empty_2{};
  EXPECT_FALSE(lexicographical_compare(empty_1.begin(), empty_1.end(), empty_2.begin(), empty_2.end()));
}

/// @test Verify that an empty sequence is always lexicographically less than another non-empty sequence.
TYPED_TEST(LexicographicalCompareTest, AnEmptySequenceIsAlwaysLexicographicallyLessThanAnotherNonEmptySequence) {
  TypeParam const empty{};
  TypeParam const not_empty{1, 2, 3};
  EXPECT_TRUE(lexicographical_compare(empty.begin(), empty.end(), not_empty.begin(), not_empty.end()));
  EXPECT_FALSE(lexicographical_compare(not_empty.begin(), not_empty.end(), empty.begin(), empty.end()));
}

/// @test Verify that a strict subset sequence is lexicographically less than its superset.
TYPED_TEST(LexicographicalCompareTest, AStrictSubsetSequenceIsLexicographicallyLessThanItsSuperset) {
  TypeParam const subset{1, 2, 3};
  TypeParam const superset{1, 2, 3, 4, 5};
  EXPECT_TRUE(lexicographical_compare(subset.begin(), subset.end(), superset.begin(), superset.end()));
  EXPECT_FALSE(lexicographical_compare(superset.begin(), superset.end(), subset.begin(), subset.end()));
}

/// @test Verify that the result of lexicographical comparison of two sequences depends on the
/// comparison of the first element that does not compare equal.
TYPED_TEST(LexicographicalCompareTest, ASequenceWhereOneElementComparesLessComparesLessThanAnotherSequence) {
  TypeParam const lesser_element{1, 2, 3};
  TypeParam const greater_element{1, 5, 3};
  EXPECT_TRUE(lexicographical_compare(
      lesser_element.begin(),
      lesser_element.end(),
      greater_element.begin(),
      greater_element.end()
  ));
  EXPECT_FALSE(lexicographical_compare(
      greater_element.begin(),
      greater_element.end(),
      lesser_element.begin(),
      lesser_element.end()
  ));
}

/// @test Verify that equal sequences do not compare less for lexicographical comparison.
TYPED_TEST(LexicographicalCompareTest, EqualSequencesDoNotCompareLess) {
  TypeParam const sequence_1{1, 2, 3};
  TypeParam const sequence_2{1, 2, 3};
  EXPECT_FALSE(lexicographical_compare(sequence_1.begin(), sequence_1.end(), sequence_2.begin(), sequence_2.end()));
  EXPECT_FALSE(lexicographical_compare(sequence_2.begin(), sequence_2.end(), sequence_1.begin(), sequence_1.end()));
}

/// @test Verify that the comparison function is called exactly twice for each pair of elements in the input sequences.
TYPED_TEST(LexicographicalCompareTest, CallsCustomCmpExactlyTwiceForEachPairOfElementsInTheInputSequences) {
  TypeParam const sequence_1{1, 2, 3, 4, 5};
  TypeParam const sequence_2{5, 4, 3, 2, 1};
  std::vector<typename TypeParam::value_type> lhs_compared_elements;
  std::vector<typename TypeParam::value_type> rhs_compared_elements;
  auto const observing_cmp = [&lhs_compared_elements, &rhs_compared_elements](auto const& lhs, auto const& rhs) {
    lhs_compared_elements.push_back(lhs);
    rhs_compared_elements.push_back(rhs);
    return false;
  };
  std::ignore = lexicographical_compare(
      sequence_1.begin(),
      sequence_1.end(),
      sequence_2.begin(),
      sequence_2.end(),
      observing_cmp
  );
  EXPECT_THAT(lhs_compared_elements, ::testing::IsSupersetOf(sequence_1));
  EXPECT_THAT(lhs_compared_elements, ::testing::IsSupersetOf(sequence_2));
  EXPECT_THAT(lhs_compared_elements, ::testing::SizeIs(sequence_1.size() + sequence_2.size()));
  EXPECT_THAT(rhs_compared_elements, ::testing::IsSupersetOf(sequence_1));
  EXPECT_THAT(rhs_compared_elements, ::testing::IsSupersetOf(sequence_2));
  EXPECT_THAT(rhs_compared_elements, ::testing::SizeIs(sequence_1.size() + sequence_2.size()));
}

/// @test Verify that the result of lexicographical comparison is true if the comparator
/// returns true for an element of the first sequence as the left-hand side.
TYPED_TEST(LexicographicalCompareTest, ReturnsTrueIfCmpReturnsTrueForAnElementOfSequenceOneAsLhs) {
  TypeParam const sequence_1{1, 2, 3, 4, 5};
  TypeParam const sequence_2{1, 2, 7, 4, 5};
  // We want accending order so our comparison is now greater rather than less
  EXPECT_FALSE(lexicographical_compare(
      sequence_1.begin(),
      sequence_1.end(),
      sequence_2.begin(),
      sequence_2.end(),
      std::greater<>{}
  ));
  EXPECT_TRUE(lexicographical_compare(
      sequence_2.begin(),
      sequence_2.end(),
      sequence_1.begin(),
      sequence_1.end(),
      std::greater<>{}
  ));
}

template <typename T>
class LexicographicalCompareThreeWayTest : public LexicographicalComparisonsTest<T> {};

TYPED_TEST_SUITE(LexicographicalCompareThreeWayTest, as_types<all_iterators>, );

/// @test Verify that an empty sequence is lexicographically equal to another empty sequence given
/// a three-way comparator.
TYPED_TEST(LexicographicalCompareThreeWayTest, AnEmptySequenceIsLexicographicallyEqualToAnotherEmptySequence) {
  TypeParam const empty_1{};
  TypeParam const empty_2{};
  EXPECT_EQ(
      strong_ordering::equal,
      lexicographical_compare_three_way(empty_1.begin(), empty_1.end(), empty_2.begin(), empty_2.end())
  );
  EXPECT_EQ(
      strong_ordering::equal,
      lexicographical_compare_three_way(empty_2.begin(), empty_2.end(), empty_1.begin(), empty_1.end())
  );
}

/// @test Verify that an empty sequence is lexicographically less than another empty sequence given
/// a three-way comparator.
TYPED_TEST(LexicographicalCompareThreeWayTest, AnEmptySequenceIsLexicographicallyLessToAnotherSequence) {
  TypeParam const empty{};
  TypeParam const not_empty{1, 2, 3};
  EXPECT_EQ(
      strong_ordering::less,
      lexicographical_compare_three_way(empty.begin(), empty.end(), not_empty.begin(), not_empty.end())
  );
  EXPECT_EQ(
      strong_ordering::greater,
      lexicographical_compare_three_way(not_empty.begin(), not_empty.end(), empty.begin(), empty.end())
  );
}

/// @test Verify that a strict subset sequence is lexicographically less than its superset given
/// a three-way comparator.
TYPED_TEST(LexicographicalCompareThreeWayTest, AStrictSubsetSequenceIsLexicographicallyLessThanItsSuperset) {
  TypeParam const subset{1, 2, 3};
  TypeParam const superset{1, 2, 3, 4, 5};
  EXPECT_EQ(
      strong_ordering::less,
      lexicographical_compare_three_way(subset.begin(), subset.end(), superset.begin(), superset.end())
  );
  EXPECT_EQ(
      strong_ordering::greater,
      lexicographical_compare_three_way(superset.begin(), superset.end(), subset.begin(), subset.end())
  );
}

/// @test Verify that the result of lexicographical comparison of two sequences depends on the
/// comparison of the first element that does not compare equal, given a three-way comparator.
TYPED_TEST(LexicographicalCompareThreeWayTest, ReturnsTheThreeWayCompareOfTheFirstElementThatDoesNotCompareEqual) {
  TypeParam const lesser_element{1, 2, 3};
  TypeParam const greater_element{1, 5, 3};
  EXPECT_EQ(
      lexicographical_compare_three_way(
          lesser_element.begin(),
          lesser_element.end(),
          greater_element.begin(),
          greater_element.end()
      ),
      strong_ordering::less
  );
  EXPECT_EQ(
      lexicographical_compare_three_way(
          greater_element.begin(),
          greater_element.end(),
          lesser_element.begin(),
          lesser_element.end()
      ),
      strong_ordering::greater
  );
}

/// @test Verify that equal sequences compare equal, given a three-way comparator.
TYPED_TEST(LexicographicalCompareThreeWayTest, EqualSequencesCompareEqual) {
  TypeParam const sequence_1{1, 2, 3};
  TypeParam const sequence_2{1, 2, 3};
  EXPECT_EQ(
      lexicographical_compare_three_way(sequence_1.begin(), sequence_1.end(), sequence_2.begin(), sequence_2.end()),
      strong_ordering::equal
  );
  EXPECT_EQ(
      lexicographical_compare_three_way(sequence_2.begin(), sequence_2.end(), sequence_1.begin(), sequence_1.end()),
      strong_ordering::equal
  );
}

/// @test Verify that the comparison function is called exactly once for each pair of elements in the input sequences,
/// given a three-way comparator.
TYPED_TEST(LexicographicalCompareThreeWayTest, CallsCustomCmpExactlyOnceForEachPairOfElementsInTheInputSequences) {
  TypeParam const sequence_1{1, 2, 3, 4, 5};
  TypeParam const sequence_2{5, 4, 3, 2, 1};
  std::vector<typename TypeParam::value_type> sequence_1_compared_elements;
  std::vector<typename TypeParam::value_type> sequence_2_compared_elements;
  auto const observing_cmp = [&sequence_1_compared_elements,
                              &sequence_2_compared_elements](auto const& lhs, auto const& rhs) {
    sequence_1_compared_elements.push_back(lhs);
    sequence_2_compared_elements.push_back(rhs);
    return strong_ordering::equal;
  };
  std::ignore = lexicographical_compare_three_way(
      sequence_1.begin(),
      sequence_1.end(),
      sequence_2.begin(),
      sequence_2.end(),
      observing_cmp
  );
  EXPECT_THAT(sequence_1_compared_elements, ::testing::ElementsAreArray(sequence_1));
  EXPECT_THAT(sequence_2_compared_elements, ::testing::ElementsAreArray(sequence_2));
}

/// @test Verify that the result of lexicographical comparison is the result of the first application of the custom
/// three-way comparator that does not return equal.
TYPED_TEST(LexicographicalCompareTest, ReturnsResultOfFirstApplicationOfCustomTWCThatDoesNotReturnEqual) {
  TypeParam const sequence_1{1, 2, 3, 4, 5};
  TypeParam const sequence_2{1, 2, 3, 4, 5};
  strong_ordering expected_return = strong_ordering::greater;
  auto const return_expected_every_two_calls = [&expected_return, count = 0](auto const&, auto const&) mutable {
    ++count;
    return count % 2 == 0 ? expected_return : strong_ordering::equal;
  };
  EXPECT_EQ(
      lexicographical_compare_three_way(
          sequence_2.begin(),
          sequence_2.end(),
          sequence_1.begin(),
          sequence_1.end(),
          return_expected_every_two_calls
      ),
      expected_return
  );
  expected_return = strong_ordering::less;
  EXPECT_EQ(
      lexicographical_compare_three_way(
          sequence_2.begin(),
          sequence_2.end(),
          sequence_1.begin(),
          sequence_1.end(),
          return_expected_every_two_calls
      ),
      expected_return
  );
  expected_return = strong_ordering::equal;
  EXPECT_EQ(
      lexicographical_compare_three_way(
          sequence_2.begin(),
          sequence_2.end(),
          sequence_1.begin(),
          sequence_1.end(),
          return_expected_every_two_calls
      ),
      expected_return
  );
}

/// @brief indirection helper to do a comparison in constexpr if T cannot be bound to constexpr variable directly.
/// @param opr Should be either lexicographical_compare or lexicographical_compare_three_way
/// @return The result of invoking @c opr with sequences that should compare less than
template <typename T, typename F>
constexpr auto do_lc_op_as_lesser_in_constexpr(F&& opr) {
  T const lesser_element{1, 2, 3};
  T const greater_element{1, 5, 3};
  return opr(lesser_element.begin(), lesser_element.end(), greater_element.begin(), greater_element.end());
}

template <typename T>
class LexicographicalCompareConstexprTest : public LexicographicalComparisonsTest<T> {};

TYPED_TEST_SUITE(LexicographicalCompareConstexprTest, as_types<constexpr_random_access_iterators>, );

/// @test Verify that `lexicographical_compare` works in a constexpr context.
TYPED_TEST(LexicographicalCompareConstexprTest, WorksInConstexpr) {
  STATIC_ASSERT_TRUE(do_lc_op_as_lesser_in_constexpr<TypeParam>(lexicographical_compare));
}

template <typename T>
class LexicographicalCompareThreeWayConstexprTest : public LexicographicalComparisonsTest<T> {};

TYPED_TEST_SUITE(LexicographicalCompareThreeWayConstexprTest, as_types<constexpr_random_access_iterators>, );

/// @test Verify that `lexicographical_compare_three_way` works in a constexpr context.
TYPED_TEST(LexicographicalCompareThreeWayConstexprTest, WorksInConstexpr) {
  STATIC_ASSERT_EQ(
      strong_ordering::less,
      do_lc_op_as_lesser_in_constexpr<TypeParam>(lexicographical_compare_three_way)
  );
}

}  // namespace
