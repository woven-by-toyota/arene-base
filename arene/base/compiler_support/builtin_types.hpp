// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_BUILTIN_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_BUILTIN_TYPES_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: size_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: size_t not reserved in this context"
/// @brief The type of the size of an object
using size_t = ::std::size_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3

/// @brief The type of nullptr
using nullptr_t = ::std::nullptr_t;

// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: ptrdiff_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: ptrdiff_t not reserved in this context"
/// @brief The type of the difference between two pointers
using ptrdiff_t = ::std::ptrdiff_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3

// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: uintptr_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: uintptr_t not reserved in this context"
/// @brief An integer type big enough to hold a pointer
using uintptr_t = ::std::uintptr_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3

/// @brief A POD type with the maximum alignment constraint
using max_align_t = ::std::max_align_t;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_BUILTIN_TYPES_HPP_
