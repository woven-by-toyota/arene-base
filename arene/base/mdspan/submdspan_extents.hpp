// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines submdspan_extents"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_EXTENTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_EXTENTS_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/submdspan_canonicalize_slices.hpp"
#include "arene/base/mdspan/submdspan_subextents_type.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace submdspan_extents_detail {

/// @brief compute the extents of a submdspan defined by slice specifiers
///
/// Given a source @c extents and a pack of submdspan slice specifiers, computes
/// the extents of the resulting submdspan after applying the slices.
///
/// The slice specifiers are first converted to canonical form using
/// @c submdspan_canonicalize_slices, and the resulting canonical slices are then
/// used to determine the rank and extent values of the submdspan.
class submdspan_extents_fn {
 public:
  /// @brief compute submdspan extents from slice specifiers
  ///
  /// @tparam IndexType index type of the source extents
  /// @tparam Extents static extent values of the source extents
  /// @tparam SliceSpecifiers submdspan slice specifier types
  ///
  /// @param src source @c extents to slice
  /// @param raw_slices submdspan slice specifiers
  ///
  /// Canonicalizes @c raw_slices with a call to
  /// <c> submdspan_canonicalize_slices(src, raw_slices...) </c>,
  /// and uses the resulting canonical slices to determine the rank and extent
  /// values of the submdspan.
  ///
  /// Returns an @c extents specialization describing the extents of the
  /// submdspan obtained by applying the canonical slices to @c src.
  ///
  /// @return A value @c ext of type
  /// @c submdspan_subextents_type_t<Extents, SliceSpecifiers...> that, for each
  /// rank index @c k of @c extents<IndexType, Extents...> where the type of the
  /// canonical slice @c slices...[k] is not a collapsing slice type, the
  /// following holds.
  /// Let @c sk denote @c slices...[k], and let @c r denote
  /// <c> MAP_RANK(slices, k) </c>:
  /// * if the type of @c sk is a specialization of @c extent_slice,
  ///   <c> ext.extent(r) </c> equals @c sk.extent;
  /// * otherwise, <c> ext.extent(r) </c> equals <c> U - L </c>, where
  ///   <c> [L, U) </c> is the submdspan slice range denoted by @c sk for the
  ///   @c k-th extent of @c src.
  ///
  /// @note Constraints <br>
  ///   * <c> sizeof...(Extents) == sizeof...(SliceSpecifiers) </c>
  ///
  /// @note Mandates <br>
  ///   * for each rank index @c k of @c src, @c SliceSpecifiers...[k] is a valid
  ///     submdspan slice type for @c IndexType
  ///   * the canonical form of each slice specifier is a valid submdspan slice
  ///     type for the associated static extent of @c src
  ///
  /// @note Preconditions <br>
  ///   * for each rank index @c k of @c src, the canonical slice
  ///     @c slices...[k] denotes a valid submdspan slice for the @c k-th extent
  ///     of @c src
  ///
  /// @note Remarks <br>
  ///   * the rank of the returned extents is equal to
  ///     <c> MAP_RANK(slices, extents::rank()) </c>
  template <
    class IndexType,
    std::size_t... Extents,
    class... SliceSpecifiers,
    constraints<std::enable_if_t<sizeof...(Extents) == sizeof...(SliceSpecifiers)>> = nullptr,
    std::nullptr_t = mdspan_detail::mandate_each_argtype_is_submdspan_slice<IndexType, SliceSpecifiers...>()
  >
  constexpr auto operator()(extents<IndexType, Extents...> const& src, SliceSpecifiers... raw_slices) const noexcept(  //
      is_nothrow_invocable_v<
          decltype(submdspan_canonicalize_slices),
          extents<IndexType, Extents...> const& ,
          SliceSpecifiers&&...>
  ) -> submdspan_subextents_type_t<extents<IndexType, Extents...>, SliceSpecifiers...> {
    std::ignore = submdspan_canonicalize_slices(src, std::move(raw_slices)...);

    // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: function does have a return"
    // parasoft-begin-suppress CERT_C-MSC37-a "False positive: function does have a return"
    // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: function does have a return"
    return {};
    // parasoft-end-suppress AUTOSAR-A8_4_2-a
    // parasoft-end-suppress CERT_C-MSC37-a
    // parasoft-end-suppress CERT_CPP-MSC52-a
  }
};

}  // namespace submdspan_extents_detail

/// @def arene::base::submdspan_extents
/// @copydoc arene::base::submdspan_extents_detail::submdspan_extents_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(submdspan_extents_detail::submdspan_extents_fn, submdspan_extents);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_EXTENTS_HPP_
