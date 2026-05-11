// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
namespace {

using testing::is_unambiguously_publicly_derived_from_v;

template <bool Expected, class To, class From>
constexpr auto check_is_assignable() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_assignable<To, From>, std::bool_constant<Expected>> &&
         (std::is_assignable_v<To, From> == Expected);
}

template <bool Expected, class To, class From>
constexpr auto check_is_nothrow_assignable() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_nothrow_assignable<To, From>, std::bool_constant<Expected>> &&
         (std::is_nothrow_assignable_v<To, From> == Expected);
}

class some_user_type {};

using self_assignable_types = ::testing::Types<
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
class AssignableTest : public testing::Test {};

TYPED_TEST_SUITE(AssignableTest, self_assignable_types, );

/// @test A type is assignable to itself
TYPED_TEST(AssignableTest, IsAssignableFromSelf) {
  static_assert(check_is_assignable<true, TypeParam&, TypeParam>(), "A type is assignable from itself");
}

/// @test A type is not assignable from an unrelated type
TYPED_TEST(AssignableTest, IsNotAssignableFromUnrelated) {
  struct unrelated {};
  static_assert(check_is_assignable<false, TypeParam&, unrelated>(), "A type is not assignable from an unrelated type");
}

/// @test A @c const lvalue reference is not assignable from a non-@c const lvalue reference
TYPED_TEST(AssignableTest, ConstLvalueRefIsNotAssignableFromLValueRef) {
  static_assert(
      check_is_assignable<false, TypeParam const&, TypeParam&>(),
      "A const lvalue reference is not assignable from an lvalue reference"
  );
}

/// @test A non-@c const lvalue reference is assignable from a const lvalue reference
TYPED_TEST(AssignableTest, LvalueRefIsAssignableFromConstLValueRef) {
  static_assert(
      check_is_assignable<true, TypeParam&, TypeParam const&>(),
      "An lvalue reference is assignable from a const lvalue reference"
  );
}

/// @test A non-@c const lvalue reference is assignable from a non-@c const lvalue reference
TYPED_TEST(AssignableTest, LvalueRefIsAssignableFromLValueRef) {
  static_assert(
      check_is_assignable<true, TypeParam&, TypeParam&>(),
      "An lvalue reference is assignable from an lvalue reference"
  );
}

/// @test A non-@c const lvalue reference is assignable from an rvalue reference
TYPED_TEST(AssignableTest, LvalueRefIsAssignableFromRValueRef) {
  static_assert(
      check_is_assignable<true, TypeParam&, TypeParam&&>(),
      "An lvalue reference is assignable from an rvalue reference"
  );
}

/// @test A @c const rvalue reference is not assignable from a non-@c const rvalue reference
TYPED_TEST(AssignableTest, ConstRvalueRefIsNotAssignableFromRvalueRef) {
  static_assert(
      check_is_assignable<false, TypeParam const&&, TypeParam&&>(),
      "A const rvalue reference is not assignable from an rvalue reference"
  );
}

/// @test Non-@c void types are not assignable from @c void
TYPED_TEST(AssignableTest, TypeIsNotAssignableFromVoid) {
  static_assert(check_is_assignable<false, TypeParam&, void>(), "A value is not assignable from void");
}

/// @test Non-@c void types are not assignable from @c const @c void
TYPED_TEST(AssignableTest, TypeIsNotAssignableFromConstVoid) {
  static_assert(check_is_assignable<false, TypeParam&, void const>(), "A value is not assignable from const void");
}

/// @test @c void cannot be assigned to
TYPED_TEST(AssignableTest, VoidCannotBeAssignedTo) {
  static_assert(check_is_assignable<false, void, TypeParam>(), "A value is not assignable from void");
}

class implicit_source {};
class explicit_source {};

class assign_source {};

class target {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  target(implicit_source) noexcept;
  explicit target(explicit_source) noexcept;

  auto operator=(assign_source const&) noexcept -> target&;
};

class convertible_source {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator target() const noexcept;
};

class explicit_convertible_source {
 public:
  explicit operator target() const noexcept;
};

