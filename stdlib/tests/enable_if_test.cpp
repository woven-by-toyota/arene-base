// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstddef"
#include "stdlib/include/cstdint"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {
using testing::has_member_type_v;

/// @brief A simple user-defined type
// NOLINTNEXTLINE(hicpp-special-member-functions)
class some_user_type {
 public:
  explicit constexpr some_user_type(int) {}
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  some_user_type(some_user_type const&) noexcept(false) {}
};

using enable_if_types =
    ::testing::Types<std::uint32_t, std::uint8_t, std::nullptr_t, std::size_t, some_user_type, void>;

template <typename T>
class EnableIfTypedTest : public testing::Test {};

TYPED_TEST_SUITE(EnableIfTypedTest, enable_if_types, );

/// @test @c enable_if does not have a @c type member if condition is @c false
TYPED_TEST(EnableIfTypedTest, FalseParameterHasNoMemberType) {
  static_assert(!has_member_type_v<std::enable_if<false, TypeParam>>, "Must not have member type");
}

/// @test @c enable_if has a @c type member of the specified type if condition is @c true
TYPED_TEST(EnableIfTypedTest, TrueParameterHasMemberType) {
  static_assert(has_member_type_v<std::enable_if<true, TypeParam>>, "Must have member type");
  ::testing::StaticAssertTypeEq<typename std::enable_if<true, TypeParam>::type, TypeParam>();
}

template <bool Condition, typename Type, typename = Type>
constexpr bool enable_if_t_is_defined{false};

template <bool Condition, typename Type>
constexpr bool enable_if_t_is_defined<Condition, Type, std::enable_if_t<Condition, Type>>{true};

/// @test @c enable_if_t is an alias for the specified type if condition is @c true
TYPED_TEST(EnableIfTypedTest, EnableIfTHasSpecifiedTypeForTrueCheck) {
  static_assert(enable_if_t_is_defined<true, TypeParam>, "Must be defined");
  ::testing::StaticAssertTypeEq<std::enable_if_t<true, TypeParam>, TypeParam>();
}

/// @test @c enable_if_t is undefined if condition is @c false
TYPED_TEST(EnableIfTypedTest, EnableIfTUndefinedForFalseCheck) {
  static_assert(!enable_if_t_is_defined<false, TypeParam>, "Must not be defined");
}

/// @test @c enable_if_t with a default type @c T has a member typedef that is the same as @c void
TEST(EnableIf, EnableIfDefaultTypeIsVoid) {
  static_assert(has_member_type_v<std::enable_if<true>>, "Must have member type");
  static_assert(!has_member_type_v<std::enable_if<false>>, "Must not have member type");
  ::testing::StaticAssertTypeEq<std::enable_if<true>::type, void>();
}

template <bool Condition, typename = void>
constexpr bool enable_if_t_is_defined_no_arg{false};

template <bool Condition>
constexpr bool enable_if_t_is_defined_no_arg<Condition, std::enable_if_t<Condition>>{true};

/// @test @c enable_if_t with a default type @c T is the same as @c void
TEST(EnableIf, EnableIfTDefaultTypeIsVoid) {
  static_assert(enable_if_t_is_defined_no_arg<true>, "Must be defined");
  static_assert(!enable_if_t_is_defined_no_arg<false>, "Must not be defined");
  ::testing::StaticAssertTypeEq<std::enable_if_t<true>, void>();
}

}  // namespace
