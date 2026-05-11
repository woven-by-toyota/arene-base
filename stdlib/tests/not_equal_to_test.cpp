// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/functional"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

template <class T>
struct has_not_equal {
  constexpr explicit has_not_equal(T value)
      : value_(value) {}
  constexpr auto operator!=(has_not_equal const& rhs) const -> bool { return value_ != rhs.value_; }

 private:
  T value_;
};

template <class T>
class NotEqualToTest : public testing::Test {};

TYPED_TEST_SUITE(NotEqualToTest, ::testing::integral_types, );

/// @test @c std::not_equal_to has proper member types
CONSTEXPR_TYPED_TEST(NotEqualToTest, NotEqualMemberTypes) {
  using not_equal_type = std::not_equal_to<TypeParam>;
  testing::StaticAssertTypeEq<typename not_equal_type::result_type, bool>();
  testing::StaticAssertTypeEq<typename not_equal_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename not_equal_type::second_argument_type, TypeParam>();
}

/// @test @c std::not_equal_to returns true for not equal values in an overloaded operator
CONSTEXPR_TYPED_TEST(NotEqualToTest, NotEqualValuesOverloaded) {
  auto const first = has_not_equal<TypeParam>{0};
  auto const second = has_not_equal<TypeParam>{1};
  auto const neq_obj = std::not_equal_to<has_not_equal<TypeParam>>{};

  EXPECT_TRUE(neq_obj(first, second));
  EXPECT_TRUE(neq_obj(second, first));
}

/// @test @c std::not_equal_to returns false for equal values in an overloaded operator
CONSTEXPR_TYPED_TEST(NotEqualToTest, EqualValuesOverloaded) {
  auto const first = has_not_equal<TypeParam>{1};
  auto const second = has_not_equal<TypeParam>{1};
  auto const neq_obj = std::not_equal_to<has_not_equal<TypeParam>>{};

  EXPECT_FALSE(neq_obj(first, second));
  EXPECT_FALSE(neq_obj(second, first));
}

/// @test @c std::not_equal_to is not noexcept when operator not defined as such
CONSTEXPR_TYPED_TEST(NotEqualToTest, EqualNoexceptValuesOverloaded) {
  auto const first = has_not_equal<TypeParam>{0};
  auto const second = has_not_equal<TypeParam>{1};
  auto const neq_obj = std::not_equal_to<has_not_equal<TypeParam>>{};

  EXPECT_FALSE(noexcept(neq_obj(first, second)));
}

/// @test @c std::not_equal_to is not noexcept when operator not defined as such with type deduction
CONSTEXPR_TYPED_TEST(NotEqualToTest, NotEqualNoexceptValuesOverloadedTypeDeduction) {
  auto const first = has_not_equal<TypeParam>{0};
  auto const second = has_not_equal<TypeParam>{1};
  auto const neq_obj = std::not_equal_to<>{};

  EXPECT_FALSE(noexcept(neq_obj(first, second)));
}

/// @test @c std::not_equal_to cannot be invoked when no @c operator> is defined
TEST(UntypedNotEqual, NotEqualNotInvocableDifferentTypes) {
  struct A {};
  struct B {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::not_equal_to<>, A, B>);
}

struct not_int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_int_convertable(int value)
      : value_(value) {}
  constexpr auto operator!=(not_int_convertable const& rhs) const -> bool { return value_ != rhs.value_; }

 private:
  int value_;
};

struct int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr int_convertable(int value)
      : value_(value) {}
  constexpr auto operator!=(int_convertable const& rhs) const -> bool { return value_ != rhs.value_; }
  constexpr auto operator!=(int const& rhs) const -> int { return static_cast<int>(value_ != rhs); }

 private:
  int value_;
};

/// @test @c std::not_equal_to does convert when necessary
TEST(UntypedNotEqual, NotEqualNotConvert) {
  auto const first = not_int_convertable{0};
  auto const second = int{1};
  auto const neq_obj = std::not_equal_to<>{};

  EXPECT_TRUE(neq_obj(first, second));
  testing::StaticAssertTypeEq<decltype(neq_obj(first, second)), bool>();
}

/// @test @c std::not_equal_to does not convert when not necessary
TEST(UntypedNotEqual, NotEqualDoesNotConvert) {
  auto const first = int_convertable{0};
  auto const second = int{1};
  auto const neq_obj = std::not_equal_to<>{};

  EXPECT_TRUE(neq_obj(first, second));
  testing::StaticAssertTypeEq<decltype(neq_obj(first, second)), int>();
}

}  // namespace
