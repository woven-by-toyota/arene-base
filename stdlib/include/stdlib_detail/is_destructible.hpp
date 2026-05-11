// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_DESTRUCTIBLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_DESTRUCTIBLE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstdint.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_reference.hpp"
#include "stdlib/include/stdlib_detail/remove_all_extents.hpp"
#include "stdlib/include/stdlib_detail/remove_reference.hpp"
// parasoft-begin-suppress AUTOSAR-A16_2_2-a "False positive: This header's definitions are used"
#include "stdlib/include/stdlib_detail/void_t.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
#ifdef __clang__
#define ARENE_STDLIB_TRIVIALLY_DESTRUCTIBLE_CHECK __is_trivially_destructible
#else
#define ARENE_STDLIB_TRIVIALLY_DESTRUCTIBLE_CHECK __has_trivial_destructor
#endif
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

namespace std {
namespace is_destructible_detail {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations
// and typedefs *are* preceded by a comment with @brief"

/// @brief Type trait to detect if a type is an array of unknown bound
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_array_of_unknown_bound_v = false;

/// @brief Type trait to detect if a type is an array of unknown bound
/// @tparam Type The type to check
template <typename Type>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
extern constexpr bool is_array_of_unknown_bound_v<Type[]> = true;

/// @brief The result of the "destructible" check: is the type not destructible, destructible but throwing, or
/// nothrow-destructible?
enum class is_destructible_result : uint8_t { not_destructible, destructible, nothrow_destructible };

/// @brief Type trait to check whether a type is destructible, and if so, if it is no-throw destructible
/// @tparam Type The type to check
template <typename Type, typename = void>
extern constexpr auto destructible_result_v = is_destructible_result::not_destructible;

/// @brief Type trait to check whether a type is destructible, and if so, if it is no-throw destructible
/// @tparam Type The type to check
template <typename Type>
extern constexpr auto destructible_result_v<Type, enable_if_t<is_array_of_unknown_bound_v<Type>>> =
    is_destructible_result::not_destructible;

/// @brief Type trait to check whether a type is destructible, and if so, if it is no-throw destructible
/// @tparam Type The type to check
template <typename Type>
extern constexpr auto destructible_result_v<Type, enable_if_t<is_reference_v<Type>>> =
    is_destructible_result::nothrow_destructible;

/// @brief Helper alias for the non-array non-reference "raw" type for a given type
/// @tparam Type the type to get the raw type for
template <typename Type>
using raw_type_t = remove_all_extents_t<remove_reference_t<Type>>;

// parasoft-begin-suppress AUTOSAR-A2_7_3-b "False positive: these is a comment with @return"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: Declaration must be non-static to avoid ODR violations"
/// @brief Helper function for checking if a type is destructible
/// @tparam T The type to check
/// @return Nothing
template <class T>
constexpr auto try_destroy() noexcept(noexcept(declval<T&>().~T())) -> decltype(declval<T&>().~T());
// parasoft-end-suppress AUTOSAR-A2_7_3-b
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

/// @brief Type trait to check whether a type is destructible, and if so, if it is no-throw destructible
/// @tparam Type The type to check
template <typename Type>
extern constexpr auto destructible_result_v<
    Type,
    enable_if_t<
        !is_array_of_unknown_bound_v<Type> && !is_reference_v<Type>,
        void_t<decltype(try_destroy<raw_type_t<Type>>())>>> =
    noexcept(try_destroy<raw_type_t<Type>>()) ? is_destructible_result::nothrow_destructible
                                              : is_destructible_result::destructible;

// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace is_destructible_detail

/// @brief Type trait to detect if a type is destructible
/// @tparam Type The type to (try to) destruct
template <typename Type>
extern constexpr bool is_destructible_v = is_destructible_detail::destructible_result_v<Type> !=
                                          is_destructible_detail::is_destructible_result::not_destructible;

/// @brief Type trait to detect if a type is destructible
/// @tparam Type The type to (try to) destruct
template <typename Type>
class is_destructible : public bool_constant<is_destructible_v<Type>> {};

/// @brief Type trait to detect if a type is destructible without throwing an exception
/// @tparam Type The type to (try to) destruct
template <typename Type>
extern constexpr bool is_nothrow_destructible_v = is_destructible_detail::destructible_result_v<Type> ==
                                                  is_destructible_detail::is_destructible_result::nothrow_destructible;

/// @brief Type trait to detect if a type is destructible without throwing an exception
/// @tparam Type The type to (try to) destruct
template <typename Type>
class is_nothrow_destructible : public bool_constant<is_nothrow_destructible_v<Type>> {};

/// @brief Type trait to detect if a type is trivially destructible
/// @tparam Type The type to (try to) destruct
template <typename Type>
extern constexpr bool is_trivially_destructible_v =
    is_destructible_v<Type> && ARENE_STDLIB_TRIVIALLY_DESTRUCTIBLE_CHECK(Type);

/// @brief Type trait to detect if a type is trivially destructible
/// @tparam Type The type to (try to) destruct
template <typename Type>
class is_trivially_destructible : public bool_constant<is_trivially_destructible_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_DESTRUCTIBLE_HPP_
