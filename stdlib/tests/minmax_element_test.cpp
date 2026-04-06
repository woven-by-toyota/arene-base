// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/iterator/next.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/cstddef"
#include "stdlib/include/functional"
#include "stdlib/include/initializer_list"
#include "stdlib/include/iterator"
#include "stdlib/include/tuple"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/constexpr_traits.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/iterator_types.hpp"

// NOLINTBEGIN(readability-identifier-length,hicpp-avoid-c-arrays)

namespace {
namespace test {

/// @brief calculates the maximum number of comparisons given the size of a range
/// @tparam T integral type
/// @param size size of a range
///
template <class T>
constexpr auto max_comparisons_for(T size) -> std::size_t {
  return size == 0U ? 0U : (3U * (static_cast<std::size_t>(size) - 1U)) / 2U;
}

/// @brief strongly typed argmin type
/// @tparam N argmin value
///
template <std::ptrdiff_t N>
struct argmin_t : std::integral_constant<std::ptrdiff_t, N> {};
template <std::ptrdiff_t N>
constexpr extern auto argmin = argmin_t<N>{};

/// @brief strongly typed argmax type
/// @tparam N argmax value
///
template <std::ptrdiff_t N>
struct argmax_t : std::integral_constant<std::ptrdiff_t, N> {};
template <std::ptrdiff_t N>
constexpr extern auto argmax = argmax_t<N>{};

}  // namespace test

using iterator_category_types =
    ::testing::Types<std::forward_iterator_tag, std::bidirectional_iterator_tag, std::random_access_iterator_tag>;

template <typename TypeParam>
struct MinMaxElement : testing::Test {
  /// @brief checks the result of @c std::mixmax_element using a user defined
  ///   comparison, demoting the category of the input range
  /// @tparam T range value type
  /// @tparam N range size
  /// @tparam Min argmin value
  /// @tparam Max argmax value
  /// @tparam Compare comparison type
  /// @param values range of values
  /// @param argmin index of the min element in @c range
  /// @param argmin index of the max element in @c range
  /// @param cmp comparison value
  ///
  template <class T, std::size_t N, std::ptrdiff_t Min, std::ptrdiff_t Max, class Compare>
  static constexpr auto
  assert_minmax_element_result(T const (&values)[N], test::argmin_t<Min>, test::argmax_t<Max>, Compare cmp) {
    auto const range = testing::demoted_view(TypeParam{}, values);

    auto const result = testing::counted_algorithm(FUNCTION_LIFT(std::minmax_element), range, cmp);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    ASSERT_EQ(result.iter.first.base(), std::begin(values) + Min);
    ASSERT_EQ(result.iter.second.base(), std::begin(values) + Max);
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    ASSERT_TRUE(result.applications <= test::max_comparisons_for(N));
  }

  /// @brief checks the result of @c std::mixmax_element using the standard
  ///   comparison, demoting the category of the input range
  /// @tparam T range value type
  /// @tparam N range size
  /// @tparam Min argmin value
  /// @tparam Max argmax value
  /// @param values range of values
  /// @param argmin index of the min element in @c range
  /// @param argmin index of the max element in @c range
  ///
  template <class T, std::size_t N, std::ptrdiff_t Min, std::ptrdiff_t Max>
  static constexpr auto assert_minmax_element_result(T const (&values)[N], test::argmin_t<Min>, test::argmax_t<Max>) {
    assert_minmax_element_result(
        values,
        test::argmin<Min>,
        test::argmax<Max>,
        arene::base::algorithm_detail::operator_less
    );

    auto const range = testing::demoted_view(TypeParam{}, values);
    ASSERT_EQ(
        std::minmax_element(range.begin(), range.end(), arene::base::algorithm_detail::operator_less),
        std::minmax_element(range.begin(), range.end())
    );
  }
};

TYPED_TEST_SUITE(MinMaxElement, iterator_category_types, );

constexpr bool using_cpp17 = __cplusplus >= 201703L;

/// @test @c std::mixmax_element returns iterators to first for an empty range
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MinMaxElement, EmptyRange, using_cpp17) {
  int const value{};
  auto const range = testing::demoted_view(TypeParam{}, &value, &value);
  auto const result = testing::counted_algorithm(
      FUNCTION_LIFT(std::minmax_element),
      range,
      arene::base::algorithm_detail::operator_less
  );

  ASSERT_EQ(result.iter.first, range.begin());
  ASSERT_EQ(result.iter.second, range.begin());
  ASSERT_EQ(result.applications, 0);
  ASSERT_EQ(result.iter, std::minmax_element(range.begin(), range.end()));
}

