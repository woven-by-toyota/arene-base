// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines submdspan_offset"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SUBMDSPAN_OFFSET_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SUBMDSPAN_OFFSET_HPP_

#include "arene/base/algorithm/equal.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/mdspan/detail/representable_cast.hpp"
#include "arene/base/mdspan/detail/slice_lower_bound.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/not_equal_to.hpp"
#include "arene/base/tuple/apply.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

namespace submdspan_offset_detail {

/// @brief Compare two ranges and return @c true if any elements are equal
/// @tparam Range1 the type of the first range
/// @tparam Range2 the type of the second range
/// @param range1 the first range
/// @param range2 the second range
/// @return @c true if any elements are equal, @c false otherwise
template <class Range1, class Range2>
constexpr auto any_equal(Range1 range1, Range2 range2) -> bool {
  return !arene::base::equal(range1.begin(), range1.end(), range2.begin(), std::not_equal_to<>{});
}

/// @brief check if any lower bound equals the corresponding extent
/// @tparam BoundsArray array type of the lower bounds
/// @tparam Extents extents type of the mapping
/// @param bounds array of lower bounds
/// @param exts the extents of the mapping
/// @return @c true if any lower bound equals the corresponding extent
template <class BoundsArray, class Extents>
constexpr auto any_at_extent(BoundsArray const& bounds, Extents const& exts) noexcept -> bool {
  return any_equal(bounds, to_array(exts));
}

}  // namespace submdspan_offset_detail

/// @brief function object that computes the submdspan offset
///
/// Computes the offset to the start of a submdspan within the original span.
/// The slice specifiers must be canonical.
class submdspan_offset_fn  //
{
 public:
  /// @brief compute the offset from a layout mapping and slice specifiers
  /// @tparam LayoutMapping a layout mapping type
  /// @tparam SliceSpecifiers types of the slice specifiers
  /// @param mapping the layout mapping of the original mdspan
  /// @param slices the slice specifiers defining the submdspan
  /// @pre @c sizeof...(SliceSpecifiers) == LayoutMapping::extents_type::rank()
  /// @return the offset to the start of the submdspan as @c std::size_t
  template <
      class LayoutMapping,
      class... SliceSpecifiers,
      constraints<std::enable_if_t<sizeof...(SliceSpecifiers) == LayoutMapping::extents_type::rank()>> = nullptr>
  constexpr auto operator()(LayoutMapping const& mapping, SliceSpecifiers const&... slices) const noexcept
      -> std::size_t {
    using index_type = typename LayoutMapping::index_type;

    // parasoft-begin-suppress AUTOSAR-M8_5_2-a "False positive: there is no subobject here that can be initialized"
    auto const lower_bounds = array<index_type, sizeof...(SliceSpecifiers)>{slice_lower_bound<index_type>(slices)...};
    // parasoft-end-suppress AUTOSAR-M8_5_2-a

    if (submdspan_offset_detail::any_at_extent(lower_bounds, mapping.extents())) {
      return representable_cast<std::size_t>(mapping.required_span_size());
    }

    return representable_cast<std::size_t>(arene::base::apply(mapping, lower_bounds));
  }
};

/// @def arene::base::mdspan_detail::submdspan_offset
/// @copydoc arene::base::mdspan_detail::submdspan_offset_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(mdspan_detail::submdspan_offset_fn, submdspan_offset);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SUBMDSPAN_OFFSET_HPP_
