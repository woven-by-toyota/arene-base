// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::full_extent"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_FULL_EXTENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_FULL_EXTENT_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"

namespace arene {
namespace base {

/// @brief tag class specifying the full extent of an mdspan
///
class full_extent_t {
 public:
  /// @brief explicit default constructor
  explicit full_extent_t() = default;
};

// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variables are initialized"
// parasoft-begin-suppress AUTOSAR-M3_4_1-b "This is a per-TU reference to a global used in multiple TUs"
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
/// @def arene::base::full_extent
/// @brief tag specifying the full extent of an mdspan
///
/// @c full_extent is a disambiguation tag that can be passed to @c submdspan to
/// maintain the full range of indices in the returned @c mdspan for a specific
/// dimension.
///
/// @relates full_extent_t
/// @see full_extent_t
ARENE_CPP14_INLINE_VARIABLE(full_extent_t, full_extent);

// parasoft-end-suppress CERT_CPP-DCL56-a
// parasoft-end-suppress AUTOSAR-M3_4_1-b
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_FULL_EXTENT_HPP_
