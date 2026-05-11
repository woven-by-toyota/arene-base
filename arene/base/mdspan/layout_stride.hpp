// parasoft-suppress CERT_C-EXP37-a AUTOSAR-A2_8_1-a  "False positive: there is no function named '()' here. Header
// defines layout_stride types"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_LAYOUT_STRIDE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_LAYOUT_STRIDE_HPP_

// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/all_valid_submdspan_slice_types_for.hpp"
#include "arene/base/mdspan/detail/layout_common.hpp"         // IWYU pragma: keep
#include "arene/base/mdspan/detail/strided_mapping_base.hpp"  // IWYU pragma: keep
#include "arene/base/mdspan/detail/submdspan_offset.hpp"
#include "arene/base/mdspan/detail/submdspan_sub_strides.hpp"
#include "arene/base/mdspan/is_layout_mapping.hpp"
#include "arene/base/mdspan/is_sliceable_mapping.hpp"  // IWYU pragma: keep
#include "arene/base/mdspan/submdspan_extents.hpp"
#include "arene/base/mdspan/submdspan_mapping_result.hpp"
#include "arene/base/mdspan/submdspan_subextents_type.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/utility/safe_comparisons.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace layout_detail {

/// @brief Check that all of the given strides are positive, crashing with a precondition violation if not
/// @tparam IndexType The index type used by @c strides
/// @param strides The strides to check
/// @pre All of the entries in @c strides are positive, else @c ARENE_PRECONDITION violation
template <typename IndexType>
constexpr auto precondition_all_strides_positive(span<IndexType const> strides) noexcept -> void {
  for (auto const& stride : strides) {
    ARENE_PRECONDITION(stride > IndexType{});
  }
}

/// @brief Cast strides of one type to another
/// @tparam OutputIndexType The type of the strides after casting
/// @tparam InputIndexType The type of the strides before casting, deduced from @c strides
/// @tparam Rank The rank (dimensionality) of the mapping, deduced from @c strides
/// @param strides A set of strides using some original type, to be converted to @c OutputIndexType
/// @return An array containing the input @c strides converted to @c OutputIndexType
/// @pre All of the entries in @c strides are positive, else @c ARENE_PRECONDITION violation
template <
    typename OutputIndexType,
    typename InputIndexType,
    std::size_t Rank,
    constraints<std::enable_if_t<Rank != 0UL>> = nullptr>
constexpr auto stride_cast(span<InputIndexType const, Rank> strides) noexcept -> array<OutputIndexType, Rank> {
  precondition_all_strides_positive<InputIndexType>(strides);

  array<OutputIndexType, Rank> converted{};
  for (std::size_t dim{}; dim < Rank; ++dim) {
    converted[dim] = static_cast<OutputIndexType>(strides[dim]);
  }
  return converted;
}

/// @brief Cast strides of one type to another
/// @tparam OutputIndexType The type of the strides after casting
/// @tparam InputIndexType The type of the strides before casting, deduced from @c strides
/// @tparam Rank The rank (dimensionality) of the mapping, deduced from @c strides
/// @return An array containing the input @c strides converted to @c OutputIndexType
/// @pre All of the entries in @c strides are positive, else @c ARENE_PRECONDITION violation
template <
    typename OutputIndexType,
    typename InputIndexType,
    std::size_t Rank,
    constraints<std::enable_if_t<Rank == 0UL>> = nullptr>
constexpr auto stride_cast(span<InputIndexType const, Rank>) noexcept -> array<OutputIndexType, Rank> {
  return {};
}

/// @brief Type trait describing whether two strided mappings can be implicitly converted
/// Conversion can be implicit if the extents are implicitly convertible and @c FromMapping is a known strided type.
/// @tparam FromMapping The strided mapping from which the conversion is to be done
/// @tparam ToMapping The strided mapping to which the conversion is to be done
template <typename FromMapping, typename ToMapping, typename = constraints<>>
class implicit_conversion_ok : public std::false_type {};

