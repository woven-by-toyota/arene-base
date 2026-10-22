// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::row_count"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_OBSERVERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_OBSERVERS_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/linalg/math_traits.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace matrix_observers_detail {

/// @brief Function object returning the dynamic extent of a matrix along a given axis
/// @tparam Axis The matrix axis to query: @c 0 for rows, @c 1 for columns
template <std::size_t Axis>
class extent_accessor_fn {
 public:
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameter 'matrix' is used"
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: function does return a value"
  // parasoft-begin-suppress CERT_C-MSC37-a "False positive: function does return a value"
  // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: function does return a value"
  /// @brief Return the extent of @c matrix along @c Axis
  /// @tparam Mat Matrix type; must satisfy @c is_in_matrix_v
  /// @param matrix Matrix whose extent along @c Axis is returned
  /// @return @c matrix.extent(Axis)
  template <typename Mat, arene::base::constraints<std::enable_if_t<is_in_matrix_v<Mat>>> = nullptr>
  constexpr auto operator()(Mat matrix) const noexcept -> math_index_t<Mat> {
    return matrix.extent(Axis);
  }
  // parasoft-end-suppress CERT_CPP-MSC52-a
  // parasoft-end-suppress CERT_C-MSC37-a
  // parasoft-end-suppress AUTOSAR-A8_4_2-a
  // parasoft-end-suppress AUTOSAR-A0_1_4-a
};

/// @brief Helper trait for implementing @c static_row_count_v and @c static_column_count_v for when @c Mat is not an
/// in-matrix
/// @tparam Mat The matrix type
/// @tparam Axis The matrix axis to query: @c 0 for rows, @c 1 for columns
/// @see static_row_count_v
/// @see static_column_count_v
///
/// The primary defines no @c value, so naming it for a type that is not an in-matrix is ill-formed. A rank-1 or rank-3
/// @c mdspan does have an extent along @c Axis, but it has no rows or columns, so the query is rejected rather than
/// answered with @c dynamic_extent. The @c static_assert reports that in the same words on every compiler.
template <typename Mat, std::size_t Axis, typename = arene::base::constraints<>>
class static_extent {
  static_assert(is_in_matrix_v<Mat>, "static_row_count/static_column_count: Mat must be an in-matrix");
};

/// @brief Helper trait for implementing @c static_row_count_v and @c static_column_count_v for when @c Mat is an
/// in-matrix
/// @tparam Mat The matrix type
/// @tparam Axis The matrix axis to query: @c 0 for rows, @c 1 for columns
/// @see static_row_count_v
/// @see static_column_count_v
template <typename Mat, std::size_t Axis>
class static_extent<Mat, Axis, arene::base::constraints<std::enable_if_t<is_in_matrix_v<Mat>>>>
    : public std::integral_constant<std::size_t, math_extents_t<Mat>::static_extent(Axis)> {};

}  // namespace matrix_observers_detail

/// @brief Function object returning the number of rows of a matrix
///
/// Given an in-matrix @c A, @c row_count(A) returns <c>A.extent(0)</c>.
/// @note Requires @c is_in_matrix_v<Mat> to be @c true.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(matrix_observers_detail::extent_accessor_fn<0U>, row_count);

/// @brief Function object returning the number of columns of a matrix
///
/// Given an in-matrix @c A, @c column_count(A) returns <c>A.extent(1)</c>.
/// @note Requires @c is_in_matrix_v<Mat> to be @c true.
ARENE_CPP14_INLINE_VARIABLE(matrix_observers_detail::extent_accessor_fn<1U>, column_count);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

/// @brief The compile-time number of rows of a matrix
/// @tparam Mat Matrix type; must satisfy @c is_in_matrix_v
/// @note Ill-formed when @c Mat is not an in-matrix.
///
/// Equals <c>math_extents_t<Mat>::static_extent(0)</c>, which is @c dynamic_extent if the row extent is not known
/// statically.
template <typename Mat>
extern constexpr std::size_t static_row_count_v{matrix_observers_detail::static_extent<Mat, 0U>::value};

/// @brief The compile-time number of columns of a matrix
/// @tparam Mat Matrix type; must satisfy @c is_in_matrix_v
/// @note Ill-formed when @c Mat is not an in-matrix.
///
/// Equals <c>math_extents_t<Mat>::static_extent(1)</c>, which is @c dynamic_extent if the column extent is not known
/// statically.
template <typename Mat>
extern constexpr std::size_t static_column_count_v{matrix_observers_detail::static_extent<Mat, 1U>::value};

/// @brief Trait holding the compile-time number of rows of a matrix
/// @tparam Mat Matrix type; must satisfy @c is_in_matrix_v
/// @note Ill-formed when @c Mat is not an in-matrix.
/// @see static_row_count_v
template <typename Mat>
class static_row_count : public std::integral_constant<std::size_t, static_row_count_v<Mat>> {};

/// @brief Trait holding the compile-time number of columns of a matrix
/// @tparam Mat Matrix type; must satisfy @c is_in_matrix_v
/// @note Ill-formed when @c Mat is not an in-matrix.
/// @see static_column_count_v
template <typename Mat>
class static_column_count : public std::integral_constant<std::size_t, static_column_count_v<Mat>> {};

namespace matrix_observers_detail {

/// @brief Class to implement @c is_square
class is_square_fn {
 public:
  /// @brief Check whether @c matrix has as many rows as it has columns
  /// @tparam Mat Matrix type; must satisfy @c possibly_square_v
  /// @param matrix Matrix to check
  /// @return @c true if <c>row_count(matrix) == column_count(matrix)</c>, @c false otherwise
  template <typename Mat, arene::base::constraints<std::enable_if_t<possibly_square_v<Mat>>> = nullptr>
  constexpr auto operator()(Mat matrix) const noexcept -> bool {
    return row_count(matrix) == column_count(matrix);
  }

  /// @brief Check whether a matrix whose static extents cannot be equal has as many rows as it has columns
  /// @tparam Mat Matrix type; must satisfy @c is_in_matrix_v but not @c possibly_square_v
  /// @return @c false
  template <
      typename Mat,
      arene::base::constraints<std::enable_if_t<is_in_matrix_v<Mat> && !possibly_square_v<Mat>>> = nullptr>
  constexpr auto operator()(Mat /*matrix*/) const noexcept -> bool {
    return false;
  }
};

}  // namespace matrix_observers_detail

/// @brief Function object returning whether or not a matrix has as many rows as it has columns
///
/// Given an in-matrix @c A, @c is_square(A) returns <c>row_count(A) == column_count(A)</c>, except that it returns
/// @c false without inspecting @c A when the static extents of @c A's type cannot be equal.
/// @note Requires @c is_in_matrix_v<Mat> to be @c true.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(matrix_observers_detail::is_square_fn, is_square);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_OBSERVERS_HPP_
