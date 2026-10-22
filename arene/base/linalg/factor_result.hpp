// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_FACTOR_RESULT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_FACTOR_RESULT_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

/// @brief Stores the result of factorization type algorithms
///
/// A @c factor_result contains an optional pivot index signaling either success or failure of the corresponding
/// factorization algorithm. On success, a @c factor_result does not have a pivot index. On failure, a
/// @c factor_result has a pivot index which specifies the first bad pivot encountered during factorization, that is
/// the pivot that caused factorization to fail.
///
/// @note A @c factor_result cannot represent a pivot index with value <c>std::numeric_limits<std::size_t>::max()</c>.
// parasoft-begin-suppress AUTOSAR-A3_1_5-a "False positive: all functions are constexpr so are intended to be
// inlined"
class factor_result {
 public:
  /// @brief Construct a result denoting success
  /// @post @c has_pivot_index() is @c false
  constexpr factor_result() = default;

  /// @brief Construct a result denoting failure
  /// @param idx Index of the first bad pivot encountered during factorization
  /// @pre <c>idx != std::numeric_limits<std::size_t>::max()</c>, otherwise @c ARENE_PRECONDITION violation
  /// @post @c has_pivot_index() is @c true and @c pivot_index() is @c idx
  constexpr explicit factor_result(std::size_t idx) noexcept
      : index_{idx} {
    ARENE_PRECONDITION(idx != no_pivot_index);
  }

  /// @brief Query whether the factorization succeeded
  /// @return @c true if the factorization succeeded, that is if there is no pivot index
  constexpr explicit operator bool() const noexcept { return index_ == no_pivot_index; }

  /// @brief Query whether the factorization failed
  /// @return @c true if the factorization failed, that is if there is a pivot index
  constexpr auto has_pivot_index() const noexcept -> bool { return !(*this); }

  /// @brief Obtain the pivot index
  /// @pre @c has_pivot_index() is @c true, otherwise @c ARENE_PRECONDITION violation
  /// @return Index of the first bad pivot encountered during factorization
  constexpr auto pivot_index() const noexcept -> std::size_t {
    ARENE_PRECONDITION(has_pivot_index());
    return index_;
  }

 private:
  /// @brief The index value used to denote the absence of a pivot index
  static constexpr std::size_t no_pivot_index{std::numeric_limits<std::size_t>::max()};

  /// @brief The pivot index, or @c no_pivot_index if there is no pivot index
  std::size_t index_{no_pivot_index};
};
// parasoft-end-suppress AUTOSAR-A3_1_5-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_FACTOR_RESULT_HPP_
