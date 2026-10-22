// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/generate.hpp"

#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/algorithm/tests/test_iterators.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

using ::arene::base::generate;

/// @brief A generator counting up from zero, one step per invocation.
/// @tparam Movable Whether the generator supports move construction.
template <bool Movable = true>
class counter {
 public:
  /// @brief produce the next value of the sequence
  /// @return the number of times this generator has been invoked, including this invocation
  constexpr auto operator()() noexcept -> int {
    ++value_;
    return value_;
  }

 private:
  /// @brief number of invocations so far
  int value_{0};
};

/// @brief A copyable counter specialization that cannot be moved.
template <>
class counter<false> : public counter<true> {
 public:
  constexpr counter() noexcept = default;
  constexpr counter(counter const&) noexcept = default;
  counter(counter&&) = delete;
  auto operator=(counter const&) noexcept -> counter& = default;
  auto operator=(counter&&) -> counter& = delete;
  ~counter() = default;
};

/// @brief A generator producing the value type of the test iterators without throwing
struct noexcept_generator {
  /// @brief produce a value
  /// @return an unspecified value
  auto operator()() noexcept -> test_value;
};

/// @brief A generator whose invocation is not marked noexcept
struct throwing_generator {
  /// @brief produce a value, potentially by throwing
  /// @return an unspecified value
  auto operator()() -> test_value;
};

/// @brief A generator producing values whose assignment is not marked noexcept
struct throwing_assignment_generator {
  /// @brief produce a value whose assignment may throw
  /// @return an unspecified value
  auto operator()() noexcept -> throwing_assignment_iterator::value;
};

/// @brief A sentinel that compares an iterator against a stored end iterator.
/// @tparam Iterator The iterator type to store.
template <typename Iterator>
class iterator_sentinel {
 public:
  /// @brief Construct a sentinel for the given end iterator.
  /// @param end The end iterator represented by the sentinel.
  explicit constexpr iterator_sentinel(Iterator end) noexcept
      : end_{std::move(end)} {}

  /// @brief Compare an iterator with the sentinel.
  /// @param iterator The iterator to compare.
  /// @param sentinel The sentinel to compare.
  /// @return Whether the iterator has not reached the sentinel.
  friend constexpr auto operator!=(Iterator const& iterator, iterator_sentinel const& sentinel) noexcept -> bool {
    return iterator != sentinel.end_;
  }

 private:
  Iterator end_;
};

/// @test Verify `generate` accepts forward iterators but rejects input iterators.
TEST(GenerateConstraints, RequiresForwardIterator) {
  using throws_on = ::testing::throws_on_specifiers;
  using forward_iterator =
      ::testing::noexcept_configurable_iterator<throws_on::nothing, std::forward_iterator_tag, test_value>;
  using input_iterator =
      ::testing::noexcept_configurable_iterator<throws_on::nothing, std::input_iterator_tag, test_value>;

  STATIC_ASSERT_TRUE(
      (arene::base::is_invocable_v<decltype(generate), forward_iterator, forward_iterator, noexcept_generator>)
  );
  STATIC_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(generate), input_iterator, input_iterator, noexcept_generator>)
  );
}

/// @test Verify `generate` is noexcept if the iterator operations, the generator and the assignment are noexcept.
TEST(GenerateNoexcept, TrueIfIncrementAssignmentAndGeneratorAreNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(
      generate(std::declval<noexcept_iterator>(), std::declval<noexcept_iterator>(), std::declval<noexcept_generator>())
  ));
}

/// @test Verify `generate` is not noexcept if the increment operator of the iterators is not noexcept.
TEST(GenerateNoexcept, FalseIfIncrementIsNotNoexcept) {
  STATIC_ASSERT_FALSE(noexcept(generate(
      std::declval<throwing_increment_iterator>(),
      std::declval<throwing_increment_iterator>(),
      std::declval<noexcept_generator>()
  )));
}

/// @test Verify `generate` is not noexcept if the inequality comparison of the iterators is not noexcept.
TEST(GenerateNoexcept, FalseIfInequalityComparisonIsNotNoexcept) {
  STATIC_ASSERT_FALSE(noexcept(generate(
      std::declval<throwing_comparison_iterator>(),
      std::declval<throwing_comparison_iterator>(),
      std::declval<noexcept_generator>()
  )));
}

