// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/algorithm"
#include "stdlib/include/cstddef"
#include "stdlib/include/functional"
#include "stdlib/include/iterator"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/iterator_types.hpp"

// NOLINTBEGIN(readability-identifier-length,hicpp-avoid-c-arrays)

namespace {
namespace test {

/// @brief function object that forwards arguments to @c std::unique
///
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
auto const unique = FUNCTION_LIFT(std::unique);

/// @brief helper to obtain the size as an unsigned type
/// @tparam R range type
/// @param range range to obtain the size of
///
template <class R>
auto size(R const& range) -> std::size_t {
  return static_cast<std::size_t>(std::end(range) - std::begin(range));
}

}  // namespace test

using iterator_category_types = ::testing::Types<  //
    std::forward_iterator_tag,
    std::bidirectional_iterator_tag,
    std::random_access_iterator_tag>;

template <typename T>
class Unique : public testing::Test {};

TYPED_TEST_SUITE(Unique, iterator_category_types, );

/// @test @c std::unique with a range of all different values and
/// @c std::equal_to as the predicate returns an iterator to the end
///
TYPED_TEST(Unique, PredicateAlwaysDifferent) {
  testing::only_movable<int> values[] = {0, -1, -2};
  auto const range = testing::demoted_view(TypeParam{}, std::begin(values), std::end(values));

  auto const result = testing::counted_algorithm(test::unique, range, std::equal_to<>{});

  ASSERT_EQ(result.iter.base(), std::end(values));
  ASSERT_EQ(result.applications, test::size(values) - 1U);
}

/// @test @c std::unique with a range of all different values and returns an iterator to the end
///
TYPED_TEST(Unique, AlwaysDifferent) {
  testing::only_movable<int> values[] = {0, -1, -2};
  auto const range = testing::demoted_view(TypeParam{}, std::begin(values), std::end(values));

  auto const result = std::unique(std::begin(range), std::end(range));

  ASSERT_EQ(result.base(), std::end(values));
}

/// @test @c std::unique with a range of duplicate values and @c std::equal_to
/// as the predicate returns an iterator after the first element
///
TYPED_TEST(Unique, PredicateAllSame) {
  testing::only_movable<int> values[] = {1, 1, 1};
  auto const range = testing::demoted_view(TypeParam{}, std::begin(values), std::end(values));

  auto const result = testing::counted_algorithm(test::unique, range, std::equal_to<>{});

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(result.iter.base(), std::begin(values) + 1);
  ASSERT_EQ(result.applications, test::size(values) - 1U);
}

/// @test @c std::unique with a range of duplicate values returns an iterator after the first element
///
TYPED_TEST(Unique, AllSame) {
  testing::only_movable<int> values[] = {1, 1, 1};
  auto const range = testing::demoted_view(TypeParam{}, std::begin(values), std::end(values));

  auto const result = std::unique(std::begin(range), std::end(range));

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(result.base(), std::begin(values) + 1);
}

/// @test @c std::unique with an empty range and @c std::equal_to as the
/// predicate returns an iterator to the first element
///
TYPED_TEST(Unique, PredicateEmptyRange) {
  auto value = testing::only_movable<int>{};
  auto const range = testing::demoted_view(TypeParam{}, &value, &value);

  auto const result = testing::counted_algorithm(test::unique, range, std::equal_to<>{});

  ASSERT_EQ(result.iter.base(), &value);
  ASSERT_EQ(result.applications, 0U);
}

/// @test @c std::unique with an empty range returns an iterator to the first element
///
TYPED_TEST(Unique, EmptyRange) {
  auto value = testing::only_movable<int>{};
  auto const range = testing::demoted_view(TypeParam{}, &value, &value);

  auto const result = std::unique(std::begin(range), std::end(range));

  ASSERT_EQ(result.base(), &value);
}

/// @test @c std::unique with @c std::equal_to as the predicate removes
/// duplicate values from the input range
///
TYPED_TEST(Unique, PredicateRemovesSomeElements) {
  testing::only_movable<int> values[] = {0, 0, 1, 1, 1, 0, 0};
  auto const range = testing::demoted_view(TypeParam{}, std::begin(values), std::end(values));

  auto const result = testing::counted_algorithm(test::unique, range, std::equal_to<>{});

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(result.iter.base(), std::begin(values) + 3);
  ASSERT_EQ(result.applications, test::size(values) - 1U);
}

/// @test @c std::unique removes duplicate values from the input range
///
TYPED_TEST(Unique, RemovesSomeElements) {
  testing::only_movable<int> values[] = {0, 0, 1, 1, 1, 0, 0};
  auto const range = testing::demoted_view(TypeParam{}, std::begin(values), std::end(values));

  auto const result = std::unique(std::begin(range), std::end(range));

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(result.base(), std::begin(values) + 3);
}

/// @brief helper for testing @c std::unique
/// @tparam Category iterator category applied to the initial array
/// @tparam InitialSize size of the initial array
/// @tparam Pred binary predicate type
/// @tparam ExpectedSize size of the expected array
/// @param initial the initial range of values
/// @param pred predicate determining which values to remove
/// @param expected the expected range after removal
///
/// Helper for testing @c std::unique. This function:
/// * demotes the array into a range with the specified iterator category
/// * calls @c std::unique with an adapted predicate that counts the number
///   of invocations
/// * ASSERTs that the predicate is invoked @c InitialSize times
/// * ASSERTs that the returned iterator is a distance @c ExpectedSize from
///   @c begin(initial)
/// * ASSERTs the values array with elements removed are equal to the elements
///   of @c expected
///
template <class Category, std::size_t InitialSize, class Pred, std::size_t ExpectedSize>
auto unique_returns(int (&&initial)[InitialSize], Pred pred, int const (&expected)[ExpectedSize]) -> void {
  auto const range = testing::demoted_view(Category{}, std::begin(initial), std::end(initial));
  auto const result = testing::counted_algorithm(test::unique, range, pred);

  ASSERT_EQ(result.applications, InitialSize - 1U);
  ASSERT_EQ(result.iter.base() - std::begin(initial), ExpectedSize);

  for (auto i = std::size_t{}; i != ExpectedSize; ++i) {
    ASSERT_EQ(initial[i], expected[i]);
  }
}

/// @brief helper for testing @c std::unique
/// @tparam Category iterator category applied to the initial array
/// @tparam InitialSize size of the initial array
/// @tparam ExpectedSize size of the expected array
/// @param initial the initial range of values
/// @param expected the expected range after removal
///
/// Helper for testing @c std::unique. This function:
/// * demotes the array into a range with the specified iterator category
/// * calls @c std::unique with an adapted predicate that counts the number
///   of invocations
/// * ASSERTs that the returned iterator is a distance @c ExpectedSize from
///   @c begin(initial)
/// * ASSERTs the values array with elements removed are equal to the elements
///   of @c expected
///
template <class Category, std::size_t InitialSize, std::size_t ExpectedSize>
auto unique_returns(int (&&initial)[InitialSize], int const (&expected)[ExpectedSize]) -> void {
  auto const range = testing::demoted_view(Category{}, std::begin(initial), std::end(initial));
  auto const result = std::unique(std::begin(range), std::end(range));

  ASSERT_EQ(result.base() - std::begin(initial), ExpectedSize);

  for (auto i = std::size_t{}; i != ExpectedSize; ++i) {
    ASSERT_EQ(initial[i], expected[i]);
  }
}

/// @test @c std::unique with equality as the predicate removes some elements
/// and returns an iterator denoting the end of the new range where:
/// * the new range only contains the first element of a group of equal elements
/// * relative order of the elements in the new range match the relative order
///   of the elements in original range
///
TYPED_TEST(Unique, ResultWithEquality) {
  unique_returns<TypeParam>({0, 1, 1, 0, -1}, std::equal_to<>{}, {0, 1, 0, -1});
  unique_returns<TypeParam>({1, 1, 1, 1}, std::equal_to<>{}, {1});
  unique_returns<TypeParam>({1, 2, 3, 4}, std::equal_to<>{}, {1, 2, 3, 4});

  unique_returns<TypeParam>({0, 1, 1, 0, -1}, {0, 1, 0, -1});
  unique_returns<TypeParam>({1, 1, 1, 1}, {1});
  unique_returns<TypeParam>({1, 2, 3, 4}, {1, 2, 3, 4});
}

/// @test @c std::unique with @c has_same_sign as the predicate removes some
/// elements and returns an iterator denoting the end of the new range where:
/// * the new range only contains the first element of a group of equivalent
///   elements
/// * relative order of the elements in the new range match the relative order
///   of the elements in original range
///
TYPED_TEST(Unique, ResultWithHasSameSign) {
  auto const has_same_sign = [](int lhs, int rhs) {
    if (lhs == rhs) {
      return true;
    }
    if ((lhs < 0) and (rhs < 0)) {
      return true;
    }
    if ((0 < lhs) and (0 < rhs)) {
      return true;
    }
    return false;
  };

  unique_returns<TypeParam>({0, 1, 1, 0, -1}, has_same_sign, {0, 1, 0, -1});
  unique_returns<TypeParam>({1, 1, 1, 1, 1}, has_same_sign, {1});
  unique_returns<TypeParam>({1, 2, 3, 4, 5}, has_same_sign, {1});
  unique_returns<TypeParam>({1, 2, 3, -4, -5, 0, 1, -3, 5}, has_same_sign, {1, -4, 0, 1, -3, 5});
}

/// @test @c std::unique with @c eq_rem_2 as the predicate removes some
/// elements and returns an iterator denoting the end of the new range where:
/// * the new range only contains the first element of a group of equivalent
///   elements
/// * relative order of the elements in the new range match the relative order
///   of the elements in original range
///
TYPED_TEST(Unique, ResultWithEqRem2) {
  auto const eq_rem_2 = [](int lhs, int rhs) { return (lhs % 2) == (rhs % 2); };

  unique_returns<TypeParam>({0, 1, 1, 0, -1}, eq_rem_2, {0, 1, 0, -1});
  unique_returns<TypeParam>({1, 1, 1, 1, 1}, eq_rem_2, {1});
  unique_returns<TypeParam>({1, 2, 3, 4, 5}, eq_rem_2, {1, 2, 3, 4, 5});
  unique_returns<TypeParam>({1, 5, 13, 1}, eq_rem_2, {1});
  unique_returns<TypeParam>({1, 2, 0, 3, 4}, eq_rem_2, {1, 2, 3, 4});
  unique_returns<TypeParam>({1, 2, 4, 0, 3, 5, 8, 3, 0}, eq_rem_2, {1, 2, 3, 8, 3, 0});
}

/// @brief determine if @c std::unique is invocable
/// @tparam T iterator value type
/// @tparam Category iterator category type
/// @tparam Pred binary predicate (is provided)
///
/// Determine if @c std::unique is invocable with an iterator type and a
/// binary predicate (if provided). The iterator type has a @c value_type of @c
/// T and all operations required by @c Category.
///
/// @return @c true if invocable, otherwise @c false
///
template <class T, class Category, class... Pred>
auto is_invocable_with(Category, Pred...) -> bool {
  using iterator_type = testing::demoted_iterator<T*, Category>;
  auto const lifted = FUNCTION_LIFT(std::unique);
  return testing::simple_is_invocable_v<decltype(lifted), iterator_type, iterator_type, Pred...>;
}

/// @test @c std::unique is not invocable with input iterator types or output
/// iterator types
///
TEST(Unique, RequiresForwardIterator) {
  ASSERT_TRUE(is_invocable_with<int>(std::forward_iterator_tag{}, testing::always_true));
  ASSERT_FALSE(is_invocable_with<int>(std::input_iterator_tag{}, testing::always_true));
  ASSERT_FALSE(is_invocable_with<int>(std::output_iterator_tag{}, testing::always_true));

  ASSERT_TRUE(is_invocable_with<int>(std::forward_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::input_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::output_iterator_tag{}));
}

/// @test @c std::unique requires a binary predicate for the iterator reference type
///
TEST(Unique, RequiresBinaryPredicate) {
  struct empty {};
  struct not_convertible_to_bool {};

  ASSERT_TRUE(is_invocable_with<int>(std::forward_iterator_tag{}, [](int, int) { return true; }));
  ASSERT_FALSE(is_invocable_with<int>(std::forward_iterator_tag{}, [](empty, empty) { return true; }));
  ASSERT_FALSE(is_invocable_with<int>(std::forward_iterator_tag{}, [](int, int) { return not_convertible_to_bool{}; }));

  struct operator_eq_result_not_bool {
    auto operator==(operator_eq_result_not_bool) const -> not_convertible_to_bool;
  };
  ASSERT_FALSE(is_invocable_with<empty>(std::forward_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<operator_eq_result_not_bool>(std::forward_iterator_tag{}));
}

/// @brief determine if @c std::unique is nothrow invocable
/// @tparam T iterator value type
/// @tparam Pred binary predicate (if provided)
///
/// Determine if @c std::unique is nothrow invocable with a pointer to @c T
/// and the given binary predicate (if provided).
///
/// @return @c true if nothrow invocable, otherwise @c false
///
template <class T, class... Pred>
auto is_nothrow_invocable_with(Pred...) -> bool {
  static_assert(sizeof...(Pred) < 2, "");
  return noexcept(std::unique(std::declval<T*>(), std::declval<T*>(), std::declval<Pred>()...));
}

/// @test @c std::unique requires a binary predicate for the iterator value type
///
TEST(Unique, RequiresMoveAssignable) {
  struct no_assignment {
    ~no_assignment() = default;
    no_assignment() = default;
    no_assignment(no_assignment const&) = default;
    no_assignment(no_assignment&&) = default;
    auto operator=(no_assignment const&) -> no_assignment& = delete;
    auto operator=(no_assignment&&) -> no_assignment& = delete;
  };

  ASSERT_FALSE(is_invocable_with<no_assignment>(std::forward_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<no_assignment>(std::forward_iterator_tag{}, testing::always_true));

  struct bad_assignment_return {
    ~bad_assignment_return() = default;
    bad_assignment_return() = default;
    bad_assignment_return(bad_assignment_return const&) = default;
    bad_assignment_return(bad_assignment_return&&) = default;
    auto operator=(bad_assignment_return const&) -> bad_assignment_return& = default;
    // NOLINTNEXTLINE(misc-unconventional-assign-operator)
    auto operator=(bad_assignment_return&&) noexcept -> void;
  };
  ASSERT_FALSE(is_invocable_with<bad_assignment_return>(std::forward_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<bad_assignment_return>(std::forward_iterator_tag{}, testing::always_true));
}

/// @test @c std::unique is not @c noexcept if move assignment can throw
///
TEST(Unique, NotNoexceptIfMoveAssignmentCanThrow) {
  struct throws_on_move_assignment {
    ~throws_on_move_assignment() = default;
    throws_on_move_assignment() = default;
    throws_on_move_assignment(throws_on_move_assignment const&) = default;
    throws_on_move_assignment(throws_on_move_assignment&&) = default;
    auto operator=(throws_on_move_assignment const&) -> throws_on_move_assignment& = default;
    auto operator=(throws_on_move_assignment&&) noexcept(false) -> throws_on_move_assignment&;

    auto operator==(throws_on_move_assignment) const -> bool;
  };

  ASSERT_FALSE(is_nothrow_invocable_with<throws_on_move_assignment>());
  ASSERT_FALSE(is_nothrow_invocable_with<throws_on_move_assignment>(testing::always_true));
}

/// @test @c std::unique is not @c noexcept if the predicate can throw
///
TEST(Unique, NotNoexceptIfPredicateCanThrow) {
  auto const pred = [](auto const&...) noexcept(false) { return true; };
  ASSERT_FALSE(is_nothrow_invocable_with<int>(pred));
}

/// @test @c std::unique is not @c noexcept if iterator operations can throw
///
TEST(Unique, NotNoexceptIfIteratorOperationsCanThrow) {
  auto const is_noexcept_if_iterator = [](auto specifier_constant, auto... eq) {
    using iterator = testing::noexcept_configurable_forward_iterator<specifier_constant>;
    return noexcept(std::unique(iterator{}, iterator{}, eq...));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing, testing::always_true));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_construct, testing::always_true));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference, testing::always_true));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::pre_increment, testing::always_true));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::equal, testing::always_true));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal, testing::always_true));

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_construct));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::pre_increment));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::equal));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal));
}

}  // namespace

// NOLINTEND(readability-identifier-length,hicpp-avoid-c-arrays)
