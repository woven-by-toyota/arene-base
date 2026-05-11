// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/tests/test_iterators.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/initializer_list"
#include "stdlib/include/iterator"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

struct type_with_swap_fn {
  std::size_t value;
  bool swapped{false};

  constexpr explicit type_with_swap_fn(std::size_t value_in)
      : value(value_in) {}
  constexpr auto operator==(type_with_swap_fn const& rhs) const -> bool { return rhs.value == value; }

  friend constexpr auto swap(type_with_swap_fn& lhs, type_with_swap_fn& rhs) {
    auto temp = lhs.value;
    lhs.value = rhs.value;
    rhs.value = temp;

    lhs.swapped = true;
    rhs.swapped = true;
  }
};

using reverse_types =
    arene::base::type_lists::concat_unique_t<::testing::signed_integer_types, ::testing::unsigned_integer_types>;

template <typename T>
class ReverseTest : public testing::Test {};

TYPED_TEST_SUITE(ReverseTest, reverse_types, );

/// @test @c std::reverse is not invocable if one of the inputs is not a bidirectional iterator
TYPED_TEST(ReverseTest, NotInvocableIfNotAnIterator) {
  struct not_an_iterator {};
  auto const lifted_reverse = FUNCTION_LIFT(std::reverse);

  EXPECT_TRUE(::testing::simple_is_invocable_v<decltype(lifted_reverse), TypeParam*, TypeParam*>);
  EXPECT_TRUE(::testing::simple_is_invocable_v<
              decltype(lifted_reverse),
              testing::demoted_iterator<TypeParam*, std::bidirectional_iterator_tag>,
              testing::demoted_iterator<TypeParam*, std::bidirectional_iterator_tag>>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_reverse),
               testing::demoted_iterator<TypeParam*, std::input_iterator_tag>,
               testing::demoted_iterator<TypeParam*, std::input_iterator_tag>>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_reverse),
               testing::demoted_iterator<TypeParam*, std::output_iterator_tag>,
               testing::demoted_iterator<TypeParam*, std::output_iterator_tag>>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_reverse),
               testing::demoted_iterator<TypeParam*, std::forward_iterator_tag>,
               testing::demoted_iterator<TypeParam*, std::forward_iterator_tag>>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<decltype(lifted_reverse), not_an_iterator, not_an_iterator>);
}

/// @test @c std::reverse is not invocable if the value type is not swappable
TYPED_TEST(ReverseTest, NotInvocableIfNotSwappable) {
  auto const lifted_reverse = FUNCTION_LIFT(std::reverse);

  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_reverse),
               testing::not_move_constructible*,
               testing::not_move_constructible*>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_reverse),
               testing::not_move_assignable*,
               testing::not_move_assignable*>);
}

/// @test @c std::reverse is noexcept if the iterator operations are noexcept and swap is noexcept
CONSTEXPR_TYPED_TEST(ReverseTest, ReverseIsNoexcept) {
  EXPECT_TRUE(noexcept(std::reverse(std::declval<TypeParam*>(), std::declval<TypeParam*>())));
  EXPECT_TRUE(noexcept(std::reverse(std::declval<noexcept_iterator>(), std::declval<noexcept_iterator>())));
}

/// @test @c std::reverse is not noexcept if the iterator operations are not noexcept
CONSTEXPR_TEST(ReverseTest, ReverseIsNotNoexceptIfIteratorThrows) {
  EXPECT_FALSE(
      noexcept(std::reverse(std::declval<throwing_increment_iterator>(), std::declval<throwing_increment_iterator>()))
  );
  EXPECT_FALSE(
      noexcept(std::reverse(std::declval<throwing_decrement_iterator>(), std::declval<throwing_decrement_iterator>()))
  );
  EXPECT_FALSE(
      noexcept(std::reverse(std::declval<throwing_comparison_iterator>(), std::declval<throwing_comparison_iterator>()))
  );
  EXPECT_FALSE(
      noexcept(std::reverse(std::declval<throwing_reference_iterator>(), std::declval<throwing_reference_iterator>()))
  );
  EXPECT_FALSE(
      noexcept(std::reverse(std::declval<throwing_assignment_iterator>(), std::declval<throwing_assignment_iterator>()))
  );
  EXPECT_FALSE(noexcept(std::reverse(std::declval<throwing_iterator>(), std::declval<throwing_iterator>())));
}

/// @test @c std::reverse is not noexcept if default swap function throws
CONSTEXPR_TEST(ReverseTest, ReverseIsNotNoexceptIfDefaultSwapThrows) {
  EXPECT_FALSE(noexcept(std::reverse(
      std::declval<testing::throwable_move_construction*>(),
      std::declval<testing::throwable_move_construction*>()
  )));
  EXPECT_FALSE(noexcept(std::reverse(
      std::declval<testing::throwable_move_assignment*>(),
      std::declval<testing::throwable_move_assignment*>()
  )));
}

