// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_DETAIL_BAD_VARIANT_ACCESS_EXCEPTIONS_DISABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_DETAIL_BAD_VARIANT_ACCESS_EXCEPTIONS_DISABLED_HPP_

// IWYU pragma: private, include "arene/base/variant.hpp"
// IWYU pragma: friend "arene/base/variant/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/variant/traits.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {

namespace variant_detail {
/// @brief checks if the variant's index is not valueless and holds a value of the given index.
/// @param requested the index of expected object
/// @param index the index of owned object
/// @pre @c requested==index and @c index!=variant_npos else @c ARENE_PRECONDITION violation.
inline void variant_must_have_value(std::size_t const requested, std::size_t const index) noexcept {
  ARENE_PRECONDITION(index != variant_npos);
  ARENE_PRECONDITION(index == requested);
}
}  // namespace variant_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_DETAIL_BAD_VARIANT_ACCESS_EXCEPTIONS_DISABLED_HPP_
