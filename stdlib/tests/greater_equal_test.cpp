// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/functional"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

// NOLINTBEGIN(modernize-use-transparent-functors)
template <class T>
struct has_greater_equal {
  constexpr explicit has_greater_equal(T value)
      : value_(value) {}
  constexpr auto operator>=(has_greater_equal const& rhs) const -> bool { return value_ >= rhs.value_; }

 private:
  T value_;
};

template <class T>
class GreaterEqualTest : public testing::Test {};

TYPED_TEST_SUITE(GreaterEqualTest, ::testing::integral_types, );

/// @test @c std::greater_equal has proper member types
CONSTEXPR_TYPED_TEST(GreaterEqualTest, GreaterEqualMemberTypes) {
  using greater_equal_type = std::greater_equal<TypeParam>;
  testing::StaticAssertTypeEq<typename greater_equal_type::result_type, bool>();
  testing::StaticAssertTypeEq<typename greater_equal_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename greater_equal_type::second_argument_type, TypeParam>();
}

/// @test @c std::greater_equal with values in an overloaded operator
CONSTEXPR_TYPED_TEST(GreaterEqualTest, GreaterValuesOverloaded) {
  auto const first = has_greater_equal<TypeParam>{0};
  auto const second = has_greater_equal<TypeParam>{1};
  auto const greater_equal_obj = std::greater_equal<has_greater_equal<TypeParam>>{};

  EXPECT_FALSE(greater_equal_obj(first, second));
  EXPECT_TRUE(greater_equal_obj(second, first));
}

/// @test @c std::greater_equal returns true for equal values in an overloaded operator
CONSTEXPR_TYPED_TEST(GreaterEqualTest, EqualValuesOverloaded) {
  auto const first = has_greater_equal<TypeParam>{1};
  auto const second = has_greater_equal<TypeParam>{1};
  auto const greater_equal_obj = std::greater_equal<has_greater_equal<TypeParam>>{};

  EXPECT_TRUE(greater_equal_obj(first, second));
  EXPECT_TRUE(greater_equal_obj(second, first));
}

/// @test @c std::greater_equal is not noexcept when operator not defined as such
CONSTEXPR_TYPED_TEST(GreaterEqualTest, GreaterEqualNoexceptValuesOverloaded) {
  auto const first = has_greater_equal<TypeParam>{0};
  auto const second = has_greater_equal<TypeParam>{1};
  auto const greater_equal_obj = std::greater_equal<has_greater_equal<TypeParam>>{};

  EXPECT_FALSE(noexcept(greater_equal_obj(first, second)));
}

/// @test @c std::greater_equal is not noexcept when operator not defined as such with type deduction
CONSTEXPR_TYPED_TEST(GreaterEqualTest, GreaterEqualNoexceptValuesOverloadedTypeDeduction) {
  auto const first = has_greater_equal<TypeParam>{0};
  auto const second = has_greater_equal<TypeParam>{1};
  auto const greater_equal_obj = std::greater_equal<>{};

  EXPECT_FALSE(noexcept(greater_equal_obj(first, second)));
}

/// @test @c std::greater_equal cannot be invoked when no @c operator>= is defined
TEST(UntypedGreater, GreaterEqualNotInvocableDifferentTypes) {
  struct A {};
  struct B {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::greater_equal<>, A, B>);
}

struct not_int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_int_convertable(int value)
      : value_(value) {}
  constexpr auto operator>=(not_int_convertable const& rhs) const -> bool { return value_ >= rhs.value_; }

 private:
  int value_;
};

struct int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr int_convertable(int value)
      : value_(value) {}
  constexpr auto operator>=(int_convertable const& rhs) const -> bool { return value_ >= rhs.value_; }
  constexpr auto operator>=(int const& rhs) const -> int { return static_cast<int>(value_ >= rhs); }

 private:
  int value_;
};

/// @test @c std::greater_equal does convert when necessary
TEST(UntypedGreater, GreaterEqualNotConvert) {
  auto const first = not_int_convertable{1};
  auto const second = int{0};
  auto const greater_equal_obj = std::greater_equal<>{};

  EXPECT_TRUE(greater_equal_obj(first, second));
  testing::StaticAssertTypeEq<decltype(greater_equal_obj(first, second)), bool>();
}

/// @test @c std::greater_equal does not convert when not necessary
TEST(UntypedGreater, GreaterEqualDoesNotConvert) {
  auto const first = int_convertable{1};
  auto const second = int{0};
  auto const greater_equal_obj = std::greater_equal<>{};

  EXPECT_TRUE(greater_equal_obj(first, second));
  testing::StaticAssertTypeEq<decltype(greater_equal_obj(first, second)), int>();
}

// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
// NOLINTEND(cppcoreguidelines-owning-memory)
// NOLINTEND(hicpp-avoid-c-arrays)
// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
