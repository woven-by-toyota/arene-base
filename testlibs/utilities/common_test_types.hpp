// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COMMON_TEST_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COMMON_TEST_TYPES_HPP_

// IWYU pragma: private, include "testlibs/minitest/minitest.hpp"

#include "arene/base/type_list.hpp"
#include "stdlib/include/utility"

namespace testing {

// Forward declaration for minitest test types
template <typename... TypeParams>
// NOLINTNEXTLINE(readability-identifier-naming)
class Types;

/// Provide a variety of common test types to use in stdlib testing.
///
/// This file provides a variety of defined types and type lists to used in unit testing for stdlib. The type lists are
/// intended to be passed to minitest @c TYPED_TEST_SUITES. A type list concatentation function is also provided for
/// users to easily combine the predefined lists with their own types to use.
///
/// This module provides a type list for each of the primary and composite type categories, as defined by the C++
/// Metaprogramming library, as well as some other type lists that are generally useful. All types provided are not
/// cv-qualified - users can extend the lists with qualifiers as necessary. These lists are intended to be
/// representative of each category, but not exhaustive.

namespace {

/// @brief Extend a typelist with reference and pointer types
/// @tparam L type list type
///
/// Undefined primary template.
///
template <class L>
struct extend_with_reference_and_pointer_types;

/// @brief Extend a typelist with reference and pointer types
/// @tparam List type list template type
/// @tparam Ts types to extend
///
/// Specialization to extend @c List<Ts...> with references and pointers of
/// <tt>Ts...</tt>.
///
/// @note This metafunction does not use type traits such as
/// `std::add_lvalue_reference`. Types in @c Ts must be referenceable.
///
template <template <class...> class List, class... Ts>
struct extend_with_reference_and_pointer_types<List<Ts...>> {
  using type = List<  //
      Ts...,          //
      Ts&...,
      Ts const&...,
      Ts volatile&...,
      Ts const volatile&...,
      Ts&&...,
      Ts const&&...,
      Ts volatile&&...,
      Ts const volatile&&...,
      Ts*...,
      Ts const*...,
      Ts volatile*...,
      Ts const volatile*...>;
};

}  // namespace

/// @brief Extend a type list with reference and pointer types
/// @tparam Ls the type list to extend
///
/// Example:
/// ~~~{.cpp}
///  StaticAssertTypeEq<
///    Types<
///      char,
///      int,
///      char &,
///      int &,
///      char const&,
///      int const&,
///      char volatile&,
///      int volatile&,
///      char const volatile&,
///      int const volatile&,
///      char &&,
///      int &&,
///      char const&&,
///      int const&&,
///      char volatile&&,
///      int volatile&&,
///      char const volatile&&,
///      int const volatile&&,
///      char *,
///      int *,
///      char const*,
///      int const*,
///      char volatile*,
///      int volatile*,
///      char const volatile*,
///      int const volatile*
///    >,
///    extend_with_reference_and_pointer_types_t<Types<char, int>>,
///  >();
/// ~~~
///
template <class L>
using extend_with_reference_and_pointer_types_t = typename extend_with_reference_and_pointer_types<L>::type;

//
// General user-defined types.
//

/// @brief A user defined struct
struct struct_type {
  struct_type() = default;
  constexpr explicit struct_type(int) {}
};

/// @brief A user defined class
class class_type {
 public:
  class_type() = default;
  constexpr explicit class_type(int) {}
};

/// @brief A struct with a static function
struct struct_with_static_function {
  static void function();
};

/// @brief A struct with a member function
struct struct_with_member_function {
  void member_function();
};

/// @brief A struct with a noexcept function
struct struct_with_noexcept_functions {
  void base_function() noexcept;
  void const_function() const noexcept;
  void lvalue_ref_function() & noexcept;
  void const_lvalue_ref_function() const& noexcept;
  void rvalue_ref_function() && noexcept;
  void const_rvalue_ref_function() const&& noexcept;
};

/// @brief A struct with member data
struct struct_with_member_object {
  int a;
};

/// @brief A user defined unscoped enum
enum unscoped_enum { unscoped_enum_min = 0, unscoped_enum_max = 127 };

/// @brief A user defined scoped enum
enum class scoped_enum { scoped_enum_min = 0, scoped_enum_max = 127 };

/// @brief A user defined union type
union union_type {
  int a;
  bool b;
};

/// @brief A simple user-defined type with `noexcept(false)` on constructors.
class not_noexcept_user_type {
 public:
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  constexpr not_noexcept_user_type() noexcept(false) {}
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  not_noexcept_user_type(not_noexcept_user_type const&) noexcept(false) {}
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  not_noexcept_user_type(not_noexcept_user_type&&) noexcept(false) {}

