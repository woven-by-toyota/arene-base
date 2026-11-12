// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_MAX_ELEMENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_MAX_ELEMENT_HPP_

#include <gtest/gtest.h>

#include "arene/base/algorithm/max_element.hpp"
#include "arene/base/algorithm/tests/max_element_fwd.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/array/to_array.hpp"
#include "arene/base/functional/detail/operator_less.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/stdlib_choice/add_pointer.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/functional.hpp"
#include "testlibs/utilities/iterator_types.hpp"
// IWYU pragma: no_include "testlibs/utilities/throws_on.hpp"

namespace max_element_test {

/// @brief specifies the max_element algorithm under test
/// @tparam Override parameter used to obtain the test configuration
///
/// To override the default value, this must be set by specializing
///   @c ::max_element_test::config::version<::max_element_test::config::override>
///   prior to including this header
///
/// @return enum value of @c ::max_element_test::config::flavor
template <class Override = max_element_test::config::override>
constexpr auto version() noexcept -> max_element_test::config::flavor {
  return ::max_element_test::config::version<Override>;
}
/// @brief provides the @c max_element algorithm as a function object
/// @tparam Override parameter used to obtain the test configuration
///
/// To override the default value, this must be set by specializing
///   @c ::max_element_test::config::function_under_test<::max_element_test::config::override>
///   prior to including this header
///
/// @return @c ::max_element_test::config::std_version<::max_element_test::config::override>
template <class Override = max_element_test::config::override>
constexpr auto max_element_fn() noexcept -> auto& {
  return ::max_element_test::config::function_under_test<Override>;
}
/// @brief specifies if the max_element algorithm comes from @c std
/// @tparam Override parameter used to obtain the test configuration
template <class Override = max_element_test::config::override>
constexpr auto is_std_version() noexcept -> bool {
  return version() == max_element_test::config::flavor::std_max_element;
}
/// @brief obtains the constexpr configuration of tests
/// @tparam Override parameter used to obtain the test configuration
///
/// Determine if tests should be invoked in a constant expression.
template <class Override = max_element_test::config::override>
constexpr auto run_test_in_constexpr() noexcept -> bool {
  constexpr bool using_cpp17 = __cplusplus >= 201703L;
  return !is_std_version() || using_cpp17;
}

}  // namespace max_element_test

// NOLINTBEGIN(readability-identifier-length)

namespace {

// local wrapper so the type list has internal linkage, matching the
// anonymous-namespace fixture and avoiding -Wsubobject-linkage with GCC
template <class T>
struct category_wrapper {
  using iterator_tag = T;
};

using iterator_category_types = ::testing::Types<
    category_wrapper<std::forward_iterator_tag>,
    category_wrapper<std::bidirectional_iterator_tag>,
    category_wrapper<std::random_access_iterator_tag> >;

using arene::base::to_array;

template <typename T>
class MaxElement : public testing::Test {};

TYPED_TEST_SUITE(MaxElement, iterator_category_types, );

/// @test @c max_element returns an iterator to the largest element, where
/// the largest element is at position 0 of the input range.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MaxElement, SingleLargestAt0, max_element_test::run_test_in_constexpr()) {
  using iterator_tag = typename TypeParam::iterator_tag;

  auto const values = to_array({0, -1, -1});
  auto const range = testing::demoted_view(iterator_tag{}, values);

  auto const it = max_element_test::max_element_fn()(range.begin(), range.end());

  CONSTEXPR_ASSERT_EQ(it, arene::base::next(range.begin(), 0));
}

/// @test @c max_element returns an iterator to the largest element, where
/// the largest element is in the middle of the input range.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MaxElement, SingleLargestAt1, max_element_test::run_test_in_constexpr()) {
  using iterator_tag = typename TypeParam::iterator_tag;

  auto const values = to_array({-1, 0, -1});
  auto const range = testing::demoted_view(iterator_tag{}, values);

  auto const it = max_element_test::max_element_fn()(range.begin(), range.end());

  CONSTEXPR_ASSERT_EQ(it, arene::base::next(range.begin(), 1));
}

