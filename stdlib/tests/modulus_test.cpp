// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/functional"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

// Need to test both versions.
// NOLINTBEGIN(modernize-use-transparent-functors)

template <class T, bool NoExcept = true>
struct has_modulus {
  constexpr explicit has_modulus(T value)
      : value_(value) {}
  constexpr auto operator%(has_modulus const&) const noexcept(NoExcept) -> has_modulus<T, NoExcept> {
    return has_modulus(operator_return_value);
  }
  constexpr auto operator==(has_modulus const& rhs) const -> bool { return value_ == rhs.value_; }

  static constexpr auto operator_return_value = T{99};

 private:
  T value_;
};

template <class T>
class ModulusTest : public testing::Test {};

using modulus_types =
    arene::base::type_lists::concat_unique_t<::testing::unsigned_integer_types, ::testing::signed_integer_types>;
TYPED_TEST_SUITE(ModulusTest, modulus_types, );

template <class T, class U>
constexpr auto check_modulus(T first, T second, U expected) -> bool {
  return std::modulus<T>{}(first, second) == expected && std::modulus<>{}(first, second) == expected;
}

template <class T>
constexpr auto check_modulus(T first, T second) -> bool {
  auto const expected = first % second;
  return check_modulus(first, second, expected);
}

template <class T>
constexpr auto check_modulus_noexcept_specification() -> bool {
  auto const should_be_noexcept = noexcept(std::declval<T&>() % std::declval<T&>());

  return noexcept(std::modulus<T>{}(std::declval<T&>(), std::declval<T&>())) ==
         should_be_noexcept&& noexcept(std::modulus<>{}(std::declval<T&>(), std::declval<T&>())) == should_be_noexcept;
}

/// @test @c std::modulus has proper member types
CONSTEXPR_TYPED_TEST(ModulusTest, MemberTypes) {
  using modulus_type = std::modulus<TypeParam>;
  testing::StaticAssertTypeEq<typename modulus_type::result_type, TypeParam>();
  testing::StaticAssertTypeEq<typename modulus_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename modulus_type::second_argument_type, TypeParam>();
}

/// @test @c std::modulus has proper nothrow properties
CONSTEXPR_TYPED_TEST(ModulusTest, Properties) {
  using modulus_type = std::modulus<TypeParam>;

  EXPECT_TRUE(std::is_nothrow_move_constructible_v<modulus_type>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<modulus_type>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<modulus_type>);
}

/// @test @c std::modulus with built in types
CONSTEXPR_TYPED_TEST(ModulusTest, WithBuiltInTypes) {
  auto const first = TypeParam{3};
  auto const second = TypeParam{2};

  EXPECT_TRUE(check_modulus(first, second));
}

/// @test @c std::modulus with values in an overloaded operator
CONSTEXPR_TYPED_TEST(ModulusTest, WithValuesOverloaded) {
  using T = has_modulus<TypeParam>;
  auto const first = T{3};
  auto const second = T{2};

  EXPECT_TRUE(check_modulus(first, second, T(T::operator_return_value)));
}

/// @test @c std::modulus has correct noexcept specification for integral types
CONSTEXPR_TYPED_TEST(ModulusTest, NoexceptIntegralTypes) {
  EXPECT_TRUE(check_modulus_noexcept_specification<TypeParam>());
}

/// @test @c std::modulus has correct noexcept specification for custom types
CONSTEXPR_TYPED_TEST(ModulusTest, NoexceptValuesOverloaded) {
  EXPECT_TRUE(check_modulus_noexcept_specification<has_modulus<TypeParam, true>>());
  EXPECT_TRUE(check_modulus_noexcept_specification<has_modulus<TypeParam, false>>());
}

/// @test @c std::modulus cannot be invoked when no @c operator% is defined
TEST(ModulusTest, NotInvocableWithDifferentTypes) {
  struct no_operator_modulus {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::modulus<>, no_operator_modulus, no_operator_modulus>);
}

struct not_int_modable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_int_modable(int value)
      : value_(value) {}
  constexpr auto operator%(not_int_modable const& rhs) const -> not_int_modable {
    return not_int_modable{value_ % rhs.value_};
  }
  constexpr auto value() const -> int { return value_; }

 private:
  int value_;
};

struct int_modable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr int_modable(int value)
      : value_(value) {}
  constexpr auto operator%(int_modable const& rhs) const -> int_modable { return int_modable{value_ % rhs.value_}; }
  constexpr auto operator%(int const& rhs) const -> int { return value_ % rhs; }

 private:
  int value_;
};

/// @test @c std::modulus does convert input when necessary
TEST(ModulusTest, DoesConvertInput) {
  auto const first = not_int_modable{3};
  auto const second = int{2};

  testing::StaticAssertTypeEq<decltype(std::modulus<>{}(first, second)), not_int_modable>();
  EXPECT_EQ(std::modulus<>{}(first, second).value(), 1);
}

/// @test @c std::modulus does not convert input when not necessary
TEST(ModulusTest, ModulusDoesNotConvert) {
  auto const first = int_modable{3};
  auto const second = int{2};

  testing::StaticAssertTypeEq<decltype(std::modulus<>{}(first, second)), int>();
  EXPECT_EQ(std::modulus<>{}(first, second), 1);
}

// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