  auto operator=(not_noexcept_user_type const&) noexcept -> not_noexcept_user_type& = default;
  auto operator=(not_noexcept_user_type&&) noexcept -> not_noexcept_user_type& = default;

  ~not_noexcept_user_type() = default;
};

//
// General building-block type lists.
//

using character_types = ::testing::Types<char, signed char, unsigned char, wchar_t, char16_t, char32_t>;

// NOLINTBEGIN(google-runtime-int)
using signed_integer_types = ::testing::Types<signed char, short, int, long, long long>;

using unsigned_integer_types =
    ::testing::Types<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;
// NOLINTEND(google-runtime-int)

using bool_types = ::testing::Types<bool>;

// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
using pointer_to_object_types = ::testing::Types<int*, struct_type*, scoped_enum*, int (*)[]>;
using pointer_to_function_types = ::testing::Types<void (*)(), int (*)(float, class_type)>;
using pointer_to_void_types = ::testing::Types<void*>;

//
// Primary type categories
//

using void_types = ::testing::Types<void>;
using null_pointer_types = ::testing::Types<decltype(nullptr)>;
using integral_types =
    arene::base::type_lists::concat_unique_t<signed_integer_types, unsigned_integer_types, bool_types, character_types>;
using floating_point_types = ::testing::Types<float, double, long double>;
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
using array_types = ::testing::Types<int[3], struct_type[3]>;
using enum_types = ::testing::Types<unscoped_enum, scoped_enum>;
using union_types = ::testing::Types<union_type>;
using class_types = ::testing::Types<struct_type, class_type>;
using function_types =
    ::testing::Types<void(), void(int), int(int, float, double), decltype(struct_with_static_function::function)>;
using pointer_types =
    arene::base::type_lists::concat_unique_t<pointer_to_object_types, pointer_to_function_types, pointer_to_void_types>;
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
using lvalue_reference_types = ::testing::Types<int&, class_type&, int*&, int (&)[], void (&)(int)>;
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
using rvalue_reference_types = ::testing::Types<int&&, class_type&&, int*&&, int (&&)[], void (&&)(int)>;
// NOLINTBEGIN(hicpp-avoid-c-arrays)
using member_object_pointer_types = ::testing::Types<
    int struct_with_member_object::*,
    int const struct_with_member_object::*,
    int volatile struct_with_member_object::*,
    int const volatile struct_with_member_object::*,
    int (struct_with_member_object::*)[],
    int const (struct_with_member_object::*)[],
    int volatile (struct_with_member_object::*)[],
    int const volatile (struct_with_member_object::*)[],
    int (struct_with_member_object::*)[3],
    int const (struct_with_member_object::*)[3],
    int volatile (struct_with_member_object::*)[3],
    int const volatile (struct_with_member_object::*)[3]>;
// NOLINTEND(hicpp-avoid-c-arrays)
using member_function_pointer_types = ::testing::Types<
    void (struct_with_member_function::*)(),
    void (struct_with_member_function::*)() const,
    void (struct_with_member_function::*)()&,
    void (struct_with_member_function::*)() const&,
    void (struct_with_member_function::*)()&&,
    void (struct_with_member_function::*)() const&&,
    decltype(&struct_with_noexcept_functions::base_function),
    decltype(&struct_with_noexcept_functions::const_function),
    decltype(&struct_with_noexcept_functions::lvalue_ref_function),
    decltype(&struct_with_noexcept_functions::const_lvalue_ref_function),
    decltype(&struct_with_noexcept_functions::rvalue_ref_function),
    decltype(&struct_with_noexcept_functions::const_rvalue_ref_function)>;

//
// Composite type categories
//

using arithmetic_types = arene::base::type_lists::concat_unique_t<integral_types, floating_point_types>;
using fundamental_types = arene::base::type_lists::concat_unique_t<arithmetic_types, void_types, null_pointer_types>;
using reference_types = arene::base::type_lists::concat_unique_t<lvalue_reference_types, rvalue_reference_types>;
using member_pointer_types =
    arene::base::type_lists::concat_unique_t<member_object_pointer_types, member_function_pointer_types>;
using scalar_types = arene::base::type_lists::
    concat_unique_t<arithmetic_types, enum_types, pointer_types, member_pointer_types, null_pointer_types>;
using object_types = arene::base::type_lists::concat_unique_t<scalar_types, array_types, union_types, class_types>;
using compound_types = arene::base::type_lists::concat_unique_t<
    array_types,
    function_types,
    pointer_types,
    member_pointer_types,
    reference_types,
    class_types,
    union_types,
    enum_types>;

//
// Additionally useful type lists
//

/// @brief all object_types other than C arrays.
///
/// C arrays don't always behave like other objects, which can make it difficult to write tests.
using non_array_object_types = arene::base::type_lists::concat_unique_t<scalar_types, union_types, class_types>;

/// @brief cv-qualified function types
///
/// Sometimes called "abominable function types", see
/// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html#2.1
using cv_qualified_function_types = ::testing::Types<
    void() const,
    void() volatile,
    void() const volatile,
    void()&,
    void() const&,
    void() volatile&,
    void() const volatile&,
    void()&&,
    void() const&&,
    void() volatile&&,
    void() const volatile&&>;

/// @brief types that cannot be referenced
///
/// Non-referenceable types are @c void and function types that have cv-ref qualifiers (sometimes called "abominable
/// function types", see https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html#2.1).
using non_referenceable_types = arene::base::type_lists::concat_unique_t<void_types, cv_qualified_function_types>;

struct not_move_constructible {
  not_move_constructible() = default;
  ~not_move_constructible() = default;

