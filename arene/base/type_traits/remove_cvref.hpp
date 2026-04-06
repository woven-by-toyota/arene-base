// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_REMOVE_CVREF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_REMOVE_CVREF_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"

namespace arene {
namespace base {

/// @brief Backport of std::remove_cv_t
/// @tparam T The source type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T>
using remove_cv_t = typename std::remove_cv<T>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Backport of std::remove_reference_t
/// @tparam T The source type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Backport of std::remove_cvref
/// @tparam T The source type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T>
using remove_cvref = std::remove_cv<remove_reference_t<T>>;
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Backport of std::remove_cvref_t
/// @tparam T The source type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_REMOVE_CVREF_HPP_
