// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_INDEX_ARRAY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_INDEX_ARRAY_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/inline_container/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "False positive: all includes are used"
#include "arene/base/algorithm/iota.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace arene {
namespace base {
namespace inline_container_detail {

/// @brief Make an index array for the inline map, containing sequential values from @c 0 to <c>Capacity - 1</c>
/// @tparam IndexType The type to use for the indices
/// @tparam Capacity The capacity of the index array
/// @return An array containing sequential values from @c 0 to <c>Capacity - 1</c>
template <class IndexType, std::size_t Capacity>
constexpr auto make_index_array() noexcept -> arene::base::array<IndexType, Capacity> {
  arene::base::array<IndexType, Capacity> index_array{};
  arene::base::iota(index_array.begin(), index_array.end(), IndexType{});
  return index_array;
}

}  // namespace inline_container_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_INDEX_ARRAY_HPP_
