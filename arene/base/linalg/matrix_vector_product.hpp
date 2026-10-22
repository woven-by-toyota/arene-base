// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::column_view"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_VECTOR_PRODUCT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_VECTOR_PRODUCT_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/detail/constraints_for.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/linalg/detail/preconditions.hpp"
#include "arene/base/linalg/dot.hpp"
#include "arene/base/linalg/matrix_observers.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/linalg/row_column_view.hpp"
#include "arene/base/stdlib_choice/common_type.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/safe_comparisons.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace matrix_vector_product_detail {

/// @brief Type trait to check whether or not the type parameters could fail at runtime to be multipliable
/// @tparam InMatrix Type of the matrix to multiply
/// @tparam InVector Type of the vector to multiply
/// @tparam OutVector Type of the vector to save the result to
/// @note Assumes that the arguments *are* possibly-multipliable
template <class InMatrix, class InVector, class OutVector>
constexpr bool possibly_not_multipliable_v{
    static_row_count_v<InMatrix> == dynamic_extent || static_column_count_v<InMatrix> == dynamic_extent ||
    InVector::static_extent(0U) == dynamic_extent || OutVector::static_extent(0U) == dynamic_extent
};

/// @brief Type trait to check whether or not the type parameters could fail at runtime to be addable
/// @tparam LeftVector Type of the left vector to add
/// @tparam RightVector Type of the right vector to add
/// @tparam OutVector Type of the vector to save the result to
/// @note Assumes that the arguments *are* possibly-addable
template <class LeftVector, class RightVector, class OutVector>
constexpr bool possibly_not_addable_v{
    LeftVector::static_extent(0U) == dynamic_extent || RightVector::static_extent(0U) == dynamic_extent ||
    OutVector::static_extent(0U) == dynamic_extent
};

/// @brief Does a precondition check that the operands form a valid multiplication, or a no-op if this is guaranteed
/// @tparam InMatrix Type of the matrix to multiply
/// @tparam InVector Type of the vector to multiply
/// @tparam OutVector Type of the vector to save the result to
/// @param in_matrix Matrix to multiply
/// @param in_vector Vector to multiply
/// @param out_vector Vector to save the result to
template <
    class InMatrix,
    class InVector,
    class OutVector,
    constraints<std::enable_if_t<possibly_not_multipliable_v<InMatrix, InVector, OutVector>>> = nullptr>
constexpr auto precondition_actually_multipliable(InMatrix in_matrix, InVector in_vector, OutVector out_vector) noexcept
    -> void {
  ARENE_PRECONDITION(actually_multipliable(in_matrix, in_vector, out_vector));
}

/// @brief Does a precondition check that the operands form a valid multiplication, or a no-op if this is guaranteed
/// @tparam InMatrix Type of the matrix to multiply
/// @tparam InVector Type of the vector to multiply
/// @tparam OutVector Type of the vector to save the result to
template <
    class InMatrix,
    class InVector,
    class OutVector,
    constraints<std::enable_if_t<!possibly_not_multipliable_v<InMatrix, InVector, OutVector>>> = nullptr>
constexpr auto precondition_actually_multipliable(InMatrix, InVector, OutVector) noexcept -> void {}

/// @brief Does a precondition check that the operands form a valid addition, or a no-op if this is guaranteed
/// @tparam LeftVector Type of the left vector to add
/// @tparam RightVector Type of the right vector to add
/// @tparam OutVector Type of the vector to save the result to
/// @param left_vector Left vector to add
/// @param right_vector Right vector to add
/// @param out_vector Vector to save the result to
template <
    class LeftVector,
    class RightVector,
    class OutVector,
    constraints<std::enable_if_t<possibly_not_addable_v<LeftVector, RightVector, OutVector>>> = nullptr>
constexpr auto
precondition_actually_addable(LeftVector left_vector, RightVector right_vector, OutVector out_vector) noexcept -> void {
  ARENE_PRECONDITION(actually_addable(left_vector, right_vector, out_vector));
}

/// @brief Does a precondition check that the operands form a valid addition, or a no-op if this is guaranteed
/// @tparam LeftVector Type of the left vector to add
/// @tparam RightVector Type of the right vector to add
/// @tparam OutVector Type of the vector to save the result to
template <
    class LeftVector,
    class RightVector,
    class OutVector,
    constraints<std::enable_if_t<!possibly_not_addable_v<LeftVector, RightVector, OutVector>>> = nullptr>
constexpr auto precondition_actually_addable(LeftVector, RightVector, OutVector) noexcept -> void {}

