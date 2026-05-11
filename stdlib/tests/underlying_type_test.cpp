// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/remove_duplicates.hpp"
#include "stdlib/include/cstdint"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @brief helper type providing enum aliases
/// @tparam Underlying underlying enum type
///
/// Provides aliases for an unscoped enum, enum struct, and enum class with
/// @c Underlying as the underlying integral type.
///
template <typename Underlying>
struct test_case {
  using underlying = Underlying;

  enum enum_unscoped : underlying {};
  enum struct enum_struct : underlying {};
  enum class enum_class : underlying {};
};

using test_case_types = arene::base::type_lists::remove_duplicates_t<::testing::Types<
    test_case<char>,
    test_case<int>,
    test_case<std::int8_t>,
    test_case<std::int16_t>,
    test_case<std::int32_t>,
    test_case<std::uint8_t>,
    test_case<std::uint16_t>,
    test_case<std::uint32_t>>>;

template <class T>
struct UnderlyingType : ::testing::Test {};

TYPED_TEST_SUITE(UnderlyingType, test_case_types, );

using testing::StaticAssertTypeEq;

/// @test @c underlying_type defines type alias @c type for enum types and @c underlying_type_t is an alias to @c type
TYPED_TEST(UnderlyingType, HasTypeMember) {
  using enum_unscoped = typename TypeParam::enum_unscoped;
  using enum_struct = typename TypeParam::enum_struct;
  using enum_class = typename TypeParam::enum_class;

  StaticAssertTypeEq<  //
      typename std::underlying_type<enum_unscoped>::type,
      std::underlying_type_t<enum_unscoped>>();
  StaticAssertTypeEq<
      typename std::underlying_type<enum_unscoped const>::type,
      std::underlying_type_t<enum_unscoped const>>();
  StaticAssertTypeEq<
      typename std::underlying_type<enum_unscoped volatile>::type,
      std::underlying_type_t<enum_unscoped volatile>>();
  StaticAssertTypeEq<
      typename std::underlying_type<enum_unscoped const volatile>::type,
      std::underlying_type_t<enum_unscoped const volatile>>();

  StaticAssertTypeEq<  //
      typename std::underlying_type<enum_struct>::type,
      std::underlying_type_t<enum_struct>>();
  StaticAssertTypeEq<  //
      typename std::underlying_type<enum_struct const>::type,
      std::underlying_type_t<enum_struct const>>();
  StaticAssertTypeEq<
      typename std::underlying_type<enum_struct volatile>::type,
      std::underlying_type_t<enum_struct volatile>>();
  StaticAssertTypeEq<
      typename std::underlying_type<enum_struct const volatile>::type,
      std::underlying_type_t<enum_struct const volatile>>();

  StaticAssertTypeEq<  //
      typename std::underlying_type<enum_class>::type,
      std::underlying_type_t<enum_class>>();
  StaticAssertTypeEq<  //
      typename std::underlying_type<enum_class const>::type,
      std::underlying_type_t<enum_class const>>();
  StaticAssertTypeEq<
      typename std::underlying_type<enum_class volatile>::type,
      std::underlying_type_t<enum_class volatile>>();
  StaticAssertTypeEq<
      typename std::underlying_type<enum_class const volatile>::type,
      std::underlying_type_t<enum_class const volatile>>();
}

/// @test @c underlying_type is the same as the test case specified type
TYPED_TEST(UnderlyingType, TypeSameAs) {
  using underlying = typename TypeParam::underlying;
  using enum_unscoped = typename TypeParam::enum_unscoped;
  using enum_struct = typename TypeParam::enum_struct;
  using enum_class = typename TypeParam::enum_class;

  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_unscoped>>();
  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_unscoped const>>();
  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_unscoped volatile>>();
  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_unscoped const volatile>>();

  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_struct>>();
  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_struct const>>();
  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_struct volatile>>();
  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_struct const volatile>>();

  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_class>>();
  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_class const>>();
  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_class volatile>>();
  StaticAssertTypeEq<underlying, std::underlying_type_t<enum_class const volatile>>();
}

