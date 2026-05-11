// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/inline_container/set.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

namespace {

using ::arene::base::inline_set;

// A capacity large enough for most tests
constexpr std::size_t standard_capacity = 20;

// A set with a large enough capacity for most tests
template <typename T>
using standard_capacity_set = inline_set<T, standard_capacity>;

// A set with a fixed type and capacity to be used in most tests
using standard_set = standard_capacity_set<std::int32_t>;

/// @test Derefencing a default-constructed `inline_set::iterator` terminates the program with a precondition violation
TEST(InlineSetIteratorDeathTest, DereferencingADefaultConstructedIteratorIsAPreconditionViolation) {
  ASSERT_DEATH(std::ignore = *standard_set::iterator{}, "Precondition");
}

/// @test Comparing iterators from different `inline_set` instances terminates the program with a precondition violation
TEST(InlineSetIteratorDeathTest, ComparingIteratorsFromDifferentSetsIsAPreconditionViolation) {
  standard_set const first_set{1, 2, 3};
  standard_set const second_set{4, 5, 6};
  ASSERT_DEATH(std::ignore = first_set.begin() == second_set.begin(), "Precondition");
  ASSERT_DEATH(std::ignore = first_set.begin() == standard_set::iterator{}, "Precondition");
}

/// @test Constructing an `inline_set` with an initializer holding more elements than the capacity terminates the
/// program with a precondition violation
TEST(InlineSetCtorDeathTest, IfInitListTooLongIsPrecondition) {
  constexpr std::int32_t capacity = 3;
  using set_t = inline_set<std::int32_t, capacity>;

  ASSERT_DEATH((set_t{1, 2, 3, 4}), "Precondition");
}

/// @test Constructing an `inline_set` with an iterator range holding more elements than the capacity terminates the
/// program with a precondition violation
TEST(InlineSetCtorDeathTest, IfIteratorsContainMoreUniqueElementsThanCapacityIsPrecondition) {
  arene::base::array<std::int32_t, standard_set::capacity() + 1> elements{};
  for (std::int32_t idx = 0; ::arene::base::cmp_less(idx, standard_set::capacity() + 1); ++idx) {
    elements[static_cast<std::size_t>(idx)] = idx;
  }

  ASSERT_DEATH((standard_set{elements.begin(), elements.end()}), "Precondition");
}

/// @test Invoking `emplace` when the size of the `inline_set` is equal to its capacity terminates the program with a
/// precondition violation
TEST(InlineSetEmplaceDeathTest, IfSetAtCapacityAnEmplaceInsertsIsPrecondition) {
  constexpr std::int32_t capacity = 3;
  using set_t = inline_set<std::int32_t, capacity>;

  set_t set{1, 2, 3};

  ASSERT_DEATH(set.emplace(4), "Precondition");
  ASSERT_EQ(set.size(), 3);
  auto itr = set.begin();
  ASSERT_EQ(*itr, 1);
  ++itr;
  ASSERT_EQ(*itr, 2);
  ++itr;
  ASSERT_EQ(*itr, 3);
}

/// @test Constructing an `inline_set` from an source set with a size that is larger than the capacity of the
/// destination terminates the program with a precondition violation
TEST(InlineSetCtorDeathTest, MoveConstructionIfSourceHasTooManyElementsIsPrecondition) {
  constexpr std::int32_t larger_capacity = 20;
  using set1_t = inline_set<std::int32_t, larger_capacity>;
  constexpr std::int32_t smaller_capacity = 3;
  using set2_t = inline_set<std::int32_t, smaller_capacity>;

  set1_t set{10, 20, 30, 40, 50, 60, 70};

  ASSERT_DEATH(set2_t{std::move(set)}, "Precondition");
}

/// @test Constructing an `inline_set` from a set with a size that is larger than the capacity of the destination
/// terminates the program with a precondition violation
TEST(InlineSetCtorDeathTest, CopyConstructionIfSourceHasTooManyElementsIsPrecondition) {
  constexpr std::int32_t capacity1 = 20;
  using set1_t = inline_set<std::int32_t, capacity1>;
  constexpr std::int32_t capacity2 = 3;
  using set2_t = inline_set<std::int32_t, capacity2>;

  set1_t const set{10, 20, 30, 40, 50, 60, 70};

  ASSERT_DEATH(set2_t{set}, "Precondition");
}

/// @test Assigning to an `inline_set` from another set with a size that is larger than the capacity of the destination
/// terminates the program with a precondition violation
TEST(InlineSetAssignmentDeathTest, CopyAssignmentFromLargerCapacityIfSourceIsTooLargeIsPrecondition) {
  constexpr std::int32_t capacity1 = 5;
  using set1_t = inline_set<std::int32_t, capacity1>;
  constexpr std::int32_t capacity2 = 2;
  using set2_t = inline_set<std::int32_t, capacity2>;
  static_assert(std::is_assignable<set2_t, set1_t const&>::value, "Must be copy assignable");

  set1_t const set1{1, 2, 3};
  set2_t set2;
  ASSERT_DEATH(set2 = set1, "Precondition");
}

/// @test Assigning to an `inline_set` from an rvalue `inline_set` with a size that is larger than the capacity of the
/// destination terminates the program with a precondition violation
TEST(InlineSetAssignmentDeathTest, MoveAssignmentFromLargerCapacityIfSourceIsTooLargeIsPrecondition) {
  constexpr std::int32_t capacity1 = 5;
  using set1_t = inline_set<std::int32_t, capacity1>;
  constexpr std::int32_t capacity2 = 2;
  using set2_t = inline_set<std::int32_t, capacity2>;
  static_assert(std::is_assignable<set2_t, set1_t const&>::value, "Must be move assignable");

  set1_t set1{1, 2, 3};
  set2_t set2;
  ASSERT_DEATH(set2 = std::move(set1), "Precondition");
}

}  // namespace
