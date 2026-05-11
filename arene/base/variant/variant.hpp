// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_VARIANT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_VARIANT_HPP_

// IWYU pragma: private, include "arene/base/variant.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/memory/construct_at.hpp"
#include "arene/base/memory/destroy_at.hpp"
#include "arene/base/stdlib_choice/add_pointer.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/equal_to.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_reference.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_void.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/max_initializer_list_overload.hpp"  // IWYU pragma: keep
#include "arene/base/type_traits/conditional.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_manipulation/non_constructible_dummy.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/index_of.hpp"
#include "arene/base/type_traits/void_t.hpp"
#include "arene/base/utility/forward_like.hpp"
#include "arene/base/variant/traits.hpp"

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
#include "arene/base/variant/detail/bad_variant_access_exceptions_enabled.hpp"  // IWYU pragma: export
#else
#include "arene/base/variant/detail/bad_variant_access_exceptions_disabled.hpp"  // IWYU pragma: export
#endif

// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A15_5_3-h-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress CERT_CPP-ERR50-h    "False positive: Exception specification is conditional"
// parasoft-begin-suppress CERT_CPP-ERR55-a    "False positive: Exception specification is conditional"

// parasoft-begin-suppress CERT_CPP-ERR51-b    "False positive: Thrown exception is propagated"
// parasoft-begin-suppress AUTOSAR-A15_3_2-a   "False positive: Thrown exception is propagated"
// parasoft-begin-suppress AUTOSAR-A15_5_3-g   "False positive: Thrown exception is propagated"
// parasoft-begin-suppress CERT_CPP-ERR50-g    "False positive: Thrown exception is propagated"
// parasoft-begin-suppress AUTOSAR-M15_3_4-b   "False positive: Thrown exception is propagated"
// parasoft-begin-suppress AUTOSAR-A15_2_1-b   "False positive: Throwing constructor not invoked before program startup"

