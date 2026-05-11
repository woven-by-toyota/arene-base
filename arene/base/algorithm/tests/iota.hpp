// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_IOTA_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_IOTA_HPP_

#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/iota_fwd.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/add_pointer.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counter_mixin.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace iota_test {

/// @brief specifies if the @c std version of @c iota is being tested
/// @tparam Override parameter used to obtain the test configuration
///
/// To override the default value, this must be set by specializing
///   @c ::testing::config::std_version<::testing::config::override>
///   prior to including this header
///
/// @return @c ::iota_test::config::std_version<::iota_test::config::override>
///
template <class Override = iota_test::config::override>
constexpr auto is_std_version() noexcept -> bool {
  return ::iota_test::config::std_version<Override>;
}
/// @brief provides the @c iota algorithm as a function object
/// @tparam Override parameter used to obtain the test configuration
///
/// To override the default value, this must be set by specializing
///   @c ::testing::config::std_version<::testing::config::override>
///   prior to including this header
///
///
/// @return @c ::iota_test::config::std_version<::iota_test::config::override>
///
template <class Override = iota_test::config::override>
constexpr auto iota_fn() noexcept -> auto& {
  return ::iota_test::config::iota<Override>;
}
/// @brief obtains the constexpr configuration of tests
/// @tparam Override parameter used to obtain the test configuration
///
/// Determine if tests should be invoked in a constant expression.
///
/// @return <c> ! is_std_version() </c>
///
template <class Override = iota_test::config::override>
constexpr auto run_test_in_constexpr() noexcept -> bool {
  return !is_std_version<Override>();
}

template <bool Convertible, bool Incrementable>
struct custom_converting_type {
  template <bool B = Convertible, class = std::enable_if_t<B>>
  operator int() const {  // NOLINT(hicpp-explicit-conversions)
    return 0;
  }

  template <bool B = Incrementable, class = std::enable_if_t<B>>
  auto operator++() const -> auto& {
    return *this;
  }
};

template <bool Constructible>
struct custom_constructible_type {
  template <bool B = Constructible, class = std::enable_if_t<B>>
  custom_constructible_type(int) {}  // NOLINT(hicpp-explicit-conversions)
};

template <bool Assignable>
struct custom_assignable_type {
  template <bool B = Assignable, class = std::enable_if_t<B>>
  auto operator=(int) -> auto& {
    return *this;
  }
};

// check if a range has sequentially increasing values
// @{

template <class I>
constexpr auto is_sequentially_increasing(I first, I last) -> bool {
  if (first == last) {
    return true;
  }

  auto next = first;
  while (++next != last) {
    auto tmp = *first;
    // there is no requirement that the value define '+' or "one"
    if (*next != ++tmp) {
      return false;
    }
    first = next;
  }

  return true;
}

template <class R>
constexpr auto is_sequentially_increasing(R const& values) -> bool {
  return iota_test::is_sequentially_increasing(values.begin(), values.end());
}

// @}

template <class T>
struct incrementable_counting_wrapper : testing::counter_mixin<incrementable_counting_wrapper<T>> {
  T value{};

  incrementable_counting_wrapper() = default;

  explicit incrementable_counting_wrapper(T arg)
      : value{arg} {
    --testing::counter_mixin<incrementable_counting_wrapper>::count.default_ctor;
    ++testing::counter_mixin<incrementable_counting_wrapper>::count.value_ctor;
  }

  static std::size_t increment_count;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  auto operator++() noexcept -> incrementable_counting_wrapper& {
    ++increment_count;
    ++value;
    return *this;
  }

  friend auto operator==(incrementable_counting_wrapper const& lhs, incrementable_counting_wrapper const& rhs) noexcept
      -> bool {
    return lhs.value == rhs.value;
  }
  friend auto operator!=(incrementable_counting_wrapper const& lhs, incrementable_counting_wrapper const& rhs) noexcept
      -> bool {
    return !(lhs == rhs);
  }
};

template <class T>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::size_t incrementable_counting_wrapper<T>::increment_count = {};

// invoke @c iota and check:
// * algorithm postconditions are satisfied
//
template <class R, class T>
constexpr auto checked_iota(R&& range, T value) {
  ::iota_test::iota_fn()(range.begin(), range.end(), std::move(value));
  CONSTEXPR_ASSERT(iota_test::is_sequentially_increasing(range));
}

}  // namespace iota_test

