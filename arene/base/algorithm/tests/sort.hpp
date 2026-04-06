// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_SORT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_SORT_HPP_

#include <gmock/gmock.h>

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/algorithm/iota.hpp"
#include "arene/base/algorithm/sort.hpp"
#include "arene/base/algorithm/tests/generated_test_data.hpp"
#include "arene/base/algorithm/tests/sort_fwd.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/math/log2.hpp"
#include "arene/base/stdlib_choice/add_pointer.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/end.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/greater.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/for_each.hpp"
#include "arene/base/type_list/filter.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace sort_test {

/// @brief specifies the sort algorithm under test
/// @tparam Override parameter used to obtain the test configuration
///
/// To override the default value, this must be set by specializing
///   @c ::sort_test::config::version<::sort_test::config::override>
///   prior to including this header
///
/// @return enum value of @c ::sort_test::config::flavor
///
template <class Override = sort_test::config::override>
constexpr auto version() noexcept -> sort_test::config::flavor {
  return ::sort_test::config::version<Override>;
}
/// @brief provides the @c sort algorithm as a function object
/// @tparam Override parameter used to obtain the test configuration
///
/// To override the default value, this must be set by specializing
///   @c ::sort_test::config::function_under_test<::sort_test::config::override>
///   prior to including this header
///
///
/// @return @c ::sort_test::config::std_version<::sort_test::config::override>
///
template <class Override = sort_test::config::override>
constexpr auto sort_fn() noexcept -> auto& {
  return ::sort_test::config::function_under_test<Override>;
}
/// @brief specifies if the sort algorithm comes from @c std
/// @tparam Override parameter used to obtain the test configuration
///
template <class Override = sort_test::config::override>
constexpr auto is_std_version() noexcept -> bool {
  return version() == sort_test::config::flavor::std_sort ||  //
         version() == sort_test::config::flavor::std_stable_sort;
}
/// @brief obtains the constexpr configuration of tests
/// @tparam Override parameter used to obtain the test configuration
///
/// Determine if tests should be invoked in a constant expression.
///
template <class Override = sort_test::config::override>
constexpr auto run_test_in_constexpr() noexcept -> bool {
  return !is_std_version<Override>();
}
/// @brief obtains the comparison complexity limit
/// @tparam Override parameter used to obtain the test configuration
/// @param size size of the range
///
/// Upper bound for number of comparisons used during the call to sort with a
/// range of size @c size.
///
template <class Override = sort_test::config::override>
constexpr auto complexity_limit(std::size_t size) noexcept -> std::size_t {
  // The specified value is based on the current implementation of 'std::sort',
  // which is implemented using heap sort. The standard specifies the complexity
  // as 'O(N log N)' but in our tests we provide an explicit upper bound.
  //
  // This value has not been explicitly updated for 'stable_sort' as it provides
  // an upper bound with existing test cases.
  return size == 0U ? 0U : (3 * size) + (2 * size) * arene::base::log2(size);
}

// check if a range is sorted
// NOTE: there is currently no constexpr is_sorted function available in
// `arene::base`
// @{

template <class I, class C>
constexpr auto is_sorted(I first, I last, C comp) -> bool {
  if (first == last) {
    return true;
  }

  auto next = first;
  while (++next != last) {
    if (comp(*next, *first)) {
      return false;
    }
    first = next;
  }

  return true;
}

template <class R, class C>
constexpr auto is_sorted(R& values, C comp) -> bool {
  return sort_test::is_sorted(values.begin(), values.end(), comp);
}

// @}

// NOLINTBEGIN(hicpp-avoid-c-arrays)

// create an @c arene::base::array from a C-array with a possibly-different value type
//
template <class T, std::size_t... Is, class U>
constexpr auto to_array_of(std::index_sequence<Is...>, U const (&values)[sizeof...(Is)])
    -> arene::base::array<T, sizeof...(Is)> {
  return {T{values[Is]}...};
}
template <class T, class U, std::size_t N>
constexpr auto to_array_of(U const (&values)[N]) -> arene::base::array<T, N> {
  return to_array_of<T>(std::make_index_sequence<N>{}, values);
}

// NOLINTEND(hicpp-avoid-c-arrays)

