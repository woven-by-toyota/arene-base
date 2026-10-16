// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::detail::apply_row_permutation"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_APPLY_ROW_PERMUTATION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_APPLY_ROW_PERMUTATION_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/linalg/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/linalg/detail/preconditions.hpp"
#include "arene/base/linalg/detail/swap_rows_or_elements.hpp"
#include "arene/base/linalg/math_traits.hpp"
#include "arene/base/linalg/matrix_observers.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/linalg/row_column_view.hpp"
#include "arene/base/linalg/vector_idx_abs_max.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_lvalue_reference.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/utility/safe_comparisons.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {
namespace detail {

namespace apply_row_permutation_detail {

/// @brief Class to implement @c apply_row_permutation
class apply_row_permutation_fn {
 public:
  /// @brief Update a vector or matrix in place by applying a permutation matrix to its rows
  /// @tparam InMat Type of the permutation matrix; should be a square rank-2 matrix
  /// @tparam InOutObj Type of the object to permute; should be a rank-1 vector or a rank-2 matrix
  /// @param permutation Permutation matrix to apply
  /// @param object Vector or matrix to permute in place
  ///
  /// Performs the equivalent of <c>object = permutation * object</c> using row swaps, so unlike a matrix product it
  /// neither needs a second buffer nor propagates NaN values. A rank-1 @c object is interpreted as a column vector.
  ///
  /// @pre The first extent of @c object must match both the width and height of @c permutation, otherwise precondition
  ///   violation
  /// @pre @c permutation is a permutation matrix, that is every one of its rows and columns holds exactly one element
  ///   equal to <c>numbers::one_v<math_value_t<InMat>></c> and zeros elsewhere, otherwise behaviour is undefined
  /// @pre The storage accessed by @c permutation and @c object does not overlap, otherwise behaviour is undefined
  /// @note @c object must expose its elements through lvalue references.
  template <
      typename InMat,
      typename InOutObj,
      arene::base::constraints<
          std::enable_if_t<is_in_matrix_v<InMat>>,
          std::enable_if_t<is_inout_object_v<InOutObj>>,
          std::enable_if_t<possibly_multipliable_v<InMat, InOutObj, InOutObj>>,
          std::enable_if_t<std::is_lvalue_reference<math_reference_t<InOutObj>>::value>,
          std::enable_if_t<is_invocable_v<decltype(vector_idx_abs_max), row_vector_of_t<InMat>>>,
          std::enable_if_t<is_swappable_v<math_reference_t<InOutObj>>>> = nullptr>
  constexpr auto operator()(InMat permutation, InOutObj object) const
      noexcept(is_nothrow_invocable_v<decltype(vector_idx_abs_max), row_vector_of_t<InMat>> && is_nothrow_invocable_v<decltype(swap_rows_or_elements), InOutObj, math_size_t<InMat>, math_size_t<row_vector_of_t<InMat>>>)
          -> void {
    ARENE_PRECONDITION(actually_multipliable(permutation, object, object));

    for (auto destination_row_index = math_size_t<InMat>{};
         arene::base::cmp_less(destination_row_index, row_count(permutation));
         ++destination_row_index) {
      // The column containing the one identifies the original input row needed at destination_row_index.
      auto source_row_index = vector_idx_abs_max(row_view(permutation, destination_row_index));

      // Rows before destination_row_index are already in their final positions. If source_row_index refers to one,
      // earlier swaps have displaced the required input row. Follow the permutation to find its current position
      // at or after destination_row_index, without disturbing any finished rows.
      while (arene::base::cmp_less(source_row_index, destination_row_index)) {
        source_row_index = vector_idx_abs_max(row_view(permutation, source_row_index));
      }

      swap_rows_or_elements(object, destination_row_index, source_row_index);
    }
  }
};

}  // namespace apply_row_permutation_detail

/// @brief Apply a permutation matrix to the rows of a vector or matrix in place
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(apply_row_permutation_detail::apply_row_permutation_fn, apply_row_permutation);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace detail
}  // namespace linalg
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a
// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_APPLY_ROW_PERMUTATION_HPP_