struct has_throwing_swap {
  friend constexpr auto swap(has_throwing_swap&, has_throwing_swap&) noexcept(false) -> void {}
};

/// @test @c std::reverse is not noexcept if ADL swap function throws
CONSTEXPR_TEST(ReverseTest, ReverseIsNotNoexceptIfADLSwapThrows) {
  // Provide a use of this swap function to satisfy the compiler.
  has_throwing_swap value;
  swap(value, value);

  EXPECT_FALSE(noexcept(std::reverse(std::declval<has_throwing_swap*>(), std::declval<has_throwing_swap*>())));
}

/// @test @c std::reverse reverses all elements in a range
CONSTEXPR_TYPED_TEST(ReverseTest, ElementsInRangeAreReversed) {
  constexpr auto size = std::size_t{5};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  TypeParam values[size] = {TypeParam{1}, TypeParam{2}, TypeParam{3}, TypeParam{4}, TypeParam{5}};
  auto range = testing::demoted_view(std::bidirectional_iterator_tag{}, values);

  std::reverse(std::begin(range), std::end(range));

  for (std::size_t i = 0; i < size; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    EXPECT_EQ(values[i], static_cast<TypeParam>(size - i));
  }
}

/// @test @c std::reverse uses an ADL found swap function to swap elements of range
CONSTEXPR_TEST(ReverseTest, ADLSwapIsUsed) {
  using T = type_with_swap_fn;
  constexpr auto size = std::size_t{6};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  T values[size] = {T{1}, T{2}, T{3}, T{4}, T{5}, T{6}};
  auto range = testing::demoted_view(std::bidirectional_iterator_tag{}, values);

  std::reverse(std::begin(range), std::end(range));

  for (std::size_t i = 0; i < size; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    EXPECT_EQ(values[i], T{size - i});
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    EXPECT_TRUE(values[i].swapped);
  }
}

/// @test @c std::reverse does not reverse any elements of a zero length range
CONSTEXPR_TYPED_TEST(ReverseTest, ZeroLengthRangeNothingIsReversed) {
  constexpr auto size = std::size_t{5};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  TypeParam values[size] = {TypeParam{0}, TypeParam{1}, TypeParam{2}, TypeParam{3}, TypeParam{4}};
  auto range = testing::demoted_view(std::bidirectional_iterator_tag{}, values);

  std::reverse(std::begin(range), begin(range));

  for (std::size_t i = 0; i < size; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    EXPECT_EQ(values[i], static_cast<TypeParam>(i));
  }
}

/// @test @c std::reverse can be used on a subset of a range
CONSTEXPR_TYPED_TEST(ReverseTest, ReverseASubsetOfARange) {
  constexpr auto size = std::size_t{5};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  TypeParam values[size] = {TypeParam{0}, TypeParam{1}, TypeParam{2}, TypeParam{3}, TypeParam{4}};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  auto range = testing::demoted_view(std::bidirectional_iterator_tag{}, std::begin(values), std::begin(values) + 3);

  std::reverse(std::begin(range), std::end(range));

  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
  EXPECT_EQ(values[0], TypeParam{2});
  EXPECT_EQ(values[1], TypeParam{1});
  EXPECT_EQ(values[2], TypeParam{0});
  EXPECT_EQ(values[3], TypeParam{3});
  EXPECT_EQ(values[4], TypeParam{4});
  // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
}

struct type_with_swap_counter {
  std::size_t* counter{nullptr};

  friend constexpr auto swap(type_with_swap_counter& lhs, type_with_swap_counter&) -> void { ++(*lhs.counter); }
};

/// @return A reference to the last element of @c array
template <typename Element, size_t Size>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr auto back(Element const (&array)[Size]) noexcept -> Element const& {
  return array[Size - 1];
}

/// @test @c std::reverse meets the documented algorithmic complexity requirements
CONSTEXPR_TEST(ReverseTest, MeetsComplexityRequirements) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  constexpr std::size_t sizes[6] = {1, 5, 10, 100, 999, 12345};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  constexpr auto max_size = back(sizes);
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  type_with_swap_counter values[max_size];

  auto swap_counter = std::size_t{};
  for (auto& value : values) {
    value = type_with_swap_counter{&swap_counter};
  }

  for (auto const& size : sizes) {
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    auto range =
        testing::demoted_view(std::bidirectional_iterator_tag{}, std::begin(values), std::begin(values) + size);
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::reverse(std::begin(range), std::end(range));

    EXPECT_EQ(swap_counter, size / 2);
    swap_counter = 0;
  }
}

}  // namespace
