// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
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
struct has_divides {
  constexpr explicit has_divides(T value)
      : value_(value) {}
  constexpr auto operator/(has_divides const&) const noexcept(NoExcept) -> has_divides<T, NoExcept> {
    return has_divides(operator_return_value);
  }
  constexpr auto operator==(has_divides const& rhs) const -> bool {
    ARENE_IGNORE_START();
    ARENE_IGNORE_ALL(
        "-Wfloat-equal",
        "Performing identitical operations on floating point values, so exact equality is ok."
    );
    auto result = value_ == rhs.value_;
    ARENE_IGNORE_END();

    return result;
  }

  static constexpr auto operator_return_value = T{99};

 private:
  T value_;
};

template <class T>
class DividesTest : public testing::Test {};

using divides_types = arene::base::type_lists::concat_unique_t<
    ::testing::unsigned_integer_types,
    ::testing::signed_integer_types,
    ::testing::floating_point_types>;
TYPED_TEST_SUITE(DividesTest, divides_types, );

template <class T, class U>
constexpr auto check_divides(T first, T second, U expected) -> bool {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL(
      "-Wfloat-equal",
      "Performing identical operations on floating point values, so exact equality is ok."
  );
  auto result = std::divides<T>{}(first, second) == expected && std::divides<>{}(first, second) == expected;
  ARENE_IGNORE_END();

  return result;
}

template <class T>
constexpr auto check_divides(T first, T second) -> bool {
  auto const expected = first / second;
  return check_divides(first, second, expected);
}

template <class T>
constexpr auto check_divides_noexcept_specification() -> bool {
  auto const should_be_noexcept = noexcept(std::declval<T&>() / std::declval<T&>());

  return noexcept(std::divides<T>{}(std::declval<T&>(), std::declval<T&>())) ==
         should_be_noexcept&& noexcept(std::divides<>{}(std::declval<T&>(), std::declval<T&>())) == should_be_noexcept;
}

/// @test @c std::divides has proper member types
CONSTEXPR_TYPED_TEST(DividesTest, MemberTypes) {
  using divides_type = std::divides<TypeParam>;
  testing::StaticAssertTypeEq<typename divides_type::result_type, TypeParam>();
  testing::StaticAssertTypeEq<typename divides_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename divides_type::second_argument_type, TypeParam>();
}

/// @test @c std::divides has proper nothrow properties
CONSTEXPR_TYPED_TEST(DividesTest, Properties) {
  using divides_type = std::divides<TypeParam>;

  EXPECT_TRUE(std::is_nothrow_move_constructible_v<divides_type>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<divides_type>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<divides_type>);
}

/// @test @c std::divides with built in types
CONSTEXPR_TYPED_TEST(DividesTest, WithBuiltInTypes) {
  auto const first = TypeParam{2};
  auto const second = TypeParam{1};

  EXPECT_TRUE(check_divides(first, second));
}

/// @test @c std::divides with values in an overloaded operator
CONSTEXPR_TYPED_TEST(DividesTest, WithValuesOverloaded) {
  using T = has_divides<TypeParam>;
  auto const first = T{2};
  auto const second = T{1};

  EXPECT_TRUE(check_divides(first, second, T(T::operator_return_value)));
}

/// @test @c std::divides has correct noexcept specification for integral types
CONSTEXPR_TYPED_TEST(DividesTest, NoexceptIntegralTypes) {
  EXPECT_TRUE(check_divides_noexcept_specification<TypeParam>());
}

/// @test @c std::divides has correct noexcept specification for custom types
CONSTEXPR_TYPED_TEST(DividesTest, NoexceptValuesOverloaded) {
  EXPECT_TRUE(check_divides_noexcept_specification<has_divides<TypeParam, true>>());
  EXPECT_TRUE(check_divides_noexcept_specification<has_divides<TypeParam, false>>());
}

/// @test @c std::divides cannot be invoked when no @c operator/ is defined
TEST(DividesTest, NotInvocableWithDifferentTypes) {
  struct no_operator_divides {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::divides<>, no_operator_divides, no_operator_divides>);
}

struct not_int_dividable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_int_dividable(int value)
      : value_(value) {}
  constexpr auto operator/(not_int_dividable const& rhs) const -> not_int_dividable {
    return not_int_dividable{value_ / rhs.value_};
  }
  constexpr auto value() const -> int { return value_; }

 private:
  int value_;
};

struct int_dividable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr int_dividable(int value)
      : value_(value) {}
  constexpr auto operator/(int_dividable const& rhs) const -> int_dividable {
    return int_dividable{value_ / rhs.value_};
  }
  constexpr auto operator/(int const& rhs) const -> int { return value_ / rhs; }

 private:
  int value_;
};

/// @test @c std::divides does convert input when necessary
TEST(DividesTest, DoesConvertInput) {
  auto const first = not_int_dividable{4};
  auto const second = int{2};

  testing::StaticAssertTypeEq<decltype(std::divides<>{}(first, second)), not_int_dividable>();
  EXPECT_EQ(std::divides<>{}(first, second).value(), 2);
}

/// @test @c std::divides does not convert input when not necessary
TEST(DividesTest, DividesDoesNotConvert) {
  auto const first = int_dividable{4};
  auto const second = int{2};

  testing::StaticAssertTypeEq<decltype(std::divides<>{}(first, second)), int>();
  EXPECT_EQ(std::divides<>{}(first, second), 2);
}

// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
