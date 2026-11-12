// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::equal_elements"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_EQUAL_ELEMENTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_EQUAL_ELEMENTS_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/index_permutation_range.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/equal_to.hpp"
#include "arene/base/type_traits/is_binary_predicate.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace equal_elements_detail {

/// @brief Class to implement @c equal_elements
class equal_elements_fn {
 public:
  /// @brief Check if the elements of two @c mdspan instantations are equal according to the supplied @c Comparator
  /// @tparam LhsSpan The type of the first @c mdspan
  /// @tparam RhsSpan The type of the second @c mdspan
  /// @tparam Comparator The type of the comparator
  /// @param lhs_span The first @c mdspan
  /// @param rhs_span The second @c mdspan
  /// @param comp The instance of the comparator to use
  /// @pre @c lhs_span and @c rhs_span are both instantiations of @c mdspan with matching extents
  /// @return @c true if @c lhs_span[indices] is equal to @c rhs_span[indices] for each valid @c indices array given the
  /// @c extents, @c false otherwise
  template <
      typename LhsSpan,
      typename RhsSpan,
      typename Comparator = std::equal_to<>,
      arene::base::constraints<
         linalg_detail::possibly_comparable_constraints<LhsSpan, RhsSpan>,
         std::enable_if_t<is_binary_predicate_v<Comparator&, typename LhsSpan::reference, typename RhsSpan::reference>>> = nullptr>
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: All parameters are used"
  constexpr auto operator()(LhsSpan const lhs_span, RhsSpan const rhs_span, Comparator comp = {}) const noexcept( //
  is_nothrow_invocable_v<Comparator&, typename LhsSpan::reference, typename RhsSpan::reference>
  &&
  is_nothrow_invocable_v<LhsSpan const&, array<typename LhsSpan::index_type, LhsSpan::rank()>> &&
  is_nothrow_invocable_v<RhsSpan const&, array<typename RhsSpan::index_type, RhsSpan::rank()>>
) -> bool {
    // parasoft-end-suppress AUTOSAR-A0_1_4-a
    ARENE_PRECONDITION(lhs_span.extents() == rhs_span.extents());
    if (lhs_span.empty()) {
      return true;
    }
    auto const extents_array = mdspan_detail::to_array(lhs_span.extents());
    // parasoft-begin-suppress AUTOSAR-M0_1_3 "False positive: All local variables are used"
    for (auto& indices : arene::base::detail::make_index_permutation_range(extents_array)) {
      // parasoft-end-suppress AUTOSAR-M0_1_3
      if (!comp(lhs_span(indices), rhs_span(indices))) {
        return false;
      }
    }
    return true;
  }
};
}  // namespace equal_elements_detail

/// @brief Compare the corresponding elements of two multidimensional views for equality.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(equal_elements_detail::equal_elements_fn, equal_elements);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_EQUAL_ELEMENTS_HPP_
