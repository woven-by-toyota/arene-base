// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MASKED_ARRAY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MASKED_ARRAY_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/count.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace mdspan_detail {

// parasoft-begin-suppress AUTOSAR-A13_5_1-a "False positive: only const operator[] overload is provided"

/// @brief mapping of indices from a domain to codomain
/// @tparam Size upper bound of the domain
template <std::size_t Size>
class masked_array {
  /// @brief underlying data type
  using underlying_type = array<optional<std::size_t>, Size>;
  /// @brief underlying mapping values
  underlying_type values_;

 public:
  /// @brief construct from values
  /// @param values range where empty elements are not part of the mapping
  constexpr explicit masked_array(underlying_type const& values) noexcept
      : values_{values} {}

  // parasoft-begin-suppress AUTOSAR-M0_1_2-z "False positive: result is not always true or always false"
  /// @brief determine if an index is valid
  /// @param index index, value in domain
  /// @return @c true if the @c index has an associated output value
  constexpr auto is_valid(std::size_t index) const noexcept -> bool {
    return (index < Size) && (values_[index].has_value());
  }
  // parasoft-end-suppress AUTOSAR-M0_1_2-z

  /// @brief obtain the output value for an index
  /// @param index input value
  /// @pre @c index is valid
  /// @return the mapped @c index value
  constexpr auto operator[](std::size_t index) const noexcept -> std::size_t {
    ARENE_PRECONDITION(is_valid(index));
    return *values_[index];
  }

  /// @brief number of elements in the codomain
  /// @return number of elements in the codomain
  constexpr auto codomain_size() const noexcept -> std::size_t {
    return Size - static_cast<std::size_t>(count(values_.cbegin(), values_.cend(), null_opt));
  }
};

// parasoft-end-suppress AUTOSAR-A13_5_1-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MASKED_ARRAY_HPP_