namespace {

template <class T>
struct Iota : ::testing::Test {};

template <class T, std::size_t N>
struct test_case {
  using value_type = T;
  static constexpr auto size = N;
};

using test_types = ::testing::Types<  //
    test_case<char, 0>,
    test_case<char, 1>,
    test_case<char, 2>,
    test_case<char, 3>,
    test_case<char, 5>,
    test_case<char, 10>,
    test_case<int, 0>,
    test_case<int, 1>,
    test_case<int, 2>,
    test_case<int, 3>,
    test_case<int, 5>,
    test_case<int, 10>>;

TYPED_TEST_SUITE(Iota, test_types, );

/// @test @c iota on a range sets values in sequential order
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Iota, RangeOfValues, ::iota_test::run_test_in_constexpr()) {
  using value_type = typename TypeParam::value_type;
  constexpr auto size = TypeParam::size;

  iota_test::checked_iota(arene::base::array<value_type, size>{}, value_type{});
}

/// @test @c iota on a range sets values in sequential order and has linear
/// complexity
/// @note These values are all user defined types
///
TYPED_TEST(Iota, RangeComplexity) {
  using counted_type = iota_test::incrementable_counting_wrapper<typename TypeParam::value_type>;
  constexpr auto size = TypeParam::size;

  auto values = arene::base::array<counted_type, size>{};

  counted_type::count = {};
  counted_type::increment_count = {};

  ::iota_test::iota_fn()(values.begin(), values.end(), counted_type{});

  auto expected = typename counted_type::count_t{};
  if (::iota_test::is_std_version()) {
    // The 'FUNCTION_LIFT' macro used to define 'iota_test::iota_fn' calls 'std::iota',
    // which takes the initial value by *value*, not by reference.
    // As a result, there will be an additional object created by move
    // construction.
    expected.default_ctor = 1;
    expected.move_ctor = 1;
    expected.dtor = 2;
  } else {
    expected.default_ctor = 1;
    expected.dtor = 1;
  }
  expected.copy_assign = size;
  EXPECT_EQ(expected, counted_type::count);
  EXPECT_EQ(size, counted_type::increment_count);

  EXPECT_TRUE(iota_test::is_sequentially_increasing(values.begin(), values.end()));
}

/// @test @c iota on a range sets values in sequential order where the start does not need to be zero
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Iota, NonZeroStart, ::iota_test::run_test_in_constexpr()) {
  using value_type = typename TypeParam::value_type;
  constexpr auto size = TypeParam::size;

  auto values = arene::base::array<value_type, size>{};

  iota_test::checked_iota(values, value_type{1});

  if (!values.empty()) {
    CONSTEXPR_ASSERT(value_type{1} == values.front());
  }
}

/// @test @c iota can be used with a value type of pointers
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Iota, WithPointersAsValue, ::iota_test::run_test_in_constexpr()) {
  using value_type = typename TypeParam::value_type;
  constexpr auto size = TypeParam::size;

  auto items = arene::base::array<value_type, size + 1>{};
  auto values = arene::base::array<value_type*, size>{};

  iota_test::checked_iota(values, &items.front());

  if (!values.empty()) {
    CONSTEXPR_ASSERT(&items.front() == values.front());
  }
}

/// @test @c iota with a value that is not copyable
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Iota, WithNonCopyableValue, ::iota_test::run_test_in_constexpr()) {
  using value_type = typename TypeParam::value_type;
  constexpr auto size = TypeParam::size;

  // The value type of the target array is the copyable underlying type, not the uncopyable 'configurable_value'
  auto values = arene::base::array<value_type, size>{};

  using increment_type = testing::configurable_value<
      value_type,
      testing::throws_on::nothing,
      testing::disable::copy_construct | testing::disable::copy_assign>;

  // `values` holds the copyable base type, to which 'increment_type' is convertible, so this works
  iota_test::checked_iota(values, increment_type{});
}

/// @test @c iota with a range of non-comparable values
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Iota, WithNonComparableValues, ::iota_test::run_test_in_constexpr()) {
  using value_type = testing::configurable_value<
      typename TypeParam::value_type,
      testing::throws_on::nothing,
      testing::disable::equal | testing::disable::not_equal>;

  constexpr auto size = TypeParam::size;

  auto values = arene::base::array<value_type, size>{};

  iota_test::iota_fn()(values.begin(), values.end(), value_type{});

  auto index = typename TypeParam::value_type{};
  for (auto const& elem : values) {
    CONSTEXPR_ASSERT(elem == index++);
  }
}

/// @test @c iota can be used with forward iterators
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Iota, WithForwardIterators, ::iota_test::run_test_in_constexpr()) {
  using value_type = typename TypeParam::value_type;
  constexpr auto size = TypeParam::size;

  auto values = arene::base::array<value_type, size>{};

  iota_test::checked_iota(  //
      testing::demoted_view(std::forward_iterator_tag{}, values.begin(), values.end()),
      value_type{}
  );
}

