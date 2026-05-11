// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/fill.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <list>
#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/test_iterators.hpp"
#include "arene/base/array.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::fill;

/// @test Verify `fill` is noexcept if all of the operators of the iterators and value are noexcept.
TEST(FillNoexcept, TrueIfIncrementAndAssignmentAreNoexcept) {
  STATIC_ASSERT_TRUE(
      noexcept(fill(std::declval<noexcept_iterator>(), std::declval<noexcept_iterator>(), std::declval<test_value>()))
  );
}

/// @test Verify `fill` is not noexcept if the increment operator of the iterators is not noexcept.
TEST(FillNoexcept, FalseIfIncrementIsNotNoexcept) {
  STATIC_ASSERT_FALSE(noexcept(fill(
      std::declval<throwing_increment_iterator>(),
      std::declval<throwing_increment_iterator>(),
      std::declval<test_value>()
  )));
}

/// @test Verify `fill` is not noexcept if the copy assignment operator of the iterators is not noexcept.
TEST(FillNoexcept, FalseIfCopyAssignmentIsNotNoexcept) {
  STATIC_ASSERT_FALSE(noexcept(fill(
      std::declval<std::vector<std::string>::iterator>(),
      std::declval<std::vector<std::string>::iterator>(),
      std::declval<std::string>()
  )));
}

// Simple data makers that return fixed values
// @{
auto initial_value(int) -> int { return 1; }
auto make_value(int) -> int { return 10; }
auto initial_value(std::string const&) -> std::string { return "1"; }
auto make_value(std::string const&) -> std::string { return "10"; }
// @}

// base class for templated test fixtures.
template <typename T>
class FillTest : public ::testing::Test {
 public:
  FillTest()
      : non_empty_container(10U, initial_value(typename T::value_type{})) {}

  T non_empty_container;
};
using sequence_types =
    ::testing::Types<std::vector<int>, std::list<int>, std::vector<std::string>, std::list<std::string>>;

TYPED_TEST_SUITE(FillTest, sequence_types, );

/// @test `fill` with an empty sequence is a no-op.
TYPED_TEST(FillTest, ForEmptySequenceIsNoOp) {
  TypeParam empty{};
  EXPECT_NO_FATAL_FAILURE(fill(empty.begin(), empty.end(), make_value(typename TypeParam::value_type{})));
  EXPECT_TRUE(empty.empty());

  auto const original_non_empty_container = this->non_empty_container;
  for (auto itr = this->non_empty_container.begin(); itr != this->non_empty_container.end(); ++itr) {
    EXPECT_NO_FATAL_FAILURE(fill(itr, itr, make_value(typename TypeParam::value_type{})));
    EXPECT_EQ(this->non_empty_container, original_non_empty_container);
  }
}

/// @test `fill` a container with a value replaces all elements with that value.
TYPED_TEST(FillTest, ReplacesElementsInSequenceWithValue) {
  {
    SCOPED_TRACE("Full fill");
    auto const fill_value = make_value(typename TypeParam::value_type{});
    auto const start_itr = this->non_empty_container.begin();
    auto const end_itr = this->non_empty_container.end();

    EXPECT_NO_FATAL_FAILURE(fill(start_itr, end_itr, fill_value));
    auto const filled_subset = TypeParam{start_itr, end_itr};
    EXPECT_THAT(filled_subset, ::testing::Each(::testing::Eq(fill_value)));
  }

  {
    SCOPED_TRACE("Subset fill");
    auto const fill_value = make_value(typename TypeParam::value_type{});
    auto const start_itr = std::next(this->non_empty_container.begin(), 1);
    auto const end_itr = std::prev(this->non_empty_container.end(), 1);

    EXPECT_NO_FATAL_FAILURE(fill(start_itr, end_itr, fill_value));
    auto const filled_subset = TypeParam{start_itr, end_itr};
    EXPECT_THAT(filled_subset, ::testing::Each(::testing::Eq(fill_value)));
  }
}

/// @test `fill` part of a container with a value does not alter elements outside of the sequence.
TYPED_TEST(FillTest, DoesNotAlterElementsOutsideOfSequence) {
  auto const start_itr = std::next(this->non_empty_container.begin(), 1);
  auto const end_itr = std::prev(this->non_empty_container.end(), 1);
  auto const original_leading_unfilled_subset = TypeParam{this->non_empty_container.begin(), start_itr};
  auto const original_trailing_unfilled_subset = TypeParam{this->non_empty_container.begin(), start_itr};

  EXPECT_NO_FATAL_FAILURE(fill(start_itr, end_itr, make_value(typename TypeParam::value_type{})));
  auto const leading_unfilled_subset = TypeParam{this->non_empty_container.begin(), start_itr};
  EXPECT_EQ(leading_unfilled_subset, original_leading_unfilled_subset);
  auto const trailing_unfilled_subset = TypeParam{end_itr, this->non_empty_container.end()};
  EXPECT_EQ(trailing_unfilled_subset, original_trailing_unfilled_subset);
}

/// constexpr wrapper around fill
/// @returns A copy of the input with a (begin,end) fill of value performed on it.
template <typename T, std::size_t Size>
constexpr auto do_full_fill(arene::base::array<T, Size> original, T value) -> arene::base::array<T, Size> {
  fill(original.begin(), original.end(), value);
  return original;
}

/// constexpr wrapper around fill
/// @returns A copy of the input with a (begin,begin) fill of value performed on it.
template <typename T, std::size_t Size>
constexpr auto do_empty_fill(arene::base::array<T, Size> original, T value) -> arene::base::array<T, Size> {
  fill(original.begin(), original.begin(), value);
  return original;
}

/// @test `fill` can be executed in a constexpr context.
TEST(FillConstexpr, CanBeExecutedInConstexpr) {
  STATIC_ASSERT_EQ(
      do_full_fill(arene::base::array<int, 5>{1, 2, 3, 4, 5}, -10),
      (arene::base::array<int, 5>{-10, -10, -10, -10, -10})
  );

  STATIC_ASSERT_EQ(
      do_empty_fill(arene::base::array<int, 5>{1, 2, 3, 4, 5}, -10),
      (arene::base::array<int, 5>{1, 2, 3, 4, 5})
  );
}

}  // namespace
