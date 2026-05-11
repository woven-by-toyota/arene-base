// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/constraints.hpp"
#include "stdlib/include/functional"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

// NOLINTBEGIN(modernize-use-transparent-functors)
// NOLINTBEGIN(hicpp-avoid-c-arrays)
// NOLINTBEGIN(cppcoreguidelines-owning-memory)
// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)

template <class T, class = arene::base::constraints<>>
constexpr auto has_is_transparent_v = false;

template <class T>
constexpr auto has_is_transparent_v<T, arene::base::constraints<typename T::is_transparent>> = true;

constexpr std::size_t k_array_size = 10;

/// @test type deduced comparison object must have @c is_transparent
TEST(UntypedComparisonTest, IsTransparent) {
  static_assert(has_is_transparent_v<std::equal_to<>>, "Must have is_transparent");
  static_assert(has_is_transparent_v<std::not_equal_to<>>, "Must have is_transparent");
  static_assert(has_is_transparent_v<std::greater<>>, "Must have is_transparent");
  static_assert(has_is_transparent_v<std::greater_equal<>>, "Must have is_transparent");
  static_assert(has_is_transparent_v<std::less<>>, "Must have is_transparent");
  static_assert(has_is_transparent_v<std::less_equal<>>, "Must have is_transparent");
}

template <class T>
class ComparisonTest : public testing::Test {};

TYPED_TEST_SUITE(ComparisonTest, ::testing::integral_types, );

/// @test comparison object with different values
CONSTEXPR_TYPED_TEST(ComparisonTest, DifferentValues) {
  auto const first = TypeParam{0};
  auto const second = TypeParam{1};

  EXPECT_FALSE(std::equal_to<TypeParam>{}(first, second));
  EXPECT_FALSE(std::equal_to<TypeParam>{}(second, first));
  EXPECT_TRUE(std::not_equal_to<TypeParam>{}(first, second));
  EXPECT_TRUE(std::not_equal_to<TypeParam>{}(second, first));
  EXPECT_FALSE(std::greater<TypeParam>{}(first, second));
  EXPECT_TRUE(std::greater<TypeParam>{}(second, first));
  EXPECT_FALSE(std::greater_equal<TypeParam>{}(first, second));
  EXPECT_TRUE(std::greater_equal<TypeParam>{}(second, first));
  EXPECT_TRUE(std::less<TypeParam>{}(first, second));
  EXPECT_FALSE(std::less<TypeParam>{}(second, first));
  EXPECT_TRUE(std::less_equal<TypeParam>{}(first, second));
  EXPECT_FALSE(std::less_equal<TypeParam>{}(second, first));
}

/// @test comparison object returns false for equal values
CONSTEXPR_TYPED_TEST(ComparisonTest, EqualValues) {
  auto const first = TypeParam{1};
  auto const second = TypeParam{1};

  EXPECT_TRUE(std::equal_to<TypeParam>{}(first, second));
  EXPECT_TRUE(std::equal_to<TypeParam>{}(second, first));
  EXPECT_FALSE(std::not_equal_to<TypeParam>{}(first, second));
  EXPECT_FALSE(std::not_equal_to<TypeParam>{}(second, first));
  EXPECT_FALSE(std::greater<TypeParam>{}(first, second));
  EXPECT_FALSE(std::greater<TypeParam>{}(second, first));
  EXPECT_TRUE(std::greater_equal<TypeParam>{}(first, second));
  EXPECT_TRUE(std::greater_equal<TypeParam>{}(second, first));
  EXPECT_FALSE(std::less<TypeParam>{}(first, second));
  EXPECT_FALSE(std::less<TypeParam>{}(second, first));
  EXPECT_TRUE(std::less_equal<TypeParam>{}(first, second));
  EXPECT_TRUE(std::less_equal<TypeParam>{}(second, first));
}

