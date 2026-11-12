// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::copy_elements"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_COPY_ELEMENTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_COPY_ELEMENTS_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/linalg/elementwise_unary_transform.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace copy_elements_detail {

/// @brief Class to implement @c copy_elements
class copy_elements_fn {
 public:
  /// @brief Copy the elements of one @c mdspan into another
  /// @tparam InSpan The type of the source (input) @c mdspan, must be the same rank as @c OutSpan
  /// @tparam OutSpan The type of the destination (output) @c mdspan, must be the same rank as @c InSpan
  /// @param source The @c mdspan to copy elements from
  /// @param dest The @c mdspan to copy elements into
  /// @note This corresponds to the SCOPY and DCOPY functions from BLAS 1 (xCOPY).
  /// @pre <c>source.extents() == dest.extents()</c>, otherwise precondition violation
  template <
      typename InSpan,
      typename OutSpan,
      arene::base::constraints<
      linalg_detail::possibly_comparable_constraints<InSpan, OutSpan>,
          std::enable_if_t<is_invocable_v<decltype(elementwise_unary_transform), InSpan, OutSpan, arene::base::identity>>
      > = nullptr>
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: All parameters are used"
  constexpr auto operator()(InSpan const source, OutSpan const dest) const noexcept(  //
      is_nothrow_invocable_v<decltype(elementwise_unary_transform), InSpan, OutSpan, arene::base::identity>
  ) -> void {
    // parasoft-end-suppress AUTOSAR-A0_1_4-a
    return elementwise_unary_transform(source, dest, ::arene::base::identity{});
  }
};
}  // namespace copy_elements_detail

/// @brief Copy corresponding elements from one multidimensional view to another.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(copy_elements_detail::copy_elements_fn, copy_elements);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_COPY_ELEMENTS_HPP_
