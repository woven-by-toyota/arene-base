// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
namespace {

using testing::is_unambiguously_publicly_derived_from_v;

class some_user_type {};

using self_convertible_types = ::testing::Types<
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
class SelfConvertibleTest : public testing::Test {};

TYPED_TEST_SUITE(SelfConvertibleTest, self_convertible_types, );

/// @test A type is convertible to itself
TYPED_TEST(SelfConvertibleTest, IsConvertibleToSelf) {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<TypeParam, TypeParam>, std::true_type>,
      "A type is convertible to itself"
  );
}

/// @test A type is not convertible to an unrelated type
TYPED_TEST(SelfConvertibleTest, IsNotConvertibleToUnrelated) {
  struct unrelated {};
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<TypeParam, unrelated>, std::false_type>,
      "A type is not convertible to an unrelated type"
  );
}

/// @test An lvalue reference is convertible to a @c const lvalue reference
TYPED_TEST(SelfConvertibleTest, LvalueRefIsConvertibleToConstLValueRef) {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<TypeParam&, TypeParam const&>, std::true_type>,
      "An lvalue reference is convertible to a const lvalue reference"
  );
}

/// @test A @c const lvalue reference is not convertible to a non-const lvalue reference
TYPED_TEST(SelfConvertibleTest, ConstLvalueRefIsNotConvertibleToLValueRef) {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<TypeParam const&, TypeParam&>, std::false_type>,
      "A const lvalue reference is not convertible to a lvalue reference"
  );
}

/// @test Non-@c void types are not convertible to @c void
TYPED_TEST(SelfConvertibleTest, TypeIsNotConvertibleToVoid) {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<TypeParam, void>, std::false_type>,
      "A value is not implicitly convertible to void"
  );
}

/// @test Non-@c void types are not convertible to @c const @c void
TYPED_TEST(SelfConvertibleTest, TypeIsNotConvertibleToConstVoid) {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<TypeParam, void const>, std::false_type>,
      "A value is not convertible to const void"
  );
}

/// @test A type is convertible to itself
TYPED_TEST(SelfConvertibleTest, ValueTraitIsConvertibleToSelf) {
  static_assert(std::is_convertible_v<TypeParam, TypeParam>, "A type is convertible to itself");
}

/// @test A type is not convertible to an unrelated type
TYPED_TEST(SelfConvertibleTest, ValueTraitIsNotConvertibleToUnrelated) {
  struct unrelated {};
  static_assert(!std::is_convertible_v<TypeParam, unrelated>, "A type is not convertible to an unrelated type");
}

/// @test An lvalue reference is convertible to a @c const lvalue reference
TYPED_TEST(SelfConvertibleTest, ValueTraitLvalueRefIsConvertibleToConstLValueRef) {
  static_assert(
      std::is_convertible_v<TypeParam&, TypeParam const&>,
      "An lvalue reference is convertible to a const lvalue reference"
  );
}

/// @test A @c const lvalue reference is not convertible to a non-const lvalue reference
TYPED_TEST(SelfConvertibleTest, ValueTraitConstLvalueRefIsNotConvertibleToLValueRef) {
  static_assert(
      !std::is_convertible_v<TypeParam const&, TypeParam&>,
      "A const lvalue reference is not convertible to a lvalue reference"
  );
}

/// @test All types are convertible to @c void
TYPED_TEST(SelfConvertibleTest, ValueTraitTypeIsNotConvertibleToVoid) {
  static_assert(!std::is_convertible_v<TypeParam, void>, "A value is not convertible to void");
}

/// @test All types are convertible to @c const @c void
TYPED_TEST(SelfConvertibleTest, ValueTraitTypeIsNotConvertibleToConstVoid) {
  static_assert(!std::is_convertible_v<TypeParam, void const>, "A value is not convertible to const void");
}

class base {};
class derived : public base {};

/// @test A derived class is convertible to a base class
TEST(Convertible, DerivedIsConvertibleToBase) {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<derived, base>, std::true_type>,
      "A derived class is convertible to base"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<derived&, base&>, std::true_type>,
      "A derived class is convertible to base"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<derived const&, base const&>, std::true_type>,
      "A derived class is convertible to base"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<derived const&, base&>, std::false_type>,
      "A const derived class is not convertible to base reference"
  );
}

/// @test A base class is not convertible to a derived class
TEST(Convertible, BaseIsNotConvertibleToDerived) {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<base, derived>, std::false_type>,
      "A base class is not convertible to a derived class"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<base&, derived&>, std::false_type>,
      "A base class is not convertible to a derived class"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<base const&, derived const&>, std::false_type>,
      "A base class is not convertible to a derived class"
  );
}

/// @test A derived class is convertible to a base class
TEST(ConvertibleV, DerivedIsConvertibleToBase) {
  static_assert(std::is_convertible_v<derived, base>, "A derived class is convertible to base");
  static_assert(std::is_convertible_v<derived&, base&>, "A derived class is convertible to base");
  static_assert(std::is_convertible_v<derived const&, base const&>, "A derived class is convertible to base");
  static_assert(
      !std::is_convertible_v<derived const&, base&>,
      "A const derived class is not convertible to base reference"
  );
}

