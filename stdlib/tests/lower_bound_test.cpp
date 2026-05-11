// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/math/log2.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/cstddef"
#include "stdlib/include/cstdint"
#include "stdlib/include/initializer_list"
#include "stdlib/include/iterator"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/iterator_types.hpp"

// NOLINTBEGIN(readability-identifier-length)

namespace {

using iterator_category_types =
    ::testing::Types<std::forward_iterator_tag, std::bidirectional_iterator_tag, std::random_access_iterator_tag>;

template <typename T>
class LowerBound : public testing::Test {};

TYPED_TEST_SUITE(LowerBound, iterator_category_types, );

/// @test In a sorted range, @c std::lower_bound finds the first element *not*
/// ordered before 1.
///
CONSTEXPR_TYPED_TEST(LowerBound, ElementInRange) {
  auto const values = {0, 1, 2, 3, 4};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::lower_bound(range.begin(), range.end(), 1);

  ASSERT_EQ(it, arene::base::next(range.begin(), 1));
}

/// @test In a sorted range, @c std::lower_bound finds the first element *not*
/// ordered before 1 when it is the first element.
///
CONSTEXPR_TYPED_TEST(LowerBound, ElementIsFirstInRange) {
  auto const values = {1, 2, 3, 4};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::lower_bound(range.begin(), range.end(), 1);

  ASSERT_EQ(it, range.begin());
}

/// @test In a sorted range, @c std::lower_bound finds the first element *not*
/// ordered before 1 when it is the last element.
///
CONSTEXPR_TYPED_TEST(LowerBound, ElementIsLastInRange) {
  auto const values = {-1, 0, 1};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::lower_bound(range.begin(), range.end(), 1);

  ASSERT_EQ(it, arene::base::next(range.begin(), 2));
}

/// @test In a sorted range with duplicate values, @c std::lower_bound finds the
/// first element *not* ordered before 1.
///
CONSTEXPR_TYPED_TEST(LowerBound, DuplicateElementsInRange) {
  auto const values = {0, 1, 1};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::lower_bound(range.begin(), range.end(), 1);

  ASSERT_EQ(it, arene::base::next(range.begin(), 1));
}

/// @test In a sorted range without the target element, @c std::lower_bound
/// finds the first element *not* ordered before 1.
///
CONSTEXPR_TYPED_TEST(LowerBound, ElementNotInRange) {
  auto const values = {0, 0, 2, 3, 4};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::lower_bound(range.begin(), range.end(), 1);

  ASSERT_EQ(it, arene::base::next(range.begin(), 2));
}

/// @test In a partitioned range without the target element, @c std::lower_bound
/// finds the first element *not* ordered before 1.
///
CONSTEXPR_TYPED_TEST(LowerBound, ElementNotInPartitionedRange) {
  auto const values = {0, 0, 4, 3, 2};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::lower_bound(range.begin(), range.end(), 1);

  ASSERT_EQ(it, arene::base::next(range.begin(), 2));
}

/// @test In a partitioned range, @c std::lower_bound
/// returns the last element if *no* elements are ordered before 10.
///
CONSTEXPR_TYPED_TEST(LowerBound, NoElementsOrderedBefore) {
  auto const values = {0, 0, 4, 3, 2};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::lower_bound(range.begin(), range.end(), 10);

  ASSERT_EQ(it, range.end());
}

/// @test In an empty range, @c std::lower_bound returns the last element.
///
CONSTEXPR_TYPED_TEST(LowerBound, EmptyRange) {
  auto const values = std::initializer_list<int>{};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::lower_bound(range.begin(), range.end(), 0);

  ASSERT_EQ(it, range.end());
}

/// @brief type different than `int` that is `operator<` comparable
///
class less_than_comparable {
  int value_;

 public:
  constexpr explicit less_than_comparable(int init)
      : value_{init} {}