/// @test @c underlying_type is @c int for scoped enums without an explicit underlying type
TEST(UnderlyingType, ScopedImplicitUnderlyingType) {
  enum struct enum_struct {};

  testing::StaticAssertTypeEq<  //
      typename std::underlying_type<enum_struct>::type,
      std::underlying_type_t<enum_struct>>();
  testing::StaticAssertTypeEq<
      typename std::underlying_type<enum_struct const>::type,
      std::underlying_type_t<enum_struct const>>();
  testing::StaticAssertTypeEq<
      typename std::underlying_type<enum_struct volatile>::type,
      std::underlying_type_t<enum_struct volatile>>();
  testing::StaticAssertTypeEq<
      typename std::underlying_type<enum_struct const volatile>::type,
      std::underlying_type_t<enum_struct const volatile>>();

  StaticAssertTypeEq<int, std::underlying_type_t<enum_struct>>();
  StaticAssertTypeEq<int, std::underlying_type_t<enum_struct const>>();
  StaticAssertTypeEq<int, std::underlying_type_t<enum_struct volatile>>();
  StaticAssertTypeEq<int, std::underlying_type_t<enum_struct const volatile>>();

  enum class enum_class {};

  testing::StaticAssertTypeEq<  //
      typename std::underlying_type<enum_class>::type,
      std::underlying_type_t<enum_class>>();
  testing::StaticAssertTypeEq<
      typename std::underlying_type<enum_class const>::type,
      std::underlying_type_t<enum_class const>>();
  testing::StaticAssertTypeEq<
      typename std::underlying_type<enum_class volatile>::type,
      std::underlying_type_t<enum_class volatile>>();
  testing::StaticAssertTypeEq<
      typename std::underlying_type<enum_class const volatile>::type,
      std::underlying_type_t<enum_class const volatile>>();

  StaticAssertTypeEq<int, std::underlying_type_t<enum_class>>();
  StaticAssertTypeEq<int, std::underlying_type_t<enum_class const>>();
  StaticAssertTypeEq<int, std::underlying_type_t<enum_class volatile>>();
  StaticAssertTypeEq<int, std::underlying_type_t<enum_class const volatile>>();
}

/// @test The @c underlying_type of an unscoped enum with an empty enumerator list and without an explicit underlying
/// type is not larger than int
TEST(UnderlyingType, UnscopedImplicitUnderlyingType) {
  enum empty_enum {};

  testing::StaticAssertTypeEq<  //
      typename std::underlying_type<empty_enum>::type,
      std::underlying_type_t<empty_enum>>();
  testing::StaticAssertTypeEq<
      typename std::underlying_type<empty_enum const>::type,
      std::underlying_type_t<empty_enum const>>();
  testing::StaticAssertTypeEq<
      typename std::underlying_type<empty_enum volatile>::type,
      std::underlying_type_t<empty_enum volatile>>();
  testing::StaticAssertTypeEq<
      typename std::underlying_type<empty_enum const volatile>::type,
      std::underlying_type_t<empty_enum const volatile>>();

  StaticAssertTypeEq<std::underlying_type_t<empty_enum>, std::underlying_type_t<empty_enum const>>();
  StaticAssertTypeEq<std::underlying_type_t<empty_enum>, std::underlying_type_t<empty_enum volatile>>();
  StaticAssertTypeEq<std::underlying_type_t<empty_enum>, std::underlying_type_t<empty_enum const volatile>>();

  ASSERT_TRUE(sizeof(std::underlying_type_t<empty_enum>) <= sizeof(int));
}

enum enum_type {};
using pointer_type = enum_type*;
using lvalue_reference_type = enum_type&;
using rvalue_reference_type = enum_type&&;
using integral_type = int;
using floating_point_type = double;
union union_type {
  enum_type value;
};
using array_type = enum_type[1];  // NOLINT(hicpp-avoid-c-arrays)
struct class_type {
  enum_type value;
};
using member_pointer_type = enum_type class_type::*;
using function_pointer_type = auto (*)(enum_type) -> int;
using function_type = decltype(*std::declval<function_pointer_type>());

using non_enum_test_types = ::testing::Types<  //
    pointer_type,
    lvalue_reference_type,
    rvalue_reference_type,
    integral_type,
    floating_point_type,
    union_type,
    array_type,
    class_type,
    member_pointer_type,
    function_pointer_type,
    function_type>;

template <class T>
struct UnderlyingTypeForNonEnum : ::testing::Test {};

TYPED_TEST_SUITE(UnderlyingTypeForNonEnum, non_enum_test_types, );

/// @test @c underlying_type does not define type alias @c type for non-enum types
///
/// @note this implementation includes the C++20 implementation change to make
/// the traits SFINAE-friendly.
///
TYPED_TEST(UnderlyingTypeForNonEnum, NoAliasType) {
  ASSERT_FALSE(::testing::has_member_type_v<std::underlying_type<TypeParam>>);
  ASSERT_FALSE(::testing::has_member_type_v<std::underlying_type<TypeParam const>>);
  ASSERT_FALSE(::testing::has_member_type_v<std::underlying_type<TypeParam volatile>>);
  ASSERT_FALSE(::testing::has_member_type_v<std::underlying_type<TypeParam const volatile>>);
}

}  // namespace
