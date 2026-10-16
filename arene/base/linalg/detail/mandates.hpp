// parasoft-suppress AUTOSAR-A2_8_1-a-2 "Defines traits for linalg related to mandates"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_MANDATES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_MANDATES_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace arene {
namespace base {
namespace linalg {

namespace linalg_detail {

/// @brief function object to compare static extents of two spans for compatibility
/// @tparam LeftSpan The left mdspan or extents type to compare
/// @tparam RightSpan The right mdspan or extents type to compare
template <class LeftSpan, class RightSpan>
class compatible_static_extents_fn {
 public:
  static_assert(
      is_mdspan_v<LeftSpan> || is_extents_v<LeftSpan>,
      "'LeftSpan' must be a specialization of 'mdspan' or 'extents'"
  );
  static_assert(
      is_mdspan_v<RightSpan> || is_extents_v<RightSpan>,
      "'RightSpan' must be a specialization of 'mdspan' or 'extents'"
  );

  /// @brief Check whether or not the given extents of these two <c>mdspan</c>s are potentially compatible
  /// @tparam LeftSpan The left of the two <c>mdspan</c>s to check
  /// @tparam RightSpan The right of the two <c>mdspan</c>s to check
  /// @param left_rank_idx The specific rank within @c LeftSpan to compare
  /// @param right_rank_idx The specific rank within @c RightSpan to compare
  /// @return @c true if @c decltype<LeftSpan&>().extent(left_rank_idx) could possibly be equal to
  /// <c>decltype<RightSpan&>().extent(right_rank_idx)</c>, @c false otherwise
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters) The parameters are symmetric
  constexpr auto operator()(std::size_t left_rank_idx, std::size_t right_rank_idx) const noexcept -> bool {
    std::size_t const left_extent{LeftSpan::static_extent(left_rank_idx)};
    std::size_t const right_extent{RightSpan::static_extent(right_rank_idx)};

    return (left_extent == right_extent) || (left_extent == dynamic_extent) || (right_extent == dynamic_extent);
  }

  /// @brief Check whether or not the given extents of these two <c>mdspan</c>s are potentially compatible
  /// @tparam LeftSpan The left of the two <c>mdspan</c>s to check
  /// @tparam RightSpan The right of the two <c>mdspan</c>s to check
  /// @param rank_idx The specific rank within @c LeftSpan and @c RightSpan to compare
  /// @return @c true if @c decltype<LeftSpan&>().extent(rank_idx) could possibly be equal to
  /// <c>decltype<RightSpan&>().extent(rank_idx)</c>, @c false otherwise
  constexpr auto operator()(std::size_t rank_idx) const noexcept -> bool { return (*this)(rank_idx, rank_idx); }

