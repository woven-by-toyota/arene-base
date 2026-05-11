// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_VOID_T_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_VOID_T_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

///
/// @file void_t.hpp
/// @brief Provides a backport of @c std::void_t
///

namespace arene {
namespace base {

/// @brief a backport of C++17's @c std::void_t
template <class...>
using void_t = void;

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_VOID_T_HPP_
