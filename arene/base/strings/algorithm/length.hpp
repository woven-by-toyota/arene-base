// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_ALGORITHM_LENGTH_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_ALGORITHM_LENGTH_HPP_

// IWYU pragma: private, include "arene/base/string_algorithms.hpp"
// IWYU pragma: friend "arene/base/strings/.*"

#include "arene/base/strings/detail/string_length.hpp"  // IWYU pragma: export

namespace arene {
namespace base {
// parasoft-begin-suppress AUTOSAR-M7_3_6-a "This is defined in a separate
// header, namespace and Bazel package to avoid circular dependencies. There are
// no other overloads of this function defined in other headers and header
// inclusion order does not change program behavior."
using ::arene::base::detail::string_length;
// parasoft-end-suppress AUTOSAR-M7_3_6-a
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_ALGORITHM_LENGTH_HPP_