/// @test A base class is not convertible to a derived class
TEST(ConvertibleV, BaseIsNotConvertibleToDerived) {
  static_assert(!std::is_convertible_v<base, derived>, "A base class is not convertible to a derived class");
  static_assert(!std::is_convertible_v<base&, derived&>, "A base class is not convertible to a derived class");
  static_assert(
      !std::is_convertible_v<base const&, derived const&>,
      "A base class is not convertible to a derived class"
  );
}

class implicit_source {};
class explicit_source {};
class target {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  target(implicit_source);
  explicit target(explicit_source);
};

class convertible_source {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator target() const;
};

class explicit_convertible_source {
 public:
  explicit operator target() const;
};

/// @test a source class is convertible to target class if there is a non-explicit constructor or conversion operator
TEST(Convertible, ImplicitConversions) {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<implicit_source, target>, std::true_type>,
      "A class is convertible to another with an implicit constructor"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<explicit_source, target>, std::false_type>,
      "A class is not convertible to another with an explicit constructor"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<convertible_source, target>, std::true_type>,
      "A class is convertible to another with an implicit conversion operator"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<
          std::is_convertible<explicit_convertible_source, target>,
          std::false_type>,
      "A class is not convertible to another with an explicit conversion operator"
  );
}

/// @test a source class is convertible to target class if there is a non-explicit constructor or conversion operator
TEST(ConvertibleV, ImplicitConversions) {
  static_assert(
      std::is_convertible_v<implicit_source, target>,
      "A class is convertible to another with an implicit constructor"
  );
  static_assert(
      !std::is_convertible_v<explicit_source, target>,
      "A class is not convertible to another with an explicit constructor"
  );
  static_assert(
      std::is_convertible_v<convertible_source, target>,
      "A class is convertible to another with an implicit conversion operator"
  );
  static_assert(
      !std::is_convertible_v<explicit_convertible_source, target>,
      "A class is not convertible to another with an explicit conversion operator"
  );
}

/// @test cv-qualified @c void is convertible to other-cv-qualified @c void if qualifiers are added or the same.
TEST(Convertible, VoidConversions) {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void, void>, std::true_type>,
      "void can convert to void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void, void const>, std::true_type>,
      "void can convert to const void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void, void volatile>, std::true_type>,
      "void can convert to volatile void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void, void const volatile>, std::true_type>,
      "void can convert to const volatile void"
  );

  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void const, void>, std::false_type>,
      "const void cannot convert to void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void const, void const>, std::true_type>,
      "const void can convert to const void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void const, void volatile>, std::false_type>,
      "const void cannot convert to volatile void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void const, void const volatile>, std::true_type>,
      "const void can convert to const volatile void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void volatile, void>, std::false_type>,
      "volatile void cannot convert to void"
  );

  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void volatile, void const>, std::false_type>,
      "volatile void cannot convert to const void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void volatile, void volatile>, std::true_type>,
      "volatile void can convert to volatile void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void volatile, void const volatile>, std::true_type>,
      "volatile void can convert to const volatile void"
  );

  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void const volatile, void>, std::false_type>,
      "const volatile void cannot convert to void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_convertible<void const volatile, void const>, std::false_type>,
      "const volatile void cannot convert to const void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<
          std::is_convertible<void const volatile, void volatile>,
          std::false_type>,
      "const volatile void cannot convert to volatile void"
  );
  static_assert(
      is_unambiguously_publicly_derived_from_v<
          std::is_convertible<void const volatile, void const volatile>,
          std::true_type>,
      "const volatile void can convert to const volatile void"
  );
}

/// @test cv-qualified @c void is convertible to other-cv-qualified @c void if qualifiers are added or the same.
TEST(ConvertibleV, VoidConversions) {
  static_assert(std::is_convertible_v<void, void>, "void can convert to void");
  static_assert(std::is_convertible_v<void, void const>, "void can convert to const void");
  static_assert(std::is_convertible_v<void, void volatile>, "void can convert to volatile void");
  static_assert(std::is_convertible_v<void, void const volatile>, "void can convert to const volatile void");

  static_assert(!std::is_convertible_v<void const, void>, "const void cannot convert to void");
  static_assert(std::is_convertible_v<void const, void const>, "const void can convert to const void");
  static_assert(!std::is_convertible_v<void const, void volatile>, "const void cannot convert to volatile void");
  static_assert(
      std::is_convertible_v<void const, void const volatile>,
      "const void can convert to const volatile void"
  );
  static_assert(!std::is_convertible_v<void volatile, void>, "volatile void cannot convert to void");

  static_assert(!std::is_convertible_v<void volatile, void const>, "volatile void cannot convert to const void");
  static_assert(std::is_convertible_v<void volatile, void volatile>, "volatile void can convert to volatile void");
  static_assert(
      std::is_convertible_v<void volatile, void const volatile>,
      "volatile void can convert to const volatile void"
  );

  static_assert(!std::is_convertible_v<void const volatile, void>, "const volatile void cannot convert to void");
  static_assert(
      !std::is_convertible_v<void const volatile, void const>,
      "const volatile void cannot convert to const void"
  );
  static_assert(
      !std::is_convertible_v<void const volatile, void volatile>,
      "const volatile void cannot convert to volatile void"
  );
  static_assert(
      std::is_convertible_v<void const volatile, void const volatile>,
      "const volatile void can convert to const volatile void"
  );
}

}  // namespace
