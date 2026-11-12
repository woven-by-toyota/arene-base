// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_ZERO_SCALAR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_ZERO_SCALAR_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/math_traits.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace arene {
namespace base {
namespace linalg {
namespace detail {

/// @brief zero scalar value derived from an object's normed element type
/// @tparam InObject linear-algebra object type whose element reference type
///   determines the scalar
/// @tparam Norm elementwise norm function object applied to the element
///   reference to determine the scalar type
template <
    typename InObject,
    typename Norm,
    arene::base::constraints<std::enable_if_t<is_invocable_v<Norm, math_reference_t<InObject>>>> = nullptr>
constexpr auto const& zero_scalar_v = numbers::zero_v<invoke_result_t<Norm, math_reference_t<InObject>>>;

/// @brief type of the zero scalar produced by @c zero_scalar_v
/// @tparam InObject linear-algebra object type determining the element type
/// @tparam Norm elementwise norm function object determining the scalar type
template <typename InObject, typename Norm>
using zero_scalar_t = arene::base::remove_cvref_t<decltype(zero_scalar_v<InObject, Norm>)>;

}  // namespace detail
}  // namespace linalg
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_ZERO_SCALAR_HPP_
