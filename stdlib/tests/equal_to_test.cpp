// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/functional"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

template <class T>
struct has_equal {
  constexpr explicit has_equal(T value)
      : value_(value) {}
  constexpr auto operator==(has_equal const& rhs) const -> bool { return value_ == rhs.value_; }

 private:
  T value_;
};

template <class T>
class EqualToTest : public testing::Test {};

TYPED_TEST_SUITE(EqualToTest, ::testing::integral_types, );

/// @test @c std::equal_to has proper member types
CONSTEXPR_TYPED_TEST(EqualToTest, EqualMemberTypes) {
  using equal_type = std::equal_to<TypeParam>;
  testing::StaticAssertTypeEq<typename equal_type::result_type, bool>();
  testing::StaticAssertTypeEq<typename equal_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename equal_type::second_argument_type, TypeParam>();
}

/// @test @c std::equal_to returns true for equal values in an overloaded operator
CONSTEXPR_TYPED_TEST(EqualToTest, EqualValuesOverloaded) {
  auto const first = has_equal<TypeParam>{1};
  auto const second = has_equal<TypeParam>{1};
  auto const eq_obj = std::equal_to<has_equal<TypeParam>>{};

  EXPECT_TRUE(eq_obj(first, second));
  EXPECT_TRUE(eq_obj(second, first));
}

/// @test @c std::equal_to returns false for not equal values in an overloaded operator
CONSTEXPR_TYPED_TEST(EqualToTest, NotEqualValuesOverloaded) {
  auto const first = has_equal<TypeParam>{0};
  auto const second = has_equal<TypeParam>{1};
  auto const eq_obj = std::equal_to<has_equal<TypeParam>>{};

  EXPECT_FALSE(eq_obj(first, second));
  EXPECT_FALSE(eq_obj(second, first));
}

/// @test @c std::equal_to is not noexcept when operator not defined as such
CONSTEXPR_TYPED_TEST(EqualToTest, EqualNoexceptValuesOverloaded) {
  auto const first = has_equal<TypeParam>{0};
  auto const second = has_equal<TypeParam>{1};
  auto const eq_obj = std::equal_to<has_equal<TypeParam>>{};

  EXPECT_FALSE(noexcept(eq_obj(first, second)));
}

/// @test @c std::equal_to is not noexcept when operator not defined as such with type deduction
CONSTEXPR_TYPED_TEST(EqualToTest, EqualNoexceptValuesOverloadedTypeDeduction) {
  auto const first = has_equal<TypeParam>{0};
  auto const second = has_equal<TypeParam>{1};
  auto const eq_obj = std::equal_to<>{};

  EXPECT_FALSE(noexcept(eq_obj(first, second)));
}

/// @test @c std::equal_to cannot be invoked when no @c operator> is defined
TEST(UntypedEqual, EqualNotInvocableDifferentTypes) {
  struct A {};
  struct B {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::equal_to<>, A, B>);
}

struct not_int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_int_convertable(int value)
      : value_(value) {}
  constexpr auto operator==(not_int_convertable const& rhs) const -> bool { return value_ == rhs.value_; }

 private:
  int value_;
};

struct int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr int_convertable(int value)
      : value_(value) {}
  constexpr auto operator==(int_convertable const& rhs) const -> bool { return value_ == rhs.value_; }
  constexpr auto operator==(int const& rhs) const -> int { return static_cast<int>(value_ == rhs); }

 private:
  int value_;
};

/// @test @c std::equal_to does convert when necessary
TEST(UntypedEqual, EqualNotConvert) {
  auto const first = not_int_convertable{1};
  auto const second = int{1};
  auto const eq_obj = std::equal_to<>{};

  EXPECT_TRUE(eq_obj(first, second));
  testing::StaticAssertTypeEq<decltype(eq_obj(first, second)), bool>();
}

/// @test @c std::equal_to does not convert when not necessary
TEST(UntypedEqual, EqualDoesNotConvert) {
  auto const first = int_convertable{1};
  auto const second = int{1};
  auto const eq_obj = std::equal_to<>{};

  EXPECT_TRUE(eq_obj(first, second));
  testing::StaticAssertTypeEq<decltype(eq_obj(first, second)), int>();
}

}  // namespace
