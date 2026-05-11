// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/algorithm"
#include "stdlib/include/cstddef"
#include "stdlib/include/iterator"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/iterator_types.hpp"

// NOLINTBEGIN(readability-identifier-length,hicpp-avoid-c-arrays)

namespace {
namespace test {

/// @brief function object that forwards arguments to @c std::remove_if
///
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
auto const remove_if = FUNCTION_LIFT(std::remove_if);

/// @brief helper to obtain the size as an unsigned type
/// @tparam R range type
/// @param range range to obtain the size of
///
template <class R>
auto size(R const& range) -> std::size_t {
  return static_cast<std::size_t>(std::end(range) - std::begin(range));
}

/// @brief predicate function object that returns @c true if the value is positive
///
struct is_positive_fn {
  template <class T>
  constexpr auto operator()(T const& value) const -> bool {
    return T{} < value;
  }
};
constexpr auto is_positive = is_positive_fn{};

}  // namespace test

using iterator_category_types = ::testing::Types<  //
    std::forward_iterator_tag,
    std::bidirectional_iterator_tag,
    std::random_access_iterator_tag>;

template <typename T>
class RemoveIf : public testing::Test {};

TYPED_TEST_SUITE(RemoveIf, iterator_category_types, );

/// @test @c std::remove_if with a predicate that always returns @c false
/// (for the given values) returns an iterator to the last element
///
TYPED_TEST(RemoveIf, PredicateAlwaysFalse) {
  testing::only_movable<int> values[] = {0, -1, -2};
  auto const range = testing::demoted_view(TypeParam{}, std::begin(values), std::end(values));

  auto const result = testing::counted_algorithm(test::remove_if, range, test::is_positive);

  ASSERT_EQ(result.iter.base(), std::end(values));
  ASSERT_EQ(result.applications, test::size(values));
}

/// @test @c std::remove_if with a predicate that always returns @c true
/// (for the given values) returns an iterator to the first element
///
TYPED_TEST(RemoveIf, PredicateAlwaysTrue) {
  testing::only_movable<int> values[] = {1, 2, 3};
  auto const range = testing::demoted_view(TypeParam{}, std::begin(values), std::end(values));

  auto const result = testing::counted_algorithm(test::remove_if, range, test::is_positive);

  ASSERT_EQ(result.iter.base(), std::begin(values));
  ASSERT_EQ(result.applications, test::size(values));
}

/// @test @c std::remove_if with a predicate that always returns @c true
/// (for the given values) and an empty range returns an iterator to the first
/// element
///
TYPED_TEST(RemoveIf, PredicateAlwaysTrueAndEmptyRange) {
  auto value = testing::only_movable<int>{};
  auto const range = testing::demoted_view(TypeParam{}, &value, &value);

  auto const result = testing::counted_algorithm(test::remove_if, range, test::is_positive);

  ASSERT_EQ(result.iter.base(), &value);
  ASSERT_EQ(result.applications, 0U);
}

/// @test @c std::remove_if with a predicate that always returns @c true for
/// positive values removes positive values from the input range
///
TYPED_TEST(RemoveIf, PredicateRemovesSomeElements) {
  testing::only_movable<int> values[] = {0, 1, -1, 2, -2, 3, -3};
  auto const range = testing::demoted_view(TypeParam{}, std::begin(values), std::end(values));

  auto const result = testing::counted_algorithm(test::remove_if, range, test::is_positive);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_EQ(result.iter.base(), std::begin(values) + 4);
  ASSERT_EQ(result.applications, test::size(values));
}

/// @brief helper for testing @c std::remove_if
/// @tparam Category iterator category applied to the initial array
/// @tparam InitialSize size of the initial array
/// @tparam Pred unary predicate type
/// @tparam ExpectedSize size of the expected array
/// @param initial the initial range of values
/// @param pred predicate determining which values to remove
/// @param expected the expected range after removal
///
/// Helper for testing @c std::remove_if. This function:
/// * demotes the array into a range with the specified iterator category
/// * calls @c std::remove_if with an adapted predicate that counts the number
///   of invocations
/// * ASSERTs that the predicate is invoked @c InitialSize times
/// * ASSERTs that the returned iterator is a distance @c ExpectedSize from
///   @c begin(initial)
/// * ASSERTs the values array with elements removed are equal to the elements
///   of @c expected
///
template <class Category, std::size_t InitialSize, class Pred, std::size_t ExpectedSize>
auto remove_if_returns(int (&&initial)[InitialSize], Pred pred, int const (&expected)[ExpectedSize]) -> void {
  auto const range = testing::demoted_view(Category{}, std::begin(initial), std::end(initial));
  auto const result = testing::counted_algorithm(test::remove_if, range, pred);

  ASSERT_EQ(result.applications, InitialSize);
  ASSERT_EQ(result.iter.base() - std::begin(initial), ExpectedSize);

  for (auto i = std::size_t{}; i != ExpectedSize; ++i) {
    ASSERT_EQ(initial[i], expected[i]);
  }
}

/// @test @c std::remove_if with a predicate that removes some elements returns an iterator
/// denoting the end of the new range where:
/// * the new range only contains elements where the predicate returned false
/// * relative order of the elements in the new range match the relative order
///   of the elements in original range
///
TYPED_TEST(RemoveIf, PredicateSometimesTrue) {
  auto const eq = [](auto value) { return [value](auto arg) { return arg == value; }; };
  remove_if_returns<TypeParam>({0, 1, -1}, eq(+0), {1, -1});
  remove_if_returns<TypeParam>({0, 1, -1}, eq(+1), {0, -1});
  remove_if_returns<TypeParam>({0, 1, -1}, eq(-1), {0, 1});

  auto const ne = [](auto value) { return [value](auto arg) { return arg != value; }; };
  remove_if_returns<TypeParam>({0, 1, -1}, ne(+0), {0});
  remove_if_returns<TypeParam>({0, 1, -1}, ne(+1), {1});
  remove_if_returns<TypeParam>({0, 1, -1}, ne(-1), {-1});
}

/// @brief determine if @c std::remove_if is invocable
/// @tparam T iterator value type
/// @tparam Category iterator category type
/// @tparam Pred unary predicate
///
/// Determine if @c std::remove_if is invocable with an iterator type and a
/// unary predicate. The iterator type has a @c value_type of @c T and all
/// operations required by @c Category.
///
/// @return @c true if invocable, otherwise @c false
///
template <class T, class Category, class Pred>
auto is_invocable_with(Category, Pred) -> bool {
  using iterator_type = testing::demoted_iterator<T*, Category>;
  auto const lifted = FUNCTION_LIFT(std::remove_if);
  return testing::simple_is_invocable_v<decltype(lifted), iterator_type, iterator_type, Pred>;
}

/// @test @c std::remove_if is not invocable with input iterator types or output
/// iterator types
///
TEST(RemoveIf, RequiresForwardIterator) {
  ASSERT_TRUE(is_invocable_with<int>(std::forward_iterator_tag{}, testing::always_true));
  ASSERT_FALSE(is_invocable_with<int>(std::input_iterator_tag{}, testing::always_true));
  ASSERT_FALSE(is_invocable_with<int>(std::output_iterator_tag{}, testing::always_true));
}

/// @test @c std::remove_if requires a unary predicate for the iterator reference type
///
TEST(RemoveIf, RequiresUnaryPredicate) {
  struct empty {};
  struct not_convertible_to_bool {};

  ASSERT_TRUE(is_invocable_with<int>(std::forward_iterator_tag{}, [](int) { return true; }));
  ASSERT_FALSE(is_invocable_with<int>(std::forward_iterator_tag{}, [](empty) { return true; }));
  ASSERT_FALSE(is_invocable_with<int>(std::forward_iterator_tag{}, [](int) { return not_convertible_to_bool{}; }));
}

/// @brief determine if @c std::remove_if is nothrow invocable
/// @tparam T iterator value type
/// @tparam Pred unary predicate
///
/// Determine if @c std::remove_if is nothrow invocable with a pointer to @c T
/// and the given unary predicate.
///
/// @return @c true if nothrow invocable, otherwise @c false
///
template <class T, class Pred>
auto is_nothrow_invocable_with(Pred) -> bool {
  return noexcept(std::remove_if(std::declval<T*>(), std::declval<T*>(), std::declval<Pred>()));
}

/// @test @c std::remove_if requires a unary predicate for the iterator value type
///
TEST(RemoveIf, RequiresMoveAssignable) {
  struct no_assignment {
    ~no_assignment() = default;
    no_assignment() = default;
    no_assignment(no_assignment const&) = default;
    no_assignment(no_assignment&&) = default;
    auto operator=(no_assignment const&) -> no_assignment& = delete;
    auto operator=(no_assignment&&) -> no_assignment& = delete;
  };

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
  ASSERT_FALSE(is_invocable_with<bad_assignment_return>(std::forward_iterator_tag{}, testing::always_true));
}

/// @test @c std::remove_if is not @c noexcept if move assignment can throw
///
TEST(RemoveIf, NotNoexceptIfMoveAssignmentCanThrow) {
  struct throws_on_move_assignment {
    ~throws_on_move_assignment() = default;
    throws_on_move_assignment() = default;
    throws_on_move_assignment(throws_on_move_assignment const&) = default;
    throws_on_move_assignment(throws_on_move_assignment&&) = default;
    auto operator=(throws_on_move_assignment const&) -> throws_on_move_assignment& = default;
    auto operator=(throws_on_move_assignment&&) noexcept(false) -> throws_on_move_assignment&;
  };

  ASSERT_FALSE(is_nothrow_invocable_with<throws_on_move_assignment>(testing::always_true));
}

/// @test @c std::remove_if is not @c noexcept if the predicate can throw
///
TEST(RemoveIf, NotNoexceptIfPredicateCanThrow) {
  auto const pred = [](auto const&...) noexcept(false) { return true; };
  ASSERT_FALSE(is_nothrow_invocable_with<int>(pred));
}

/// @test @c std::remove_if is not @c noexcept if iterator operations can throw
///
TEST(RemoveIf, NotNoexceptIfIteratorOperationsCanThrow) {
  auto const is_noexcept_if_iterator = [](auto specifier_constant) {
    using iterator = testing::noexcept_configurable_forward_iterator<specifier_constant>;
    return noexcept(std::remove_if(iterator{}, iterator{}, testing::always_true));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_construct));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_assign));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::pre_increment));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal));
}

}  // namespace

// NOLINTEND(readability-identifier-length,hicpp-avoid-c-arrays)
