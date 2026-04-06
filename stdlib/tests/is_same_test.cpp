// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A simple user-defined type
class some_user_type {};

using is_same_types = ::testing::Types<
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
    float,
    double,
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
class IsSameTest : public testing::Test {};

TYPED_TEST_SUITE(IsSameTest, is_same_types, );

/// @test The type of @c is_same is derived from @c true_type for identical types
TYPED_TEST(IsSameTest, SameTypeSameCvRefIsTrueType) {
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam, TypeParam>, std::true_type>));
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const, TypeParam const>, std::true_type>)
  );
  ASSERT_TRUE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam volatile, TypeParam volatile>, std::true_type>)
  );
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<
               std::is_same<TypeParam const volatile, TypeParam const volatile>,
               std::true_type>));

  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam&, TypeParam&>, std::true_type>));
  ASSERT_TRUE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const&, TypeParam const&>, std::true_type>)
  );
  ASSERT_TRUE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam volatile&, TypeParam volatile&>, std::true_type>)
  );
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<
               std::is_same<TypeParam const volatile&, TypeParam const volatile&>,
               std::true_type>));

  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam&&, TypeParam&&>, std::true_type>));
  ASSERT_TRUE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const&&, TypeParam const&&>, std::true_type>)
  );
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<
               std::is_same<TypeParam volatile&&, TypeParam volatile&&>,
               std::true_type>));
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<
               std::is_same<TypeParam const volatile&&, TypeParam const volatile&&>,
               std::true_type>));
}

/// @test The type of @c is_same is not derived from @c false_type for identical types
TYPED_TEST(IsSameTest, SameTypeSameCvRefIsFalseType) {
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam, TypeParam>, std::false_type>));
  ASSERT_FALSE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const, TypeParam const>, std::false_type>)
  );
  ASSERT_FALSE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam volatile, TypeParam volatile>, std::false_type>)
  );
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<
                std::is_same<TypeParam const volatile, TypeParam const volatile>,
                std::false_type>));

  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam&, TypeParam&>, std::false_type>));
  ASSERT_FALSE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const&, TypeParam const&>, std::false_type>)
  );
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<
                std::is_same<TypeParam volatile&, TypeParam volatile&>,
                std::false_type>));
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<
                std::is_same<TypeParam const volatile&, TypeParam const volatile&>,
                std::false_type>));

  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam&&, TypeParam&&>, std::false_type>));
  ASSERT_FALSE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const&&, TypeParam const&&>, std::false_type>)
  );
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<
                std::is_same<TypeParam volatile&&, TypeParam volatile&&>,
                std::false_type>));
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<
                std::is_same<TypeParam const volatile&&, TypeParam const volatile&&>,
                std::false_type>));
}

/// @test The type of @c is_same is derived from @c false_type if the types have different qualifications
TYPED_TEST(IsSameTest, SameTypeDifferentCvIsFalseType) {
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam, TypeParam const>, std::false_type>));
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam, TypeParam volatile>, std::false_type>));
  ASSERT_TRUE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam, TypeParam const volatile>, std::false_type>)
  );

  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const, TypeParam>, std::false_type>));
  ASSERT_TRUE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const, TypeParam volatile>, std::false_type>)
  );
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<
               std::is_same<TypeParam const, TypeParam const volatile>,
               std::false_type>));

  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam volatile, TypeParam>, std::false_type>));
  ASSERT_TRUE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam volatile, TypeParam const>, std::false_type>)
  );
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<
               std::is_same<TypeParam volatile, TypeParam const volatile>,
               std::false_type>));

  ASSERT_TRUE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const volatile, TypeParam>, std::false_type>)
  );
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<
               std::is_same<TypeParam const volatile, TypeParam const>,
               std::false_type>));
  ASSERT_TRUE((is_unambiguously_publicly_derived_from_v<
               std::is_same<TypeParam const volatile, TypeParam volatile>,
               std::false_type>));
}

