// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

template <class T, class... Args>
constexpr auto check_is_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_constructible<T, Args...>, std::true_type> &&
         std::is_constructible_v<T, Args...>;
}

template <class T, class... Args>
constexpr auto check_is_nothrow_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_nothrow_constructible<T, Args...>, std::true_type> &&
         std::is_nothrow_constructible_v<T, Args...>;
}

template <class T, class... Args>
constexpr auto check_all_is_constructible() -> bool {
  return check_is_constructible<T, Args...>() && check_is_nothrow_constructible<T, Args...>();
}

class some_user_type {};

class constructible_with_args {
 public:
  constructible_with_args() noexcept;
  explicit constructible_with_args(int) noexcept;
  explicit constructible_with_args(some_user_type&, int, double) noexcept;
};

class not_default_constructible {
 public:
  explicit not_default_constructible(int) noexcept;
};

/// @test A type is constructible from arguments that match the constructors
TEST(Constructible, CanConstructWithCorrectArgs) {
  static_assert(check_all_is_constructible<constructible_with_args>(), "A type is constructible from correct args");
  static_assert(
      check_all_is_constructible<constructible_with_args, int>(),
      "A type is constructible from correct args"
  );
  static_assert(
      check_all_is_constructible<constructible_with_args, constructible_with_args const&>(),
      "A type is constructible from correct args"
  );
  static_assert(
      check_all_is_constructible<constructible_with_args, constructible_with_args&&>(),
      "A type is constructible from correct args"
  );
  static_assert(
      check_all_is_constructible<constructible_with_args, some_user_type&, int, double>(),
      "A type is constructible from correct args"
  );
}

/// @test A type is not constructible from arguments that don't match the constructors
TEST(Constructible, CannotConstructWithIncorrectArgs) {
  static_assert(
      !check_all_is_constructible<not_default_constructible>(),
      "A type is not constructible from incorrect args"
  );
  static_assert(
      !check_all_is_constructible<constructible_with_args, some_user_type&>(),
      "A type is not constructible from incorrect args"
  );
  static_assert(
      !check_all_is_constructible<constructible_with_args, int, int>(),
      "A type is not constructible from incorrect args"
  );
  static_assert(
      !check_all_is_constructible<constructible_with_args, constructible_with_args const&, int>(),
      "A type is not constructible from incorrect args"
  );
  static_assert(
      !check_all_is_constructible<constructible_with_args, some_user_type const&, int, double>(),
      "A type is not constructible from incorrect args"
  );
  static_assert(
      !check_all_is_constructible<constructible_with_args, some_user_type&, some_user_type&, double>(),
      "A type is not constructible from incorrect args"
  );
}

using self_constructible_types =
    arene::base::type_lists::concat_unique_t<::testing::scalar_types, ::testing::class_types, ::testing::union_types>;

template <typename T>
class SelfConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(SelfConstructibleTest, self_constructible_types, );

/// @test A type is constructible to itself
TYPED_TEST(SelfConstructibleTest, IsConstructibleFromSelf) {
  static_assert(check_all_is_constructible<TypeParam, TypeParam>(), "A type is constructible from itself");
}

/// @test A type is not constructible from an unrelated type
TYPED_TEST(SelfConstructibleTest, IsNotConstructibleFromUnrelated) {
  struct unrelated {};
  static_assert(
      !check_all_is_constructible<TypeParam, unrelated>(),
      "A type is not constructible from an unrelated type"
  );
}

/// @test A @c const lvalue reference is constructible from a non-@c const lvalue reference
TYPED_TEST(SelfConstructibleTest, ConstLvalueRefIsConstructibleFromLValueRef) {
  static_assert(
      check_all_is_constructible<TypeParam const&, TypeParam&>(),
      "A const lvalue reference is constructible from a non-const lvalue reference"
  );
}

/// @test A non-@c const lvalue reference is not constructible from a const lvalue reference
TYPED_TEST(SelfConstructibleTest, LvalueRefIsNotConstructibleFromConstLValueRef) {
  static_assert(
      !check_all_is_constructible<TypeParam&, TypeParam const&>(),
      "A non-const lvalue reference is not constructible from a const lvalue reference"
  );
}

/// @test Non-@c void types are not constructible from @c void
TYPED_TEST(SelfConstructibleTest, TypeIsNotConstructibleFromVoid) {
  static_assert(!check_all_is_constructible<TypeParam, void>(), "A value is not implicitly constructible from void");
}

/// @test Non-@c void types are not constructible from @c const @c void
TYPED_TEST(SelfConstructibleTest, TypeIsNotConstructibleFromConstVoid) {
  static_assert(!check_all_is_constructible<TypeParam, void const>(), "A value is not constructible from const void");
}

