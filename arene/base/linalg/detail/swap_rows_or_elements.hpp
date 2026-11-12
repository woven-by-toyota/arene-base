// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_SWAP_ROWS_OR_ELEMENTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_SWAP_ROWS_OR_ELEMENTS_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/linalg/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/math_traits.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/linalg/swap_rows.hpp"
#include "arene/base/mdspan/detail/canonical_index.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_lvalue_reference.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/utility/swap.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace arene {
namespace base {
namespace linalg {
namespace detail {

namespace swap_rows_or_elements_detail {

/// @brief Class to swap matrix rows or vector elements
class swap_rows_or_elements_fn {
 public:
  /// @brief Swap two rows of an inout matrix
  /// @tparam InOutMat Type of the matrix whose rows should be swapped
  /// @tparam IndexI Type of the first row index
  /// @tparam IndexJ Type of the second row index
  /// @param mat Matrix whose rows should be swapped
  /// @param left_index Index of the first row to swap
  /// @param right_index Index of the second row to swap
  template <
      typename InOutMat,
      typename IndexI,
      typename IndexJ,
      arene::base::constraints<
          std::enable_if_t<is_inout_matrix_v<InOutMat>>,
          std::enable_if_t<is_invocable_v<decltype(swap_rows), InOutMat, IndexI, IndexJ>>> = nullptr>
  constexpr auto operator()(InOutMat mat, IndexI left_index, IndexJ right_index) const
      noexcept(is_nothrow_invocable_v<decltype(swap_rows), InOutMat, IndexI, IndexJ>) -> void {
    swap_rows(mat, left_index, right_index);
  }

  /// @brief Swap two elements of an inout vector
  /// @tparam InOutVec Type of the vector whose elements should be swapped
  /// @tparam IndexI Type of the first element index; must be nothrow-convertible to the vector's @c index_type
  /// @tparam IndexJ Type of the second element index; must be nothrow-convertible to the vector's @c index_type
  /// @param vec Vector whose elements should be swapped
  /// @param left_index Index of the first element to swap
  /// @param right_index Index of the second element to swap
  /// @note @c swap_elements cannot be used because each element is a rank-0 mdspan
  /// @note Integral-constant indices not representable in @c InOutVec::index_type are rejected at compile time
  /// @pre both indices are representable in the vector's @c index_type
  /// @pre both indices are non-negative and <c>< vec.extent(0)</c>
  template <
      typename InOutVec,
      typename IndexI,
      typename IndexJ,
      arene::base::constraints<
          std::enable_if_t<is_inout_vector_v<InOutVec>>,
          std::enable_if_t<std::is_convertible<IndexI, typename InOutVec::index_type>::value>,
          std::enable_if_t<std::is_convertible<IndexJ, typename InOutVec::index_type>::value>,
          std::enable_if_t<std::is_nothrow_constructible<typename InOutVec::index_type, IndexI>::value>,
          std::enable_if_t<std::is_nothrow_constructible<typename InOutVec::index_type, IndexJ>::value>,
          std::enable_if_t<std::is_lvalue_reference<typename InOutVec::reference>::value>,
          std::enable_if_t<is_swappable_with_v<typename InOutVec::reference, typename InOutVec::reference>>> = nullptr>
  constexpr auto operator()(InOutVec vec, IndexI left_index, IndexJ right_index) const
      noexcept(is_nothrow_swappable_with_v<math_reference_t<InOutVec>, math_reference_t<InOutVec>>) -> void {
    using index_type = typename InOutVec::index_type;
    auto const canonical_left_index = mdspan_detail::canonical_index<index_type>(left_index);
    auto const canonical_right_index = mdspan_detail::canonical_index<index_type>(right_index);

    // Access both elements before the equality check to enforce bounds preconditions even when the indices are equal.
    auto& left_ref = vec(canonical_left_index);
    auto& right_ref = vec(canonical_right_index);

    if (canonical_left_index != canonical_right_index) {
      arene::base::swap(left_ref, right_ref);
    }
  }
};

}  // namespace swap_rows_or_elements_detail

/// @def arene::base::linalg::detail::swap_rows_or_elements
/// @copydoc arene::base::linalg::detail::swap_rows_or_elements_detail::swap_rows_or_elements_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(swap_rows_or_elements_detail::swap_rows_or_elements_fn, swap_rows_or_elements);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace detail
}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_SWAP_ROWS_OR_ELEMENTS_HPP_