  friend constexpr auto operator<(int lhs, less_than_comparable rhs) -> bool { return lhs < rhs.value_; }
};

/// @test @c std::lower_bound uses `operator<` for comparison
/// of the range element type with the input value type.
///
CONSTEXPR_TYPED_TEST(LowerBound, ValueIsDifferentType) {
  auto const values = {0, 1, 2, 3, 4};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::lower_bound(range.begin(), range.end(), less_than_comparable{1});

  ASSERT_EQ(it, arene::base::next(range.begin(), 1));
}

/// @test @c std::lower_bound uses a custom comparison function object if one is provided.
///
CONSTEXPR_TYPED_TEST(LowerBound, WithCustomComparitor) {
  auto const values = {0, 1, 2, 3, 4};
  auto const range = testing::demoted_view(TypeParam{}, values);

  struct less_equal {
    constexpr auto operator()(int lhs, int rhs) const -> bool { return lhs <= rhs; }
  };

  auto const it = std::lower_bound(range.begin(), range.end(), 1, less_equal{});

  ASSERT_EQ(it, arene::base::next(range.begin(), 2));
}

/// compares two @c int arguments with @c <=
///
constexpr auto int_less_equal(int lhs, int rhs) noexcept -> bool { return lhs <= rhs; }

/// @test @c std::lower_bound uses a custom comparison function if one is provided.
///
CONSTEXPR_TYPED_TEST(LowerBound, WithCustomComparitorFunction) {
  auto const values = {0, 1, 2, 3, 4};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::lower_bound(range.begin(), range.end(), 1, int_less_equal);

  ASSERT_EQ(it, arene::base::next(range.begin(), 2));
}

/// @test @c std::lower_bound performs at most log2(N) + O(1) comparisons.
///
TYPED_TEST(LowerBound, MeetsComplexityRequirement) {
  auto const values = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto const begin = ::testing::make_demoted_iterator<TypeParam>(values.begin());

  for (auto n = std::size_t{}; n != values.size(); ++n) {
    auto const complexity_limit = 1U + (n == 0U ? 0U : arene::base::log2(n));

    auto const end = ::testing::make_demoted_iterator<TypeParam>(values.begin() + static_cast<std::ptrdiff_t>(n));

    auto result = testing::counted_algorithm(
        FUNCTION_LIFT(std::lower_bound),
        begin,
        end,
        10,
        arene::base::algorithm_detail::operator_less
    );

    ASSERT_TRUE(result.applications <= complexity_limit);
  }
}

/// @test @c std::lower_bound is @c noexcept if iterator operations and
/// application of the default comparison object are @c noexcept.
///
TEST(LowerBound, NoexceptIfIteratorOpsAndCompareAreNoexcept) {
  using iterator_type = int const*;

  ASSERT_TRUE(noexcept(std::lower_bound(iterator_type{}, iterator_type{}, 0)));
}

struct throwing_cmp {
  template <typename... Args>
  auto operator()(Args const&...) const noexcept(false) -> bool {
    return true;
  }
};

/// @test @c std::lower_bound is not @c noexcept if the comparison can throw.
///
TEST(LowerBound, NotNoexceptIfComparisonCanThrow) {
  using iterator_type = int const*;

  // one of the arguments must be 'declval', otherwise using GCC8 and C++17 will
  // *always* deduce 'noexcept' as 'true'
  ASSERT_FALSE(noexcept(std::lower_bound(std::declval<iterator_type>(), iterator_type{}, 0, throwing_cmp{})));
}

/// @brief used to specify operations of @c noexcept_test_iterator that can throw
///
enum class throws_on : std::uint8_t {
  nothing = 0b0'0000U,
  construct = 0b0'0001U,
  assign = 0b0'0010U,
  deref = 0b0'0100U,
  minus = 0b0'1000U,
  plus_equals = 0b1'0000U
};

/// @brief an incomplete iterator type used for noexcept testing
/// @tparam Spec determines the operations that are *not* @c noexcept
///
/// This incomplete type must be used with `demoted_iterator` to define all
/// iterator operations.
///
template <throws_on Spec>
// NOLINTNEXTLINE(hicpp-special-member-functions)
struct incomplete_iterator : testing::demoted_iterator<int*> {
  using base_type = testing::demoted_iterator<int*>;

  /// @brief returns @c false if the bit associated with @c value is set, otherwise @c true
  ///
  static constexpr auto if_spec(throws_on value) noexcept -> bool {
    return 0U == (static_cast<std::uint8_t>(value) & static_cast<std::uint8_t>(Spec));
  }

