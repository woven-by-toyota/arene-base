// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A simple user-defined type
class some_user_type {};

enum raw_enum {};
enum raw_enum_int : int {};
// NOLINTNEXTLINE(google-runtime-int)
enum raw_enum_short : short {};
// NOLINTNEXTLINE(google-runtime-int)
enum raw_enum_long : long {};
enum raw_enum_char : char {};
enum raw_enum_uint : unsigned {};
// NOLINTNEXTLINE(google-runtime-int)
enum raw_enum_ushort : unsigned short {};
// NOLINTNEXTLINE(google-runtime-int)
enum raw_enum_ulong : unsigned long {};
enum raw_enum_uchar : unsigned char {};

enum class enum_class_int : int {};
// NOLINTNEXTLINE(google-runtime-int)
enum class enum_class_short : short {};
// NOLINTNEXTLINE(google-runtime-int)
enum class enum_class_long : long {};
enum class enum_class_char : char {};
enum class enum_class_uint : unsigned {};
// NOLINTNEXTLINE(google-runtime-int)
enum class enum_class_ushort : unsigned short {};
// NOLINTNEXTLINE(google-runtime-int)
enum class enum_class_ulong : unsigned long {};
enum class enum_class_uchar : unsigned char {};

using enum_types = testing::Types<
    raw_enum,
    raw_enum_char,
    raw_enum_int,
    raw_enum_short,
    raw_enum_long,
    raw_enum_uchar,
    raw_enum_uint,
    raw_enum_ushort,
    raw_enum_ulong,
    enum_class_char,
    enum_class_int,
    enum_class_short,
    enum_class_long,
    enum_class_uchar,
    enum_class_uint,
    enum_class_ushort,
    enum_class_ulong>;

using non_enum_types = ::testing::Types<
    raw_enum&,
    raw_enum_char&,
    raw_enum_int&,
    raw_enum_short&,
    raw_enum_long&,
    enum_class_char&,
    enum_class_int&,
    enum_class_short&,
    enum_class_long&,
    raw_enum&&,
    raw_enum_char&&,
    raw_enum_int&&,
    raw_enum_short&&,
    raw_enum_long&&,
    enum_class_char&&,
    enum_class_int&&,
    enum_class_short&&,
    enum_class_long&&,
    char,
    signed char,
    unsigned char,
    short,           // NOLINT
    unsigned short,  // NOLINT
    int,
    unsigned,
    long,                // NOLINT
    unsigned long,       // NOLINT
    long long,           // NOLINT
    unsigned long long,  // NOLINT
    bool,
    wchar_t,
    std::nullptr_t,
    int&,
    unsigned&,
    int const&,
    unsigned const&,
    int&&,
    unsigned&&,
    int const&&,
    unsigned const&&,
    float,
    double,
    long double,
    some_user_type,
    void (*)(int, double),
    int*,
    int const*,
    int volatile*,
    int const volatile*,
    void*,
    void const*,
    void volatile*,
    void const volatile*,
    int some_user_type::*,
    int (some_user_type::*)()>;

template <typename T>
class IsEnumTest : public testing::Test {};

TYPED_TEST_SUITE(IsEnumTest, non_enum_types, );

/// @test @c is_enum is derived from @c false_type and @c is_enum_v is @c false for types that are not enum types
TYPED_TEST(IsEnumTest, NonEnumTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_enum<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_enum_v<TypeParam>);
}

template <typename T>
class IsEnumWithEnumsTest : public testing::Test {};

TYPED_TEST_SUITE(IsEnumWithEnumsTest, enum_types, );

/// @test @c is_enum is derived from @c true_type and @c is_enum_v is @c true for enum types
TYPED_TEST(IsEnumWithEnumsTest, EnumTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_enum<TypeParam>, std::true_type>);
  ASSERT_TRUE(std::is_enum_v<TypeParam>);

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_enum<TypeParam const>, std::true_type>);
  ASSERT_TRUE(std::is_enum_v<TypeParam const>);

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_enum<TypeParam volatile>, std::true_type>);
  ASSERT_TRUE(std::is_enum_v<TypeParam volatile>);

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_enum<TypeParam const volatile>, std::true_type>);
  ASSERT_TRUE(std::is_enum_v<TypeParam const volatile>);
}

}  // namespace
