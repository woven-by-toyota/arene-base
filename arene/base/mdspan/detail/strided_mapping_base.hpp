// parasoft-suppress CERT_C-EXP37-a AUTOSAR-A2_8_1-a  "False positive: there is no function named '()' here. Header
// defines layout types"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_STRIDED_MAPPING_BASE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_STRIDED_MAPPING_BASE_HPP_

// IWYU pragma: private, include "arene/base/mdspan/layout.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/mdspan/detail/checked_math.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/all_of.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
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

/// @brief Implementation base class providing shared strided mapping data and methods for all layout types
/// @tparam Extents The extents of the space used for this mapping; must be a specialization of @c extents
// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: class is copyable without a user-defined copy constructor"
template <class Extents>
class strided_mapping_base : private Extents {
  static_assert(is_extents_v<Extents>, "Extents type parameter must be a specialization of arene::base::extents");
  static_assert(!mapped_indices_overflow<Extents>(), "Index space mapped by Extents must not overflow");

 public:
  /// @brief The @c extents type mapped by this @c mapping
  using extents_type = Extents;
  /// @brief The type used to represent both input and output indices
  using index_type = typename Extents::index_type;
  /// @brief A version of @c index_type which is guaranteed to always be unsigned and thus can be used as a size
  using size_type = typename Extents::size_type;
  /// @brief The type used to represent the *rank* indices of <c>extents_type</c>, e.g. to iterate over its dimensions
  using rank_type = typename Extents::rank_type;

  static_assert(
      std::is_same<rank_type, std::size_t>::value,
      "According to the C++26 standard, the rank_type must always be the one set in extents, which is std::size_t"
  );

 private:
  // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: Identifier array is qualified"
  /// @brief A list of constant strides for each of the dimensions in @c Extents
  ::arene::base::array<index_type, extents_type::rank()> strides_;
  // parasoft-end-suppress AUTOSAR-M14_6_1-a

 protected:
  /// @brief Construct a mapping base with the given extents and strides
  /// @param exts A set of extents to use for this mapping
  /// @param strds A set of strides to use for this mapping
  constexpr strided_mapping_base(
      extents_type const& exts,
      array<index_type, extents_type::rank()> const& strds
  ) noexcept
      : extents_type(exts),
        strides_(strds) {}

 public:
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

  // parasoft-begin-suppress AUTOSAR-A2_10_1 "False positive: 'required_span_size' does not hide anything"
  /// @brief Get the required size of an underlying data span in order for it to be validly mapped by this @c mapping
  /// @return The largest number of elements of a contiguous span which this @c mapping will map an index to
  constexpr auto required_span_size() const noexcept -> index_type {
    return layout_detail::required_span_size(this->extents(), this->strides_);
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1

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
    array<index_type, extents_type::rank()> indices{static_cast<index_type>(std::forward<Indices>(input_indices))...};
    // parasoft-end-suppress AUTOSAR-M8_5_2-a
    return stride_map_indices(this->extents(), this->strides_, indices);
  }

  /// @brief Return whether or not this mapping is always unique, i.e. distinct indices always map to distinct elements
  /// @return @c true because this is enforced by the constructors as a precondition
  /// @note In mathematical terms this asks if the mapping is always injective/one-to-one.
  static constexpr auto is_always_unique() noexcept -> bool { return true; }

  /// @brief Return whether or not this mapping is always strided, i.e. every dimension has a constant stride
  /// @return @c true because this class directly uses striding to compute its mapped indices
  /// @note In mathematical terms this asks if the mapping is always affine (isomorphic to a dot product plus offset).
  static constexpr auto is_always_strided() noexcept -> bool { return true; }

  /// @brief Return whether or not this instance is unique, i.e. distinct indices always map to distinct elements
  /// @return Always @c true because this is enforced by the constructors as a precondition
  /// @note In mathematical terms this asks if the mapping is injective/one-to-one.
  static constexpr auto is_unique() noexcept -> bool { return true; }

  /// @brief Return whether or not this instance is strided, i.e. every dimension has a constant stride
  /// @return Always @c true because this class directly uses striding to compute its mapped indices
  /// @note In mathematical terms this asks if the mapping is affine (isomorphic to a dot product plus an offset).
  static constexpr auto is_strided() noexcept -> bool { return true; }
};
// parasoft-end-suppress AUTOSAR-A14_5_1-a

}  // namespace layout_detail
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_STRIDED_MAPPING_BASE_HPP_
