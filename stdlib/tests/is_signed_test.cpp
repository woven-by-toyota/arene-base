// Copyright 2024, Toyota Motor Corporation
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
using signed_types = ::testing::Types<
    signed char,
    short,  // NOLINT
    int,
    long,       // NOLINT
    long long,  // NOLINT
    float,
    double,
    long double>;

using non_signed_types = ::testing::Types<
    unsigned char,
    unsigned short,  // NOLINT
    unsigned,
    unsigned long,       // NOLINT
    unsigned long long,  // NOLINT
    char16_t,
    char32_t,
    bool,
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
class IsSignedTest : public testing::Test {};

TYPED_TEST_SUITE(IsSignedTest, signed_types, );

/// @test @c is_signed is derived from @c true_type and @c is_signed_v is @c true for signed types
TYPED_TEST(IsSignedTest, SignedTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_signed<TypeParam>, std::true_type>);
  ASSERT_TRUE(std::is_signed_v<TypeParam>);
}

/// @test @c is_signed is derived from @c true_type and @c is_signed_v is @c true for @c const signed types
TYPED_TEST(IsSignedTest, ConstSignedTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_signed<TypeParam const>, std::true_type>);
  ASSERT_TRUE(std::is_signed_v<TypeParam const>);
}

/// @test @c is_signed is derived from @c true_type and @c is_signed_v is @c true for @c volatile signed types
TYPED_TEST(IsSignedTest, VolatileSignedTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_signed<TypeParam volatile>, std::true_type>);
  ASSERT_TRUE(std::is_signed_v<TypeParam volatile>);
}

/// @test @c is_signed is derived from @c true_type and @c is_signed_v is @c true for @c const @c volatile signed types
TYPED_TEST(IsSignedTest, ConstVolatileSignedTypesYieldTrue) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_signed<TypeParam const volatile>, std::true_type>);
  ASSERT_TRUE(std::is_signed_v<TypeParam const volatile>);
}

template <typename T>
class IsNonSignedTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonSignedTest, non_signed_types, );

/// @test @c is_signed is derived from @c false_type and @c is_signed_v is @c false for types that are not signed types
TYPED_TEST(IsNonSignedTest, NonSignedTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_signed<TypeParam>, std::false_type>);
  ASSERT_FALSE(std::is_signed_v<TypeParam>);
}

/// @test @c is_signed is derived from @c false_type and @c is_signed_v is @c false for @c const types that are not
/// signed types
TYPED_TEST(IsNonSignedTest, ConstNonSignedTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_signed<TypeParam const>, std::false_type>);
  ASSERT_FALSE(std::is_signed_v<TypeParam const>);
}

/// @test @c is_signed is derived from @c false_type and @c is_signed_v is @c false for @c volatile types that are not
/// signed types
TYPED_TEST(IsNonSignedTest, VolatileNonSignedTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_signed<TypeParam volatile>, std::false_type>);
  ASSERT_FALSE(std::is_signed_v<TypeParam volatile>);
}

/// @test @c is_signed is derived from @c false_type and @c is_signed_v is @c false for @c const @c volatile types that
/// are not signed types
TYPED_TEST(IsNonSignedTest, ConstVolatileNonSignedTypesYieldFalse) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_signed<TypeParam const volatile>, std::false_type>);
  ASSERT_FALSE(std::is_signed_v<TypeParam const volatile>);
}

/// @test @c char may or may not be signed, but @c is_signed and @c is_signed_v give correct results
TEST(IsSignedTest, CorrectResultsForChar) {
  constexpr bool char_is_signed = static_cast<char>(static_cast<char>(0) - static_cast<char>(1)) < static_cast<char>(0);
  ASSERT_EQ((is_unambiguously_publicly_derived_from_v<std::is_signed<char>, std::true_type>), char_is_signed);
  ASSERT_EQ(std::is_signed_v<char>, char_is_signed);
}

/// @test @c wchar_t may or may not be signed, but @c is_signed and @c is_signed_v give correct results
TEST(IsSignedTest, CorrectResultsForWcharT) {
  constexpr bool wchar_t_is_signed =
      static_cast<wchar_t>(static_cast<wchar_t>(0) - static_cast<wchar_t>(1)) < static_cast<wchar_t>(0);
  ASSERT_EQ((is_unambiguously_publicly_derived_from_v<std::is_signed<wchar_t>, std::true_type>), wchar_t_is_signed);
  ASSERT_EQ(std::is_signed_v<wchar_t>, wchar_t_is_signed);
}

}  // namespace