/// @test An array is constructible
TEST(Constructible, CanConstructArray) {
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  static_assert(check_all_is_constructible<int[3]>(), "An array is (nothrow) default constructible");
  static_assert(check_all_is_constructible<int[3][2]>(), "A 2d array is (nothrow) default constructible");

  static_assert(
      !check_is_constructible<not_default_constructible[3]>(),
      "An array of types without default constuctors is not constructible"
  );
  static_assert(
      !check_is_constructible<not_default_constructible[3][2]>(),
      "A 2d array of types without default constuctors is not constructible"
  );

  struct throwing {
    throwing() noexcept(false);
  };
  static_assert(
      check_is_constructible<throwing[3]>(),
      "An array with a throwing default constructor is default constructible"
  );
  static_assert(
      !check_is_nothrow_constructible<throwing[3]>(),
      "An array with a throwing default constructor is not nothrow default constructible"
  );

  static_assert(
      check_is_constructible<throwing[3][2]>(),
      "A 2d array with a throwing default constructor is default constructible"
  );
  static_assert(
      !check_is_nothrow_constructible<throwing[3][2]>(),
      "A 2d array with a throwing default constructor is not nothrow default constructible"
  );
  // NOLINTEND(hicpp-avoid-c-arrays)
}

class base {};
class derived : public base {};

/// @test A base class instance or reference to base is constructible from a derived class
TEST(Constructible, BaseIsConstructibleFromDerived) {
  static_assert(check_all_is_constructible<base, derived>(), "A base class is constructible from derived");
  static_assert(check_all_is_constructible<base&, derived&>(), "A base class reference is constructible from derived");
  static_assert(
      check_all_is_constructible<base const&, derived const&>(),
      "A const base class reference is constructible from const derived reference"
  );
  static_assert(
      !check_all_is_constructible<base&, derived const&>(),
      "A base class reference is not constructible from const derived reference"
  );
}

/// @test A derived class instance or reference to derived is constructible from a base class
TEST(Constructible, DerivedIsNotConstructibleFromBase) {
  static_assert(!check_all_is_constructible<derived, base>(), "A derived class is not constructible from a base class");
  static_assert(
      !check_all_is_constructible<derived&, base&>(),
      "A derived class is not constructible from a base class"
  );
  static_assert(
      !check_all_is_constructible<derived const&, base const&>(),
      "A derived class is not constructible from a base class"
  );
}

class implicit_source {};
class explicit_source {};
class target {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  target(implicit_source) noexcept;
  explicit target(explicit_source) noexcept;
};

class constructible_source {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator target() const noexcept;
};

class explicit_constructible_source {
 public:
  explicit operator target() const noexcept;
};

/// @test A class is constructible from something with an implicit conversion, but not an explicit conversion
TEST(Constructible, ImplicitConversions) {
  static_assert(
      check_all_is_constructible<target, implicit_source>(),
      "A class is constructible from another with an implicit constructor"
  );
  static_assert(
      check_all_is_constructible<target, explicit_source>(),
      "A class is constructible from another with an explicit constructor"
  );
  static_assert(
      check_all_is_constructible<target, constructible_source>(),
      "A class is constructible from another with an implicit conversion operator"
  );
  static_assert(
      check_all_is_constructible<target, explicit_constructible_source>(),
      "A class is constructible from another with an explicit conversion operator"
  );
}