  using reference = int&;
  using difference_type = std::ptrdiff_t;

  incomplete_iterator() = default;
  incomplete_iterator(incomplete_iterator const&) noexcept(if_spec(throws_on::construct))
      : base_type{} {}
  auto operator=(incomplete_iterator const&) noexcept(if_spec(throws_on::assign)) -> incomplete_iterator&;

  auto operator*() const noexcept(if_spec(throws_on::deref)) -> reference;
  auto operator-(incomplete_iterator) const noexcept(if_spec(throws_on::minus)) -> difference_type;
  auto operator+=(difference_type) noexcept(if_spec(throws_on::plus_equals)) -> incomplete_iterator&;
};

/// @brief alias to define an iterator type for noexcept testing
/// @tparam Spec determines the operations that are *not* @c noexcept
///
template <throws_on Spec>
using noexcept_test_iterator = testing::demoted_iterator<incomplete_iterator<Spec>>;

/// @test @c std::lower_bound is not @c noexcept if iterator operations can throw
///
TEST(LowerBound, NotNoexceptIfIteratorOperationsCanThrow) {
  auto const is_lower_bound_noexcept = [](auto iter) { return noexcept(std::lower_bound(iter, iter, 0)); };

  ASSERT_TRUE(is_lower_bound_noexcept(noexcept_test_iterator<throws_on::nothing>{}));

  ASSERT_FALSE(is_lower_bound_noexcept(noexcept_test_iterator<throws_on::construct>{}));
  ASSERT_FALSE(is_lower_bound_noexcept(noexcept_test_iterator<throws_on::assign>{}));
  ASSERT_FALSE(is_lower_bound_noexcept(noexcept_test_iterator<throws_on::deref>{}));
  ASSERT_FALSE(is_lower_bound_noexcept(noexcept_test_iterator<throws_on::minus>{}));
  ASSERT_FALSE(is_lower_bound_noexcept(noexcept_test_iterator<throws_on::plus_equals>{}));
}

/// @test all overloads of @c std::lower_bound are not invocable with input
/// iterator types or output iterator types
///
TEST(LowerBound, RequiresForwardIterator) {
  auto const is_invocable_with = [](auto category, auto... cmp) {
    using iterator_type = testing::demoted_iterator<int*, decltype(category)>;
    auto const lower_bound = FUNCTION_LIFT(std::lower_bound);
    return testing::simple_is_invocable_v<decltype(lower_bound), iterator_type, iterator_type, int, decltype(cmp)...>;
  };

  ASSERT_TRUE(is_invocable_with(std::forward_iterator_tag{}));
  ASSERT_TRUE(is_invocable_with(std::forward_iterator_tag{}, testing::always_true));

  ASSERT_FALSE(is_invocable_with(std::input_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with(std::output_iterator_tag{}));

  ASSERT_FALSE(is_invocable_with(std::input_iterator_tag{}, testing::always_true));
  ASSERT_FALSE(is_invocable_with(std::output_iterator_tag{}, testing::always_true));
}

/// @test @c std::lower_bound requires a binary predicate where the iterator
/// reference type is convertible for arg1 and the value argument is convertible
/// to arg2
///
TEST(LowerBound, RequiresBinaryPredicate) {
  auto const is_invocable_with = [](auto iter, auto value, auto... cmp) {
    auto const lower_bound = FUNCTION_LIFT(std::lower_bound);
    return testing::
        simple_is_invocable_v<decltype(lower_bound), decltype(iter), decltype(iter), decltype(value), decltype(cmp)...>;
  };

  using iterator_type = int*;

  struct empty {};

  ASSERT_TRUE(is_invocable_with(iterator_type{}, empty{}, testing::always_true));

  ASSERT_FALSE(is_invocable_with(iterator_type{}, empty{}));

  struct not_convertible_to_bool {};

  ASSERT_FALSE(is_invocable_with(iterator_type{}, 0, testing::always_constant<not_convertible_to_bool>{}));

  auto const not_invocable_with_int_args = [](empty, empty) -> bool { return true; };

  ASSERT_FALSE(is_invocable_with(iterator_type{}, 0, not_invocable_with_int_args));
}

}  // namespace

// NOLINTEND(readability-identifier-length)
