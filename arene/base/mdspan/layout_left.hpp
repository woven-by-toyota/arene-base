// parasoft-suppress CERT_C-EXP37-a AUTOSAR-A2_8_1-a  "False positive: there is no function named '()' here. Header
// defines layout_left types"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_LAYOUT_LEFT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_LAYOUT_LEFT_HPP_

// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/all_valid_submdspan_slice_types_for.hpp"
#include "arene/base/mdspan/detail/checked_math.hpp"
#include "arene/base/mdspan/detail/is_submdspan_layout_preserving.hpp"
#include "arene/base/mdspan/detail/layout_common.hpp"         // IWYU pragma: keep
#include "arene/base/mdspan/detail/strided_mapping_base.hpp"  // IWYU pragma: keep
#include "arene/base/mdspan/detail/submdspan_mapping_impl.hpp"
#include "arene/base/mdspan/submdspan_subextents_type.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace layout_detail {

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

}  // namespace layout_detail

// parasoft-begin-suppress AUTOSAR-A2_10_1 "Base class is inherited privately so its identifiers are not visible"
// parasoft-begin-suppress AUTOSAR-A7_3_1-a "False positive: there is no rule A7-3-1"
// parasoft-begin-suppress AUTOSAR-A10_2_1-b "False positive: this is not a multiple inheritance hierarchy"
// parasoft-begin-suppress AUTOSAR-A10_2_1-a "False positive: this is not a multiple inheritance hierarchy"
/// @brief A mapping from a logical pack of indices into a single flat physical output index
/// @tparam The extents of the space used for this mapping; must be a specialization of @c extents
template <class Extents>
class layout_left::mapping : private layout_detail::strided_mapping_base<Extents> {
 private:
  /// @brief Convenience typedef referring to the base strided mapping
  using base_type = layout_detail::strided_mapping_base<Extents>;

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
      : base_type(exts, layout_detail::left_strides(exts)) {}

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

 private:
  /// @brief obtain a check_bypasser passkey to skip precondition checks on
  ///   mapping construction
  /// @return check_bypasser value
  ///
  /// @note This exists as a workaround for a GCC bug where access checking for
  ///   private constructors of friend classes can be incorrectly performed in
  ///   the caller's instantiation context rather than the definition context.
  static constexpr auto passkey() noexcept -> layout_detail::check_bypasser { return layout_detail::check_bypasser{}; }

  /// @brief Access the strides of a @c mapping via the derived class rather than its private base
  /// @param src the @c mapping to read strides from
  /// @return @c src 's strides
  ///
  /// @note This exists as a workaround for a GCC bug (present on gcc8) where access checking for a
  ///   hidden friend's trailing return type incorrectly treats the mapping's private base class as
  ///   inaccessible when the friend is found via ADL through a separate SFINAE context
  ///   (e.g. @c is_invocable_v inside @c arene::base::invoke ). Routing the call through this
  ///   static member performs the access check on the derived class, which GCC handles correctly,
  ///   just like the @c passkey() workaround above.
  static constexpr auto strides_of(mapping const& src) noexcept
      -> ::arene::base::array<index_type, extents_type::rank()> {
    return src.strides();
  }

 public:
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"

  /// @brief Compute the sub-mapping for a @c layout_left::mapping
  /// @tparam CanonicalSliceSpecifiers types of the slice specifiers, one per dimension
  /// @param src the source mapping to slice
  /// @param slices the slice specifiers
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
  ///   * the @c layout_left specialization of @c submdspan_mapping produces a @c layout_left::mapping if:
  ///     * for each k in the range <c> [0, SubExtents::rank() - 1) </c>, @c CanonicalSliceSpecifiers...[k] denotes @c
  ///     full_extent_t; and
  ///     * for k equal to <c> SubExtents::rank() - 1 </c>, @c CanonicalSliceSpecifiers...[k] is a unit-stride slice
  ///     type;
  ///   * Otherwise, it produces a @c layout_stride::mapping.
  template <
      class... CanonicalSliceSpecifiers,
      constraints<
          std::enable_if_t<sizeof...(CanonicalSliceSpecifiers) == extents_type::rank()>,
          std::enable_if_t<
              mdspan_detail::all_valid_submdspan_slice_types_for_v<extents_type, CanonicalSliceSpecifiers...>>> =
          nullptr>
  friend constexpr auto submdspan_mapping(mapping const& src, CanonicalSliceSpecifiers... slices) noexcept
      -> decltype(mdspan_detail::submdspan_mapping_impl<
                  submdspan_subextents_type_t<extents_type, CanonicalSliceSpecifiers...>>(
          mdspan_detail::is_submdspan_layout_preserving<
              layout_left,
              submdspan_subextents_type_t<extents_type, CanonicalSliceSpecifiers...>::rank(),
              CanonicalSliceSpecifiers...>{},
          src,
          mapping::passkey(),
          mapping::strides_of(src),
          std::move(slices)...
      )) {
    return mdspan_detail::submdspan_mapping_impl<
        submdspan_subextents_type_t<extents_type, CanonicalSliceSpecifiers...>>(
        mdspan_detail::is_submdspan_layout_preserving<
            layout_left,
            submdspan_subextents_type_t<extents_type, CanonicalSliceSpecifiers...>::rank(),
            CanonicalSliceSpecifiers...>{},
        src,
        mapping::passkey(),
        src.strides(),
        std::move(slices)...
    );
  }

  // parasoft-end-suppress AUTOSAR-A11_3_1-a
};
// parasoft-end-suppress AUTOSAR-A2_10_1
// parasoft-end-suppress AUTOSAR-A7_3_1-a
// parasoft-end-suppress AUTOSAR-A10_2_1-b
// parasoft-end-suppress AUTOSAR-A10_2_1-a

/// @brief Specialization of @c is_sliceable_mapping_v indicating @c layout_left::mapping supports @c submdspan
/// @tparam Extents Any specialization of @c arene::base::extents
template <typename Extents>
extern constexpr bool submdspan_detail::is_sliceable_mapping_v<layout_left::mapping<Extents>> = true;

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_LAYOUT_LEFT_HPP_
