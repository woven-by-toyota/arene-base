// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::subtract"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SUBTRACT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SUBTRACT_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/functional/bind_back.hpp"                 // IWYU pragma: export
#include "arene/base/linalg/elementwise_binary_transform.hpp"  // IWYU pragma: export
#include "arene/base/stdlib_choice/minus.hpp"                  // IWYU pragma: export
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace arene {
namespace base {
namespace linalg {

/// @brief subract two mdspans elementwise, writing the result into a third
/// @tparam LhsInSpan The type of the first input span
/// @tparam RhsInSpan The type of the second input span
/// @tparam OutSpan The type of the output span
/// Pass the first and second input spans as @c lhs_span and @c rhs_span, and the output span as @c out_span.
///
/// @pre All of @c LhsInSpan , @c RhsInSpan , and @c OutSpan must be instantiations of @c mdspan with the same @c rank
/// @pre if @c lhs_in_span and @c out_span overlap, they must alias
/// @pre if @c rhs_in_span and @c out_span overlap, they must alias
/// @pre @c std::minus must be invocable with elements of the input spans, and the result must be assignable to the
/// elements of the output span
/// @pre All of @c lhs_span , @c rhs_span , and @c out_span must have equal extents
// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable is initialized"
// parasoft-begin-suppress AUTOSAR-M3_4_1-b "This is a per-TU reference to a global used in multiple TUs"
ARENE_CPP14_INLINE_VARIABLE_INIT(subtract, arene::base::bind_back(elementwise_binary_transform, std::minus<>{}));
// parasoft-begin-suppress AUTOSAR-M3_4_1-b
// parasoft-end-suppress CERT_CPP-DCL56-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SUBTRACT_HPP_
