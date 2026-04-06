// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/inplace_merge.hpp"

#include <gmock/gmock.h>

#include "arene/base/algorithm/detail/heap_sort.hpp"
#include "arene/base/algorithm/tests/generated_test_data.hpp"
#include "arene/base/algorithm/transform.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/math/log2.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_sorted.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

/// @brief binary predicate with disabled special member functions
template <testing::disable Disable = testing::disable::nothing>
struct always_returns_true : testing::configurable_value<int, testing::throws_on::nothing, Disable> {
  auto operator()(int, int) const -> bool { return true; }
};

template <class T>
struct InplaceMergeOnGeneratedData : ::testing::Test {};

using test_data_indices = std::make_index_sequence<std::tuple_size<decltype(algorithm_test::test_data)>::value>;

template <std::size_t... Is>
auto test_index_constant_types(std::index_sequence<Is...>)
    -> ::testing::Types<std::integral_constant<std::size_t, Is>...>;

TYPED_TEST_SUITE(InplaceMergeOnGeneratedData, decltype(test_index_constant_types(test_data_indices{})), );

/// @test @c inplace_merge with an empty first range returns the same range unchanged
CONSTEXPR_TYPED_TEST(InplaceMergeOnGeneratedData, EmptyFirstRange) {
  using std::get;
  auto values = get<TypeParam::value>(algorithm_test::test_data);

  arene::base::algorithm_detail::heap_sort(values.begin(), values.end(), std::less<>{});
  auto const copied_values = values;

  auto count = std::size_t{};
  arene::base::inplace_merge(values.begin(), values.begin(), values.end(), testing::counted<std::less<>>{count});
  CONSTEXPR_ASSERT(values == copied_values);

  // this is a sanity check for the current implementation
  // there is no requirement that count is 0 if one of the ranges is empty
  CONSTEXPR_ASSERT(count == 0U);
}

/// @test @c inplace_merge with an empty second range returns the same range unchanged
CONSTEXPR_TYPED_TEST(InplaceMergeOnGeneratedData, EmptySecondRange) {
  using std::get;
  auto values = get<TypeParam::value>(algorithm_test::test_data);

  arene::base::algorithm_detail::heap_sort(values.begin(), values.end(), std::less<>{});
  auto const copied_values = values;

  auto count = std::size_t{};
  arene::base::inplace_merge(values.begin(), values.end(), values.end(), testing::counted<std::less<>>{count});
  CONSTEXPR_ASSERT(values == copied_values);

  // this is a sanity check for the current implementation
  // there is no requirement that count is 0 if one of the ranges is empty
  CONSTEXPR_ASSERT(count == 0U);
}

/// @test @c inplace_merge leaves the resulting range sorted
/// @note This will exceed the default constexpr step limit if we try to do this
///   at compile time
TYPED_TEST(InplaceMergeOnGeneratedData, ResultRangeIsSorted) {
  using std::get;
  auto const test_values = get<TypeParam::value>(algorithm_test::test_data);

  auto const sorted_values = [&test_values] {
    auto copied = test_values;
    arene::base::algorithm_detail::heap_sort(copied.begin(), copied.end(), std::less<>{});
    return copied;
  }();

  for (auto left_size = std::size_t{}; left_size <= test_values.size(); ++left_size) {
    auto local_values = test_values;

    auto const first = local_values.begin();
    auto const middle = local_values.begin() + static_cast<std::ptrdiff_t>(left_size);
    auto const last = local_values.end();

    arene::base::algorithm_detail::heap_sort(first, middle, std::less<>{});
    arene::base::algorithm_detail::heap_sort(middle, last, std::less<>{});

    auto count = std::size_t{};
    arene::base::inplace_merge(first, middle, last, testing::counted<std::less<>>{count});

    ASSERT_THAT(local_values, ::testing::ElementsAreArray(sorted_values));

    // While the standard allows O(N * log(N)), we can provide an upper bound
    // for this implementation.
    if (local_values.empty()) {
      ASSERT_EQ(count, 0U);
    } else {
      auto const n = local_values.size();  // NOLINT(readability-identifier-length)
      using arene::base::log2;
      ASSERT_LE(count, n * log2(n));
    }
  }
}

/// @test @c inplace_merge is usable with bidirectional ranges
CONSTEXPR_TYPED_TEST(InplaceMergeOnGeneratedData, UsableWithBidirectional) {
  using std::get;
  auto values = get<TypeParam::value>(algorithm_test::test_data);

  arene::base::algorithm_detail::heap_sort(values.begin(), values.end(), std::less<>{});
  auto copied_values = values;

  auto view = testing::demoted_view(std::bidirectional_iterator_tag{}, copied_values);
  arene::base::inplace_merge(view.begin(), view.begin(), view.end(), std::less<>{});

  arene::base::inplace_merge(values.begin(), values.begin(), values.end(), std::less<>{});

  CONSTEXPR_ASSERT(values == copied_values);
}

