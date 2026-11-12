// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_LAYOUT_TRANSPOSE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_LAYOUT_TRANSPOSE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/do_submdspan_mapping.hpp"
#include "arene/base/mdspan/detail/reverse_extents.hpp"
#include "arene/base/mdspan/is_sliceable_mapping.hpp"  // IWYU pragma: keep
#include "arene/base/mdspan/submdspan_mapping_result.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace transpose_detail {

/// @brief Verify the strided precondition of @c layout_transpose::mapping::stride for an always-strided mapping
/// @tparam NestedMapping The wrapped mapping type; selected when it is always strided
///
/// An always-strided mapping satisfies the precondition statically, so there is nothing to check at runtime
template <class NestedMapping, constraints<std::enable_if_t<NestedMapping::is_always_strided()>> = nullptr>
constexpr auto check_nested_mapping_is_strided(NestedMapping const&) noexcept -> void {}

/// @brief Verify the strided precondition of @c layout_transpose::mapping::stride for a sometimes-strided mapping
/// @tparam NestedMapping The wrapped mapping type; selected when it is not always strided
/// @param nested The wrapped mapping whose @c is_strided() is checked
/// @pre @c nested.is_strided() is true, else @c ARENE_PRECONDITION violation
template <class NestedMapping, constraints<std::enable_if_t<!NestedMapping::is_always_strided()>> = nullptr>
constexpr auto check_nested_mapping_is_strided(NestedMapping const& nested) noexcept -> void {
  ARENE_PRECONDITION(nested.is_strided());
}

}  // namespace transpose_detail

/// @brief A layout mapping policy that swaps the two indices, extents, and strides of a rank-2 mapping
/// @tparam NestedLayout The layout mapping policy being transposed; its rank-2 @c mapping is wrapped by this policy
///
/// Given a @c NestedLayout mapping over extents <c>(R, C)</c>, the @c layout_transpose::mapping presents extents
/// <c>(C, R)</c> and maps index <c>(i, j)</c> to the nested mapping's <c>(j, i)</c>.
template <class NestedLayout>
class layout_transpose {
 public:
  /// @brief The wrapped (un-transposed) layout type
  using nested_layout_type = NestedLayout;

  /// @brief The mapping type produced by @c layout_transpose for a given set of extents
  /// @tparam The extents of the space used for this mapping
  ///
  /// @note Mandates <br>
  /// * @c Extents is a specialization of @c arene::base::extents
  /// * @c Extents::rank() equals 2
  template <class Extents>
  // parasoft-begin-suppress AUTOSAR-A2_10_1 "False positive: There is no identifier 'mapping' being hidden"
  class mapping : private NestedLayout::template mapping<mdspan_detail::reverse_extents_t<Extents>> {
    // parasoft-end-suppress AUTOSAR-A2_10_1
   public:
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(Extents::rank() == 2U, "Extents must be rank 2.");
    // parasoft-end-suppress CERT_C-PRE31-c-3

    /// @brief This mapping's extents type
    using extents_type = Extents;
    /// @brief The size type of this mapping
    using size_type = typename extents_type::size_type;
    /// @brief The index type of this mapping
    using index_type = typename extents_type::index_type;
    /// @brief The rank type of this mapping
    using rank_type = typename extents_type::rank_type;
    /// @brief The tag type of the layout used by this @c mapping
    using layout_type = layout_transpose;
    /// @brief The wrapped (un-transposed) mapping type
    using nested_mapping_type = typename NestedLayout::template mapping<mdspan_detail::reverse_extents_t<extents_type>>;

    /// @brief Construct a transposed mapping from a nested mapping
    /// @param nested The nested mapping to transpose
    constexpr explicit mapping(nested_mapping_type const& nested) noexcept
        : nested_mapping_type{nested},
          extents_{nested.extents().extent(1U), nested.extents().extent(0U)} {}

    /// @brief Obtain the (transposed) extents of this mapping
    /// @return The extents, with the two dimensions of the nested mapping swapped
    constexpr auto extents() const noexcept -> extents_type const& { return extents_; }

    /// @brief Obtain the wrapped (un-transposed) mapping
    /// @return The nested mapping before the transpose is applied
    constexpr auto nested_mapping() const noexcept -> nested_mapping_type const& {
      return static_cast<nested_mapping_type const&>(*this);
    }

    /// @brief Obtain the required span size
    /// @return @c nested_mapping().required_span_size()
    constexpr auto required_span_size() const noexcept -> index_type { return nested_mapping().required_span_size(); }

