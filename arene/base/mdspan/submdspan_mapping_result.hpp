// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_MAPPING_RESULT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_MAPPING_RESULT_HPP_

#include "arene/base/mdspan/is_layout_mapping.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"

namespace arene {
namespace base {

/// @brief Specializations of @c submdspan_mapping_result are returned by overloads of
/// @c submdspan_mapping.
/// @tparam LayoutMapping the resulting mapping type of the @c submdspan.
template <class LayoutMapping>
struct submdspan_mapping_result {
  static_assert(is_layout_mapping_v<LayoutMapping>, "LayoutMapping must meet the layout mapping requirements");

  // parasoft-begin-suppress AUTOSAR-A2_10_1 "False positive: There is no identifier 'mapping' being hidden"
  /// @brief The resulting mapping for the @c submdspan.
  LayoutMapping mapping{};
  // parasoft-end-suppress AUTOSAR-A2_10_1 "False positive: There is no identifier 'mapping' being hidden"

  /// @brief The starting offset for the @c submdspan.
  std::size_t offset{};
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_MAPPING_RESULT_HPP_
