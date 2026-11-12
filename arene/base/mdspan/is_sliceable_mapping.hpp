// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_SLICEABLE_MAPPING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_SLICEABLE_MAPPING_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/mdspan/detail/count.hpp"
#include "arene/base/mdspan/detail/do_submdspan_mapping.hpp"
#include "arene/base/mdspan/detail/non_collapsing_slice_type.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/is_layout_mapping.hpp"
#include "arene/base/mdspan/submdspan_mapping_result.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_manipulation/repeat_type.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace arene {
namespace base {
namespace is_sliceable_mapping_detail {

/// @brief Invalid slice type
struct invalid_slice {};

/// @brief Primary template (false by default) to check if the provided submdspan_mapping_result specialization
/// satisfies the sliceable layout mapping requirements.
/// @tparam SubmdspanMappingResult A specialization of submdspan_mapping_result
/// @tparam ExpectedIndexType The expected layout mapping extents index type
/// @tparam ExpectedRank The expected rank of the result mapping (i.e. the number of non-collapsing slices)
template <typename SubmdspanMappingResult, class ExpectedIndexType, std::size_t ExpectedRank>
extern constexpr bool is_valid_submdspan_mapping_result = false;

/// @brief Array of @c bool with @c true values at non-collapsing slices
/// @tparam Slices Parameter pack of slices
template <class... Slices>
constexpr auto non_collapsing_slice_mask =
    arene::base::array<bool, sizeof...(Slices)>{arene::base::mdspan_detail::non_collapsing_slice_type_v<Slices>...};

/// @brief Number of non-collapsing slices in a @c type_list of slice types
/// @tparam SlicesTypeList A specialization of @c arene::base::type_list holding the slice types
template <class SlicesTypeList>
extern constexpr std::size_t non_collapsing_slice_count = 0U;

/// @brief Number of non-collapsing slices in a @c type_list of slice types
/// @tparam Slices The pack of slice types held by the @c type_list
template <class... Slices>
extern constexpr std::size_t non_collapsing_slice_count<arene::base::type_list<Slices...>> = static_cast<std::size_t>(
    mdspan_detail::count(non_collapsing_slice_mask<Slices...>.begin(), non_collapsing_slice_mask<Slices...>.end(), true)
);

/// @brief Specialization template to check if the provided submdspan_mapping_result specialization satisfies the
/// sliceable layout mapping requirements.
/// @tparam LayoutMapping The type used to specialize @c submdspan_mapping_result
/// @tparam ExpectedIndexType The expected layout mapping extents index type
/// @tparam ExpectedRank The expected rank of the result mapping (i.e. the number of non-collapsing slices)
///
/// @see https://eel.is/c++draft/views#mdspan.sub.map.sliceable-4
template <typename LayoutMapping, class ExpectedIndexType, std::size_t ExpectedRank>
extern constexpr bool is_valid_submdspan_mapping_result<
    submdspan_mapping_result<LayoutMapping>,
    ExpectedIndexType,
    ExpectedRank> = is_layout_mapping_v<LayoutMapping> &&                     //
                    is_extents_v<typename LayoutMapping::extents_type> &&     //
                    (LayoutMapping::extents_type::rank() == ExpectedRank) &&  //
                    (std::is_same<typename LayoutMapping::extents_type::index_type, ExpectedIndexType>::value);

/// @brief Template to check if a type meets the sliceable layout mapping requirements
/// @tparam M The class to check
///
/// Primary template is false by default.
template <class M, class = constraints<>>
extern constexpr bool is_sliceable_mapping_v = false;

/// @brief Template to check if a type meets the sliceable layout mapping requirements
/// @tparam M The class to check
///
/// Specialization template is SFINAE constrained and conditionalized upon @c is_valid_submdspan_mapping_result.
///
/// @see https://eel.is/c++draft/views#mdspan.sub.map.sliceable-3
template <class M>
extern constexpr bool is_sliceable_mapping_v<
    M,
    constraints<
        std::enable_if_t<is_layout_mapping_v<M>>,
        std::enable_if_t<
            (M::extents_type::rank() == 0) ==
            type_lists::concat_t<
                is_invocable<decltype(mdspan_detail::do_submdspan_mapping), M>,
                repeat_type_t<M::extents_type::rank(), type_list, invalid_slice>>::value>,
        std::enable_if_t<type_lists::concat_t<
            is_invocable<decltype(mdspan_detail::do_submdspan_mapping), M>,
            repeat_type_t<M::extents_type::rank(), type_list, full_extent_t>>::value>,
        std::enable_if_t<type_lists::concat_t<
            is_invocable<decltype(mdspan_detail::do_submdspan_mapping), M>,
            repeat_type_t<M::extents_type::rank(), type_list, typename M::extents_type::index_type>>::value>>> =
    is_valid_submdspan_mapping_result<
        typename type_lists::concat_t<
            invoke_result<decltype(mdspan_detail::do_submdspan_mapping), M>,
            repeat_type_t<M::extents_type::rank(), type_list, full_extent_t>>::type,
        typename M::extents_type::index_type,
        non_collapsing_slice_count<repeat_type_t<M::extents_type::rank(), type_list, full_extent_t>>> &&
    is_valid_submdspan_mapping_result<
        typename type_lists::concat_t<
            invoke_result<decltype(mdspan_detail::do_submdspan_mapping), M>,
            repeat_type_t<M::extents_type::rank(), type_list, typename M::extents_type::index_type>>::type,
        typename M::extents_type::index_type,
        non_collapsing_slice_count<
            repeat_type_t<M::extents_type::rank(), type_list, typename M::extents_type::index_type>>>;

}  // namespace is_sliceable_mapping_detail

// parasoft-begin-suppress AUTOSAR-A2_7_2-a-2 "False positive: no commented-out code"
/// @brief Check if a type meets the sliceable layout mapping requirements
/// @tparam M The class to check
/// @tparam Enable SFINAE hook used by specializations that select on a property of @c M (e.g.
/// @c layout_transpose::mapping, whose nested layout is not deducible via an exact-type partial specialization)
///
/// Let:
/// - @c M denote a layout mapping class;
/// - @c IT denote @c M::extents_type::index_type;
/// - @c m denote a value of type (possibly @c const) @c M;
/// - @c M_rank be equal to @c M::extents_type::rank();
/// - @c valid_slices denote a pack of (possibly @c const) objects for which <c> sizeof...(valid_slices) == M_rank </c>
///   is @c true and, for each rank index @c i of @c m.extents(), @c valid_slices...[i] is a valid submdspan slice for
///   the @c i <sup>th</sup> extent of @c m.extents();
/// - @c invalid_slices denote a pack of objects for which <c> sizeof...(invalid_slices) == M_rank </c> is @c true and
///   there exists an integer @c k such that the cv-unqualified type of @c invalid_slices...[k] is none of the
///   following:
///   - @c IT,
///   - @c full_extent_t,
///   - a specialization of @c constant_wrapper, or
///   - a specialization of @c extent_slice.
///
/// A type @c M meets the sliceable layout mapping requirements if
/// - @c M meets the layout mapping requirements
/// - the expression <c> submdspan_mapping(m, invalid_slices...) </c> is ill-formed, and
/// - the following expression is well-formed and has the specified semantics:
///   <c> submdspan_mapping(m, valid_slices...) </c>
///   *Result*: A type @c SMR that is a specialization of type @c submdspan_mapping_result<SM> for some type @c SM such
///   that
///   - @c SM meets the layout mapping requirements,
///   - <c> SM::extents_type </c> is a specialization of extents,
///   - <c> SM::extents_type::rank() </c> equals <c> MAP_RANK(valid_slices, M_rank) </c>, and
///   - <c> SM::extents_type::index_type </c> denotes @c IT.
///   *Returns*: An object @c smr of type @c SMR such that
///   - <c> smr.mapping.extents() == subextents(m.extents(), valid_slices...) </c> is @c true; and
///   - for each integer pack @c i which is a multidimensional index in <c> smr.mapping.extents() </c>, <c>
///     smr.mapping(i...) + smr.offset == m(j) </c> is @c true, where @c j is an integer pack such that
///     - @c sizeof...(j) is equal to @c M_rank; and
///     - for each rank index @c p of @c m.extents(), @c j...[p] is equal to the sum of
///       - the lower bound of the submdspan slice range of <c> valid_slices...[p] </c> for extent @c p of <c>
///         m.extents() </c>, and
///       - zero if the type of <c> valid_slices...[p] </c> is a collapsing slice type, <c>
///         i...[MAP_RANK(valid_slices, p)] </c> otherwise.
template <class M, class Enable = void>
extern constexpr bool is_sliceable_mapping_v = is_sliceable_mapping_detail::is_sliceable_mapping_v<M>;
// parasoft-begin-suppress AUTOSAR-A2_7_2-a-2 "False positive: no commented-out code"

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_SLICEABLE_MAPPING_HPP_