    /// @brief Access the flat element index for the given two-dimensional index
    /// @tparam Index0 The type of the first index
    /// @tparam Index1 The type of the second index
    /// @param index0 The index into the first (transposed) dimension
    /// @param index1 The index into the second (transposed) dimension
    /// @return @c nested_mapping()(index1, index0), i.e. the underlying element with the indices reversed
    template <class Index0, class Index1>
    constexpr auto operator()(Index0 index0, Index1 index1) const noexcept -> index_type {
      return nested_mapping()(index1, index0);
    }

    /// @brief Determine if this mapping is always unique
    /// @return @c nested_mapping_type::is_always_unique()
    static constexpr auto is_always_unique() noexcept -> bool { return nested_mapping_type::is_always_unique(); }

    /// @brief Determine if this mapping is always exhaustive
    /// @return @c nested_mapping_type::is_always_exhaustive()
    static constexpr auto is_always_exhaustive() noexcept -> bool {
      return nested_mapping_type::is_always_exhaustive();
    }

    /// @brief Determine if this mapping is always strided
    /// @return @c nested_mapping_type::is_always_strided()
    static constexpr auto is_always_strided() noexcept -> bool { return nested_mapping_type::is_always_strided(); }

    /// @brief Determine if this mapping is unique
    /// @return @c nested_mapping().is_unique()
    constexpr auto is_unique() const noexcept -> bool { return nested_mapping().is_unique(); }

    /// @brief Determine if this mapping is exhaustive
    /// @return @c nested_mapping().is_exhaustive()
    constexpr auto is_exhaustive() const noexcept -> bool { return nested_mapping().is_exhaustive(); }

    /// @brief Determine if this mapping is strided
    /// @return @c nested_mapping().is_strided()
    constexpr auto is_strided() const noexcept -> bool { return nested_mapping().is_strided(); }

    /// @brief Obtain the stride along the specified (transposed) dimension
    /// @param rank_index The (transposed) dimension index; 0 maps to the nested dimension 1 and vice versa
    /// @return The stride of the nested mapping along the swapped dimension
    /// @pre @c is_strided() is true, else @c ARENE_PRECONDITION violation
    /// @pre <c>rank_index < 2</c>, else @c ARENE_PRECONDITION violation
    constexpr auto stride(rank_type rank_index) const noexcept -> index_type {
      ARENE_PRECONDITION(rank_index < rank_type{2});
      transpose_detail::check_nested_mapping_is_strided(nested_mapping());
      // rank_index is 0 or 1 (guaranteed above), so the swap to the nested dimension is rank_type{1} - rank_index.
      return nested_mapping().stride(static_cast<rank_type>(rank_type{1} - rank_index));
    }

    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
    // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
    /// @brief Compare two transposed mappings for equality
    /// @tparam OtherExtents The extents type of the right-hand side mapping
    /// @param left A transposed mapping
    /// @param right A transposed mapping with possibly different extents
    /// @return @c true if the wrapped nested mappings compare equal
    template <
        class OtherExtents,
        std::enable_if_t<
            is_equality_comparable_v<nested_mapping_type, typename mapping<OtherExtents>::nested_mapping_type> &&
                extents_type::rank() == OtherExtents::rank(),
            bool> = true>
    friend constexpr auto operator==(mapping const left, mapping<OtherExtents> const right) noexcept -> bool {
      return left.nested_mapping() == right.nested_mapping();
    }

    /// @brief Compare two transposed mappings for inequality
    /// @tparam OtherExtents The extents type of the right-hand side mapping
    /// @param left A transposed mapping
    /// @param right A transposed mapping with possibly different extents
    /// @return @c true if the wrapped nested mappings compare unequal
    template <
        class OtherExtents,
        std::enable_if_t<
            is_equality_comparable_v<nested_mapping_type, typename mapping<OtherExtents>::nested_mapping_type> &&
                extents_type::rank() == OtherExtents::rank(),
            bool> = true>
    friend constexpr auto operator!=(mapping const left, mapping<OtherExtents> const right) noexcept -> bool {
      return !(left == right);
    }
    // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
    // parasoft-end-suppress AUTOSAR-A11_3_1-a

   private:
    /// @brief The transposed mapping type wrapping a rank-2 sub-mapping @c Mapping
    /// @tparam Mapping A rank-2 sub-mapping produced by slicing the nested mapping
    template <class Mapping>
    using transposed_sub_mapping_t = typename layout_transpose<typename Mapping::layout_type>::template mapping<
        mdspan_detail::reverse_extents_t<typename Mapping::extents_type>>;

