// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_TYPE_NAME_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_TYPE_NAME_HPP_

#include <stddef.h>  // NOLINT(hicpp-deprecated-headers)

#include "testlibs/minitest/cstring.hpp"

namespace testing_control {

/// @brief reference type used to determine offsets in __PRETTY_FUNCTION__
/// @note Not all compilers will display the namespace if the function and type
///   are defined in the same namespace
struct type_name_offset_reference {
  static constexpr auto name = "testing_control::type_name_offset_reference";
};

}  // namespace testing_control

namespace testing {
namespace type_name {
namespace detail {

/// @brief returns a C-string containing the name of the type
/// @tparam T type
/// @return C-string containing the name of the type
template <class T>
constexpr auto typed_pretty_func() noexcept -> char const* {
  return static_cast<char const*>(__PRETTY_FUNCTION__);
}

// In GCC8, a bug causes the __PRETTY_FUNCTION__ macro to be treated as not constexpr. Assigning the result into a
// constexpr variable however allows for constant folding, so it can be used like a constexpr function.
constexpr auto test_result = cstring::find(
    typed_pretty_func<testing_control::type_name_offset_reference>(),
    testing_control::type_name_offset_reference::name
);

// False positive: Not directly comparing the result of pointer additional against nullptr. @c cstring::find will
// explicitly return nullptr if the substring is not found. Only flagged on gcc versions > 12.
#if defined(__GNUC__) && !defined(__clang__)
#if (__GNUC__ > 12) || (__GNUC__ == 12 && __GNUC_MINOR__ > 0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress"
#endif
#endif

static_assert(
    test_result != nullptr,
    "__PRETTY_FUNCTION__ does not contain the function name as an intact string -- type_name can not be computed"
);

#if defined(__GNUC__) && !defined(__clang__)
#if (__GNUC__ > 12) || (__GNUC__ == 12 && __GNUC_MINOR__ > 0)
#pragma GCC diagnostic pop
#endif
#endif

/// @brief number of characters preceeding the type when using @c
///   typed_pretty_func
constexpr auto type_name_offset =  //
    cstring::find(
        typed_pretty_func<testing_control::type_name_offset_reference>(),
        testing_control::type_name_offset_reference::name
    ) -
    typed_pretty_func<testing_control::type_name_offset_reference>();

/// @brief number of characters in @c typed_pretty_func that are not used in
///   spelling the type name
constexpr auto type_name_trim =  //
    cstring::size(typed_pretty_func<testing_control::type_name_offset_reference>()) -
    cstring::size(testing_control::type_name_offset_reference::name);

/// @brief number of characters needed to spell the name of a type
/// @tparam T type
template <class T>
constexpr auto type_name_size =              //
    cstring::size(typed_pretty_func<T>()) -  //
    type_name_trim;

/// @brief character storage for a type name
/// @tparam T type
template <class T>
struct storage {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays,misc-non-private-member-variables-in-classes)
  char chars[type_name_size<T> + 1] = {};

  constexpr storage() noexcept {
    for (auto i = ::ptrdiff_t{}; i != type_name_size<T>; ++i) {
      chars[i] = *(type_name_offset + i + typed_pretty_func<T>());
    }
  }
};

/// @brief provides the name of a type
/// @tparam T type
template <class T>
struct type_name {
  static constexpr storage<T> buf{};
  static constexpr auto name = static_cast<char const*>(buf.chars);
};

template <class T>
constexpr storage<T> type_name<T>::buf;

}  // namespace detail

/// @brief provides the name of a type
/// @tparam T type
template <class T>
constexpr auto type_name = detail::type_name<T>::name;

}  // namespace type_name
}  // namespace testing
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_TYPE_NAME_HPP_