  not_move_constructible(not_move_constructible const&) = delete;
  auto operator=(not_move_constructible const&) -> not_move_constructible& = delete;

  not_move_constructible(not_move_constructible&&) = delete;
  auto operator=(not_move_constructible&&) -> not_move_constructible& = default;
};

struct not_move_assignable {
  not_move_assignable() = default;
  ~not_move_assignable() = default;

  not_move_assignable(not_move_assignable const&) = delete;
  auto operator=(not_move_assignable const&) -> not_move_assignable& = delete;

  not_move_assignable(not_move_assignable&&) = default;
  auto operator=(not_move_assignable&&) -> not_move_assignable& = delete;
};

struct nothrow_move_construction_and_assignment {
  constexpr nothrow_move_construction_and_assignment() = default;
  constexpr nothrow_move_construction_and_assignment(nothrow_move_construction_and_assignment const&) = default;
  constexpr nothrow_move_construction_and_assignment(nothrow_move_construction_and_assignment&&) noexcept(true) {}
  constexpr auto operator=(nothrow_move_construction_and_assignment const&)
      -> nothrow_move_construction_and_assignment& = default;
  constexpr auto operator=(nothrow_move_construction_and_assignment&&) noexcept(true)
      -> nothrow_move_construction_and_assignment& {
    return *this;
  }
  ~nothrow_move_construction_and_assignment() = default;
};

struct throwable_move_construction {
  constexpr throwable_move_construction() = default;
  constexpr throwable_move_construction(throwable_move_construction const&) = default;
  constexpr throwable_move_construction(throwable_move_construction&&) noexcept(false) {}
  constexpr auto operator=(throwable_move_construction const&) -> throwable_move_construction& = default;
  constexpr auto operator=(throwable_move_construction&&) -> throwable_move_construction& = default;
  ~throwable_move_construction() = default;
};

struct throwable_move_assignment {
  constexpr throwable_move_assignment() = default;
  constexpr throwable_move_assignment(throwable_move_assignment const&) = default;
  constexpr throwable_move_assignment(throwable_move_assignment&&) = default;
  constexpr auto operator=(throwable_move_assignment const&) -> throwable_move_assignment& = default;
  constexpr auto operator=(throwable_move_assignment&&) noexcept(false) -> throwable_move_assignment& { return *this; }
  ~throwable_move_assignment() = default;
};

/// @brief wraps a type and disables copy construction and copy assignment
/// @tparam T underlying type
///
/// A helper type used to disable copy construction and copy assignment. This type defines an implicit converting
/// constructor from `T`.
///
template <class T>
struct only_movable {
  using value_type = T;

  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  value_type value{};

  only_movable(value_type val)  // NOLINT(hicpp-explicit-conversions)
      : value{static_cast<value_type&&>(val)} {}

  ~only_movable() = default;
  only_movable() = default;
  only_movable(only_movable const&) = delete;
  only_movable(only_movable&&) = default;
  auto operator=(only_movable const&) -> only_movable& = delete;
  auto operator=(only_movable&&) -> only_movable& = default;

  friend auto operator==(only_movable const& lhs, only_movable const& rhs)  //
      noexcept(noexcept(std::declval<T>() == std::declval<T>()))            //
      -> decltype(std::declval<T>() == std::declval<T>()) {
    return lhs.value == rhs.value;
  }

  friend auto operator<(only_movable const& lhs, only_movable const& rhs)  //
      noexcept(noexcept(std::declval<T>() < std::declval<T>()))            //
      -> decltype(std::declval<T>() < std::declval<T>()) {
    return lhs.value < rhs.value;
  }
};

}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_COMMON_TEST_TYPES_HPP_