/// @test @c max_element returns an iterator to the largest element, where
/// the largest element is at the end of the input range.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MaxElement, SingleLargestAt2, max_element_test::run_test_in_constexpr()) {
  using iterator_tag = typename TypeParam::iterator_tag;

  auto const values = to_array({-1, -1, 0});
  auto const range = testing::demoted_view(iterator_tag{}, values);

  auto const it = max_element_test::max_element_fn()(range.begin(), range.end());

  CONSTEXPR_ASSERT_EQ(it, arene::base::next(range.begin(), 2));
}

/// @test @c max_element returns an iterator to the first occurence of the
/// largest element, if there are multiple, and where the first occurence is at
/// position 0 of the input range.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MaxElement, MultipleLargestAt0, max_element_test::run_test_in_constexpr()) {
  using iterator_tag = typename TypeParam::iterator_tag;

  auto const values = to_array({0, 0, -1});
  auto const range = testing::demoted_view(iterator_tag{}, values);

  auto const it = max_element_test::max_element_fn()(range.begin(), range.end());

  CONSTEXPR_ASSERT_EQ(it, arene::base::next(range.begin(), 0));
}

/// @test @c max_element returns an iterator to the first occurence of the
/// largest element, if there are multiple, and where the first occurence is at
/// position 1 of the input range.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MaxElement, MultipleLargestAt1, max_element_test::run_test_in_constexpr()) {
  using iterator_tag = typename TypeParam::iterator_tag;

  auto const values = to_array({-1, 0, 0});
  auto const range = testing::demoted_view(iterator_tag{}, values);

  auto const it = max_element_test::max_element_fn()(range.begin(), range.end());

  CONSTEXPR_ASSERT_EQ(it, arene::base::next(range.begin(), 1));
}

/// @test @c max_element returns an iterator to the first occurence of the
/// largest element, if there are multiple, and where the second occurence is
/// not adjacent to the first occurence.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    MaxElement,
    MultipleLargestNotContiguous,
    max_element_test::run_test_in_constexpr()
) {
  using iterator_tag = typename TypeParam::iterator_tag;

  auto const values = to_array({0, -1, 0});
  auto const range = testing::demoted_view(iterator_tag{}, values);

  auto const it = max_element_test::max_element_fn()(range.begin(), range.end());

  CONSTEXPR_ASSERT_EQ(it, arene::base::next(range.begin(), 0));
}

/// @test @c max_element returns @c last if the range is empty.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MaxElement, EmptyRange, max_element_test::run_test_in_constexpr()) {
  using iterator_tag = typename TypeParam::iterator_tag;

  auto const values = arene::base::array<int, 0>{};
  auto const range = testing::demoted_view(iterator_tag{}, values);

  auto const it = max_element_test::max_element_fn()(range.begin(), range.end());

  CONSTEXPR_ASSERT_EQ(it, range.end());
}

/// @test @c max_element uses a custom comparator if one is provided.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MaxElement, CustomComparator, max_element_test::run_test_in_constexpr()) {
  using iterator_tag = typename TypeParam::iterator_tag;

  auto const values = to_array({0, -2, -1});
  auto const range = testing::demoted_view(iterator_tag{}, values);

  struct less_than_negated {
    constexpr auto operator()(int lhs, int rhs) const noexcept -> bool { return -lhs < -rhs; }
  };

  auto const it = max_element_test::max_element_fn()(range.begin(), range.end(), less_than_negated{});

  CONSTEXPR_ASSERT_EQ(it, arene::base::next(range.begin(), 1));
}

/// @test @c max_element performs exactly max(N - 1 , 0) comparisons.
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MaxElement, MeetsComplexityRequirement, max_element_test::run_test_in_constexpr()) {
  using iterator_tag = typename TypeParam::iterator_tag;

  auto const values = to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  auto const begin = ::testing::make_demoted_iterator<iterator_tag>(values.begin());

  for (auto n = std::size_t{}; n != values.size(); ++n) {
    auto const end = ::testing::make_demoted_iterator<iterator_tag>(values.begin() + static_cast<std::ptrdiff_t>(n));

    auto result = testing::counted_algorithm(
        max_element_test::max_element_fn(),
        begin,
        end,
        arene::base::functional_detail::operator_less
    );

    if (n == 0U) {
      CONSTEXPR_ASSERT_EQ(result.applications, 0U);
    } else {
      CONSTEXPR_ASSERT_EQ(result.applications, n - 1U);
    }
  }
}

