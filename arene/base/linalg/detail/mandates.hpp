// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_MANDATES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_MANDATES_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"

namespace arene {
namespace base {
namespace linalg {

// parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: the function does always return"
// parasoft-begin-suppress CERT_C-MSC37-a "False positive: the function does always return"
// parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: the function does always return"
// parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: all parameters are used"
/// @brief Check whether or not the given extents of these two <c>mdspan</c>s are potentially compatible
/// @tparam LeftSpan The left of the two <c>mdspan</c>s to check
/// @tparam RightSpan The right of the two <c>mdspan</c>s to check
/// @param left_rank_idx The specific rank within @c LeftSpan to compare
/// @param right_rank_idx The specific rank within @c RightSpan to compare
/// @return @c true if @c decltype<LeftSpan&>().extent(left_rank_idx) could possibly be equal to
/// <c>decltype<RightSpan&>().extent(right_rank_idx)</c>, @c false otherwise
template <
    class LeftSpan,
    class RightSpan,
    arene::base::constraints<std::enable_if_t<is_mdspan_v<LeftSpan>>, std::enable_if_t<is_mdspan_v<RightSpan>>> =
        nullptr>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters) The parameters are symmetric
constexpr auto compatible_static_extents(std::size_t left_rank_idx, std::size_t right_rank_idx) noexcept -> bool {
  std::size_t const left_extent{LeftSpan::static_extent(left_rank_idx)};
  std::size_t const right_extent{RightSpan::static_extent(right_rank_idx)};

  return (left_extent == right_extent) || (left_extent == dynamic_extent) || (right_extent == dynamic_extent);
}
// parasoft-end-suppress AUTOSAR-A8_4_2-a
// parasoft-end-suppress CERT_C-MSC37-a
// parasoft-end-suppress CERT_CPP-MSC52-a
// parasoft-end-suppress AUTOSAR-A0_1_4-a

namespace mandates_detail {

/// @brief Check whether or not it's possible to form a valid addition with operands of the given types
/// @tparam LeftInput The type of the left of the two inputs to the addition
/// @tparam RightInput The type of the right of the two inputs to the addition
/// @tparam Output The type of the output of the addition
template <class LeftInput, class RightInput, class Output, class = arene::base::constraints<>>
constexpr bool possibly_addable_impl{false};

/// @brief Check whether or not it's possible to form a valid addition with vector + vector operands
/// @tparam LeftInVector The type of the vector used as the left of the two inputs to the addition
/// @tparam RightInVector The type of the vector used as the right of the two inputs to the addition
/// @tparam OutVector The type of the vector used as the output of the addition
template <class LeftInVector, class RightInVector, class OutVector>
constexpr bool possibly_addable_impl<
    LeftInVector,
    RightInVector,
    OutVector,
    arene::base::constraints<
        std::enable_if_t<is_in_vector_v<LeftInVector>>,
        std::enable_if_t<is_in_vector_v<RightInVector>>,
        std::enable_if_t<is_in_vector_v<OutVector>>>>{
    compatible_static_extents<LeftInVector, RightInVector>(0, 0) &&
    compatible_static_extents<LeftInVector, OutVector>(0, 0) &&
    compatible_static_extents<RightInVector, OutVector>(0, 0)
};

/// @brief Check whether or not it's possible to form a valid addition with matrix + matrix operands
/// @tparam LeftInMatrix The type of the matrix used as the left of the two inputs to the addition
/// @tparam RightInMatrix The type of the matrix used as the right of the two inputs to the addition
/// @tparam OutMatrix The type of the matrix used as the output of the addition
template <class LeftInMatrix, class RightInMatrix, class OutMatrix>
constexpr bool possibly_addable_impl<
    LeftInMatrix,
    RightInMatrix,
    OutMatrix,
    arene::base::constraints<
        std::enable_if_t<is_in_matrix_v<LeftInMatrix>>,
        std::enable_if_t<is_in_matrix_v<RightInMatrix>>,
        std::enable_if_t<is_in_matrix_v<OutMatrix>>>>{
    compatible_static_extents<LeftInMatrix, RightInMatrix>(0, 0) &&
    compatible_static_extents<LeftInMatrix, RightInMatrix>(1, 1) &&
    compatible_static_extents<LeftInMatrix, OutMatrix>(0, 0) &&
    compatible_static_extents<LeftInMatrix, OutMatrix>(1, 1) &&
    compatible_static_extents<RightInMatrix, OutMatrix>(0, 0) &&
    compatible_static_extents<RightInMatrix, OutMatrix>(1, 1)
};

/// @brief Check whether or not it's possible to form a valid multiplication with operands of the given types
/// @tparam LeftInput The type of the left of the two inputs to the multiplication
/// @tparam RightInput The type of the right of the two inputs to the multiplication
/// @tparam Output The type of the output of the multiplication
template <class LeftInput, class RightInput, class Output, class = arene::base::constraints<>>
constexpr bool possibly_multipliable_impl{false};

/// @brief Check whether or not it's possible to form a valid multiplication with matrix*vector operands
/// @tparam LeftInMatrix The type of the matrix used as the left of the two inputs to the multiplication
/// @tparam RightInVector The type of the vector used as the right of the two inputs to the multiplication
/// @tparam OutVector The type of the vector used as the output of the multiplication
template <class LeftInMatrix, class RightInVector, class OutVector>
constexpr bool possibly_multipliable_impl<
    LeftInMatrix,
    RightInVector,
    OutVector,
    arene::base::constraints<
        std::enable_if_t<is_in_matrix_v<LeftInMatrix>>,
        std::enable_if_t<is_in_vector_v<RightInVector>>,
        std::enable_if_t<is_in_vector_v<OutVector>>>>{
    compatible_static_extents<LeftInMatrix, RightInVector>(1, 0) &&
    compatible_static_extents<LeftInMatrix, OutVector>(0, 0)
};

/// @brief Check whether or not it's possible to form a valid multiplication with covector*matrix operands
/// @tparam LeftInVector The type of the vector whose transpose is the left of the two inputs to the multiplication
/// @tparam RightInMatrix The type of the matrix used as the right of the two inputs to the multiplication
/// @tparam OutVector The type of the vector used as the output of the multiplication
/// @note This is actually considering multiplying the transpose of @c LeftInVector by @c RightInMatrix rather than
/// @c LeftInVector itself (which would be impossible)
template <class LeftInVector, class RightInMatrix, class OutVector>
constexpr bool possibly_multipliable_impl<
    LeftInVector,
    RightInMatrix,
    OutVector,
    arene::base::constraints<
        std::enable_if_t<is_in_vector_v<LeftInVector>>,
        std::enable_if_t<is_in_matrix_v<RightInMatrix>>,
        std::enable_if_t<is_in_vector_v<OutVector>>>>{
    compatible_static_extents<LeftInVector, RightInMatrix>(0, 0) &&
    compatible_static_extents<RightInMatrix, OutVector>(1, 0)
};

/// @brief Check whether or not it's possible to form a valid multiplication with matrix*matrix operands
/// @tparam LeftInMatrix The type of the matrix used as the left of the two inputs to the multiplication
/// @tparam RightInMatrix The type of the matrix used as the right of the two inputs to the multiplication
/// @tparam OutMatrix The type of the matrix used as the output of the multiplication
template <class LeftInMatrix, class RightInMatrix, class OutMatrix>
constexpr bool possibly_multipliable_impl<
    LeftInMatrix,
    RightInMatrix,
    OutMatrix,
    arene::base::constraints<
        std::enable_if_t<is_in_matrix_v<LeftInMatrix>>,
        std::enable_if_t<is_in_matrix_v<RightInMatrix>>,
        std::enable_if_t<is_in_matrix_v<OutMatrix>>>>{
    compatible_static_extents<LeftInMatrix, RightInMatrix>(1, 0) &&
    compatible_static_extents<LeftInMatrix, OutMatrix>(0, 0) &&
    compatible_static_extents<RightInMatrix, OutMatrix>(1, 1)
};

}  // namespace mandates_detail

/// @brief Check whether or not it's possible to form a valid addition with operands of the given types
/// @tparam LeftInput The type of the left of the two inputs to the addition
/// @tparam RightInput The type of the right of the two inputs to the addition
/// @tparam Output The type of the output of the addition
template <class LeftInput, class RightInput, class Output>
extern constexpr bool possibly_addable_v{mandates_detail::possibly_addable_impl<LeftInput, RightInput, Output>};

/// @brief Check whether or not it's possible to form a valid multiplication with operands of the given types
/// @tparam LeftInput The type of the left of the two inputs to the multiplication
/// @tparam RightInput The type of the right of the two inputs to the multiplication
/// @tparam Output The type of the output of the multiplication
template <class LeftInput, class RightInput, class Output>
extern constexpr bool
    possibly_multipliable_v{mandates_detail::possibly_multipliable_impl<LeftInput, RightInput, Output>};

/// @brief Check whether or not it's possible to form a valid comparison with operands of the given types
/// @tparam LeftInput The type of the left of the two inputs to be compared
/// @tparam RightInput The type of the right of the two inputs to be compared
template <class LeftInput, class RightInput>
extern constexpr bool possibly_comparable_v{mandates_detail::possibly_addable_impl<LeftInput, RightInput, RightInput>};

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_MANDATES_HPP_
