// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::matrix_one_norm"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_ONE_NORM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_ONE_NORM_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/algorithm/transform_reduce.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/bind_back.hpp"
#include "arene/base/functional/detail/max_fn.hpp"
#include "arene/base/linalg/detail/constraints_for.hpp"
#include "arene/base/linalg/detail/zero_scalar.hpp"
#include "arene/base/linalg/math_traits.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/linalg/row_column_view.hpp"
#include "arene/base/linalg/vector_abs_sum.hpp"
#include "arene/base/linalg/vector_idx_abs_max.hpp"
#include "arene/base/math/abs_cpo.hpp"
#include "arene/base/math/detail/less_nan_highest.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {
namespace detail {

/// @brief constraints specialization for @c matrix_one_norm_fn
template <>
class algorithm_constraints<class matrix_one_norm_fn> {
  /// @brief constraints of the underlying algorithms used by @c matrix_one_norm
  /// @tparam InVec column vector slice of the input matrix
  /// @tparam Scalar scalar/accumulator type
  /// @tparam Norm elementwise norm function object
  template <typename InVec, typename Scalar, typename Norm>
  using underlying_constraints = arene::base::constraints<
      constraints_for<decltype(vector_abs_sum), InVec, Scalar, Norm>,
      constraints_for<decltype(vector_idx_abs_max), InVec, auto(math_reference_t<InVec>)->Scalar>>;

 public:
  /// @brief constraints for @c matrix_one_norm
  /// @tparam InMat input matrix type (a rank-2 specialization of @c mdspan)
  /// @tparam Scalar scalar/accumulator type
  /// @tparam Norm elementwise norm function object that determines the
  ///   absolute value
  template <typename InMat, typename Scalar, typename Norm>
  using type = arene::base::constraints<
      std::enable_if_t<is_in_matrix_v<InMat>>,
      std::enable_if_t<is_invocable_v<Norm&, math_reference_t<InMat>>>,
      std::enable_if_t<std::is_convertible<invoke_result_t<Norm&, math_reference_t<InMat>>, Scalar>::value>,
      underlying_constraints<column_vector_of_t<InMat>, Scalar, Norm>>;
};

/// @brief class to implement @c matrix_one_norm
class matrix_one_norm_fn {
  /// @brief default elementwise norm
  using default_norm = decltype(math::abs);

 public:
  /// @brief compute the L1 (maximum absolute column sum) norm of a matrix
  /// @tparam InMat matrix type (a rank-2 specialization of @c mdspan)
  /// @tparam Scalar scalar/accumulator type
  /// @tparam Norm elementwise normalization function that determines the absolute value
  ///   (defaults to @c decltype(arene::base::math::abs))
  /// @param mat input matrix
  /// @param init initial value to which the L1 norm is added
  /// @param norm elementwise norm function object determining absolute value
  /// @return the sum of @c init and the L1 norm of @c mat
  /// @note If @c mat has zero columns, returns @c init
  /// @note This function is equivalent to xLANGE from LAPACK when xLANGE is
  ///   specified with @c norm1 and when <c> Norm = decltype(arene::base::math::abs) </c>.
  template <  //
      typename InMat,
      typename Scalar,
      typename Norm = default_norm,
      constraints_for<matrix_one_norm_fn, InMat, Scalar, Norm> = nullptr>
  constexpr auto operator()(InMat mat, Scalar init, Norm norm = {}) const noexcept(              //
      is_nothrow_less_than_comparable_v<Scalar> &&                                               //
      is_nothrow_invocable_v<decltype(vector_abs_sum), column_vector_of_t<InMat>, Scalar, Norm>  //
  ) -> Scalar {
    // NOTE: it is semantically invalid for the result of 'abs_column_sum' to
    // ever be ordered before 'init'. The result of a norm can never be ordered
    // below "zero".
    auto const abs_column_sum = bind_back(vector_abs_sum, init, std::move(norm));

    return transform_reduce(
        column_view(std::move(mat)),
        std::move(init),
        functional_detail::max_fn(math_detail::less_nan_highest),
        abs_column_sum
    );
  }

  /// @brief compute the L1 (maximum absolute column sum) norm of a matrix
  /// @tparam InMat matrix type (a rank-2 specialization of @c mdspan)
  /// @param mat input matrix
  /// @return the L1 norm of @c mat
  /// @note This function is equivalent to xLANGE from LAPACK when xLANGE is
  ///   specified with @c norm1.
  ///
  /// Expression-equivalent to <c> matrix_one_norm(mat, init) </c> where @c init is
  /// initialized as <c>
  /// arene::base::numbers::zero_v<decltype(arene::base::math::abs(mat(0, 0)))
  /// </c>.
  template <
      typename InMat,
      arene::base::constraints<
          std::enable_if_t<is_in_matrix_v<InMat>>,
          constraints_for<
              matrix_one_norm_fn,
              InMat,
              zero_scalar_t<InMat, default_norm>,
              default_norm>> = nullptr>
  constexpr auto operator()(InMat mat) const noexcept(  //
      noexcept(std::declval<matrix_one_norm_fn const&>()(std::declval<InMat>(), zero_scalar_v<InMat, default_norm>))
  ) -> decltype(std::declval<matrix_one_norm_fn const&>()(std::declval<InMat>(), zero_scalar_v<InMat, default_norm>)) {
    return (*this)(std::move(mat), zero_scalar_v<InMat, default_norm>);
  }
};

}  // namespace detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object"
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object"
/// @brief Compute the maximum absolute column sum of a matrix.
ARENE_CPP14_INLINE_VARIABLE(detail::matrix_one_norm_fn, matrix_one_norm);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATRIX_ONE_NORM_HPP_