/// @test Verify `generate` is not noexcept if invoking the generator is not noexcept.
TEST(GenerateNoexcept, FalseIfGeneratorIsNotNoexcept) {
  STATIC_ASSERT_FALSE(noexcept(
      generate(std::declval<noexcept_iterator>(), std::declval<noexcept_iterator>(), std::declval<throwing_generator>())
  ));
}

/// @test Verify `generate` is not noexcept if assigning the generated value is not noexcept.
TEST(GenerateNoexcept, FalseIfAssignmentIsNotNoexcept) {
  STATIC_ASSERT_FALSE(noexcept(generate(
      std::declval<throwing_assignment_iterator>(),
      std::declval<throwing_assignment_iterator>(),
      std::declval<throwing_assignment_generator>()
  )));
}

/// @test `generate` assigns each element a successive invocation of the generator, in order.
CONSTEXPR_TEST(GenerateTest, AssignsSuccessiveInvocationsInOrder) {
  arene::base::array<int, 5> values{};

  generate(values.begin(), values.end(), counter<>{});

  CONSTEXPR_ASSERT_EQ(values, (arene::base::array<int, 5>{1, 2, 3, 4, 5}));
}

/// @test Given a copyable generator that cannot be moved, when `generate` is called with the generator as an lvalue,
/// then it assigns each element a successive invocation of the generator.
CONSTEXPR_TEST(GenerateTest, SupportsNonMovableGenerator) {
  arene::base::array<int, 5> values{};
  counter<false> const generator{};

  generate(values.begin(), values.end(), generator);

  CONSTEXPR_ASSERT_EQ(values, (arene::base::array<int, 5>{1, 2, 3, 4, 5}));
}

/// @test Given a sentinel whose type differs from the iterator, when `generate` is called, then it assigns each element
/// a successive invocation of the generator.
TEST(GenerateTest, SupportsSentinelWithDifferentTypeFromIterator) {
  arene::base::array<int, 5> values{};

  generate(values.begin(), iterator_sentinel<decltype(values.end())>{values.end()}, counter<>{});

  EXPECT_EQ(values, (arene::base::array<int, 5>{1, 2, 3, 4, 5}));
}

/// @test `generate` over an empty sequence is a no-op.
CONSTEXPR_TEST(GenerateTest, ForEmptySequenceIsNoOp) {
  arene::base::array<int, 5> values{6, 7, 8, 9, 10};

  generate(values.begin(), values.begin(), counter<>{});

  CONSTEXPR_ASSERT_EQ(values, (arene::base::array<int, 5>{6, 7, 8, 9, 10}));
}

/// @test The range overload of `generate` assigns each element a successive invocation of the generator, in order.
CONSTEXPR_TEST(GenerateTest, RangeOverloadAssignsSuccessiveInvocationsInOrder) {
  arene::base::array<int, 5> values{};

  generate(values, counter<>{});

  CONSTEXPR_ASSERT_EQ(values, (arene::base::array<int, 5>{1, 2, 3, 4, 5}));
}

/// @test Verify the range overload of `generate` propagates the noexcept specification of the iterator overload.
TEST(GenerateNoexcept, RangeOverloadPropagatesNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(generate(std::declval<arene::base::array<int, 5>&>(), std::declval<counter<>>())));
  STATIC_ASSERT_FALSE(
      noexcept(generate(std::declval<arene::base::array<test_value, 5>&>(), std::declval<throwing_generator>()))
  );
}

/// @test `generate` invokes the generator and assigns exactly once per element of the sequence.
TEST(GenerateComplexity, InvokesGeneratorAndAssignsOncePerElement) {
  using counted_value = ::testing::counting_wrapper<int>;
  arene::base::array<counted_value, 5> values{};
  counted_value::count = {};

  auto const result = ::testing::counted_algorithm(generate, values.begin(), values.end(), counter<>{});

  EXPECT_EQ(result.applications, 5U);
  EXPECT_EQ(counted_value::count.move_assign + counted_value::count.copy_assign, 5U);
}

/// @test `generate` does not alter elements outside of the sequence.
CONSTEXPR_TEST(GenerateTest, DoesNotAlterElementsOutsideOfSequence) {
  arene::base::array<int, 5> values{-1, 0, 0, 0, -1};

  generate(values.begin() + 1, values.end() - 1, counter<>{});

  CONSTEXPR_ASSERT_EQ(values, (arene::base::array<int, 5>{-1, 1, 2, 3, -1}));
}

}  // namespace
