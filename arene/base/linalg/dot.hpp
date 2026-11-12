// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::dot"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DOT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DOT_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/linalg/detail/constraints_for.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/linalg/detail/preconditions.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/stdlib_choice/common_type.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/arithmetic_traits.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {
namespace detail {

/// @brief The type resulting from multiplying the two parameter types together; alias for brevity
/// @tparam LeftValue The left type to multiply
/// @tparam RightValue The right type to multiply
template <typename LeftValue, typename RightValue>
using mul_result_t = decltype(std::declval<LeftValue>() * std::declval<RightValue>());

/// @brief Whether or not the given reference types support the necessary operations to use them in a dot product
/// @tparam LeftRef The left of the two reference types used in the dot product
/// @tparam RightRef The right of the two reference types used in the dot product
/// @tparam Result The type used to accumulate the entries of the dot product; defaults to @c LeftRef*RightRef
template <
    typename LeftRef,
    typename RightRef,
    typename Result = mul_result_t<LeftRef, RightRef>,
    typename = constraints<>>
constexpr bool dot_ops_supported_v{false};

/// @brief Whether or not the given reference types support the necessary operations to use them in a dot product
/// @tparam LeftRef The left of the two reference types used in the dot product
/// @tparam RightRef The right of the two reference types used in the dot product
/// @tparam Result The type used to accumulate the entries of the dot product; defaults to @c LeftRef*RightRef
template <typename LeftRef, typename RightRef, typename Result>
constexpr bool dot_ops_supported_v<
    LeftRef,
    RightRef,
    Result,
    constraints<std::enable_if_t<is_addable_v<Result&&, mul_result_t<LeftRef, RightRef>>>>>{std::is_assignable<
    Result&,
    decltype(std::declval<Result&&>() + std::declval<mul_result_t<LeftRef, RightRef>&&>())>::value};

/// @brief The default result type for a dot product when no scalar accumulator is provided
/// @tparam LeftVec The left of the two vector types used in the dot product
/// @tparam RightVec The right of the two vector types used in the dot product
/// @note This follows the C++ standard in [linalg.algs.blas1.dot] in using @c value_type instead of <c>reference</c>,
/// even though the actual type multiplied is <c>reference</c>, because the latter may be a proxy type unsuitable for
/// data.
template <typename LeftVec, typename RightVec>
using default_dot_result_t = mul_result_t<typename LeftVec::value_type, typename RightVec::value_type>;

/// @brief constraints specialization for @c dot
template <>
class algorithm_constraints<class dot_fn> {
 public:
  /// @brief constraints for @c dot
  /// @tparam InVec input vector type (a rank-1 specialization of @c mdspan)
  /// @tparam Scalar scalar/accumulator type
  /// @tparam Norm elementwise norm function object
  template <typename LeftVec, typename RightVec, typename Scalar = default_dot_result_t<LeftVec, RightVec>>
  using type = arene::base::constraints<
      std::enable_if_t<is_in_vector_v<LeftVec>>,
      std::enable_if_t<is_in_vector_v<RightVec>>,
      std::enable_if_t<is_scalar_v<Scalar>>,
      std::enable_if_t<dot_ops_supported_v<typename LeftVec::reference, typename RightVec::reference, Scalar>>>;
};

/// @brief Class to implement @c dot
class dot_fn {
 public:
  /// @brief Compute the dot product of two vectors with a given initial value
  /// @tparam LeftVec The type of the first vector (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam RightVec The type of the second vector (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam Scalar The scalar type of the initial value
  /// @param left_vec The first vector
  /// @param right_vec The second vector
  /// @param init The initial value to which the dot product result is added
  /// @pre @c left_vec and @c right_vec are both instantiations of @c mdspan with matching 1-dimensional extents,
  /// otherwise @c ARENE_PRECONDITION violation
  /// @pre The dot product of @c left_vec and @c right_vec would not overflow <c>init</c>, otherwise the behaviour is
  /// undefined
  /// @return The sum of the product of each entry of @c left_vec with each matching entry of @c right_vec
  /// @note This version with the initial value corresponds to the SDSDOT function from BLAS 1.
  template <
      typename LeftVec,
      typename RightVec,
      typename Scalar,
      constraints_for<dot_fn, LeftVec, RightVec, Scalar> = nullptr>
  constexpr auto operator()(LeftVec const left_vec, RightVec const right_vec, Scalar init) const
      noexcept(noexcept(init = std::move(init) + (left_vec(0U) * right_vec(0U)))) -> Scalar {
    static_assert(possibly_comparable_v<LeftVec, RightVec>, "Operands to dot must have the same extents");

    ARENE_PRECONDITION(actually_comparable(left_vec, right_vec));

    using index_type = std::common_type_t<typename LeftVec::index_type, typename RightVec::index_type>;

    // parasoft-begin-suppress AUTOSAR-A6_5_2-a "False positive: the loop does contain a loop counter"
    for (index_type idx{}; cmp_less(idx, left_vec.extent(0U)); ++idx) {
      // Writing like this instead of *= so that it works for types with @c operator* defined but no <c>operator*=</c>.
      init = static_cast<Scalar>(std::move(init) + (left_vec(idx) * right_vec(idx)));
    }
    // parasoft-end-suppress AUTOSAR-A6_5_2-a

    return std::move(init);
  }

  /// @brief Compute the dot product of two vectors
  /// @tparam LeftVec The type of the first vector (a rank-1 specialization of <c>mdspan</c>)
  /// @tparam RightVec The type of the second vector (a rank-1 specialization of <c>mdspan</c>)
  /// @param left_vec The first vector
  /// @param right_vec The second vector
  /// @pre @c left_vec and @c right_vec are both instantiations of @c mdspan with matching 1-dimensional extents,
  /// otherwise @c ARENE_PRECONDITION violation
  /// @pre The dot product of @c left_vec and @c right_vec would not overflow
  /// <c>default_dot_result_t<LeftVec, RightVec></c>, otherwise the behaviour is undefined
  /// @return The sum of the product of each entry of @c left_vec with each matching entry of @c right_vec
  /// @note This version without the initial value corresponds to the SDOT and DDOT functions from BLAS 1.
  template <typename LeftVec, typename RightVec, constraints_for<dot_fn, LeftVec, RightVec> = nullptr>
  constexpr auto operator()(LeftVec const left_vec, RightVec const right_vec) const noexcept(noexcept(
      std::declval<dot_fn const&>()(left_vec, right_vec, numbers::zero_v<default_dot_result_t<LeftVec, RightVec>>)
  )) -> default_dot_result_t<LeftVec, RightVec> {
    return (*this)(left_vec, right_vec, numbers::zero_v<default_dot_result_t<LeftVec, RightVec>>);
  }
};

}  // namespace detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object"
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object"
/// @brief Compute the dot product of two vectors.
ARENE_CPP14_INLINE_VARIABLE(detail::dot_fn, dot);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2
// parasoft-end-suppress AUTOSAR-A7_1_5-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DOT_HPP_
