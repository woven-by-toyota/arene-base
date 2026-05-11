// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/tests/test_iterators.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/functional"
#include "stdlib/include/initializer_list"
#include "stdlib/include/iterator"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

using adjacent_find_types =
    arene::base::type_lists::concat_unique_t<::testing::signed_integer_types, ::testing::unsigned_integer_types>;

template <typename T>
class AdjacentFindTest : public testing::Test {};

TYPED_TEST_SUITE(AdjacentFindTest, adjacent_find_types, );

/// @test @c std::adjacent_find is not invocable if one of the inputs is not a forward iterator.
TYPED_TEST(AdjacentFindTest, NotInvocableIfNotAnIterator) {
  auto const lifted_adjacent_find = FUNCTION_LIFT(std::adjacent_find);

  struct not_an_iterator {};

  EXPECT_FALSE(::testing::simple_is_invocable_v<decltype(lifted_adjacent_find), not_an_iterator, not_an_iterator>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_adjacent_find),
               testing::demoted_iterator<TypeParam*, std::input_iterator_tag>,
               testing::demoted_iterator<TypeParam*, std::input_iterator_tag>>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_adjacent_find),
               testing::demoted_iterator<TypeParam*, std::output_iterator_tag>,
               testing::demoted_iterator<TypeParam*, std::output_iterator_tag>>);
  EXPECT_TRUE(::testing::simple_is_invocable_v<
              decltype(lifted_adjacent_find),
              testing::demoted_iterator<TypeParam*, std::forward_iterator_tag>,
              testing::demoted_iterator<TypeParam*, std::forward_iterator_tag>>);
  EXPECT_TRUE(::testing::simple_is_invocable_v<
              decltype(lifted_adjacent_find),
              testing::demoted_iterator<TypeParam*, std::bidirectional_iterator_tag>,
              testing::demoted_iterator<TypeParam*, std::bidirectional_iterator_tag>>);
  EXPECT_TRUE(::testing::simple_is_invocable_v<
              decltype(lifted_adjacent_find),
              testing::demoted_iterator<TypeParam*, std::random_access_iterator_tag>,
              testing::demoted_iterator<TypeParam*, std::random_access_iterator_tag>>);
  EXPECT_TRUE(::testing::simple_is_invocable_v<decltype(lifted_adjacent_find), TypeParam*, TypeParam*>);
}

/// @test @c std::adjacent_find requires a binary predicate where the iterator
/// reference type is convertible for arg1 and arg2 and the predicate returns a type convertible to bool.
TEST(AdjacentFindTest, RequiresBinaryPredicate) {
  auto const is_invocable_with = [](auto iter, auto... cmp) {
    auto const adjacent_find = FUNCTION_LIFT(std::adjacent_find);
    return testing::simple_is_invocable_v<decltype(adjacent_find), decltype(iter), decltype(iter), decltype(cmp)...>;
  };

  using iterator_type = int*;

  // Invocable when input iterator reference type is convertible to predicate input type
  auto cmp_inputs_are_bool = [](bool, bool) -> bool { return true; };
  ASSERT_TRUE(is_invocable_with(iterator_type{}, cmp_inputs_are_bool));

  // Not invocable when input iterator reference type is not convertible to predicate input type
  struct empty {};
  auto cmp_inputs_are_empty = [](empty, empty) -> bool { return true; };
  ASSERT_FALSE(is_invocable_with(iterator_type{}, cmp_inputs_are_empty));

  // Invocable when predicate return type is convertible to bool
  ASSERT_TRUE(is_invocable_with(iterator_type{}, testing::always_constant<int>{}));

  // Not invocable when predicate return type is not convertible to bool
  ASSERT_FALSE(is_invocable_with(iterator_type{}, testing::always_constant<empty>{}));

  struct custom_equality {
    constexpr auto operator==(custom_equality const&) -> int { return {}; }
  };
  using custom_equality_iterator = custom_equality*;

  // Invocable when built-in operator== returns a type convertible to bool
  ASSERT_TRUE(is_invocable_with(custom_equality_iterator{}));

  struct bad_custom_equality {
    constexpr auto operator==(custom_equality const&) -> empty { return {}; }
  };
  using bad_custom_equality_iterator = bad_custom_equality*;

  // Not invocable when built-in operator== does not return a type convertible to bool
  ASSERT_FALSE(is_invocable_with(bad_custom_equality_iterator{}));
}

