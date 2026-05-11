// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NOTHROW_T_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NOTHROW_T_HPP_

// IWYU pragma: private, include <new>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

namespace std {

/// @brief Tag type used to disambiguate the overloads of throwing and non-throwing allocation functions
class nothrow_t {
 public:
  /// @brief Default explicit constructor
  explicit nothrow_t() = default;
};

/// @brief A constant of @c std::nothrow_t
extern nothrow_t const nothrow;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NOTHROW_T_HPP_