/// @test comparison object with different values converted to @c bool
CONSTEXPR_TYPED_TEST(ComparisonTest, DifferentValuesConvert) {
  auto const first = TypeParam{0};
  auto const second = TypeParam{1};

  EXPECT_FALSE(std::equal_to<bool>{}(first, second));
  EXPECT_FALSE(std::equal_to<bool>{}(second, first));
  EXPECT_TRUE(std::not_equal_to<bool>{}(first, second));
  EXPECT_TRUE(std::not_equal_to<bool>{}(second, first));
  EXPECT_FALSE(std::greater<bool>{}(first, second));
  EXPECT_TRUE(std::greater<bool>{}(second, first));
  EXPECT_FALSE(std::greater_equal<bool>{}(first, second));
  EXPECT_TRUE(std::greater_equal<bool>{}(second, first));
  EXPECT_TRUE(std::less<bool>{}(first, second));
  EXPECT_FALSE(std::less<bool>{}(second, first));
  EXPECT_TRUE(std::less_equal<bool>{}(first, second));
  EXPECT_FALSE(std::less_equal<bool>{}(second, first));
}

/// @test comparison object returns false for equal values converted to @c bool
CONSTEXPR_TYPED_TEST(ComparisonTest, EqualValuesConvert) {
  auto const first = TypeParam{1};
  auto const second = TypeParam{1};

  EXPECT_TRUE(std::equal_to<bool>{}(first, second));
  EXPECT_TRUE(std::equal_to<bool>{}(second, first));
  EXPECT_FALSE(std::not_equal_to<bool>{}(first, second));
  EXPECT_FALSE(std::not_equal_to<bool>{}(second, first));
  EXPECT_FALSE(std::greater<bool>{}(first, second));
  EXPECT_FALSE(std::greater<bool>{}(second, first));
  EXPECT_TRUE(std::greater_equal<bool>{}(first, second));
  EXPECT_TRUE(std::greater_equal<bool>{}(second, first));
  EXPECT_FALSE(std::less<bool>{}(first, second));
  EXPECT_FALSE(std::less<bool>{}(second, first));
  EXPECT_TRUE(std::less_equal<bool>{}(first, second));
  EXPECT_TRUE(std::less_equal<bool>{}(second, first));
}

/// @test comparison object with values with type deduction
CONSTEXPR_TYPED_TEST(ComparisonTest, DifferentValuesTypeDeduction) {
  auto const first = TypeParam{0};
  auto const second = TypeParam{1};

  EXPECT_FALSE(std::equal_to<>{}(first, second));
  EXPECT_FALSE(std::equal_to<>{}(second, first));
  EXPECT_TRUE(std::not_equal_to<>{}(first, second));
  EXPECT_TRUE(std::not_equal_to<>{}(second, first));
  EXPECT_FALSE(std::greater<>{}(first, second));
  EXPECT_TRUE(std::greater<>{}(second, first));
  EXPECT_FALSE(std::greater_equal<>{}(first, second));
  EXPECT_TRUE(std::greater_equal<>{}(second, first));
  EXPECT_TRUE(std::less<>{}(first, second));
  EXPECT_FALSE(std::less<>{}(second, first));
  EXPECT_TRUE(std::less_equal<>{}(first, second));
  EXPECT_FALSE(std::less_equal<>{}(second, first));
}

/// @test comparison object returns false for equal values with type deduction
CONSTEXPR_TYPED_TEST(ComparisonTest, EqualValuesTypeDeduction) {
  auto const first = TypeParam{1};
  auto const second = TypeParam{1};

  EXPECT_TRUE(std::equal_to<>{}(first, second));
  EXPECT_TRUE(std::equal_to<>{}(second, first));
  EXPECT_FALSE(std::not_equal_to<>{}(first, second));
  EXPECT_FALSE(std::not_equal_to<>{}(second, first));
  EXPECT_FALSE(std::greater<>{}(first, second));
  EXPECT_FALSE(std::greater<>{}(second, first));
  EXPECT_TRUE(std::greater_equal<>{}(first, second));
  EXPECT_TRUE(std::greater_equal<>{}(second, first));
  EXPECT_FALSE(std::less<>{}(first, second));
  EXPECT_FALSE(std::less<>{}(second, first));
  EXPECT_TRUE(std::less_equal<>{}(first, second));
  EXPECT_TRUE(std::less_equal<>{}(second, first));
}

