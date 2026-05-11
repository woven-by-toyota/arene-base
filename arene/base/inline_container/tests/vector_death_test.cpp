// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/inline_container/tests/testing_types.hpp"  // IWYU pragma: keep
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"

namespace {

using ::arene::base::inline_vector;

constexpr std::size_t standard_capacity = 25;

template <typename T>
using standard_capacity_vector = inline_vector<T, standard_capacity>;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) These global counters are used for white box testing
struct op_tracker {
  static std::size_t default_constructor_count;
  static std::size_t copy_constructor_count;
  static std::size_t move_constructor_count;
  static std::size_t copy_assign_count;
  static std::size_t move_assign_count;
  static std::size_t destructor_count;
  static std::size_t swap_count;

  op_tracker() noexcept { ++default_constructor_count; }
  ~op_tracker() { ++destructor_count; }
  op_tracker(op_tracker&&) noexcept { ++move_constructor_count; }
  op_tracker(op_tracker const&) noexcept { ++copy_constructor_count; }
  auto operator=(op_tracker&&) noexcept -> op_tracker& {
    ++move_assign_count;
    return *this;
  }
  auto operator=(op_tracker const&) noexcept -> op_tracker& {
    ++copy_assign_count;
    return *this;
  }

  static void reset() {
    default_constructor_count = 0;
    copy_constructor_count = 0;
    move_constructor_count = 0;
    copy_assign_count = 0;
    move_assign_count = 0;
    destructor_count = 0;
    swap_count = 0;
  }
};

std::size_t op_tracker::default_constructor_count = 0;
std::size_t op_tracker::copy_constructor_count = 0;
std::size_t op_tracker::move_constructor_count = 0;
std::size_t op_tracker::copy_assign_count = 0;
std::size_t op_tracker::move_assign_count = 0;
std::size_t op_tracker::destructor_count = 0;
std::size_t op_tracker::swap_count = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

/// @test Invoking `resize` with a count that is larger than the capacity of the vector terminates the application with
/// a precondition violation
TEST(InlineVectorOpTrackerDeathTest, ResizingBeyondCapacityIsPreconditionViolation) {
  constexpr std::size_t initial_count = 5;
  standard_capacity_vector<op_tracker> vec(initial_count);
  op_tracker::reset();
  ASSERT_DEATH(vec.resize(standard_capacity + 1), "Precondition violation");
  EXPECT_EQ(vec.size(), initial_count);

  EXPECT_EQ(op_tracker::default_constructor_count, 0);
  EXPECT_EQ(op_tracker::move_constructor_count, 0);
  EXPECT_EQ(op_tracker::copy_constructor_count, 0);
  EXPECT_EQ(op_tracker::move_assign_count, 0);
  EXPECT_EQ(op_tracker::copy_assign_count, 0);
  EXPECT_EQ(op_tracker::destructor_count, 0);
}

/// @test Invoking the `inline_vector` constructor with an iterator range holding more elements than the capacity of the
/// vector terminates the program with a precondition violation
TEST(InlineVectorOpTrackerDeathTest, ConstructingFromOverLongBidirectionalRangeIsPreconditionViolation) {
  constexpr std::size_t capacity = 10;
  arene::base::array<op_tracker, capacity + 1> const source{};
  op_tracker::reset();

  ASSERT_DEATH((inline_vector<op_tracker, capacity>(source.begin(), source.end())), "Precondition violation");

  EXPECT_EQ(op_tracker::default_constructor_count, 0);
  EXPECT_EQ(op_tracker::move_constructor_count, 0);
  EXPECT_EQ(op_tracker::copy_constructor_count, 0);
  EXPECT_EQ(op_tracker::move_assign_count, 0);
  EXPECT_EQ(op_tracker::copy_assign_count, 0);
  EXPECT_EQ(op_tracker::destructor_count, 0);
}

}  // namespace