/// @brief Type trait describing whether two strided mappings can be implicitly converted
/// Conversion can be implicit if the extents are implicitly convertible and @c FromMapping is a known strided type.
/// @tparam FromMapping The strided mapping from which the conversion is to be done
/// @tparam ToMapping The strided mapping to which the conversion is to be done
template <typename FromMapping, typename ToMapping>
class implicit_conversion_ok<
    FromMapping,
    ToMapping,
    constraints<typename FromMapping::extents_type, typename ToMapping::extents_type>>
    : public std::integral_constant<
          bool,
          std::is_convertible<typename FromMapping::extents_type, typename ToMapping::extents_type>::value &&
              (std::is_same<layout_left::mapping<typename FromMapping::extents_type>, FromMapping>::value ||
               std::is_same<layout_right::mapping<typename FromMapping::extents_type>, FromMapping>::value ||
               std::is_same<layout_stride::mapping<typename FromMapping::extents_type>, FromMapping>::value)> {};

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Extract the strides from a mapping into an array using the mapping's public @c stride(dim) function
/// @tparam The type of the mapping to extract from, deduced from @c mapping
/// @param mapping The mapping to extract from
/// @return An array containing the strides of @c mapping
template <typename Mapping, constraints<std::enable_if_t<Mapping::extents_type::rank() != 0U>> = nullptr>
constexpr auto extract_strides(Mapping const& mapping) noexcept
    -> array<typename Mapping::index_type, Mapping::extents_type::rank()> {
  array<typename Mapping::index_type, Mapping::extents_type::rank()> strides{};
  for (typename Mapping::rank_type dim{}; dim < Mapping::extents_type::rank(); ++dim) {
    strides[dim] = mapping.stride(dim);
  }

  return strides;
}

/// @brief Extract the strides from a mapping into an array using the mapping's public @c stride(dim) function
/// @tparam The type of the mapping to extract from, deduced from @c mapping
/// @return An array containing the strides of @c mapping
/// @note This SFINAE overload is needed for GCC8 which thinks that 0-iteration for loops are not constexpr.
template <typename Mapping, constraints<std::enable_if_t<Mapping::extents_type::rank() == 0U>> = nullptr>
constexpr auto extract_strides(Mapping const&) noexcept -> array<typename Mapping::index_type, 0U> {
  return {};
}

/// @brief Get the affine offset of the given mapping by checking the image of <c>(0, 0, ...)</c>
/// @note Implements the exposition-only helper @c OFFSET(m) in [mdspan.layout.stride.expo].
/// @tparam Mapping The type of the mapping to check
/// @tparam Dims A pack of indices, one for each dimension of @c Mapping::extents_type
/// @param mapping The mapping to check
/// @return The constant offset of @c mapping (0 for all built-in layouts)
template <typename Mapping, typename Mapping::rank_type... Dims>
constexpr auto
get_offset_of_mapping_impl(Mapping const& mapping, std::integer_sequence<typename Mapping::rank_type, Dims...>) noexcept
    -> typename Mapping::index_type {
  // If the output index space's size is 0, then the mapping's offset is 0 regardless of what the strides are set to.
  if (mapping.required_span_size() == typename Mapping::index_type{}) {
    return {};
  }

  // parasoft-begin-suppress AUTOSAR-M5_18_1-a "Comma operator avoids an expensive recursive iteration"
  return mapping((static_cast<void>(Dims), typename Mapping::index_type{})...);
  // parasoft-end-suppress AUTOSAR-M5_18_1-a
}

/// @brief Get the affine offset of the given mapping by checking the image of <c>(0, 0, ...)</c>
/// @note Implements the exposition-only helper @c OFFSET(m) in [mdspan.layout.stride.expo].
/// @tparam Mapping The type of the mapping to check
/// @param mapping The mapping to check
/// @return The constant offset of @c mapping (0 for all built-in layouts)
template <typename Mapping, constraints<std::enable_if_t<Mapping::extents_type::rank() != 0U>> = nullptr>
constexpr auto get_offset_of_mapping(Mapping const& mapping) noexcept -> typename Mapping::index_type {
  return get_offset_of_mapping_impl(
      mapping,
      std::make_integer_sequence<typename Mapping::rank_type, Mapping::extents_type::rank()>{}
  );
}