namespace arene {
namespace base {
template <typename... Ts>
class variant;

namespace variant_detail {

/// @brief A utility class to recursively destroy/copy/move an object with the correct type base on the index of the
///        type in the variant.
/// @note This class should only be used when the type of the object to be constructed is unknown at compile time.
///       Otherwise, @c construct_at should be used.
/// @tparam Ts variadic template type argument of the types
template <typename... Ts>
class variant_util_impl;

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief copy construct @c T object from @c source to @c dest if @c selected is true
/// @tparam T type of object
/// @param selected enables the operation, otherwise function is no-op
/// @param source pointer to the source object
/// @param dest pointer to the destination object
template <typename T>
inline constexpr void copy_construct_object(bool selected, void const* source, void* dest) noexcept(
    std::is_copy_constructible<T>::value
) {
  static_assert(std::is_copy_constructible<T>::value, "Type must be copyable");
  if (selected) {
    construct_at(static_cast<T*>(dest), *static_cast<T const*>(source));
  }
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief copy assign @c T object from @c source to @c dest if @c selected is true
/// @tparam T type of object
/// @param selected enables the operation, otherwise function is no-op
/// @param source pointer to the source object
/// @param dest pointer to the destination object
template <typename T>
inline constexpr void copy_assign_object(bool selected, void const* source, void* dest) noexcept(
    std::is_nothrow_copy_assignable<T>::value
) {
  static_assert(std::is_copy_assignable<T>::value, "Type must be copy-assignable");
  if (selected) {
    *static_cast<T*>(dest) = *static_cast<T const*>(source);
  }
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief move construct @c T object from @c source to @c dest if @c selected is true
/// @tparam T type of object
/// @param selected enables the operation, otherwise function is no-op
/// @param source pointer to the source object
/// @param dest pointer to the destination object
template <typename T>
inline constexpr void move_construct_object(bool selected, void* source, void* dest) noexcept(
    std::is_move_constructible<T>::value
) {
  static_assert(std::is_move_constructible<T>::value, "Type must be movable");
  if (selected) {
    construct_at(static_cast<T*>(dest), static_cast<std::remove_reference_t<T>&&>(*static_cast<T*>(source)));
  }
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief move assign @c T object from @c source to @c dest if @c selected is true
/// @tparam T type of object
/// @param selected enables the operation, otherwise function is no-op
/// @param source pointer to the source object
/// @param dest pointer to the destination object
template <typename T>
inline constexpr void move_assign_object(bool selected, void* source, void* dest) noexcept(
    std::is_move_assignable<T>::value
) {
  static_assert(std::is_move_assignable<T>::value, "Type must be copy-assignable");
  if (selected) {
    *static_cast<T*>(dest) = static_cast<std::remove_reference_t<T>&&>(*static_cast<T*>(source));
  }
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief destroy object @c T object at @c data if @c selected is true
/// @tparam T type of object
/// @param selected enables the operation, otherwise function is no-op
/// @param data address of object to destroy
template <typename T>
inline constexpr void destroy_object(bool selected, void* data) noexcept {
  if (selected) {
    destroy_at(static_cast<T*>(data));
  }
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief check objects pointed at @c lhs and @c rhs of type  @c T for equality if @c selected is true
/// @tparam T type of object
/// @param selected enables the operation, otherwise function is no-op
/// @param lhs pointer to the object on left
/// @param rhs pointer to the object on right
/// @return true if @c selected is true and objects @c rhs and @c lhs are equal
template <typename T>
inline constexpr auto check_object_equality(bool selected, void const* lhs, void const* rhs) noexcept -> bool {
  if (!selected) {
    return false;
  }

  return std::equal_to<T>{}(*static_cast<T const*>(lhs), *static_cast<T const*>(rhs));
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief check objects pointed at @c lhs and @c rhs of type  @c T for less-than ordering if @c selected is true
/// @tparam T type of object
/// @param selected enables the operation, otherwise function is no-op
/// @param lhs pointer to the object on left
/// @param rhs pointer to the object on right
/// @return true if @c selected is true and object @c lhs is less-than object @c rhs
template <typename T>
inline constexpr auto check_object_less_than(bool selected, void const* lhs, void const* rhs) noexcept -> bool {
  if (!selected) {
    return false;
  }

  return std::less<T>{}(*static_cast<T const*>(lhs), *static_cast<T const*>(rhs));
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Recursive specialization of @c variant_util_impl
/// @tparam T template type argument to be processed recursively with member functions
/// @tparam Ts variadic template type argument of the types
template <typename T, typename... Ts>
class variant_util_impl<T, Ts...> {
 public:
  /// @brief Copy construct the object from @c source to @c dest if @c current_idx is equal to @c requested_idx.
  /// Placement new is used to copy construct the object.
  /// @param current_idx the index in @c Ts of the current type
  /// @param requested_idx the index in @c Ts of the type to be copied
  /// @param source pointer to the source object
  /// @param dest pointer to the destination object
  static constexpr void
  copy(std::size_t current_idx, std::size_t requested_idx, void const* source, void* dest) noexcept(
      (std::is_nothrow_copy_constructible<T>::value) &&
      (base::all_of_v<(std::is_nothrow_copy_constructible<Ts>::value)...>)
  ) {
    copy_construct_object<T>(current_idx == requested_idx, source, dest);
    variant_util_impl<Ts...>::copy(current_idx + 1U, requested_idx, source, dest);
  }

  /// @brief Copy assign the object from @c source to @c dest if @c current_idx is equal to @c requested_idx.
  /// @param current_idx the index in @c Ts of the current type
  /// @param requested_idx the index in @c Ts of the type to be copied
  /// @param source pointer to the source object
  /// @param dest pointer to the destination object
  static constexpr void
  copy_assign(std::size_t current_idx, std::size_t requested_idx, void const* source, void* dest) noexcept(
      (std::is_nothrow_copy_assignable<T>::value) && (base::all_of_v<(std::is_nothrow_copy_assignable<Ts>::value)...>)
  ) {
    copy_assign_object<T>(current_idx == requested_idx, source, dest);
    variant_util_impl<Ts...>::copy_assign(current_idx + 1U, requested_idx, source, dest);
  }

  /// @brief Move construct the object from @c source to @c dest if @c current_idx is equal to @c requested_idx.
  /// Placement new is used to move construct the object.
  /// @param current_idx the index in @c Ts of the current type
  /// @param requested_idx the index in @c Ts of the type to be moved
  /// @param source pointer to the source object
  /// @param dest pointer to the destination object
  static constexpr void move(std::size_t current_idx, std::size_t requested_idx, void* source, void* dest) noexcept(
      (std::is_nothrow_move_constructible<T>::value) &&
      (base::all_of_v<(std::is_nothrow_move_constructible<Ts>::value)...>)
  ) {
    move_construct_object<T>(current_idx == requested_idx, source, dest);
    variant_util_impl<Ts...>::move(current_idx + 1U, requested_idx, source, dest);
  }

  /// @brief Move assign the object from @c source to @c dest if @c current_idx is equal to @c requested_idx.
  /// @param current_idx the index in @c Ts of the current type
  /// @param requested_idx the index in @c Ts of the type to be moved
  /// @param source pointer to the source object
  /// @param dest pointer to the destination object
  static constexpr void
  move_assign(std::size_t current_idx, std::size_t requested_idx, void* source, void* dest) noexcept(
      (std::is_nothrow_move_assignable<T>::value) && (base::all_of_v<(std::is_nothrow_move_assignable<Ts>::value)...>)
  ) {
    move_assign_object<T>(current_idx == requested_idx, source, dest);
    variant_util_impl<Ts...>::move_assign(current_idx + 1U, requested_idx, source, dest);
  }

  /// @brief Destroy the input object pointed at @c data if @c current_idx is equal to @c requested_idx.
  /// @param current_idx the index in @c Ts of the current type
  /// @param requested_idx the index in @c Ts of the type to be moved
  /// @param data pointer to the object to destroy
  static constexpr void destroy(std::size_t current_idx, std::size_t requested_idx, void* data) noexcept {
    destroy_object<T>(current_idx == requested_idx, data);
    variant_util_impl<Ts...>::destroy(current_idx + 1U, requested_idx, data);
  }

  /// @brief Check two objects pointed by @c rhs and @c lhs for equality if @c current_idx is equal to @c requested_idx.
  /// @param current_idx the index in @c Ts of the current type
  /// @param requested_idx the index in @c Ts of the type to be moved
  /// @param lhs pointer to the lhs object
  /// @param rhs pointer to the rhs object
  /// @return the result of the comparison
  static constexpr auto
  check_equality(std::size_t current_idx, std::size_t requested_idx, void const* lhs, void const* rhs) noexcept
      -> bool {
    return check_object_equality<T>(current_idx == requested_idx, lhs, rhs) ||
           variant_util_impl<Ts...>::check_equality(current_idx + 1U, requested_idx, lhs, rhs);
  }

  /// @brief Compare two objects pointed by @c rhs and @c lhs for ordering if @c current_idx is equal to @c
  /// requested_idx.
  /// @param current_idx the index in @c Ts of the current type
  /// @param requested_idx the index in @c Ts of the type to be moved
  /// @param lhs pointer to the lhs object
  /// @param rhs pointer to the rhs object
  /// @return the result of the comparison
  static constexpr auto
  check_less_than(std::size_t current_idx, std::size_t requested_idx, void const* lhs, void const* rhs) noexcept
      -> bool {
    return check_object_less_than<T>(current_idx == requested_idx, lhs, rhs) ||
           variant_util_impl<Ts...>::check_less_than(current_idx + 1U, requested_idx, lhs, rhs);
  }
};

/// @brief Specialization of the end of the recursion. No-op.
template <>
class variant_util_impl<> {
 public:
  // parasoft-begin-suppress AUTOSAR-M0_1_8-b "The intent of these function are that they do nothing"

  /// @brief Copy construct object at @c to from object at @c from if @c current_idx equals @c idx
  ///
  /// This is a no-op for the recursive base specialization.
  ///
  static constexpr void
  copy(std::size_t /*current_idx*/, std::size_t /*idx*/, void const* /*from*/, void* /*to*/) noexcept {}

  /// @brief Copy assign object from @c from to @c to if @c current_idx equals @c idx
  ///
  /// This is a no-op for the recursive base specialization.
  ///
  static constexpr void
  copy_assign(std::size_t /*current_idx*/, std::size_t /*idx*/, void const* /*from*/, void* /*to*/) noexcept {}
  /// @brief Move construct object at @c to from object at @c from if @c current_idx equals @c idx
  ///
  /// This is a no-op for the recursive base specialization.
  ///
  static constexpr void move(std::size_t /*current_idx*/, std::size_t /*idx*/, void* /*from*/, void* /*to*/) noexcept {}

  /// @brief Move assign object from @c from to @c to if @c current_idx equals @c idx
  ///
  /// This is a no-op for the recursive base specialization.
  ///
  static constexpr void
  move_assign(std::size_t /*current_idx*/, std::size_t /*idx*/, void* /*from*/, void* /*to*/) noexcept {}

  /// @brief Destroy object at @c data if @c current_idx equals @c idx
  ///
  /// This is a no-op for the recursive base specialization.
  ///
  static constexpr void destroy(std::size_t /*current_idx*/, std::size_t /*idx*/, void* /*data*/) noexcept {}

  // parasoft-end-suppress AUTOSAR-M0_1_8-b

  /// @brief Check two objects pointed by @c rhs and @c lhs for equality if @c current_idx is equal to @c requested_idx.
  /// @param current_idx the index in @c Ts of the current type
  /// @param requested_idx the index in @c Ts of the type to be moved
  /// @param lhs pointer to the lhs object
  /// @param rhs pointer to the rhs object
  /// @return true if @c requested_idx is representing valueless_by_exception state
  static constexpr auto check_equality(
      std::size_t const current_idx,    // NOLINT(bugprone-easily-swappable-parameters)
      std::size_t const requested_idx,  // NOLINT(bugprone-easily-swappable-parameters)
      void const* const lhs,
      void const* const rhs
  ) noexcept -> bool {
    std::ignore = current_idx;
    std::ignore = lhs;
    std::ignore = rhs;
    // two variants in valueless_by_exception state are equal
    return requested_idx == variant_npos;
  }
  /// @brief Compare two objects pointed by @c rhs and @c lhs for ordering if @c current_idx is equal to @c
  /// @param current_idx the index in @c Ts of the current type
  /// @param requested_idx the index in @c Ts of the type to be moved
  /// @param lhs pointer to the lhs object
  /// @param rhs pointer to the rhs object
  /// @return always false
  static constexpr auto check_less_than(
      std::size_t const current_idx,
      std::size_t const requested_idx,
      void const* const lhs,
      void const* const rhs
  ) noexcept -> bool {
    std::ignore = current_idx;
    std::ignore = requested_idx;
    std::ignore = lhs;
    std::ignore = rhs;
    // two variants in valueless_by_exception state are not ordered
    return false;
  }
};

/// @brief A utility class to recursively destroy/copy/move an object with the correct type base on the index of the
///        type in the variant.
/// @note This class should only be used when the type of the object to be constructed is unknown at compile time.
///       Otherwise, @c construct_at should be used.
/// @tparam Ts variadic template type argument of the types
template <typename... Ts>
class variant_util {
 public:
  /// @brief Copy construct the object from @c source to @c dest. Type of the object is @c idx -th from typelist @c Ts
  /// Placement new is used to copy construct the object.
  /// @param idx the index in @c Ts of the current type
  /// @param source pointer to the source object
  /// @param dest pointer to the destination object
  inline static void copy(
      std::size_t idx,
      void const* source,
      void* dest
  ) noexcept(base::all_of_v<std::is_nothrow_copy_constructible<Ts>::value...>) {
    variant_util_impl<Ts...>::copy(0U, idx, source, dest);
  }

  /// @brief Copy assign the object from @c source to @c dest. Type of the object is @c idx -th from typelist @c Ts
  /// @param idx the index in @c Ts of the current type
  /// @param source pointer to the source object
  /// @param dest pointer to the destination object
  inline static void copy_assign(
      std::size_t idx,
      void const* source,
      void* dest
  ) noexcept(base::all_of_v<std::is_nothrow_copy_assignable<Ts>::value...>) {
    variant_util_impl<Ts...>::copy_assign(0U, idx, source, dest);
  }

  /// @brief Move construct the object from @c source to @c dest. Type of the object is @c idx -th from typelist @c Ts
  /// Placement new is used to move construct the object.
  /// @param idx the index in @c Ts of the current type
  /// @param source pointer to the source object
  /// @param dest pointer to the destination object
  inline static void move(
      std::size_t idx,
      void* source,
      void* dest
  ) noexcept(base::all_of_v<std::is_nothrow_move_constructible<Ts>::value...>) {
    variant_util_impl<Ts...>::move(0U, idx, source, dest);
  }

  /// @brief Move assign the object from @c source to @c dest. Type of the object is @c idx -th from typelist @c Ts
  /// @param idx the index in @c Ts of the current type
  /// @param source pointer to the source object
  /// @param dest pointer to the destination object
  inline static void move_assign(
      std::size_t idx,
      void* source,
      void* dest
  ) noexcept(base::all_of_v<std::is_nothrow_move_assignable<Ts>::value...>) {
    variant_util_impl<Ts...>::move_assign(0U, idx, source, dest);
  }

  /// @brief Destroy the input object pointed at @c data. Type of the object is @c idx -th from typelist @c Ts
  /// @param idx the index in @c Ts of the current type
  /// @param data pointer to the object to destroy
  inline static void destroy(std::size_t idx, void* data) noexcept { variant_util_impl<Ts...>::destroy(0U, idx, data); }

  /// @brief Check two objects pointed by @c rhs and @c lhs for equality. Type of the objects is @c idx -th from
  /// typelist @c Ts
  /// @param idx the index in @c Ts of the current type
  /// @param lhs pointer to the lhs object
  /// @param rhs pointer to the rhs object
  /// @return true if objects @c lhs and @c rhs are equal or @c idx represents valueless-by-exception state
  inline static auto check_equality(std::size_t idx, void const* lhs, void const* rhs) noexcept -> bool {
    return variant_util_impl<Ts...>::check_equality(0U, idx, lhs, rhs);
  }

  /// @brief Check two objects pointed by @c rhs and @c lhs for less-than ordering. Type of the objects is @c idx -th
  /// from typelist @c Ts
  /// @param idx the index in @c Ts of the current type
  /// @param lhs pointer to the lhs object
  /// @param rhs pointer to the rhs object
  /// @return true if object @c lhs is less than @c rhs or false when @c idx represents valueless-by-exception state
  inline static auto check_less_than(std::size_t idx, void const* lhs, void const* rhs) noexcept -> bool {
    return variant_util_impl<Ts...>::check_less_than(0U, idx, lhs, rhs);
  }
};

/// @brief Forward declaration. Helper to check if a given type can be constructed from the @c I 'th type in a variant.
/// @tparam Variant the variant type
/// @tparam I the index of the type to be checked
template <class Variant, std::size_t I = 0>
class convertible_checker;

/// @brief Specialization of the empty(end of recursion).
/// @tparam I always equal to the size of the real variant
template <std::size_t I>
class convertible_checker<variant<>, I> {
 public:
  /// @brief function used to determine if a type is convertible to a variant alternative
  ///
  /// This overload is defined in the recursive base so that all
  /// convertible_checker specializations declare @c run
  static void run() = delete;
};

/// @brief A recursive specialization of the convertible_checker.
///
/// This class has two static member functions @c run for each type in the variant. @c accepted_index will use the
/// overload resolution rule to determine which type of the variant is the best match for the type given in the convert
/// constructor of the variant. It is a compile error if two or more types in the variant can be converted from the
/// given type with the same level of match.
/// @tparam H the head type of the variant
/// @tparam Ts the tail types of the variant
/// @tparam I the index of the type to be checked
template <class H, class... Ts, std::size_t I>
class convertible_checker<variant<H, Ts...>, I> : convertible_checker<variant<Ts...>, I + 1> {
 public:
  using convertible_checker<variant<Ts...>, I + 1>::run;

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief undefined function used to determine if a type is convertible to a variant alternative
  /// @return integral constant containing the alternative index
  static auto run(H const&, std::integral_constant<std::size_t, I>) -> std::integral_constant<std::size_t, I>;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief undefined function used to determine if a type is convertible to a variant alternative
  /// @return integral constant containing the alternative index
  static auto run(H&&, std::integral_constant<std::size_t, I>) -> std::integral_constant<std::size_t, I>;
  // parasoft-end-suppress CERT_C-EXP37-a-3
};

/// @brief Helper to determine which type in the variant can be constructed from the given type.
/// @tparam Variant the variant type
/// @tparam T the type to be checked
template <class Variant, class T, class = void>
class accepted_index {};

/// @brief Implementation of @c accepted_index .
/// @tparam Variant the variant type
/// @tparam T the type to be checked
template <class Variant, class T>
class accepted_index<Variant, T, void_t<decltype(convertible_checker<Variant>::run(std::declval<T>(), {}))>>
    : public decltype(convertible_checker<Variant>::run(std::declval<T>(), {})) {};

}  // namespace variant_detail

// parasoft-begin-suppress AUTOSAR-A12_0_1-a "False Positive: Copy constructor is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_1_5-a "Delegating constructors would not reduce duplication"
/// @brief a backport of C++17's @c std::variant
/// @tparam Ts The set of alternatives that can be held by the variant.
template <typename... Ts>
// Justification: False positive about copy constructor not generated
// NOLINTNEXTLINE(hicpp-special-member-functions)
class variant : full_ordering_operators_from_less_than_and_equals<variant<Ts...>> {
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'util_type' does not hide another identifier"
  /// @brief Alias to a utility class for this variant type.
  using util_type = variant_detail::variant_util<Ts...>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'nth_type' does not hide another identifier"
  /// @brief Obtains the alternative type for a given index
  /// @tparam Index alternative type index
  template <std::size_t Index>
  using nth_type = type_lists::at_t<type_list<Ts...>, Index>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  static_assert(sizeof...(Ts) > 0, "Empty variant is not allowed");
  static_assert(base::all_of_v<!std::is_reference<Ts>::value...>, "variant must have no reference alternative");

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'num_types' does not hide another identifier"
  /// @brief Number of alternative types
  static constexpr std::size_t num_types{sizeof...(Ts)};
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'max_size' does not hide another identifier"
  /// @brief The maximum size of the alternative types
  static constexpr std::size_t max_size{std::max({sizeof(Ts)...})};
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'max_align' does not hide another identifier"
  /// @brief The maximum alignment of the alternative types
  static constexpr std::size_t max_align{std::max({alignof(Ts)...})};
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief The type of the argument to the "copy assignment operator".
  ///
  /// Either @c variant to provide a copy assignment when all the alternative types are copy constructible and copy
  /// assignable, or @c non_constructible_dummy otherwise.
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'copy_assignment_arg' does not hide another identifier"
  using copy_assignment_arg = conditional_t<
      base::all_of_v<std::is_copy_constructible<Ts>::value...> && base::all_of_v<std::is_copy_assignable<Ts>::value...>,
      variant,
      non_constructible_dummy>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief the type of the argument to the "copy assignment operator". The opposite to @c copy_assignment_arg .
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'dummy_copy_assign_source' does not hide another
  // identifier"
  using dummy_copy_assign_source = conditional_t<
      base::all_of_v<std::is_copy_constructible<Ts>::value...> && base::all_of_v<std::is_copy_assignable<Ts>::value...>,
      non_constructible_dummy,
      variant>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief The type of the argument to the "copy constructor".
  ///
  /// Either @c variant to provide a copy ctor when all the alternative types are copy constructible, or
  /// @c non_constructible_dummy otherwise.
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'copy_ctor_arg' does not hide another identifier"
  using copy_ctor_arg =
      conditional_t<base::all_of_v<std::is_copy_constructible<Ts>::value...>, variant, non_constructible_dummy>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief The type of the argument to the "copy constructor". The opposite to @c copy_ctor_arg.
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'dummy_copy_ctor_source' does not hide another
  // identifier"
  using dummy_copy_ctor_source =
      conditional_t<base::all_of_v<std::is_copy_constructible<Ts>::value...>, non_constructible_dummy, variant>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief The type of the argument to the "move assignment operator".
  ///
  /// Either @c variant to provide a move assignment when all the alternative types are move constructible and move
  /// assignable, or @c non_constructible_dummy otherwise.
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'move_assignment_arg' does not hide another identifier"
  using move_assignment_arg = conditional_t<
      base::all_of_v<std::is_move_constructible<Ts>::value...> && base::all_of_v<std::is_move_assignable<Ts>::value...>,
      variant,
      non_constructible_dummy>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief The type of the argument to the "move assignment operator". The opposite to @c move_assignment_arg.
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'dummy_move_assign_source' does not hide another
  // identifier"
  using dummy_move_assign_source = conditional_t<
      base::all_of_v<std::is_move_constructible<Ts>::value...> && base::all_of_v<std::is_move_assignable<Ts>::value...>,
      non_constructible_dummy,
      variant>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief The type of the argument to the "move constructor".
  ///
  /// Either @c variant to provide a move ctor when all the alternative types are move constructible, or
  /// @c non_constructible_dummy otherwise.
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'move_ctor_arg' does not hide another identifier"
  using move_ctor_arg =
      conditional_t<base::all_of_v<std::is_move_constructible<Ts>::value...>, variant, non_constructible_dummy>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief The type of the argument to the "move constructor". The opposite to @c move_ctor_arg.
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'dummy_move_ctor_source' does not hide another
  // identifier"
  using dummy_move_ctor_source =
      conditional_t<base::all_of_v<std::is_move_constructible<Ts>::value...>, non_constructible_dummy, variant>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

 public:
  /// @brief Default constructor, which initializes the variant to a default constructed instance of its 0'th
  /// alternative.
  ///
  /// @pre Only participates in overload resolution if the first type in @c Ts is default constructible.
  template <
      typename T0 = nth_type<0>,
      constraints<std::enable_if_t<std::is_default_constructible<T0>::value>> = nullptr>
  variant() noexcept(std::is_nothrow_default_constructible<T0>::value)
      : full_ordering_operators_from_less_than_and_equals<variant<Ts...>>{},
        idx_{} {
    // Justification: It's intended to use reinterpret_cast to construct
    // the object on an uninitialized address.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    construct_at(reinterpret_cast<T0*>(data_.data()));
  }

  /// @brief Copy Constructor from another variant of the same alternative types.
  ///
  /// If the other variant is valueless, this variant will also be valueless.
  /// @pre Only participates in overload resolution if every type in @c Ts is copy constructible.
  /// @param other the other variant to copy from
  // parasoft-begin-suppress AUTOSAR-A13_3_1-a "False positive: No forwarding reference"
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  variant(copy_ctor_arg const& other) noexcept(arene::base::all_of_v<std::is_nothrow_copy_constructible<Ts>::value...>)
      : full_ordering_operators_from_less_than_and_equals<variant<Ts...>>{},
        idx_{other.idx_} {
    // util_type::copy is no-op for valueless-by-exception
    util_type::copy(other.idx_, other.data_.data(), data_.data());
  }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a

  /// @brief Copy Constructor from another variant of the same alternative types.
  ///
  /// If the other variant is valueless, this variant will also be valueless.
  /// @pre Only participates in overload resolution if every type in @c Ts is copy constructible.
  /// @param other the other variant to copy from
  variant(dummy_copy_ctor_source const& other) = delete;

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2  "False positive: The data is moved via the 'util_type::move' function"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2  "False positive: The data is moved via the 'util_type::move' function"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: The data is moved via the 'util_type::move' function"
  /// @brief Move Constructor from another variant.
  ///
  /// If the other variant is valueless, this variant will also be valueless.
  /// @pre Only participates in overload resolution if every type in @c Ts is move constructible.
  /// @param other the other variant to move from
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  variant(move_ctor_arg&& other) noexcept(base::all_of_v<std::is_nothrow_move_constructible<Ts>::value...>)
      : full_ordering_operators_from_less_than_and_equals<variant<Ts...>>{},
        idx_{other.idx_} {
    // util_type::move is no-op for valueless-by-exception
    util_type::move(other.idx_, other.data_.data(), data_.data());
  }
  // parasoft-env-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-env-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-env-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-env-suppress AUTOSAR-A15_5_1-b-2

  /// @brief Move Constructor form another variant.
  ///
  /// If the other variant is valueless, this variant will also be valueless.
  /// @pre Only participates in overload resolution if every type in @c Ts is move constructible.
  /// @param other the other variant to move from
  variant(dummy_move_ctor_source&& other) = delete;

  // parasoft-begin-suppress AUTOSAR-A2_7_2-a-2 "False positive: no commented-out code"
  /// @brief Constructor that initializes this variant from one of its alternative types.
  ///
  /// This constructor will pick the constructor overload of the alternative type @c Tj that <c> T_i x[] = {
  /// std::forward<T>(value) }; </c> is valid. If more than one alternative type satisfies this condition, the
  /// compilation will fail.
  /// @tparam T the type to construct this variant
  /// @tparam I the deduced index of the alternative type that T can construct
  /// @tparam Tj the deduced alternative type that T can construct
  /// @param value the object to forward into the variant
  // parasoft-end-suppress AUTOSAR-A2_7_2-a-2
  template <
      typename T,
      std::size_t I = variant_detail::accepted_index<variant, T>::type::value,
      typename Tj = nth_type<I>,
      constraints<
          std::enable_if_t<!std::is_same<typename std::decay<T>::type, variant>::value>,
          std::enable_if_t<std::is_constructible<Tj, T>::value>> = nullptr>
  // Justification: The forwarding reference constructor won't hide copy/move
  // constructor since the constraints ensures T cannot be another variant.
  // Justification: std::variant doesn't require this to be explicit.
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload,google-explicit-constructor,hicpp-explicit-conversions)
  variant(T&& value) noexcept(std::is_nothrow_constructible<Tj, T>::value)
      : full_ordering_operators_from_less_than_and_equals<variant<Ts...>>{},
        idx_(I) {
    // Justification: It's intended to use reinterpret_cast to construct
    // the object on an uninitialized address.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    construct_at(reinterpret_cast<Tj*>(data_.data()), std::forward<T>(value));
  }

  /// @brief Copy Assignment operator from another variant of the same alternative types.
  ///
  /// If the other type is valueless, or exception is thrown during copying, this variant will also be valueless.
  /// @pre Only participates in overload resolution if every type in @c Ts is copy-assignable.
  /// @param rhs the other variant to copy from
  auto operator=(copy_assignment_arg const& rhs
  ) noexcept(base::all_of_v<std::is_nothrow_copy_constructible<Ts>::value...> && base::all_of_v<std::is_nothrow_copy_assignable<Ts>::value...>)
      -> variant& {
    if (this != &rhs) {
      if (index() != rhs.index()) {  // different
        destroy();
        util_type::copy(rhs.idx_, rhs.data_.data(), data_.data());  // no-op for valueless-by-exception
        idx_ = rhs.idx_;
      } else {
        util_type::copy_assign(rhs.idx_, rhs.data_.data(), data_.data());  // no-op for valueless-by-exception
      }
    }

    return *this;
  }
  /// @brief Copy Assignment operator from another variant of the same alternative types.
  ///
  /// If the other type is valueless, or exception is thrown during copying, this variant will also be valueless.
  /// @pre Only participates in overload resolution if every type in @c Ts is copy-assignable.
  /// @param rhs the other variant to copy from
  auto operator=(dummy_copy_assign_source const& rhs) -> variant& = delete;

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2  "False positive: The data is moved via the 'util_type::move*' function"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2  "False positive: The data is moved via the 'util_type::move*' function"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: The data is moved via the 'util_type::move*' function"
  /// @brief Move Assignment operator from another variant of the same alternative types.
  ///
  /// If the other type is valueless, this variant will also be valueless.
  /// @pre Only participates in overload resolution if every type in @c Ts is move-assignable.
  /// @param rhs the other variant to move from
  auto operator=(move_assignment_arg&& rhs
  ) noexcept(base::all_of_v<std::is_nothrow_move_constructible<Ts>::value...> && base::all_of_v<std::is_nothrow_move_assignable<Ts>::value...>)
      -> variant& {
    if (this != &rhs) {
      if (index() != rhs.index()) {
        destroy();
        util_type::move(rhs.idx_, rhs.data_.data(), data_.data());  // no-op for valueless-by-exception
        idx_ = rhs.idx_;
      } else {
        util_type::move_assign(rhs.idx_, rhs.data_.data(), data_.data());  // no-op for valueless-by-exception
      }
    }

    return *this;
  }

  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2

  /// @brief Move Assignment operator from another variant of the same alternative types.
  ///
  /// If the other type is valueless, this variant will also be valueless.
  /// @pre Only participates in overload resolution if every type in @c Ts is move-assignable.
  /// @param rhs the other variant to move from
  auto operator=(dummy_move_assign_source&& rhs) -> variant& = delete;

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  /// @brief Equality comparison
  /// @pre The behavior is undefined if not all the alternatives support equality comparison.
  /// @param lhs variant to compare
  /// @param rhs the other variant to compare
  /// @return true if both variants are valueless, or both variants hold the same alternative type and the value of that
  ///         alternative type is equal.
  /// @return false otherwise.
  friend auto operator==(variant const& lhs, variant const& rhs) noexcept -> bool {
    if (lhs.index() != rhs.index()) {
      return false;
    }

    // util_type::check_equality returns true for valueless-by-exception state
    return util_type::check_equality(
        lhs.idx_,
        static_cast<void const*>(lhs.data_.data()),
        static_cast<void const*>(rhs.data_.data())
    );
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  /// @brief Less-than comparison
  /// @pre The behavior is undefined if not all the alternatives support less than comparison.
  /// @param lhs variant to compare
  /// @param rhs the other variant to compare
  /// @return false if either variant is valueless
  /// @return Otherwise, returns the result of invoking @c std::less on the held alternatives of the two variants.
  friend auto operator<(variant const& lhs, variant const& rhs) noexcept -> bool {
    if (lhs.index() != rhs.index()) {
      return (lhs.index() + 1U) < (rhs.index() + 1U);
    }

    // util_type::check_less_than returns false for valueless-by-exception state
    return util_type::check_less_than(
        lhs.idx_,
        static_cast<void const*>(lhs.data_.data()),
        static_cast<void const*>(rhs.data_.data())
    );
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  /// @brief Get the index of the alternative type that this variant currently holds.
  /// @return std::size_t the index of the alternative type that this variant currently holds, or @c variant_npos if the
  ///         variant is valueless.
  auto index() const noexcept -> std::size_t { return idx_; }

  /// @brief Emplaces a value into the variant by index.
  /// @tparam I the index of the alternative type to emplace
  /// @tparam Args the variadic argument type to pass into the constructor of
  /// the alternative type
  /// @param args Arguments to be forwarded to the constructor of the emplaced type.
  /// @return A reference to the emplaced object.
  /// @post If the variant was not valueless, the currently held value is destroyed
  /// @post The variant will hold the value the returned reference points to.
  /// @post @c holds_alternative<I>(*this) will return true
  /// @post index() will return @c I
  template <std::size_t I, typename... Args>
  auto emplace(Args&&... args) noexcept(std::is_nothrow_constructible<nth_type<I>, Args...>::value) -> nth_type<I>& {
    destroy();
    using selected_type = nth_type<I>;
    // Justification: It's intended to use reinterpret_cast to construct
    // the object on an uninitialized address.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    construct_at(reinterpret_cast<selected_type*>(data_.data()), std::forward<Args>(args)...);
    idx_ = I;
    return get<I>();
  }

  /// @brief Emplaces a value into the variant by index.
  /// @tparam I the index of the alternative type to emplace
  /// @tparam Args the variadic argument type to pass into the constructor of
  /// the alternative type
  /// @param init An initializer list to forward to the constructor of the emplaced type.
  /// @param args Arguments to be forwarded to the constructor of the emplaced type.
  /// @return A reference to the emplaced object.
  /// @post If the variant was not valueless, the currently held value is destroyed
  /// @post The variant will hold the value the returned reference points to.
  /// @post @c holds_alternative<I>(*this) will return true
  /// @post index() will return @c I
  template <std::size_t I, typename U, typename... Args>
  auto emplace(std::initializer_list<U> init, Args&&... args) noexcept(
      std::is_nothrow_constructible<nth_type<I>, std::initializer_list<U>, Args...>::value
  ) -> nth_type<I>& {
    destroy();
    using selected_type = nth_type<I>;
    // Justification: It's intended to use reinterpret_cast to construct
    // the object on an uninitialized address.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    construct_at(reinterpret_cast<selected_type*>(data_.data()), init, std::forward<Args>(args)...);
    idx_ = I;
    return get<I>();
  }

  /// @brief Emplaces a value into the variant by type.
  /// @tparam T the alternative type to emplace
  /// @param args Arguments to be forwarded to the constructor of the emplaced type.
  /// @return A reference to the emplaced object.
  /// @post If the variant was not valueless, the currently held value is destroyed
  /// @post The variant will hold the value the returned reference points to.
  /// @post @c holds_alternative<T>(*this) will return true
  /// @post index() will return the the equivalent of <c> index_of_v<T, Ts...> </c>
  template <typename T, typename... Args>
  auto emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T& {
    destroy();
    // Justification: It's intended to use reinterpret_cast to construct
    // the object on an uninitialized address.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    construct_at(reinterpret_cast<T*>(data_.data()), std::forward<Args>(args)...);
    idx_ = base::index_of_v<T, Ts...>;
    return get<T>();
  }

  /// @brief Emplaces a value into the variant by type.
  /// @tparam T the alternative type to emplace
  /// @param init An initializer list to forward to the constructor of the emplaced type.
  /// @param args Arguments to be forwarded to the constructor of the emplaced type.
  /// @return A reference to the emplaced object.
  /// @post If the variant was not valueless, the currently held value is destroyed
  /// @post The variant will hold the value the returned reference points to.
  /// @post @c holds_alternative<T>(*this) will return true
  /// @post index() will return the the equivalent of <c> index_of_v<T, Ts...> </c>
  template <typename T, typename U, typename... Args>
  auto emplace(std::initializer_list<U> init, Args&&... args) noexcept(
      std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>::value
  ) -> T& {
    destroy();
    // Justification: It's intended to use reinterpret_cast to construct
    // the object on an uninitialized address.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    construct_at(reinterpret_cast<T*>(data_.data()), init, std::forward<Args>(args)...);
    idx_ = base::index_of_v<T, Ts...>;
    return get<T>();
  }

  /// @brief Get by type.
  /// @tparam T the alternative type to get
  /// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type of the object currently held
  /// by the variant does not match the type requested.
  /// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type of the object currently held by the variant must
  /// match the type requested else @c ARENE_PRECONDITION violation.
  /// @return T& The reference to the held object.
  template <typename T>
  auto get() & noexcept(!detail::are_exceptions_enabled_v) -> T& {
    return get<base::index_of_v<T, Ts...>>();
  }
  /// @brief Get by type.
  /// @tparam T the alternative type to get
  /// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type of the object currently held
  /// by the variant does not match the type requested.
  /// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type of the object currently held by the variant must
  /// match the type requested else @c ARENE_PRECONDITION violation.
  /// @return T const& The reference to the held object.
  template <typename T>
  auto get() const& noexcept(!detail::are_exceptions_enabled_v) -> T const& {
    return get<base::index_of_v<T, Ts...>>();
  }

  /// @brief Get by type.
  /// @tparam T the alternative type to get
  /// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type of the object currently held
  /// by the variant does not match the type requested.
  /// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type of the object currently held by the variant must
  /// match the type requested else @c ARENE_PRECONDITION violation.
  /// @return T&& The reference to the held object.
  template <typename T>
  auto get() && noexcept(!detail::are_exceptions_enabled_v) -> T&& {
    return std::move(get<base::index_of_v<T, Ts...>>());
  }
  /// @brief Get by type.
  /// @tparam T the alternative type to get
  /// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type of the object currently held
  /// by the variant does not match the type requested.
  /// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type of the object currently held by the variant must
  /// match the type requested else @c ARENE_PRECONDITION violation.
  /// @return T const&& The reference to the held object.
  template <typename T>
  auto get() const&& noexcept(!detail::are_exceptions_enabled_v) -> T const&& {
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a "Must maintain the reference type of the return value"
    return std::move(get<base::index_of_v<T, Ts...>>());
    // parasoft-end-suppress AUTOSAR-A18_9_3-a "Must maintain the reference type of the return value"
  }

  /// @brief Get the object stored in this variant by the index
  /// @tparam Index the index into @c Ts of the alternative to get
  /// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type index of the object
  /// currently held by the variant does not match the index requested.
  /// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type index of the object currently held by the variant must
  /// match the type requested else @c ARENE_PRECONDITION violation.
  /// @return nth_type<Index>& The reference to the held object.
  template <std::size_t Index>
  auto get() & noexcept(!detail::are_exceptions_enabled_v) -> nth_type<Index>& {
    return get_impl<Index, nth_type<Index>&>(*this);
  }

  /// @brief Get the object stored in this variant by the index
  /// @tparam Index the index into @c Ts of the alternative to get
  /// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type index of the object
  /// currently held by the variant does not match the index requested.
  /// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type index of the object currently held by the variant must
  /// match the type requested else @c ARENE_PRECONDITION violation.
  /// @return nth_type<Index> const& The reference to the held object.
  template <std::size_t Index>
  auto get() const& noexcept(!detail::are_exceptions_enabled_v) -> nth_type<Index> const& {
    return get_impl<Index, nth_type<Index> const&>(*this);
  }

  /// @brief Get the object stored in this variant by the index
  /// @tparam Index the index into @c Ts of the alternative to get
  /// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type index of the object
  /// currently held by the variant does not match the index requested.
  /// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type index of the object currently held by the variant must
  /// match the type requested else @c ARENE_PRECONDITION violation.
  /// @return nth_type<Index>&& The reference to the held object.
  template <std::size_t Index>
  auto get() && noexcept(!detail::are_exceptions_enabled_v) -> nth_type<Index>&& {
    return get_impl<Index, nth_type<Index>&&>(std::move(*this));
  }

  /// @brief Get the object stored in this variant by the index
  /// @tparam Index the index into @c Ts of the alternative to get
  /// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type index of the object
  /// currently held by the variant does not match the index requested.
  /// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type index of the object currently held by the variant must
  /// match the type requested else @c ARENE_PRECONDITION violation.
  /// @return nth_type<Index> const&& The reference to the held object.
  template <std::size_t Index>
  auto get() const&& noexcept(!detail::are_exceptions_enabled_v) -> nth_type<Index> const&& {
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a "Must maintain the reference type of the return value"
    return get_impl<Index, nth_type<Index> const&&>(std::move(*this));
    // parasoft-end-suppress AUTOSAR-A18_9_3-a "Must maintain the reference type of the return value"
  }

  /// @brief Check if the variant is valueless.
  ///
  /// A variant becomes valueless if and only if an exception is thrown during copy or move initialization
  /// @return true if the variant does not hold a value
  /// @return false if the variant holds a value.
  constexpr auto valueless_by_exception() const noexcept -> bool { return idx_ == variant_npos; }

  /// @brief Calls the destructor of the alternative type.
  ~variant() { destroy(); }

 private:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'get_impl' does not hide another identifier"
  /// @brief Get the object stored in this variant by the index
  /// @tparam Index the index into @c Ts of the alternative to get
  /// @tparam Alternative (const-qualified) alternative type for Index
  /// @tparam Self (const-qualified) variant type
  /// @param self reference to (const-qualified) variant type
  /// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type index of the object
  /// currently held by the variant does not match the index requested.
  /// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type index of the object currently held by the variant must
  /// match the type index requested else @c ARENE_PRECONDITION violation.
  /// @return The reference to the held object.
  template <std::size_t Index, typename AlternativeRefT, typename Self>
  static auto get_impl(Self&& self) noexcept(!detail::are_exceptions_enabled_v) -> AlternativeRefT {
    variant_detail::variant_must_have_value(Index, self.idx_);
    ARENE_IGNORE_START();
    ARENE_IGNORE_ALL(
        "-Wstrict-aliasing",
        "It's safe to use reinterpret_cast here since the check above ensures that the type of the object "
        "currently held by the variant is the same as the type requested."
    );
    return ::arene::base::forward_like<Self>(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        *reinterpret_cast<std::remove_reference_t<AlternativeRefT>*>(std::forward<Self>(self).data_.data())
    );
    ARENE_IGNORE_END();
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief Destroy the object held by this variant if it's not valueless.
  void destroy() noexcept {
    // util_type::destroy is no-op for valueless-by-exception
    util_type::destroy(idx_, data_.data());
    idx_ = variant_npos;
  }

  /// @brief index of the active alternative
  std::size_t idx_{variant_npos};
  /// @brief actual type and data storage
  alignas(max_align) array<byte, max_size> data_{};
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a
// parasoft-end-suppress AUTOSAR-A12_0_1-a

/// @brief Get value held by a variant by type.
/// @tparam T the alternative type to get
/// @param var variant to get from
/// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type of the object currently held
/// by the variant does not match the type requested.
/// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type of the object currently held by the variant must
/// match the type requested else @c ARENE_PRECONDITION violation.
/// @return The reference to the held object.
template <typename T, typename... Ts>
inline constexpr auto get(variant<Ts...>& var) noexcept(!detail::are_exceptions_enabled_v) -> T& {
  static_assert(!std::is_void<T>::value, "T should not be void");
  return var.template get<T>();
}

/// @brief Get value held by a variant by type.
/// @tparam T the alternative type to get
/// @param var variant to get from
/// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type of the object currently held
/// by the variant does not match the type requested.
/// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type of the object currently held by the variant must
/// match the type requested else @c ARENE_PRECONDITION violation.
/// @return The reference to the held object.
template <typename T, typename... Ts>
inline constexpr auto get(variant<Ts...> const& var) noexcept(!detail::are_exceptions_enabled_v) -> T const& {
  static_assert(!std::is_void<T>::value, "T should not be void");
  return var.template get<T>();
}

/// @brief Get value held by a variant by type.
/// @tparam T the alternative type to get
/// @param var variant to get from
/// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type of the object currently held
/// by the variant does not match the type requested.
/// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type of the object currently held by the variant must
/// match the type requested else @c ARENE_PRECONDITION violation.
/// @return The reference to the held object.
template <typename T, typename... Ts>
inline constexpr auto get(variant<Ts...>&& var) noexcept(!detail::are_exceptions_enabled_v) -> T&& {
  static_assert(!std::is_void<T>::value, "T should not be void");
  return std::move(var).template get<T>();
}

/// @brief Get value held by a variant by type.
/// @tparam T the alternative type to get
/// @param var variant to get from
/// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type of the object currently held
/// by the variant does not match the type requested.
/// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type of the object currently held by the variant must
/// match the type requested else @c ARENE_PRECONDITION violation.
/// @return The reference to the held object.
template <typename T, typename... Ts>
inline constexpr auto get(variant<Ts...> const&& var) noexcept(!detail::are_exceptions_enabled_v) -> T const&& {
  static_assert(!std::is_void<T>::value, "T should not be void");
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a "Must maintain the reference type of the return value"
  return std::move(var).template get<T>();
  // parasoft-end-suppress AUTOSAR-A18_9_3-a "Must maintain the reference type of the return value"
}

/// @brief Get value held by a variant by index.
/// @tparam Index the index into @c Ts of the alternative to get
/// @param var variant to get from
/// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type index of the object currently
/// held by the variant does not match the index requested.
/// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type index of the object currently held by the variant must
/// match the type requested else @c ARENE_PRECONDITION violation.
/// @return The reference to the held object.
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."
template <std::size_t I, typename... Ts>
constexpr auto get(variant<Ts...>& var) noexcept(!detail::are_exceptions_enabled_v)
    -> variant_alternative_t<I, variant<Ts...>>& {
  static_assert(I < sizeof...(Ts), "The index should be in [0, number of alternatives)");
  return var.template get<I>();
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."
/// @brief Get value held by a variant by index.
/// @tparam Index the index into @c Ts of the alternative to get
/// @param var variant to get from
/// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type index of the object currently
/// held by the variant does not match the index requested.
/// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type index of the object currently held by the variant must
/// match the type requested else @c ARENE_PRECONDITION violation.
/// @return The reference to the held object.
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."
template <std::size_t I, typename... Ts>
constexpr auto get(variant<Ts...> const& var) noexcept(!detail::are_exceptions_enabled_v)
    -> variant_alternative_t<I, variant<Ts...>> const& {
  static_assert(I < sizeof...(Ts), "The index should be in [0, number of alternatives)");
  return var.template get<I>();
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."

/// @brief Get value held by a variant by index.
/// @tparam Index the index into @c Ts of the alternative to get
/// @param var variant to get from
/// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type index of the object currently
/// held by the variant does not match the index requested.
/// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the index type of the object currently held by the variant must
/// match the type requested else @c ARENE_PRECONDITION violation.
/// @return The reference to the held object.
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."
template <std::size_t I, typename... Ts>
constexpr auto get(variant<Ts...>&& var) noexcept(!detail::are_exceptions_enabled_v)
    -> variant_alternative_t<I, variant<Ts...>>&& {
  static_assert(I < sizeof...(Ts), "The index should be in [0, number of alternatives)");
  return std::move(var).template get<I>();
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."
/// @brief Get value held by a variant by index.
/// @tparam Index the index into @c Ts of the alternative to get
/// @param var variant to get from
/// @throw bad_variant_access if @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) and if the type index of the object currently
/// held by the variant does not match the index requested.
/// @pre If @c ARENE_IS_OFF(ARENE_EXCEPTIONS_ENABLED) the type index of the object currently held by the variant must
/// match the type requested else @c ARENE_PRECONDITION violation.
/// @return The reference to the held object.
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."
template <std::size_t I, typename... Ts>
constexpr auto get(variant<Ts...> const&& var) noexcept(!detail::are_exceptions_enabled_v)
    -> variant_alternative_t<I, variant<Ts...>> const&& {
  static_assert(I < sizeof...(Ts), "The index should be in [0, number of alternatives)");
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a "Must maintain the reference type of the return value"
  return std::move(var).template get<I>();
  // parasoft-end-suppress AUTOSAR-A18_9_3-a "Must maintain the reference type of the return value"
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive, this is not a member function and thus cannot be static."

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Check if the variant holds a value of a given alternative type.
/// @tparam T the alternative type to check
/// @tparam Ts the types that the variant can hold
/// @param var the variant to check
/// @return true if and only if the variant holds a value of type T.
/// @return false otherwise.
/// @pre Ill-formed if @c T does not appear exactly once in @c Ts .
template <class T, class... Ts>
constexpr auto holds_alternative(variant<Ts...> const& var) noexcept -> bool {
  return var.index() == base::index_of_v<T, Ts...>;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Attempt to get a pointer to the object held by the variant by index.
/// @tparam I Index of the type in @c Ts to get
/// @tparam Ts the types that the variant can hold
/// @param ptr A pointer to the variant to access
/// @return A pointer to the held object if the variant holds a value of the type, else @c nullptr .
template <std::size_t I, typename... Ts>
inline constexpr auto get_if(variant<Ts...>* ptr) noexcept
    -> std::add_pointer_t<variant_alternative_t<I, variant<Ts...>>> {
  using type = variant_alternative_t<I, variant<Ts...>>;
  static_assert(I < sizeof...(Ts), "The index should be in [0, number of alternatives)");
  static_assert(!std::is_void<type>::value, "T should not be void");
  if ((ptr != nullptr) && (ptr->index() == I)) {
    return std::addressof(ptr->template get<I>());
  }
  return nullptr;
}

/// @brief Attempt to get a pointer to the object held by the variant by index.
/// @tparam I Index of the type in @c Ts to get
/// @tparam Ts the types that the variant can hold
/// @param ptr A pointer to the variant to access
/// @return A pointer to the held object if the variant holds a value of the type, else @c nullptr .
template <std::size_t I, typename... Ts>
inline constexpr auto get_if(variant<Ts...> const* ptr) noexcept
    -> std::add_pointer_t<variant_alternative_t<I, variant<Ts...>> const> {
  using type = variant_alternative_t<I, variant<Ts...>>;
  static_assert(I < sizeof...(Ts), "The index should be in [0, number of alternatives)");
  static_assert(!std::is_void<type>::value, "T should not be void");
  if ((ptr != nullptr) && (ptr->index() == I)) {
    return std::addressof(ptr->template get<I>());
  }
  return nullptr;
}

/// @brief Attempt to get a pointer to the object held by the variant by type.
/// @tparam T Type in @c Ts to get
/// @tparam Ts the types that the variant can hold
/// @param ptr A pointer to the variant to access
/// @return A pointer to the held object if the variant holds a value of the type, else @c nullptr .
template <typename T, typename... Ts>
inline constexpr auto get_if(variant<Ts...>* ptr) noexcept -> std::add_pointer_t<T> {
  static_assert(!std::is_void<T>::value, "T should not be void");
  return get_if<base::index_of_v<T, Ts...>>(ptr);
}
/// @brief Attempt to get a pointer to the object held by the variant by type.
/// @tparam T Type in @c Ts to get
/// @tparam Ts the types that the variant can hold
/// @param ptr A pointer to the variant to access
/// @return A pointer to the held object if the variant holds a value of the type, else @c nullptr .
template <typename T, typename... Ts>
inline constexpr auto get_if(variant<Ts...> const* ptr) noexcept -> std::add_pointer_t<T const> {
  static_assert(!std::is_void<T>::value, "T should not be void");
  return get_if<base::index_of_v<T, Ts...>>(ptr);
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_VARIANT_HPP_