/// @test @c inplace_merge is stable
TYPED_TEST(InplaceMergeOnGeneratedData, Stable) {
  using std::get;
  auto const test_values = get<TypeParam::value>(algorithm_test::test_data);

  struct indexed_value {
    std::size_t index;
    int value;
  };

  auto const to_indexed_values = [](auto arr) {
    constexpr auto size = std::tuple_size<decltype(arr)>::value;

    auto tmp = arene::base::array<indexed_value, size>{};
    arene::base::transform(
        arene::base::sequential_values<std::size_t, size>.begin(),
        arene::base::sequential_values<std::size_t, size>.end(),
        tmp.begin(),
        [&arr](auto index) -> indexed_value {
          return {index, arr[index]};
        }
    );
    return tmp;
  };

  for (auto left_size = std::size_t{}; left_size <= test_values.size(); ++left_size) {
    auto local_values = [&to_indexed_values, &test_values, left_size] {
      auto values = test_values;
      auto const first = values.begin();
      auto const middle = values.begin() + static_cast<std::ptrdiff_t>(left_size);
      auto const last = values.end();

      arene::base::algorithm_detail::heap_sort(first, middle, std::less<>{});
      arene::base::algorithm_detail::heap_sort(middle, last, std::less<>{});

      return to_indexed_values(values);
    }();

    auto const first = local_values.begin();
    auto const middle = local_values.begin() + static_cast<std::ptrdiff_t>(left_size);
    auto const last = local_values.end();

    auto const cmp_value = [](indexed_value lhs, indexed_value rhs) { return lhs.value < rhs.value; };
    arene::base::inplace_merge(first, middle, last, cmp_value);

    auto const cmp_indexed_value = [](indexed_value lhs, indexed_value rhs) {
      return lhs.value == rhs.value  //
                 ? lhs.index < rhs.index
                 : lhs.value < rhs.value;
    };

    ASSERT_TRUE(std::is_sorted(local_values.begin(), local_values.end(), cmp_indexed_value));
  }
}

/// @brief determine if @c inplace_merge is invocable
/// @tparam T iterator value type
/// @tparam Category iterator category type
/// @tparam Compare compare function object (defaults to 'std::less')
///
/// Determine if @c inplace_merge is invocable with an iterator type and a
/// compare. The iterator type has a @c value_type of @c T and all operations
/// required by @c Category.
///
/// @return @c true if invocable, otherwise @c false
///
template <class T, class Category = std::random_access_iterator_tag, class Compare = std::less<>>
auto is_invocable_with(Category = {}, Compare = {}) -> bool {
  using iterator_type = testing::demoted_iterator<T*, Category>;
  return arene::base::is_invocable_v<  //
      decltype(arene::base::inplace_merge),
      iterator_type,
      iterator_type,
      iterator_type,
      Compare>;
}

/// @test @c inplace_merge requires bidirectional iterator or stronger
TEST(InplaceMerge, RequiresBidirectionalIterator) {
  ASSERT_TRUE(is_invocable_with<int>(std::random_access_iterator_tag{}));
  ASSERT_TRUE(is_invocable_with<int>(std::bidirectional_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::forward_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::input_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::output_iterator_tag{}));
}

/// @test @c inplace_merge requires value swappable and movable
TEST(Sort, RequiresSwappableAndMovable) {
  ASSERT_TRUE(is_invocable_with<int>());
  ASSERT_FALSE(is_invocable_with<int const>());

  ASSERT_TRUE((is_invocable_with<testing::configurable_value<int, testing::throws_on::nothing, testing::disable::swap>>(
  )));

  ASSERT_FALSE((is_invocable_with<
                testing::configurable_value<int, testing::throws_on::nothing, testing::disable::move_construct>>()));
  ASSERT_FALSE((is_invocable_with<testing::configurable_value<
                    int,
                    testing::throws_on::nothing,
                    testing::disable::move_construct | testing::disable::swap>>()));
}

/// @test @c inplace_merge requires compare
TEST(Sort, RequiresCompare) {
  ASSERT_TRUE(is_invocable_with<int>(std::random_access_iterator_tag{}, [](int, int) { return true; }));
  ASSERT_TRUE(is_invocable_with<int>(std::random_access_iterator_tag{}, [](int, int) { return std::true_type{}; }));
  ASSERT_TRUE(is_invocable_with<int>(std::random_access_iterator_tag{}, always_returns_true<>{}));

  ASSERT_FALSE(is_invocable_with<int>(std::random_access_iterator_tag{}, [](int&, int&) { return true; }));

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

  ASSERT_FALSE(
      (is_invocable_with<int, std::random_access_iterator_tag, always_returns_true<testing::disable::move_construct>>())
  );
  ASSERT_FALSE(
      (is_invocable_with<int, std::random_access_iterator_tag, always_returns_true<testing::disable::copy_construct>>())
  );
}

