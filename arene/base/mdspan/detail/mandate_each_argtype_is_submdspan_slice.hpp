// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MANDATE_EACH_ARGTYPE_IS_SUBMDSPAN_SLICE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MANDATE_EACH_ARGTYPE_IS_SUBMDSPAN_SLICE_HPP_

#include "arene/base/mdspan/is_submdspan_slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/type_traits/all_of.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

// parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: This is a namespace scope function"
/// @brief helper function to generate a hard error if any argument type is not
/// a submdspan slice for an index type
/// @tparam IndexType index type
/// @tparam SliceSpecifiers slice types
/// @return always returns @c nullptr on success
template <class IndexType, class... SliceSpecifiers>
static constexpr auto mandate_each_argtype_is_submdspan_slice() noexcept -> std::nullptr_t {
  static_assert(
      all_of_v<is_submdspan_slice_v<IndexType, SliceSpecifiers>...>,
      "each type in 'SliceSpecifiers' must be a 'submdspan' slice type"
  );
  return nullptr;
}
// parasoft-end-suppress AUTOSAR-A0_1_3-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MANDATE_EACH_ARGTYPE_IS_SUBMDSPAN_SLICE_HPP_