    /// @brief Re-apply the transpose to a rank-2 sub-mapping result of the nested mapping
    /// @tparam Mapping The sub-mapping type produced by slicing the nested mapping; selected when it is rank 2
    /// @param result The @c submdspan_mapping_result from slicing the nested mapping
    /// @return A @c submdspan_mapping_result whose mapping is @c result 's mapping wrapped in a @c layout_transpose,
    /// preserving the offset
    ///
    /// A rank-2 slice of the nested mapping is transposed back so that the sub-mapping matches the transposed indexing.
    template <class Mapping, constraints<std::enable_if_t<Mapping::extents_type::rank() == 2U>> = nullptr>
    static constexpr auto transpose_result(submdspan_mapping_result<Mapping> result) noexcept
        -> submdspan_mapping_result<transposed_sub_mapping_t<Mapping>> {
      return {transposed_sub_mapping_t<Mapping>{result.mapping}, result.offset};
    }

    /// @brief Pass a rank-reduced sub-mapping result through unchanged
    /// @tparam Mapping The sub-mapping type produced by slicing the nested mapping; selected when its rank is below 2
    /// @param result The @c submdspan_mapping_result from slicing the nested mapping
    /// @return @c result unchanged
    ///
    /// A slice that drops a rank produces a rank-1 (or rank-0) sub-mapping, which has no transpose to re-apply.
    template <class Mapping, constraints<std::enable_if_t<Mapping::extents_type::rank() < 2U>> = nullptr>
    static constexpr auto transpose_result(submdspan_mapping_result<Mapping> result) noexcept
        -> submdspan_mapping_result<Mapping> {
      return result;
    }

   public:
    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
    /// @brief Compute the sub-mapping and offset for a transposed mapping
    /// @tparam CanonicalSlice0 The type of the slice specifier for the first (transposed) dimension
    /// @tparam CanonicalSlice1 The type of the slice specifier for the second (transposed) dimension
    /// @tparam Mapping Defaulted to @c mapping so the trailing return type can be SFINAE-friendly
    /// @param src The source transposed mapping to slice
    /// @param slice0 The slice specifier for the first (transposed) dimension
    /// @param slice1 The slice specifier for the second (transposed) dimension
    /// @return A @c submdspan_mapping_result obtained by slicing the nested mapping with the slices reversed, then
    /// re-transposing a rank-2 result
    ///
    /// Because @c layout_transpose swaps the two dimensions, slicing dimension 0 of the transpose is equivalent to
    /// slicing dimension 1 of the nested mapping. The slices are therefore reversed and forwarded to the nested
    /// mapping's own @c submdspan_mapping via @c do_submdspan_mapping. The resulting sub-mapping is re-transposed if it
    /// is still rank 2.
    template <class CanonicalSlice0, class CanonicalSlice1, class Mapping = mapping>
    friend constexpr auto submdspan_mapping(mapping const src, CanonicalSlice0 slice0, CanonicalSlice1 slice1) noexcept(
        noexcept(transpose_result(mdspan_detail::do_submdspan_mapping(
            std::declval<Mapping const&>().nested_mapping(),
            std::move(std::declval<CanonicalSlice1>()),
            std::move(std::declval<CanonicalSlice0>())
        )))
    )
        -> decltype(transpose_result(mdspan_detail::do_submdspan_mapping(
            std::declval<Mapping const&>().nested_mapping(),
            std::move(slice1),
            std::move(slice0)
        ))) {
      return transpose_result(
          mdspan_detail::do_submdspan_mapping(src.nested_mapping(), std::move(slice1), std::move(slice0))
      );
    }
    // parasoft-end-suppress AUTOSAR-A11_3_1-a

   private:
    /// @brief The transposed extents, stored so that @c extents() can return a reference
    extents_type extents_{};
  };
  // parasoft-end-suppress AUTOSAR-A10_2_1-a AUTOSAR-A10_2_1-b
  // parasoft-end-suppress AUTOSAR-A2_10_1
};

}  // namespace linalg

/// @brief Specialization of @c is_layout_mapping_v indicating a @c layout_transpose::mapping is a layout mapping
/// @tparam LayoutMapping A layout mapping whose @c layout_type is a @c layout_transpose instantiation
///
/// This short-circuits the recursive instantiation cycle that would otherwise occur while completing the mapping type,
/// which gcc parses as always being false
template <class LayoutMapping>
extern constexpr bool is_layout_mapping_v<
    LayoutMapping,
    std::enable_if_t<is_instantiation_of_v<typename LayoutMapping::layout_type, linalg::layout_transpose>>> = true;

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_LAYOUT_TRANSPOSE_HPP_
