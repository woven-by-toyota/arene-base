// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::vector_abs_sum"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_VECTOR_ABS_SUM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_VECTOR_ABS_SUM_HPP_

#include "arene/base/algorithm/transform_reduce.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/make_range_from_indexable.hpp"
#include "arene/base/linalg/detail/constraints_for.hpp"
#include "arene/base/linalg/detail/zero_scalar.hpp"
#include "arene/base/linalg/math_traits.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/math/complex_norm.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {
namespace detail {

/// @brief constraints specialization for @c vector_abs_sum_fn
template <>
class algorithm_constraints<class vector_abs_sum_fn> {
  /// @brief constraints for the reduce update operation
  /// @tparam Dest destination (accumulator) type being assigned to
  /// @tparam Reduce binary reduction function object (e.g. @c std::plus<>)
  /// @tparam Source1 first source operand type (typically the running accumulator)
  /// @tparam Source2 second source operand type (typically the per-element norm result)
  ///
  /// Checks that @c Reduce is invocable with every combination of the two source types
  /// and that the result of <c> Reduce(Source1, Source2) </c> is assignable to
  /// @c Dest. This provides the constraints of GENERALIZED_SUM.
  template <typename Dest, typename Reduce, typename Source1, typename Source2>
  using reduce_update_constraints = arene::base::constraints<
      std::enable_if_t<is_invocable_v<Reduce, Source1, Source2>>,  //
      std::enable_if_t<is_invocable_v<Reduce, Source2, Source1>>,  //
      std::enable_if_t<is_invocable_v<Reduce, Source1, Source1>>,  //
      std::enable_if_t<is_invocable_v<Reduce, Source2, Source2>>,  //
      std::enable_if_t<std::is_assignable<                         //
          Dest,                                                    //
          invoke_result_t<                                         //
              Reduce,                                              //
              Source1,                                             //
              Source2>>::value>                                    //
      >;

 public:
  /// @brief constraints for @c vector_abs_sum
  /// @tparam InVec input vector type (a rank-1 specialization of @c mdspan)
  /// @tparam Scalar scalar/accumulator type
  /// @tparam Norm elementwise norm function object
  template <typename InVec, typename Scalar, typename Norm>
  using type = arene::base::constraints<
      std::enable_if_t<is_in_vector_v<InVec>>,
      std::enable_if_t<is_scalar_v<Scalar>>,
      std::enable_if_t<is_invocable_v<Norm&, math_reference_t<InVec>>>,
      reduce_update_constraints<Scalar&, std::plus<>&, Scalar&&, invoke_result_t<Norm&, math_reference_t<InVec>>>>;
};

/// @brief class to implement @c vector_abs_sum
class vector_abs_sum_fn {
  /// @brief default elementwise norm
  using default_norm = decltype(complex_l1_norm);

 public:
  /// @brief compute the sum of absolute values
  /// @tparam InVec vector type (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam Scalar scalar/accumulator type
  /// @tparam Norm elementwise normalization function that determines the absolute value
  ///   (defaults to @c decltype(arene::base::complex_l1_norm))
  /// @param vec input vector
  /// @param init initial value to which the sum of absolute values is added
  /// @param norm elementwise normalization function used to determine the absolute value
  /// @pre The sum of absolute values of @c vec would not overflow <c>init</c>, otherwise the
  ///   behaviour is undefined
  /// @return <c> GENERALIZED_SUM(std::plus<>{}, init, norm(v(0)), ..., norm(v(v.size() - 1))) </c>
  /// @note This function is equivalent to xASUM from BLAS 1 with the default
  ///   @c Norm.
  /// @note For complex types, this function is equivalent to functions SCSUM1
  ///   and DZSUM1 from LAPACK when <c> Norm = decltype(arene::base::math::abs)
  ///   </c>
  template <
      typename InVec,
      typename Scalar,
      typename Norm = default_norm,
      constraints_for<vector_abs_sum_fn, InVec, Scalar, Norm> = nullptr>
  constexpr auto operator()(InVec vec, Scalar init, Norm norm = {}) const noexcept(  //
      noexcept(transform_reduce(                                                     //
          arene::base::detail::make_range_from_indexable(std::declval<InVec>()),
          std::declval<Scalar>(),
          std::declval<std::plus<>>(),
          std::declval<Norm>()
      ))
  ) -> Scalar {
    return transform_reduce(  //
        arene::base::detail::make_range_from_indexable(std::move(vec)),
        std::move(init),
        std::plus<>{},
        std::move(norm)
    );
  }

  /// @brief compute the sum of absolute values
  /// @tparam InVec vector type (a rank-1 specialization of <c>mdspan</c>)
  /// @param vec input vector
  ///
  /// Let @c init be initialized to <c>
  /// arene::base::numbers::zero_v<decltype(arene::base::complex_l1_norm(vec(0)))
  /// </c>.
  ///
  /// @pre The sum of absolute values of @c vec would not overflow <c>init</c>, otherwise the
  ///   behaviour is undefined
  /// @return <c> GENERALIZED_SUM(std::plus<>{}, init, norm(v(0)), ..., norm(v(v.size() - 1))) </c>
  /// @note This function is equivalent to xASUM from BLAS 1.
  template <  //
      typename InVec,
      arene::base::constraints<
          std::enable_if_t<is_in_vector_v<InVec>>,
          constraints_for<
              vector_abs_sum_fn,
              InVec,
              zero_scalar_t<InVec, default_norm>,
              default_norm>> = nullptr>
  constexpr auto operator()(InVec vec) const noexcept(  //
      noexcept(std::declval<vector_abs_sum_fn const&>()(std::declval<InVec>(), zero_scalar_v<InVec, default_norm>))
  ) -> decltype(std::declval<vector_abs_sum_fn const&>()(std::declval<InVec>(), zero_scalar_v<InVec, default_norm>)) {
    return (*this)(std::move(vec), zero_scalar_v<InVec, default_norm>);
  }
};

}  // namespace detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object"
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object"
/// @brief Compute the sum of the absolute values of a vector's elements.
ARENE_CPP14_INLINE_VARIABLE(detail::vector_abs_sum_fn, vector_abs_sum);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_VECTOR_ABS_SUM_HPP_