// invoke @c sort and check:
// * algorithm postconditions are satisfied
// * algorithm complexity does not exceed the complexity limit (if @c comp is provided)
//
constexpr struct {
  template <class T, std::size_t N, class C>
  constexpr auto operator()(arene::base::array<T, N> values, C comp) const {
    auto const result = testing::counted_algorithm(sort_test::sort_fn(), values, comp);

    CONSTEXPR_ASSERT(result.applications <= sort_test::complexity_limit(N));
    CONSTEXPR_ASSERT(sort_test::is_sorted(values, comp));
  }

  template <class T, std::size_t N>
  constexpr auto operator()(arene::base::array<T, N> values) const {
    using compare = std::conditional_t<  //
        is_std_version(),
        decltype(arene::base::algorithm_detail::operator_less),
        std::less<>>;

    auto copied_values = values;

    sort_test::sort_fn()(values.begin(), values.end());
    CONSTEXPR_ASSERT(sort_test::is_sorted(values, compare{}));

    auto const result = testing::counted_algorithm(sort_test::sort_fn(), copied_values, compare{});
    CONSTEXPR_ASSERT(result.applications <= sort_test::complexity_limit(N));
    CONSTEXPR_ASSERT(arene::base::equal(values.begin(), values.end(), copied_values.begin()));
  }
} checked_sort{};

// @}

}  // namespace sort_test

