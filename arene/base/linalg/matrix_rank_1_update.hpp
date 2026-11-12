// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::matrix_rank_1_update"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_RANK_1_UPDATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_RANK_1_UPDATE_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/detail/constraints_for.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/linalg/detail/zero_object.hpp"
#include "arene/base/linalg/math_traits.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace detail {

/// @brief Constraints for computing <c>target = static_cast<Result>(addend + (left * right))</c>
/// @tparam Target the type being assigned to
/// @tparam Result the type the sum is converted to before being assigned
/// @tparam Addend the addend that the product is added to
/// @tparam Left the left operand of the product
/// @tparam Right the right operand of the product
template <typename Target, typename Result, typename Addend, typename Left, typename Right>
using multiply_add_constraints = arene::base::constraints<
    std::enable_if_t<is_invocable_v<std::multiplies<>&, Left, Right>>,
    std::enable_if_t<is_invocable_v<std::plus<>&, Addend, invoke_result_t<std::multiplies<>&, Left, Right>>>,
    std::enable_if_t<std::is_constructible<
        Result,
        invoke_result_t<std::plus<>&, Addend, invoke_result_t<std::multiplies<>&, Left, Right>>>::value>,
    std::enable_if_t<std::is_assignable<Target, Result>::value>>;

/// @brief constraints specialization for @c matrix_rank_1_update_fn
template <>
class algorithm_constraints<class matrix_rank_1_update_fn> {
 public:
  /// @brief constraints for @c matrix_rank_1_update
  /// @tparam InVec1 The type of the first vector (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam InVec2 The type of the second vector (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam InMat The type of the matrix the outer product is added to (a rank-2 specialization of <c>mdspan</c>)
  /// @tparam OutMat The type of the matrix the result is written to (a rank-2 specialization of <c>mdspan</c>)
  template <typename InVec1, typename InVec2, typename InMat, typename OutMat>
  using type = arene::base::constraints<
      std::enable_if_t<is_in_vector_v<InVec1>>,
      std::enable_if_t<is_in_vector_v<InVec2>>,
      std::enable_if_t<is_out_matrix_v<OutMat>>,
      std::enable_if_t<compatible_static_extents<OutMat, InVec1>(0)>,
      std::enable_if_t<compatible_static_extents<OutMat, InVec2>(1, 0)>,
      std::enable_if_t<is_in_matrix_v<InMat>>,
      std::enable_if_t<possibly_comparable_v<OutMat, InMat>>,
      multiply_add_constraints<
          math_reference_t<OutMat>,
          math_value_t<OutMat>,
          math_reference_t<InMat>,
          math_reference_t<InVec1>,
          math_reference_t<InVec2>>>;
};

/// @brief Function object to compute rank-1 matrix updates
///
/// This class implements the rank-1 update operation, computing the outer product of two vectors and either adding
/// it to an existing matrix or overwriting a matrix with the result. The computation corresponds to the BLAS
/// function xGER (for real element types).
class matrix_rank_1_update_fn {
 public:
  /// @brief Compute the outer product of two vectors, adding it to a given matrix
  /// @tparam InVec1 The type of the first vector (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam InVec2 The type of the second vector (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam InMat The type of the matrix the outer product is added to (a rank-2 specialization of <c>mdspan</c>)
  /// @tparam OutMat The type of the matrix the result is written to (a rank-2 specialization of <c>mdspan</c>)
  /// @param x_vec The first vector
  /// @param y_vec The second vector
  /// @param e_mat The matrix the outer product of @c x_vec and @c y_vec is added to
  /// @param a_mat The matrix that <c>e_mat + x_vec * y_vecT</c> is written to
  /// @pre @c a_mat and @c e_mat have compatible extents
  /// @pre <c>row_count(a_mat) == x_vec.size()</c> and <c>column_count(a_mat) == y_vec.size()</c>
  /// @pre @c a_mat and @c x_vec must not overlap
  /// @pre @c a_mat and @c y_vec must not overlap
  /// @pre if @c a_mat and @c e_mat overlap, they must alias
  ///
  /// @note Complexity <br>
  ///   <c>O(x_vec.extent(0) * y_vec.extent(0))</c>.
  ///
  /// @note This function corresponds to the BLAS function xGER (for real element types)
  template <
      typename InVec1,
      typename InVec2,
      typename InMat,
      typename OutMat,
      constraints_for<matrix_rank_1_update_fn, InVec1, InVec2, InMat, OutMat> = nullptr>
  auto operator()(InVec1 x_vec, InVec2 y_vec, InMat e_mat, OutMat a_mat) const
      noexcept(noexcept(a_mat(0U, 0U) = static_cast<math_value_t<OutMat>>(e_mat(0U, 0U) + (x_vec(0U) * y_vec(0U)))))
          -> void;

  /// @brief Compute the outer product of two vectors, overwriting a given matrix
  /// @tparam InVec1 The type of the first vector (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam InVec2 The type of the second vector (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam OutMat The type of the matrix the result is written to (a rank-2 specialization of <c>mdspan</c>)
  /// @param x_vec The first vector
  /// @param y_vec The second vector
  /// @param a_mat The matrix that <c>x_vec * y_vecT</c> is written to
  /// @pre <c>row_count(a_mat) == x_vec.size()</c> and <c>column_count(a_mat) == y_vec.size()</c>
  /// @pre @c a_mat and @c x_vec must not overlap
  /// @pre @c a_mat and @c y_vec must not overlap
  ///
  /// @note Complexity <br>
  ///   <c>O(x_vec.extent(0) * y_vec.extent(0))</c>.
  ///
  /// @note This function corresponds to the BLAS function xGER (for real element types)
  template <
      typename InVec1,
      typename InVec2,
      typename OutMat,
      arene::base::constraints<
          std::enable_if_t<is_out_matrix_v<OutMat>>,
          constraints_for<matrix_rank_1_update_fn, InVec1, InVec2, zero_object_t<OutMat>, OutMat>> = nullptr>
  auto operator()(InVec1 x_vec, InVec2 y_vec, OutMat a_mat) const
      noexcept(noexcept(std::declval<matrix_rank_1_update_fn const&>()(
          std::declval<InVec1>(),
          std::declval<InVec2>(),
          zero_object<OutMat>(std::declval<OutMat>().extents()),
          std::declval<OutMat>()
      ))) -> void;
};

}  // namespace detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object"
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object"
/// @brief Compute a rank-1 matrix update of two vectors.
ARENE_CPP14_INLINE_VARIABLE(detail::matrix_rank_1_update_fn, matrix_rank_1_update);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a
// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_RANK_1_UPDATE_HPP_