/// @test @c std::adjacent_find is noexcept if the iterator operations are noexcept and default predicate is noexcept.
CONSTEXPR_TYPED_TEST(AdjacentFindTest, AdjacentFindIsNoexcept) {
  EXPECT_TRUE(noexcept(std::adjacent_find(std::declval<TypeParam*>(), std::declval<TypeParam*>())));
  EXPECT_TRUE(noexcept(std::adjacent_find(std::declval<noexcept_int_iterator>(), std::declval<noexcept_int_iterator>()))
  );
}

/// @test @c std::adjacent_find is not noexcept if the iterator operations are not noexcept.
CONSTEXPR_TEST(AdjacentFindTest, AdjacentFindIsNotNoexceptIfIteratorThrows) {
  EXPECT_FALSE(noexcept(std::adjacent_find(
      std::declval<throwing_increment_int_iterator>(),
      std::declval<throwing_increment_int_iterator>()
  )));
  EXPECT_FALSE(noexcept(std::adjacent_find(
      std::declval<throwing_comparison_int_iterator>(),
      std::declval<throwing_comparison_int_iterator>()
  )));
  EXPECT_FALSE(noexcept(std::adjacent_find(
      std::declval<throwing_reference_int_iterator>(),
      std::declval<throwing_reference_int_iterator>()
  )));
  EXPECT_FALSE(noexcept(
      std::adjacent_find(std::declval<throwing_assignment_iterator>(), std::declval<throwing_assignment_iterator>())
  ));
  EXPECT_FALSE(noexcept(std::adjacent_find(std::declval<throwing_int_iterator>(), std::declval<throwing_int_iterator>())
  ));
}

template <bool Throws>
struct throwing_compare {
  constexpr auto operator==(throwing_compare const&) noexcept(!Throws) -> bool { return true; }
};

/// @test @c std::adjacent_find is not noexcept if the default comparison is not noexcept.
CONSTEXPR_TEST(AdjacentFindTest, AdjacentFindIsNotNoexceptIfDefaultComparisonThrows) {
  EXPECT_TRUE(
      noexcept(std::adjacent_find(std::declval<throwing_compare<false>*>(), std::declval<throwing_compare<false>*>()))
  );
  EXPECT_FALSE(
      noexcept(std::adjacent_find(std::declval<throwing_compare<true>*>(), std::declval<throwing_compare<true>*>()))
  );
}

/// @test @c std::adjacent_find is not noexcept if the predicate is not noexcept.
TEST(AdjacentFindTest, AdjacentFindIsNotNoexceptIfPredicateThrows) {
  auto const non_throwing_predicate = [](auto const&...) noexcept(true) { return true; };
  auto const throwing_predicate = [](auto const&...) noexcept(false) { return true; };

  EXPECT_TRUE(
      noexcept(std::adjacent_find(std::declval<int const*>(), std::declval<int const*>(), non_throwing_predicate))
  );
  EXPECT_FALSE(noexcept(std::adjacent_find(std::declval<int const*>(), std::declval<int const*>(), throwing_predicate))
  );
}

/// @test @c std::adjacent_find returns the iterator to the first of the adjacent elements at the beginning of the
/// range.
TYPED_TEST(AdjacentFindTest, AdjacentElementsAreFoundAtBeginning) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  constexpr TypeParam values[] = {1, 1, 2, 3, 4};
  auto const range = testing::demoted_view(std::forward_iterator_tag{}, values);

  auto const result = std::adjacent_find(std::begin(range), std::end(range));

  EXPECT_EQ(result, std::begin(range));
}

/// @test @c std::adjacent_find returns the iterator to the first of the adjacent elements in the middle of the range.
TYPED_TEST(AdjacentFindTest, AdjacentElementsAreFoundInMiddle) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  constexpr TypeParam values[] = {1, 2, 3, 3, 4};
  auto const range = testing::demoted_view(std::forward_iterator_tag{}, values);

  auto const result = std::adjacent_find(std::begin(range), std::end(range));

  EXPECT_EQ(result, arene::base::next(std::begin(range), 2));
}

/// @test @c std::adjacent_find returns the iterator to the first of the adjacent elements at the end of the range.
TYPED_TEST(AdjacentFindTest, AdjacentElementsAreFoundAtEnd) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  constexpr TypeParam values[] = {1, 2, 3, 4, 4};
  auto const range = testing::demoted_view(std::forward_iterator_tag{}, values);

  auto const result = std::adjacent_find(std::begin(range), std::end(range));

  EXPECT_EQ(result, arene::base::next(std::begin(range), 3));
}

