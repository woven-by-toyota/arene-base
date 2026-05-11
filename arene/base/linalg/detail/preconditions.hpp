// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_PRECONDITIONS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_PRECONDITIONS_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"

namespace arene {
namespace base {
namespace linalg {

namespace preconditions_detail {

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

/// @brief Determine if a set of object instances forms a valid linalg addition (not merely potentially valid)
class actually_addable_impl {
 public:
  /// @brief Check whether or not it's actually possible to perform an addition with this particular set of linalg
  /// objects
  /// @tparam LeftInput The type of the left of the two inputs to the addition, here constrained to be a vector
  /// @tparam RightInput The type of the right of the two inputs to the addition, here constrained to be a vector
  /// @tparam Output The type of the output of the addition, here constrained to be a vector
  /// @param left The actual instance of @c LeftInput which we're attempting to add
  /// @param right The actual instance of @c RightInput which we're attempting to add
  /// @param output The actual instance of @c Output into which we're attempting to write the result of the addition
  /// @return @c true if all of the parameters are suitable for the given addition, @c false if not
  template <
      class LeftInput,
      class RightInput,
      class Output,
      constraints<
          std::enable_if_t<is_in_vector_v<LeftInput>>,
          std::enable_if_t<is_in_vector_v<RightInput>>,
          std::enable_if_t<is_in_vector_v<Output>>> = nullptr>
  constexpr auto operator()(LeftInput const& left, RightInput const& right, Output const& output) const noexcept
      -> bool {
    return (left.extent(0U) == right.extent(0U)) && (left.extent(0U) == output.extent(0U));
  }

  /// @brief Check whether or not it's actually possible to perform an addition with this particular set of linalg
  /// objects
  /// @tparam LeftInput The type of the left of the two inputs to the addition, here constrained to be a matrix
  /// @tparam RightInput The type of the right of the two inputs to the addition, here constrained to be a matrix
  /// @tparam Output The type of the output of the addition, here constrained to be a matrix
  /// @param left The actual instance of @c LeftInput which we're attempting to add
  /// @param right The actual instance of @c RightInput which we're attempting to add
  /// @param output The actual instance of @c Output into which we're attempting to write the result of the addition
  /// @return @c true if all of the parameters are suitable for the given addition, @c false if not
  template <
      class LeftInput,
      class RightInput,
      class Output,
      constraints<
          std::enable_if_t<is_in_matrix_v<LeftInput>>,
          std::enable_if_t<is_in_matrix_v<RightInput>>,
          std::enable_if_t<is_in_matrix_v<Output>>> = nullptr>
  constexpr auto operator()(LeftInput const& left, RightInput const& right, Output const& output) const noexcept
      -> bool {
    bool const heights_compatible{(left.extent(0U) == right.extent(0U)) && (left.extent(0U) == output.extent(0U))};
    bool const widths_compatible{(left.extent(1U) == right.extent(1U)) && (left.extent(1U) == output.extent(1U))};
    return heights_compatible && widths_compatible;
  }
};

/// @brief Determine if a set of object instances forms a valid linalg multiplication (not merely potentially valid)
class actually_multipliable_impl {
 public:
  /// @brief Check whether or not it's actually possible to perform a multiplication with this particular set of objects
  /// @tparam LeftInput The type of the left of the two inputs to the multiplication, here constrained to be a vector
  /// @tparam RightInput The type of the right of the two inputs to the multiplication, here constrained to be a matrix
  /// @tparam Output The type of the output of the multiplication, here constrained to be a vector
  /// @param left The actual instance of @c LeftInput which we're attempting to multiply
  /// @param right The actual instance of @c RightInput which we're attempting to multiply
  /// @param output The actual instance of @c Output into which we're attempting to write the result of the
  /// multiplication
  /// @return @c true if all of the parameters are suitable for the given multiplication, @c false if not
  template <
      class LeftInput,
      class RightInput,
      class Output,
      constraints<
          std::enable_if_t<is_in_vector_v<LeftInput>>,
          std::enable_if_t<is_in_matrix_v<RightInput>>,
          std::enable_if_t<is_in_vector_v<Output>>> = nullptr>
  constexpr auto operator()(LeftInput const& left, RightInput const& right, Output const& output) const noexcept
      -> bool {
    // This overload is for vector-matrix multiplication so we need to check that the transpose of @c left is compatible
    // with the height of <c>right</c>, and the width of @c right with the length of <c>output</c>.
    return (left.extent(0U) == right.extent(0U)) && (right.extent(1U) == output.extent(0U));
  }