/// @test comparison object with different stack pointers
TYPED_TEST(ComparisonTest, DifferentStackPointers) {
  TypeParam arr[k_array_size]{};
  auto const first = &arr[0];
  auto const second = &arr[1];

  EXPECT_FALSE(std::equal_to<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::equal_to<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::not_equal_to<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::not_equal_to<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::greater<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::greater<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::greater_equal<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::greater_equal<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::less<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::less<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::less_equal<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::less_equal<TypeParam*>{}(second, first));
}

/// @test comparison object returns false for equal stack pointers
TYPED_TEST(ComparisonTest, EqualStackPointers) {
  TypeParam arr[k_array_size]{};
  auto const first = &arr[1];
  auto const second = &arr[1];

  EXPECT_TRUE(std::equal_to<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::equal_to<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::not_equal_to<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::not_equal_to<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::greater<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::greater<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::greater_equal<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::greater_equal<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::less<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::less<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::less_equal<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::less_equal<TypeParam*>{}(second, first));
}

/// @test comparison object with stack pointers with type deduction
TYPED_TEST(ComparisonTest, DifferentStackPointersTypeDeduction) {
  TypeParam arr[k_array_size]{};
  auto const first = &arr[0];
  auto const second = &arr[1];

  EXPECT_FALSE(std::equal_to<>{}(first, second));
  EXPECT_FALSE(std::equal_to<>{}(second, first));
  EXPECT_TRUE(std::not_equal_to<>{}(first, second));
  EXPECT_TRUE(std::not_equal_to<>{}(second, first));
  EXPECT_FALSE(std::greater<>{}(first, second));
  EXPECT_TRUE(std::greater<>{}(second, first));
  EXPECT_FALSE(std::greater_equal<>{}(first, second));
  EXPECT_TRUE(std::greater_equal<>{}(second, first));
  EXPECT_TRUE(std::less<>{}(first, second));
  EXPECT_FALSE(std::less<>{}(second, first));
  EXPECT_TRUE(std::less_equal<>{}(first, second));
  EXPECT_FALSE(std::less_equal<>{}(second, first));
}

/// @test comparison object returns false for equal stack pointers with type deduction
TYPED_TEST(ComparisonTest, EqualStackPointersTypeDeduction) {
  TypeParam arr[k_array_size]{};
  auto const first = &arr[1];
  auto const second = &arr[1];

  EXPECT_TRUE(std::equal_to<>{}(first, second));
  EXPECT_TRUE(std::equal_to<>{}(second, first));
  EXPECT_FALSE(std::not_equal_to<>{}(first, second));
  EXPECT_FALSE(std::not_equal_to<>{}(second, first));
  EXPECT_FALSE(std::greater<>{}(first, second));
  EXPECT_FALSE(std::greater<>{}(second, first));
  EXPECT_TRUE(std::greater_equal<>{}(first, second));
  EXPECT_TRUE(std::greater_equal<>{}(second, first));
  EXPECT_FALSE(std::less<>{}(first, second));
  EXPECT_FALSE(std::less<>{}(second, first));
  EXPECT_TRUE(std::less_equal<>{}(first, second));
  EXPECT_TRUE(std::less_equal<>{}(second, first));
}

/// @test comparison object with different heap pointers
TYPED_TEST(ComparisonTest, DifferentHeapPointers) {
  auto* arr = new TypeParam[k_array_size]{};
  auto const first = &arr[0];
  auto const second = &arr[1];

  EXPECT_FALSE(std::equal_to<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::equal_to<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::not_equal_to<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::not_equal_to<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::greater<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::greater<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::greater_equal<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::greater_equal<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::less<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::less<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::less_equal<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::less_equal<TypeParam*>{}(second, first));

  delete[] arr;
}

