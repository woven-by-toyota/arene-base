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
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/priority_tag.hpp"

namespace arene {
namespace base {
namespace submdspan_detail {

/// @brief the layout mapping produced by @c submdspan for a source mapping, extents, and slice specifiers
/// @tparam Mapping the source mdspan's layout mapping type
/// @tparam Extents the source mdspan's extents type
/// @tparam SliceSpecifiers pack of slice specifier types, one per rank
template <class Mapping, class Extents, class... SliceSpecifiers>
using sub_mapping_t =
    decltype(apply(
                 bind_front(mdspan_detail::do_submdspan_mapping, std::declval<Mapping const&>()),
                 submdspan_canonicalize_slices(std::declval<Extents const&>(), std::declval<SliceSpecifiers>()...)
    )
                 .mapping);

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
/// @brief unimplemented function whose return type is the return type of <c>submdspan</c>
/// @tparam ElementType element type of the source mdspan
/// @tparam Extents extents type of the source mdspan
/// @tparam LayoutPolicy layout policy of the source mdspan
/// @tparam AccessorPolicy accessor policy of the source mdspan
/// @tparam SliceSpecifiers pack of slice specifier types, one per rank
/// @return The type that should be returned by @c submdspan when called on these arguments
template <class ElementType, class Extents, class LayoutPolicy, class AccessorPolicy, class... SliceSpecifiers>
constexpr auto sub_mdspan_impl(priority_tag<1>) -> mdspan<
    ElementType,
    typename sub_mapping_t<typename LayoutPolicy::template mapping<Extents>, Extents, SliceSpecifiers...>::extents_type,
    typename sub_mapping_t<typename LayoutPolicy::template mapping<Extents>, Extents, SliceSpecifiers...>::layout_type,
    typename AccessorPolicy::offset_policy>;

/// @brief unimplemented function returning <c>void</c>, chosen when the above overload fails to compile
/// @note Specifying this as a fallback type prevents @c submdspan from having a substitution failure (it will instead
/// fail to compile when instantiating the template body), which produces more useful errors for users.
template <class...>
constexpr auto sub_mdspan_impl(priority_tag<0>) -> void;
// parasoft-end-suppress CERT_C-EXP37-a

/// @brief the type of the mdspan returned by <c>submdspan</c>, implemented to avoid return-type SFINAE
/// @tparam ElementType element type of the source mdspan
/// @tparam Extents extents type of the source mdspan
/// @tparam LayoutPolicy layout policy of the source mdspan
/// @tparam AccessorPolicy accessor policy of the source mdspan
/// @tparam SliceSpecifiers pack of slice specifier types, one per rank
template <class ElementType, class Extents, class LayoutPolicy, class AccessorPolicy, class... SliceSpecifiers>
using sub_mdspan_t =
    decltype(sub_mdspan_impl<ElementType, Extents, LayoutPolicy, AccessorPolicy, SliceSpecifiers...>(priority_tag<1>{})
    );

/// @brief Creates a view of a subset of an existing @c mdspan.
class submdspan_fn {
 public:
  /// @brief create a submdspan from a source mdspan and slice specifiers
  /// @tparam ElementType element type of the source mdspan
  /// @tparam Extents extents type of the source mdspan
  /// @tparam LayoutPolicy layout policy of the source mdspan
  /// @tparam AccessorPolicy accessor policy of the source mdspan
  /// @tparam SliceSpecifiers pack of slice specifier types, one per rank
  /// @param src the source mdspan to slice
  /// @param slices one slice specifier per rank of @c src
  /// @return a new mdspan viewing the subset of @c src described by @c slices
  ///
  /// @c submdspan returns an @c mdspan that is a subset of the source @c mdspan. That is, every element of the returned
  /// @c mdspan is an element of the source @c mdspan.
  ///
  /// A slice specifier for each rank controls which elements are included:
  /// * a single integral value or @c integral-constant-like reduces the rank by one, selecting a single index
  /// * a pair-like or @c extent_slice or @c range_slice selects a contiguous range of indices
  /// * @c full_extent_t selects the entire rank
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
          std::enable_if_t<is_sliceable_mapping_v<typename LayoutPolicy::template mapping<Extents>>>> = nullptr>
  constexpr auto
  operator()(mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> const& src, SliceSpecifiers... slices) const
      noexcept(is_nothrow_invocable_v<decltype(submdspan_canonicalize_slices), Extents const&, SliceSpecifiers&&...>)
          -> sub_mdspan_t<ElementType, Extents, LayoutPolicy, AccessorPolicy, SliceSpecifiers...> {
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

/// @brief Create a view of an mdspan selected by a slice for each dimension.
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
