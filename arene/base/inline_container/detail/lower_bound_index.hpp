// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::lower_bound_index"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_LOWER_BOUND_INDEX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_LOWER_BOUND_INDEX_HPP_

#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/functional/function_ref.hpp"
// IWYU pragma: private
// IWYU pragma: friend "arene/base/inline_container/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace inline_container {
namespace detail {

namespace lower_bound_index_detail {

/// @brief Find the mid point between two values
/// @tparam IndexType The types of the args
/// @param low The low value
/// @param high The high value
/// @return The mid point
template <typename IndexType>
constexpr auto mid_point(IndexType const low, IndexType const high) noexcept -> IndexType {
  constexpr IndexType two{static_cast<IndexType>(2U)};
  return static_cast<IndexType>(low + (high - low) / two);
}
}  // namespace lower_bound_index_detail

/// @brief simple POD to represent the return from @c lower_bound_index .
template <typename IndexType>
struct lower_bound_index_info {
  /// @brief The index of the lower-bound point.
  IndexType index;
  /// @brief The ordering of the element at the lower-bound point relative to the searched element.
  strong_ordering order;
};

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Obtain the index of the first element such that @c element < @c value
///
/// Performs a binary search, comparing elements accessed via @c key_at_index in the index range @c [0,max_index)
/// against @c value using @c compare .
///
/// @tparam KeyType the type of keys to search across
/// @tparam OtherKey the type of the value to search for
/// @tparam IndexType the type of the index to use to access elements.
/// @param key The key to search for
/// @param max_index The maximum index to attempt to access.
/// @param compare A functor which can compare a @c KeyType and a @c OtherKey and produce a @c strong_ordering .
/// @param key_at_index A functor which, given an index, returns a reference to the key to compare @c key against for
///                     that index.
/// @return lower_bound_index_info<IndexType> The index of the element, and the strong_ordering between @c key
///         and that element . If @c key is greater than all elements, then the index is @c max_index and the
///         ordering is @c strong_ordering::greater .
/// @throws Any exception thrown by the comparisons
template <typename KeyType, typename OtherKey, typename IndexType>
auto lower_bound_index(
    OtherKey const& key,
    IndexType const max_index,
    function_ref<strong_ordering(KeyType const&, OtherKey const&)> compare,
    function_ref<KeyType const&(IndexType)> key_at_index
) -> lower_bound_index_info<IndexType> {
  IndexType low{static_cast<IndexType>(0U)};
  IndexType high{max_index};
  while (low != high) {
    IndexType const mid{lower_bound_index_detail::mid_point(low, high)};
    // parasoft-begin-suppress AUTOSAR-M6_4_5-a-2 "All branches terminated as per permit M6-4-5#1"
    // parasoft-begin-suppress AUTOSAR-M6_4_3-a-2 "All branches terminated as per permit M6-4-3#1"
    switch (compare(key_at_index(mid), key)) {
      case strong_ordering::equal:
        return {mid, strong_ordering::equal};
      case strong_ordering::less:
        low = mid;
        ++low;
        break;
      case strong_ordering::greater:
        high = mid;
        break;
      default:
        ARENE_INVARIANT_UNREACHABLE("strong_ordering out of range");
    }
    // parasoft-end-suppress AUTOSAR-M6_4_5-a-2
    // parasoft-end-suppress AUTOSAR-M6_4_3-a-2
  }
  return {low, strong_ordering::greater};
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace detail
}  // namespace inline_container
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_LOWER_BOUND_INDEX_HPP_
