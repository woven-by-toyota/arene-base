// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::column_view"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_IDENTITY_MATRIX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_IDENTITY_MATRIX_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/linalg/layout_constant_diagonal.hpp"
#include "arene/base/linalg/matrix_observers.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace identity_matrix_detail {

/// @brief Statically assert that the type parameter is an @c extents object
/// @tparam Extents Type to assert
/// @return @c true so that it can be used in a constraints list
/// @note This is in a helper function so it can be called from the constraints list before entering the function body
template <typename Extents>
constexpr auto mandate_is_extents() noexcept -> bool {
  static_assert(
      is_extents_v<Extents>,
      "The argument to linalg::identity_matrix should be an instance of arene::base::extents"
  );
  return true;
}

/// @brief A compile-time constant pair of elements for an identity matrix to refer to: a one and a zero
/// @tparam ElementType The element type to hold a one and a zero of
template <typename ElementType>
extern constexpr array<ElementType, 2U> identity_elements{numbers::one_v<ElementType>, numbers::zero_v<ElementType>};

/// @brief Class implementing @c identity_matrix
/// @tparam ElementType The element type to use for the returned matrix; must be a linalg scalar
template <typename ElementType>
class identity_matrix_impl {
  static_assert(is_scalar_v<ElementType>, "To create a valid matrix, the ElementType must be a linalg scalar");

  /// @brief Type of the @c mdspan to return when calling this @c identity_matrix function object
  /// @tparam Extents Type of the extents given as an argument to @c identity_matrix
  /// @note Adds @c const to the element type since the backing data is a single compile-time-constant buffer
  template <typename Extents>
  using identity_mdspan = mdspan<ElementType const, Extents, layout_constant_diagonal<extents<std::size_t, 0U, 0U>>>;

 public:
  /// @brief Return an @c mdspan representing an identity matrix with the same shape as @c extents
  /// @tparam Extents Type of the extents to use for the identity matrix; constrained to be possibly square
  /// @param extents Extents to use for the identity span
  /// @return An identity mdspan with element type <c>ElementType const</c> and extents constructed from @c extents
  /// @pre @c extents must actually be square (width == height), otherwise @c ARENE_PRECONDITION violation
  template <
      class Extents,
      arene::base::constraints<
          std::enable_if_t<mandate_is_extents<remove_cvref_t<Extents>>()>,
          std::enable_if_t<linalg_detail::possibly_square_extents_v<remove_cvref_t<Extents>>>> = nullptr>
  constexpr auto operator()(Extents&& extents) const noexcept -> identity_mdspan<remove_cvref_t<Extents>> {
    // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
    identity_mdspan<remove_cvref_t<Extents>> const result{
        identity_elements<ElementType>.data(),
        std::forward<Extents>(extents)
    };
    // parasoft-end-suppress AUTOSAR-A7_1_3-a

    ARENE_PRECONDITION(is_square(result));

    return result;
  }
};

}  // namespace identity_matrix_detail

/// @brief Return an @c mdspan representing an identity matrix with the same shape as the passed-in extents
/// @tparam ElementType The element type to use for the returned matrix; must be a linalg scalar
template <typename ElementType>
extern constexpr identity_matrix_detail::identity_matrix_impl<ElementType> identity_matrix{};

}  // namespace linalg
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a
// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_IDENTITY_MATRIX_HPP_
