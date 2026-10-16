// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// parasoft-suppress CERT_C-EXP37-a "False positive: there is no function named '()' in this header"
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_LAYOUT_CONSTANT_DIAGONAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_LAYOUT_CONSTANT_DIAGONAL_HPP_

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/algorithm/any_of.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/representable_cast.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_manipulation/ebo_holder.hpp"

namespace arene {
namespace base {
namespace linalg {

namespace layout_constant_diagonal_detail {

/// @brief The maximum required span size for the underlying layout
/// @tparam IndexType the type required
template <typename IndexType>
extern constexpr IndexType max_required_span_size{2};

/// @brief Check if any extents are zero
/// @tparam ExtentsType The type of the extents
/// @tparam Indices The indices into the dimensions of the extents object
/// @param extents The extents object
/// @return @c true if any of @c extents.extent(Indices)... are zero, @c false otherwise
template <typename ExtentsType, std::size_t... Indices>
constexpr auto any_extents_are_zero(ExtentsType const& extents, std::index_sequence<Indices...>) noexcept -> bool {
  return any_of({(extents.extent(Indices) == typename ExtentsType::index_type{})...});
}

/// @brief Check if any extents are zero
/// @tparam ExtentsType The type of the extents
/// @param extents The extents object
/// @return @c true if any of @c extents.extent(Index) is zero for all values of @c Index in the range @c 0 to @c
/// extents.rank()-1 , @c false otherwise
template <typename ExtentsType>
constexpr auto any_extents_are_zero(ExtentsType const& extents) noexcept -> bool {
  return any_extents_are_zero(extents, std::make_index_sequence<ExtentsType::rank()>());
}

/// @brief Check if any static extents are zero
/// @tparam ExtentsType The type of the extents
/// @tparam Indices The indices into the dimensions of the extents object
/// @return @c true if any of @c ExtentsType::static_extent(Indices)... are zero, @c false otherwise
template <typename ExtentsType, std::size_t... Indices>
constexpr auto any_static_extents_are_zero(std::index_sequence<Indices...>) noexcept -> bool {
  return any_of({(ExtentsType::static_extent(Indices) == std::size_t{})...});
}

/// @brief Check if any static extents are zero
/// @tparam ExtentsType The type of the extents
/// @return @c true if any of @c ExtentsType::static_extent(Index) is zero for all values of @c Index in the range @c 0
/// to
/// @c ExtentsType::rank()-1 , @c false otherwise
template <typename ExtentsType>
constexpr auto any_static_extents_are_zero() noexcept -> bool {
  return any_static_extents_are_zero<ExtentsType>(std::make_index_sequence<ExtentsType::rank()>());
}

/// @brief Check if the supplied set of extents and offsets is exhaustive
///
/// This handles the rank-1 layouts
/// @tparam OffsetType The type of the offsets
/// @tparam IndexType The type of the extents
/// @param extents The array of extents
/// @return @c true if the extent is zero, @c false otherwise
///
/// A rank-1 layout is treated as a full (1-D) diagonal matrix, so every index is on the diagonal and maps to slot 0. A
/// non-empty layout therefore reports <c>required_span_size() == 2</c> while slot 1 is never referenced, so it is not
/// exhaustive. Rank-reduced slices of a higher-rank diagonal matrix -- where a rank-1 view could sit off the diagonal
/// -- are a separate layout
template <typename OffsetType, typename IndexType>
constexpr auto is_exhaustive(array<OffsetType, 1> const&, array<IndexType, 1> const& extents) noexcept -> bool {
  return extents[0U] == IndexType{};
}

/// @brief Check if the supplied set of extents and offsets is exhaustive
///
/// This handles the layouts with a rank greater than 1
/// @tparam OffsetType The type of the offsets
/// @tparam IndexType The type of the extents
/// @param offsets The array of offsets
/// @param extents The array of extents
/// @return @c true if any extent is zero, or if there is at least one dimension with an extent of two or more, no
/// dimensions with an extent of zero, and there is at least one value of @c index for each dimension such that @c
/// index+offset is the same for all dimensions, @c false otherwise
template <typename OffsetType, typename IndexType, std::size_t Rank>
constexpr auto is_exhaustive(array<OffsetType, Rank> const& offsets, array<IndexType, Rank> const& extents) noexcept
    -> bool {
  auto const dim_zero_extent = extents[0U];
  if (dim_zero_extent == IndexType{}) {
    return true;
  }
  bool has_exhaustive_extent{dim_zero_extent >= max_required_span_size<IndexType>};
  IndexType min_overlap_position{mdspan_detail::representable_cast<IndexType>(offsets[0U])};
  IndexType max_overlap_position{min_overlap_position + dim_zero_extent};
  for (std::size_t index{1U}; index < Rank; ++index) {
    auto const this_extent = extents[index];
    if (this_extent == IndexType{}) {
      return true;
    }
    if (this_extent >= max_required_span_size<IndexType>) {
      has_exhaustive_extent = true;
    }
    IndexType const min_value_this_extent{mdspan_detail::representable_cast<IndexType>(offsets[index])};
    IndexType const max_value_this_extent{min_value_this_extent + this_extent};
    if (min_value_this_extent > min_overlap_position) {
      min_overlap_position = min_value_this_extent;
    }
    if (max_value_this_extent < max_overlap_position) {
      max_overlap_position = max_value_this_extent;
    }
  }
  return (max_overlap_position > min_overlap_position) && has_exhaustive_extent;
}
}  // namespace layout_constant_diagonal_detail

/// @brief An @c mdspan layout mapping policy for a diagonal tensorlet.
/// @tparam OffsetExtentsType An instantiation of @c extents which specifies the offset of the diagonal
///
/// Within the mapped layouts, the @c OffsetExtentsType determine where the diagonal is
/// - For a rank 0 tensorlet, there are no offsets, and the one element is always on the diagonal
/// - For a rank 1 tensorlet, all elements are on the diagonal
/// - For a rank <c>N>=2</c> tensorlet, the diagonal is where all values of <c>index_n + offsets.extent(n)</c> are
/// equal, where @c index_n is an index for dimension @c n, such that <c>index_n>=0</c> and <c>index_n <
/// extents.extent(n)</c>
template <typename OffsetExtentsType>
class layout_constant_diagonal {
  static_assert(is_extents_v<OffsetExtentsType>, "The offsets must be an instantiation of arene::base::extents");

