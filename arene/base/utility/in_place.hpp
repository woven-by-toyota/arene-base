// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_IN_PLACE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_IN_PLACE_HPP_

// IWYU pragma: private, include "arene/base/utility.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

///
/// @file in_place.hpp
/// @brief Provides backports of @c std::in_place_t, @c std::in_place_type_t, and @c std::in_place_index_t from C++17.
///

#include "arene/base/stdlib_choice/cstddef.hpp"

namespace arene {
namespace base {

/// @brief backport of @c std::in_place_t from C++17
class in_place_t {
 public:
  /// @brief Explicit default constructor to prevent construction from @c {}
  explicit in_place_t() = default;
};

// parasoft-begin-suppress AUTOSAR-M3_4_1-b "This is a per-TU reference to a global used in multiple TUs"
/// @brief backport of @c std::in_place from C++17, a tag variable for in-place construction
constexpr in_place_t in_place{};  // NOLINT(clang-diagnostic-unused-const-variable) clang seems to false positive here
// parasoft-end-suppress AUTOSAR-M3_4_1-b

/// @brief backport of @c std::in_place_type_t<T> from C++17
template <typename T>
class in_place_type_t {
 public:
  /// @brief Explicit default constructor to prevent construction from @c {}
  explicit in_place_type_t() = default;
};

/// @brief backport of @c std::in_place_type<T> from C++17
template <typename T>
extern constexpr in_place_type_t<T> in_place_type{};

/// @brief backport of @c std::in_place_index_t<size_t> from C++17
template <std::size_t I>
class in_place_index_t {
 public:
  /// @brief Explicit default constructor to prevent construction from @c {}
  explicit in_place_index_t() = default;
};

/// @brief backport of @c std::in_place_index<T> from C++17
template <std::size_t I>
extern constexpr in_place_index_t<I> in_place_index{};

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_IN_PLACE_HPP_
