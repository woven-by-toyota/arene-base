// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/next.hpp"

#include <gtest/gtest.h>

#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/copy_n.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/end.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

namespace iterator_tests = ::arene::base::iterator_tests;

/// @test Given a random access iterator, @c arene::base::next is noexcept if the iterator's @c operator+= is noexcept.
TEST(NextNoexcept, ForRandomAccessIteratorIsTrueIfIncrementAssignIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(::arene::base::next(std::declval<iterator_tests::random_access_iterator_facade<>>(), 1)));
  STATIC_ASSERT_FALSE(noexcept(::arene::base::next(
      std::declval<iterator_tests::random_access_iterator_facade<iterator_tests::throwing_increment_assign>>(),
      1
  )));
}
/// @test Given a bidirectional iterator, @c arene::base::next is noexcept if the iterator's @c operator++ and
/// @c operator-- are noexcept.
TEST(NextNoexcept, ForBidirectionalIteratorIsTrueIfPostIncrementAndPostDecrementAreNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(::arene::base::next(std::declval<iterator_tests::bidirectional_iterator_facade<>>(), 1)));
  STATIC_ASSERT_FALSE(noexcept(::arene::base::next(
      std::declval<iterator_tests::bidirectional_iterator_facade<iterator_tests::throwing_preincrement>>(),
      1
  )));
  STATIC_ASSERT_FALSE(noexcept(::arene::base::next(
      std::declval<iterator_tests::bidirectional_iterator_facade<iterator_tests::throwing_predecrement>>(),
      1
  )));
}
/// @test Given an input iterator, @c arene::base::next is noexcept if the iterator's @c operator++ is noexcept.
TEST(NextNoexcept, ForInputIteratorIsTrueIfPostIncrementIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(::arene::base::next(std::declval<iterator_tests::input_iterator_facade<>>(), 1)));
  STATIC_ASSERT_FALSE(noexcept(::arene::base::next(
      std::declval<iterator_tests::input_iterator_facade<iterator_tests::throwing_preincrement>>(),
      1
  )));
}

template <typename Container>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
struct NextTest
    : protected iterator_tests::backing_data<Container>
    , public ::testing::Test {};

template <typename Container>
class NextReturnTypeTest : public NextTest<Container> {};

TYPED_TEST_SUITE(NextReturnTypeTest, iterator_tests::all_iterator_category_containers, );

/// @test Given any iterator, the return type of @c arene::base::next is always the same as its input iterator's type.
TYPED_TEST(NextReturnTypeTest, ReturnTypeIsInputIteratorType) {
  using input_iterator_t = decltype(this->begin());
  ::testing::StaticAssertTypeEq<decltype(::arene::base::next(std::declval<input_iterator_t>(), 1U)), input_iterator_t>(
  );
}

template <typename Container>
class NextPositiveStepTest : public NextTest<Container> {};

TYPED_TEST_SUITE(NextPositiveStepTest, iterator_tests::at_least_forward_iterator_containers, );

/// @test Given a positive step and an at least forward iterator, @c arene::base::next produces an iterator
/// equivalent to incrementing the input iterator step times.
TYPED_TEST(NextPositiveStepTest, ReturnIteratorIsInputIteratorIncrementedStepTypes) {
  auto itr = this->begin();
  for (auto const steps : std::initializer_list<int>{0U, 1U, 2U, this->ssize()}) {
    auto expected_itr = itr;
    for (int count = 0; count < steps; ++count) {
      expected_itr++;
    }
    EXPECT_EQ(::arene::base::next(itr, steps), expected_itr);
  }
}

template <typename Container>
class NextNegativeStepTest : public NextTest<Container> {};

TYPED_TEST_SUITE(NextNegativeStepTest, iterator_tests::at_least_bidirectional_iterator_containers, );

/// @test Given a negative step and an at least bidirectional iterator, @c arene::base::next produces an iterator
/// equivalent to decrementing the input iterator step times.
TYPED_TEST(NextNegativeStepTest, ReturnIteratorIsInputIteratorDecrementedStepTypes) {
  auto itr = this->end();
  for (auto const steps : std::initializer_list<int>{-1, -2, -this->ssize()}) {
    auto expected_itr = itr;
    for (int count = 0; count > steps; --count) {
      expected_itr--;
    }
    EXPECT_EQ(::arene::base::next(itr, steps), expected_itr);
  }
}

template <typename Container>
class NextInputIteratorTest : public NextTest<Container> {};

TYPED_TEST_SUITE(NextInputIteratorTest, iterator_tests::input_iterator_containers, );

/// @test Given a positive step and an input iterator, @c arene::base::next produces an iterator
/// equivalent to incrementing the input iterator step times.
TYPED_TEST(NextInputIteratorTest, ReturnIteratorIsInputIteratorIncrementedStepTypes) {
  // Since input iterators are single-pass, we have to hard code expectations here.
  auto advanced_zero = ::arene::base::next(this->begin(), 0);
  EXPECT_EQ(*advanced_zero, 1);
  auto advanced_one = ::arene::base::next(advanced_zero, 1);
  EXPECT_EQ(*advanced_one, 2);
  auto advanced_five = ::arene::base::next(advanced_one, this->ssize() - 2);
  EXPECT_EQ(*advanced_five, 5);
}

/// @test Validates @c ::arene::base::next functions in a constexpr context with a positive step.
TEST(NextConstexprTest, ReturnIteratorIsInputIteratorIncrementedStepTypes) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) c-array is only constexpr container in stdlib in C++14.
  constexpr int arr[5]{1, 2, 3, 4, 5};
  STATIC_ASSERT_EQ(::arene::base::next(std::begin(arr), 1), &arr[1]);
}
/// @test Validates @c ::arene::base::next functions in a constexpr context with a negative step.
TEST(NextConstexprTest, ReturnIteratorIsInputIteratorDecrementedStepTypes) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) c-array is only constexpr container in stdlib in C++14.
  constexpr int arr[5]{1, 2, 3, 4, 5};
  STATIC_ASSERT_EQ(::arene::base::next(std::end(arr), -1), &arr[4]);
}

}  // namespace