/// @test @c std::mixmax_element returns iterators to first for a range with a single element
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MinMaxElement, SingleElementRange, using_cpp17) {
  TestFixture::assert_minmax_element_result({1}, test::argmin<0>, test::argmax<0>);
}

/// @test @c std::mixmax_element returns iterators to the first smallest element
/// as the min and the last largest element as the max for a range with two
/// elements
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MinMaxElement, TwoElements, using_cpp17) {
  TestFixture::assert_minmax_element_result({0, 0}, test::argmin<0>, test::argmax<1>);
  TestFixture::assert_minmax_element_result({0, 1}, test::argmin<0>, test::argmax<1>);
  TestFixture::assert_minmax_element_result({0, 2}, test::argmin<0>, test::argmax<1>);
  TestFixture::assert_minmax_element_result({1, 0}, test::argmin<1>, test::argmax<0>);
}

/// @test @c std::mixmax_element returns iterators to the first smallest element
/// as the min and the last largest element as the max for a range with three
/// elements
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MinMaxElement, ThreeElements, using_cpp17) {
  TestFixture::assert_minmax_element_result({0, 0, 0}, test::argmin<0>, test::argmax<2>);
  TestFixture::assert_minmax_element_result({0, 0, 1}, test::argmin<0>, test::argmax<2>);
  TestFixture::assert_minmax_element_result({0, 1, 1}, test::argmin<0>, test::argmax<2>);
  TestFixture::assert_minmax_element_result({0, 1, 2}, test::argmin<0>, test::argmax<2>);
  TestFixture::assert_minmax_element_result({0, 0, 2}, test::argmin<0>, test::argmax<2>);

  TestFixture::assert_minmax_element_result({0, 1, 0}, test::argmin<0>, test::argmax<1>);
  TestFixture::assert_minmax_element_result({0, 2, 1}, test::argmin<0>, test::argmax<1>);

  TestFixture::assert_minmax_element_result({2, 0, 1}, test::argmin<1>, test::argmax<0>);
  TestFixture::assert_minmax_element_result({2, 0, 0}, test::argmin<1>, test::argmax<0>);

  TestFixture::assert_minmax_element_result({1, 2, 0}, test::argmin<2>, test::argmax<1>);
  TestFixture::assert_minmax_element_result({2, 2, 0}, test::argmin<2>, test::argmax<1>);

  TestFixture::assert_minmax_element_result({2, 1, 0}, test::argmin<2>, test::argmax<0>);
}

/// @test @c std::mixmax_element returns iterators to the first smallest element
/// as the min and the last largest element as the max for a range with four
/// elements
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MinMaxElement, FourElements, using_cpp17) {
  TestFixture::assert_minmax_element_result({0, 0, 0, 0}, test::argmin<0>, test::argmax<3>);
  TestFixture::assert_minmax_element_result({0, 1, 2, 3}, test::argmin<0>, test::argmax<3>);
  TestFixture::assert_minmax_element_result({0, 1, 3, 2}, test::argmin<0>, test::argmax<2>);
  TestFixture::assert_minmax_element_result({0, 3, 1, 2}, test::argmin<0>, test::argmax<1>);
  TestFixture::assert_minmax_element_result({3, 0, 1, 2}, test::argmin<1>, test::argmax<0>);
  TestFixture::assert_minmax_element_result({3, 1, 0, 2}, test::argmin<2>, test::argmax<0>);
  TestFixture::assert_minmax_element_result({3, 1, 2, 0}, test::argmin<3>, test::argmax<0>);
}

