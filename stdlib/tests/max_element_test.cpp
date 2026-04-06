// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/detail/functional.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/cstddef"
#include "stdlib/include/initializer_list"
#include "stdlib/include/iterator"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/iterator_types.hpp"

// NOLINTBEGIN(readability-identifier-length)

namespace {

constexpr bool using_cpp17 = __cplusplus >= 201703L;

using iterator_category_types =
    ::testing::Types<std::forward_iterator_tag, std::bidirectional_iterator_tag, std::random_access_iterator_tag>;

template <typename T>
class MaxElement : public testing::Test {};

TYPED_TEST_SUITE(MaxElement, iterator_category_types, );

/// @test @c std::max_element returns an iterator to the largest element, where
/// the largest element is at position 0 of the input range.
///
CONSTEXPR_TYPED_TEST(MaxElement, SingleLargestAt0) {
  auto const values = {0, -1, -1};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::max_element(range.begin(), range.end());

  ASSERT_EQ(it, arene::base::next(range.begin(), 0));
}

/// @test @c std::max_element returns an iterator to the largest element, where
/// the largest element is in the middle of the input range.
///
CONSTEXPR_TYPED_TEST(MaxElement, SingleLargestAt1) {
  auto const values = {-1, 0, -1};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::max_element(range.begin(), range.end());

  ASSERT_EQ(it, arene::base::next(range.begin(), 1));
}

/// @test @c std::max_element returns an iterator to the largest element, where
/// the largest element is at the end of the input range.
///
CONSTEXPR_TYPED_TEST(MaxElement, SingleLargestAt2) {
  auto const values = {-1, -1, 0};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::max_element(range.begin(), range.end());

  ASSERT_EQ(it, arene::base::next(range.begin(), 2));
}

/// @test @c std::max_element returns an iterator to the first occurence of the
/// largest element, if there are multiple, and where the first occurence is at
/// position 0 of the input range.
///
CONSTEXPR_TYPED_TEST(MaxElement, MultipleLargestAt0) {
  auto const values = {0, 0, -1};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::max_element(range.begin(), range.end());

  ASSERT_EQ(it, arene::base::next(range.begin(), 0));
}

/// @test @c std::max_element returns an iterator to the first occurence of the
/// largest element, if there are multiple, and where the first occurence is at
/// position 1 of the input range.
///
CONSTEXPR_TYPED_TEST(MaxElement, MultipleLargestAt1) {
  auto const values = {-1, 0, 0};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::max_element(range.begin(), range.end());

  ASSERT_EQ(it, arene::base::next(range.begin(), 1));
}

/// @test @c std::max_element returns an iterator to the first occurence of the
/// largest element, if there are multiple, and where the second occurence is
/// not adjacent to the first occurence.
///
CONSTEXPR_TYPED_TEST(MaxElement, MultipleLargestNotContiguous) {
  auto const values = {0, -1, 0};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::max_element(range.begin(), range.end());

  ASSERT_EQ(it, arene::base::next(range.begin(), 0));
}

/// @test @c std::max_element returns @c last if the range is empty.
///
CONSTEXPR_TYPED_TEST(MaxElement, EmptyRange) {
  auto const values = std::initializer_list<int>{};
  auto const range = testing::demoted_view(TypeParam{}, values);

  auto const it = std::max_element(range.begin(), range.end());

  ASSERT_EQ(it, range.end());
}

/// @test @c std::max_element uses a custom comparator if one is provided.
///
CONSTEXPR_TYPED_TEST(MaxElement, CustomComparator) {
  auto const values = {0, -2, -1};
  auto const range = testing::demoted_view(TypeParam{}, values);

  struct less_than_negated {
    constexpr auto operator()(int lhs, int rhs) const noexcept -> bool { return -lhs < -rhs; }
  };

  auto const it = std::max_element(range.begin(), range.end(), less_than_negated{});

  ASSERT_EQ(it, arene::base::next(range.begin(), 1));
}

/// @test @c std::max_element performs exactly max(N - 1 , 0) comparisons.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MaxElement, MeetsComplexityRequirement, using_cpp17) {
  auto const values = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto const begin = ::testing::make_demoted_iterator<TypeParam>(values.begin());

  for (auto n = std::size_t{}; n != values.size(); ++n) {
    auto const end = ::testing::make_demoted_iterator<TypeParam>(values.begin() + static_cast<std::ptrdiff_t>(n));

    auto result = testing::counted_algorithm(
        FUNCTION_LIFT(std::max_element),
        begin,
        end,
        arene::base::algorithm_detail::operator_less
    );

    if (n == 0U) {
      ASSERT_EQ(result.applications, 0U);
    } else {
      ASSERT_EQ(result.applications, n - 1U);
    }
  }
}

/// @test @c std::max_element is @c noexcept if iterator operations and
/// application of the default comparison object are @c noexcept.
///
TYPED_TEST(MaxElement, NoexceptIfIteratorOpsAndCompareAreNoexcept) {
  using iterator_type = int const*;

  ASSERT_TRUE(noexcept(std::max_element(iterator_type{}, iterator_type{})));
}

/// @test @c std::max_element is not @c noexcept if the comparison can throw.
///
TYPED_TEST(MaxElement, NotNoexceptIfComparisonCanThrow) {
  using iterator_type = int const*;

  auto const cmp = [](auto const&...) noexcept(false) { return true; };

  // one of the arguments must be 'declval', otherwise using GCC8 and C++17 will
  // *always* deduce 'noexcept' as 'true'
  ASSERT_FALSE(noexcept(std::max_element(std::declval<iterator_type>(), iterator_type{}, cmp)));
}

