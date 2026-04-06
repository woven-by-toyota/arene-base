// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines
// arene::base::mdspan_detail::rank_scan"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_RANK_SCAN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_RANK_SCAN_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/mdspan/detail/masked_array.hpp"  // IWYU pragma: export
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/utility/in_place.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A13_5_1-a "False positive: rule does not require non-const 'operator[]'"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object implementing rank_scan
class rank_scan_fn  //
{
  /// @brief helper to initialize elements with or without a value
  /// @param value @c true to initialize as non-empty
  /// @return a non-empty @c optional if @c value is @c true, otherwise an empty
  ///   @c optional
  static constexpr auto init_if(bool const value) noexcept -> optional<std::size_t> {
    return value ? optional<std::size_t>{in_place} : optional<std::size_t>{};
  }

  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameter 'mask' is used"
  /// @brief helper to initialize elements with or without a value
  /// @tparam Is indices
  /// @param mask boolean mask specifying the elements with a value
  /// @return array of optionals where each element is initialized to contain a value or not
  /// @note Elements of the returned array must be initialized to either contain a value or not contain a value as we
  ///   cannot change the active element in a union within a constant expression with C++14.
  template <std::size_t... Is>
  static constexpr auto
  init_masked_elements(array<bool, sizeof...(Is)> const& mask, std::index_sequence<Is...>) noexcept
      -> array<optional<std::size_t>, sizeof...(Is)> {
    // parasoft-begin-suppress AUTOSAR-M8_5_2-a-2 "False positive: correct initialization"
    return array<optional<std::size_t>, sizeof...(Is)>{init_if(mask[Is])...};
    // parasoft-end-suppress AUTOSAR-M8_5_2-a-2
  }
  // parasoft-end-suppress AUTOSAR-A0_1_4-a

 public:
  /// @brief construct a mapping for extents from a domain to a codomain
  /// @tparam Rank The number of extents
  /// @param mask for each rank, @c true if the rank is kept by the
  ///   mapping, @c false if it is dropped
  ///
  /// Provides a mapping for rank indices in a domain to the corresponding index
  /// in the codomain. The codomain value is empty if the rank has been dropped
  /// by the mapping and trying to use this rank will result in a precondition
  /// violation.
  ///
  /// @return a masked array providing a mapping of each extent @c r:
  ///   * no mapping if @c mask[r] is @c false
  ///   * the sum of @c true values in the range <c> [mask[0], mask[r]) <c> if @c
  ///     mask[r] is @c true
  ///
  /// @note This behavior is similar to a prefix sum or scan algorithm
  ///
  template <std::size_t Rank>
  constexpr auto operator()(array<bool, Rank> const& mask) const noexcept  //
      -> masked_array<Rank>                                                //
  {
    auto masked = init_masked_elements(mask, std::make_index_sequence<Rank>{});

    std::size_t index{};
    for (auto& elem : masked) {
      if (elem) {
        *elem = index;
        ++index;
      }
    }

    return masked_array<Rank>{masked};
  }
};

/// @def arene::base::mdspan_detail::rank_scan
/// @copydoc arene::base::mdspan_detail::rank_scan_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(mdspan_detail::rank_scan_fn, rank_scan);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_RANK_SCAN_HPP_
