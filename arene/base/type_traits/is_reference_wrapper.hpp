// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_REFERENCE_WRAPPER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_REFERENCE_WRAPPER_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// Use forward declaration when building stdlib to avoid circular dependecy
#include "arene/base/stdlib_choice/reference_wrapper_fwd.hpp"

namespace arene {
namespace base {

template <typename T>
constexpr bool is_reference_wrapper_v = false;

template <typename T>
constexpr bool is_reference_wrapper_v<std::reference_wrapper<T>> = true;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_REFERENCE_WRAPPER_HPP_
