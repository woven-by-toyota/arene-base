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
struct has_plus {
  constexpr explicit has_plus(T value)
      : value_(value) {}
  constexpr auto operator+(has_plus const&) const noexcept(NoExcept) -> has_plus<T, NoExcept> {
    return has_plus<T, NoExcept>(operator_return_value);
  }

  constexpr auto operator==(has_plus const& rhs) const -> bool {
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
class PlusTest : public testing::Test {};

using plus_types = arene::base::type_lists::concat_unique_t<
    ::testing::unsigned_integer_types,
    ::testing::signed_integer_types,
    ::testing::floating_point_types>;
TYPED_TEST_SUITE(PlusTest, plus_types, );

template <class T, class U>
constexpr auto check_plus(T first, T second, U expected) -> bool {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL(
      "-Wfloat-equal",
      "Performing identitical operations on floating point values, so exact equality is ok."
  );
  auto result = std::plus<T>{}(first, second) == expected && std::plus<T>{}(second, first) == expected &&
                std::plus<>{}(first, second) == expected && std::plus<>{}(second, first) == expected;
  ARENE_IGNORE_END();

  return result;
}

template <class T>
constexpr auto check_plus(T first, T second) -> bool {
  auto const expected = first + second;
  return check_plus(first, second, expected);
}

template <class T>
constexpr auto check_plus_noexcept_specification() -> bool {
  auto const should_be_noexcept = noexcept(std::declval<T&>() + std::declval<T&>());

  return noexcept(std::plus<T>{}(std::declval<T&>(), std::declval<T&>())) ==
         should_be_noexcept&& noexcept(std::plus<>{}(std::declval<T&>(), std::declval<T&>())) == should_be_noexcept;
}

/// @test @c std::plus has proper member types
CONSTEXPR_TYPED_TEST(PlusTest, MemberTypes) {
  using plus_type = std::plus<TypeParam>;
  testing::StaticAssertTypeEq<typename plus_type::result_type, TypeParam>();
  testing::StaticAssertTypeEq<typename plus_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename plus_type::second_argument_type, TypeParam>();
}

/// @test @c std::plus has proper nothrow properties
CONSTEXPR_TYPED_TEST(PlusTest, Properties) {
  using plus_type = std::plus<TypeParam>;

  EXPECT_TRUE(std::is_nothrow_move_constructible_v<plus_type>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<plus_type>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<plus_type>);
}

/// @test @c std::plus with built in types
CONSTEXPR_TYPED_TEST(PlusTest, WithBuiltInTypes) {
  auto const first = TypeParam{1};
  auto const second = TypeParam{2};

  EXPECT_TRUE(check_plus(first, second));
}

/// @test @c std::plus with values in an overloaded operator
CONSTEXPR_TYPED_TEST(PlusTest, WithValuesOverloaded) {
  using T = has_plus<TypeParam>;
  auto const value = T{2};

  EXPECT_TRUE(check_plus(value, T{T::operator_return_value}));
}

/// @test @c std::plus has correct noexcept specification for integral types
CONSTEXPR_TYPED_TEST(PlusTest, NoexceptIntegralTypes) { EXPECT_TRUE(check_plus_noexcept_specification<TypeParam>()); }

/// @test @c std::plus has correct noexcept specification for custom types
CONSTEXPR_TYPED_TEST(PlusTest, NoexceptValuesOverloaded) {
  EXPECT_TRUE(check_plus_noexcept_specification<has_plus<TypeParam, true>>());
  EXPECT_TRUE(check_plus_noexcept_specification<has_plus<TypeParam, false>>());
}

/// @test @c std::plus cannot be invoked when no @c operator+ is defined
TEST(PlusTest, NotInvocableWithDifferentTypes) {
  struct no_operator_plus {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::plus<>, no_operator_plus, no_operator_plus>);
}

struct not_int_addable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr not_int_addable(int value)
      : value_(value) {}
  constexpr auto operator+(not_int_addable const& rhs) const -> not_int_addable {
    return not_int_addable{value_ + rhs.value_};
  }
  constexpr auto value() const -> int { return value_; }

 private:
  int value_;
};

struct int_addable {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr int_addable(int value)
      : value_(value) {}
  constexpr auto operator+(int_addable const& rhs) const -> int_addable { return int_addable{value_ + rhs.value_}; }
  constexpr auto operator+(int const& rhs) const -> int { return value_ + rhs; }

 private:
  int value_;
};

/// @test @c std::plus does convert input when necessary
TEST(PlusTest, DoesConvertInput) {
  auto const first = not_int_addable{1};
  auto const second = int{2};

  testing::StaticAssertTypeEq<decltype(std::plus<>{}(first, second)), not_int_addable>();
  EXPECT_EQ(std::plus<>{}(first, second).value(), 3);
}

/// @test @c std::plus does not convert input when not necessary
TEST(PlusTest, PlusDoesNotConvert) {
  auto const first = int_addable{1};
  auto const second = int{2};

  testing::StaticAssertTypeEq<decltype(std::plus<>{}(first, second)), int>();
  EXPECT_EQ(std::plus<>{}(first, second), 3);
}

// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