/// @test cv-qualified @c void is not constructible from anything.
TEST(Constructible, VoidConversions) {
  static_assert(!check_all_is_constructible<void, void>(), "void is not constructible from anything");
  static_assert(!check_all_is_constructible<void const, void>(), "void is not constructible from anything");
  static_assert(!check_all_is_constructible<void volatile, void>(), "void is not constructible from anything");
  static_assert(!check_all_is_constructible<void const volatile, void>(), "void is not constructible from anything");

  static_assert(!check_all_is_constructible<void, void const>(), "void is not constructible from anything");
  static_assert(!check_all_is_constructible<void const, void const>(), "void is not constructible from anything");
  static_assert(!check_all_is_constructible<void volatile, void const>(), "void is not constructible from anything");
  static_assert(
      !check_all_is_constructible<void const volatile, void const>(),
      "void is not constructible from anything"
  );

  static_assert(!check_all_is_constructible<void, void volatile>(), "void is not constructible from anything");
  static_assert(!check_all_is_constructible<void const, void volatile>(), "void is not constructible from anything");
  static_assert(!check_all_is_constructible<void volatile, void volatile>(), "void is not constructible from anything");
  static_assert(
      !check_all_is_constructible<void const volatile, void volatile>(),
      "void is not constructible from anything"
  );

  static_assert(!check_all_is_constructible<void, void const volatile>(), "void is not constructible from anything");
  static_assert(
      !check_all_is_constructible<void const, void const volatile>(),
      "void is not constructible from anything"
  );
  static_assert(
      !check_all_is_constructible<void volatile, void const volatile>(),
      "void is not constructible from anything"
  );
  static_assert(
      !check_all_is_constructible<void const volatile, void const volatile>(),
      "void is not constructible from anything"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class no_copy_or_move {
 public:
  no_copy_or_move(no_copy_or_move const&) = delete;
  no_copy_or_move(no_copy_or_move&&) = delete;
};

/// @test A class with deleted copy and move is not copy or move constructible
TEST(IsConstructible, NoCopyOrMove) {
  static_assert(!check_all_is_constructible<no_copy_or_move, no_copy_or_move&>(), "Not constructible from lvalue ref");
  static_assert(
      !check_all_is_constructible<no_copy_or_move, no_copy_or_move const&>(),
      "Not constructible from const lvalue ref"
  );
  static_assert(!check_all_is_constructible<no_copy_or_move, no_copy_or_move&&>(), "Not constructible from rvalue ref");
  static_assert(
      !check_all_is_constructible<no_copy_or_move, no_copy_or_move const&&>(),
      "Not constructible from const rvalue ref"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class only_copy {
 public:
  only_copy(only_copy const&) noexcept;
  only_copy(only_copy&&) = delete;
};

/// @test A class with deleted move is not move constructible
TEST(IsConstructible, OnlyCopy) {
  static_assert(check_all_is_constructible<only_copy, only_copy&>(), "Constructible from lvalue ref");
  static_assert(check_all_is_constructible<only_copy, only_copy const&>(), "Constructible from const lvalue ref");
  static_assert(!check_all_is_constructible<only_copy, only_copy&&>(), "Not constructible from rvalue ref");
  static_assert(
      check_all_is_constructible<only_copy, only_copy const&&>(),
      "Constructible from const rvalue ref via decay to const lvalue ref"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class only_move {
 public:
  only_move(only_move const&) = delete;
  only_move(only_move&&) noexcept;
};

/// @test A class with deleted move is not move constructible
TEST(IsConstructible, OnlyMove) {
  static_assert(!check_all_is_constructible<only_move, only_move&>(), "Not constructible from lvalue ref");
  static_assert(!check_all_is_constructible<only_move, only_move const&>(), "Not constructible from const lvalue ref");
  static_assert(check_all_is_constructible<only_move, only_move&&>(), "Constructible from rvalue ref");
  static_assert(!check_all_is_constructible<only_move, only_move const&&>(), "Not constructible from const rvalue ref");
}

class is_not_noexcept_default_constructible {
 public:
  is_not_noexcept_default_constructible() noexcept(false);
  explicit is_not_noexcept_default_constructible(int) noexcept;
};

/// @test A type is constructible but not nothrow constructible if the constructor is not noexcept
TEST(NothrowConstructible, IsNotNothrowDefaultConstructible) {
  static_assert(check_is_constructible<is_not_noexcept_default_constructible>(), "Default constructible");
  static_assert(
      check_is_constructible<is_not_noexcept_default_constructible, int>(),
      "Constructible from correct args"
  );

  static_assert(
      !check_is_nothrow_constructible<is_not_noexcept_default_constructible>(),
      "Not nothrow default constructible"
  );
  static_assert(
      check_is_nothrow_constructible<is_not_noexcept_default_constructible, int>(),
      "Nothrow default constructible with arguments"
  );
}

template <class Base>
class conditionally_noexcept : Base {
 public:
  conditionally_noexcept() noexcept(noexcept(Base())) = default;
};

/// @test A type is constructible but not nothrow constructible if the constructor is not noexcept
TEST(NothrowConstructible, ConditionalNoexcept) {
  static_assert(check_is_nothrow_constructible<constructible_with_args>(), "Is nothrow default constructible");
  static_assert(
      !check_is_nothrow_constructible<is_not_noexcept_default_constructible>(),
      "Is not nothrow default constructible"
  );

  static_assert(
      check_is_nothrow_constructible<conditionally_noexcept<constructible_with_args>>(),
      "Is nothrow constructible as base is nothrow constructible"
  );
  static_assert(
      !check_is_nothrow_constructible<conditionally_noexcept<is_not_noexcept_default_constructible>>(),
      "Is not notrhow constructible as base is not nothrow consturctible"
  );
}

class throwable_target {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  throwable_target(implicit_source) noexcept(false);
  explicit throwable_target(explicit_source) noexcept(false);
};

class throwable_constructible_source {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator throwable_target() const noexcept(false);
};

class throwable_explicit_constructible_source {
 public:
  explicit operator throwable_target() const noexcept(false);
};

/// @test A class is nothrow constructible from something with an implicit conversion, but not an explicit conversion if
/// the conversion is noexcept
TEST(NothrowConstructible, ImplicitConversions) {
  static_assert(
      check_is_constructible<throwable_target, implicit_source>(),
      "A class is constructible from another with an implicit constructor"
  );
  static_assert(
      check_is_constructible<throwable_target, explicit_source>(),
      "A class is constructible from another with an explicit constructor"
  );
  static_assert(
      check_is_constructible<throwable_target, throwable_constructible_source>(),
      "A class is constructible from another with an implicit conversion operator"
  );
  static_assert(
      check_is_constructible<throwable_target, throwable_explicit_constructible_source>(),
      "A class is constructible from another with an explicit conversion operator"
  );

  static_assert(
      !check_is_nothrow_constructible<throwable_target, implicit_source>(),
      "A class is not nothrow constructible from another with a throwable implicit constructor"
  );
  static_assert(
      !check_is_nothrow_constructible<throwable_target, explicit_source>(),
      "A class is not nothrow constructible from another with a throwable explicit constructor"
  );
  static_assert(
      !check_is_nothrow_constructible<throwable_target, throwable_constructible_source>(),
      "A class is not nothrow constructible from another with a throwable implicit conversion operator"
  );
  static_assert(
      !check_is_nothrow_constructible<throwable_target, throwable_explicit_constructible_source>(),
      "A class is not nothrow constructible from another with a throwable explicit conversion operator"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class throwable_only_copy {
 public:
  throwable_only_copy(throwable_only_copy const&) noexcept(false);
  throwable_only_copy(throwable_only_copy&&) = delete;
};

/// @test A class with deleted move is not move constructible and follows noexcept qualifier
TEST(NothrowIsConstructible, OnlyCopy) {
  static_assert(check_is_constructible<throwable_only_copy, throwable_only_copy&>(), "Constructible from lvalue ref");
  static_assert(
      check_is_constructible<throwable_only_copy, throwable_only_copy const&>(),
      "Constructible from const lvalue ref"
  );
  static_assert(
      !check_is_constructible<throwable_only_copy, throwable_only_copy&&>(),
      "Not constructible from rvalue ref"
  );
  static_assert(
      check_is_constructible<throwable_only_copy, throwable_only_copy const&&>(),
      "Constructible from const rvalue ref via decay to const lvalue ref"
  );

  static_assert(
      !check_is_nothrow_constructible<throwable_only_copy, throwable_only_copy&>(),
      "Not nothrow constructible with throwable copy constructor"
  );
  static_assert(
      !check_is_nothrow_constructible<throwable_only_copy, throwable_only_copy const&>(),
      "Not nothrow constructible with throwable copy constructor"
  );
  static_assert(
      !check_is_nothrow_constructible<throwable_only_copy, throwable_only_copy&&>(),
      "Not constructible from rvalue ref"
  );
  static_assert(
      !check_is_nothrow_constructible<throwable_only_copy, throwable_only_copy const&&>(),
      "Not nothrow constructible with throwable copy constructor"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class throwable_only_move {
 public:
  throwable_only_move(throwable_only_move const&) = delete;
  throwable_only_move(throwable_only_move&&) noexcept(false);
};

/// @test A class with deleted move is not move constructible and follows noexcept qualifier
TEST(NothrowIsConstructible, OnlyMove) {
  static_assert(
      !check_is_constructible<throwable_only_move, throwable_only_move&>(),
      "Not constructible from lvalue ref"
  );
  static_assert(
      !check_is_constructible<throwable_only_move, throwable_only_move const&>(),
      "Not constructible from const lvalue ref"
  );
  static_assert(check_is_constructible<throwable_only_move, throwable_only_move&&>(), "Constructible from rvalue ref");
  static_assert(
      !check_is_constructible<throwable_only_move, throwable_only_move const&&>(),
      "Not constructible from const rvalue ref"
  );

  static_assert(
      !check_is_nothrow_constructible<throwable_only_move, throwable_only_move&>(),
      "Not constructible from lvalue ref"
  );
  static_assert(
      !check_is_nothrow_constructible<throwable_only_move, throwable_only_move const&>(),
      "Not constructible from const lvalue ref"
  );
  static_assert(
      !check_is_nothrow_constructible<throwable_only_move, throwable_only_move&&>(),
      "Not nothrow constructible with throwable move constructor"
  );
  static_assert(
      !check_is_nothrow_constructible<throwable_only_move, throwable_only_move const&&>(),
      "Not constructible from const rvalue ref"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class throwable_destructor {
 public:
  throwable_destructor() noexcept(true);
  ~throwable_destructor() noexcept(false);
};

/// @test A class with a noexcept constructor but throwable destructor is not nothrow constructible
// See https://cplusplus.github.io/LWG/issue2116 and
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2842r0.pdf
TEST(NothrowIsConstructible, ThrowableDestructor) {
  static_assert(
      !check_is_nothrow_constructible<throwable_destructor>(),
      "Not nothrow constructible due to throwable destructor"
  );
}

}  // namespace
