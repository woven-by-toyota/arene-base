// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_CSTDDEF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_CSTDDEF_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "Part of a standard library implementation"

// IWYU pragma: private, include <cstddef>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress AUTOSAR-A18_0_1-a-2 "False positive: This is the implementation of <cstddef>"
// parasoft-begin-suppress AUTOSAR-A1_1_1-d-2 "False positive: This is the implementation of <cstddef>"
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stddef.h>
// parasoft-end-suppress AUTOSAR-A1_1_1-d-2
// parasoft-end-suppress AUTOSAR-A18_0_1-a-2

namespace std {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations
// and typedefs *are* preceded by a comment with @brief"

/// @brief A type suitable for holding the size of an object
using size_t = ::size_t;
/// @brief A type suitable for holding the difference between two pointers
using ptrdiff_t = ::ptrdiff_t;
/// @brief The type of @c nullptr
using nullptr_t = decltype(nullptr);
/// @brief A POD type which has the maximum alignment required for any built-in type
using max_align_t = ::max_align_t;

// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_CSTDDEF_HPP_