/// @test @c iota can be used with bidirectional iterators
///
CONDITIONALLY_CONSTEXPR_TYPED_TEST(Iota, WithBidirectionalterators, ::iota_test::run_test_in_constexpr()) {
  using value_type = typename TypeParam::value_type;
  constexpr auto size = TypeParam::size;

  auto values = arene::base::array<value_type, size>{};

  iota_test::checked_iota(  //
      testing::demoted_view(std::bidirectional_iterator_tag{}, values.begin(), values.end()),
      value_type{}
  );
}

/// @test @c iota is @c noexcept(false) if value type operations can throw
///
TEST(Iota, NoexceptFalseIfValueTypeCanThrow) {
  auto const is_nothrow_invocable_with = [](auto value) {
    using pointer_type = std::add_pointer_t<decltype(value)>;
    return noexcept(iota_test::iota_fn()(  //
        std::declval<pointer_type>(),
        std::declval<pointer_type>(),
        std::declval<decltype(value)>()
    ));
  };

  using testing::throws_on;

  EXPECT_TRUE(is_nothrow_invocable_with(testing::configurable_value<int, throws_on::nothing>{}));
  EXPECT_FALSE(is_nothrow_invocable_with(testing::configurable_value<int, throws_on::copy_assign>{}));
}

/// @test @c iota is @c noexcept(false) if iterator operations can throw
///
TEST(Iota, NoexceptFalseIfIteratorTypeCanThrow) {
  auto const is_noexcept_if_iterator = [](auto specifier_constant) {
    using iterator = testing::noexcept_configurable_forward_iterator<specifier_constant>;
    return noexcept(iota_test::iota_fn()(  //
        std::declval<iterator>(),
        std::declval<iterator>(),
        typename iterator::value_type{}
    ));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_if_iterator(throws_on::nothing));

  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::dereference));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::post_increment));
  ASSERT_FALSE(is_noexcept_if_iterator(throws_on::not_equal));
}

/// @brief determine if @c iota_test::iota_fn is invocable
/// @tparam Iter iterator value type
/// @tparam Category iterator category type
/// @tparam Value increment value type, if provided
///
/// @return @c true if invocable, otherwise @c false
///
template <class IterValue, class Category, class Value = IterValue>
auto is_invocable_with(Category, Value = {}) -> bool {
  using iterator_type = testing::demoted_iterator<IterValue*, Category>;
  return arene::base::is_invocable_v<decltype(iota_test::iota_fn()), iterator_type, iterator_type, Value>;
}

/// @test @c iota is not invocable with iterator types with
/// category weaker than forward
///
TEST(Iota, RequiresForwardIterator) {
  ASSERT_TRUE(is_invocable_with<int>(std::random_access_iterator_tag{}));
  ASSERT_TRUE(is_invocable_with<int>(std::bidirectional_iterator_tag{}));
  ASSERT_TRUE(is_invocable_with<int>(std::forward_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::input_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::output_iterator_tag{}));
}

/// @test @c iota requires <c> ++value </c> to be well formed and for the incrementing value to be
/// convertible to the range value type
///
TEST(Iota, RequiresPreIncrementAndConvertible) {
  ASSERT_TRUE(is_invocable_with<int>(std::forward_iterator_tag{}, int{}));
  ASSERT_TRUE(is_invocable_with<int>(std::forward_iterator_tag{}, iota_test::custom_converting_type<true, true>{}));

  ASSERT_FALSE(is_invocable_with<int>(std::forward_iterator_tag{}, iota_test::custom_converting_type<false, true>{}));
  ASSERT_FALSE(is_invocable_with<int>(std::forward_iterator_tag{}, iota_test::custom_converting_type<true, false>{}));
  ASSERT_FALSE(is_invocable_with<int>(std::forward_iterator_tag{}, iota_test::custom_converting_type<false, false>{}));

  ASSERT_TRUE(is_invocable_with<iota_test::custom_constructible_type<true>>(std::forward_iterator_tag{}, int{}));

  ASSERT_FALSE(is_invocable_with<iota_test::custom_constructible_type<false>>(std::forward_iterator_tag{}, int{}));

  // 'iota' cannot be used with this type, even though assignment would work
  ASSERT_FALSE(is_invocable_with<iota_test::custom_assignable_type<true>>(std::forward_iterator_tag{}, int{}));

  ASSERT_FALSE(is_invocable_with<iota_test::custom_assignable_type<false>>(std::forward_iterator_tag{}, int{}));
}

}  // namespace

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_IOTA_HPP_
