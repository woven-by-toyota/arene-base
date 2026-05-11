// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/result.hpp"
#include "arene/base/result/detail/in_place_error.hpp"
#include "arene/base/result/detail/in_place_value.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/move.hpp"

namespace {

using arene::base::in_place_error;
using arene::base::in_place_value;
using arene::base::result;

/// @test Given an @c arene::base::result<T,E> for which @c has_value() returns @c false , it is an
/// @c ARENE_PRECONDITION violation to invoke @c value() .
TEST(ResultValueDeathTest, IsPreconditionViolationIfValueChannelIsNotPopulated) {
  {
    using result_t = result<int, int>;
    result_t res{in_place_error, 1};
    auto const& const_r = res;
    ASSERT_DEATH(std::ignore = res.value(), "has_value");
    ASSERT_DEATH(std::ignore = const_r.value(), "has_value");
    ASSERT_DEATH(std::ignore = std::move(res).value(), "has_value");
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    ASSERT_DEATH(std::ignore = std::move(const_r).value(), "has_value");
  }
}

/// @test Given an @c arene::base::result<void,E> for which @c has_value() returns @c false , it is an
/// @c ARENE_PRECONDITION violation to invoke @c value() .
TEST(ResultValueDeathTest, VoidValueIsPreconditionViolationIfValueChannelIsNotPopulated) {
  {
    using result_t = result<void, int>;
    result_t res{in_place_error, 1};
    auto const& const_r = res;
    ASSERT_DEATH(res.value(), "has_value");
    ASSERT_DEATH(const_r.value(), "has_value");
    ASSERT_DEATH(std::move(res).value(), "has_value");
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    ASSERT_DEATH(std::move(const_r).value(), "has_value");
  }
}

/// @test Given an @c arene::base::result<T,E> for which @c has_error() returns @c false , it is an
/// @c ARENE_PRECONDITION violation to invoke @c error() .
TEST(ResultErrorDeathTest, IsPreconditionViolationIfErrorChannelIsNotPopulated) {
  {
    using result_t = result<int, int>;
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    result_t res{in_place_value, 1};
    auto const& const_r = res;
    ASSERT_DEATH(std::ignore = res.error(), "has_error");
    ASSERT_DEATH(std::ignore = const_r.error(), "has_error");
    ASSERT_DEATH(std::ignore = std::move(res).error(), "has_error");
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    ASSERT_DEATH(std::ignore = std::move(const_r).error(), "has_error");
  }
}

/// @brief Helper type which stores the sum of its arguments
///
/// This helper type adds the two arguments to the ocnstructor and stores the result as a data member. This is used to
/// verify that the in_place constructors forreclty forward arguments.
struct value_sum_constructor_args {
  explicit value_sum_constructor_args(int& lhs, int&& rhs)
      : value(lhs + rhs) {}
  int value;
};

/// @test Given an @c arene::base::result<T,E> for which @c has_value() returns @c false , invoking @c operator->() is
/// an @c ARENE_PRECONDITION violation.
TEST(ResultArrowOperatorDeathTest, IsPreconditionViolationIfValueChannelIsNotPopulated) {
  result<value_sum_constructor_args, int> res = in_place_error;
  auto const& const_r = res;
  ASSERT_DEATH(std::ignore = res.operator->(), "has_value");
  ASSERT_DEATH(std::ignore = const_r.operator->(), "has_value");
}

/// @test Given an @c arene::base::result<T,E> for which
/// @c has_value() returns @c false , invoking @c operator*() is an
/// @c ARENE_PRECONDITION violation.
TEST(ResultDeferenceOperatorDeathTest, IsPreconditionViolationIfValueChannelIsNotPopulated) {
  {
    result<value_sum_constructor_args, int> res = in_place_error;
    auto const& const_r = res;
    ASSERT_DEATH(std::ignore = *res, "has_value");
    ASSERT_DEATH(std::ignore = *const_r, "has_value");
  }
}

/// @test Given an @c arene::base::result<void,E> for which
/// @c has_value() returns @c false , invoking @c operator*() is an
/// @c ARENE_PRECONDITION violation.
TEST(ResultDeferenceOperatorDeathTest, VoidValueIsPreconditionViolationIfValueChannelIsNotPopulated) {
  {
    result<void, int> res = in_place_error;  // NOLINT(misc-const-correctness) this must be a non-const lvalue
    auto const& const_r = res;
    ASSERT_DEATH(*res, "has_value");
    ASSERT_DEATH(*const_r, "has_value");
  }
}
}  // namespace