/// @test Instances can be assigned to where there is an assignment operator that takes the right type
TEST(Assignable, ExplicitAssignmentOperator) {
  static_assert(
      check_is_assignable<true, target&, assign_source>(),
      "A class is assignable from another when there is an assignment operator"
  );
}

/// @test Instances can be assigned to where there is an implicit conversion from the source to the target
TEST(Assignable, ImplicitConversions) {
  static_assert(
      check_is_assignable<true, target&, implicit_source>(),
      "A class is assignable from another with an implicit constructor"
  );
  static_assert(
      check_is_assignable<false, target&, explicit_source>(),
      "A class is not assignable from another with an explicit constructor"
  );
  static_assert(
      check_is_assignable<true, target&, convertible_source>(),
      "A class is assignable from another with an implicit conversion operator"
  );
  static_assert(
      check_is_assignable<false, target&, explicit_convertible_source>(),
      "A class is not assignable from another with an explicit conversion operator"
  );
}

class assignment_takes_implicit_conversion_target {
 public:
  auto operator=(target const&) noexcept -> assignment_takes_implicit_conversion_target&;
};

/// @test Assignments from a type that implicit converts to the assignment arg is allowed
TEST(Assignable, ImplicitConversionsToAssignmentArg) {
  static_assert(
      check_is_assignable<true, assignment_takes_implicit_conversion_target&, convertible_source>(),
      "A class is assignable from another with an implicit conversion operator"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class no_copy_or_move {
 public:
  auto operator=(no_copy_or_move const&) -> no_copy_or_move& = delete;
  auto operator=(no_copy_or_move&&) -> no_copy_or_move& = delete;
};

/// @test A type without copy or move cannot be self-assigned
TEST(Assignable, NoCopyOrMoveAssignment) {
  static_assert(check_is_assignable<false, no_copy_or_move&, no_copy_or_move&>(), "No assign from lvalue ref");

  static_assert(
      check_is_assignable<false, no_copy_or_move&, no_copy_or_move const&>(),
      "No assign from const lvalue ref"
  );

  static_assert(check_is_assignable<false, no_copy_or_move&, no_copy_or_move&&>(), "No assign from rvalue ref");

  static_assert(
      check_is_assignable<false, no_copy_or_move&, no_copy_or_move const&&>(),
      "No assign from const rvalue ref"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class only_copy {
 public:
  auto operator=(only_copy const&) noexcept -> only_copy&;
  auto operator=(only_copy&&) -> only_copy& = delete;
};

/// @test A copy-only type can be assigned from an lvalue reference or a @c const rvalue reference
TEST(Assignable, OnlyCopyAssignment) {
  static_assert(check_is_assignable<true, only_copy&, only_copy&>(), "Can assign from lvalue ref");

  static_assert(check_is_assignable<true, only_copy&, only_copy const&>(), "Can assign from const lvalue ref");

  static_assert(check_is_assignable<false, only_copy&, only_copy&&>(), "No assign from rvalue ref");

  static_assert(
      check_is_assignable<true, only_copy&, only_copy const&&>(),
      "Can assign from const rvalue ref (via decay to const lvalue ref)"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class only_move {
 public:
  auto operator=(only_move const&) noexcept -> only_move& = delete;
  auto operator=(only_move&&) noexcept -> only_move&;
};

/// @test A move-only type can only be assigned from a non-const rvalue reference
TEST(Assignable, OnlyMoveAssignment) {
  static_assert(check_is_assignable<false, only_move&, only_move&>(), "No assign from lvalue ref");

  static_assert(check_is_assignable<false, only_move&, only_move const&>(), "No assign from const lvalue ref");

  static_assert(check_is_assignable<true, only_move&, only_move&&>(), "Can assign from rvalue ref");

  static_assert(
      check_is_assignable<false, only_move&, only_move const&&>(),
      "No assign from const rvalue ref (via decay to const lvalue ref)"
  );
}

/// @test cv-qualified @c void is not assignable from anything.
TEST(Assignable, VoidConversions) {
  static_assert(check_is_assignable<false, void, void>(), "void is not assignable from anything");
  static_assert(check_is_assignable<false, void const, void>(), "void is not assignable from anything");
  static_assert(check_is_assignable<false, void volatile, void>(), "void is not assignable from anything");
  static_assert(check_is_assignable<false, void const volatile, void>(), "void is not assignable from anything");

  static_assert(check_is_assignable<false, void, void const>(), "void is not assignable from anything");
  static_assert(check_is_assignable<false, void const, void const>(), "void is not assignable from anything");
  static_assert(check_is_assignable<false, void volatile, void const>(), "void is not assignable from anything");
  static_assert(check_is_assignable<false, void const volatile, void const>(), "void is not assignable from anything");

  static_assert(check_is_assignable<false, void, void volatile>(), "void is not assignable from anything");
  static_assert(check_is_assignable<false, void const, void volatile>(), "void is not assignable from anything");
  static_assert(check_is_assignable<false, void volatile, void volatile>(), "void is not assignable from anything");
  static_assert(
      check_is_assignable<false, void const volatile, void volatile>(),
      "void is not assignable from anything"
  );

  static_assert(check_is_assignable<false, void, void const volatile>(), "void is not assignable from anything");
  static_assert(check_is_assignable<false, void const, void const volatile>(), "void is not assignable from anything");
  static_assert(
      check_is_assignable<false, void volatile, void const volatile>(),
      "void is not assignable from anything"
  );
  static_assert(
      check_is_assignable<false, void const volatile, void const volatile>(),
      "void is not assignable from anything"
  );
}

class base {};
class derived : public base {};

/// @test Can assign to a base class object from a derived class object
TEST(Assignable, CanAssignDerivedToBase) {
  static_assert(check_is_assignable<true, base, derived>(), "Can assign to base from derived");
}

/// @test Cannot assign to a derived class object from a base class object
TEST(Assignable, CannotAssignBaseToDerived) {
  static_assert(check_is_assignable<false, derived, base>(), "Cannot assign to derived from base");
}

/// @test Can assign to a pointer to base class from a pointer to derived class
TEST(Assignable, CanAssignDerivedPointerToBasePointer) {
  static_assert(check_is_assignable<true, base*&, derived*>(), "Can assign to base pointer from derived pointer");
}

/// @test Cannot assign to a pointer to a derived class from a pointer to a base class
TEST(Assignable, CannotAssignBasePointerToDerivedPointer) {
  static_assert(check_is_assignable<false, derived*&, base*>(), "Cannot assign to derived pointer from base pointer");
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class only_lvalue_assignable {
 public:
  auto operator=(only_lvalue_assignable const&) & noexcept -> only_lvalue_assignable&;
};

/// @test Cannot assign to rvalue if only lvalue assignable
TEST(Assignable, OnlyLValueAssignableNotAssignableFromRvalue) {
  static_assert(
      check_is_assignable<false, only_lvalue_assignable&&, only_lvalue_assignable>(),
      "Cannot assign to rvalue"
  );
}

/// @test Can still assign to lvalue if only lvalue assignable
TEST(Assignable, OnlyLValueAssignableAssignableFromLValue) {
  static_assert(check_is_assignable<true, only_lvalue_assignable&, only_lvalue_assignable>(), "Can assign to lvalue");
}

/// @test A type is assignable to itself
TYPED_TEST(AssignableTest, IsNothrowAssignableFromSelf) {
  static_assert(check_is_nothrow_assignable<true, TypeParam&, TypeParam>(), "A type is assignable from itself");
}

/// @test A type is not assignable from an unrelated type
TYPED_TEST(AssignableTest, IsNotNothrowAssignableFromUnrelated) {
  struct unrelated {};
  static_assert(
      check_is_nothrow_assignable<false, TypeParam&, unrelated>(),
      "A type is not assignable from an unrelated type"
  );
}

/// @test A @c const lvalue reference is not assignable from a non-@c const lvalue reference
TYPED_TEST(AssignableTest, ConstLvalueRefIsNotNothrowAssignableFromLValueRef) {
  static_assert(
      check_is_nothrow_assignable<false, TypeParam const&, TypeParam&>(),
      "A const lvalue reference is not assignable from an lvalue reference"
  );
}

/// @test A non-@c const lvalue reference is assignable from a const lvalue reference
TYPED_TEST(AssignableTest, LvalueRefIsNothrowAssignableFromConstLValueRef) {
  static_assert(
      check_is_nothrow_assignable<true, TypeParam&, TypeParam const&>(),
      "An lvalue reference is assignable from a const lvalue reference"
  );
}

/// @test A non-@c const lvalue reference is assignable from a non-@c const lvalue reference
TYPED_TEST(AssignableTest, LvalueRefIsNothrowAssignableFromLValueRef) {
  static_assert(
      check_is_nothrow_assignable<true, TypeParam&, TypeParam&>(),
      "An lvalue reference is assignable from an lvalue reference"
  );
}

/// @test A non-@c const lvalue reference is assignable from an rvalue reference
TYPED_TEST(AssignableTest, LvalueRefIsNothrowAssignableFromRValueRef) {
  static_assert(
      check_is_nothrow_assignable<true, TypeParam&, TypeParam&&>(),
      "An lvalue reference is assignable from an rvalue reference"
  );
}

/// @test A @c const rvalue reference is not assignable from a non-@c const rvalue reference
TYPED_TEST(AssignableTest, ConstRvalueRefIsNotNothrowAssignableFromRvalueRef) {
  static_assert(
      check_is_nothrow_assignable<false, TypeParam const&&, TypeParam&&>(),
      "A const rvalue reference is not assignable from an rvalue reference"
  );
}

/// @test Non-@c void types are not assignable from @c void
TYPED_TEST(AssignableTest, TypeIsNotNothrowAssignableFromVoid) {
  static_assert(check_is_nothrow_assignable<false, TypeParam&, void>(), "A value is not assignable from void");
}

/// @test Non-@c void types are not assignable from @c const @c void
TYPED_TEST(AssignableTest, TypeIsNotNothrowAssignableFromConstVoid) {
  static_assert(
      check_is_nothrow_assignable<false, TypeParam&, void const>(),
      "A value is not assignable from const void"
  );
}

/// @test @c void cannot be assigned to
TYPED_TEST(AssignableTest, VoidCannotBeAssignedToWithoutThrowing) {
  static_assert(check_is_nothrow_assignable<false, void, TypeParam>(), "A value is not assignable from void");
}

/// @test Instances can be assigned to where there is an assignment operator that takes the right type
TEST(NothrowAssignable, ExplicitAssignmentOperator) {
  static_assert(
      check_is_nothrow_assignable<true, target&, assign_source>(),
      "A class is assignable from another when there is an assignment operator"
  );
}

/// @test Instances can be assigned to where there is an implicit conversion from the source to the target
TEST(NothrowAssignable, ImplicitConversions) {
  static_assert(
      check_is_nothrow_assignable<true, target&, implicit_source>(),
      "A class is assignable from another with an implicit constructor"
  );
  static_assert(
      check_is_nothrow_assignable<false, target&, explicit_source>(),
      "A class is not assignable from another with an explicit constructor"
  );
  static_assert(
      check_is_nothrow_assignable<true, target&, convertible_source>(),
      "A class is assignable from another with an implicit conversion operator"
  );
  static_assert(
      check_is_nothrow_assignable<false, target&, explicit_convertible_source>(),
      "A class is not assignable from another with an explicit conversion operator"
  );
}

/// @test Assignments from a type that implicit converts to the assignment arg is allowed
TEST(NothrowAssignable, ImplicitConversionsToAssignmentArg) {
  static_assert(
      check_is_nothrow_assignable<true, assignment_takes_implicit_conversion_target&, convertible_source>(),
      "A class is assignable from another with an implicit conversion operator"
  );
}

/// @test A type without copy or move cannot be self-assigned
TEST(NothrowAssignable, NoCopyOrMoveAssignment) {
  static_assert(check_is_nothrow_assignable<false, no_copy_or_move&, no_copy_or_move&>(), "No assign from lvalue ref");

  static_assert(
      check_is_nothrow_assignable<false, no_copy_or_move&, no_copy_or_move const&>(),
      "No assign from const lvalue ref"
  );

  static_assert(check_is_nothrow_assignable<false, no_copy_or_move&, no_copy_or_move&&>(), "No assign from rvalue ref");

  static_assert(
      check_is_nothrow_assignable<false, no_copy_or_move&, no_copy_or_move const&&>(),
      "No assign from const rvalue ref"
  );
}

/// @test A copy-only type can be assigned from an lvalue reference or a @c const rvalue reference
TEST(NothrowAssignable, OnlyCopyAssignment) {
  static_assert(check_is_nothrow_assignable<true, only_copy&, only_copy&>(), "Can assign from lvalue ref");

  static_assert(check_is_nothrow_assignable<true, only_copy&, only_copy const&>(), "Can assign from const lvalue ref");

  static_assert(check_is_nothrow_assignable<false, only_copy&, only_copy&&>(), "No assign from rvalue ref");

  static_assert(
      check_is_nothrow_assignable<true, only_copy&, only_copy const&&>(),
      "Can assign from const rvalue ref (via decay to const lvalue ref)"
  );
}

/// @test A move-only type can only be assigned from a non-const rvalue reference
TEST(NothrowAssignable, OnlyMoveAssignment) {
  static_assert(check_is_nothrow_assignable<false, only_move&, only_move&>(), "No assign from lvalue ref");

  static_assert(check_is_nothrow_assignable<false, only_move&, only_move const&>(), "No assign from const lvalue ref");

  static_assert(check_is_nothrow_assignable<true, only_move&, only_move&&>(), "Can assign from rvalue ref");

  static_assert(
      check_is_nothrow_assignable<false, only_move&, only_move const&&>(),
      "No assign from const rvalue ref (via decay to const lvalue ref)"
  );
}

/// @test cv-qualified @c void is not assignable from anything.
TEST(NothrowAssignable, VoidConversions) {
  static_assert(check_is_nothrow_assignable<false, void, void>(), "void is not assignable from anything");
  static_assert(check_is_nothrow_assignable<false, void const, void>(), "void is not assignable from anything");
  static_assert(check_is_nothrow_assignable<false, void volatile, void>(), "void is not assignable from anything");
  static_assert(
      check_is_nothrow_assignable<false, void const volatile, void>(),
      "void is not assignable from anything"
  );

  static_assert(check_is_nothrow_assignable<false, void, void const>(), "void is not assignable from anything");
  static_assert(check_is_nothrow_assignable<false, void const, void const>(), "void is not assignable from anything");
  static_assert(
      check_is_nothrow_assignable<false, void volatile, void const>(),
      "void is not assignable from anything"
  );
  static_assert(
      check_is_nothrow_assignable<false, void const volatile, void const>(),
      "void is not assignable from anything"
  );

  static_assert(check_is_nothrow_assignable<false, void, void volatile>(), "void is not assignable from anything");
  static_assert(
      check_is_nothrow_assignable<false, void const, void volatile>(),
      "void is not assignable from anything"
  );
  static_assert(
      check_is_nothrow_assignable<false, void volatile, void volatile>(),
      "void is not assignable from anything"
  );
  static_assert(
      check_is_nothrow_assignable<false, void const volatile, void volatile>(),
      "void is not assignable from anything"
  );

  static_assert(
      check_is_nothrow_assignable<false, void, void const volatile>(),
      "void is not assignable from anything"
  );
  static_assert(
      check_is_nothrow_assignable<false, void const, void const volatile>(),
      "void is not assignable from anything"
  );
  static_assert(
      check_is_nothrow_assignable<false, void volatile, void const volatile>(),
      "void is not assignable from anything"
  );
  static_assert(
      check_is_nothrow_assignable<false, void const volatile, void const volatile>(),
      "void is not assignable from anything"
  );
}

/// @test cv-qualified @c void is assignable from other-cv-qualified @c void if qualifiers are added or the same.
TEST(NothrowAssignableV, VoidConversions) {}

/// @test Can assign to a base class object from a derived class object
TEST(NothrowAssignable, CanAssignDerivedToBase) {
  static_assert(check_is_nothrow_assignable<true, base, derived>(), "Can assign to base from derived");
}

/// @test Cannot assign to a derived class object from a base class object
TEST(NothrowAssignable, CannotAssignBaseToDerived) {
  static_assert(check_is_nothrow_assignable<false, derived, base>(), "Cannot assign to derived from base");
}

/// @test Can assign to a pointer to base class from a pointer to derived class
TEST(NothrowAssignable, CanAssignDerivedPointerToBasePointer) {
  static_assert(
      check_is_nothrow_assignable<true, base*&, derived*>(),
      "Can assign to base pointer from derived pointer"
  );
}

/// @test Cannot assign to a pointer to a derived class from a pointer to a base class
TEST(NothrowAssignable, CannotAssignBasePointerToDerivedPointer) {
  static_assert(
      check_is_nothrow_assignable<false, derived*&, base*>(),
      "Cannot assign to derived pointer from base pointer"
  );
}

/// @test Cannot assign to rvalue if only lvalue assignable
TEST(NothrowAssignable, OnlyLValueNothrowAssignableNotNothrowAssignableFromRvalue) {
  static_assert(
      check_is_nothrow_assignable<false, only_lvalue_assignable&&, only_lvalue_assignable>(),
      "Cannot assign to rvalue"
  );
}

/// @test Can still assign to lvalue if only lvalue assignable
TEST(NothrowAssignable, OnlyLValueNothrowAssignableNothrowAssignableFromLValue) {
  static_assert(
      check_is_nothrow_assignable<true, only_lvalue_assignable&, only_lvalue_assignable>(),
      "Can assign to lvalue"
  );
}

class throwing_target {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  throwing_target(implicit_source) noexcept(false);
  explicit throwing_target(explicit_source) noexcept(false);

  auto operator=(assign_source const&) noexcept(false) -> throwing_target&;
};

class throwing_convertible_source {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator target() const noexcept(false);
};

class throwing_explicit_convertible_source {
 public:
  explicit operator target() const noexcept(false);
};

/// @test Instances with throwing assignment are not nothrow-assignable
TEST(NothrowAssignable, ThrowingAssignmentOperator) {
  static_assert(
      check_is_assignable<true, throwing_target&, assign_source>(),
      "A class is assignable from another when there is an assignment operator"
  );
  static_assert(
      check_is_nothrow_assignable<false, throwing_target&, assign_source>(),
      "A class is not nothrow assignable from another when there is a throwing assignment operator"
  );
}

/// @test Instances can be assigned to where there is an implicit conversion from the source to the target
TEST(NothrowAssignable, ThrowingImplicitConversions) {
  static_assert(
      check_is_assignable<true, throwing_target&, implicit_source>(),
      "A class is assignable from another with an implicit constructor"
  );
  static_assert(
      check_is_assignable<false, throwing_target&, explicit_source>(),
      "A class is not assignable from another with an explicit constructor"
  );
  static_assert(
      check_is_assignable<true, target&, throwing_convertible_source>(),
      "A class is assignable from another with an implicit conversion operator"
  );
  static_assert(
      check_is_assignable<false, target&, throwing_explicit_convertible_source>(),
      "A class is not assignable from another with an explicit conversion operator"
  );
  static_assert(
      check_is_nothrow_assignable<false, throwing_target&, implicit_source>(),
      "A class is not nothrow assignable from another with a throwing implicit constructor"
  );
  static_assert(
      check_is_nothrow_assignable<false, throwing_target&, explicit_source>(),
      "A class is not assignable from another with an explicit constructor"
  );
  static_assert(
      check_is_nothrow_assignable<false, target&, throwing_convertible_source>(),
      "A class is not nothrow assignable from another with a throwing implicit constructor"
  );
  static_assert(
      check_is_nothrow_assignable<false, target&, throwing_explicit_convertible_source>(),
      "A class is not assignable from another with an explicit conversion operator"
  );
}

class throwing_assignment_takes_implicit_conversion_target {
 public:
  auto operator=(target const&) noexcept(false) -> throwing_assignment_takes_implicit_conversion_target&;
};

/// @test Assignments from a type that implicit converts to the assignment arg is allowed
TEST(NothrowAssignable, ThrowingImplicitConversionsToAssignmentArg) {
  static_assert(
      check_is_assignable<true, throwing_assignment_takes_implicit_conversion_target&, convertible_source>(),
      "A class is assignable from another with an implicit conversion operator"
  );
  static_assert(
      check_is_nothrow_assignable<false, throwing_assignment_takes_implicit_conversion_target&, convertible_source>(),
      "A class is not nothrow assignable from another with an implicit conversion operator and a throwing assignment"
  );
}

}  // namespace