/// @test @c inplace_merge is noexcept(false) if compare can throw
TEST(InplaceMerge, NotNoexceptIfCompareCanThrow) {
  auto const pred1 = [](auto const&...) noexcept(true) { return true; };
  auto const pred2 = [](auto const&...) noexcept(false) { return true; };

  ASSERT_TRUE(noexcept(arene::base::inplace_merge(  //
      std::declval<int*>(),
      std::declval<int*>(),
      std::declval<int*>(),
      pred1
  )));
  ASSERT_FALSE(noexcept(arene::base::inplace_merge(  //
      std::declval<int*>(),
      std::declval<int*>(),
      std::declval<int*>(),
      pred2
  )));
}

/// @test @c inplace_merge is noexcept(false) if value type operations can throw
TEST(InplaceMerge, NoexceptQualIfValueTypeCanThrow) {
  auto const is_nothrow_invocable_with = [](auto iter_category, auto value) {
    using iterator = ::testing::demoted_iterator<decltype(value)*, decltype(iter_category)>;
    return noexcept(arene::base::inplace_merge(
        std::declval<iterator>(),
        std::declval<iterator>(),
        std::declval<iterator>(),
        std::less<>{}
    ));
  };

  using testing::disable;
  using testing::throws_on;

  EXPECT_TRUE(is_nothrow_invocable_with(
      std::random_access_iterator_tag{},
      testing::configurable_value<int, throws_on::nothing>{}
  ));
  EXPECT_FALSE(is_nothrow_invocable_with(  //
      std::random_access_iterator_tag{},
      testing::configurable_value<int, throws_on::less>{}
  ));
  EXPECT_FALSE(is_nothrow_invocable_with(
      std::random_access_iterator_tag{},
      testing::configurable_value<int, throws_on::move_construct, disable::swap>{}
  ));
  EXPECT_FALSE(is_nothrow_invocable_with(
      std::random_access_iterator_tag{},
      testing::configurable_value<int, throws_on::move_assign, disable::swap>{}
  ));

  EXPECT_TRUE(is_nothrow_invocable_with(
      std::bidirectional_iterator_tag{},
      testing::configurable_value<int, throws_on::nothing>{}
  ));
  EXPECT_FALSE(is_nothrow_invocable_with(  //
      std::bidirectional_iterator_tag{},
      testing::configurable_value<int, throws_on::less>{}
  ));
  EXPECT_FALSE(is_nothrow_invocable_with(
      std::bidirectional_iterator_tag{},
      testing::configurable_value<int, throws_on::move_construct, disable::swap>{}
  ));
  EXPECT_FALSE(is_nothrow_invocable_with(
      std::bidirectional_iterator_tag{},
      testing::configurable_value<int, throws_on::move_assign, disable::swap>{}
  ));

  // 'swap' is not used by 'rotate' for random access iterators
  EXPECT_TRUE(is_nothrow_invocable_with(  //
      std::random_access_iterator_tag{},
      testing::configurable_value<int, throws_on::swap>{}
  ));
  EXPECT_FALSE(is_nothrow_invocable_with(  //
      std::bidirectional_iterator_tag{},
      testing::configurable_value<int, throws_on::swap>{}
  ));
}

/// @test @c inplace_merge is noexcept(false) if iterator operations can throw
TEST(InplaceMerge, NotNoexceptIfIteratorOperationsCanThrow) {
  auto const is_noexcept_if_iterator = [](auto specifier_constant) {
    using iterator = testing::noexcept_configurable_random_access_iterator<specifier_constant>;
    return noexcept(arene::base::inplace_merge(iterator{}, iterator{}, iterator{}, std::less<>{}));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing));

  EXPECT_FALSE(is_noexcept_if_iterator(throws_on::copy_construct));
  EXPECT_FALSE(is_noexcept_if_iterator(throws_on::copy_assign));
  EXPECT_FALSE(is_noexcept_if_iterator(throws_on::dereference));
  EXPECT_FALSE(is_noexcept_if_iterator(throws_on::pre_increment));
  EXPECT_FALSE(is_noexcept_if_iterator(throws_on::plus_equals_difference));
  EXPECT_FALSE(is_noexcept_if_iterator(throws_on::minus_self));
  EXPECT_FALSE(is_noexcept_if_iterator(throws_on::equal));
  EXPECT_FALSE(is_noexcept_if_iterator(throws_on::not_equal));
}

}  // namespace
