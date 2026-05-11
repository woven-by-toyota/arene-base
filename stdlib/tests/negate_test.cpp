// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/cstdint"
#include "stdlib/include/functional"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

// Need to test both versions.
// NOLINTBEGIN(modernize-use-transparent-functors)

template <class T, bool NoExcept = true>
struct has_negate {
  constexpr explicit has_negate(T value)
      : value_(value) {}
  constexpr auto operator-() const noexcept(NoExcept) -> has_negate<T, NoExcept> {
    return has_negate(operator_return_value);
  }
  constexpr auto operator==(has_negate const& rhs) const -> bool {
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
class NegateTest : public testing::Test {};

using negate_types = arene::base::type_lists::concat_unique_t<
    ::testing::signed_integer_types,
    ::testing::Types<unsigned int, std::uint64_t>,
    ::testing::floating_point_types>;
TYPED_TEST_SUITE(NegateTest, negate_types, );

template <class T, class U>
constexpr auto check_negate(T value, U expected) -> bool {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL(
      "-Wfloat-equal",
      "Performing identitical operations on floating point values, so exact equality is ok."
  );
  auto result = std::negate<T>{}(value) == expected && std::negate<>{}(value) == expected;
  ARENE_IGNORE_END();
  return result;
}

template <class T>
constexpr auto check_negate(T value) -> bool {
  return check_negate(value, static_cast<T>(-value));
}

template <class T>
constexpr auto check_negate_noexcept_specification() -> bool {
  auto const should_be_noexcept = noexcept(-std::declval<T&>());

  return noexcept(std::negate<T>{}(std::declval<T&>())) ==
         should_be_noexcept&& noexcept(std::negate<>{}(std::declval<T&>())) == should_be_noexcept;
}

/// @test @c std::negate has proper member types
CONSTEXPR_TYPED_TEST(NegateTest, MemberTypes) {
  using negate_type = std::negate<TypeParam>;
  testing::StaticAssertTypeEq<typename negate_type::result_type, TypeParam>();
  testing::StaticAssertTypeEq<typename negate_type::argument_type, TypeParam>();
}

/// @test @c std::negate has proper nothrow properties
CONSTEXPR_TYPED_TEST(NegateTest, Properties) {
  using negate_type = std::negate<TypeParam>;

  EXPECT_TRUE(std::is_nothrow_move_constructible_v<negate_type>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<negate_type>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<negate_type>);
}

/// @test @c std::negate with built-in types
CONSTEXPR_TYPED_TEST(NegateTest, WithBuiltInTypes) {
  auto const value = TypeParam{2};

  EXPECT_TRUE(check_negate(value));
}

/// @test @c std::negate with small unsigned types
CONSTEXPR_TYPED_TEST(NegateTest, WithSmallUnsignedType) {
  auto const value = std::uint8_t{2};

  EXPECT_EQ(std::negate<std::uint8_t>{}(value), static_cast<std::uint8_t>(-value));
  EXPECT_EQ(std::negate<>{}(value), -value);
}

/// @test @c std::negate with values in an overloaded operator
CONSTEXPR_TYPED_TEST(NegateTest, WithValuesOverloaded) {
  using T = has_negate<TypeParam>;
  auto const value = T{2};

  EXPECT_TRUE(check_negate(value, T{T::operator_return_value}));
}

/// @test @c std::negate has correct noexcept specification for integral types
CONSTEXPR_TYPED_TEST(NegateTest, NoexceptIntegralTypes) {
  EXPECT_TRUE(check_negate_noexcept_specification<TypeParam>());
}

/// @test @c std::negate has correct noexcept specification for custom types
CONSTEXPR_TYPED_TEST(NegateTest, NoexceptValuesOverloaded) {
  EXPECT_TRUE(check_negate_noexcept_specification<has_negate<TypeParam, true>>());
  EXPECT_TRUE(check_negate_noexcept_specification<has_negate<TypeParam, false>>());
}

/// @test @c std::negate cannot be invoked when no @c operator- is defined
TEST(NegateTest, NotInvocableWithDifferentTypes) {
  struct no_operator_negate {};

  ASSERT_FALSE(testing::simple_is_invocable_v<std::negate<>, no_operator_negate, no_operator_negate>);
}

// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