/// @test @c std::adjacent_find returns the iterator to the first of the adjacent elements when there are more than two
/// adjacent elements.
TYPED_TEST(AdjacentFindTest, FirstAdjacentElementReturnedInLongerSequence) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  constexpr TypeParam values[] = {1, 4, 4, 4, 4};
  auto const range = testing::demoted_view(std::forward_iterator_tag{}, values);

  auto const result = std::adjacent_find(std::begin(range), std::end(range));

  EXPECT_EQ(result, arene::base::next(std::begin(range), 1));
}

/// @test @c std::adjacent_find returns the end iterator when there are no adjacent elements.
TYPED_TEST(AdjacentFindTest, NoAdjacentElementsInRange) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  constexpr TypeParam values[] = {1, 2, 3, 2, 1};
  auto const range = testing::demoted_view(std::forward_iterator_tag{}, values);

  auto const result = std::adjacent_find(std::begin(range), std::end(range));

  EXPECT_EQ(result, std::end(range));
}

/// @test @c std::adjacent_find returns the end iterator for an empty range.
TYPED_TEST(AdjacentFindTest, EmptyRange) {
  constexpr auto values = std::initializer_list<int>{};
  auto const range = testing::demoted_view(std::forward_iterator_tag{}, values);

  auto const result = std::adjacent_find(std::begin(range), std::end(range));

  EXPECT_EQ(result, std::end(range));
}

/// @test @c std::adjacent_find uses a custom predicate if provided.
TYPED_TEST(AdjacentFindTest, WithACustomPredicate) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  constexpr TypeParam values[] = {0, 1, 2, 3};
  auto const range = testing::demoted_view(std::forward_iterator_tag{}, values);

  auto const result = std::adjacent_find(std::begin(range), std::end(range), std::not_equal_to<>{});

  EXPECT_EQ(result, std::begin(range));
}

/// @test @c std::adjacent_find performs exactly X comparisons when there are no equal adjacent elements.
TEST(AdjacentFindTest, MeetsComplexityRequirementWhenNoAdjacentElements) {
  auto const values = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto const begin = ::testing::make_demoted_iterator<std::forward_iterator_tag>(values.begin());

  for (auto idx = std::size_t{}; idx != values.size(); ++idx) {
    auto const complexity_limit = static_cast<std::size_t>(std::max(static_cast<int>(idx) - 1, 0));

    auto const end =
        ::testing::make_demoted_iterator<std::forward_iterator_tag>(values.begin() + static_cast<std::ptrdiff_t>(idx));

    auto result = testing::counted_algorithm(
        FUNCTION_LIFT(std::adjacent_find),
        begin,
        end,
        arene::base::algorithm_detail::operator_equal
    );

    ASSERT_TRUE(result.applications == complexity_limit);
  }
}

/// @test @c std::adjacent_find performs exactly X comparisons
TEST(AdjacentFindTest, MeetsComplexityRequirementWhenAdjacentElements) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  int values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  constexpr auto size = sizeof(values) / sizeof(values[0]);
  auto const begin = ::testing::make_demoted_iterator<std::forward_iterator_tag>(std::begin(values));

  for (auto idx = std::size_t{2}; idx != size; ++idx) {
    // Make last two values adjacent.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    --(values[idx - 1]);

    // The complexity requirement is defined as:
    // min((i - first) + 1, (last - first) - 1)
    // The second half of the min does not apply, as we guarantee there is a result.
    // So our complexity requirement will be (i - first) + 1
    //
    // We put the adjacent pair at the end of the current range. As idx is the one-passed-the-end index, the adjacent
    // pair will be at idx - 2 and idx - 1. std::adjacent_find returns the first item in the pair, so the result will be
    // at idx - 2. In this case, first is the index 0. So the final complexity limit should be (idx - 2) + 1.
    auto const result = idx - 2;
    auto const complexity_limit = result + 1;

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    auto const end = ::testing::make_demoted_iterator<std::forward_iterator_tag>(
        std::begin(values) + static_cast<std::ptrdiff_t>(idx)
    );
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    auto info = testing::counted_algorithm(
        FUNCTION_LIFT(std::adjacent_find),
        begin,
        end,
        arene::base::algorithm_detail::operator_equal
    );

    ASSERT_TRUE(info.applications == complexity_limit);

    // Reset back to initial list.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ++(values[idx - 1]);
  }
}

}  // namespace
