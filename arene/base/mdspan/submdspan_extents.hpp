// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines submdspan_extents"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_EXTENTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_EXTENTS_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/mdspan/detail/canonical_slice.hpp"
#include "arene/base/mdspan/detail/mandate_each_argtype_is_submdspan_slice.hpp"
#include "arene/base/mdspan/detail/map_rank.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/mdspan/submdspan_canonicalize_slices.hpp"
#include "arene/base/mdspan/submdspan_subextents_type.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/tuple/detail/get.hpp"
#include "arene/base/tuple/detail/tuple_transform.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace submdspan_extents_detail {

/// @brief filter tuple elements using a compile-time inverse mapping
/// @tparam Is index sequence corresponding to the elements of the *filtered* tuple
/// @tparam Tuple input tuple type
/// @tparam LiftedInverseMapping compile-time inverse rank mapping
/// @param tuple input tuple
///
/// Constructs a new @c std::tuple by selecting elements from @c tuple at the
/// indices specified by @c LiftedInverseMapping::value.
///
/// @return tuple containing the selected elements
///
/// @note @c LiftedInverseMapping an @c inverse_map_rank lifted into type-space.
///   Lifting the inverse mapping into type-space enables compile-time filtering
///   of tuple elements by index.
template <std::size_t... Is, class Tuple, class LiftedInverseMapping>
constexpr auto filter_impl(std::index_sequence<Is...>, Tuple&& tuple, LiftedInverseMapping) noexcept
    -> std::tuple<std::tuple_element_t<LiftedInverseMapping::value[Is], Tuple>...> {
  return std::tuple<std::tuple_element_t<LiftedInverseMapping::value[Is], Tuple>...>{
      tuple_detail::get<LiftedInverseMapping::value[Is]>(std::forward<Tuple>(tuple))...
  };
}

/// @brief filter tuple elements using a compile-time inverse mapping
/// @tparam Tuple input tuple type
/// @tparam LiftedInverseMapping compile-time inverse rank mapping
/// @param tuple input tuple
/// @return tuple containing the selected elements
///
/// Creates a new tuple by selecting elements from @c tuple according to the
/// indices specified by @c LiftedInverseMapping::value.
///
/// @note @c LiftedInverseMapping an @c inverse_map_rank lifted into type-space.
///   Lifting the inverse mapping into type-space enables compile-time filtering
///   of tuple elements by index.
template <class Tuple, class LiftedInverseMapping>
constexpr auto filter(Tuple tuple, LiftedInverseMapping) noexcept  //
    -> decltype(filter_impl(
        std::make_index_sequence<LiftedInverseMapping::value.size()>{},
        std::move(tuple),
        LiftedInverseMapping{}
    )) {
  return filter_impl(  //
      std::make_index_sequence<LiftedInverseMapping::value.size()>{},
      std::move(tuple),
      LiftedInverseMapping{}
  );
}

/// @brief compute the extents of a submdspan defined by slice specifiers
///
/// Given a source @c extents and a pack of submdspan slice specifiers, computes
/// the extents of the resulting submdspan after applying the slices.
///
/// The slice specifiers are first converted to canonical form using
/// @c submdspan_canonicalize_slices, and the resulting canonical slices are then
/// used to determine the rank and extent values of the submdspan.
class submdspan_extents_fn {
  /// @brief function object to compute extent values for canonical non-collapsing slices
  class extent_from_slice {
   public:
    /// @brief compute the sliced extent value from an @c extent_slice
    /// @tparam IndexType index type
    /// @tparam Offset @c extent_slice offset type
    /// @tparam Extent @c extent_slice extent type
    /// @tparam Stride @c extent_slice stride type
    /// @param slice canonical extent slice
    /// @return extent value of the slice
    template <class IndexType, class Offset, class Extent, class Stride>
    constexpr auto operator()(IndexType const&, extent_slice<Offset, Extent, Stride> slice) const noexcept
        -> IndexType {
      return slice.extent;
    }

    /// @brief propagate the original extent for a full slice
    /// @tparam IndexType index type
    /// @param ext original extent
    /// @return unchanged extent value
    template <class IndexType>
    constexpr auto operator()(IndexType const& ext, full_extent_t) const noexcept -> IndexType {
      return ext;
    }
  };

 public:
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameters 'src' and 'slices' are used"
  /// @brief compute submdspan extents from slice specifiers
  ///
  /// @tparam IndexType index type of the source extents
  /// @tparam Extents static extent values of the source extents
  /// @tparam SliceSpecifiers submdspan slice specifier types
  ///
  /// @param src source @c extents to slice
  /// @param slices submdspan slice specifiers
  ///
  /// Canonicalizes @c slices with a call to
  /// <c> submdspan_canonicalize_slices(src, slices...) </c>,
  /// and uses the resulting canonical slices to determine the rank and extent
  /// values of the submdspan.
  ///
  /// Returns an @c extents specialization describing the extents of the
  /// submdspan obtained by applying the canonical slices to @c src.
  ///
  /// @return A value @c ext of type
  /// @c submdspan_subextents_type_t<Extents, SliceSpecifiers...> that, for each
  /// rank index @c k of @c extents<IndexType, Extents...> where the type of the
  /// canonical slice @c canonical...[k] is not a collapsing slice type, the
  /// following holds.
  /// Let @c sk denote @c canonical...[k], and let @c r denote
  /// <c> MAP_RANK(canonical, k) </c>:
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
  constexpr auto operator()(extents<IndexType, Extents...> const& src, SliceSpecifiers... slices) const noexcept(  //
      is_nothrow_invocable_v<
          decltype(submdspan_canonicalize_slices),
          extents<IndexType, Extents...> const& ,
          SliceSpecifiers&&...>
  ) -> submdspan_subextents_type_t<extents<IndexType, Extents...>, SliceSpecifiers...> {
    // parasoft-begin-suppress AUTOSAR-M0_1_3-a "False positive: 'lifted_inverse_mapping' is used"
    // parasoft-begin-suppress AUTOSAR-M0_1_3-b "False positive: 'lifted_inverse_mapping' is used"
    // Lift the inverse mapping into type-space so we can determine which elements of the tuple can be filtered out.
    constexpr auto lifted_inverse_mapping = std::integral_constant<
        decltype((mdspan_detail::inverse_map_rank<mdspan_detail::canonical_slice_t<IndexType, SliceSpecifiers>...>)),
        mdspan_detail::inverse_map_rank<mdspan_detail::canonical_slice_t<IndexType, SliceSpecifiers>...>>{};
    // parasoft-end-suppress AUTOSAR-M0_1_3-a
    // parasoft-end-suppress AUTOSAR-M0_1_3-b

    // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: function does have a return"
    // parasoft-begin-suppress CERT_C-MSC37-a "False positive: function does have a return"
    // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: function does have a return"
    return submdspan_subextents_type_t<extents<IndexType, Extents...>, SliceSpecifiers...>{
        tuple_detail::tuple_transform_to_array<IndexType>(
            filter(mdspan_detail::to_array(src), lifted_inverse_mapping),
            filter(submdspan_canonicalize_slices(src, std::move(slices)...), lifted_inverse_mapping),
            extent_from_slice{}
        )
    };
    // parasoft-end-suppress AUTOSAR-A8_4_2-a
    // parasoft-end-suppress CERT_C-MSC37-a
    // parasoft-end-suppress CERT_CPP-MSC52-a
  }
  // parasoft-end-suppress AUTOSAR-A0_1_4-a
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
