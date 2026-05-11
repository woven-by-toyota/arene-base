// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/type_traits/is_transparent_comparator_for.hpp"
#include "testlibs/minitest/minitest.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_CLANG(
    "-Wnull-conversion",
    "For testing, `nullptr && nullptr` is a valid conversion of nullptr constant to bool and should evaluate to false."
);
#include "stdlib/include/functional"
ARENE_IGNORE_END();
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"

namespace {

// Need to test both the transparent and non-transparent template specializations.
// NOLINTBEGIN(modernize-use-transparent-functors)

template <bool NoExcept = true>
struct has_logical_and {
  friend constexpr auto operator&&(has_logical_and const&, has_logical_and const&) noexcept(NoExcept)
      -> has_logical_and {
    return has_logical_and{operator_return_value};
  }

  friend constexpr auto operator==(has_logical_and const& lhs, has_logical_and const& rhs) noexcept -> bool {
    return lhs.value == rhs.value;
  }

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator bool() const noexcept { return value != operator_return_value; }

  static constexpr auto operator_return_value = int{42};

  int value{};
};

template <class T>
class LogicalAndTest : public testing::Test {};

using logical_and_types = ::testing::Types<int, bool, has_logical_and<true>, has_logical_and<false>>;
TYPED_TEST_SUITE(LogicalAndTest, logical_and_types, );

/// @test @c std::logical_and has correct member types
CONSTEXPR_TYPED_TEST(LogicalAndTest, MemberTypes) {
  using logical_and_type = std::logical_and<TypeParam>;
  testing::StaticAssertTypeEq<typename logical_and_type::result_type, bool>();
  testing::StaticAssertTypeEq<typename logical_and_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename logical_and_type::second_argument_type, TypeParam>();
  ASSERT_FALSE(arene::base::is_transparent_comparator_v<logical_and_type>);
}

/// @test @c std::logical_and has correct member types for void specialization
CONSTEXPR_TEST(LogicalAndTest, VoidSpecializationMemberTypes) {
  ASSERT_TRUE(arene::base::is_transparent_comparator_v<typename std::logical_and<void>>);
}

/// @test @c std::logical_and has proper nothrow properties
CONSTEXPR_TYPED_TEST(LogicalAndTest, NoThrowProperties) {
  using logical_and_type = std::logical_and<TypeParam>;

  ASSERT_TRUE(std::is_nothrow_move_constructible_v<logical_and_type>);
  ASSERT_TRUE(std::is_nothrow_copy_constructible_v<logical_and_type>);
  ASSERT_TRUE(std::is_nothrow_copy_assignable_v<logical_and_type>);
}

/// @test @c std::logical_and<void> has proper nothrow properties
CONSTEXPR_TEST(LogicalAndTest, VoidSpecializationNoThrowProperties) {
  using logical_and_type = std::logical_and<void>;

  ASSERT_TRUE(std::is_nothrow_move_constructible_v<logical_and_type>);
  ASSERT_TRUE(std::is_nothrow_copy_constructible_v<logical_and_type>);
  ASSERT_TRUE(std::is_nothrow_copy_assignable_v<logical_and_type>);
}

template <class T, class U>
constexpr auto check_logical_and(T first, T second, U expected) -> bool {
  auto result = std::logical_and<T>{}(first, second) == expected && std::logical_and<>{}(first, second) == expected;

  return result;
}

template <class T>
constexpr auto check_logical_and(T first, T second) -> bool {
  auto const expected = first && second;
  return check_logical_and(first, second, expected);
}

/// @test @c std::logical_and with fundamental types
CONDITIONAL_TYPED_TEST(LogicalAndTest, WithBuiltInTypes, std::is_fundamental_v<TypeParam>) {
  constexpr auto first = TypeParam{0};
  constexpr auto second = TypeParam{1};
  static_assert(check_logical_and(first, second), "");
  ASSERT_TRUE(check_logical_and(first, second));
}

/// @test @c std::logical_and with non-fundamental (e.g., user defined) types
CONDITIONAL_TYPED_TEST(LogicalAndTest, HasLogicalAnd, !std::is_fundamental_v<TypeParam> && !std::is_pointer_v<TypeParam>) {
  constexpr auto first = TypeParam{0};
  constexpr auto second = TypeParam{1};

  static_assert(
      std::is_same_v<bool, arene::base::invoke_result_t<std::logical_and<TypeParam>, TypeParam, TypeParam>>,
      ""
  );
  static_assert(std::is_same_v<TypeParam, arene::base::invoke_result_t<std::logical_and<>, TypeParam, TypeParam>>, "");
  static_assert(check_logical_and(first, second), "");
  ASSERT_TRUE(check_logical_and(first, second));

  ASSERT_TRUE(check_logical_and(first, second, TypeParam{TypeParam::operator_return_value}));
}

template <class T>
constexpr auto default_constructed = T{};

/// @test @c std::logical_and with pointer types
CONSTEXPR_TYPED_TEST(LogicalAndTest, PointerTypes) {
  using pointer_t = TypeParam const*;

  static_assert(
      std::is_same_v<bool, arene::base::invoke_result_t<std::logical_and<pointer_t>, pointer_t, pointer_t>>,
      ""
  );
  static_assert(std::is_same_v<bool, arene::base::invoke_result_t<std::logical_and<>, pointer_t, pointer_t>>, "");
  ASSERT_TRUE(check_logical_and(&default_constructed<TypeParam>, &default_constructed<TypeParam>));
  ASSERT_TRUE(check_logical_and(pointer_t{}, &default_constructed<TypeParam>));
  ASSERT_TRUE(check_logical_and(&default_constructed<TypeParam>, pointer_t{}));
  ASSERT_TRUE(check_logical_and(pointer_t{}, pointer_t{}));
  ASSERT_FALSE(std::logical_and<pointer_t>{}(nullptr, nullptr));
}

ARENE_IGNORE_START();
ARENE_IGNORE_CLANG(
    "-Wnull-conversion",
    "For testing, `nullptr && nullptr` is a valid conversion of nullptr constant to bool and should evaluate to false."
);
ARENE_IGNORE_ARMCLANG(
    "-Wnull-conversion",
    "For testing, `nullptr && nullptr` is a valid conversion of nullptr constant to bool and should evaluate to false."
);
ARENE_IGNORE_ALL("-Wunreachable-code", "`nullptr && nullptr` is a sanity check on our nullptr implementation.");
constexpr auto check_logical_and_with_nullptr() -> bool {
  // NOLINTNEXTLINE(readability-implicit-bool-conversion)
  return !(std::logical_and<std::nullptr_t>{}(nullptr, nullptr)) && !(nullptr && nullptr);
}
ARENE_IGNORE_END();

/// @test @c std::logical_and with nullptr
CONSTEXPR_TEST(LogicalAndTest, NullPointer) {
  ASSERT_TRUE(check_logical_and_with_nullptr());
  ASSERT_FALSE(std::logical_and<>{}(nullptr, nullptr));
}

/// @test @c std::logical_and has correct noexcept specification
CONSTEXPR_TYPED_TEST(LogicalAndTest, NoexceptCallable) {
  {
    constexpr auto expected_noexceptness =
        noexcept(std::declval<TypeParam const&>() && std::declval<TypeParam const&>());

    ASSERT_TRUE(
        noexcept(std::logical_and<TypeParam>{}(std::declval<TypeParam const&>(), std::declval<TypeParam const&>())) ==
        expected_noexceptness
    );
    ASSERT_TRUE(
        noexcept(std::logical_and<>{}(std::declval<TypeParam&>(), std::declval<TypeParam&>())) == expected_noexceptness
    );
  }
  {
    using pointer_t = TypeParam*;
    constexpr auto expected_noexceptness = noexcept(std::declval<pointer_t*>() && std::declval<pointer_t*>());

    ASSERT_TRUE(
        noexcept(std::logical_and<pointer_t*>{}(std::declval<pointer_t*>(), std::declval<pointer_t*>())) ==
        expected_noexceptness
    );
    ASSERT_TRUE(
        noexcept(std::logical_and<>{}(std::declval<pointer_t*>(), std::declval<pointer_t*>())) == expected_noexceptness
    );
  }
}

struct T1 {};

struct T2 {};

struct T3 {
  T3() = default;

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr T3(T1) {}

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr T3(T2) {}