/// @test comparison object returns false for equal heap pointers
TYPED_TEST(ComparisonTest, EqualHeapPointers) {
  auto* arr = new TypeParam[k_array_size]{};
  auto const first = &arr[1];
  auto const second = &arr[1];

  EXPECT_TRUE(std::equal_to<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::equal_to<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::not_equal_to<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::not_equal_to<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::greater<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::greater<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::greater_equal<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::greater_equal<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::less<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::less<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::less_equal<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::less_equal<TypeParam*>{}(second, first));

  delete[] arr;
}

/// @test comparison object with different heap pointers with type deduction
TYPED_TEST(ComparisonTest, DifferentHeapPointersTypeDeduction) {
  auto* arr = new TypeParam[k_array_size]{};
  auto const first = &arr[0];
  auto const second = &arr[1];

  EXPECT_FALSE(std::equal_to<>{}(first, second));
  EXPECT_FALSE(std::equal_to<>{}(second, first));
  EXPECT_TRUE(std::not_equal_to<>{}(first, second));
  EXPECT_TRUE(std::not_equal_to<>{}(second, first));
  EXPECT_FALSE(std::greater<>{}(first, second));
  EXPECT_TRUE(std::greater<>{}(second, first));
  EXPECT_FALSE(std::greater_equal<>{}(first, second));
  EXPECT_TRUE(std::greater_equal<>{}(second, first));
  EXPECT_TRUE(std::less<>{}(first, second));
  EXPECT_FALSE(std::less<>{}(second, first));
  EXPECT_TRUE(std::less_equal<>{}(first, second));
  EXPECT_FALSE(std::less_equal<>{}(second, first));

  delete[] arr;
}

/// @test comparison object returns false for equal heap pointers with type deduction
TYPED_TEST(ComparisonTest, EqualHeapPointersTypeDeduction) {
  auto* arr = new TypeParam[k_array_size]{};
  auto const first = &arr[1];
  auto const second = &arr[1];

  EXPECT_TRUE(std::equal_to<>{}(first, second));
  EXPECT_TRUE(std::equal_to<>{}(second, first));
  EXPECT_FALSE(std::not_equal_to<>{}(first, second));
  EXPECT_FALSE(std::not_equal_to<>{}(second, first));
  EXPECT_FALSE(std::greater<>{}(first, second));
  EXPECT_FALSE(std::greater<>{}(second, first));
  EXPECT_TRUE(std::greater_equal<>{}(first, second));
  EXPECT_TRUE(std::greater_equal<>{}(second, first));
  EXPECT_FALSE(std::less<>{}(first, second));
  EXPECT_FALSE(std::less<>{}(second, first));
  EXPECT_TRUE(std::less_equal<>{}(first, second));
  EXPECT_TRUE(std::less_equal<>{}(second, first));

  delete[] arr;
}

/// @test comparison object with different global pointers
TYPED_TEST(ComparisonTest, DifferentGlobalPointers) {
  static TypeParam arr[k_array_size]{};
  auto const first = &arr[0];
  auto const second = &arr[1];

  EXPECT_FALSE(std::equal_to<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::equal_to<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::not_equal_to<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::not_equal_to<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::greater<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::greater<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::greater_equal<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::greater_equal<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::less<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::less<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::less_equal<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::less_equal<TypeParam*>{}(second, first));
}

/// @test comparison object returns false for equal global pointers
TYPED_TEST(ComparisonTest, EqualGlobalPointers) {
  static TypeParam arr[k_array_size]{};
  auto const first = &arr[1];
  auto const second = &arr[1];

  EXPECT_TRUE(std::equal_to<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::equal_to<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::not_equal_to<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::not_equal_to<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::greater<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::greater<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::greater_equal<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::greater_equal<TypeParam*>{}(second, first));
  EXPECT_FALSE(std::less<TypeParam*>{}(first, second));
  EXPECT_FALSE(std::less<TypeParam*>{}(second, first));
  EXPECT_TRUE(std::less_equal<TypeParam*>{}(first, second));
  EXPECT_TRUE(std::less_equal<TypeParam*>{}(second, first));
}