namespace {

template <class T>
struct Sort : ::testing::Test {};
template <class T>
struct SortLessComparable : ::testing::Test {};

template <class T, class C = std::less<>>
struct test_case {
  using value_type = T;
  using compare_type = C;
};

// compare function object for a 'configurable_value'
//
struct custom_compare {
  template <class T>
  constexpr auto operator()(T const& lhs, T const& rhs) const -> bool {
    return lhs.value < rhs.value;
  }
};

using test_types = ::testing::Types<                                                            //
    test_case<int>,                                                                             //
    test_case<                                                                                  //
        testing::configurable_value<int, testing::throws_on::nothing, testing::disable::less>,  //
        custom_compare                                                                          //
        >,                                                                                      //
    test_case<testing::configurable_value<
        int,
        testing::throws_on::nothing,
        (testing::disable::equal |                                                             //
         testing::disable::not_equal |                                                         //
         testing::disable::less_equal |                                                        //
         testing::disable::greater |                                                           //
         testing::disable::greater_equal)>>,                                                   //
    test_case<                                                                                 //
        testing::configurable_value<int, testing::throws_on::nothing, testing::disable::swap>  //
        >                                                                                      //
    >;

template <class T>
using is_value_type_less_than_comparable =
    std::integral_constant<bool, arene::base::is_less_than_comparable_v<typename T::value_type>>;

using less_comparable_test_types = arene::base::type_lists::filter_t<  //
    test_types,
    is_value_type_less_than_comparable>;

TYPED_TEST_SUITE(Sort, test_types, );
TYPED_TEST_SUITE(SortLessComparable, less_comparable_test_types, );

/// @test @c sort on a 0 element range leaves the elements sorted
/// and does not exceed the complexity limit
///
TYPED_TEST(Sort, EmptyRange) {
  auto const comp = typename TypeParam::compare_type{};

  using T = typename TypeParam::value_type;

  sort_test::checked_sort(arene::base::array<T, 0>{}, comp);
}

/// @test @c sort on a 1 element range leaves the elements sorted
/// and does not exceed the complexity limit
///
TYPED_TEST(Sort, SingleElementRange) {
  auto const comp = typename TypeParam::compare_type{};

  using T = typename TypeParam::value_type;

  sort_test::checked_sort(sort_test::to_array_of<T>({0}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({1}), comp);
}

/// @test @c sort on a 2 element range leaves the elements sorted
/// and does not exceed the complexity limit
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Sort, TwoElementRange, ::sort_test::run_test_in_constexpr()) {
  auto const comp = typename TypeParam::compare_type{};

  using T = typename TypeParam::value_type;

  sort_test::checked_sort(sort_test::to_array_of<T>({2, 1}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({1, 1}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({1, 2}), comp);
}

/// @test @c sort on a 3 element range leaves the elements sorted
/// and does not exceed the complexity limit
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Sort, ThreeElementRange, ::sort_test::run_test_in_constexpr()) {
  auto const comp = typename TypeParam::compare_type{};

  using T = typename TypeParam::value_type;

  sort_test::checked_sort(sort_test::to_array_of<T>({0, 0, 0}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({1, 1, 1}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({2, 2, 2}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({0, 0, 1}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({0, 1, 0}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({1, 0, 0}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({0, 0, 2}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({0, 2, 0}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({2, 0, 0}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({0, 1, 2}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({1, 2, 0}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({2, 0, 1}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({2, 0, 2}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({2, 1, 2}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({1, 2, 2}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({2, 2, 1}), comp);
}

/// @test @c sort on a 10 element range leaves the elements sorted
/// and does not exceed the complexity limit
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Sort, TenElementRange, ::sort_test::run_test_in_constexpr()) {
  auto const comp = typename TypeParam::compare_type{};

  using T = typename TypeParam::value_type;

  sort_test::checked_sort(sort_test::to_array_of<T>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({0, 1, 2, 3, 4, 7, 6, 5, 8, 9}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({0, 1, 8, 3, 4, 5, 6, 7, 2, 9}), comp);
  sort_test::checked_sort(sort_test::to_array_of<T>({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}), comp);
}

/// @test @c sort on a 20 element range leaves the elements sorted
/// and does not exceed the complexity limit
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Sort, TwentyElementRange, ::sort_test::run_test_in_constexpr()) {
  auto const comp = typename TypeParam::compare_type{};

  using T = typename TypeParam::value_type;

  sort_test::checked_sort(
      sort_test::to_array_of<T>({
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0   //
      }),
      comp
  );

  sort_test::checked_sort(
      sort_test::to_array_of<T>({
          1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0   //
      }),
      comp
  );

  sort_test::checked_sort(
      sort_test::to_array_of<T>({
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //
          0, 0, 0, 0, 0, 0, 0, 0, 0, 1   //
      }),
      comp
  );

  sort_test::checked_sort(
      sort_test::to_array_of<T>({
          0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  //
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0   //
      }),
      comp
  );

  sort_test::checked_sort(
      sort_test::to_array_of<T>({
          1, 0, 0, 0, 0, 0, 0, 0, 0, 1,  //
          0, 0, 0, 0, 0, 0, 0, 0, 0, 1   //
      }),
      comp
  );
}

/// @test @c sort on a 100 element range leaves the elements sorted
/// and does not exceed the complexity limit
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Sort, OneHundredElementRange, ::sort_test::run_test_in_constexpr()) {
  auto const comp = typename TypeParam::compare_type{};

  using T = typename TypeParam::value_type;

  sort_test::checked_sort(arene::base::array<T, 100>{}, comp);

  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  {
    int increasing[100] = {};
    arene::base::iota(std::begin(increasing), std::end(increasing), int{});
    sort_test::checked_sort(sort_test::to_array_of<T>(increasing), comp);
  }

  {
    int decreasing[100] = {};
    arene::base::iota(
        arene::base::make_reverse_iterator(std::end(decreasing)),
        arene::base::make_reverse_iterator(std::begin(decreasing)),
        int{}
    );
    sort_test::checked_sort(sort_test::to_array_of<T>(decreasing), comp);
  }
  // NOLINTEND(hicpp-avoid-c-arrays)
}

/// @test @c sort with std::greater as the comparator sorts in decending order
///
TEST(Sort, WithStdGreater) {
  auto values = sort_test::to_array_of<int>({2, 1, 3});
  sort_test::checked_sort(values, std::greater<>{});

  sort_test::sort_fn()(values.begin(), values.end(), std::greater<>{});

  ASSERT_THAT(values, ::testing::ElementsAreArray({3, 2, 1}));
}

/// @test after runnning @c sort on generated data, the values are sorted
///
TEST(Sort, OnGeneratedData) { arene::base::for_each(algorithm_test::test_data, sort_test::checked_sort); }

/// @test @c std::sort uses operator less for comparison and @c
/// arene::base::sort uses @c std::less if a comparator is not provided
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(SortLessComparable, UsesOperatorLess, ::sort_test::run_test_in_constexpr()) {
  using T = typename TypeParam::value_type;

  sort_test::checked_sort(sort_test::to_array_of<T>({2, 1}));
  sort_test::checked_sort(sort_test::to_array_of<T>({1, 1}));
  sort_test::checked_sort(sort_test::to_array_of<T>({1, 2}));
}

/// @test @c sort is not @c noexcept if members of the range value type can throw
///
TEST(Sort, NoexceptQualIfValueTypeCanThrow) {
  auto const is_nothrow_invocable_with = [](auto value) {
    using pointer_type = std::add_pointer_t<decltype(value)>;
    return noexcept(sort_test::sort_fn()(std::declval<pointer_type>(), std::declval<pointer_type>()));
  };

  using testing::disable;
  using testing::throws_on;

  EXPECT_TRUE(is_nothrow_invocable_with(testing::configurable_value<int, throws_on::nothing>{}));
  EXPECT_FALSE(is_nothrow_invocable_with(testing::configurable_value<int, throws_on::less>{}));
  EXPECT_FALSE(is_nothrow_invocable_with(testing::configurable_value<int, throws_on::move_construct, disable::swap>{}));
  EXPECT_FALSE(is_nothrow_invocable_with(testing::configurable_value<int, throws_on::move_assign, disable::swap>{}));

  if (sort_test::version() == sort_test::config::flavor::sort ||
      sort_test::version() == sort_test::config::flavor::std_sort) {
    EXPECT_FALSE(is_nothrow_invocable_with(testing::configurable_value<int, throws_on::swap>{}));
  } else {
    // 'stable_sort' never calls 'swap'
    EXPECT_TRUE(is_nothrow_invocable_with(testing::configurable_value<int, throws_on::swap>{}));
  }
}

/// @test @c sort is not @c noexcept if the predicate can throw
///
TEST(Sort, NotNoexceptIfPredicateCanThrow) {
  auto const pred1 = [](auto const&...) noexcept(true) { return true; };
  auto const pred2 = [](auto const&...) noexcept(false) { return true; };

  ASSERT_TRUE(noexcept(sort_test::sort_fn()(std::declval<int*>(), std::declval<int*>(), pred1)));
  ASSERT_FALSE(noexcept(sort_test::sort_fn()(std::declval<int*>(), std::declval<int*>(), pred2)));
}

/// @test @c sort is not @c noexcept if iterator operations can throw
///
TEST(Sort, NotNoexceptIfIteratorOperationsCanThrow) {
  auto const is_noexcept_if_iterator = [](auto specifier_constant) {
    using iterator = testing::noexcept_configurable_random_access_iterator<specifier_constant>;
    return noexcept(sort_test::sort_fn()(iterator{}, iterator{}));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_construct));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_assign));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::pre_increment));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::plus_difference));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::minus_self));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::equal));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal));

  if (sort_test::version() == sort_test::config::flavor::sort ||
      sort_test::version() == sort_test::config::flavor::std_sort) {
    ASSERT_FALSE(is_noexcept_if_iterator(throws_on::pre_decrement));
  } else {
    // iterator decrement is not used in 'stable_sort'
    ASSERT_TRUE(is_noexcept_if_iterator(throws_on::pre_decrement));
  }
}

/// @brief determine if @c sort is invocable
/// @tparam T iterator value type
/// @tparam Category iterator category type
/// @tparam Pred binary predicate (is provided)
///
/// Determine if @c sort is invocable with an iterator type and a
/// binary predicate (if provided). The iterator type has a @c value_type of @c
/// T and all operations required by @c Category.
///
/// @return @c true if invocable, otherwise @c false
///
template <class T, class Category = std::random_access_iterator_tag, class... Pred>
auto is_invocable_with(Category = {}, Pred...) -> bool {
  using iterator_type = testing::demoted_iterator<T*, Category>;
  return arene::base::is_invocable_v<decltype(sort_test::sort_fn()), iterator_type, iterator_type, Pred...>;
}

/// @test @c sort is not invocable with iterator types with
/// category weaker than bidirectional
///
TEST(Sort, RequiresForwardIterator) {
  ASSERT_TRUE(is_invocable_with<int>(std::random_access_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::bidirectional_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::forward_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::input_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::output_iterator_tag{}));
}

/// @test @c sort requires a binary predicate for the iterator reference type
///
TEST(Sort, RequiresBinaryPredicate) {
  ASSERT_TRUE(is_invocable_with<int>(std::random_access_iterator_tag{}, [](int, int) { return true; }));
  ASSERT_TRUE(is_invocable_with<int>(std::random_access_iterator_tag{}, [](int, int) { return std::true_type{}; }));

  struct not_convertible_from_int {};
  ASSERT_FALSE(is_invocable_with<int>(
      std::random_access_iterator_tag{},
      [](not_convertible_from_int, not_convertible_from_int) { return true; }
  ));

  struct not_convertible_to_bool {};
  ASSERT_FALSE(is_invocable_with<int>(std::random_access_iterator_tag{}, [](int, int) {
    return not_convertible_to_bool{};
  }));

  struct no_operator_less {};
  ASSERT_FALSE(is_invocable_with<no_operator_less>());

  struct operator_less_result_not_bool {
    auto operator<(operator_less_result_not_bool) const -> not_convertible_to_bool;
  };
  ASSERT_FALSE(is_invocable_with<operator_less_result_not_bool>());
}

/// @test @c sort requires a range of swappable types
///
TEST(Sort, RequiresSwappable) {
  ASSERT_TRUE(is_invocable_with<int>());
  ASSERT_FALSE(is_invocable_with<int const>());

  ASSERT_TRUE((is_invocable_with<testing::configurable_value<  //
                   int,
                   testing::throws_on::nothing,
                   testing::disable::swap>>()));
  ASSERT_FALSE((is_invocable_with<testing::configurable_value<  //
                    int,
                    testing::throws_on::nothing,
                    testing::disable::move_construct>>()));
  ASSERT_FALSE((is_invocable_with<testing::configurable_value<
                    int,
                    testing::throws_on::nothing,
                    testing::disable::move_construct | testing::disable::swap>>()));
}

}  // namespace

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_SORT_HPP_
