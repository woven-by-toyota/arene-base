// parasoft-suppress CERT_C-EXP37-a AUTOSAR-A2_8_1-a  "False positive: there is no function named '()' here. Header
// defines layout types"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_LAYOUT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_LAYOUT_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/mdspan/detail/checked_math.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/is_layout_mapping.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/utility/safe_comparisons.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

/// @brief A strided data layout, i.e. incrementing any dimension moves the output by that dimension's constant stride
struct layout_stride {
  /// @brief A mapping from a logical pack of indices into a single flat physical output index
  /// @tparam The extents of the space used for this mapping; must be a specialization of @c extents
  template <class Extents>
  class mapping;
};

/// @brief A strided data layout in which the leftmost stride is the largest and the rightmost stride is the smallest
struct layout_right {
  /// @brief A mapping from a logical pack of indices into a single flat physical output index
  /// @tparam The extents of the space used for this mapping; must be a specialization of @c extents
  template <class Extents>
  class mapping;
};

/// @brief A strided data layout in which the leftmost stride is the smallest and the rightmost stride is the largest
struct layout_left {
  /// @brief A mapping from a logical pack of indices into a single flat physical output index
  /// @tparam The extents of the space used for this mapping; must be a specialization of @c extents
  template <class Extents>
  class mapping;
};