/// @test comparison object with different global pointers with type deduction
TYPED_TEST(ComparisonTest, DifferentGlobalPointersTypeDeduction) {
  static TypeParam arr[k_array_size]{};
  auto const first = &arr[0];
  auto const second = &arr[1];

  EXPECT_FALSE(std::equal_to<>{}(first, second));
  EXPECT_FALSE(std::equal_to<>{}(second, first));
  EXPECT_TRUE(std::not_equal_to<>{}(first, second));
  EXPECT_TRUE(std::not_equal_to<>{}(second, first));
  EXPECT_FALSE(std::greater<>{}(first, second));
  EXPECT_TRUE(std::greater<>{}(second, first));
  EXPECT_FALSE(std::greater_equal<>{}(first, second));
  EXPECT_TRUE(std::greater_equal<>{}(second, first));
  EXPECT_TRUE(std::less<>{}(first, second));
  EXPECT_FALSE(std::less<>{}(second, first));
  EXPECT_TRUE(std::less_equal<>{}(first, second));
  EXPECT_FALSE(std::less_equal<>{}(second, first));
}

/// @test comparison object returns false for equal global pointers with type deduction
TYPED_TEST(ComparisonTest, EqualGlobalPointersTypeDeduction) {
  static TypeParam arr[k_array_size]{};
  auto const first = &arr[1];
  auto const second = &arr[1];

  EXPECT_TRUE(std::equal_to<>{}(first, second));
  EXPECT_TRUE(std::equal_to<>{}(second, first));
  EXPECT_FALSE(std::not_equal_to<>{}(first, second));
  EXPECT_FALSE(std::not_equal_to<>{}(second, first));
  EXPECT_FALSE(std::greater<>{}(first, second));
  EXPECT_FALSE(std::greater<>{}(second, first));
  EXPECT_TRUE(std::greater_equal<>{}(first, second));
  EXPECT_TRUE(std::greater_equal<>{}(second, first));
  EXPECT_FALSE(std::less<>{}(first, second));
  EXPECT_FALSE(std::less<>{}(second, first));
  EXPECT_TRUE(std::less_equal<>{}(first, second));
  EXPECT_TRUE(std::less_equal<>{}(second, first));
}

/// @test comparison object is noexcept for integral types
CONSTEXPR_TYPED_TEST(ComparisonTest, DifferentNoexpectValues) {
  auto const first = TypeParam{0};
  auto const second = TypeParam{1};

  EXPECT_TRUE(noexcept(std::equal_to<TypeParam>{}(first, second)));
  EXPECT_TRUE(noexcept(std::not_equal_to<TypeParam>{}(first, second)));
  EXPECT_TRUE(noexcept(std::greater<TypeParam>{}(first, second)));
  EXPECT_TRUE(noexcept(std::greater_equal<TypeParam>{}(first, second)));
  EXPECT_TRUE(noexcept(std::less<TypeParam>{}(first, second)));
  EXPECT_TRUE(noexcept(std::less_equal<TypeParam>{}(first, second)));
}

/// @test comparison object is noexcept for integral types with type deduction
CONSTEXPR_TYPED_TEST(ComparisonTest, DifferentNoexpectValuesTypeDeduction) {
  auto const first = TypeParam{0};
  auto const second = TypeParam{1};

  EXPECT_TRUE(noexcept(std::equal_to<>{}(first, second)));
  EXPECT_TRUE(noexcept(std::not_equal_to<>{}(first, second)));
  EXPECT_TRUE(noexcept(std::greater<>{}(first, second)));
  EXPECT_TRUE(noexcept(std::greater_equal<>{}(first, second)));
  EXPECT_TRUE(noexcept(std::less<>{}(first, second)));
  EXPECT_TRUE(noexcept(std::less_equal<>{}(first, second)));
}

