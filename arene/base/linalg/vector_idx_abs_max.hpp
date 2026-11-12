// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::vector_idx_abs_max"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_VECTOR_IDX_ABS_MAX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_VECTOR_IDX_ABS_MAX_HPP_

#include "arene/base/algorithm/detail/relative_find.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/make_range_from_indexable.hpp"
#include "arene/base/functional/detail/flip.hpp"
#include "arene/base/linalg/detail/constraints_for.hpp"
#include "arene/base/linalg/math_traits.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/math/complex_norm.hpp"
#include "arene/base/math/detail/less_nan_highest.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {
namespace detail {

/// @brief constraints specialization for @c vector_idx_abs_max_fn
template <>
class algorithm_constraints<class vector_idx_abs_max_fn> {
  /// @brief constraints requiring a type to be partially ordered
  /// @tparam T the type to check for comparison operator support
  /// Checks that @c T supports the full set of relational and equality
  /// comparison operators (<c>==</c>, <c>!=</c>, <c><</c>, <c>></c>,
  /// <c><=</c>, <c>>=</c>). This is applied to the result of @c Norm
  /// so that the maximum element can be determined.
  template <typename T>
  using partially_ordered_constraints = arene::base::constraints<
      std::enable_if_t<is_equality_comparable_v<T>>,
      std::enable_if_t<is_inequality_comparable_v<T>>,
      std::enable_if_t<is_less_than_comparable_v<T>>,
      std::enable_if_t<is_greater_than_comparable_v<T>>,
      std::enable_if_t<is_less_than_or_equal_comparable_v<T>>,
      std::enable_if_t<is_greater_than_or_equal_comparable_v<T>>>;

 public:
  /// @brief constraints for @c vector_idx_abs_max_fn
  /// @tparam InVec input vector type (a rank-1 specialization of @c mdspan)
  /// @tparam Norm elementwise norm function object
  template <typename InVec, typename Norm>
  using type = arene::base::constraints<
      std::enable_if_t<is_in_vector_v<InVec>>,
      std::enable_if_t<is_invocable_v<Norm&, math_reference_t<InVec>>>,
      partially_ordered_constraints<invoke_result_t<Norm&, math_reference_t<InVec>>>>;
};

/// @brief class to implement @c vector_idx_abs_max
class vector_idx_abs_max_fn {
 public:
  /// @brief finds the index of maximum absolute value of vector elements
  /// @tparam InVec vector type (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam Norm normalization function that determines the absolute value
  ///   (defaults to @c decltype(arene::base::complex_l1_norm))
  /// @param vec input vector
  /// @param norm normalization function used to determine the absolute value
  ///
  /// Finds the index corresponding to the element with the maximum absolute value
  /// in vector @c vec, where the absolute value is determined by @c norm. By
  /// default, <c>arene::base::complex_l1_norm</c> is used. If multiple elements
  /// contain the maximum absolute value, returns the index of the first element
  /// with the maximum absolute value.
  ///
  /// @return <c>math_size_t<InVec></c> with the index of maximum absolute value
  /// or <c>std::numeric_limits<math_size_t<InVec>>::max()</c> if @c vec is empty
  /// @note This function is equivalent to IxAMAX from BLAS 1 with the default
  ///   @c Norm.
  /// @note For complex types, this function is equivalent to functions ICMAX1
  ///   and IZMAX1 from LAPACK when <c> Norm = decltype(arene::base::math::abs)
  ///   </c>
  /// @note NaN norm results are treated as the largest value; any NaN
  ///   compares greater than every non-NaN value. When one or more elements
  ///   have a NaN norm result, the index of the first such element is
  ///   returned. This does *not* implement the IEEE 754 total ordering over
  ///   NaN, which distinguishes NaN sign and payload: in this implementation, all NaN norm results are
  ///   treated as equivalent.
  template <  //
      typename InVec,
      typename Norm = decltype(complex_l1_norm),
      constraints_for<vector_idx_abs_max_fn, InVec, Norm> = nullptr>
  constexpr auto operator()(InVec vec, Norm norm = {}) const noexcept(  //
      noexcept(algorithm_detail::relative_find(                         //
          arene::base::detail::make_range_from_indexable(std::declval<InVec&>()).begin(),
          arene::base::detail::make_range_from_indexable(std::declval<InVec&>()).end(),
          math_detail::less_nan_highest,
          std::declval<Norm&>()
      ))
  ) -> math_size_t<InVec> {
    if (vec.empty()) {
      return std::numeric_limits<math_size_t<InVec>>::max();
    }

    auto const values = arene::base::detail::make_range_from_indexable(vec);

    // 'arene::base::max_element' requires LegacyForwardIterator, which will not
    // be satisfied if 'InVec' has an AccessorPolicy that returns a value.
    // Also note that the spec doesn't require the norm result to be copyable
    // so we can't store the best norm result on the stack and update it.
    auto const iter = algorithm_detail::relative_find(  //
        values.begin(),
        values.end(),
        functional_detail::flip(math_detail::less_nan_highest),
        norm
    );

    return static_cast<math_size_t<InVec>>(iter - values.begin());
  }
};

}  // namespace detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object"
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object"
/// @brief Find the index of a vector element with greatest absolute value.
ARENE_CPP14_INLINE_VARIABLE(detail::vector_idx_abs_max_fn, vector_idx_abs_max);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_VECTOR_IDX_ABS_MAX_HPP_
