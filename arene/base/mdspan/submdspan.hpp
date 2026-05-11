// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::submdspan"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/mdspan/detail/do_submdspan_mapping.hpp"
#include "arene/base/mdspan/detail/mandate_each_argtype_is_submdspan_slice.hpp"
#include "arene/base/mdspan/is_sliceable_mapping.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/submdspan_canonicalize_slices.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace arene {
namespace base {
namespace submdspan_detail {

/// @brief Creates a view of a subset of an existing @c mdspan.
class submdspan_fn {
 public:
  /// @brief create a submdspan from a source mdspan and slice specifiers
  /// @tparam ElementType element type of the source mdspan
  /// @tparam Extents extents type of the source mdspan
  /// @tparam LayoutPolicy layout policy of the source mdspan
  /// @tparam AccessorPolicy accessor policy of the source mdspan
  /// @tparam SliceSpecifiers pack of slice specifier types, one per dimension
  /// @param src the source mdspan to slice
  /// @param slices one slice specifier per dimension of @c src
  /// @return a new mdspan viewing the subset of @c src described by @c slices
  ///
  /// @c submdspan returns an @c mdspan that is a subset of the source @c mdspan. That is, every element of the returned
  /// @c mdspan is an element of the source @c mdspan.
  ///
  /// A slice specifier for each dimension controls which elements are included:
  /// * a single integral value or @c integral-constant-like reduces the rank by one, selecting a single index
  /// * a pair-like or @c extent_slice or @c range_slice selects a contiguous range of indices
  /// * @c full_extent_t selects the entire dimension
  ///
  /// @c submdspan calls @c submdspan_mapping using argument-dependent lookup to obtain the sub-mapping and offset.
  ///
  /// @note Constraints <br>
  ///   * <c> sizeof...(SliceSpecifiers) == Extents::rank() </c>
  ///   * <c> is_sliceable_mapping_v<LayoutPolicy::mapping<Extents>> </c> is @c true
  ///
  /// @note Mandates <br>
  ///   * each type @c S in @c SliceSpecifiers... is a valid submdspan slice for @c Extents::index_type
  ///   * the canonical form of each @c S in @c SliceSpecifiers... is a valid submdspan slice type for the associated
  ///   static extent of @c Extents
  ///
  /// @note Preconditions <br>
  ///   * for each rank index @c k of @c src.extents(), @c the canonical form of each @c s in @c slices... denotes a
  ///     valid submdspan slice for the @c k-th extent of @c src.extents()
  template <
      class ElementType,
      class Extents,
      class LayoutPolicy,
      class AccessorPolicy,
      class... SliceSpecifiers,
      std::nullptr_t =
          mdspan_detail::mandate_each_argtype_is_submdspan_slice<typename Extents::index_type, SliceSpecifiers...>(),
      constraints<
          std::enable_if_t<sizeof...(SliceSpecifiers) == Extents::rank()>,
          std::enable_if_t<
              submdspan_detail::is_sliceable_mapping_v<typename LayoutPolicy::template mapping<Extents>>>> = nullptr>
  constexpr auto
  operator()(mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> const& src, SliceSpecifiers... slices) const
      noexcept(is_nothrow_invocable_v<decltype(submdspan_canonicalize_slices), Extents const&, SliceSpecifiers&&...>) {
    auto const sub_map_offset = apply(
        bind_front(mdspan_detail::do_submdspan_mapping, src.mapping()),
        submdspan_canonicalize_slices(src.extents(), std::move(slices)...)
    );

    using sub_mapping = decltype(sub_map_offset.mapping);

    return mdspan<
        ElementType,
        typename sub_mapping::extents_type,
        typename sub_mapping::layout_type,
        typename AccessorPolicy::offset_policy>{
        src.accessor().offset(src.data_handle(), sub_map_offset.offset),
        sub_map_offset.mapping,
        typename AccessorPolicy::offset_policy{src.accessor()}
    };
  }
};

}  // namespace submdspan_detail

/// @def arene::base::submdspan
/// @copydoc arene::base::submdspan_detail::submdspan_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(submdspan_detail::submdspan_fn, submdspan);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_HPP_