/// @test @c std::mixmax_element returns iterators to the first smallest element
/// as the min and the last largest element as the max for a range with five
/// elements
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MinMaxElement, FiveElements, using_cpp17) {
  TestFixture::assert_minmax_element_result({0, 0, 0, 0, 0}, test::argmin<0>, test::argmax<4>);
  TestFixture::assert_minmax_element_result({0, 1, 2, 3, 4}, test::argmin<0>, test::argmax<4>);
  TestFixture::assert_minmax_element_result({0, 1, 2, 4, 3}, test::argmin<0>, test::argmax<3>);
  TestFixture::assert_minmax_element_result({0, 1, 4, 2, 3}, test::argmin<0>, test::argmax<2>);
  TestFixture::assert_minmax_element_result({0, 4, 1, 2, 3}, test::argmin<0>, test::argmax<1>);
  TestFixture::assert_minmax_element_result({4, 0, 1, 2, 3}, test::argmin<1>, test::argmax<0>);
  TestFixture::assert_minmax_element_result({4, 1, 0, 2, 3}, test::argmin<2>, test::argmax<0>);
  TestFixture::assert_minmax_element_result({4, 1, 2, 0, 3}, test::argmin<3>, test::argmax<0>);
  TestFixture::assert_minmax_element_result({4, 1, 2, 3, 0}, test::argmin<4>, test::argmax<0>);
  TestFixture::assert_minmax_element_result({4, 3, 2, 1, 0}, test::argmin<4>, test::argmax<0>);
}

/// @test @c std::mixmax_element returns iterators to the first smallest element
/// as the min and the last largest element as the max with a custom compare function
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MinMaxElement, WithCustomCompare, using_cpp17) {
  auto const cmp = std::greater_equal<>{};

  TestFixture::assert_minmax_element_result({0, 0, 0, 0, 0}, test::argmin<4>, test::argmax<0>, cmp);
  TestFixture::assert_minmax_element_result({0, 1, 2, 3, 4}, test::argmin<4>, test::argmax<0>, cmp);
  TestFixture::assert_minmax_element_result({0, 1, 2, 4, 3}, test::argmin<3>, test::argmax<0>, cmp);
  TestFixture::assert_minmax_element_result({0, 1, 4, 2, 3}, test::argmin<2>, test::argmax<0>, cmp);
  TestFixture::assert_minmax_element_result({0, 4, 1, 2, 3}, test::argmin<1>, test::argmax<0>, cmp);
  TestFixture::assert_minmax_element_result({4, 0, 1, 2, 3}, test::argmin<0>, test::argmax<1>, cmp);
  TestFixture::assert_minmax_element_result({4, 1, 0, 2, 3}, test::argmin<0>, test::argmax<2>, cmp);
  TestFixture::assert_minmax_element_result({4, 1, 2, 0, 3}, test::argmin<0>, test::argmax<3>, cmp);
  TestFixture::assert_minmax_element_result({4, 1, 2, 3, 0}, test::argmin<0>, test::argmax<4>, cmp);
  TestFixture::assert_minmax_element_result({4, 3, 2, 1, 0}, test::argmin<0>, test::argmax<4>, cmp);
}

/// @test @c std::mixmax_element only requires the values to be less than comparable
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(MinMaxElement, OnlyLessThanComparable, using_cpp17) {
  struct less_than_comparable {
    int value;

    constexpr less_than_comparable(int arg)  // NOLINT(hicpp-explicit-conversions)
        : value{arg} {}

    constexpr auto operator<(less_than_comparable const& other) const -> bool { return value < other.value; }
  };

  // can't implicitly convert braced list of ints with gcc8
  {
    less_than_comparable const values[] = {0, 0, 0, 0, 0};
    TestFixture::assert_minmax_element_result(values, test::argmin<0>, test::argmax<4>);
  }
  {
    less_than_comparable const values[] = {0, 1, 2, 3, 4};
    TestFixture::assert_minmax_element_result(values, test::argmin<0>, test::argmax<4>);
  }
  {
    less_than_comparable const values[] = {0, 1, 2, 4, 3};
    TestFixture::assert_minmax_element_result(values, test::argmin<0>, test::argmax<3>);
  }
  {
    less_than_comparable const values[] = {0, 1, 4, 2, 3};
    TestFixture::assert_minmax_element_result(values, test::argmin<0>, test::argmax<2>);
  }
  {
    less_than_comparable const values[] = {0, 4, 1, 2, 3};
    TestFixture::assert_minmax_element_result(values, test::argmin<0>, test::argmax<1>);
  }
  {
    less_than_comparable const values[] = {4, 0, 1, 2, 3};
    TestFixture::assert_minmax_element_result(values, test::argmin<1>, test::argmax<0>);
  }
  {
    less_than_comparable const values[] = {4, 1, 0, 2, 3};
    TestFixture::assert_minmax_element_result(values, test::argmin<2>, test::argmax<0>);
  }
  {
    less_than_comparable const values[] = {4, 1, 2, 0, 3};
    TestFixture::assert_minmax_element_result(values, test::argmin<3>, test::argmax<0>);
  }
  {
    less_than_comparable const values[] = {4, 1, 2, 3, 0};
    TestFixture::assert_minmax_element_result(values, test::argmin<4>, test::argmax<0>);
  }
  {
    less_than_comparable const values[] = {4, 3, 2, 1, 0};
    TestFixture::assert_minmax_element_result(values, test::argmin<4>, test::argmax<0>);
  }
}