/// @brief Function object implementing a matrix-vector product (matrix on left, vector on right) with optional offset
class matrix_vector_product_impl {
 public:
  /// @brief Multiply a matrix with a vector on the right, saving the result to another vector
  /// @tparam InMatrix Type of the matrix to multiply
  /// @tparam InVector Type of the vector to multiply
  /// @tparam OutVector Type of the vector to save the result to
  /// @param in_matrix Matrix to multiply
  /// @param in_vector Vector to multiply
  /// @param out_vector Vector to save the result to
  /// @pre The length of @c in_vector must match the width of <c>in_matrix</c>, and the length of @c out_vector must
  /// match the height of <c>in_matrix</c>, otherwise precondition violation
  /// @pre @c out_vector must not overlap @c in_matrix or <c>in_vector</c>, otherwise the behaviour is undefined
  /// @note @c in_matrix and @c in_vector may overlap
  template <
      class InMatrix,
      class InVector,
      class OutVector,
      constraints<
          std::enable_if_t<is_in_matrix_v<InMatrix>>,
          std::enable_if_t<is_in_vector_v<InVector>>,
          std::enable_if_t<is_out_vector_v<OutVector>>,
          detail::constraints_for<
              decltype(dot),
              invoke_result_t<decltype(row_view), InMatrix, typename InMatrix::index_type>,
              InVector>> = nullptr>
  constexpr auto operator()(InMatrix in_matrix, InVector in_vector, OutVector out_vector) const
      noexcept(noexcept(out_vector(0) = dot(row_view(in_matrix, 0), in_vector))) -> void {
    static_assert(
        compatible_static_extents<InMatrix, InVector>(1, 0),
        "In a matrix-vector product, the input vector's extent must match the matrix's width"
    );
    static_assert(
        compatible_static_extents<InMatrix, OutVector>(0),
        "In a matrix-vector product, the output vector's extent must match the matrix's height"
    );

    precondition_actually_multipliable(in_matrix, in_vector, out_vector);

    using index_type = std::common_type_t<typename InMatrix::index_type, typename OutVector::index_type>;

    for (index_type row_idx{}; cmp_less(row_idx, row_count(in_matrix)); ++row_idx) {
      out_vector(row_idx) = dot(row_view(in_matrix, row_idx), in_vector);
    }
  }

  /// @brief Multiply a matrix with a vector on the right and add to an offset, saving the result to another vector
  /// @tparam InMatrix Type of the matrix to multiply
  /// @tparam MultiplicandVector Type of the vector to multiply
  /// @tparam OffsetVector Type of the vector to add as an offset
  /// @tparam OutVector Type of the vector to save the result to
  /// @param in_matrix Matrix to multiply
  /// @param multiplicand_vector Vector to multiply
  /// @param offset_vector Vector to add as an offset
  /// @param out_vector Vector to save the result to
  /// @pre The length of @c multiplicand_vector must match the width of <c>in_matrix</c>, and the lengths of both
  /// @c offset_vector and @c out_vector must match the height of <c>in_matrix</c>, otherwise precondition violation
  /// @pre @c out_vector must not overlap @c in_matrix or <c>multiplicand_vector</c>, otherwise the behaviour is
  /// undefined
  /// @pre @c out_vector must either not overlap @c offset_vector or must alias it completely, otherwise the behaviour
  /// is undefined
  /// @note <c>in_matrix</c>, <c>multiplicand_vector</c>, and @c offset_vector may overlap each other
  template <
      class InMatrix,
      class MultiplicandVector,
      class OffsetVector,
      class OutVector,
      constraints<
          std::enable_if_t<is_in_matrix_v<InMatrix>>,
          std::enable_if_t<is_in_vector_v<MultiplicandVector>>,
          std::enable_if_t<is_in_vector_v<OffsetVector>>,
          std::enable_if_t<is_out_vector_v<OutVector>>,
          detail::constraints_for<
              decltype(dot),
              invoke_result_t<decltype(row_view), InMatrix, typename InMatrix::index_type>,
              MultiplicandVector,
              typename OffsetVector::value_type>> = nullptr>
  constexpr auto operator()(
      InMatrix in_matrix,
      MultiplicandVector multiplicand_vector,
      OffsetVector offset_vector,
      OutVector out_vector
  ) const noexcept(noexcept(out_vector(0) = dot(row_view(in_matrix, 0), multiplicand_vector, offset_vector(0))))
      -> void {
    static_assert(
        compatible_static_extents<InMatrix, MultiplicandVector>(1, 0),
        "In a matrix-vector product, the input vector's extent must match the matrix's width"
    );
    static_assert(
        compatible_static_extents<InMatrix, OutVector>(0),
        "In a matrix-vector product, the output vector's extent must match the matrix's height"
    );
    static_assert(
        possibly_addable_v<OffsetVector, OutVector, OutVector>,
        "In a matrix-vector product, the offset vector (if present) must have the same extent as the output vector"
    );

    precondition_actually_multipliable(in_matrix, multiplicand_vector, out_vector);
    precondition_actually_addable(offset_vector, out_vector, out_vector);

    using index_type = std::
        common_type_t<typename InMatrix::index_type, typename OffsetVector::index_type, typename OutVector::index_type>;

    for (index_type row_idx{}; row_idx < row_count(in_matrix); ++row_idx) {
      out_vector(row_idx) = dot(row_view(in_matrix, row_idx), multiplicand_vector, offset_vector(row_idx));
    }
  }
};

}  // namespace matrix_vector_product_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object"
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object"
/// @brief Function object implementing a matrix-vector product with optional offset.
ARENE_CPP14_INLINE_VARIABLE(matrix_vector_product_detail::matrix_vector_product_impl, matrix_vector_product);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a
// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_VECTOR_PRODUCT_HPP_
