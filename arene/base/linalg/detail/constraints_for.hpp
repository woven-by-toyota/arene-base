// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_CONSTRAINTS_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_CONSTRAINTS_FOR_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace arene {
namespace base {
namespace linalg {
namespace detail {

/// @brief primary template mapping a function object type to its constraints
/// @note Intentionally left undefined; each supported function object provides
///   an explicit specialization exposing a @c type member alias template.
/// @note This mechanism lets linalg algorithms express fine-grained, composable
///   requirements that surface as granular "requirement not satisfied"
///   diagnostics at the user's call site with Clang. By centralizing each function
///   object's constraints in its @c algorithm_constraints specialization,
///   higher-level algorithms (e.g. @c matrix_l1_norm) can reuse the constraints
///   of the primitives they build upon (e.g. @c vector_abs_sum,
///   @c vector_idx_abs_max) via @c constraints_for, rather than duplicating and
///   re-maintaining those requirements in every caller.
template <class>
class algorithm_constraints;  // IWYU pragma: keep

/// @brief constraints for invoking a given function object with @p Args
/// @tparam F function object type (cv-ref qualifiers are stripped)
/// @tparam Args argument types forwarded to the corresponding
///   @c algorithm_constraints specialization's @c type alias
template <class F, class... Args>
using constraints_for = typename algorithm_constraints<remove_cvref_t<F>>::template type<Args...>;

}  // namespace detail
}  // namespace linalg
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_CONSTRAINTS_FOR_HPP_
