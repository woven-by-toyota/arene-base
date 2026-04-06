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
struct has_multiplies {
  constexpr explicit has_multiplies(T value)
      : value_(value) {}
  constexpr auto operator*(has_multiplies const&) const noexcept(NoExcept) -> has_multiplies<T, NoExcept> {
    return has_multiplies(operator_return_value);
  }
  constexpr auto operator==(has_multiplies const& rhs) const -> bool {
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
class MultipliesTest : public testing::Test {};

using multiplies_types = arene::base::type_lists::concat_unique_t<
    ::testing::unsigned_integer_types,
    ::testing::signed_integer_types,
    ::testing::floating_point_types>;
TYPED_TEST_SUITE(MultipliesTest, multiplies_types, );

template <class T, class U>
constexpr auto check_multiplies(T first, T second, U expected) -> bool {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL(
      "-Wfloat-equal",
      "Performing identitical operations on floating point values, so exact equality is ok."
  );
  auto result = std::multiplies<T>{}(first, second) == expected && std::multiplies<T>{}(second, first) == expected &&
                std::multiplies<>{}(first, second) == expected && std::multiplies<>{}(second, first) == expected;
  ARENE_IGNORE_END();

  return result;
}

template <class T>
constexpr auto check_multiplies(T first, T second) -> bool {
  auto const expected = first * second;
  return check_multiplies(first, second, expected);
}

template <class T>
constexpr auto check_multiplies_noexcept_specification() -> bool {
  auto const should_be_noexcept = noexcept(std::declval<T&>() * std::declval<T&>());

  return noexcept(std::multiplies<T>{}(std::declval<T&>(), std::declval<T&>())) ==
         should_be_noexcept&& noexcept(std::multiplies<>{}(std::declval<T&>(), std::declval<T&>())) ==
         should_be_noexcept;
}

/// @test @c std::multiplies has proper member types
CONSTEXPR_TYPED_TEST(MultipliesTest, MemberTypes) {
  using multiplies_type = std::multiplies<TypeParam>;
  testing::StaticAssertTypeEq<typename multiplies_type::result_type, TypeParam>();
  testing::StaticAssertTypeEq<typename multiplies_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename multiplies_type::second_argument_type, TypeParam>();
}

/// @test @c std::multiplies has proper nothrow properties
CONSTEXPR_TYPED_TEST(MultipliesTest, Properties) {
  using multiplies_type = std::multiplies<TypeParam>;

  EXPECT_TRUE(std::is_nothrow_move_constructible_v<multiplies_type>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<multiplies_type>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<multiplies_type>);
}

/// @test @c std::multiplies with built in types
CONSTEXPR_TYPED_TEST(MultipliesTest, WithBuiltInTypes) {
  auto const first = TypeParam{1};
  auto const second = TypeParam{2};

  EXPECT_TRUE(check_multiplies(first, second));
}

/// @test @c std::multiplies with values in an overloaded operator
CONSTEXPR_TYPED_TEST(MultipliesTest, WithValuesOverloaded) {
  using T = has_multiplies<TypeParam>;
  auto const first = T{0};
  auto const second = T{1};

  EXPECT_TRUE(check_multiplies(first, second, T(T::operator_return_value)));
}

/// @test @c std::multiplies has correct noexcept specification for integral types
CONSTEXPR_TYPED_TEST(MultipliesTest, NoexceptIntegralTypes) {
  EXPECT_TRUE(check_multiplies_noexcept_specification<TypeParam>());
}

/// @test @c std::multiplies has correct noexcept specification for custom types
CONSTEXPR_TYPED_TEST(MultipliesTest, NoexceptValuesOverloaded) {
  EXPECT_TRUE(check_multiplies_noexcept_specification<has_multiplies<TypeParam, true>>());
  EXPECT_TRUE(check_multiplies_noexcept_specification<has_multiplies<TypeParam, false>>());
}

/// @test @c std::multiplies cannot be invoked when no @c operator* is defined
TEST(MultipliesTest, NotInvocableWithDifferentTypes) {
  struct no_operator_multiplies {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::multiplies<>, no_operator_multiplies, no_operator_multiplies>);
}

struct not_int_multipliable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_int_multipliable(int value)
      : value_(value) {}
  constexpr auto operator*(not_int_multipliable const& rhs) const -> not_int_multipliable {
    return not_int_multipliable{value_ * rhs.value_};
  }
  constexpr auto value() const -> int { return value_; }

 private:
  int value_;
};

struct int_multipliable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr int_multipliable(int value)
      : value_(value) {}
  constexpr auto operator*(int_multipliable const& rhs) const -> int_multipliable {
    return int_multipliable{value_ * rhs.value_};
  }
  constexpr auto operator*(int const& rhs) const -> int { return value_ * rhs; }

 private:
  int value_;
};

/// @test @c std::multiplies does convert input when necessary
TEST(MultipliesTest, DoesConvertInput) {
  auto const first = not_int_multipliable{1};
  auto const second = int{2};

  testing::StaticAssertTypeEq<decltype(std::multiplies<>{}(first, second)), not_int_multipliable>();
  EXPECT_EQ(std::multiplies<>{}(first, second).value(), 2);
}

/// @test @c std::multiplies does not convert input when not necessary
TEST(MultipliesTest, MultipliesDoesNotConvert) {
  auto const first = int_multipliable{1};
  auto const second = int{2};

  testing::StaticAssertTypeEq<decltype(std::multiplies<>{}(first, second)), int>();
  EXPECT_EQ(std::multiplies<>{}(first, second), 2);
}

// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