  /// @brief Check whether or not the given extents of these two <c>mdspan</c>s are potentially compatible
  /// @tparam LeftSpan The left of the two <c>mdspan</c>s to check
  /// @tparam RightSpan The right of the two <c>mdspan</c>s to check
  /// @return @c true if @c decltype<LeftSpan&>().extent(rank_idx) could possibly be equal to
  /// <c>decltype<RightSpan&>().extent(rank_idx)</c> for all ranks of @c LeftSpan and @c RightSpan,
  /// otherwise @c false
  template <
      bool SameRank = (LeftSpan::rank() == RightSpan::rank()),
      arene::base::constraints<std::enable_if_t<SameRank>> = nullptr>
  constexpr auto operator()() const noexcept -> bool {
    return all_of(  //
        sequential_values<std::size_t, LeftSpan::rank()>,
        identity{},
        *this
    );
  }
};

}  // namespace linalg_detail

/// @def arene::base::linalg::compatible_static_extents
/// @copydoc arene::base::linalg::linalg_detail::compatible_static_extents_fn::operator()
template <class LeftSpan, class RightSpan>
extern constexpr auto compatible_static_extents = linalg_detail::compatible_static_extents_fn<LeftSpan, RightSpan>{};

namespace linalg_detail {

/// @brief Implements a SFINAE constraint that all extents of the two parameters are compatible
/// @tparam LeftInput The left mdspan or extents type
/// @tparam RightInput The right mdspan or extents type
template <class LeftSpan, class RightSpan>
using all_extents_compatible_constraints = arene::base::constraints<
    std::enable_if_t<LeftSpan::rank() == RightSpan::rank()>,
    std::enable_if_t<compatible_static_extents<LeftSpan, RightSpan>()>>;

/// @brief Implements a SFINAE constraint that it is possible to form a valid comparison with operands of the given
/// types
/// @tparam LeftInObject The type of the left of the two inputs to the comparison
/// @tparam RightInObject The type of the right of the two inputs to the comparison
/// @tparam Gcc8Workaround defaulted and unused parameter; avoids a spurious
///   "substitution failed" with GCC 8 in constexpr contexts
template <class LeftInObject, class RightInObject, class Gcc8Workaround = void>
using possibly_comparable_constraints = arene::base::constraints<
    std::enable_if_t<is_in_object_v<LeftInObject>>,
    std::enable_if_t<is_in_object_v<RightInObject>>,
    // comparing the extents type in 'all_extents_compatible_constraints' gives a better error message
    all_extents_compatible_constraints<typename LeftInObject::extents_type, typename RightInObject::extents_type>>;

/// @brief Check whether or not it's possible to form a valid addition with operands of the given types
/// @tparam LeftInput The type of the left of the two inputs to the addition
/// @tparam RightInput The type of the right of the two inputs to the addition
/// @tparam Output The type of the output of the addition
template <class LeftInObject, class RightInObject, class OutObject>
using possibly_addable_constraints = arene::base::constraints<
    std::enable_if_t<is_in_object_v<LeftInObject>>,
    std::enable_if_t<is_in_object_v<RightInObject>>,
    std::enable_if_t<is_in_object_v<OutObject>>,
    // comparing the extents type in 'all_extents_compatible_constraints' gives a better error message
    all_extents_compatible_constraints<typename LeftInObject::extents_type, typename RightInObject::extents_type>,
    all_extents_compatible_constraints<typename LeftInObject::extents_type, typename OutObject::extents_type>,
    all_extents_compatible_constraints<typename OutObject::extents_type, typename RightInObject::extents_type>>;

/// @brief Check whether or not it's possible to form a valid multiplication with matrix*vector operands
/// @tparam LeftInMatrix The type of the matrix used as the left of the two inputs to the multiplication
/// @tparam RightInVector The type of the vector used as the right of the two inputs to the multiplication
/// @tparam OutVector The type of the vector used as the output of the multiplication
template <class LeftInMatrix, class RightInVector, class OutVector>
using possibly_multipliable_matrix_vector_constraints = arene::base::constraints<
    std::enable_if_t<is_in_matrix_v<LeftInMatrix>>,
    std::enable_if_t<is_in_vector_v<RightInVector>>,
    std::enable_if_t<is_in_vector_v<OutVector>>,
    std::enable_if_t<compatible_static_extents<LeftInMatrix, RightInVector>(1, 0)>,
    std::enable_if_t<compatible_static_extents<LeftInMatrix, OutVector>(0)>>;

/// @brief Check whether or not it's possible to form a valid multiplication with covector*matrix operands
/// @tparam LeftInVector The type of the vector whose transpose is the left of the two inputs to the multiplication
/// @tparam RightInMatrix The type of the matrix used as the right of the two inputs to the multiplication
/// @tparam OutVector The type of the vector used as the output of the multiplication
/// @note This is actually considering multiplying the transpose of @c LeftInVector by @c RightInMatrix rather than
/// @c LeftInVector itself (which would be impossible)
template <class LeftInVector, class RightInMatrix, class OutVector>
using possibly_multipliable_vector_matrix_constraints = arene::base::constraints<
    std::enable_if_t<is_in_vector_v<LeftInVector>>,
    std::enable_if_t<is_in_matrix_v<RightInMatrix>>,
    std::enable_if_t<is_in_vector_v<OutVector>>,
    std::enable_if_t<compatible_static_extents<LeftInVector, RightInMatrix>(0)>,
    std::enable_if_t<compatible_static_extents<RightInMatrix, OutVector>(1, 0)>>;

/// @brief Check whether or not it's possible to form a valid multiplication with matrix*matrix operands
/// @tparam LeftInMatrix The type of the matrix used as the left of the two inputs to the multiplication
/// @tparam RightInMatrix The type of the matrix used as the right of the two inputs to the multiplication
/// @tparam OutMatrix The type of the matrix used as the output of the multiplication
template <class LeftInMatrix, class RightInMatrix, class OutMatrix>
using possibly_multipliable_matrix_matrix_constraints = arene::base::constraints<
    std::enable_if_t<is_in_matrix_v<LeftInMatrix>>,
    std::enable_if_t<is_in_matrix_v<RightInMatrix>>,
    std::enable_if_t<is_in_matrix_v<OutMatrix>>,
    std::enable_if_t<compatible_static_extents<LeftInMatrix, RightInMatrix>(1, 0)>,
    std::enable_if_t<compatible_static_extents<LeftInMatrix, OutMatrix>(0)>,
    std::enable_if_t<compatible_static_extents<RightInMatrix, OutMatrix>(1)>>;

/// @brief Check whether or not the static extents of a type permit its row and column extents to be equal
/// @tparam Extents The type to check
/// @note The primary template answers @c false for every type which is not a rank-2 @c extents specialization
template <class Extents>
extern constexpr bool possibly_square_extents_v{false};

/// @brief Check whether or not the static extents of a rank-2 @c extents permit its row and column extents to be equal
/// @tparam IndexType @c index_type of the @c extents type being checked
/// @tparam RowCount Static row count (extent 0) of the @c extents type being checked
/// @tparam ColumnCount Static column count (extent 1) of the @c extents type being checked
template <class IndexType, std::size_t RowCount, std::size_t ColumnCount>
extern constexpr bool possibly_square_extents_v<extents<IndexType, RowCount, ColumnCount>>{
    compatible_static_extents<extents<IndexType, RowCount, ColumnCount>, extents<IndexType, RowCount, ColumnCount>>(
        0U,
        1U
    )
};

/// @brief Implements a SFINAE constraint that a matrix of the given type can be square
/// @tparam InMatrix The type of the matrix to check
template <class InMatrix>
using possibly_square_constraints = arene::base::constraints<
    std::enable_if_t<is_in_matrix_v<InMatrix>>,
    std::enable_if_t<possibly_square_extents_v<typename InMatrix::extents_type>>>;

}  // namespace linalg_detail

/// @brief Check whether or not it's possible to form a valid comparison with operands of the given types
/// @tparam LeftInput The type of the left of the two inputs to be compared
/// @tparam RightInput The type of the right of the two inputs to be compared
template <class LeftInput, class RightInput>
extern constexpr bool possibly_comparable_v{substitution_succeeds<  //
    linalg_detail::possibly_comparable_constraints,
    LeftInput,
    RightInput>};

/// @brief Check whether or not it's possible to form a valid addition with operands of the given types
/// @tparam LeftInput The type of the left of the two inputs to the addition
/// @tparam RightInput The type of the right of the two inputs to the addition
/// @tparam Output The type of the output of the addition
template <class LeftInput, class RightInput, class Output>
extern constexpr bool possibly_addable_v{substitution_succeeds<
    linalg_detail::possibly_addable_constraints,
    LeftInput,
    RightInput,
    Output>};

/// @brief Check whether or not it's possible to form a valid multiplication with operands of the given types
/// @tparam LeftInput The type of the left of the two inputs to the multiplication
/// @tparam RightInput The type of the right of the two inputs to the multiplication
/// @tparam Output The type of the output of the multiplication
template <class LeftInput, class RightInput, class Output>
extern constexpr bool possibly_multipliable_v{ //
    substitution_succeeds<
        linalg_detail::possibly_multipliable_matrix_vector_constraints,
        LeftInput,
        RightInput,
        Output> ||
    substitution_succeeds<
        linalg_detail::possibly_multipliable_vector_matrix_constraints,
        LeftInput,
        RightInput,
        Output> ||
    substitution_succeeds<
        linalg_detail::possibly_multipliable_matrix_matrix_constraints,
        LeftInput,
        RightInput,
        Output>
};

/// @brief Check whether or not the given type represents a matrix which could be square
/// @tparam InMat The type to check
///
/// @c true when @c InMat is an in-matrix whose static row and column extents are equal or at least one of
/// them is not known statically. @c false for every other type, including any type which is not an in-matrix.
template <class InMat>
extern constexpr bool possibly_square_v{substitution_succeeds<linalg_detail::possibly_square_constraints, InMat>};

/// @brief Trait holding whether or not the given type represents a matrix which could be square
/// @tparam InMat The type to check
/// @see possibly_square_v
template <class InMat>
class possibly_square : public std::integral_constant<bool, possibly_square_v<InMat>> {};

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_MANDATES_HPP_