namespace layout_detail {
/// @brief A type used to represent a range of dimension indices in a way which won't cause "easily-swapped" warnings
/// @tparam RankType The type used to represent a dimension index (@c rank_type in @c extents terminology)
template <typename RankType>
struct dim_range {
  /// @brief The first dimension to include in the operation
  RankType begin;
  /// @brief The first dimension to *not* include in the operation
  RankType end;
};

/// @brief Gives the product of a range of extents chosen by index within the given Extents type
/// @note Implements the exposition-only helpers fwd-prod-of-extents and rev-prod-of-extents in [mdspan.extents.expo].
/// The former corresponds to <c>begin=0, end=i</c>; the latter is <c>begin=i+1, end=rank()</c>.
/// @tparam Extents The type of the extents object to get a product of, deduced from @c extents
/// @param extents The extents object to get a product of
/// @param dims The range of dimensions to use for the product
/// @return The product of the extents entries with dimension indices in <c>[dims.begin, dims.end)</c>, or an
/// empty @c optional if this product would wrap
template <typename Extents, constraints<std::enable_if_t<Extents::rank() != 0U>> = nullptr>
constexpr auto extent_product(
    Extents const& extents,
    dim_range<typename Extents::rank_type> const& dims = {0U, Extents::rank()}
) noexcept -> mdspan_detail::checked_math_result<typename Extents::index_type> {
  using index_type = typename Extents::index_type;

  auto total = mdspan_detail::checked_math_result<index_type>{index_type{1}};

  for (typename Extents::rank_type dim{dims.begin}; dim < dims.end; ++dim) {
    total = mdspan_detail::checked_multiplies<index_type>(total, extents.extent(dim));
  }

  return total;
}

/// @brief Gives the product of a range of extents chosen by index within the given Extents type
/// @note Implements the exposition-only helpers fwd-prod-of-extents and rev-prod-of-extents in [mdspan.extents.expo].
/// The former corresponds to <c>begin=0, end=i</c>; the latter is <c>begin=i+1, end=rank()</c>.
/// @tparam Extents The type of the extents object to get a product of, deduced from @c extents
/// @return The product of the extents entries with dimension indices in <c>[dims.begin, dims.end)</c>, or an
/// empty @c optional if this product would wrap
template <typename Extents, constraints<std::enable_if_t<Extents::rank() == 0U>> = nullptr>
constexpr auto extent_product(  //
    Extents const&,
    dim_range<typename Extents::rank_type> const& = {0U, Extents::rank()}
) noexcept -> mdspan_detail::checked_math_result<typename Extents::index_type> {
  return mdspan_detail::checked_math_result<typename Extents::index_type>{typename Extents::index_type{1}};
}

// parasoft-begin-suppress AUTOSAR-A8_4_2-a CERT_CPP-MSC52-a-2 CERT_C-MSC37-a "False positive: this function returns a
// value"

/// @brief Check if overflow would occur when stride-mapping the given @c extents into a one-dimensional output span
/// @tparam Extents The @c extents type to check
/// @return @c true if no extents are dynamic and some output indices would overflow an @c Extents::index_type
template <typename Extents, constraints<std::enable_if_t<Extents::rank_dynamic() == 0U>> = nullptr>
constexpr auto mapped_indices_overflow() noexcept -> bool {
  // There would be an overflow in the mapping if and only if the product of all extents overflows.
  return extent_product(Extents{}).overflowed;
}

// parasoft-end-suppress AUTOSAR-A8_4_2-a CERT_CPP-MSC52-a-2 CERT_C-MSC37-a

/// @brief Check if overflow would occur when stride-mapping the given @c extents into a one-dimensional output span
/// @tparam Extents The @c extents type to check
/// @return @c true if no extents are dynamic and some output indices would overflow an @c Extents::index_type
template <typename Extents, constraints<std::enable_if_t<Extents::rank_dynamic() != 0U>> = nullptr>
constexpr auto mapped_indices_overflow() noexcept -> bool {
  // Per [mdspan.layout.stride.overview:4], if there are any dynamic extents, they could be 0, so we let it through.
  return false;
}

/// @brief Get a set of strides with the smallest stride on the left and the largest on the right
/// @tparam Extents The type of the extents to use, deduced from @c extents
/// @param extents An extents object
/// @return An array of strides; the leftmost is @c 1 and each entry is the product of the extents to its left
/// @pre Each stride entry will fit in @c Extents::index_type without wrapping, else @c ARENE_PRECONDITION violation
template <typename Extents, constraints<std::enable_if_t<Extents::rank() != 0U>> = nullptr>
constexpr auto left_strides(Extents const& extents) noexcept
    -> ::arene::base::array<typename Extents::index_type, Extents::rank()> {
  using index_type = typename Extents::index_type;
  using rank_type = typename Extents::rank_type;

  ::arene::base::array<index_type, Extents::rank()> strides{};
  mdspan_detail::checked_math_result<index_type> current_stride{index_type{1}};
  strides[0U] = current_stride.value;
  for (rank_type dim{}; dim + rank_type{1} < Extents::rank(); ++dim) {
    current_stride = mdspan_detail::checked_multiplies<index_type>(current_stride, extents.extent(dim));
    if (current_stride.value == index_type{}) {
      // The standard doesn't allow strides to be 0, which can be naively computed if an extent is 0.
      // If any extents are 0, trying to use this mapping will always go out of bounds so the value doesn't matter.
      current_stride.value = index_type{1};
    }
    strides[dim + rank_type{1}] = current_stride.value;
  }
  ARENE_PRECONDITION(!current_stride.overflowed);

  return strides;
}

/// @brief Get a set of strides with the smallest stride on the left and the largest on the right
/// @note This special overload for <c>rank == 0</c> exists entirely to make it easier to measure test coverage.
/// @tparam Extents The type of the extents to use, deduced from the first parameter
/// @return An array of strides; the leftmost is @c 1 and each entry is the product of the extents to its left
template <typename Extents, constraints<std::enable_if_t<Extents::rank() == 0U>> = nullptr>
constexpr auto left_strides(Extents const&) noexcept
    -> ::arene::base::array<typename Extents::index_type, Extents::rank()> {
  return {};
}

/// @brief Get a set of strides with the smallest stride on the right and the largest on the left
/// @tparam Extents The type of the extents to use, deduced from @c extents
/// @param extents An extents object
/// @return An array of strides; the rightmost is @c 1 and each entry is the product of the extents to its right
/// @pre Each stride entry will fit in @c Extents::index_type without wrapping, else @c ARENE_PRECONDITION violation
template <typename Extents, constraints<std::enable_if_t<Extents::rank() != 0U>> = nullptr>
constexpr auto right_strides(Extents const& extents) noexcept
    -> ::arene::base::array<typename Extents::index_type, Extents::rank()> {
  using index_type = typename Extents::index_type;
  using rank_type = typename Extents::rank_type;

  ::arene::base::array<index_type, Extents::rank()> strides{};
  mdspan_detail::checked_math_result<index_type> current_stride{index_type{1}};
  strides[Extents::rank() - rank_type{1}] = current_stride.value;
  for (rank_type dim{Extents::rank() - rank_type{1}}; dim > rank_type{}; --dim) {
    current_stride = mdspan_detail::checked_multiplies<index_type>(current_stride, extents.extent(dim));
    if (current_stride.value == index_type{}) {
      // The standard doesn't allow strides to be 0, which can be naively computed if an extent is 0.
      // If any extents are 0, trying to use this mapping will always go out of bounds so the value doesn't matter.
      current_stride.value = index_type{1};
    }
    strides[dim - rank_type{1}] = current_stride.value;
  }
  ARENE_PRECONDITION(!current_stride.overflowed);

  return strides;
}

/// @brief Get a set of strides with the smallest stride on the right and the largest on the left
/// @note This special overload for <c>rank == 0</c> exists entirely to make it easier to measure test coverage.
/// @tparam Extents The type of the extents to use, deduced from the first parameter
/// @return An array of strides; the rightmost is @c 1 and each entry is the product of the extents to its right
template <typename Extents, constraints<std::enable_if_t<Extents::rank() == 0U>> = nullptr>
constexpr auto right_strides(Extents const&) noexcept
    -> ::arene::base::array<typename Extents::index_type, Extents::rank()> {
  return {};
}

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

/// @brief Get the required size of an underlying data span in order for it to be validly mapped by a @c mapping
/// @tparam Extents The type of the extents used for this mapping
/// @param extents The extents being mapped by this @c mapping
/// @param strides A span of the strides being used by this @c mapping
/// @return The largest number of elements of a contiguous span which can be mapped to by the given strides and extents
/// @pre The return value would fit in an instance of <c>IndexType</c>, else @c ARENE_PRECONDITION violation
template <typename Extents, constraints<std::enable_if_t<Extents::rank() != 0>> = nullptr>
constexpr auto
required_span_size(Extents const& extents, span<typename Extents::index_type const, Extents::rank()> strides) noexcept
    -> typename Extents::index_type {
  using index_type = typename Extents::index_type;
  using rank_type = typename Extents::rank_type;

  constexpr auto dimensions = sequential_values<rank_type, Extents::rank>;

  // If any of the extents' sizes are 0, then the size of their product space is also 0.
  for (auto const dim : dimensions) {
    if (extents.extent(dim) == index_type{}) {
      return {};
    }
  }

  // In general, the number of mapped elements is 1 (for 0,0,0,...) plus the sum of the count of additional elements
  // in each dimension (i.e. its extent - 1), multiplied by that dimension's stride.
  auto required_size = mdspan_detail::checked_math_result<index_type>{index_type{1}};
  for (auto const dim : dimensions) {
    index_type const increment{static_cast<index_type>((extents.extent(dim) - index_type{1}) * strides[dim])};
    required_size = mdspan_detail::checked_plus<index_type>(required_size, increment);
  }

  ARENE_PRECONDITION(!required_size.overflowed);
  return required_size.value;
}

/// @brief Get the required size of an underlying data span in order for it to be validly mapped by a @c mapping
/// @tparam Extents The type of the extents used for this mapping
/// @return The largest number of elements of a contiguous span which can be mapped to by the given strides and extents
template <typename Extents, constraints<std::enable_if_t<Extents::rank() == 0>> = nullptr>
constexpr auto required_span_size(Extents const&, span<typename Extents::index_type const, Extents::rank()>) noexcept ->
    typename Extents::index_type {
  // If there are no extents at all, then this is a scalar which should have size 1.
  return {1U};
}

/// @brief Precondition check for an index bound, hoisted into its own function for coverage tooling purposes
/// @tparam IndexType The type of the indices on both the input and output sides of a mapping
/// @param index An index that someone has put into this mapping
/// @param extent The extent of the dimension that @c index is for
/// @pre @c index must be within bounds of <c>extent</c>, else @c ARENE_PRECONDITION violation
template <typename IndexType>
constexpr auto precondition_bounds_check(IndexType index, IndexType extent) noexcept -> void {
  ARENE_PRECONDITION(index < extent);
}

/// @brief Stride-map the given set of input indices into a single output index
/// @tparam Extents The type of the extents used for this mapping
/// @param extents The extents of the input dimensions
/// @param strides The strides of this mapping in each of the input dimensions
/// @param indices A set of actual indices in the input dimensions, to be mapped to the output dimension
/// @return The image of @c indices under the mapping defined by @c extents and @c strides
/// @pre Every @c indices[i] is within bounds of the matching <c>extents[i]</c>, else @c ARENE_PRECONDITION violation
template <typename Extents, constraints<std::enable_if_t<Extents::rank() != 0U>> = nullptr>
constexpr auto stride_map_indices(
    Extents const& extents,
    span<typename Extents::index_type const, Extents::rank()> strides,
    span<typename Extents::index_type const, Extents::rank()> indices
) noexcept -> typename Extents::index_type {
  typename Extents::index_type mapped_idx{};
  for (typename Extents::rank_type dim{}; dim < Extents::rank(); ++dim) {
    precondition_bounds_check(indices[dim], extents.extent(dim));
    mapped_idx = static_cast<typename Extents::index_type>(mapped_idx + (indices[dim] * strides[dim]));
  }
  return mapped_idx;
}

/// @brief Stride-map the given set of input indices into a single output index
/// @tparam Extents The type of the extents used for this mapping
/// @return The image of @c indices under the mapping defined by @c extents and @c strides
/// @pre Every @c indices[i] is within bounds of the matching <c>extents[i]</c>, else @c ARENE_PRECONDITION violation
template <typename Extents, constraints<std::enable_if_t<Extents::rank() == 0U>> = nullptr>
constexpr auto
stride_map_indices(Extents const&, span<typename Extents::index_type const, Extents::rank()>, span<typename Extents::index_type const, Extents::rank()>) noexcept
    -> typename Extents::index_type {
  return {};
}

// ----- Constraints and precondition checks for various constructors -----

/// @brief Assert that a checked math operation has not overflowed (separate function for coverage purposes)
/// @tparam T The type in which the checked math operation was conducted
/// @param result A result of a math operation
/// @pre @c result has not overflowed, else @c ARENE_PRECONDITION violation
template <typename T>
constexpr auto precondition_not_overflowed(mdspan_detail::checked_math_result<T> const& result) noexcept -> void {
  ARENE_PRECONDITION(!result.overflowed);
}

/// @brief Cast one extents type to another, checking along the way that the product of the extents can be represented
/// @tparam ToExtents The output type of the cast
/// @tparam FromExtents The input type of the cast
/// @param from_extents A set of extents to cast to @c ToExtents
/// @pre The product of all the extents in @c from_extents can be represented as a <c>ToExtents::index_type</c>, else
/// @c ARENE_PRECONDITION violation
/// @return @c from_extents represented as a @c to_extents
template <
    typename ToExtents,
    typename FromExtents,
    constraints<std::enable_if_t<!std::is_convertible<FromExtents, ToExtents>::value>> = nullptr>
constexpr auto extents_cast(FromExtents&& from_extents) noexcept -> ToExtents {
  ToExtents const output{std::forward<FromExtents>(from_extents)};

  // Checking this for overflow in the *destination* index type; if it overflows, we crash.
  precondition_not_overflowed(extent_product(output));
  return output;
}

/// @brief Cast one extents type to another, checking along the way that the product of the extents can be represented
/// @tparam ToExtents The output type of the cast
/// @tparam FromExtents The input type of the cast
/// @param from_extents A set of extents to cast to @c ToExtents
/// @pre The product of all the extents in @c from_extents can be represented as a <c>ToExtents::index_type</c>, else
/// @c ARENE_PRECONDITION violation
/// @return @c from_extents represented as a @c to_extents
template <
    typename ToExtents,
    typename FromExtents,
    constraints<std::enable_if_t<std::is_convertible<FromExtents, ToExtents>::value>> = nullptr>
constexpr auto extents_cast(FromExtents&& from_extents) noexcept -> ToExtents {
  return std::forward<FromExtents>(from_extents);
}

/// @brief Check that two sets of strides are equal to each other, crashing with a precondition violation if not
/// @tparam LeftIndex The @c index_type of the @c mapping from which @c left was taken
/// @tparam RightIndex The @c index_type of the @c mapping from which @c right was taken
/// @param left The left list of strides to compare
/// @param right The right list of strides to compare
/// @pre The elements of @c left and @c right are equal to each other, else @c ARENE_PRECONDITION violation
template <typename LeftIndex, typename RightIndex>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters) These parameters are symmetric; swapping them changes nothing
constexpr auto precondition_strides_equal(span<LeftIndex const> left, span<RightIndex const> right) noexcept -> void {
  ARENE_PRECONDITION(equal(left.begin(), left.end(), right.begin(), right.end()));
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
class layout_stride::mapping : private Extents {
  static_assert(is_extents_v<Extents>, "Extents type parameter must be a specialization of arene::base::extents");
  static_assert(!layout_detail::mapped_indices_overflow<Extents>(), "Index space mapped by Extents must not overflow");

 public:
  /// @brief The @c extents type mapped by this @c mapping
  using extents_type = Extents;
  /// @brief The type used to represent both input and output indices
  using index_type = typename Extents::index_type;
  /// @brief A version of @c index_type which is guaranteed to always be unsigned and thus can be used as a size
  using size_type = typename Extents::size_type;
  /// @brief The type used to represent the *rank* indices of <c>extents_type</c>, e.g. to iterate over its dimensions
  using rank_type = typename Extents::rank_type;
  /// @brief The tag type of the layout used by this @c mapping
  using layout_type = layout_stride;

  static_assert(
      std::is_same<mapping::rank_type, std::size_t>::value,
      "According to the C++26 standard, the rank_type must always be the one set in extents, which is std::size_t"
  );

 private:
  // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: Identifier array is qualified"
  /// @brief A list of constant strides for each of the dimensions in @c Extents
  ::arene::base::array<index_type, extents_type::rank()> strides_;
  // parasoft-end-suppress AUTOSAR-M14_6_1-a

 protected:
  /// @brief A passkey class used to bypass construction precondition checks; only usable from child layout types
  class check_bypasser {
    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "friend declaration is used for passkey, not for exposing private data"
    friend layout_left::mapping<extents_type>;
    friend layout_right::mapping<extents_type>;
    // parasoft-end-suppress AUTOSAR-A11_3_1-a

    /// @brief Construct a check_bypasser (private so only callable from friend classes)
    constexpr check_bypasser() noexcept = default;

    // parasoft-begin-suppress CERT_C-EXP37-a "False positive: Rule does not require all parameters to be named"
    /// @brief Copy a check_bypasser (private so only callable from friend classes)
    constexpr check_bypasser(check_bypasser const&) noexcept = default;
    /// @brief Move a check_bypasser (private so only callable from friend classes)
    constexpr check_bypasser(check_bypasser&&) noexcept = default;
    /// @brief Copy assign a check_bypasser (private so only callable from friend classes)
    constexpr auto operator=(check_bypasser const&) noexcept -> check_bypasser& = default;
    /// @brief Move assign a check_bypasser (private so only callable from friend classes)
    constexpr auto operator=(check_bypasser&&) noexcept -> check_bypasser& = default;
    // parasoft-end-suppress CERT_C-EXP37-a

    /// @brief Destroy a check_bypasser (private so only callable from friend classes)
    ~check_bypasser() = default;
  };

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: Rule does not require all parameters to be named"
  /// @brief Construct a mapping with the given strides, bypassing precondition checks
  /// @note This can only be called from layout_left and layout_right, which are specified to *not* have the positivity
  /// or required_span_size preconditions. They inherently can not violate the uniqueness precondition.
  /// @param exts A set of extents to use for this mapping
  /// @param strds A set of strides to use for this mapping
  constexpr mapping(
      extents_type const& exts,
      array<index_type, extents_type::rank()> const& strds,
      check_bypasser
  ) noexcept
      : extents_type(exts),
        strides_(strds) {}
  // parasoft-end-suppress CERT_C-EXP37-a

 public:
  /// @brief Construct a default strided mapping for the given extents, which has the same layout as @c layout_right
  constexpr mapping() noexcept
      : extents_type(),
        strides_(layout_detail::right_strides(extents_type())) {
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
      : extents_type(exts),
        strides_(layout_detail::stride_cast<index_type>(strds)) {
    // The representable precondition is checked here as a side-effect of 'required_span_size'.
    index_type const span_size{layout_detail::required_span_size(this->extents(), this->strides_)};

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
      : extents_type(other.extents()),
        strides_(layout_detail::stride_cast<index_type>(
            span<typename StridedLayoutMapping::index_type const, StridedLayoutMapping::extents_type::rank()>(
                layout_detail::extract_strides(other)
            )
        )) {
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
      : extents_type(other.extents()),
        strides_(layout_detail::stride_cast<index_type>(
            span<typename StridedLayoutMapping::index_type const, StridedLayoutMapping::extents_type::rank()>(
                layout_detail::extract_strides(other)
            )
        )) {
    // The second precondition is enforced by layout_detail::stride_cast
    ARENE_PRECONDITION(arene::base::cmp_less_equal(other.required_span_size(), std::numeric_limits<index_type>::max()));
    ARENE_PRECONDITION(layout_detail::get_offset_of_mapping(other) == typename StridedLayoutMapping::index_type{});
  }

  /// @brief Get the extents of this @c mapping
  /// @return The extents of this @c mapping
  constexpr auto extents() const noexcept -> extents_type const& { return *this; }

  /// @brief Get a copy of the strides in this @c mapping (they're immutable so this can't go out of date)
  /// @return The strides of this @c mapping
  constexpr auto strides() const noexcept -> array<index_type, extents_type::rank()> { return strides_; }

  /// @brief Get the stride of a particular rank in this @c mapping
  /// @param dim_idx The index of the dimension to return the stride of
  /// @return The stride of the selected rank
  /// @pre <c>dim_idx < extents_type::rank()</c>, else @c ARENE_PRECONDITION violation
  constexpr auto stride(rank_type dim_idx) const noexcept -> index_type { return strides_[dim_idx]; }

  /// @brief Get the required size of an underlying data span in order for it to be validly mapped by this @c mapping
  /// @return The largest number of elements of a contiguous span which this @c mapping will map an index to
  constexpr auto required_span_size() const noexcept -> index_type {
    return layout_detail::required_span_size(this->extents(), this->strides_);
  }

  /// @brief Map from a pack of indices into the one-dimensional output index they'd correspond to in an underlying span
  /// @tparam Indices The types of the indices to map; must all be nothrow-convertible to @c index_type
  /// @param input_indices The indices to map; there must be exactly one index per dimension of @c extents_type
  /// @return The one-dimension index within an underlying span corresponding to @c indices
  /// @pre Every index is in bounds for its respective extent in <c>extents()</c>, else @c ARENE_PRECONDITION violation
  template <
      class... Indices,
      constraints<
          std::enable_if_t<sizeof...(Indices) == extents_type::rank()>,
          std::enable_if_t<all_of_v<std::is_convertible<Indices, index_type>::value...>>,
          std::enable_if_t<all_of_v<std::is_nothrow_constructible<index_type, Indices>::value...>>> = nullptr>
  constexpr auto operator()(Indices&&... input_indices) const noexcept -> index_type {
    // This uses a normal static_cast instead of index-cast from the standard; since the Indices are all constrained to
    // be implicitly convertible to index_type, it should not lose information, and this way avoids compiler warnings.

    // parasoft-begin-suppress AUTOSAR-M8_5_2-a "False positive: there is no subobject here that can be initialized"
    array<mapping::index_type, extents_type::rank()> indices{static_cast<index_type>(std::forward<Indices>(input_indices
    ))...};
    // parasoft-end-suppress AUTOSAR-M8_5_2-a
    return layout_detail::stride_map_indices(this->extents(), this->strides_, indices);
  }

  /// @brief Return whether or not this mapping is always unique, i.e. distinct indices always map to distinct elements
  /// @return @c true because this is enforced by the constructors as a precondition
  /// @note In mathematical terms this asks if the mapping is always injective/one-to-one.
  static constexpr auto is_always_unique() noexcept -> bool { return true; }

  /// @brief Return whether or not this mapping is always exhaustive, i.e. every element is reachable using some indices
  /// @return @c false because it's possible to construct non-exhaustive mappings using this class
  /// @note In mathematical terms this asks if the mapping is always surjective/onto.
  static constexpr auto is_always_exhaustive() noexcept -> bool { return false; }

  /// @brief Return whether or not this mapping is always strided, i.e. every dimension has a constant stride
  /// @return @c true because this class directly uses striding to compute its mapped indices
  /// @note In mathematical terms this asks if the mapping is always affine (isomorphic to a dot product plus offset).
  static constexpr auto is_always_strided() noexcept -> bool { return true; }

  /// @brief Return whether or not this instance is unique, i.e. distinct indices always map to distinct elements
  /// @return Always @c true because this is enforced by the constructors as a precondition
  /// @note In mathematical terms this asks if the mapping is injective/one-to-one.
  static constexpr auto is_unique() noexcept -> bool { return true; }

  /// @brief Return whether or not this instance is exhaustive, i.e. every element is reachable using some indices
  /// @return @c true if every provided stride is the product of the extents corresponding to some subset of the other
  /// strides, otherwise @c false
  /// @note In mathematical terms this asks if the mapping is surjective/onto.
  constexpr auto is_exhaustive() const noexcept -> bool {
    return required_span_size() == layout_detail::extent_product(this->extents()).value;
  }

  /// @brief Return whether or not this instance is strided, i.e. every dimension has a constant stride
  /// @return Always @c true because this class directly uses striding to compute its mapped indices
  /// @note In mathematical terms this asks if the mapping is affine (isomorphic to a dot product plus an offset).
  static constexpr auto is_strided() noexcept -> bool { return true; }

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
      if ((left.extents().extent(dim) != right.extents().extent(dim)) || (left.stride(dim) != right.stride(dim))) {
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
};
// parasoft-end-suppress AUTOSAR-A14_5_1-a
// parasoft-end-suppress AUTOSAR-A12_1_5-a

/// @brief Specialization of @c is_layout_mapping_v to break a recursive cycle in the @c layout_stride equality operator
/// @tparam Extents Any specialization of @c arene::base::extents
template <typename Extents>
extern constexpr bool is_layout_mapping_v<layout_stride::mapping<Extents>> = true;

// parasoft-begin-suppress AUTOSAR-A2_10_1 "Base class is inherited privately so its identifiers are not visible"
// parasoft-begin-suppress AUTOSAR-A7_3_1-a "False positive: there is no rule A7-3-1"
// parasoft-begin-suppress AUTOSAR-A10_2_1-b "False positive: this is not a multiple inheritance hierarchy"
// parasoft-begin-suppress AUTOSAR-A10_2_1-a "False positive: this is not a multiple inheritance hierarchy"
/// @brief A mapping from a logical pack of indices into a single flat physical output index
/// @tparam The extents of the space used for this mapping; must be a specialization of @c extents
template <class Extents>
class layout_right::mapping : private layout_stride::mapping<Extents> {
 private:
  /// @brief Convenience typedef referring to the base strided mapping
  using base_type = layout_stride::mapping<Extents>;

 public:
  using typename base_type::extents_type;
  using typename base_type::index_type;
  using typename base_type::rank_type;
  using typename base_type::size_type;
  /// @brief The tag type of the layout used by this @c mapping
  using layout_type = layout_right;

  /// @brief Construct a right-strided mapping for a default-constructed set of extents
  constexpr mapping() noexcept = default;

  /// @brief Construct a right-strided mapping for the given extents
  /// @param exts The extents to use
  /// @pre The size of the index space represented by @c exts fits within <c>index_type</c>, else @c ARENE_PRECONDITION
  /// violation
  // NOLINTNEXTLINE(hicpp-explicit-conversions) C++23 defines this as implicit
  constexpr mapping(extents_type const& exts) noexcept
      : base_type(exts, layout_detail::right_strides(exts), typename base_type::check_bypasser{}) {}

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a mapping with a different (but compatible) @c extents_type
  /// @note This is implicit if and only if @c OtherExtents is implicitly convertible to @c extents_type
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The other @c layout_right::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<std::enable_if_t<std::is_convertible<OtherExtents, extents_type>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) C++23 defines this as implicit
  constexpr mapping(mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a mapping with a different (but compatible) @c extents_type
  /// @note This is implicit if and only if @c OtherExtents is implicitly convertible to @c extents_type
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The other @c layout_right::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<
          std::enable_if_t<std::is_constructible<extents_type, OtherExtents>::value>,
          std::enable_if_t<!std::is_convertible<OtherExtents, extents_type>::value>> = nullptr>
  constexpr explicit mapping(mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a @c layout_left::mapping when both have 0- or 1-dimensional extents
  /// @note This is implicit if and only if @c OtherExtents is implicitly convertible to @c extents_type
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The @c layout_left::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<
          std::enable_if_t<std::is_convertible<OtherExtents, extents_type>::value>,
          std::enable_if_t<OtherExtents::rank() <= rank_type{1U}>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) C++23 defines this as implicit
  constexpr mapping(layout_left::mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a @c layout_left::mapping when both have 0- or 1-dimensional extents
  /// @note This is implicit if and only if @c OtherExtents is implicitly convertible to @c extents_type
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The @c layout_left::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<
          std::enable_if_t<std::is_constructible<extents_type, OtherExtents>::value>,
          std::enable_if_t<!std::is_convertible<OtherExtents, extents_type>::value>,
          std::enable_if_t<OtherExtents::rank() <= rank_type{1U}>> = nullptr>
  constexpr explicit mapping(layout_left::mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a @c layout_stride::mapping with a compatible @c extents_type
  /// @note This is implicit if and only if <c>OtherExtents::rank() == 0</c>
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The @c layout_stride::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  /// @pre <c>other</c>'s strides are exactly those of a @c layout_right::mapping with its <c>extents</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<
          std::enable_if_t<std::is_constructible<extents_type, OtherExtents>::value>,
          std::enable_if_t<OtherExtents::rank() == rank_type{0U}>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) C++23 defines this as implicit
  constexpr mapping(layout_stride::mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {
    // The second precondition need not be checked here, because rank-0 strides are empty and so always right-strided.
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a @c layout_stride::mapping with a compatible @c extents_type
  /// @note This is implicit if and only if <c>OtherExtents::rank() == 0</c>
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The @c layout_stride::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  /// @pre <c>other</c>'s strides are exactly those of a @c layout_right::mapping with its <c>extents</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<
          std::enable_if_t<std::is_constructible<extents_type, OtherExtents>::value>,
          std::enable_if_t<(OtherExtents::rank() > rank_type{0U})>> = nullptr>
  constexpr explicit mapping(layout_stride::mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {
    // For coverage purposes, this enforces the 2nd precondition in a separate function that doesn't depend on Extents.
    layout_detail::precondition_strides_equal<index_type, typename OtherExtents::index_type>(
        this->strides(),
        other.strides()
    );
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  using base_type::extents;
  using base_type::required_span_size;
  using base_type::stride;
  using base_type::operator();

  using base_type::is_always_strided;
  using base_type::is_always_unique;
  using base_type::is_strided;
  using base_type::is_unique;

  /// @brief Return whether or not this mapping is always exhaustive, i.e. every element is reachable using some indices
  /// @return @c true because the mapping is always computed by a simple invertible right-strided algorithm
  /// @note In mathematical terms this asks if the mapping is always surjective/onto.
  static constexpr auto is_always_exhaustive() noexcept -> bool { return true; }

  /// @brief Return whether or not this instance is exhaustive, i.e. every element is reachable using some indices
  /// @return @c true if every provided stride is the product of the extents corresponding to some subset of the other
  /// strides, otherwise @c false
  /// @note In mathematical terms this asks if the mapping is surjective/onto.
  static constexpr auto is_exhaustive() noexcept -> bool { return true; }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  /// @brief Compare two right-strided mappings for equality
  /// @tparam OtherExtents The type of the extents used by the right-hand side of the comparison
  /// @param left An instance of @c layout_right::mapping
  /// @param right An instance of @c layout_right::mapping<OtherExtents>
  /// @return @c true if @c left and @c right have equal extents
  template <
      typename OtherExtents,
      constraints<std::enable_if_t<extents_type::rank() == OtherExtents::rank()>> = nullptr>
  friend constexpr auto operator==(mapping const& left, mapping<OtherExtents> const& right) noexcept -> bool {
    return left.extents() == right.extents();
  }

  /// @brief Compare two right-strided mappings for inequality
  /// @tparam OtherExtents The type of the extents used by the right-hand side of the comparison
  /// @param left An instance of @c layout_right::mapping
  /// @param right An instance of @c layout_right::mapping<OtherExtents>
  /// @return @c true if @c left and @c right have unequal extents
  template <
      typename OtherExtents,
      constraints<std::enable_if_t<extents_type::rank() == OtherExtents::rank()>> = nullptr>
  friend constexpr auto operator!=(mapping const& left, mapping<OtherExtents> const& right) noexcept -> bool {
    return !(left == right);
  }
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
};

/// @brief A mapping from a logical pack of indices into a single flat physical output index
/// @tparam The extents of the space used for this mapping; must be a specialization of @c extents
template <class Extents>
class layout_left::mapping : private layout_stride::mapping<Extents> {
 private:
  /// @brief Convenience typedef referring to the base strided mapping
  using base_type = layout_stride::mapping<Extents>;

 public:
  using typename base_type::extents_type;
  using typename base_type::index_type;
  using typename base_type::rank_type;
  using typename base_type::size_type;
  /// @brief The tag type of the layout used by this @c mapping
  using layout_type = layout_left;

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Construct a default left-strided mapping for the given extents
  constexpr mapping() noexcept
      : mapping(extents_type{}) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @brief Construct a left-strided mapping for the given extents
  /// @param exts The extents to use
  /// @pre The size of the index space represented by @c exts fits within <c>index_type</c>, else @c ARENE_PRECONDITION
  /// violation
  // NOLINTNEXTLINE(hicpp-explicit-conversions) C++23 defines this as implicit
  constexpr mapping(extents_type const& exts) noexcept
      : base_type(exts, layout_detail::left_strides(exts), typename base_type::check_bypasser{}) {}

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a mapping with a different (but compatible) @c extents_type
  /// @note This is implicit if and only if @c OtherExtents is implicitly convertible to @c extents_type
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The other @c layout_left::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<std::enable_if_t<std::is_convertible<OtherExtents, extents_type>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) C++23 defines this as implicit
  constexpr mapping(mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a mapping with a different (but compatible) @c extents_type
  /// @note This is implicit if and only if @c OtherExtents is implicitly convertible to @c extents_type
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The other @c layout_left::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<
          std::enable_if_t<std::is_constructible<extents_type, OtherExtents>::value>,
          std::enable_if_t<!std::is_convertible<OtherExtents, extents_type>::value>> = nullptr>
  constexpr explicit mapping(mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a @c layout_right::mapping when both have 0- or 1-dimensional extents
  /// @note This is implicit if and only if @c OtherExtents is implicitly convertible to @c extents_type
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The @c layout_right::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<
          std::enable_if_t<std::is_convertible<OtherExtents, extents_type>::value>,
          std::enable_if_t<OtherExtents::rank() <= rank_type{1U}>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) C++23 defines this as implicit
  constexpr mapping(layout_right::mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a @c layout_right::mapping when both have 0- or 1-dimensional extents
  /// @note This is implicit if and only if @c OtherExtents is implicitly convertible to @c extents_type
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The @c layout_right::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<
          std::enable_if_t<std::is_constructible<extents_type, OtherExtents>::value>,
          std::enable_if_t<!std::is_convertible<OtherExtents, extents_type>::value>,
          std::enable_if_t<OtherExtents::rank() <= rank_type{1U}>> = nullptr>
  constexpr explicit mapping(layout_right::mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a @c layout_stride::mapping with a compatible @c extents_type
  /// @note This is implicit if and only if <c>OtherExtents::rank() == 0</c>
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The @c layout_stride::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  /// @pre <c>other</c>'s strides are exactly those of a @c layout_right::mapping with its <c>extents</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<
          std::enable_if_t<std::is_constructible<extents_type, OtherExtents>::value>,
          std::enable_if_t<OtherExtents::rank() == rank_type{0U}>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) C++23 defines this as implicit
  constexpr mapping(layout_stride::mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {
    // The second precondition need not be checked here, because rank-0 strides are empty and so always left-strided.
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Converting constructor from a @c layout_stride::mapping with a compatible @c extents_type
  /// @note This is implicit if and only if <c>OtherExtents::rank() == 0</c>
  /// @tparam OtherExtents Type of <c>other</c>'s @c extents
  /// @param other The @c layout_stride::mapping
  /// @pre @c other.required_span_size() can be represented as a value of *this* type's <c>index_type</c>, else
  /// @c ARENE_PRECONDITION violation
  /// @pre <c>other</c>'s strides are exactly those of a @c layout_right::mapping with its <c>extents</c>, else
  /// @c ARENE_PRECONDITION violation
  template <
      typename OtherExtents,
      constraints<
          std::enable_if_t<std::is_constructible<extents_type, OtherExtents>::value>,
          std::enable_if_t<(OtherExtents::rank() > rank_type{0U})>> = nullptr>
  constexpr explicit mapping(layout_stride::mapping<OtherExtents> const& other) noexcept
      : mapping(layout_detail::extents_cast<extents_type>(other.extents())) {
    // For coverage purposes, this enforces the 2nd precondition in a separate function that doesn't depend on Extents.
    layout_detail::precondition_strides_equal<index_type, typename OtherExtents::index_type>(
        this->strides(),
        other.strides()
    );
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  using base_type::extents;
  using base_type::required_span_size;
  using base_type::stride;
  using base_type::operator();

  using base_type::is_always_strided;
  using base_type::is_always_unique;
  using base_type::is_strided;
  using base_type::is_unique;

  /// @brief Return whether or not this mapping is always exhaustive, i.e. every element is reachable using some indices
  /// @return @c true because the mapping is always computed by a simple invertible left-strided algorithm
  /// @note In mathematical terms this asks if the mapping is always surjective/onto.
  static constexpr auto is_always_exhaustive() noexcept -> bool { return true; }

  /// @brief Return whether or not this instance is exhaustive, i.e. every element is reachable using some indices
  /// @return @c true if every provided stride is the product of the extents corresponding to some subset of the other
  /// strides, otherwise @c false
  /// @note In mathematical terms this asks if the mapping is surjective/onto.
  static constexpr auto is_exhaustive() noexcept -> bool { return true; }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  /// @brief Compare two left-strided mappings for equality
  /// @tparam OtherExtents The type of the extents used by the right-hand side of the comparison
  /// @param left An instance of @c layout_left::mapping
  /// @param right An instance of @c layout_left::mapping<OtherExtents>
  /// @return @c true if @c left and @c right have equal extents
  template <
      typename OtherExtents,
      constraints<std::enable_if_t<extents_type::rank() == OtherExtents::rank()>> = nullptr>
  friend constexpr auto operator==(mapping const& left, mapping<OtherExtents> const& right) noexcept -> bool {
    return left.extents() == right.extents();
  }

  /// @brief Compare two left-strided mappings for inequality
  /// @tparam OtherExtents The type of the extents used by the right-hand side of the comparison
  /// @param left An instance of @c layout_left::mapping
  /// @param right An instance of @c layout_left::mapping<OtherExtents>
  /// @return @c true if @c left and @c right have unequal extents
  template <
      typename OtherExtents,
      constraints<std::enable_if_t<extents_type::rank() == OtherExtents::rank()>> = nullptr>
  friend constexpr auto operator!=(mapping const& left, mapping<OtherExtents> const& right) noexcept -> bool {
    return !(left == right);
  }
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
};
// parasoft-end-suppress AUTOSAR-A2_10_1
// parasoft-end-suppress AUTOSAR-A7_3_1-a
// parasoft-end-suppress AUTOSAR-A10_2_1-b
// parasoft-end-suppress AUTOSAR-A10_2_1-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_LAYOUT_HPP_
