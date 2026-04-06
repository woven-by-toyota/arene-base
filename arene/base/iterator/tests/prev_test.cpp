// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/prev.hpp"

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

/// @test Given a random access iterator, @c arene::base::prev is noexcept if the iterator's @c operator+= is noexcept.
TEST(PrevNoexcept, ForRandomAccessIteratorIsTrueIfIncrementAssignIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(::arene::base::prev(std::declval<iterator_tests::random_access_iterator_facade<>>(), 1)));
  STATIC_ASSERT_FALSE(noexcept(::arene::base::prev(
      std::declval<iterator_tests::random_access_iterator_facade<iterator_tests::throwing_increment_assign>>(),
      1
  )));
}
/// @test Given a bidirectional iterator, @c arene::base::prev is noexcept if the iterator's @c operator++ and
/// @c operator-- are noexcept.
TEST(PrevNoexcept, ForBidirectionalIteratorIsTrueIfPostIncrementAndPostDecrementAreNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(::arene::base::prev(std::declval<iterator_tests::bidirectional_iterator_facade<>>(), 1)));
  STATIC_ASSERT_FALSE(noexcept(::arene::base::prev(
      std::declval<iterator_tests::bidirectional_iterator_facade<iterator_tests::throwing_preincrement>>(),
      1
  )));
  STATIC_ASSERT_FALSE(noexcept(::arene::base::prev(
      std::declval<iterator_tests::bidirectional_iterator_facade<iterator_tests::throwing_predecrement>>(),
      1
  )));
}

template <typename Container>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
struct PrevTest
    : protected iterator_tests::backing_data<Container>
    , public ::testing::Test {};

template <typename Container>
using PrevReturnTypeTest = PrevTest<Container>;

TYPED_TEST_SUITE(PrevReturnTypeTest, iterator_tests::at_least_bidirectional_iterator_containers, );

/// @test Given an at least bidirectional iterator, the return type of @c arene::base::prev is always the same as its
/// input iterator's type.
TYPED_TEST(PrevReturnTypeTest, ReturnTypeIsInputIteratorType) {
  using input_iterator_t = decltype(this->begin());
  ::testing::StaticAssertTypeEq<decltype(::arene::base::prev(std::declval<input_iterator_t>(), 1U)), input_iterator_t>(
  );
}

template <typename Container>
using PrevPositiveStepTest = PrevTest<Container>;

TYPED_TEST_SUITE(PrevPositiveStepTest, iterator_tests::at_least_bidirectional_iterator_containers, );

/// @test Given a positive step and an at least bidirectional iterator, @c arene::base::prev produces an iterator
/// equivalent to decrementing the input iterator step times.
TYPED_TEST(PrevPositiveStepTest, ReturnIteratorIsInputIteratorDecrementedStepTypes) {
  auto itr = this->end();
  for (auto const steps : std::initializer_list<int>{0U, 1U, 2U, this->ssize()}) {
    auto expected_itr = itr;
    for (int count = 0; count < steps; ++count) {
      expected_itr--;
    }
    EXPECT_EQ(::arene::base::prev(itr, steps), expected_itr);
  }
}

template <typename Container>
using PrevNegativeStepTest = PrevTest<Container>;

TYPED_TEST_SUITE(PrevNegativeStepTest, iterator_tests::at_least_bidirectional_iterator_containers, );

/// @test Given a negative step and an at least bidirectional iterator, @c arene::base::prev produces an iterator
/// equivalent to incrementing the input iterator step times.
TYPED_TEST(PrevNegativeStepTest, ReturnIteratorIsInputIteratorIncrementedStepTypes) {
  auto itr = this->begin();
  for (auto const steps : std::initializer_list<int>{-1, -2, -this->ssize()}) {
    auto expected_itr = itr;
    for (int count = 0; count > steps; --count) {
      expected_itr++;
    }
    EXPECT_EQ(::arene::base::prev(itr, steps), expected_itr);
  }
}

/// @test Validates @c ::arene::base::prev functions in a constexpr context with a positive step.
TEST(PrevConstexprTest, ReturnIteratorIsInputIteratorIncrementedStepTypes) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) c-array is only constexpr container in stdlib in C++14.
  constexpr int arr[5]{1, 2, 3, 4, 5};
  STATIC_ASSERT_EQ(::arene::base::prev(std::end(arr), 1), &arr[4]);
}
/// @test Validates @c ::arene::base::prev functions in a constexpr context with a negative step.
TEST(PrevConstexprTest, ReturnIteratorIsInputIteratorDecrementedStepTypes) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) c-array is only constexpr container in stdlib in C++14.
  constexpr int arr[5]{1, 2, 3, 4, 5};
  STATIC_ASSERT_EQ(::arene::base::prev(std::begin(arr), -1), &arr[1]);
}

}  // namespace