/// @test @c max_element is @c noexcept if iterator operations and
/// application of the default comparison object are @c noexcept.
///
TEST(MaxElement, NoexceptIfIteratorOpsAndCompareAreNoexcept) {
  using iterator_type = int const*;

  ASSERT_TRUE(noexcept(max_element_test::max_element_fn()(iterator_type{}, iterator_type{})));
}

/// @test @c max_element is not @c noexcept if the comparison can throw.
///
TEST(MaxElement, NotNoexceptIfComparisonCanThrow) {
  using iterator_type = int const*;

  auto const cmp = [](auto const&...) noexcept(false) { return true; };

  // one of the arguments must be 'declval', otherwise using GCC8 and C++17 will
  // *always* deduce 'noexcept' as 'true'
  ASSERT_FALSE(noexcept(max_element_test::max_element_fn()(std::declval<iterator_type>(), iterator_type{}, cmp)));
}

/// @test @c max_element is not @c noexcept if iterator operations can throw
///
TEST(MaxElement, NotNoexceptIfIteratorOperationsCanThrow) {
  auto const is_algorithm_noexcept = [](auto iter) { return noexcept(max_element_test::max_element_fn()(iter, iter)); };

  ASSERT_TRUE(is_algorithm_noexcept(testing::noexcept_configurable_random_access_iterator<testing::throws_on::nothing>{}
  ));

  ASSERT_FALSE(
      is_algorithm_noexcept(testing::noexcept_configurable_random_access_iterator<testing::throws_on::copy_construct>{})
  );
  ASSERT_FALSE(
      is_algorithm_noexcept(testing::noexcept_configurable_random_access_iterator<testing::throws_on::copy_assign>{})
  );
  ASSERT_FALSE(
      is_algorithm_noexcept(testing::noexcept_configurable_random_access_iterator<testing::throws_on::dereference>{})
  );
  ASSERT_FALSE(
      is_algorithm_noexcept(testing::noexcept_configurable_random_access_iterator<testing::throws_on::pre_increment>{})
  );
  ASSERT_FALSE(is_algorithm_noexcept(testing::noexcept_configurable_random_access_iterator<testing::throws_on::equal>{})
  );
  ASSERT_FALSE(
      is_algorithm_noexcept(testing::noexcept_configurable_random_access_iterator<testing::throws_on::not_equal>{})
  );
}

/// @test all overloads of @c max_element are not invocable with input
/// iterator types or output iterator types
///
TEST(MaxElement, RequiresForwardIterator) {
  auto const is_invocable_with = [](auto category, auto... cmp) {
    using iterator_type = testing::demoted_iterator<int*, decltype(category)>;
    return arene::base::
        is_invocable_v<decltype(max_element_test::max_element_fn()), iterator_type, iterator_type, decltype(cmp)...>;
  };

  ASSERT_TRUE(is_invocable_with(std::forward_iterator_tag{}));
  ASSERT_TRUE(is_invocable_with(std::forward_iterator_tag{}, testing::always_true));

  ASSERT_FALSE(is_invocable_with(std::input_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with(std::output_iterator_tag{}));

  ASSERT_FALSE(is_invocable_with(std::input_iterator_tag{}, testing::always_true));
  ASSERT_FALSE(is_invocable_with(std::output_iterator_tag{}, testing::always_true));
}

/// @test @c max_element requires a binary predicate where the iterator
/// reference type is convertible for arg1 and for arg2
///
TEST(MaxElement, RequiresBinaryPredicate) {
  auto const is_invocable_with = [](auto iter, auto... cmp) {
    return arene::base::
        is_invocable_v<decltype(max_element_test::max_element_fn()), decltype(iter), decltype(iter), decltype(cmp)...>;
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

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_MAX_ELEMENT_HPP_
