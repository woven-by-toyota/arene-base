// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/functional"
#include "stdlib/include/memory"
#include "stdlib/include/tuple"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"

namespace {

template <class T, bool NoExcept = true>
struct less_than_comparable {
  constexpr explicit less_than_comparable(T value)
      : value_(value) {}
  constexpr auto operator<(less_than_comparable const& rhs) const noexcept(NoExcept) -> bool {
    return value_ < rhs.value_;
  }
  constexpr auto value() const -> T const& { return value_; }

 private:
  T value_;
};

template <class T>
struct type_with_overloaded_address_operator {
  constexpr explicit type_with_overloaded_address_operator(T value)
      : value_(value) {}
  constexpr auto operator<(type_with_overloaded_address_operator const& rhs) const -> bool {
    return value_ < rhs.value_;
  }

  // NOLINTNEXTLINE(google-runtime-operator)
  constexpr auto operator&() const -> type_with_overloaded_address_operator const* { return nullptr; }

 private:
  T value_;
};

using max_types = arene::base::type_lists::concat_unique_t<
    ::testing::signed_integer_types,
    ::testing::unsigned_integer_types,
    ::testing::floating_point_types>;

template <typename T>
class MaxTest : public testing::Test {};

TYPED_TEST_SUITE(MaxTest, max_types, );

/// @test The return type of @c max is @c T&
TYPED_TEST(MaxTest, HasRightReturnType) {
  testing::
      StaticAssertTypeEq<decltype(std::max(std::declval<TypeParam>(), std::declval<TypeParam>())), TypeParam const&>();
  testing::
      StaticAssertTypeEq<decltype(std::max(std::declval<TypeParam&>(), std::declval<TypeParam&>())), TypeParam const&>(
      );

  testing::StaticAssertTypeEq<
      decltype(std::max(std::declval<TypeParam const>(), std::declval<TypeParam const>())),
      TypeParam const&>();
  testing::StaticAssertTypeEq<
      decltype(std::max(std::declval<TypeParam const&>(), std::declval<TypeParam const&>())),
      TypeParam const&>();
  testing::StaticAssertTypeEq<
      decltype(std::max(std::declval<TypeParam const&&>(), std::declval<TypeParam const&&>())),
      TypeParam const&>();

  testing::StaticAssertTypeEq<
      decltype(std::max(std::declval<TypeParam const volatile&>(), std::declval<TypeParam const volatile&>())),
      TypeParam const volatile&>();
}

/// @test @c std::max is noexcept if comparison is noexcept
TYPED_TEST(MaxTest, NoExceptIfComparisonIsNoExcept) {
  EXPECT_TRUE(noexcept(std::max(TypeParam{}, TypeParam{})));
  EXPECT_TRUE(noexcept(std::max(TypeParam{}, TypeParam{}, std::less<>{})));

  // Make sure this instantiation of @c std::max is tracked by coverage.
  std::ignore = std::max(TypeParam{0}, TypeParam{1});
  std::ignore = std::max(TypeParam{1}, TypeParam{0});
  std::ignore = std::max(TypeParam{0}, TypeParam{1}, std::less<>{});
  std::ignore = std::max(TypeParam{1}, TypeParam{0}, std::less<>{});
}

/// @test @c std::max is not noexcept if comparison is not noexcept
TYPED_TEST(MaxTest, NotNoExceptIfComparisonIsNotNoExcept) {
  using T = less_than_comparable<TypeParam, false>;
  // at least one argument must be 'declval', otherwise GCC8 with -std=c++17 will *always* deduce 'noexcept' as 'true'

  EXPECT_FALSE(noexcept(std::max(std::declval<T&>(), std::declval<T&>())));

  auto const nothrow_cmp = std::less<>{};
  auto const throwing_cmp = [](auto const& lhs, auto const& rhs) noexcept(false) -> bool { return lhs < rhs; };
  EXPECT_TRUE(noexcept(std::max(std::declval<TypeParam&>(), TypeParam{}, nothrow_cmp)));
  EXPECT_FALSE(noexcept(std::max(std::declval<TypeParam&>(), TypeParam{}, throwing_cmp)));

  // Make sure this instantiation of @c std::max is tracked by coverage.
  std::ignore = std::max(TypeParam{0}, TypeParam{1}, nothrow_cmp);
  std::ignore = std::max(TypeParam{1}, TypeParam{0}, nothrow_cmp);
  std::ignore = std::max(TypeParam{0}, TypeParam{1}, throwing_cmp);
  std::ignore = std::max(TypeParam{1}, TypeParam{0}, throwing_cmp);
}

/// @test @c std::max returns the larger of two values as defined by @c operator<
CONSTEXPR_TYPED_TEST(MaxTest, ReturnsLargerValue) {
  auto const smaller = TypeParam{0};
  auto const larger = TypeParam{1};

  EXPECT_EQ(std::addressof(std::max(smaller, larger)), std::addressof(larger));
  EXPECT_EQ(std::addressof(std::max(larger, smaller)), std::addressof(larger));
}

/// @test @c std::max only requires @c operator< to be defined
CONSTEXPR_TYPED_TEST(MaxTest, WorksWithOnlyLessThanOperator) {
  auto const smaller = less_than_comparable<TypeParam>{0};
  auto const larger = less_than_comparable<TypeParam>{1};

  EXPECT_EQ(std::addressof(std::max(smaller, larger)), std::addressof(larger));
  EXPECT_EQ(std::addressof(std::max(larger, smaller)), std::addressof(larger));
}

/// @test @c std::max returns a reference when @c operator& is overloaded
CONSTEXPR_TYPED_TEST(MaxTest, WorksWhenAddressOfOperatorIsOverloaded) {
  auto const smaller = type_with_overloaded_address_operator<TypeParam>{0};
  auto const larger = type_with_overloaded_address_operator<TypeParam>{1};

  EXPECT_EQ(std::addressof(std::max(smaller, larger)), std::addressof(larger));
  EXPECT_EQ(std::addressof(std::max(larger, smaller)), std::addressof(larger));
}

/// @test @c std::max returns the the first argument when the arguments are equal
CONSTEXPR_TYPED_TEST(MaxTest, ReturnsFirstWhenEqual) {
  auto first = TypeParam{0};
  auto second = TypeParam{0};

  EXPECT_EQ(std::addressof(std::max(first, second)), std::addressof(first));
}

template <class T>
struct no_less_than_operator {
  constexpr explicit no_less_than_operator(T value)
      : value_(value) {}

