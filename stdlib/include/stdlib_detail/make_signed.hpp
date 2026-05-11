// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAKE_SIGNED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAKE_SIGNED_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/add_const.hpp"
#include "stdlib/include/stdlib_detail/add_volatile.hpp"
#include "stdlib/include/stdlib_detail/conditional.hpp"
#include "stdlib/include/stdlib_detail/cstdint.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_const.hpp"
#include "stdlib/include/stdlib_detail/is_enum.hpp"
#include "stdlib/include/stdlib_detail/is_integral.hpp"
#include "stdlib/include/stdlib_detail/is_same.hpp"
#include "stdlib/include/stdlib_detail/is_signed.hpp"
#include "stdlib/include/stdlib_detail/is_volatile.hpp"
#include "stdlib/include/stdlib_detail/remove_cv.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations
// and typedefs *are* preceded by a comment with @brief"

namespace make_signed_detail {
/// @brief Internal class providing @c std::make_signed
/// @tparam Type The type to convert to a signed equivalent
// parasoft-begin-suppress AUTOSAR-A11_0_2 "False positive: Not used as a base class here"
template <typename Type, typename = arene::base::constraints<>>
struct make_signed {};
// parasoft-end-suppress AUTOSAR-A11_0_2

/// @brief Internal class providing @c std::make_signed for integral types that are already signed
/// @tparam Type The type to convert to a signed equivalent
template <typename Type>
struct make_signed<
    Type,
    arene::base::constraints<
        enable_if_t<is_integral_v<Type>>,
        enable_if_t<is_signed_v<Type>>,
        enable_if_t<!is_same_v<Type, bool>>,
        enable_if_t<!is_const_v<Type>>,
        enable_if_t<!is_volatile_v<Type>>>> {
  /// @brief The resulting type
  using type = Type;
};

/// @brief Internal class providing @c std::make_signed for integral types or enumeration types that are @c const
/// qualified
/// @tparam Type The type to convert to a signed equivalent
template <typename Type>
struct make_signed<
    Type,
    arene::base::constraints<
        enable_if_t<is_integral_v<Type> || is_enum_v<Type>>,
        enable_if_t<is_const_v<Type>>,
        enable_if_t<!is_volatile_v<Type>>>> {
  /// @brief The resulting type
  using type = add_const_t<typename make_signed<remove_cv_t<Type>>::type>;
};

/// @brief Internal class providing @c std::make_signed for integral types or enumeration types that are @c volatile
/// qualified
/// @tparam Type The type to convert to a signed equivalent
template <typename Type>
struct make_signed<
    Type,
    arene::base::constraints<
        enable_if_t<is_integral_v<Type> || is_enum_v<Type>>,
        enable_if_t<is_volatile_v<Type>>,
        enable_if_t<!is_const_v<Type>>>> {
  /// @brief The resulting type
  using type = add_volatile_t<typename make_signed<remove_cv_t<Type>>::type>;
};

/// @brief Internal class providing @c std::make_signed for integral types or enumeration types that are @c const
/// and @c volatile qualified
/// @tparam Type The type to convert to a signed equivalent
template <typename Type>
struct make_signed<
    Type,
    arene::base::constraints<
        enable_if_t<is_integral_v<Type> || is_enum_v<Type>>,
        enable_if_t<is_const_v<Type>>,
        enable_if_t<is_volatile_v<Type>>>> {
  /// @brief The resulting type
  using type = add_const_t<add_volatile_t<typename make_signed<remove_cv_t<Type>>::type>>;
};

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "The built-in types must be handled explicitly here for correctness"
/// @brief Internal class providing @c std::make_signed for @c unsigned @c char
template <>
struct make_signed<unsigned char> {
  /// @brief The resulting type
  using type = signed char;
};

/// @brief Internal class providing @c std::make_signed for @c unsigned @c short
template <>
// NOLINTNEXTLINE(google-runtime-int)
struct make_signed<unsigned short> {
  /// @brief The resulting type
  // NOLINTNEXTLINE(google-runtime-int)
  using type = short;
};

/// @brief Internal class providing @c std::make_signed for @c unsigned @c int
template <>
struct make_signed<unsigned int> {
  /// @brief The resulting type
  using type = int;
};

/// @brief Internal class providing @c std::make_signed for @c long
template <>
// NOLINTNEXTLINE(google-runtime-int)
struct make_signed<unsigned long> {
  /// @brief The resulting type
  // NOLINTNEXTLINE(google-runtime-int)
  using type = long;
};

/// @brief Internal class providing @c std::make_signed for @c unsigned @c long @c long
template <>
// NOLINTNEXTLINE(google-runtime-int)
struct make_signed<unsigned long long> {
  /// @brief The resulting type
  // NOLINTNEXTLINE(google-runtime-int)
  using type = long long;
};

/// @brief Internal class providing @c std::make_signed for @c char
template <>
struct make_signed<char> {
  /// @brief The resulting type
  // NOLINTNEXTLINE(google-runtime-int)
  using type = signed char;
};

/// @brief Internal class providing a signed type with a specified size
/// @tparam Size the size to check
template <size_t Size, typename = arene::base::constraints<>>
struct sized_signed {};

/// @brief Internal class providing a signed type with the size of @c char
/// @tparam Size the size to check
template <size_t Size>
struct sized_signed<Size, arene::base::constraints<enable_if_t<Size == sizeof(char)>>> {
  /// @brief The resulting type
  using type = signed char;
};

/// @brief Internal class providing a signed type with the size of @c short
/// @tparam Size the size to check
template <size_t Size>
// NOLINTNEXTLINE(google-runtime-int)
struct sized_signed<Size, arene::base::constraints<enable_if_t<Size == sizeof(short)>>> {
  /// @brief The resulting type
  // NOLINTNEXTLINE(google-runtime-int)
  using type = short;
};

/// @brief Internal class providing a signed type with the size of @c int
/// @tparam Size the size to check
template <size_t Size>
struct sized_signed<
    Size,
    // NOLINTNEXTLINE(google-runtime-int)
    arene::base::constraints<enable_if_t<(Size > sizeof(short))>, enable_if_t<Size == sizeof(int)>>> {
  /// @brief The resulting type
  using type = int;
};

/// @brief Internal class providing a signed type with the size of @c long
/// @tparam Size the size to check
template <size_t Size>
struct sized_signed<
    Size,
    // NOLINTNEXTLINE(google-runtime-int)
    arene::base::constraints<enable_if_t<(Size > sizeof(int))>, enable_if_t<Size == sizeof(long)>>> {
  /// @brief The resulting type
  // NOLINTNEXTLINE(google-runtime-int)
  using type = long;
};

/// @brief Internal class providing a signed type with the size of @c long @c long
/// @tparam Size the size to check
template <size_t Size>
struct sized_signed<
    Size,
    // NOLINTNEXTLINE(google-runtime-int)
    arene::base::constraints<enable_if_t<(Size > sizeof(long))>, enable_if_t<Size == sizeof(long long)>>> {
  /// @brief The resulting type
  // NOLINTNEXTLINE(google-runtime-int)
  using type = long long;
};
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A2_13_3-a-2 "wchar_t must be used here to ensure correctness"
/// @brief Internal class providing a signed type for @c wchar_t
template <>
struct make_signed<wchar_t> {
  /// @brief The resulting type
  // NOLINTNEXTLINE(google-runtime-int)
  using type = typename sized_signed<sizeof(wchar_t)>::type;
};
// parasoft-end-suppress AUTOSAR-A2_13_3-a-2

/// @brief Internal class providing a signed type for @c char16_t
template <>
struct make_signed<char16_t> {
  /// @brief The resulting type
  using type = typename sized_signed<sizeof(char16_t)>::type;
};

/// @brief Internal class providing a signed type for @c char32_t
template <>
struct make_signed<char32_t> {
  /// @brief The resulting type
  using type = typename sized_signed<sizeof(char32_t)>::type;
};

/// @brief Internal class providing @c std::make_signed for integral types that are already signed
/// @tparam Type The type to convert to a signed equivalent
template <typename Type>
struct make_signed<
    Type,
    arene::base::
        constraints<enable_if_t<is_enum_v<Type>>, enable_if_t<!is_const_v<Type>>, enable_if_t<!is_volatile_v<Type>>>> {
  /// @brief The resulting type
  using type = typename sized_signed<sizeof(Type)>::type;
};

}  // namespace make_signed_detail

/// @brief A type trait to provide the equivalent signed type for integral and enumeration types other than @c bool
/// @tparam Type The type to get the equivalent of
template <typename Type>
using make_signed = make_signed_detail::make_signed<Type>;

/// @brief A type alias for the equivalent signed type for integral and enumeration types other than @c bool
/// @tparam Type The type to get the equivalent of
template <typename Type>
using make_signed_t = typename make_signed<Type>::type;

// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAKE_SIGNED_HPP_
