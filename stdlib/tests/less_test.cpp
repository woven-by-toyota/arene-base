// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/functional"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

// NOLINTBEGIN(modernize-use-transparent-functors)

template <class T>
struct has_less {
  constexpr explicit has_less(T value)
      : value_(value) {}
  constexpr auto operator<(has_less const& rhs) const -> bool { return value_ < rhs.value_; }

 private:
  T value_;
};

template <class T>
class LessTest : public testing::Test {};

TYPED_TEST_SUITE(LessTest, ::testing::integral_types, );

/// @test @c std::less has proper member types
CONSTEXPR_TYPED_TEST(LessTest, LessMemberTypes) {
  using less_type = std::less<TypeParam>;
  testing::StaticAssertTypeEq<typename less_type::result_type, bool>();
  testing::StaticAssertTypeEq<typename less_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename less_type::second_argument_type, TypeParam>();
}

/// @test @c std::less with values in an overloaded operator
CONSTEXPR_TYPED_TEST(LessTest, LessValuesOverloaded) {
  auto const first = has_less<TypeParam>{0};
  auto const second = has_less<TypeParam>{1};
  auto const less_obj = std::less<has_less<TypeParam>>{};

  EXPECT_TRUE(less_obj(first, second));
  EXPECT_FALSE(less_obj(second, first));
}

/// @test @c std::less returns false for equal values in an overloaded operator
CONSTEXPR_TYPED_TEST(LessTest, EqualValuesOverloaded) {
  auto const first = has_less<TypeParam>{1};
  auto const second = has_less<TypeParam>{1};
  auto const less_obj = std::less<has_less<TypeParam>>{};

  EXPECT_FALSE(less_obj(first, second));
  EXPECT_FALSE(less_obj(second, first));
}

/// @test @c std::less is not noexcept when operator not defined as such
CONSTEXPR_TYPED_TEST(LessTest, LessNoexceptValuesOverloaded) {
  auto const first = has_less<TypeParam>{0};
  auto const second = has_less<TypeParam>{1};
  auto const less_obj = std::less<has_less<TypeParam>>{};

  EXPECT_FALSE(noexcept(less_obj(first, second)));
}

/// @test @c std::less is not noexcept when operator not defined as such with type deduction
CONSTEXPR_TYPED_TEST(LessTest, LessNoexceptValuesOverloadedTypeDeduction) {
  auto const first = has_less<TypeParam>{0};
  auto const second = has_less<TypeParam>{1};
  auto const less_obj = std::less<>{};

  EXPECT_FALSE(noexcept(less_obj(first, second)));
}

/// @test @c std::less cannot be invoked when no @c operator< is defined
TEST(UntypedLess, LessNotInvocableDifferentTypes) {
  struct A {};
  struct B {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::less<>, A, B>);
}

struct not_int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_int_convertable(int value)
      : value_(value) {}
  constexpr auto operator<(not_int_convertable const& rhs) const -> bool { return value_ < rhs.value_; }

 private:
  int value_;
};

struct int_convertable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr int_convertable(int value)
      : value_(value) {}
  constexpr auto operator<(int_convertable const& rhs) const -> bool { return value_ < rhs.value_; }
  constexpr auto operator<(int const& rhs) const -> int { return static_cast<int>(value_ < rhs); }

 private:
  int value_;
};

/// @test @c std::less does convert when necessary
TEST(UntypedLess, LessNotConvert) {
  auto const first = not_int_convertable{0};
  auto const second = int{1};
  auto const less_obj = std::less<>{};

  EXPECT_TRUE(less_obj(first, second));
  testing::StaticAssertTypeEq<decltype(less_obj(first, second)), bool>();
}

/// @test @c std::less does not convert when not necessary
TEST(UntypedLess, LessDoesNotConvert) {
  auto const first = int_convertable{0};
  auto const second = int{1};
  auto const less_obj = std::less<>{};

  EXPECT_TRUE(less_obj(first, second));
  testing::StaticAssertTypeEq<decltype(less_obj(first, second)), int>();
}

// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
// NOLINTEND(cppcoreguidelines-owning-memory)
// NOLINTEND(hicpp-avoid-c-arrays)
// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