  /// @brief Tag type to use for selecting the extents instance held via @c ebo_holder
  struct extents_tag {};
  /// @brief Tag type to use for selecting the offsets instance held via @c ebo_holder
  struct offsets_tag {};

 public:
  /// @brief An @c mdspan layout mapping for the specified @c ExtentsType
  template <typename ExtentsType>
  // parasoft-begin-suppress AUTOSAR-A10_1_1-a "Multiple base classes needed for empty-base-class optimizations."
  // parasoft-begin-suppress AUTOSAR-A2_10_1 "False positive: There is no identifier 'mapping' being hidden"
  class mapping
      // parasoft-end-suppress AUTOSAR-A2_10_1
      // parasoft-end-suppress AUTOSAR-A10_1_1-a
      : ebo_holder<extents_tag, ExtentsType>
      , ebo_holder<offsets_tag, OffsetExtentsType> {
    static_assert(is_extents_v<ExtentsType>, "The extents must be an instantiation of arene::base::extents");
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert"
    static_assert(ExtentsType::rank() == OffsetExtentsType::rank(), "The offsets and extents must have the same rank");
    // parasoft-end-suppress CERT_C-PRE31-c-3

    /// @brief Base class for holding the extents
    using extents_base = ebo_holder<extents_tag, ExtentsType>;

    /// @brief Base class for holding the offsets
    using offsets_base = ebo_holder<offsets_tag, OffsetExtentsType>;

   public:
    /// @brief The type of the offsets
    using offsets_type = OffsetExtentsType;
    /// @brief The type of the extents
    using extents_type = ExtentsType;
    /// @brief The index type for the extents
    using index_type = typename extents_type::index_type;
    /// @brief The rank type for the extents
    using rank_type = typename extents_type::rank_type;
    /// @brief The layout mapping policy type for this mapping
    using layout_type = layout_constant_diagonal;

   private:
    /// @brief The mapped index to return for an empty index list
    /// @return Zero
    // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
    static constexpr auto mapped_index(std::index_sequence<>) noexcept -> index_type { return 0; }
    // parasoft-end-suppress CERT_C-EXP37-a

    /// @brief The mapped index to return for a single index
    /// @tparam FirstAccessIndex The type of the index
    /// @return Zero; all elements in a rank-1 layout are on the "diagonal"
    ///
    /// @note This never returns 1, so slot 1 of the (size-2) span is unused; see the rank-1 @c is_exhaustive overload.
    template <typename FirstAccessIndex>
    static constexpr auto mapped_index(std::index_sequence<0>, FirstAccessIndex) noexcept -> index_type {
      return index_type{};
    }

    /// @brief The mapped index to return for a list of indices
    /// @tparam DimensionIndices The indices into the dimensions
    /// @tparam FirstAccessIndex The type of the first index value
    /// @tparam AccessIndices The type of the remaining index values
    /// @param first_access_index The first index value supplied
    /// @param access_indices The remaining index values supplied
    /// @return Zero if the specified access indices are on the "diagonal" (all equal, when added to the corresponding
    /// offsets), one otherwise
    template <std::size_t... DimensionIndices, typename FirstAccessIndex, typename... AccessIndices>
    constexpr auto mapped_index(
        std::index_sequence<0, DimensionIndices...>,
        FirstAccessIndex first_access_index,
        AccessIndices... access_indices
    ) const noexcept -> index_type {
      index_type const first_index{
          mdspan_detail::representable_cast<index_type>(this->offsets().extent(0U)) +
          mdspan_detail::representable_cast<index_type>(first_access_index)  //
      };
      return all_of(
                 {((mdspan_detail::representable_cast<index_type>(this->offsets().extent(DimensionIndices)) +
                    mdspan_detail::representable_cast<index_type>(access_indices)) == first_index)...}
             )
                 ? index_type{}
                 : index_type{1};
    }

   public:
    /// @brief Construct the mapping with default-constructed extents and offsets
    constexpr mapping() = default;

    /// @brief Construct the mapping with the specified extents
    /// @param source_extents the extents to use for this mapping
    explicit constexpr mapping(extents_type const& source_extents) noexcept
        : extents_base(source_extents),
          offsets_base{} {}

    /// @brief Construct the mapping with the specified extents and offsets
    /// @param source_extents the extents to use for this mapping
    /// @param offset_extents an extents object specifying the offsets to use for this mapping
    constexpr mapping(extents_type const& source_extents, offsets_type const& offset_extents) noexcept
        : extents_base(source_extents),
          offsets_base(offset_extents) {}

    /// @brief Default destructor
    ~mapping() = default;

    // parasoft-begin-suppress AUTOSAR-A12_8_6-a "False positive: this is not the base class of a hierarchy"
    // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
    /// @brief Default copy constructor
    mapping(mapping const&) = default;
    /// @brief Default move constructor
    mapping(mapping&&) = default;

    /// @brief Default copy assignment operator
    auto operator=(mapping const&) -> mapping& = default;
    /// @brief Default move constructor
    auto operator=(mapping&&) -> mapping& = default;
    // parasoft-end-suppress CERT_C-EXP37-a
    // parasoft-end-suppress AUTOSAR-A12_8_6-a

    /// @brief The required span size for the underlying storage.
    ///
    /// This is 1 for a zero-rank @c mdspan since there is exactly one element
    ///
    /// @return 1
    // parasoft-begin-suppress AUTOSAR-M9_3_3-a "Constrained overload of non-static member function"
    template <
        typename SfinaeExtentsType = ExtentsType,
        arene::base::constraints<
            std::enable_if_t<std::is_same<SfinaeExtentsType, ExtentsType>::value>,
            std::enable_if_t<SfinaeExtentsType::rank() == 0>> = nullptr>
    constexpr auto required_span_size() const noexcept -> index_type {
      return index_type{1};
    }
    // parasoft-end-suppress AUTOSAR-M9_3_3-a

    /// @brief The required span size for the underlying storage.
    ///
    /// This is 0 where any of the extents are statically zero
    /// @return 0
    // parasoft-begin-suppress AUTOSAR-M9_3_3-a "Constrained overload of non-static member function"
    template <
        typename SfinaeExtentsType = ExtentsType,
        arene::base::constraints<
            std::enable_if_t<std::is_same<SfinaeExtentsType, ExtentsType>::value>,
            std::enable_if_t<layout_constant_diagonal_detail::any_static_extents_are_zero<SfinaeExtentsType>()>,
            std::enable_if_t<SfinaeExtentsType::rank() != 0>> = nullptr>
    constexpr auto required_span_size() const noexcept -> index_type {
      return index_type{};
    }
    // parasoft-end-suppress AUTOSAR-M9_3_3-a

    /// @brief The required span size for the underlying storage.
    ///
    /// This is 2 where all extents are statically non-zero
    /// @return 2
    // parasoft-begin-suppress AUTOSAR-M9_3_3-a "Constrained overload of non-static member function"
    template <
        typename SfinaeExtentsType = ExtentsType,
        arene::base::constraints<
            std::enable_if_t<std::is_same<SfinaeExtentsType, ExtentsType>::value>,
            std::enable_if_t<!layout_constant_diagonal_detail::any_static_extents_are_zero<SfinaeExtentsType>()>,
            std::enable_if_t<SfinaeExtentsType::rank_dynamic() == 0>,
            std::enable_if_t<SfinaeExtentsType::rank() != 0>> = nullptr>
    constexpr auto required_span_size() const noexcept -> index_type {
      return layout_constant_diagonal_detail::max_required_span_size<index_type>;
    }
    // parasoft-end-suppress AUTOSAR-M9_3_3-a

    /// @brief The required span size for the underlying storage.
    ///
    /// This is 0 or 2 for a non-zero-rank @c mdspan : zero where any of the extents are zero, 2 otherwise (on diagonal
    /// and off-diagonal)
    /// @return 0 if any extent is zero, 2 otherwise
    template <
        typename SfinaeExtentsType = ExtentsType,
        arene::base::constraints<
            std::enable_if_t<std::is_same<SfinaeExtentsType, ExtentsType>::value>,
            std::enable_if_t<!layout_constant_diagonal_detail::any_static_extents_are_zero<SfinaeExtentsType>()>,
            std::enable_if_t<SfinaeExtentsType::rank_dynamic() != 0>,
            std::enable_if_t<SfinaeExtentsType::rank() != 0>> = nullptr>
    constexpr auto required_span_size() const noexcept -> index_type {
      if (layout_constant_diagonal_detail::any_extents_are_zero(extents())) {
        return index_type{};
      }
      return layout_constant_diagonal_detail::max_required_span_size<index_type>;
    }

    /// @brief Check if this layout is exhaustive
    ///
    /// For rank zero layouts, this is always true
    ///
    /// @return @c true if exhaustive, @c false otherwise
    // parasoft-begin-suppress AUTOSAR-M9_3_3-a "Constrained overload of non-static member function"
    template <
        typename SfinaeExtentsType = ExtentsType,
        arene::base::constraints<
            std::enable_if_t<std::is_same<SfinaeExtentsType, ExtentsType>::value>,
            std::enable_if_t<SfinaeExtentsType::rank() == 0>> = nullptr>
    constexpr auto is_exhaustive() const noexcept -> bool {
      return true;
    }
    // parasoft-end-suppress AUTOSAR-M9_3_3-a

    /// @brief Check if this layout is exhaustive
    ///
    /// This is always @c true for extents that have a dimension with a static extent of zero
    ///
    /// @return @c true if exhaustive, @c false otherwise
    // parasoft-begin-suppress AUTOSAR-M9_3_3-a "Constrained overload of non-static member function"
    template <
        typename SfinaeExtentsType = ExtentsType,
        arene::base::constraints<
            std::enable_if_t<std::is_same<SfinaeExtentsType, ExtentsType>::value>,
            std::enable_if_t<layout_constant_diagonal_detail::any_static_extents_are_zero<SfinaeExtentsType>()>,
            std::enable_if_t<SfinaeExtentsType::rank() != 0>> = nullptr>
    constexpr auto is_exhaustive() const noexcept -> bool {
      return true;
    }
    // parasoft-end-suppress AUTOSAR-M9_3_3-a

    /// @brief Check if this layout is exhaustive
    ///
    /// For non-rank-zero layouts, this depends on the extents and offsets: if there is at least one dimension with an
    /// extent of two or more, no dimensions with an extent of zero, and there is at least one value of @c index for
    /// each dimension such that @c index+offset is the same for all dimensions
    ///
    /// @return @c true if exhaustive, @c false otherwise
    template <
        typename SfinaeExtentsType = ExtentsType,
        typename SfinaeOffsets = OffsetExtentsType,
        arene::base::constraints<
            std::enable_if_t<std::is_same<SfinaeExtentsType, ExtentsType>::value>,
            std::enable_if_t<!layout_constant_diagonal_detail::any_static_extents_are_zero<SfinaeExtentsType>()>,
            std::enable_if_t<SfinaeExtentsType::rank_dynamic() == 0>,
            std::enable_if_t<SfinaeOffsets::rank_dynamic() == 0>,
            std::enable_if_t<SfinaeExtentsType::rank() != 0>> = nullptr>
    constexpr auto is_exhaustive() const noexcept -> bool {
      constexpr bool result = layout_constant_diagonal_detail::is_exhaustive(
          mdspan_detail::to_array(OffsetExtentsType{}),
          mdspan_detail::to_array(ExtentsType{})
      );
      return result;
    }

    /// @brief Check if this layout is exhaustive
    ///
    /// For non-rank-zero layouts, this depends on the extents and offsets: if there is at least one dimension with an
    /// extent of two or more, no dimensions with an extent of zero, and there is at least one value of @c index for
    /// each dimension such that @c index+offset is the same for all dimensions
    ///
    /// @return @c true if exhaustive, @c false otherwise
    template <
        typename SfinaeExtentsType = ExtentsType,
        typename SfinaeOffsets = OffsetExtentsType,
        arene::base::constraints<
            std::enable_if_t<std::is_same<SfinaeExtentsType, ExtentsType>::value>,
            std::enable_if_t<!layout_constant_diagonal_detail::any_static_extents_are_zero<SfinaeExtentsType>()>,
            std::enable_if_t<(SfinaeExtentsType::rank_dynamic() != 0) || (SfinaeOffsets::rank_dynamic() != 0)>,
            std::enable_if_t<SfinaeExtentsType::rank() != 0>> = nullptr>
    constexpr auto is_exhaustive() const noexcept -> bool {
      return layout_constant_diagonal_detail::is_exhaustive(
          mdspan_detail::to_array(offsets()),
          mdspan_detail::to_array(this->extents())
      );
    }

    /// @brief Check if there is exactly one index combination for each target element
    ///
    /// This must be the case of it returns @c true, may or not be the case if it returns @c false ; identifying all the
    /// cases where it is @c true requires too much implementation complexity.
    ///
    /// @return @c true if conclusively so, @c false if not guaranteed
    // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: There is a return statement with a value"
    // parasoft-begin-suppress CERT_C-MSC37-a "False positive: There is a return statement with a value"
    // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: There is a return statement with a value"
    // parasoft-begin-suppress AUTOSAR-M9_3_3-a "False positive: This calls a non-static member function"
    constexpr auto is_unique() const noexcept -> bool {
      return required_span_size() < layout_constant_diagonal_detail::max_required_span_size<index_type>;
    }
    // parasoft-end-suppress AUTOSAR-A8_4_2-a
    // parasoft-end-suppress AUTOSAR-M9_3_3-a
    // parasoft-end-suppress CERT_C-MSC37-a
    // parasoft-end-suppress CERT_CPP-MSC52-a

    /// @brief Check if this is a strided layout, which it is not
    /// @return @c false
    static constexpr auto is_strided() noexcept -> bool { return false; }
    /// @brief Check if this is always a strided layout, which it is not
    /// @return @c false
    static constexpr auto is_always_strided() noexcept -> bool { return false; }

    /// @brief Get the stride for the provided dimension. Provided for interface compatibility only
    /// @pre This function cannot be legitimately called
    /// @return Never returns
    // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
    constexpr auto stride(rank_type) const noexcept -> index_type {
      ARENE_PRECONDITION(is_strided());
      return index_type{};
    }
    // parasoft-end-suppress CERT_C-EXP37-a

    /// @brief Check if this is always a unique layout.
    ///
    /// Identifying all the cases where this is @c true requires too much implementation complexity, so this function
    /// may return @c false even if the layout is always a unique layout.
    ///
    /// @return @c true if always unique, @c false if it may not be always unique
    static constexpr auto is_always_unique() noexcept -> bool {
      return (ExtentsType::rank() == 0U) || layout_constant_diagonal_detail::any_static_extents_are_zero<ExtentsType>();
    }

    /// @brief Check if this is always an exhaustive layout.
    ///
    /// This requires that all extents and offsets are static constants, and that the specific values give an exhaustive
    /// layout
    /// @return @c true if always unique, @c false otherwise
    static constexpr auto is_always_exhaustive() noexcept -> bool {
      return (ExtentsType::rank_dynamic() == 0U) && (OffsetExtentsType::rank_dynamic() == 0U) &&
             mapping{}.is_exhaustive();
    }

    /// @brief Get the index from the underlying storage to use for the specified combination of indices
    /// @tparam Indices The types of the indices
    /// @param indices The indices to retrieve the mapped index for
    /// @pre The number of elements in @c Indices must equal @c ExtentsType::rank()
    /// @return The mapped index, which is 0 for values on the diagonal, 1 otherwise.
    template <typename... Indices, constraints<std::enable_if_t<sizeof...(Indices) == ExtentsType::rank()>> = nullptr>
    constexpr auto operator()(Indices... indices) const noexcept -> index_type {
      return mapped_index(std::make_index_sequence<sizeof...(Indices)>(), indices...);
    }

    /// @brief Obtain the extents of the layout
    /// @return The stored @c ExtentsType object
    constexpr auto extents() const noexcept -> ExtentsType const& { return extents_base::get_value(extents_tag{}); }

    /// @brief Obtain the offsets of the layout
    /// @return The stored @c OffsetExtentsType object
    constexpr auto offsets() const noexcept -> OffsetExtentsType const& {
      return offsets_base::get_value(offsets_tag{});
    }

    /// @brief Equality comparison: two mappings are equal if they have the same offsets and extents
    /// @param lhs The first mapping
    /// @param rhs The second mapping
    /// @return @c true if the mappings are the same, @c false otherwise
    // parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "False positive: comparison operators are permitted"
    // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: This is an inline function used in multiple TUs"
    friend constexpr auto operator==(mapping const& lhs, mapping const& rhs) noexcept -> bool {
      return (lhs.extents() == rhs.extents()) && (lhs.offsets() == rhs.offsets());
    }
    // parasoft-end-suppress AUTOSAR-A0_1_3-a
    // parasoft-end-suppress AUTOSAR-A11_3_1-a
    // parasoft-end-suppress AUTOSAR-A7_1_5-a

    /// @brief Inequality comparison: two mappings are not equal if they have the different offsets or extents
    /// @param lhs The first mapping
    /// @param rhs The second mapping
    /// @return @c true if the mappings are not the same, @c false otherwise
    // parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "False positive: comparison operators are permitted"
    // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: This is an inline function used in multiple TUs"
    friend constexpr auto operator!=(mapping const& lhs, mapping const& rhs) noexcept -> bool { return !(lhs == rhs); }
    // parasoft-end-suppress AUTOSAR-A0_1_3-a
    // parasoft-end-suppress AUTOSAR-A11_3_1-a
    // parasoft-end-suppress AUTOSAR-A7_1_5-a
  };
};

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_LAYOUT_CONSTANT_DIAGONAL_HPP_
