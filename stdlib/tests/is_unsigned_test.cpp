// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

#if !defined(__clang__) && defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
_Pragma("GCC diagnostic ignored \"-Wpedantic\"");
_Pragma("GCC diagnostic ignored \"-Wtype-limits\"");
#endif

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A simple user-defined type
class some_user_type {};

// See https://eel.is/c++draft/type.traits#tab:meta.unary.prop
using unsigned_types = ::testing::Types<
    bool,
    char16_t,
    char32_t,
    unsigned char,
    unsigned short,  // NOLINT
    unsigned int,
    unsigned long,        // NOLINT
    unsigned long long>;  // NOLINT

using non_unsigned_types = ::testing::Types<
    signed char,  //
    short,        // NOLINT
    int,
    long,       // NOLINT
    long long,  // NOLINT
    float,
    double,
    long double,
    std::nullptr_t,
    void,
    int&,
    unsigned&,
    int const&,
    unsigned const&,
    int&&,
    unsigned&&,
    int const&&,
    unsigned const&&,
    some_user_type,
    void (*)(int, double),
    int*,
    int const*,
    int volatile*,
    int const volatile*,
    void*,
    int some_user_type::*,
    int (some_user_type::*)()>;

template <typename T>
class IsUnsignedTest : public testing::Test {};

TYPED_TEST_SUITE(IsUnsignedTest, unsigned_types, );

/// @test @c is_unsigned is derived from @c true_type and @c is_unsigned_v is @c true for unsigned types
TYPED_TEST(IsUnsignedTest, SignedTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_unsigned<TypeParam>, std::true_type>);
  ASSERT_TRUE(std::is_unsigned_v<TypeParam>);
}

/// @test @c is_unsigned is derived from @c true_type and @c is_unsigned_v is @c true for @c const unsigned types
TYPED_TEST(IsUnsignedTest, ConstSignedTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_unsigned<TypeParam const>, std::true_type>);
  ASSERT_TRUE(std::is_unsigned_v<TypeParam const>);
}

/// @test @c is_unsigned is derived from @c true_type and @c is_unsigned_v is @c true for @c volatile unsigned types
TYPED_TEST(IsUnsignedTest, VolatileSignedTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_unsigned<TypeParam volatile>, std::true_type>);
  ASSERT_TRUE(std::is_unsigned_v<TypeParam volatile>);
}

/// @test @c is_unsigned is derived from @c true_type and @c is_unsigned_v is @c true for @c const @c volatile unsigned
/// types
TYPED_TEST(IsUnsignedTest, ConstVolatileSignedTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_unsigned<TypeParam const volatile>, std::true_type>);
  ASSERT_TRUE(std::is_unsigned_v<TypeParam const volatile>);
}

template <typename T>
class IsNonSignedTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonSignedTest, non_unsigned_types, );

/// @test @c is_unsigned is derived from @c false_type and @c is_unsigned_v is @c false for types that are not unsigned
/// types
TYPED_TEST(IsNonSignedTest, NonSignedTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_unsigned<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_unsigned_v<TypeParam>);
}

/// @test @c is_unsigned is derived from @c false_type and @c is_unsigned_v is @c false for @c const types that are not
/// unsigned types
TYPED_TEST(IsNonSignedTest, ConstNonSignedTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_unsigned<TypeParam const>, std::false_type>);
  ASSERT_FALSE(std::is_unsigned_v<TypeParam const>);
}

/// @test @c is_unsigned is derived from @c false_type and @c is_unsigned_v is @c false for @c volatile types that are
/// not unsigned types
TYPED_TEST(IsNonSignedTest, VolatileNonSignedTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_unsigned<TypeParam volatile>, std::false_type>);
  ASSERT_FALSE(std::is_unsigned_v<TypeParam volatile>);
}

/// @test @c is_unsigned is derived from @c false_type and @c is_unsigned_v is @c false for @c const @c volatile types
/// that are not unsigned types
TYPED_TEST(IsNonSignedTest, ConstVolatileNonSignedTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_unsigned<TypeParam const volatile>, std::false_type>);
  ASSERT_FALSE(std::is_unsigned_v<TypeParam const volatile>);
}

template <class T>
constexpr auto decrement(T value) -> T {
  return --value;
}

/// @test @c char may or may not be signed, but @c is_unsigned and @c is_unsigned_v give correct results
TEST(IsUnsignedTest, CorrectResultsForChar) {
  constexpr bool char_is_unsigned = char{} < decrement(char{});
  ASSERT_EQ((is_unambiguously_publicly_derived_from_v<std::is_unsigned<char>, std::true_type>), char_is_unsigned);
  ASSERT_EQ(std::is_unsigned_v<char>, char_is_unsigned);
}

/// @test @c wchar_t may or may not be signed, but @c is_unsigned and @c is_unsigned_v give correct results
TEST(IsUnsignedTest, CorrectResultsForWcharT) {
  constexpr bool wchar_t_is_unsigned = wchar_t{} < decrement(wchar_t{});
  ASSERT_EQ((is_unambiguously_publicly_derived_from_v<std::is_unsigned<wchar_t>, std::true_type>), wchar_t_is_unsigned);
  ASSERT_EQ(std::is_unsigned_v<wchar_t>, wchar_t_is_unsigned);
}

}  // namespace
