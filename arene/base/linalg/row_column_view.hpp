// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::column_view"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_ROW_COLUMN_VIEW_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_ROW_COLUMN_VIEW_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/mdspan/axis_view.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace row_column_view_detail {

/// @brief Class implementing @c row_view and @c column_view by fixing one axis of an in-matrix
/// @tparam Axis the matrix axis held fixed: @c 0 for rows, @c 1 for columns
template <std::size_t Axis>
class matrix_axis_view_fn {
 public:
  /// @brief Return a range over the slices along axis @c Axis of an in-matrix
  /// @tparam InMatrix the input matrix type; must satisfy @c is_in_matrix_v
  /// @tparam Args the types of any other arguments (e.g. indices) to be forwarded to @c axis_view from mdspan
  /// @param matrix the rank-2 mdspan whose rows (@c Axis==0 ) or columns (@c Axis==1 ) are viewed
  /// @param args other arguments (e.g. indices) to be forwarded to @c axis_view from mdspan
  /// @return the range yielded by <c>arene::base::axis_view<Axis>(matrix, std::forward<Args>(args)...)</c>
  template <
      class InMatrix,
      class... Args,
      arene::base::constraints<
          std::enable_if_t<is_in_matrix_v<InMatrix>>,
          std::enable_if_t<is_invocable_v<decltype(axis_view<Axis>), InMatrix, Args...>>> = nullptr>
  constexpr auto operator()(InMatrix const matrix, Args&&... args) const
      noexcept(noexcept(arene::base::axis_view<Axis>(matrix, std::forward<Args>(args)...)))
          -> decltype(arene::base::axis_view<Axis>(matrix, std::forward<Args>(args)...)) {
    return arene::base::axis_view<Axis>(matrix, std::forward<Args>(args)...);
  }
};

}  // namespace row_column_view_detail

// parasoft-begin-suppress AUTOSAR-A2_7_2-a "False positive: this is documentation, not commented-out code"
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
/// @brief Return a range over the rows of a matrix
///
/// Given a matrix, @c row_view returns a range whose elements are the rows of that matrix, in order. The range has one
/// element per row, totaling @c matrix.extent(0) elements, and its @c i-th element is a rank-1 view of the @c i-th
/// row, holding the @c matrix.extent(1) elements of that row. The result is a range, so it may be used in a
/// range-based for loop and with functions that operate on ranges.
///
/// The rows are views rather than copies, so each one refers to the elements of @c matrix. Where the element
/// type is non-const, writing through a row writes through to the matrix.
///
/// ~~~{.cpp}
/// auto const A = ...;  // a 3-by-4 matrix
/// for (auto const row : arene::base::linalg::row_view(A)) {
///   // the loop runs 3 times, once per row of A
///   // row is a rank-1 view of the 4 elements of that row, so row(0) is its first element
/// }
/// ~~~
///
/// The argument must be an in-matrix, that is a rank-2 mdspan whose value type is a scalar
/// (see @c arene::base::linalg::is_in_matrix_v). The @c i -th row is exactly
/// <tt> submdspan(matrix, i, full_extent) </tt>, so the layout of @c matrix must support slicing.
///
/// Can also be called with a second index parameter @c i to get a view of the @c i-th row:
///
/// ~~~{.cpp}
/// auto const A = ...;  // a 3-by-4 matrix
/// auto const row = arene::base::linalg::row_view(A, std::integral_constant<int, 1>);
/// // row is a rank-1 view of the 4 elements of row 1, so row(0) is A(1, 0)
/// ~~~
/// @see arene::base::linalg::column_view
/// @see arene::base::axis_view
ARENE_CPP14_INLINE_VARIABLE(row_column_view_detail::matrix_axis_view_fn<0U>, row_view);

/// @brief Return a range over the columns of a matrix
///
/// Given a matrix, @c column_view returns a range whose elements are the columns of that matrix, in order. The range
/// has one element per column, totaling @c matrix.extent(1) elements, and its @c i-th element is a rank-1 view of
/// the @c i-th column, holding the @c matrix.extent(0) elements of that column. The result is a range, so it may
/// be used in a range-based for loop and with functions that operate on ranges.
///
/// The columns are views rather than copies, so each one refers to the elements of @c matrix itself. Where the element
/// type is non-const, writing through a column writes through to the matrix.
///
/// ~~~{.cpp}
/// auto const A = ...;  // a 3-by-4 matrix
/// for (auto const column : arene::base::linalg::column_view(A)) {
///   // the loop runs 4 times, once per column of A
///   // column is a rank-1 view of the 3 elements of that column, so column(0) is its first element
/// }
/// ~~~
///
/// The argument must be an in-matrix, that is a rank-2 mdspan whose value type is a scalar
/// (see @c arene::base::linalg::is_in_matrix_v). The @c i -th column is exactly
/// <tt> submdspan(matrix, full_extent, i) </tt>, so the layout of @c matrix must support slicing.
///
/// Can also be called with a second index parameter @c i to get a view of the @c i-th column:
///
/// ~~~{.cpp}
/// auto const A = ...;  // a 3-by-4 matrix
/// auto const col = arene::base::linalg::column_view(A, std::integral_constant<int, 1>);
/// // col is a rank-1 view of the 3 elements of column 1, so col(0) is A(0, 1)
/// ~~~
/// @see arene::base::linalg::row_view
/// @see arene::base::axis_view
ARENE_CPP14_INLINE_VARIABLE(row_column_view_detail::matrix_axis_view_fn<1U>, column_view);
// parasoft-end-suppress CERT_CPP-DCL59-a
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress AUTOSAR-A2_7_2-a

namespace detail {

/// @brief column vector view type obtained from a matrix
/// @tparam InMat input matrix type (a rank-2 specialization of @c mdspan)
template <class InMat>
using column_vector_of_t = decltype(column_view(std::declval<InMat>(), 0));

/// @brief row vector view type obtained from a matrix
/// @tparam InMat input matrix type (a rank-2 specialization of @c mdspan)
template <class InMat>
using row_vector_of_t = decltype(row_view(std::declval<InMat>(), 0));

}  // namespace detail

}  // namespace linalg
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a
// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_ROW_COLUMN_VIEW_HPP_
