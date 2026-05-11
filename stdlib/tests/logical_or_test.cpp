// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/type_traits/is_transparent_comparator_for.hpp"
#include "testlibs/minitest/minitest.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_CLANG(
    "-Wnull-conversion",
    "For testing, `nullptr || nullptr` is a valid conversion of nullptr constant to bool and should evaluate to false."
);
#include "stdlib/include/functional"
ARENE_IGNORE_END();
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"

namespace {

// Need to test both the transparent and non-transparent template specializations.
// NOLINTBEGIN(modernize-use-transparent-functors)

template <bool NoExcept = true>
struct has_logical_or {
  friend constexpr auto operator||(has_logical_or const&, has_logical_or const&) noexcept(NoExcept) -> has_logical_or {
    return has_logical_or{operator_return_value};
  }

  friend constexpr auto operator==(has_logical_or const& lhs, has_logical_or const& rhs) noexcept -> bool {
    return lhs.value == rhs.value;
  }

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator bool() const noexcept { return value != operator_return_value; }

  static constexpr auto operator_return_value = int{42};

  int value{};
};

template <class T>
class LogicalOrTest : public testing::Test {};

using logical_or_types = ::testing::Types<int, bool, has_logical_or<true>, has_logical_or<false>>;
TYPED_TEST_SUITE(LogicalOrTest, logical_or_types, );

/// @test @c std::logical_or has correct member types
CONSTEXPR_TYPED_TEST(LogicalOrTest, MemberTypes) {
  using logical_or_type = std::logical_or<TypeParam>;
  testing::StaticAssertTypeEq<typename logical_or_type::result_type, bool>();
  testing::StaticAssertTypeEq<typename logical_or_type::first_argument_type, TypeParam>();
  testing::StaticAssertTypeEq<typename logical_or_type::second_argument_type, TypeParam>();
  ASSERT_FALSE(arene::base::is_transparent_comparator_v<logical_or_type>);
}

/// @test @c std::logical_or has correct member types for void specialization
CONSTEXPR_TEST(LogicalOrTest, VoidSpecializationMemberTypes) {
  ASSERT_TRUE(arene::base::is_transparent_comparator_v<typename std::logical_or<void>>);
}

/// @test @c std::logical_or has proper nothrow properties
CONSTEXPR_TYPED_TEST(LogicalOrTest, NoThrowProperties) {
  using logical_or_type = std::logical_or<TypeParam>;

  ASSERT_TRUE(std::is_nothrow_move_constructible_v<logical_or_type>);
  ASSERT_TRUE(std::is_nothrow_copy_constructible_v<logical_or_type>);
  ASSERT_TRUE(std::is_nothrow_copy_assignable_v<logical_or_type>);
}

/// @test @c std::logical_or<void> has proper nothrow properties
CONSTEXPR_TEST(LogicalOrTest, VoidSpecializationNoThrowProperties) {
  using logical_or_type = std::logical_or<void>;

  ASSERT_TRUE(std::is_nothrow_move_constructible_v<logical_or_type>);
  ASSERT_TRUE(std::is_nothrow_copy_constructible_v<logical_or_type>);
  ASSERT_TRUE(std::is_nothrow_copy_assignable_v<logical_or_type>);
}

template <class T, class U>
constexpr auto check_logical_or(T first, T second, U expected) -> bool {
  auto result = std::logical_or<T>{}(first, second) == expected && std::logical_or<>{}(first, second) == expected;

  return result;
}

template <class T>
constexpr auto check_logical_or(T first, T second) -> bool {
  auto const expected = first || second;
  return check_logical_or(first, second, expected);
}

/// @test @c std::logical_or with fundamental types
CONDITIONAL_TYPED_TEST(LogicalOrTest, WithBuiltInTypes, std::is_fundamental_v<TypeParam>) {
  constexpr auto first = TypeParam{0};
  constexpr auto second = TypeParam{1};
  static_assert(check_logical_or(first, second), "");
  ASSERT_TRUE(check_logical_or(first, second));
}

/// @test @c std::logical_or with non-fundamental (e.g., user defined) types
CONDITIONAL_TYPED_TEST(LogicalOrTest, HasLogicalOr, !std::is_fundamental_v<TypeParam> && !std::is_pointer_v<TypeParam>) {
  constexpr auto first = TypeParam{0};
  constexpr auto second = TypeParam{1};

  static_assert(
      std::is_same_v<bool, arene::base::invoke_result_t<std::logical_or<TypeParam>, TypeParam, TypeParam>>,
      ""
  );
  static_assert(std::is_same_v<TypeParam, arene::base::invoke_result_t<std::logical_or<>, TypeParam, TypeParam>>, "");
  static_assert(check_logical_or(first, second), "");
  ASSERT_TRUE(check_logical_or(first, second));

  ASSERT_TRUE(check_logical_or(first, second, TypeParam{TypeParam::operator_return_value}));
}

template <class T>
constexpr auto default_constructed = T{};

/// @test @c std::logical_or with pointer types
CONSTEXPR_TYPED_TEST(LogicalOrTest, PointerTypes) {
  using pointer_t = TypeParam const*;

  static_assert(
      std::is_same_v<bool, arene::base::invoke_result_t<std::logical_or<pointer_t>, pointer_t, pointer_t>>,
      ""
  );
  static_assert(std::is_same_v<bool, arene::base::invoke_result_t<std::logical_or<>, pointer_t, pointer_t>>, "");
  ASSERT_TRUE(check_logical_or(&default_constructed<TypeParam>, &default_constructed<TypeParam>));
  ASSERT_TRUE(check_logical_or(pointer_t{}, &default_constructed<TypeParam>));
  ASSERT_TRUE(check_logical_or(&default_constructed<TypeParam>, pointer_t{}));
  ASSERT_TRUE(check_logical_or(pointer_t{}, pointer_t{}));
  ASSERT_FALSE(std::logical_or<pointer_t>{}(nullptr, nullptr));
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
constexpr auto check_logical_or_with_nullptr() -> bool {
  // NOLINTNEXTLINE(readability-implicit-bool-conversion)
  return !(std::logical_or<std::nullptr_t>{}(nullptr, nullptr)) && !(nullptr || nullptr);
}
ARENE_IGNORE_END();

/// @test @c std::logical_or with nullptr
CONSTEXPR_TEST(LogicalOrTest, NullPointer) {
  ASSERT_TRUE(check_logical_or_with_nullptr());
  ASSERT_FALSE(std::logical_or<>{}(nullptr, nullptr));
}

/// @test @c std::logical_or has correct noexcept specification
CONSTEXPR_TYPED_TEST(LogicalOrTest, NoexceptCallable) {
  {
    constexpr auto expected_noexceptness =
        noexcept(std::declval<TypeParam const&>() || std::declval<TypeParam const&>());

    ASSERT_TRUE(
        expected_noexceptness == noexcept(std::logical_or<TypeParam>{}(  //
                                     std::declval<TypeParam const&>(),
                                     std::declval<TypeParam const&>()
                                 ))
    );
    ASSERT_TRUE(
        expected_noexceptness == noexcept(std::logical_or<>{}(  //
                                     std::declval<TypeParam&>(),
                                     std::declval<TypeParam&>()
                                 ))
    );
  }

  {
    using pointer_t = TypeParam*;
    constexpr auto expected_noexceptness = noexcept(std::declval<pointer_t*>() || std::declval<pointer_t*>());

    ASSERT_TRUE(
        expected_noexceptness == noexcept(std::logical_or<pointer_t*>{}(  //
                                     std::declval<pointer_t*>(),
                                     std::declval<pointer_t*>()
                                 ))
    );
    ASSERT_TRUE(
        expected_noexceptness == noexcept(std::logical_or<>{}(  //
                                     std::declval<pointer_t*>(),
                                     std::declval<pointer_t*>()
                                 ))
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

  friend constexpr auto operator||(T3, T3) -> bool { return true; }
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
constexpr auto operator||(T4, T5) -> T6 { return {}; }
ARENE_IGNORE_END();

/// @test @c std::logical_or invocability tests
CONSTEXPR_TEST(LogicalOrTest, Invocability) {
  static_assert(not arene::base::is_invocable_v<std::logical_or<>, T1, T2>, "");
  static_assert(arene::base::is_invocable_v<std::logical_or<T3>, T1, T2>, "");
  ASSERT_TRUE(std::logical_or<T3>{}(T1{}, T2{}), "");

  static_assert(arene::base::is_invocable_v<std::logical_or<>, T4, T5>, "");
  static_assert(std::is_same_v<T6, arene::base::invoke_result_t<std::logical_or<>, T4, T5>>, "");
  static_assert(not arene::base::is_invocable_v<std::logical_or<>, T5, T4>, "");
}
// NOLINTEND(modernize-use-transparent-functors)

}  // namespace
