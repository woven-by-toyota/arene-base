// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_INFO_TYPE_NAME_STRING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_INFO_TYPE_NAME_STRING_HPP_

// IWYU pragma: private, include "arene/base/type_info.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/copy.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_void.hpp"
#include "arene/base/string_algorithms.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace type_name_string_detail {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A3_9_1-b "Use of char for compatibility with std::string_view"
/// @brief Get a hint of the required length of a string which includes the name of the type @c T, but with other
/// characters before and after, and padding afterwards. This won't be exact, because this is a different function to
/// get_big_raw_type_name
/// @tparam T The type to get the name of
/// @return std::size_t holding the length hint
template <typename T>
constexpr auto get_big_raw_type_name_length_hint() noexcept -> std::size_t {
  return arene::base::string_length(static_cast<char const*>(__PRETTY_FUNCTION__));
}
// parasoft-end-suppress AUTOSAR-A3_9_1-b
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief The hint length for @c void to avoid duplicate function evaluation.
/// Uses a dummy template parameter so it is a template for initialization in header file without ODR violation
template <std::nullptr_t = nullptr>
extern constexpr std::size_t void_length_hint = get_big_raw_type_name_length_hint<void>();

/// @brief The length of the buffer to use for @c big_raw_type_name<T>
/// The primary template is only used for @c void the specialization below determines the exact size for other types
/// @tparam T The type to get the length of the raw name for
template <typename T, typename = constraints<>>
extern constexpr std::size_t raw_type_name_buffer_size = 200;  // default value, used for void

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A3_9_1 "Use of char for compatibility with std::string_view"
/// @brief Get a string which includes the name of the type @c T, but with other
/// characters before and after, and padding afterwards.
/// @tparam T The type to get the name of
/// @return array<char> containing a nul-terminated string including the
/// name
template <typename T>
constexpr auto get_big_raw_type_name() noexcept -> array<char, raw_type_name_buffer_size<T>> {
  array<char, raw_type_name_buffer_size<T>> res{};
  char const* name_str{static_cast<char const*>(__PRETTY_FUNCTION__)};
  auto const length = string_length(name_str);
  std::ignore = copy(name_str, arene::base::next(name_str, static_cast<std::ptrdiff_t>(length)), res.begin());
  res[length] = 0;
  return res;
}
// parasoft-end-suppress AUTOSAR-A3_9_1
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief A variable holding a string which includes the name of the type @c T, but
/// with other characters before and after, and padding afterwards.
/// @tparam T The type to get the name of
template <typename T>
extern constexpr auto big_raw_type_name = get_big_raw_type_name<T>();

/// @brief The length of the string stored in @c big_raw_type_name<T>
/// @tparam T The type to get the length of the raw name for
template <typename T>
extern constexpr std::size_t raw_type_name_length = arene::base::string_length(big_raw_type_name<T>.data());

/// @brief The length of the buffer to use for @c big_raw_type_name<T>.
/// This specialization handles the non-void case, and determines the exact required length of the buffer to hold the
/// string plus terminating @c NUL, using the length hint, in order to avoid excessive storage for short names, and
/// insufficient storage for long ones
/// @tparam T The type to get the length of the raw name for
template <typename T>
extern constexpr std::size_t raw_type_name_buffer_size<T, constraints<std::enable_if_t<!std::is_void<T>::value>>> =
    get_big_raw_type_name_length_hint<T>() - void_length_hint<> + raw_type_name_length<void> + 1;

static_assert(
    raw_type_name_buffer_size<bool> == raw_type_name_length<bool> + 1,
    "The buffer for a non-void type must be just big enough for the name"
);

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Get a string view that contains the name of the type @c T, as part of a longer string
/// @tparam T The type to get the string for
/// @return a string_view containing the name of the type
template <typename T>
constexpr auto get_raw_type_name_string_view() noexcept -> string_view {
  return {big_raw_type_name<T>.data(), raw_type_name_length<T>};
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief A struct holding the distance from the start of the helper string to the type name, and the number of
/// trailing characters after the type name
struct name_offsets {
  /// @brief The number of characters before the type name
  std::size_t characters_before;
  /// @brief The number of characters after the type name
  std::size_t characters_after;
};

/// @brief Get the characters before and after the type name in the helper string.  Instantiates the helper string
/// template with a type with a known name (@c void), and searches for that name in the returned string.
/// @return A @c NameOffsets struct holding the number of characters before and after the type name
inline constexpr auto get_name_offsets() noexcept -> name_offsets {
  constexpr string_view void_str{"void"};
  constexpr string_view void_name{get_raw_type_name_string_view<void>()};
  constexpr std::size_t pos{void_name.find(void_str)};

  // parasoft-begin-suppress CERT_C-PRE31-c "False positive: static_assert is a compile-time assert and can have no
  // side-effects"
  static_assert(pos < void_name.size(), "'void' not found in function name");
  // parasoft-end-suppress CERT_C-PRE31-c
  return {pos, void_name.size() - (pos + void_str.size())};
}

/// @brief The offsets for the loaction of the type name within the raw string
/// Uses a dummy template parameter so it is a template for initialization in header file without ODR violation
template <std::nullptr_t = nullptr>
extern constexpr name_offsets offsets = get_name_offsets();

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A8_4_2-a "ARENE_INVARIANT terminates, so does not need a return for its execution
// path"
// parasoft-begin-suppress CERT_CPP-MSC52-a "ARENE_INVARIANT terminates, so does not need a return for its execution
// path"
// parasoft-begin-suppress CERT_C-MSC37-a "ARENE_INVARIANT terminates, so does not need a return for its execution path"

/// @brief Get the length of a type's name
/// @tparam T The type to get the name length of
/// @return std::size_t holding the length
template <typename T>
constexpr auto get_type_name_string_length() noexcept -> std::size_t {
  constexpr string_view full_name{get_raw_type_name_string_view<T>()};
  ARENE_INVARIANT(offsets<>.characters_before < full_name.size());
  ARENE_INVARIANT(offsets<>.characters_after < (full_name.size() - offsets<>.characters_before));
  return full_name.size() - (offsets<>.characters_before + offsets<>.characters_after);
}
// parasoft-end-suppress CERT_C-MSC37-a-2
// parasoft-end-suppress CERT_CPP-MSC52-a-2
// parasoft-end-suppress AUTOSAR-A8_4_2-a-2
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief The length of a type's name
/// @tparam T The type to get the name length of
template <typename T>
extern constexpr std::size_t type_name_string_length = get_type_name_string_length<T>();

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: function is implicitly inline"
/// @brief Get the name of a type as an @c inline_string
/// @tparam T The type to get the name of
/// @return inline_string holding the name
template <typename T>
constexpr auto get_type_name_string() noexcept -> inline_string<type_name_string_length<T>> {
  constexpr string_view full_name{get_raw_type_name_string_view<T>()};
  ARENE_INVARIANT(offsets<>.characters_before < full_name.size());
  ARENE_INVARIANT(type_name_string_length<T> < (full_name.size() - offsets<>.characters_before));
  return inline_string<type_name_string_length<T>>{
      full_name.substr(offsets<>.characters_before, type_name_string_length<T>)
  };
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace type_name_string_detail

/// @brief The name of a type as an @c inline_string
/// @tparam T The type to get the name of
template <typename T>
extern constexpr auto type_name_v{type_name_string_detail::get_type_name_string<T>()};
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_INFO_TYPE_NAME_STRING_HPP_
