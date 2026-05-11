// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_TYPE_LIST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_TYPE_LIST_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {

/// @brief an empty struct that is a list of types - cheaper to instantiate than
/// tuple
/// @tparam Tn The types in the list.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class... Tn>
struct type_list {};
// parasoft-end-suppress AUTOSAR-A2_7_3

namespace type_lists {}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_TYPE_LIST_HPP_