/// @test The type of @c is_same is not derived from @c true_type if the types have different qualifications
TYPED_TEST(IsSameTest, SameTypeDifferentCvIsTrueType) {
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam, TypeParam const>, std::true_type>));
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam, TypeParam volatile>, std::true_type>));
  ASSERT_FALSE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam, TypeParam const volatile>, std::true_type>)
  );

  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const, TypeParam>, std::true_type>));
  ASSERT_FALSE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const, TypeParam volatile>, std::true_type>)
  );
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<
                std::is_same<TypeParam const, TypeParam const volatile>,
                std::true_type>));

  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam volatile, TypeParam>, std::true_type>));
  ASSERT_FALSE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam volatile, TypeParam const>, std::true_type>)
  );
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<
                std::is_same<TypeParam volatile, TypeParam const volatile>,
                std::true_type>));

  ASSERT_FALSE(
      (is_unambiguously_publicly_derived_from_v<std::is_same<TypeParam const volatile, TypeParam>, std::true_type>)
  );
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<
                std::is_same<TypeParam const volatile, TypeParam const>,
                std::true_type>));
  ASSERT_FALSE((is_unambiguously_publicly_derived_from_v<
                std::is_same<TypeParam const volatile, TypeParam volatile>,
                std::true_type>));
}

/// @test @c is_same is true for identical local types, false for types that don't match in cv-qualification, base type,
/// pointerness or referenceness
TEST(IsSame, WorksForLocalType) {
  struct local1 {};
  struct local2 {};
  ASSERT_TRUE((std::is_same<local1, local1>::value));
  ASSERT_FALSE((std::is_same<local1, local2>::value));
  ASSERT_FALSE((std::is_same<local2, local1>::value));
  ASSERT_TRUE((std::is_same<local1 const, local1 const>::value));
  ASSERT_FALSE((std::is_same<local1 const, local1>::value));
  ASSERT_FALSE((std::is_same<local1, local1 const>::value));
  ASSERT_TRUE((std::is_same<local1 volatile, local1 volatile>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1>::value));
  ASSERT_FALSE((std::is_same<local1, local1 volatile>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1 const>::value));
  ASSERT_FALSE((std::is_same<local1 const, local1 volatile>::value));
  ASSERT_TRUE((std::is_same<local1 const volatile, local1 const volatile>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1 const volatile>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile, local1 volatile>::value));

  ASSERT_FALSE((std::is_same<local1&, local1>::value));
  ASSERT_FALSE((std::is_same<local1&, local2>::value));
  ASSERT_FALSE((std::is_same<local2&, local1>::value));
  ASSERT_FALSE((std::is_same<local1 const&, local1 const>::value));
  ASSERT_FALSE((std::is_same<local1 const&, local1>::value));
  ASSERT_FALSE((std::is_same<local1&, local1 const>::value));
  ASSERT_FALSE((std::is_same<local1 volatile&, local1 volatile>::value));
  ASSERT_FALSE((std::is_same<local1 volatile&, local1>::value));
  ASSERT_FALSE((std::is_same<local1&, local1 volatile>::value));
  ASSERT_FALSE((std::is_same<local1 volatile&, local1 const>::value));
  ASSERT_FALSE((std::is_same<local1 const&, local1 volatile>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile&, local1 const volatile>::value));
  ASSERT_FALSE((std::is_same<local1 volatile&, local1 const volatile>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile&, local1 volatile>::value));

  ASSERT_FALSE((std::is_same<local1, local1&>::value));
  ASSERT_FALSE((std::is_same<local1, local2&>::value));
  ASSERT_FALSE((std::is_same<local2, local1&>::value));
  ASSERT_FALSE((std::is_same<local1 const, local1 const&>::value));
  ASSERT_FALSE((std::is_same<local1 const, local1&>::value));
  ASSERT_FALSE((std::is_same<local1, local1 const&>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1 volatile&>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1&>::value));
  ASSERT_FALSE((std::is_same<local1, local1 volatile&>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1 const&>::value));
  ASSERT_FALSE((std::is_same<local1 const, local1 volatile&>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile, local1 const volatile&>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1 const volatile&>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile, local1 volatile&>::value));

  ASSERT_TRUE((std::is_same<local1&, local1&>::value));
  ASSERT_FALSE((std::is_same<local1&, local2&>::value));
  ASSERT_FALSE((std::is_same<local2&, local1&>::value));
  ASSERT_TRUE((std::is_same<local1 const&, local1 const&>::value));
  ASSERT_FALSE((std::is_same<local1 const&, local1&>::value));
  ASSERT_FALSE((std::is_same<local1&, local1 const&>::value));
  ASSERT_TRUE((std::is_same<local1 volatile&, local1 volatile&>::value));
  ASSERT_FALSE((std::is_same<local1 volatile&, local1&>::value));
  ASSERT_FALSE((std::is_same<local1&, local1 volatile&>::value));
  ASSERT_FALSE((std::is_same<local1 volatile&, local1 const&>::value));
  ASSERT_FALSE((std::is_same<local1 const&, local1 volatile&>::value));
  ASSERT_TRUE((std::is_same<local1 const volatile&, local1 const volatile&>::value));
  ASSERT_FALSE((std::is_same<local1 volatile&, local1 const volatile&>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile&, local1 volatile&>::value));

  ASSERT_FALSE((std::is_same<local1*, local1>::value));
  ASSERT_FALSE((std::is_same<local1*, local2>::value));
  ASSERT_FALSE((std::is_same<local2*, local1>::value));
  ASSERT_FALSE((std::is_same<local1 const*, local1 const>::value));
  ASSERT_FALSE((std::is_same<local1 const*, local1>::value));
  ASSERT_FALSE((std::is_same<local1*, local1 const>::value));
  ASSERT_FALSE((std::is_same<local1 volatile*, local1 volatile>::value));
  ASSERT_FALSE((std::is_same<local1 volatile*, local1>::value));
  ASSERT_FALSE((std::is_same<local1*, local1 volatile>::value));
  ASSERT_FALSE((std::is_same<local1 volatile*, local1 const>::value));
  ASSERT_FALSE((std::is_same<local1 const*, local1 volatile>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile*, local1 const volatile>::value));
  ASSERT_FALSE((std::is_same<local1 volatile*, local1 const volatile>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile*, local1 volatile>::value));

  ASSERT_FALSE((std::is_same<local1, local1*>::value));
  ASSERT_FALSE((std::is_same<local1, local2*>::value));
  ASSERT_FALSE((std::is_same<local2, local1*>::value));
  ASSERT_FALSE((std::is_same<local1 const, local1 const*>::value));
  ASSERT_FALSE((std::is_same<local1 const, local1*>::value));
  ASSERT_FALSE((std::is_same<local1, local1 const*>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1 volatile*>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1*>::value));
  ASSERT_FALSE((std::is_same<local1, local1 volatile*>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1 const*>::value));
  ASSERT_FALSE((std::is_same<local1 const, local1 volatile*>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile, local1 const volatile*>::value));
  ASSERT_FALSE((std::is_same<local1 volatile, local1 const volatile*>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile, local1 volatile*>::value));

  ASSERT_TRUE((std::is_same<local1*, local1*>::value));
  ASSERT_FALSE((std::is_same<local1*, local2*>::value));
  ASSERT_FALSE((std::is_same<local2*, local1*>::value));
  ASSERT_TRUE((std::is_same<local1 const*, local1 const*>::value));
  ASSERT_FALSE((std::is_same<local1 const*, local1*>::value));
  ASSERT_FALSE((std::is_same<local1*, local1 const*>::value));
  ASSERT_TRUE((std::is_same<local1 volatile*, local1 volatile*>::value));
  ASSERT_FALSE((std::is_same<local1 volatile*, local1*>::value));
  ASSERT_FALSE((std::is_same<local1*, local1 volatile*>::value));
  ASSERT_FALSE((std::is_same<local1 volatile*, local1 const*>::value));
  ASSERT_FALSE((std::is_same<local1 const*, local1 volatile*>::value));
  ASSERT_TRUE((std::is_same<local1 const volatile*, local1 const volatile*>::value));
  ASSERT_FALSE((std::is_same<local1 volatile*, local1 const volatile*>::value));
  ASSERT_FALSE((std::is_same<local1 const volatile*, local1 volatile*>::value));
}