  /// @brief Check whether or not it's actually possible to perform a multiplication with this particular set of objects
  /// @tparam LeftInput The type of the left of the two inputs to the multiplication, here constrained to be a matrix
  /// @tparam RightInput The type of the right of the two inputs to the multiplication, here constrained to be a vector
  /// @tparam Output The type of the output of the multiplication, here constrained to be a vector
  /// @param left The actual instance of @c LeftInput which we're attempting to multiply
  /// @param right The actual instance of @c RightInput which we're attempting to multiply
  /// @param output The actual instance of @c Output into which we're attempting to write the result of the
  /// multiplication
  /// @return @c true if all of the parameters are suitable for the given multiplication, @c false if not
  template <
      class LeftInput,
      class RightInput,
      class Output,
      constraints<
          std::enable_if_t<is_in_matrix_v<LeftInput>>,
          std::enable_if_t<is_in_vector_v<RightInput>>,
          std::enable_if_t<is_in_vector_v<Output>>> = nullptr>
  constexpr auto operator()(LeftInput const& left, RightInput const& right, Output const& output) const noexcept
      -> bool {
    // This overload is for matrix-vector multiplication so we need to check that the width of @c left is compatible
    // with the length of <c>right</c>, and the height of @c left with the length of <c>output</c>.
    return (left.extent(1U) == right.extent(0U)) && (left.extent(0U) == output.extent(0U));
  }

  /// @brief Check whether or not it's actually possible to perform a multiplication with this particular set of objects
  /// @tparam LeftInput The type of the left of the two inputs to the multiplication, here constrained to be a matrix
  /// @tparam RightInput The type of the right of the two inputs to the multiplication, here constrained to be a matrix
  /// @tparam Output The type of the output of the multiplication, here constrained to be a matrix
  /// @param left The actual instance of @c LeftInput which we're attempting to multiply
  /// @param right The actual instance of @c RightInput which we're attempting to multiply
  /// @param output The actual instance of @c Output into which we're attempting to write the result of the
  /// multiplication
  /// @return @c true if all of the parameters are suitable for the given multiplication, @c false if not
  template <
      class LeftInput,
      class RightInput,
      class Output,
      constraints<
          std::enable_if_t<is_in_matrix_v<LeftInput>>,
          std::enable_if_t<is_in_matrix_v<RightInput>>,
          std::enable_if_t<is_in_matrix_v<Output>>> = nullptr>
  constexpr auto operator()(LeftInput const& left, RightInput const& right, Output const& output) const noexcept
      -> bool {
    // This overload is for matrix-matrix multiplication so we need to check that the width of @c left is compatible
    // with the height of <c>right</c>, while @c output comes from the height of @c left and the width of <c>right</c>.
    bool const internal_contraction_valid{(left.extent(1U) == right.extent(0U))};
    bool const output_height_matches{left.extent(0U) == output.extent(0U)};
    bool const output_width_matches{right.extent(1U) == output.extent(1U)};
    return internal_contraction_valid && output_height_matches && output_width_matches;
  }
};

/// @brief Determine if two linalg object instances can be validly compared (not merely potentially compared)
class actually_comparable_impl {
 public:
  /// @brief Check whether or not it's actually possible to compare these particular objects
  /// @tparam LeftInput The type of the left of the two inputs to the comparison
  /// @tparam RightInput The type of the right of the two inputs to the comparison
  /// @param left The actual instance of @c LeftInput which we're attempting to compare
  /// @param right The actual instance of @c RightInput which we're attempting to compare
  /// @return @c true if @c left and @c right can be compared, @c false if not
  template <
      class LeftInput,
      class RightInput,
      constraints<std::enable_if_t<
          (is_in_vector_v<LeftInput> && is_in_vector_v<RightInput>) ||
          (is_in_matrix_v<LeftInput> && is_in_matrix_v<RightInput>)>> = nullptr>
  constexpr auto operator()(LeftInput const& left, RightInput const& right) const noexcept -> bool {
    return actually_addable_impl{}(left, right, right);
  }
};

// parasoft-end-suppress AUTOSAR-M2_10_1-a

}  // namespace preconditions_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object"
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object"
/// @def arene::base::linalg::actually_addable
/// @brief Determine if a set of object instances forms a valid linalg addition (not merely potentially valid)
///
/// @see preconditions_detail::actually_addable_impl
ARENE_CPP14_INLINE_VARIABLE(preconditions_detail::actually_addable_impl, actually_addable);

/// @def arene::base::linalg::actually_multipliable
/// @brief Determine if a set of object instances forms a valid linalg multiplication (not merely potentially valid)
///
/// @see preconditions_detail::actually_multipliable_impl
ARENE_CPP14_INLINE_VARIABLE(preconditions_detail::actually_multipliable_impl, actually_multipliable);

/// @def arene::base::linalg::actually_comparable
/// @brief Determine if a set of object instances forms a valid linalg comparison (not merely potentially valid)
///
/// @see preconditions_detail::actually_comparable_impl
ARENE_CPP14_INLINE_VARIABLE(preconditions_detail::actually_comparable_impl, actually_comparable);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_PRECONDITIONS_HPP_
