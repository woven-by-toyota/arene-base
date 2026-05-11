// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_integral_constant_like.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::is_integral_constant_like;
using ::arene::base::is_integral_constant_like_v;

using ic = std::integral_constant<int, 0>;

constexpr int zero{};

/// @test `is_integral_constant_like` is `true` for most specializations of
/// `std::integral_constant`
TEST(IntegralConstantLike, MostIntegralConstantsAreTrue) {
  STATIC_ASSERT_TRUE(is_integral_constant_like_v<ic>);
  STATIC_ASSERT_TRUE(is_integral_constant_like_v<ic const>);

  STATIC_ASSERT_TRUE(is_integral_constant_like_v<std::integral_constant<std::size_t, 0>>);
  STATIC_ASSERT_TRUE(is_integral_constant_like_v<std::integral_constant<std::size_t, 0> const>);

  STATIC_ASSERT_TRUE(is_integral_constant_like_v<std::integral_constant<int const&, zero>>);
  STATIC_ASSERT_TRUE(is_integral_constant_like_v<std::integral_constant<int const&, zero> const>);

  STATIC_ASSERT_TRUE(is_integral_constant_like<ic>::value);
  STATIC_ASSERT_TRUE(is_integral_constant_like<ic const>::value);

  STATIC_ASSERT_TRUE(is_integral_constant_like<std::integral_constant<std::size_t, 0>>::value);
  STATIC_ASSERT_TRUE(is_integral_constant_like<std::integral_constant<std::size_t, 0> const>::value);

  STATIC_ASSERT_TRUE(is_integral_constant_like<std::integral_constant<int const&, zero>>::value);
  STATIC_ASSERT_TRUE(is_integral_constant_like<std::integral_constant<int const&, zero> const>::value);
}

/// @test `is_integral_constant_like` is `false` for `std::integral_constant` with a pointer type
TEST(IntegralConstantLike, PointerIntegralConstantsAreFalse) {
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<std::integral_constant<int const*, &zero>>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<std::integral_constant<int*, nullptr> const>);

  STATIC_ASSERT_FALSE(is_integral_constant_like<std::integral_constant<int const*, &zero>>::value);
  STATIC_ASSERT_FALSE(is_integral_constant_like<std::integral_constant<int*, nullptr> const>::value);
}

/// @test `is_integral_constant_like` is `false` for `std::integral_constant` with a bool type
TEST(IntegralConstantLike, BoolIntegralConstantsAreFalse) {
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<std::integral_constant<bool, true>>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<std::integral_constant<bool, true> const>);

  STATIC_ASSERT_FALSE(is_integral_constant_like<std::integral_constant<bool, true>>::value);
  STATIC_ASSERT_FALSE(is_integral_constant_like<std::integral_constant<bool, true> const>::value);
}

/// @test `is_integral_constant_like` is `false` for ints
TEST(IntegralConstantLike, IntsAreFalse) {
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<int>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<int const>);

  STATIC_ASSERT_FALSE(is_integral_constant_like<int>::value);
  STATIC_ASSERT_FALSE(is_integral_constant_like<int const>::value);
}

/// @test `is_integral_constant_like` is `false` for reference types
TEST(IntegralConstantLike, ReferenceTypesAreFalse) {
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<ic&>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<ic const&>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<ic&&>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<ic const&&>);

  STATIC_ASSERT_FALSE(is_integral_constant_like<ic&>::value);
  STATIC_ASSERT_FALSE(is_integral_constant_like<ic const&>::value);
  STATIC_ASSERT_FALSE(is_integral_constant_like<ic&&>::value);
  STATIC_ASSERT_FALSE(is_integral_constant_like<ic const&&>::value);
}

/// @test `is_integral_constant_like` is `false` for pointer types
TEST(IntegralConstantLike, PointerTypesAreFalse) {
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<ic*>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<ic const*>);

  STATIC_ASSERT_FALSE(is_integral_constant_like<ic*>::value);
  STATIC_ASSERT_FALSE(is_integral_constant_like<ic const*>::value);
}