/// @brief determine if @c std::minmax_element is nothrow invocable
/// @tparam I iterator type
/// @tparam Compare comparison type(if provided)
///
/// Determine if @c std::minmax_element is nothrow invocable with iterator type @c I
/// and the given comparison object (if provided).
///
/// @return @c true if nothrow invocable, otherwise @c false
///
template <class I, class... Compare>
auto is_nothrow_invocable_with(Compare...) -> bool {
  static_assert(sizeof...(Compare) < 2, "");
  return noexcept(std::minmax_element(std::declval<I>(), std::declval<I>(), std::declval<Compare>()...));
}

/// @test @c std::minmax_element is not @c noexcept if the comparison can throw
///
TEST(MinMaxElement, NotNoexceptIfComparisonCanThrow) {
  ASSERT_TRUE(is_nothrow_invocable_with<int*>());

  auto const cmp = [](auto const&...) noexcept(false) { return true; };
  ASSERT_FALSE(is_nothrow_invocable_with<int*>(cmp));

  struct throwing_compare {
    auto operator<(throwing_compare) const noexcept(false) -> bool;
  };
  ASSERT_FALSE(is_nothrow_invocable_with<throwing_compare*>());
}

/// @test @c std::minmax_element is not @c noexcept if iterator operations can throw
///
TEST(MinMaxElement, NotNoexceptIfIteratorOperationsCanThrow) {
  auto const is_noexcept_if_iterator = [](auto specifier_constant, auto... cmp) {
    using iterator = testing::noexcept_configurable_forward_iterator<specifier_constant>;
    return noexcept(std::minmax_element(iterator{}, iterator{}, cmp...));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing, testing::always_true));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_construct, testing::always_true));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_assign, testing::always_true));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference, testing::always_true));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::pre_increment, testing::always_true));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::post_increment, testing::always_true));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::equal, testing::always_true));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal, testing::always_true));

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_construct));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::copy_assign));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::pre_increment));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::post_increment));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::equal));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal));
}

/// @test all overloads of @c std::max_element are not invocable with input
/// iterator types or output iterator types
///
TEST(MinMaxElement, RequiresForwardIterator) {
  auto const is_invocable_with = [](auto category, auto... cmp) {
    using iterator_type = testing::demoted_iterator<int*, decltype(category)>;
    auto const lifted = FUNCTION_LIFT(std::minmax_element);
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
TEST(MinMaxElement, RequiresBinaryPredicate) {
  auto const is_invocable_with = [](auto iter, auto... cmp) {
    auto const lifted = FUNCTION_LIFT(std::minmax_element);
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

/// @test @c std::max_element is @c constexpr invocable in C++17 but not in C++14
///
TYPED_TEST(MinMaxElement, ConditionallyConstexpr) {
  struct invoke_minmax_element {
    constexpr auto operator()() const {
      int const value{};
      auto const range = testing::demoted_view(TypeParam{}, &value, &value);
      std::ignore = std::minmax_element(range.begin(), range.end());
    }
  };

  ASSERT_EQ(using_cpp17, testing::is_constexpr_invocable_v<invoke_minmax_element>);
}

}  // namespace

// NOLINTEND(readability-identifier-length,hicpp-avoid-c-arrays)
