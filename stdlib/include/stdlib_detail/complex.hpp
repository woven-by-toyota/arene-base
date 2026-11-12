// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COMPLEX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COMPLEX_HPP_

// IWYU pragma: private, include <complex>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress CERT_CPP-DCL51-f "The name 'complex' is required by the C++14 standard"

namespace std {

/// @brief complex number type
/// @note This is intentionally not implemented
template <class>
class complex {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COMPLEX_HPP_
