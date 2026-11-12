// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::swap_rows"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SWAP_ROWS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SWAP_ROWS_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/linalg/row_column_view.hpp"
#include "arene/base/linalg/swap_elements.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace swap_rows_detail {

/// @brief Class to implement @c swap_rows
class swap_rows_fn {
 public:
  /// @brief Swap two rows of a single linalg matrix with each other
  /// @tparam Matrix Type of the matrix whose rows should be swapped; should be rank 2 with a swappable value type
  /// @tparam IndexI Type of the first row index to use; using a @c std::integral_constant skips runtime bounds checks
  /// @tparam IndexJ Type of the second row index to use; using a @c std::integral_constant skips runtime bounds checks
  /// @param matrix Matrix whose rows should be swapped
  /// @param row_index_i Index of the first row to swap
  /// @param row_index_j Index of the second row to swap
  /// @note If <c>row_index_i == row_index_j</c> no swap is performed, even if the swap function would have side effects
  /// @pre both @c row_index_i and @c row_index_j are <c>< matrix.extent(0)</c>, otherwise precondition violation
  template <
      typename Matrix,
      typename IndexI,
      typename IndexJ,
      arene::base::constraints<
          std::enable_if_t<is_inout_matrix_v<Matrix>>,
          std::enable_if_t<is_invocable_v<decltype(row_view), Matrix, IndexI>>,
          std::enable_if_t<is_invocable_v<decltype(row_view), Matrix, IndexJ>>,
          std::enable_if_t<is_swappable_v<typename Matrix::reference>>> = nullptr>
  constexpr auto operator()(Matrix matrix, IndexI row_index_i, IndexJ row_index_j) const
      noexcept(is_nothrow_swappable_v<typename Matrix::reference>) -> void {
    auto row_i = row_view(matrix, row_index_i);
    auto row_j = row_view(matrix, row_index_j);

    if (row_index_i != row_index_j) {
      swap_elements(row_i, row_j);
    }
  }
};

}  // namespace swap_rows_detail

/// @brief Swap two rows of a matrix.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(swap_rows_detail::swap_rows_fn, swap_rows);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SWAP_ROWS_HPP_