/// @test @c is_same_v is true for identical local types, false for types that don't match in cv-qualification, base
/// type, pointerness or referenceness
TEST(IsSameV, WorksForLocalType) {
  struct local1 {};
  struct local2 {};
  ASSERT_TRUE((std::is_same_v<local1, local1>));
  ASSERT_FALSE((std::is_same_v<local1, local2>));
  ASSERT_FALSE((std::is_same_v<local2, local1>));
  ASSERT_TRUE((std::is_same_v<local1 const, local1 const>));
  ASSERT_FALSE((std::is_same_v<local1 const, local1>));
  ASSERT_FALSE((std::is_same_v<local1, local1 const>));
  ASSERT_TRUE((std::is_same_v<local1 volatile, local1 volatile>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1>));
  ASSERT_FALSE((std::is_same_v<local1, local1 volatile>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1 const>));
  ASSERT_FALSE((std::is_same_v<local1 const, local1 volatile>));
  ASSERT_TRUE((std::is_same_v<local1 const volatile, local1 const volatile>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1 const volatile>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile, local1 volatile>));

  ASSERT_FALSE((std::is_same_v<local1&, local1>));
  ASSERT_FALSE((std::is_same_v<local1&, local2>));
  ASSERT_FALSE((std::is_same_v<local2&, local1>));
  ASSERT_FALSE((std::is_same_v<local1 const&, local1 const>));
  ASSERT_FALSE((std::is_same_v<local1 const&, local1>));
  ASSERT_FALSE((std::is_same_v<local1&, local1 const>));
  ASSERT_FALSE((std::is_same_v<local1 volatile&, local1 volatile>));
  ASSERT_FALSE((std::is_same_v<local1 volatile&, local1>));
  ASSERT_FALSE((std::is_same_v<local1&, local1 volatile>));
  ASSERT_FALSE((std::is_same_v<local1 volatile&, local1 const>));
  ASSERT_FALSE((std::is_same_v<local1 const&, local1 volatile>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile&, local1 const volatile>));
  ASSERT_FALSE((std::is_same_v<local1 volatile&, local1 const volatile>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile&, local1 volatile>));

  ASSERT_FALSE((std::is_same_v<local1, local1&>));
  ASSERT_FALSE((std::is_same_v<local1, local2&>));
  ASSERT_FALSE((std::is_same_v<local2, local1&>));
  ASSERT_FALSE((std::is_same_v<local1 const, local1 const&>));
  ASSERT_FALSE((std::is_same_v<local1 const, local1&>));
  ASSERT_FALSE((std::is_same_v<local1, local1 const&>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1 volatile&>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1&>));
  ASSERT_FALSE((std::is_same_v<local1, local1 volatile&>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1 const&>));
  ASSERT_FALSE((std::is_same_v<local1 const, local1 volatile&>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile, local1 const volatile&>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1 const volatile&>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile, local1 volatile&>));

  ASSERT_TRUE((std::is_same_v<local1&, local1&>));
  ASSERT_FALSE((std::is_same_v<local1&, local2&>));
  ASSERT_FALSE((std::is_same_v<local2&, local1&>));
  ASSERT_TRUE((std::is_same_v<local1 const&, local1 const&>));
  ASSERT_FALSE((std::is_same_v<local1 const&, local1&>));
  ASSERT_FALSE((std::is_same_v<local1&, local1 const&>));
  ASSERT_TRUE((std::is_same_v<local1 volatile&, local1 volatile&>));
  ASSERT_FALSE((std::is_same_v<local1 volatile&, local1&>));
  ASSERT_FALSE((std::is_same_v<local1&, local1 volatile&>));
  ASSERT_FALSE((std::is_same_v<local1 volatile&, local1 const&>));
  ASSERT_FALSE((std::is_same_v<local1 const&, local1 volatile&>));
  ASSERT_TRUE((std::is_same_v<local1 const volatile&, local1 const volatile&>));
  ASSERT_FALSE((std::is_same_v<local1 volatile&, local1 const volatile&>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile&, local1 volatile&>));

  ASSERT_FALSE((std::is_same_v<local1*, local1>));
  ASSERT_FALSE((std::is_same_v<local1*, local2>));
  ASSERT_FALSE((std::is_same_v<local2*, local1>));
  ASSERT_FALSE((std::is_same_v<local1 const*, local1 const>));
  ASSERT_FALSE((std::is_same_v<local1 const*, local1>));
  ASSERT_FALSE((std::is_same_v<local1*, local1 const>));
  ASSERT_FALSE((std::is_same_v<local1 volatile*, local1 volatile>));
  ASSERT_FALSE((std::is_same_v<local1 volatile*, local1>));
  ASSERT_FALSE((std::is_same_v<local1*, local1 volatile>));
  ASSERT_FALSE((std::is_same_v<local1 volatile*, local1 const>));
  ASSERT_FALSE((std::is_same_v<local1 const*, local1 volatile>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile*, local1 const volatile>));
  ASSERT_FALSE((std::is_same_v<local1 volatile*, local1 const volatile>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile*, local1 volatile>));

  ASSERT_FALSE((std::is_same_v<local1, local1*>));
  ASSERT_FALSE((std::is_same_v<local1, local2*>));
  ASSERT_FALSE((std::is_same_v<local2, local1*>));
  ASSERT_FALSE((std::is_same_v<local1 const, local1 const*>));
  ASSERT_FALSE((std::is_same_v<local1 const, local1*>));
  ASSERT_FALSE((std::is_same_v<local1, local1 const*>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1 volatile*>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1*>));
  ASSERT_FALSE((std::is_same_v<local1, local1 volatile*>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1 const*>));
  ASSERT_FALSE((std::is_same_v<local1 const, local1 volatile*>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile, local1 const volatile*>));
  ASSERT_FALSE((std::is_same_v<local1 volatile, local1 const volatile*>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile, local1 volatile*>));

  ASSERT_TRUE((std::is_same_v<local1*, local1*>));
  ASSERT_FALSE((std::is_same_v<local1*, local2*>));
  ASSERT_FALSE((std::is_same_v<local2*, local1*>));
  ASSERT_TRUE((std::is_same_v<local1 const*, local1 const*>));
  ASSERT_FALSE((std::is_same_v<local1 const*, local1*>));
  ASSERT_FALSE((std::is_same_v<local1*, local1 const*>));
  ASSERT_TRUE((std::is_same_v<local1 volatile*, local1 volatile*>));
  ASSERT_FALSE((std::is_same_v<local1 volatile*, local1*>));
  ASSERT_FALSE((std::is_same_v<local1*, local1 volatile*>));
  ASSERT_FALSE((std::is_same_v<local1 volatile*, local1 const*>));
  ASSERT_FALSE((std::is_same_v<local1 const*, local1 volatile*>));
  ASSERT_TRUE((std::is_same_v<local1 const volatile*, local1 const volatile*>));
  ASSERT_FALSE((std::is_same_v<local1 volatile*, local1 const volatile*>));
  ASSERT_FALSE((std::is_same_v<local1 const volatile*, local1 volatile*>));
}
}  // namespace
