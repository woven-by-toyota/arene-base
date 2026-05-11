// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/functional"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

// NOLINTBEGIN(modernize-use-transparent-functors)

template <class T>
struct has_greater {
  constexpr explicit has_greater(T value)
      : value_(value) {}
  constexpr auto operator>(has_greater const& rhs) const -> bool { return value_ > rhs.value_; }

 private:
  T value_;
};

template <class T>
class GreaterTest : public testing::Test {};

TYPED_TEST_SUITE(GreaterTest, ::testing::integral_types, );

/// @test @c std::greater has proper member types
CONSTEXPR_TYPED_TEST(GreaterTest, GreaterMemberTypes) {
  using greater_type = std::greater<TypeParam>;
  testing::StaticAssertTypeEq<typename greater_type::result_type, bool>();
  testing::StaticAssertTypeEq<typename greater_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename greater_type::second_argument_type, TypeParam>();
}

/// @test @c std::greater with values in an overloaded operator
CONSTEXPR_TYPED_TEST(GreaterTest, GreaterValuesOverloaded) {
  auto const first = has_greater<TypeParam>{0};
  auto const second = has_greater<TypeParam>{1};
  auto const greater_obj = std::greater<has_greater<TypeParam>>{};

  EXPECT_FALSE(greater_obj(first, second));
  EXPECT_TRUE(greater_obj(second, first));
}

/// @test @c std::greater returns false for equal values in an overloaded operator
CONSTEXPR_TYPED_TEST(GreaterTest, EqualValuesOverloaded) {
  auto const first = has_greater<TypeParam>{1};
  auto const second = has_greater<TypeParam>{1};
  auto const greater_obj = std::greater<has_greater<TypeParam>>{};

  EXPECT_FALSE(greater_obj(first, second));
  EXPECT_FALSE(greater_obj(second, first));
}

/// @test @c std::greater is not noexcept when operator not defined as such
CONSTEXPR_TYPED_TEST(GreaterTest, GreaterNoexceptValuesOverloaded) {
  auto const first = has_greater<TypeParam>{0};
  auto const second = has_greater<TypeParam>{1};
  auto const greater_obj = std::greater<has_greater<TypeParam>>{};

  EXPECT_FALSE(noexcept(greater_obj(first, second)));
}

/// @test @c std::greater is not noexcept when operator not defined as such with type deduction
CONSTEXPR_TYPED_TEST(GreaterTest, GreaterNoexceptValuesOverloadedTypeDeduction) {
  auto const first = has_greater<TypeParam>{0};
  auto const second = has_greater<TypeParam>{1};
  auto const greater_obj = std::greater<>{};

  EXPECT_FALSE(noexcept(greater_obj(first, second)));
}

/// @test @c std::greater cannot be invoked when no @c operator> is defined
TEST(UntypedGreater, GreaterNotInvocableDifferentTypes) {
  struct A {};
  struct B {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::greater<>, A, B>);
}

struct not_int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_int_convertable(int value)
      : value_(value) {}
  constexpr auto operator>(not_int_convertable const& rhs) const -> bool { return value_ > rhs.value_; }

 private:
  int value_;
};

struct int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr int_convertable(int value)
      : value_(value) {}
  constexpr auto operator>(int_convertable const& rhs) const -> bool { return value_ > rhs.value_; }
  constexpr auto operator>(int const& rhs) const -> int { return static_cast<int>(value_ > rhs); }

 private:
  int value_;
};

/// @test @c std::greater does convert when necessary
TEST(UntypedGreater, GreaterNotConvert) {
  auto const first = not_int_convertable{1};
  auto const second = int{0};
  auto const greater_obj = std::greater<>{};

  EXPECT_TRUE(greater_obj(first, second));
  testing::StaticAssertTypeEq<decltype(greater_obj(first, second)), bool>();
}

/// @test @c std::greater does not convert when not necessary
TEST(UntypedGreater, GreaterDoesNotConvert) {
  auto const first = int_convertable{1};
  auto const second = int{0};
  auto const greater_obj = std::greater<>{};

  EXPECT_TRUE(greater_obj(first, second));
  testing::StaticAssertTypeEq<decltype(greater_obj(first, second)), int>();
}

// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