/// @brief used to specify operations of @c noexcept_test_iterator that can throw
///
enum class throws_on : std::uint8_t {
  nothing = 0b00'0000U,
  construct = 0b00'0001U,
  assign = 0b00'0010U,
  deref = 0b00'0100U,
  increment = 0b00'1000U,
  equal = 0b01'0000U,
  not_equal = 0b10'0000U
};

/// @brief an incomplete iterator type used for noexcept testing
/// @tparam Spec determines the operations that are *not* @c noexcept
///
/// This incomplete type must be used with `demoted_iterator` to define all
/// iterator operations.
///
template <throws_on Spec>
// NOLINTNEXTLINE(hicpp-special-member-functions)
struct incomplete_iterator : ::testing::demoted_iterator<int*> {
  using base_type = ::testing::demoted_iterator<int*>;

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
  auto operator++() noexcept(if_spec(throws_on::increment)) -> incomplete_iterator&;

  auto operator==(incomplete_iterator) const noexcept(if_spec(throws_on::equal)) -> bool;
  auto operator!=(incomplete_iterator) const noexcept(if_spec(throws_on::not_equal)) -> bool;
};

/// @brief alias to define an iterator type for noexcept testing
/// @tparam Spec determines the operations that are *not* @c noexcept
///
template <throws_on Spec>
using noexcept_test_iterator = ::testing::demoted_iterator<incomplete_iterator<Spec>>;

/// @test @c std::max_element is not @c noexcept if iterator operations can throw
///
TYPED_TEST(MaxElement, NotNoexceptIfIteratorOperationsCanThrow) {
  auto const is_algorithm_noexcept = [](auto iter) { return noexcept(std::max_element(iter, iter)); };

  ASSERT_TRUE(is_algorithm_noexcept(noexcept_test_iterator<throws_on::nothing>{}));

  ASSERT_FALSE(is_algorithm_noexcept(noexcept_test_iterator<throws_on::construct>{}));
  ASSERT_FALSE(is_algorithm_noexcept(noexcept_test_iterator<throws_on::assign>{}));
  ASSERT_FALSE(is_algorithm_noexcept(noexcept_test_iterator<throws_on::deref>{}));
  ASSERT_FALSE(is_algorithm_noexcept(noexcept_test_iterator<throws_on::increment>{}));
  ASSERT_FALSE(is_algorithm_noexcept(noexcept_test_iterator<throws_on::equal>{}));
  ASSERT_FALSE(is_algorithm_noexcept(noexcept_test_iterator<throws_on::not_equal>{}));
}

/// @test all overloads of @c std::max_element are not invocable with input
/// iterator types or output iterator types
///
TYPED_TEST(MaxElement, RequiresForwardIterator) {
  auto const is_invocable_with = [](auto category, auto... cmp) {
    using iterator_type = testing::demoted_iterator<int*, decltype(category)>;
    auto const lifted = FUNCTION_LIFT(std::max_element);
    return testing::simple_is_invocable_v<decltype(lifted), iterator_type, iterator_type, decltype(cmp)...>;
  };

  ASSERT_TRUE(is_invocable_with(std::forward_iterator_tag{}));
  ASSERT_TRUE(is_invocable_with(std::forward_iterator_tag{}, testing::always_true));

  ASSERT_FALSE(is_invocable_with(std::input_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with(std::output_iterator_tag{}));

  ASSERT_FALSE(is_invocable_with(std::input_iterator_tag{}, testing::always_true));
  ASSERT_FALSE(is_invocable_with(std::output_iterator_tag{}, testing::always_true));
}

/// @test @c std::max_element requires a binary predicate where the iterator
/// reference type is convertible for arg1 and for arg2
///
TEST(MaxElement, RequiresBinaryPredicate) {
  auto const is_invocable_with = [](auto iter, auto... cmp) {
    auto const lifted = FUNCTION_LIFT(std::max_element);
    return testing::simple_is_invocable_v<decltype(lifted), decltype(iter), decltype(iter), decltype(cmp)...>;
  };

  ASSERT_TRUE(is_invocable_with(std::add_pointer_t<int>{}));
  ASSERT_TRUE(is_invocable_with(std::add_pointer_t<int>{}, testing::always_true));

  struct empty {};
  ASSERT_FALSE(is_invocable_with(std::add_pointer_t<empty>{}));
  ASSERT_TRUE(is_invocable_with(std::add_pointer_t<empty>{}, testing::always_true));

  struct not_convertible_to_bool {};
  ASSERT_FALSE(is_invocable_with(std::add_pointer_t<int>{}, empty{}));
  ASSERT_FALSE(is_invocable_with(std::add_pointer_t<int>{}, testing::always_constant<not_convertible_to_bool>{}));

  auto const not_invocable_with_int_args = [](empty, empty) -> bool { return true; };
  ASSERT_FALSE(is_invocable_with(std::add_pointer_t<int>{}, not_invocable_with_int_args));
  ASSERT_TRUE(is_invocable_with(std::add_pointer_t<empty>{}, not_invocable_with_int_args));
}

}  // namespace

// NOLINTEND(readability-identifier-length)