/// @brief Get the affine offset of the given mapping by checking the image of <c>(0, 0, ...)</c>
/// @note Implements the exposition-only helper @c OFFSET(m) in [mdspan.layout.stride.expo].
/// @tparam Mapping The type of the mapping to check
/// @return The constant offset of @c mapping (0 for all built-in layouts)
template <typename Mapping, constraints<std::enable_if_t<Mapping::extents_type::rank() == 0U>> = nullptr>
constexpr auto get_offset_of_mapping(Mapping const&) noexcept -> typename Mapping::index_type {
  return {};
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace layout_detail

// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: constructor preconditions differ so they can not delegate"
// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: class is copyable without a user-defined copy constructor"
/// @brief A mapping from a logical pack of indices into a single flat physical output index, using a strided algorithm
/// @tparam The extents of the space used for this mapping; must be a specialization of @c extents
template <class Extents>
class layout_stride::mapping : private layout_detail::strided_mapping_base<Extents> {
 private:
  /// @brief Convenience typedef referring to the base strided mapping
  using base_type = layout_detail::strided_mapping_base<Extents>;

 public:
  using typename base_type::extents_type;
  using typename base_type::index_type;
  using typename base_type::rank_type;
  using typename base_type::size_type;
  /// @brief The tag type of the layout used by this @c mapping
  using layout_type = layout_stride;

 private:
  /// @brief obtain a check_bypasser passkey to skip precondition checks on
  ///   mapping construction
  /// @return check_bypasser value
  ///
  /// @note This exists as a workaround for a GCC bug where access checking for
  ///   private constructors of friend classes can be incorrectly performed in
  ///   the caller's instantiation context rather than the definition context.
  static constexpr auto passkey() noexcept -> layout_detail::check_bypasser { return layout_detail::check_bypasser{}; }

 public:
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: Rule does not require all parameters to be named"
  /// @brief Construct a mapping with the given strides, bypassing precondition checks
  ///
  /// @note This can only be called from classes which inherit from
  /// layout_stride::mapping. It is used when slicing a layout_stride::mapping
  /// if the slice arguments are valid, so that the preconditions on the public
  /// constructors need not be re-checked.
  ///
  /// @param exts A set of extents to use for this mapping
  /// @param strds A set of strides to use for this mapping
  constexpr mapping(
      extents_type const& exts,
      array<index_type, extents_type::rank()> const& strds,
      layout_detail::check_bypasser
  ) noexcept
      : base_type(exts, strds) {}
  // parasoft-end-suppress CERT_C-EXP37-a

  /// @brief Construct a default strided mapping for the given extents, which has the same layout as @c layout_right
  constexpr mapping() noexcept
      : base_type(extents_type{}, layout_detail::right_strides(extents_type{})) {
    // Note: the precondition specified in [mdspan.layout.stride.cons] can not be violated so it's never checked.
  }

  /// @brief Construct a mapping with the given strides; they must be valid for a default-constructed @c extents_type
  /// @tparam OtherIndexType The original index type used by <c>strds</c>; must be convertible to @c index_type
  /// @param exts A set of extents to use for this mapping
  /// @param strds A set of strides to use for this mapping
  /// @pre All strides are positive when converted to <c>index_type</c>
  /// @pre Size of the output index space is representable as <c>index_type</c>
  /// @pre @c strds defines a unique (one-to-one) mapping from <c>exts</c>
  template <
      class OtherIndexType,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexType const&, index_type>::value>,
          std::enable_if_t<std::is_nothrow_constructible<index_type, OtherIndexType const&>::value>> = nullptr>
  constexpr mapping(extents_type const& exts, span<OtherIndexType, extents_type::rank()> strds) noexcept
      : base_type(exts, layout_detail::stride_cast<index_type>(strds)) {
    // The representable precondition is checked here as a side-effect of 'required_span_size'.
    index_type const span_size{this->required_span_size()};

    // This is a necessary, but not sufficient check for uniqueness.
    auto const extent_size = layout_detail::extent_product(exts);
    ARENE_PRECONDITION(!extent_size.overflowed);
    ARENE_PRECONDITION(span_size >= extent_size.value);
  }

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Construct a mapping with the given strides; they must be valid for a default-constructed @c extents_type
  /// @tparam OtherIndexType The original index type used by <c>strds</c>; must be convertible to @c index_type
  /// @param exts A set of extents to use for this mapping
  /// @param strds A set of strides to use for this mapping
  /// @pre All strides are positive when converted to <c>index_type</c>, else @c ARENE_PRECONDITION violation
  /// @pre Size of the output index space is representable as <c>index_type</c>, else @c ARENE_PRECONDITION violation
  /// @pre @c strds defines a unique (one-to-one) mapping from <c>exts</c>, else @c ARENE_PRECONDITION violation
  template <
      class OtherIndexType,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexType const&, index_type>::value>,
          std::enable_if_t<std::is_nothrow_constructible<index_type, OtherIndexType const&>::value>> = nullptr>
  constexpr mapping(extents_type const& exts, array<OtherIndexType, extents_type::rank()> const& strds) noexcept
      : mapping(exts, span<OtherIndexType const, extents_type::rank()>(strds)) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @brief Construct a mapping from another strided mapping; implicit for certain known-safe mapping types
  /// @tparam StridedLayoutMapping A layout mapping satisfying [mdspan.layout.reqmts] which is always strided and unique
  /// @param other A mapping to convert
  /// @pre All strides are positive when converted to <c>index_type</c>, else @c ARENE_PRECONDITION violation
  /// @pre Size of the output index space is representable as <c>index_type</c>, else @c ARENE_PRECONDITION violation
  /// @pre <c>other(0, 0, 0...) == 0</c>, else @c ARENE_PRECONDITION violation
  template <
      class StridedLayoutMapping,
      constraints<
          std::enable_if_t<is_layout_mapping_v<StridedLayoutMapping>>,
          std::enable_if_t<std::is_constructible<extents_type, typename StridedLayoutMapping::extents_type>::value>,
          std::enable_if_t<StridedLayoutMapping::is_always_strided()>,
          std::enable_if_t<StridedLayoutMapping::is_always_unique()>,
          std::enable_if_t<layout_detail::implicit_conversion_ok<StridedLayoutMapping, mapping>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) C++23 defines this as conditionally implicit
  constexpr mapping(StridedLayoutMapping const& other) noexcept
      : base_type(
            extents_type(other.extents()),
            layout_detail::stride_cast<index_type>(
                span<typename StridedLayoutMapping::index_type const, StridedLayoutMapping::extents_type::rank()>(
                    layout_detail::extract_strides(other)
                )
            )
        ) {
    // None of the preconditions can be violated for the implicit version of this function. StridedLayoutMapping must be
    // a known type and all standard layouts have zero offset, positive strides, and in-bounds required_span_size.
    // The extents_type must also be implicitly convertible, so casting required_span_size can not go OOB either.
  }

  /// @brief Construct a mapping from another strided mapping; implicit for certain known-safe mapping types
  /// @tparam StridedLayoutMapping A layout mapping satisfying [mdspan.layout.reqmts] which is always strided and unique
  /// @param other A mapping to convert
  /// @pre All strides are positive when converted to <c>index_type</c>, else @c ARENE_PRECONDITION violation
  /// @pre Size of the output index space is representable as <c>index_type</c>, else @c ARENE_PRECONDITION violation
  /// @pre <c>other(0, 0, 0...) == 0</c>, else @c ARENE_PRECONDITION violation
  template <
      class StridedLayoutMapping,
      constraints<
          std::enable_if_t<is_layout_mapping_v<StridedLayoutMapping>>,
          std::enable_if_t<std::is_constructible<extents_type, typename StridedLayoutMapping::extents_type>::value>,
          std::enable_if_t<StridedLayoutMapping::is_always_strided()>,
          std::enable_if_t<StridedLayoutMapping::is_always_unique()>,
          std::enable_if_t<!layout_detail::implicit_conversion_ok<StridedLayoutMapping, mapping>::value>> = nullptr>
  constexpr explicit mapping(StridedLayoutMapping const& other) noexcept
      : base_type(
            extents_type(other.extents()),
            layout_detail::stride_cast<index_type>(
                span<typename StridedLayoutMapping::index_type const, StridedLayoutMapping::extents_type::rank()>(
                    layout_detail::extract_strides(other)
                )
            )
        ) {
    // The second precondition is enforced by layout_detail::stride_cast
    ARENE_PRECONDITION(arene::base::cmp_less_equal(other.required_span_size(), std::numeric_limits<index_type>::max()));
    ARENE_PRECONDITION(layout_detail::get_offset_of_mapping(other) == typename StridedLayoutMapping::index_type{});
  }

  using base_type::extents;
  using base_type::required_span_size;
  using base_type::stride;
  using base_type::strides;
  using base_type::operator();

  using base_type::is_always_strided;
  using base_type::is_always_unique;
  using base_type::is_strided;
  using base_type::is_unique;

  /// @brief Return whether or not this mapping is always exhaustive, i.e. every element is reachable using some indices
  /// @return @c false because it's possible to construct non-exhaustive mappings using this class
  /// @note In mathematical terms this asks if the mapping is always surjective/onto.
  static constexpr auto is_always_exhaustive() noexcept -> bool { return false; }

  /// @brief Return whether or not this instance is exhaustive, i.e. every element is reachable using some indices
  /// @return @c true if every provided stride is the product of the extents corresponding to some subset of the other
  /// strides, otherwise @c false
  /// @note In mathematical terms this asks if the mapping is surjective/onto.
  constexpr auto is_exhaustive() const noexcept -> bool {
    return required_span_size() == layout_detail::extent_product(this->extents()).value;
  }

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  /// @brief Compare two strided mappings for equality, of which at least one must be a @c layout_stride::mapping
  /// @tparam Right The type of the right side of the comparison; must be strided and satisfy @c is_layout_mapping_v
  /// @param left An instance of @c layout_stride::mapping
  /// @param right An instance of @c Right
  /// @return @c true if @c left and @c right have the same extents and strides, and both map (0,0,...) to @c 0
  template <
      class Right,
      constraints<
          std::enable_if_t<is_layout_mapping_v<Right>>,
          std::enable_if_t<Right::extents_type::rank() == extents_type::rank()>,
          std::enable_if_t<Right::is_always_strided()>> = nullptr>
  friend constexpr auto operator==(mapping const& left, Right const& right) noexcept -> bool {
    for (mapping::rank_type dim{}; dim < mapping::extents_type::rank(); ++dim) {
      if (cmp_not_equal(left.extents().extent(dim), right.extents().extent(dim)) ||
          cmp_not_equal(left.stride(dim), right.stride(dim))) {
        return false;
      }
    }

    return layout_detail::get_offset_of_mapping(right) == typename Right::index_type{};
  }

  /// @brief Compare two strided mappings for equality, of which at least one must be a @c layout_stride::mapping
  /// @tparam Left The type of the left side of the comparison; must be strided and satisfy @c is_layout_mapping_v
  /// @param left An instance of @c Left
  /// @param right An instance of @c layout_stride::mapping
  /// @return @c true if @c left and @c right have the same extents and strides, and both map (0,0,...) to @c 0
  template <
      class Left,
      constraints<
          std::enable_if_t<!std::is_same<typename Left::layout_type, layout_stride>::value>,
          std::enable_if_t<is_layout_mapping_v<Left>>,
          std::enable_if_t<Left::extents_type::rank() == extents_type::rank()>,
          std::enable_if_t<Left::is_always_strided()>> = nullptr>
  friend constexpr auto operator==(Left const& left, mapping const& right) noexcept -> bool {
    return right == left;
  }

  /// @brief Compare two strided mappings for inequality, of which at least one must be a @c layout_stride::mapping
  /// @tparam Right The type of the right side of the comparison; must be strided and satisfy @c is_layout_mapping_v
  /// @param left An instance of @c layout_stride::mapping
  /// @param right An instance of @c Right
  /// @return @c true if @c left and @c right have different extents or strides, or one doesn't map (0,0,...) to @c 0
  template <
      class Right,
      constraints<
          std::enable_if_t<is_layout_mapping_v<Right>>,
          std::enable_if_t<Right::extents_type::rank() == extents_type::rank()>,
          std::enable_if_t<Right::is_always_strided()>> = nullptr>
  friend constexpr auto operator!=(mapping const& left, Right const& right) noexcept -> bool {
    return !(left == right);
  }

  /// @brief Compare two strided mappings for inequality, of which at least one must be a @c layout_stride::mapping
  /// @tparam Left The type of the left side of the comparison; must be strided and satisfy @c is_layout_mapping_v
  /// @param left An instance of @c Left
  /// @param right An instance of @c layout_stride::mapping
  /// @return @c true if @c left and @c right have different extents or strides, or one doesn't map (0,0,...) to @c 0
  template <
      class Left,
      constraints<
          std::enable_if_t<!std::is_same<typename Left::layout_type, layout_stride>::value>,
          std::enable_if_t<is_layout_mapping_v<Left>>,
          std::enable_if_t<Left::extents_type::rank() == extents_type::rank()>,
          std::enable_if_t<Left::is_always_strided()>> = nullptr>
  friend constexpr auto operator!=(Left const& left, mapping const& right) noexcept -> bool {
    return !(right == left);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief compute submdspan mapping and offset for a @c layout_stride mapping
  /// @tparam CanonicalSliceSpecifiers submdspan slice specifier types
  /// @param src source @c layout_stride::mapping to slice
  /// @param slices submdspan slice specifiers
  ///
  /// Uses @c slices to compute the sub-extents, sub-strides, and offset of the submdspan. The @c slices must be in
  /// canonical form.
  ///
  /// @return A @c submdspan_mapping_result containing the sub-mapping and offset such that indexing the sub-mapping and
  /// adding the offset recovers the corresponding index in the original mapping.
  ///
  /// @note Constraints <br>
  ///   * <c> sizeof...(CanonicalSliceSpecifiers) == extents_type::rank() </c>
  ///   * for each rank index @c k of @c src.extents(), @c CanonicalSliceSpecifiers...[k] is a valid submdspan slice
  ///   type for the @c k-th extent of @c extents_type
  ///
  /// @note Preconditions <br>
  ///   * for each rank index @c k of @c src.extents(), @c slices...[k] denotes a valid submdspan slice for the @c k-th
  ///   extent of @c src.extents()
  ///
  /// @note Remarks <br>
  ///   * this function is found via argument-dependent lookup as a @c friend of @c layout_stride::mapping
  ///   * the @c layout_stride specialization of @c submdspan_mapping always produces a @c layout_stride mapping for the
  ///   submdspan.
  template <
      class SrcMapping,
      class... CanonicalSliceSpecifiers,
      constraints<
          std::enable_if_t<sizeof...(CanonicalSliceSpecifiers) == extents_type::rank()>,
          std::enable_if_t<
              mdspan_detail::all_valid_submdspan_slice_types_for_v<extents_type, CanonicalSliceSpecifiers...>>> =
          nullptr>
  friend constexpr auto submdspan_mapping(SrcMapping const& src, CanonicalSliceSpecifiers... slices) noexcept
      -> submdspan_mapping_result<
          layout_stride::mapping<submdspan_subextents_type_t<extents_type, CanonicalSliceSpecifiers...>>> {
    // precondition is transitively checked by submdspan_extents -> submdspan_canonicalize_slices
    auto sub_strides = mdspan_detail::submdspan_sub_strides<index_type>(src, slices...);
    auto offset = mdspan_detail::submdspan_offset(src, slices...);
    auto sub_ext = submdspan_extents(src.extents(), std::move(slices)...);

    using submapping = layout_stride::mapping<submdspan_subextents_type_t<extents_type, CanonicalSliceSpecifiers...>>;
    return {submapping{std::move(sub_ext), std::move(sub_strides), mapping::passkey()}, offset};
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
};
// parasoft-end-suppress AUTOSAR-A14_5_1-a
// parasoft-end-suppress AUTOSAR-A12_1_5-a

/// @brief Specialization of @c is_layout_mapping_v to break a recursive cycle in the @c layout_stride equality operator
/// @tparam Extents Any specialization of @c arene::base::extents
template <typename Extents>
extern constexpr bool is_layout_mapping_v<layout_stride::mapping<Extents>> = true;

/// @brief Specialization of @c is_sliceable_mapping_v indicating @c layout_stride::mapping supports @c submdspan
/// @tparam Extents Any specialization of @c arene::base::extents
template <typename Extents>
extern constexpr bool submdspan_detail::is_sliceable_mapping_v<layout_stride::mapping<Extents>> = true;

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_LAYOUT_STRIDE_HPP_
