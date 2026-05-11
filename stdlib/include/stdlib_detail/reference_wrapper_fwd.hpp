// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REFERENCE_WRAPPER_FWD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REFERENCE_WRAPPER_FWD_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// Used to generate stdlib_choice mapping
// IWYU pragma: private, include <functional>

namespace std {

/// @brief Forward declare reference wrapper for use in is_reference_wrapper.hpp
/// This will avoid a circular dependency
/// @tparam T value type of the reference
template <typename T>
class reference_wrapper;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REFERENCE_WRAPPER_FWD_HPP_
