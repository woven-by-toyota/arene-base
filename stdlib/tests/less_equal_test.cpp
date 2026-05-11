// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/functional"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

// NOLINTBEGIN(modernize-use-transparent-functors)

template <class T>
struct has_less_equal {
  constexpr explicit has_less_equal(T value)
      : value_(value) {}
  constexpr auto operator<=(has_less_equal const& rhs) const -> bool { return value_ <= rhs.value_; }

 private:
  T value_;
};

template <class T>
class LessEqualTest : public testing::Test {};

TYPED_TEST_SUITE(LessEqualTest, ::testing::integral_types, );

/// @test @c std::less_equal has proper member types
CONSTEXPR_TYPED_TEST(LessEqualTest, LessEqualMemberTypes) {
  using less_equal_type = std::less_equal<TypeParam>;
  testing::StaticAssertTypeEq<typename less_equal_type::result_type, bool>();
  testing::StaticAssertTypeEq<typename less_equal_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename less_equal_type::second_argument_type, TypeParam>();
}

/// @test @c std::less_equal with values in an overloaded operator
CONSTEXPR_TYPED_TEST(LessEqualTest, LessValuesOverloaded) {
  auto const first = has_less_equal<TypeParam>{0};
  auto const second = has_less_equal<TypeParam>{1};
  auto const less_equal_obj = std::less_equal<has_less_equal<TypeParam>>{};

  EXPECT_TRUE(less_equal_obj(first, second));
  EXPECT_FALSE(less_equal_obj(second, first));
}

/// @test @c std::less_equal returns true for equal values in an overloaded operator
CONSTEXPR_TYPED_TEST(LessEqualTest, EqualValuesOverloaded) {
  auto const first = has_less_equal<TypeParam>{1};
  auto const second = has_less_equal<TypeParam>{1};
  auto const less_equal_obj = std::less_equal<has_less_equal<TypeParam>>{};

  EXPECT_TRUE(less_equal_obj(first, second));
  EXPECT_TRUE(less_equal_obj(second, first));
}

/// @test @c std::less_equal is not noexcept when operator not defined as such
CONSTEXPR_TYPED_TEST(LessEqualTest, LessEqualNoexceptValuesOverloaded) {
  auto const first = has_less_equal<TypeParam>{0};
  auto const second = has_less_equal<TypeParam>{1};
  auto const less_equal_obj = std::less_equal<has_less_equal<TypeParam>>{};

  EXPECT_FALSE(noexcept(less_equal_obj(first, second)));
}

/// @test @c std::less_equal is not noexcept when operator not defined as such with type deduction
CONSTEXPR_TYPED_TEST(LessEqualTest, LessEqualNoexceptValuesOverloadedTypeDeduction) {
  auto const first = has_less_equal<TypeParam>{0};
  auto const second = has_less_equal<TypeParam>{1};
  auto const less_equal_obj = std::less_equal<>{};

  EXPECT_FALSE(noexcept(less_equal_obj(first, second)));
}

/// @test @c std::less_equal cannot be invoked when no @c operator<= is defined
TEST(UntypedLess, LessEqualNotInvocableDifferentTypes) {
  struct A {};
  struct B {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::less_equal<>, A, B>);
}

struct not_int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_int_convertable(int value)
      : value_(value) {}
  constexpr auto operator<=(not_int_convertable const& rhs) const -> bool { return value_ <= rhs.value_; }

 private:
  int value_;
};

struct int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr int_convertable(int value)
      : value_(value) {}
  constexpr auto operator<=(int_convertable const& rhs) const -> bool { return value_ <= rhs.value_; }
  constexpr auto operator<=(int const& rhs) const -> int { return static_cast<int>(value_ <= rhs); }

 private:
  int value_;
};

/// @test @c std::less_equal does convert when necessary
TEST(UntypedLess, LessEqualNotConvert) {
  auto const first = not_int_convertable{0};
  auto const second = int{1};
  auto const less_equal_obj = std::less_equal<>{};

  EXPECT_TRUE(less_equal_obj(first, second));
  testing::StaticAssertTypeEq<decltype(less_equal_obj(first, second)), bool>();
}

/// @test @c std::less_equal does not convert when not necessary
TEST(UntypedLess, LessEqualDoesNotConvert) {
  auto const first = int_convertable{0};
  auto const second = int{1};
  auto const less_equal_obj = std::less_equal<>{};

  EXPECT_TRUE(less_equal_obj(first, second));
  testing::StaticAssertTypeEq<decltype(less_equal_obj(first, second)), int>();
}

// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