/// @test comparison object is noexcept for pointers
TYPED_TEST(ComparisonTest, DifferentNoexceptPointers) {
  TypeParam arr[k_array_size]{};
  auto const first = &arr[0];
  auto const second = &arr[1];

  EXPECT_TRUE(noexcept(std::equal_to<TypeParam*>{}(first, second)));
  EXPECT_TRUE(noexcept(std::not_equal_to<TypeParam*>{}(first, second)));
  EXPECT_TRUE(noexcept(std::greater<TypeParam*>{}(first, second)));
  EXPECT_TRUE(noexcept(std::greater_equal<TypeParam*>{}(first, second)));
  EXPECT_TRUE(noexcept(std::less<TypeParam*>{}(first, second)));
  EXPECT_TRUE(noexcept(std::less_equal<TypeParam*>{}(first, second)));
}

/// @test comparison object is noexcept for pointers with type deduction
TYPED_TEST(ComparisonTest, DifferentNoexceptPointersTypeDeduction) {
  TypeParam arr[k_array_size]{};
  auto const first = &arr[0];
  auto const second = &arr[1];

  EXPECT_TRUE(noexcept(std::equal_to<>{}(first, second)));
  EXPECT_TRUE(noexcept(std::not_equal_to<>{}(first, second)));
  EXPECT_TRUE(noexcept(std::greater<>{}(first, second)));
  EXPECT_TRUE(noexcept(std::greater_equal<>{}(first, second)));
  EXPECT_TRUE(noexcept(std::less<>{}(first, second)));
  EXPECT_TRUE(noexcept(std::less_equal<>{}(first, second)));
}

template <class T>
class SignedComparisonTest : public testing::Test {};

TYPED_TEST_SUITE(SignedComparisonTest, ::testing::signed_integer_types, );

/// @test comparison object with values of different, comparable, types
CONSTEXPR_TYPED_TEST(SignedComparisonTest, TypeMismatchDifferentValues) {
  auto const first = int{0};
  auto const second = TypeParam{1};

  EXPECT_FALSE(std::equal_to<>{}(first, second));
  EXPECT_FALSE(std::equal_to<>{}(second, first));
  EXPECT_TRUE(std::not_equal_to<>{}(first, second));
  EXPECT_TRUE(std::not_equal_to<>{}(second, first));
  EXPECT_FALSE(std::greater<>{}(first, second));
  EXPECT_TRUE(std::greater<>{}(second, first));
  EXPECT_FALSE(std::greater_equal<>{}(first, second));
  EXPECT_TRUE(std::greater_equal<>{}(second, first));
  EXPECT_TRUE(std::less<>{}(first, second));
  EXPECT_FALSE(std::less<>{}(second, first));
  EXPECT_TRUE(std::less_equal<>{}(first, second));
  EXPECT_FALSE(std::less_equal<>{}(second, first));
}

/// @test comparison object returns false for equal values of different, comparable, types
CONSTEXPR_TYPED_TEST(SignedComparisonTest, TypeMismatchEqualValues) {
  auto const first = int{1};
  auto const second = TypeParam{1};

  EXPECT_TRUE(std::equal_to<>{}(first, second));
  EXPECT_TRUE(std::equal_to<>{}(second, first));
  EXPECT_FALSE(std::not_equal_to<>{}(first, second));
  EXPECT_FALSE(std::not_equal_to<>{}(second, first));
  EXPECT_FALSE(std::greater<>{}(first, second));
  EXPECT_FALSE(std::greater<>{}(second, first));
  EXPECT_TRUE(std::greater_equal<>{}(first, second));
  EXPECT_TRUE(std::greater_equal<>{}(second, first));
  EXPECT_FALSE(std::less<>{}(first, second));
  EXPECT_FALSE(std::less<>{}(second, first));
  EXPECT_TRUE(std::less_equal<>{}(first, second));
  EXPECT_TRUE(std::less_equal<>{}(second, first));
}

// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
// NOLINTEND(cppcoreguidelines-owning-memory)
// NOLINTEND(hicpp-avoid-c-arrays)
// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