  friend constexpr auto operator&&(T3, T3) -> bool { return true; }
};

struct T4 {};

struct T5 {};

struct T6 {};

ARENE_IGNORE_START();
ARENE_IGNORE_CLANG(
    "-Wunneeded-internal-declaration",
    "Performing invokability tests require this function to exist even if it is never called."
);
ARENE_IGNORE_ARMCLANG(
    "-Wunneeded-internal-declaration",
    "Performing invokability tests require this function to exist even if it is never called."
);
constexpr auto operator&&(T4, T5) -> T6 { return {}; }
ARENE_IGNORE_END();

/// @test @c std::logical_and invocability tests
CONSTEXPR_TEST(LogicalAndTest, Invocability) {
  static_assert(not arene::base::is_invocable_v<std::logical_and<>, T1, T2>, "");
  static_assert(arene::base::is_invocable_v<std::logical_and<T3>, T1, T2>, "");
  ASSERT_TRUE(std::logical_and<T3>{}(T1{}, T2{}), "");

  static_assert(arene::base::is_invocable_v<std::logical_and<>, T4, T5>, "");
  static_assert(std::is_same_v<T6, arene::base::invoke_result_t<std::logical_and<>, T4, T5>>, "");
  static_assert(not arene::base::is_invocable_v<std::logical_and<>, T5, T4>, "");
}
// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