/// @test `is_integral_constant_like` is `true` for types that publicly inherit
/// from specializations of `std::integral_constant`
TEST(IntegralConstantLike, TypesThatPubliclyInheritFromIntegralConstantsAreTrue) {
  struct derived_from_ic : ic {};

  STATIC_ASSERT_TRUE(is_integral_constant_like_v<derived_from_ic>);
  STATIC_ASSERT_TRUE(is_integral_constant_like_v<derived_from_ic const>);

  STATIC_ASSERT_TRUE(is_integral_constant_like<derived_from_ic>::value);
  STATIC_ASSERT_TRUE(is_integral_constant_like<derived_from_ic const>::value);
}

template <class T>
struct zero_constant {
  static constexpr auto value = T{};
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator T() const noexcept { return value; }
};

/// @test `is_integral_constant_like` is `true` for types provide all the same
/// members as `std::integral_constant` and where the value is integral
TEST(IntegralConstantLike, TypesThatProvideTheSameMembersAsIntegralConstantsAreTrue) {
  STATIC_ASSERT_TRUE(is_integral_constant_like_v<zero_constant<int>>);
  STATIC_ASSERT_TRUE(is_integral_constant_like_v<zero_constant<int> const>);

  STATIC_ASSERT_TRUE(is_integral_constant_like<zero_constant<int>>::value);
  STATIC_ASSERT_TRUE(is_integral_constant_like<zero_constant<int> const>::value);
}

/// @test `is_integral_constant_like` is `false` for types provide all the same
/// members as `std::integral_constant` and where the value is not integral
TEST(IntegralConstantLike, TypesThatProvideTheSameMembersAsIntegralConstantButValueIsNotIntegralAreFalse) {
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<zero_constant<float>>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<zero_constant<float> const>);

  STATIC_ASSERT_FALSE(is_integral_constant_like<zero_constant<float>>::value);
  STATIC_ASSERT_FALSE(is_integral_constant_like<zero_constant<float> const>::value);
}

struct value_is_member_function_template {
  template <class>
  auto value() const -> int {
    return {};
  }

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator int() const noexcept { return 0; }
};

/// @test `is_integral_constant_like` is `false` for types that define `value`
/// but it is not static data member.
TEST(IntegralConstantLike, TypeWithNonStaticValue) {
  struct value_is_non_static_member_data {
    int value{};

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator int() const noexcept { return value; }
  };

  struct value_is_non_static_member_function {
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto value() const -> int { return {}; }

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator int() const noexcept { return 0; }
  };

  struct value_is_static_member_function {
    static auto value() -> int { return {}; }

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator int() const noexcept { return 0; }
  };

  struct value_is_type_alias {
    using value = int;

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator int() const noexcept { return value{}; }
  };

  STATIC_ASSERT_FALSE(is_integral_constant_like_v<value_is_non_static_member_data>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<value_is_non_static_member_function>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<value_is_static_member_function>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<value_is_member_function_template>);
  STATIC_ASSERT_FALSE(is_integral_constant_like_v<value_is_type_alias>);
}

/// @test `is_integral_constant_like` is `false` for types that are not default constructible
TEST(IntegralConstantLike, NonDefaultConstructible) {
  struct not_default_constructible : std::integral_constant<int, 0> {
    not_default_constructible() = delete;
  };

  STATIC_ASSERT_FALSE(is_integral_constant_like_v<not_default_constructible>);
}

/// @test `is_integral_constant_like` is `false` for types that are not constexpr default constructible
TEST(IntegralConstantLike, NonConstexprDefaultConstructible) {
  struct not_constexpr_default_constructible : std::integral_constant<int, 0> {
    // NOLINTNEXTLINE(hicpp-use-equals-default)
    not_constexpr_default_constructible() noexcept {}
  };

  STATIC_ASSERT_FALSE(is_integral_constant_like_v<not_constexpr_default_constructible>);
}

}  // namespace