  // NOLINTNEXTLINE(google-runtime-operator)
  constexpr auto operator&() const -> no_less_than_operator const* { return nullptr; }
  constexpr auto value() const -> T const& { return value_; }

 private:
  T value_;
};

template <class T>
struct comparator {
  constexpr auto operator()(T const& lhs, T const& rhs) -> bool { return lhs < rhs; }

  constexpr auto operator()(no_less_than_operator<T> const& lhs, no_less_than_operator<T> const& rhs) -> bool {
    return lhs.value() < rhs.value();
  }
};

/// @test The return type of @c std::max is @c T& when using a custom comparison function
TYPED_TEST(MaxTest, HasRightReturnTypeWithComparisonFunction) {
  using T = no_less_than_operator<TypeParam>;

  testing::
      StaticAssertTypeEq<decltype(std::max(std::declval<T>(), std::declval<T>(), comparator<TypeParam>{})), T const&>();
  testing::
      StaticAssertTypeEq<decltype(std::max(std::declval<T&>(), std::declval<T&>(), comparator<TypeParam>{})), T const&>(
      );

  testing::StaticAssertTypeEq<
      decltype(std::max(std::declval<T const>(), std::declval<T const>(), comparator<TypeParam>{})),
      T const&>();
  testing::StaticAssertTypeEq<
      decltype(std::max(std::declval<T const&>(), std::declval<T const&>(), comparator<TypeParam>{})),
      T const&>();
  testing::StaticAssertTypeEq<
      decltype(std::max(std::declval<T const&&>(), std::declval<T const&&>(), comparator<TypeParam>{})),
      T const&>();
}

/// @test @c std::max returns the larger of two values as defined by a custom comparison function
CONSTEXPR_TYPED_TEST(MaxTest, ReturnsLargerValueWithComparisonFunction) {
  auto const smaller = TypeParam{0};
  auto const larger = TypeParam{1};

  EXPECT_EQ(std::addressof(std::max(smaller, larger, comparator<TypeParam>{})), std::addressof(larger));
  EXPECT_EQ(std::addressof(std::max(larger, smaller, comparator<TypeParam>{})), std::addressof(larger));
}

/// @test @c std::max returns the larger of two values as defined a custom comparison function
CONSTEXPR_TYPED_TEST(MaxTest, CanUseComparisonFunctionWhenNoLessThanOperatorDefined) {
  auto const smaller = no_less_than_operator<TypeParam>{0};
  auto const larger = no_less_than_operator<TypeParam>{1};

  EXPECT_EQ(std::addressof(std::max(smaller, larger, comparator<TypeParam>{})), std::addressof(larger));
  EXPECT_EQ(std::addressof(std::max(larger, smaller, comparator<TypeParam>{})), std::addressof(larger));
}

/// @test @c std::max returns the the first argument when the arguments are equal with a custom comparison function
CONSTEXPR_TYPED_TEST(MaxTest, ReturnsFirstWhenEqualWithComparisonFunction) {
  auto first = no_less_than_operator<TypeParam>{0};
  auto second = no_less_than_operator<TypeParam>{0};

  EXPECT_EQ(std::addressof(std::max(first, second, comparator<TypeParam>{})), std::addressof(first));
}

template <typename T>
class MaxListTest : public MaxTest<T> {};

TYPED_TEST_SUITE(MaxListTest, max_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL(
    "-Wfloat-equal",
    "Not performing arithmetic operations on floating point values, so exact equality is ok."
);

// Helper function to run `arene::base::algorithm_detail::relative_find` with an empty range to get full branch
// coverage.
template <class T, class Compare = arene::base::algorithm_detail::operator_less_t>
constexpr auto run_for_coverage(Compare comp = Compare{}) {
  auto const list = std::initializer_list<T>{};
  std::ignore = std::internal::relative_find(list.begin(), list.end(), arene::base::algorithm_detail::flip(comp));
}

/// @test @c std::max of a @c std::initializer_list enforces preconditions that the type @c T is CopyConstructible and
/// LessThanComparable.
TEST(MinListTest, Preconditions) {
  auto const is_invocable_with = [](auto value, auto... cmp) {
    auto const lifted = FUNCTION_LIFT(std::max);
    return testing::simple_is_invocable_v<decltype(lifted), std::initializer_list<decltype(value)>, decltype(cmp)...>;
  };

  using T = int;

  ASSERT_TRUE(is_invocable_with(T{}));
  ASSERT_TRUE(is_invocable_with(T{}, std::less<>{}));

  using not_copy_constructible = testing::only_movable<T>;
  ASSERT_FALSE(is_invocable_with(not_copy_constructible{}));
  ASSERT_FALSE(is_invocable_with(not_copy_constructible{}, std::less<>{}));

  struct not_comparable {};
  ASSERT_FALSE(is_invocable_with(not_comparable{}));
  ASSERT_FALSE(is_invocable_with(not_comparable{}, std::less<>{}));

  struct not_convertible_to_bool {};
  ASSERT_FALSE(is_invocable_with(T{}, testing::always_constant<not_convertible_to_bool>{}));

  // TODO: Add a test for the precondition that the input list is not empty.
}

/// @test @c std::max is @c noexcept if application of the default comparison object is @c noexcept.
TYPED_TEST(MaxListTest, NoexceptIfIteratorOpsAndCompareAreNoexcept) {
  ASSERT_TRUE(noexcept(std::max({TypeParam{}, TypeParam{}})));
}

/// @test @c std::max is not @c noexcept if the comparison can throw.
TYPED_TEST(MaxListTest, NotNoexceptIfComparisonCanThrow) {
  auto const throwing_cmp = [](auto const& lhs, auto const& rhs) noexcept(false) -> bool { return lhs < rhs; };
  // one of the arguments must be 'declval', otherwise GCC8 with -std=c++17 will *always* deduce 'noexcept' as 'true'
  ASSERT_FALSE(noexcept(std::max({std::declval<TypeParam&>(), std::declval<TypeParam&>()}, throwing_cmp)));

  // Make sure this instantiation of @c std::min is tracked by coverage.
  std::ignore = std::max(TypeParam{0}, TypeParam{1}, throwing_cmp);
  std::ignore = std::max(TypeParam{1}, TypeParam{0}, throwing_cmp);
}

/// @test @c std::max is not @c noexcept if the type cannot be nothrow copy constructed
TEST(MaxListTest, NotNoexceptIfCopyConstructorCanThrow) {
  struct throwing_copy {
    constexpr throwing_copy()
        : value_{} {}
    constexpr explicit throwing_copy(int value)
        : value_{value} {}
    // GCC8 does not implement P1286, so defaulting this copy constuctor will result in it being deleted.
    // https://open-std.org/JTC1/SC22/WG21/docs/papers/2019/p1286r2.html#approach
    // NOLINTNEXTLINE(hicpp-use-equals-default)
    constexpr throwing_copy(throwing_copy const& other) noexcept(false)
        : value_{other.value_} {}
    constexpr throwing_copy(throwing_copy&&) = default;
    constexpr auto operator=(throwing_copy const&) -> throwing_copy& = default;
    constexpr auto operator=(throwing_copy&&) -> throwing_copy& = default;
    ~throwing_copy() = default;

    constexpr auto operator<(throwing_copy const& rhs) const { return value_ < rhs.value_; }

   private:
    int value_;
  };

  // one of the arguments must be 'declval', otherwise GCC8 will *always* deduce 'noexcept' as 'true'
  ASSERT_FALSE(noexcept(std::max(std::declval<std::initializer_list<throwing_copy>>())));

  // Make sure this instantiation of @c std::max has full coverage.
  std::ignore = std::max({throwing_copy{0}, throwing_copy{1}});
  std::ignore = std::max({throwing_copy{1}, throwing_copy{0}});

  run_for_coverage<throwing_copy>();
}

template <class T>
struct value_and_index {
  T value;
  std::size_t index;
};

template <class T>
constexpr auto operator<(value_and_index<T> const& lhs, value_and_index<T> const& rhs) -> bool {
  return lhs.value < rhs.value;
}

/// @test @c std::max returns the largest element
CONSTEXPR_TYPED_TEST(MaxListTest, SingleLargest) {
  // Note: these three instantiations need to be in the same test to receive full coverage credit.
  auto result = std::max(std::initializer_list<value_and_index<TypeParam>>{{1, 0}, {0, 1}, {0, 2}});
  ASSERT_EQ(result.value, 1);
  ASSERT_EQ(result.index, 0);

  result = std::max(std::initializer_list<value_and_index<TypeParam>>{{0, 0}, {1, 1}, {0, 2}});
  ASSERT_EQ(result.value, 1);
  ASSERT_EQ(result.index, 1);

  result = std::max(std::initializer_list<value_and_index<TypeParam>>{{0, 0}, {0, 1}, {1, 2}});
  ASSERT_EQ(result.value, 1);
  ASSERT_EQ(result.index, 2);

  run_for_coverage<value_and_index<TypeParam>>();
}

/// @test @c std::max returns the largest element when there are multiple consecutive
CONSTEXPR_TYPED_TEST(MaxListTest, MultipleLargest) {
  auto const result = std::max(std::initializer_list<value_and_index<TypeParam>>{{0, 0}, {1, 1}, {1, 2}});
  ASSERT_EQ(result.value, 1);
  ASSERT_EQ(result.index, 1);

  run_for_coverage<value_and_index<TypeParam>>();
}

/// @test @c std::max returns the largest element when there are multiple non-consecutive
CONSTEXPR_TYPED_TEST(MaxListTest, MultipleLargestNotContiguous) {
  auto const result = std::max(std::initializer_list<value_and_index<TypeParam>>{{1, 0}, {0, 1}, {1, 2}});
  ASSERT_EQ(result.value, 1);
  ASSERT_EQ(result.index, 0);

  run_for_coverage<value_and_index<TypeParam>>();
}

/// @test @c std::max only requires @c operator< to be defined
CONSTEXPR_TYPED_TEST(MaxListTest, WorksWithOnlyLessThanOperator) {
  using T = less_than_comparable<TypeParam>;

  auto const result = std::max(std::initializer_list<value_and_index<T>>{{T{0}, 0}, {T{1}, 1}, {T{0}, 2}});
  ASSERT_EQ(result.value.value(), 1);
  ASSERT_EQ(result.index, 1);

  run_for_coverage<value_and_index<T>>();
}

/// @test @c std::max uses a custom comparator if one is provided.
CONSTEXPR_TEST(MaxListTest, CustomComparator) {
  struct less_than_negated {
    constexpr auto operator()(int lhs, int rhs) const noexcept -> bool { return -lhs < -rhs; }
  };
  // Note: If this result is @c const, we do not receive coverage credit for this instantiation.
  auto result = std::max({0, -2, -1}, less_than_negated{});
  ASSERT_EQ(result, -2);

  run_for_coverage<int>(less_than_negated{});
}

/// @test @c std::max performs exactly max(N - 1 , 0) comparisons.
CONSTEXPR_TYPED_TEST(MaxListTest, MeetsComplexityRequirement) {
  std::initializer_list<std::initializer_list<TypeParam>> const test_cases = {
      {0, 1, 2},
      {0, 1, 2, 3, 4, 5},
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
      {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
  };

  for (auto const& test_case : test_cases) {
    auto count = std::size_t{};
    auto cmp = testing::counted<std::decay_t<decltype(arene::base::algorithm_detail::operator_less)>>{count};

    std::ignore = std::max(test_case, cmp);
    ASSERT_EQ(count, test_case.size() - 1);

    run_for_coverage<TypeParam>(cmp);
  }
}

ARENE_IGNORE_END();

}  // namespace
