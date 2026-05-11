// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_EXTENTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_EXTENTS_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/functional/not_fn.hpp"
#include "arene/base/mdspan/detail/deduced_static_extent.hpp"
#include "arene/base/mdspan/detail/extent_sequence.hpp"
#include "arene/base/mdspan/detail/extract_dynamic_extents_for.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/utility/safe_comparisons.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace extents_detail {

/// @brief helper to check that index type and extents are valid
/// @tparam IndexType the index type
/// @tparam Extents sequence of static extent values
///
/// Determines if @c IndexType and @c Extents are valid for a specialization of
/// @c extents. The value of this helper is @c true if:
/// * @c IndexType is a signed or unsigned integer type, and
/// * each element of @c Extents is either equal to @c dynamic_extent, or is
///   representable as a value of type @c IndexType
///
template <class IndexType, std::size_t... Extents>
static constexpr bool valid_extents_template_parameters_v{
    //
    std::is_integral<IndexType>::value &&     //
    !std::is_same<IndexType, bool>::value &&  //
    all_of_v<((Extents == dynamic_extent) || cmp_less_equal(Extents, std::numeric_limits<IndexType>::max()))...>};

}  // namespace extents_detail

// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "Multiple inheritance does not make the code any more difficult to
// maintain than a class having multiple data members"
// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: Delegating constructors are used where they can be"
// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: There is no template constructor"
// parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'extents' does not hide an identifier in 'tuple'"
/// @brief represents a multidimensional index space
/// @tparam IndexType the index type
/// @tparam Extents the static extents of each dimension
template <typename IndexType, std::size_t... Extents>
class extents
    : mdspan_detail::extent_sequence<Extents...>                                  //
    , array<IndexType, mdspan_detail::extent_sequence<Extents...>::rank_dynamic>  //
{
  /// @brief sequence of static extents
  using extent_sequence_type = mdspan_detail::extent_sequence<Extents...>;

  /// @brief array holding the dynamic extents, if any
  using dynamic_extents_storage_type = array<IndexType, extent_sequence_type::rank_dynamic>;

  /// @brief obtain the array holding the dynamic extents
  /// @return reference to the array holding dynamic extents
  constexpr auto dynamic_extents() const noexcept -> dynamic_extents_storage_type const&  //
  {
    return static_cast<dynamic_extents_storage_type const&>(*this);
  }

  /// @brief Helper trait to specify if copying from another @c extents object with a different index type and/or
  /// extents list should be explicit. The value is @c true if the constructor should be explicit, @c false otherwise.
  /// @tparam OtherIndexType The index type of the other @c extents object
  /// @tparam OtherExtentSequence The extents of the other @c extents object
  template <class OtherIndexType, class OtherExtentSequence>
  static constexpr bool copy_from_other_extents_is_explicit_v{
      cmp_less(std::numeric_limits<IndexType>::max(), std::numeric_limits<OtherIndexType>::max()) ||
      !mdspan_detail::extent_sequence_equal(
          extent_sequence_type{},
          OtherExtentSequence{},
          not_fn(mdspan_detail::left_static_extent_and_right_dynamic_extent{})
      )
  };

 public:
  static_assert(
      extents_detail::valid_extents_template_parameters_v<IndexType, Extents...>,
      "invalid template parameters for 'extents'"
  );

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: static members cannot hide members of mdspan"
  /// @brief The specified index type for the extents
  using index_type = IndexType;
  /// @brief An unsigned type for representing an index into a dimension
  using size_type = std::make_unsigned_t<index_type>;
  /// @brief The type of the rank
  using rank_type = typename extent_sequence_type::rank_type;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  using extent_sequence_type::rank;
  using extent_sequence_type::rank_dynamic;
  using extent_sequence_type::static_extent;

  /// @brief Construct an @c extents object with the specified list of extents
  /// @tparam OtherIndexType The type of the supplied extents
  /// @tparam N The number of supplied extents
  /// @param exts The supplied extents
  /// @pre @c OtherIndexType must be convertible to @c IndexType without throwing
  /// @pre Each of @c exts must be representable as a non-negative value of @c IndexType
  /// @pre The size of the span, @c N , must be equal to @c rank()
  /// @pre For each extent in @c Extents, if the corresponding value is not @c dynamic_extent, then the provided
  /// value, when converted to @c IndexType, must be the same as the specified value in @c Extents
  template <
      typename OtherIndexType,
      std::size_t N,
      constraints<
          std::enable_if_t<N == rank()>,
          std::enable_if_t<N != rank_dynamic()>,
          std::enable_if_t<std::is_convertible<OtherIndexType, IndexType>::value>,
          std::enable_if_t<std::is_nothrow_constructible<IndexType, OtherIndexType>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  explicit constexpr extents(span<OtherIndexType, N> exts) noexcept
      : extent_sequence_type{},
        dynamic_extents_storage_type{mdspan_detail::extract_dynamic_extents_for<extents>(exts)}  //
  {}

  /// @brief Construct an @c extents object with the specified list of extents
  /// @tparam OtherIndexType The type of the supplied extents
  /// @param exts The supplied extents
  /// @pre @c OtherIndexType must be convertible to @c IndexType without throwing
  /// @pre The size of the span must be equal to @c rank() or @c dynamic_rank()
  /// @pre Each of @c exts must be representable as a non-negative value of @c IndexType
  template <
      typename OtherIndexType,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexType, IndexType>::value>,
          std::enable_if_t<std::is_nothrow_constructible<IndexType, OtherIndexType>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr extents(span<OtherIndexType, rank_dynamic()> exts) noexcept
      : extent_sequence_type{},
        dynamic_extents_storage_type{mdspan_detail::extract_dynamic_extents_for<extents>(exts)}  //
  {}

  /// @brief Construct an @c extents object with the specified list of extents
  /// @tparam OtherIndexType The type of the supplied extents
  /// @param exts The supplied extents
  /// @pre @c OtherIndexType must be convertible to @c IndexType without throwing
  /// @pre The size of the array must be equal to @c rank() or @c dynamic_rank()
  /// @pre Each of @c exts must be representable as a non-negative value of @c IndexType
  /// @pre For each extent in @c Extents, if the corresponding value is not @c dynamic_extent, then the provided
  /// value, when converted to @c IndexType, must be the same as the specified value in @c Extents
  template <
      typename OtherIndexType,
      constraints<
          std::enable_if_t<std::is_convertible<OtherIndexType, IndexType>::value>,
          std::enable_if_t<std::is_nothrow_constructible<IndexType, OtherIndexType>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr extents(array<OtherIndexType, rank_dynamic()> const& exts) noexcept
      : extent_sequence_type{},
        dynamic_extents_storage_type{mdspan_detail::extract_dynamic_extents_for<extents>(exts)}  //
  {}

  /// @brief Construct an @c extents object with the specified list of extents
  /// @tparam OtherIndexType The type of the supplied extents
  /// @tparam N The number of supplied extents
  /// @param exts The supplied extents
  /// @pre @c OtherIndexType must be convertible to @c IndexType without throwing
  /// @pre The size of the array, @c N , must be equal to @c rank()
  /// @pre Each of @c exts must be representable as a non-negative value of @c IndexType
  /// @pre For each extent in @c Extents, if the corresponding value is not @c dynamic_extent, then the provided
  /// value, when converted to @c IndexType, must be the same as the specified value in @c Extents
  template <
      typename OtherIndexType,
      std::size_t N,
      constraints<
          std::enable_if_t<N == rank()>,
          std::enable_if_t<N != rank_dynamic()>,
          std::enable_if_t<std::is_convertible<OtherIndexType, IndexType>::value>,
          std::enable_if_t<std::is_nothrow_constructible<IndexType, OtherIndexType>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  explicit constexpr extents(array<OtherIndexType, N> const& exts) noexcept
      : extent_sequence_type{},
        dynamic_extents_storage_type{mdspan_detail::extract_dynamic_extents_for<extents>(exts)}  //
  {}

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: All parameters have a name"
  /// @brief Construct an @c extents object with the specified list of dynamic extents
  /// @tparam OtherIndexTypes The types of the supplied dynamic extents
  /// @param exts The supplied dynamic extents
  /// @pre Each of @c OtherIndexTypes must be convertible to @c IndexType without throwing
  /// @pre Each of @c exts must be representable as a non-negative value of @c IndexType
  template <
      typename... OtherIndexTypes,
      constraints<
          std::enable_if_t<sizeof...(OtherIndexTypes) == rank_dynamic()>,
          std::enable_if_t<all_of_v<std::is_convertible<OtherIndexTypes, IndexType>::value...>>,
          std::enable_if_t<all_of_v<std::is_nothrow_constructible<IndexType, OtherIndexTypes>::value...>>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  explicit constexpr extents(OtherIndexTypes... exts) noexcept
      : extents{mdspan_detail::to_array_of<IndexType>(std::move(exts)...)}  //
  {}
  // parasoft-end-suppress CERT_C-EXP37-a

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: All parameters have a name"
  /// @brief Construct an @c extents object with the specified list of extents
  /// @tparam OtherIndexTypes The types of the supplied extents
  /// @param exts The supplied extents
  /// @pre Each of @c OtherIndexTypes must be convertible to @c IndexType without throwing
  /// @pre Each of @c exts must be representable as a non-negative value of @c IndexType
  /// @pre For each extent in @c Extents, if the corresponding value is not @c dynamic_extent, then the provided
  /// value, when converted to @c IndexType, must be the same as the specified value in @c Extents
  template <
      typename... OtherIndexTypes,
      constraints<
          std::enable_if_t<sizeof...(OtherIndexTypes) == rank()>,
          std::enable_if_t<sizeof...(OtherIndexTypes) != rank_dynamic()>,
          std::enable_if_t<all_of_v<std::is_convertible<OtherIndexTypes, IndexType>::value...>>,
          std::enable_if_t<all_of_v<std::is_nothrow_constructible<IndexType, OtherIndexTypes>::value...>>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  explicit constexpr extents(OtherIndexTypes... exts) noexcept
      : extents{mdspan_detail::to_array_of<IndexType>(std::move(exts)...)}  //
  {}
  // parasoft-end-suppress CERT_C-EXP37-a

  /// @brief Construct an @c extents object with the same set of extents as the source extents object
  /// @tparam OtherIndexType The index type of the supplied extents
  /// @tparam OtherExtents The supplied extents
  /// @param source_extents The @c extents object to copy the extents from
  /// @pre The other extents must match fixed extents, or be in range of @c IndexType for dynamic extents
  template <
      typename OtherIndexType,
      std::size_t... OtherExtents,
      constraints<
          std::enable_if_t<
              !copy_from_other_extents_is_explicit_v<OtherIndexType, mdspan_detail::extent_sequence<OtherExtents...>>>,
          std::enable_if_t<sizeof...(OtherExtents) == rank()>,
          std::enable_if_t<
              (Extents == OtherExtents) || (Extents == dynamic_extent) || (OtherExtents == dynamic_extent)>...> =
          nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr extents(extents<OtherIndexType, OtherExtents...> const& source_extents) noexcept
      : extents{mdspan_detail::extract_dynamic_extents_for<extents>(mdspan_detail::to_array(source_extents))}  //
  {}

  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameter 'source_extents' is used"
  /// @brief Construct an @c extents object with the same set of extents as the source extents object
  /// @tparam OtherIndexType The index type of the supplied extents
  /// @tparam OtherExtents The supplied extents
  /// @param source_extents The @c extents object to copy the extents from
  /// @pre The other extents must match fixed extents, or be in range of @c IndexType for dynamic extents
  template <
      typename OtherIndexType,
      std::size_t... OtherExtents,
      constraints<
          std::enable_if_t<
              copy_from_other_extents_is_explicit_v<OtherIndexType, mdspan_detail::extent_sequence<OtherExtents...>>>,
          std::enable_if_t<sizeof...(OtherExtents) == rank()>,
          std::enable_if_t<
              (Extents == OtherExtents) || (Extents == dynamic_extent) || (OtherExtents == dynamic_extent)>...> =
          nullptr>
  explicit constexpr extents(extents<OtherIndexType, OtherExtents...> const& source_extents) noexcept
      : extents{mdspan_detail::extract_dynamic_extents_for<extents>(mdspan_detail::to_array(source_extents))}  //
  {}
  // parasoft-end-suppress AUTOSAR-A0_1_4-a

  /// @brief Construct an @c extents object with all the dynamic extents set to zero, if any
  constexpr extents() noexcept
      // GCC earlier than 12 does not default initialize base classes correctly with '=default'
      : extents{array<index_type, rank_dynamic>{}}  //
  {}

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: member function cannot be 'static'"
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a CERT_CPP-MSC52-a-2 CERT_C-MSC37-a "False positive: this function returns a
  // value"
  /// @brief Get the actual extent for the given dimension, returning the extent supplied to the constructor for dynamic
  /// extents
  /// @param dimension The index of the dimension for which to get the extent
  /// @return The extent of the specified dimension
  constexpr auto extent(rank_type dimension) const noexcept -> index_type {
    auto const default_extent = extents::static_extent(dimension);
    if (default_extent == dynamic_extent) {
      // parasoft-begin-suppress AUTOSAR-M14_6_1-a-2 "Use of 'qualified-id::operator[]' hurts developer readability"
      return dynamic_extents()[extent_sequence_type::dynamic_index_map[dimension]];
      // parasoft-end-suppress AUTOSAR-M14_6_1-a-2
    }
    return static_cast<index_type>(default_extent);
  }
  // parasoft-end-suppress AUTOSAR-A8_4_2-a CERT_CPP-MSC52-a-2 CERT_C-MSC37-a
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  /// @brief Compare two extents objects with the same rank to see if they have the same values
  /// @tparam OtherIndexType The index type of the other extents type
  /// @tparam OtherExtents The extents of the other extents type
  /// @param lhs The first @c extents object to compare
  /// @param rhs The second @c extents object to compare
  /// @return @c true if  @c lhs.extent(index) is equal to @c rhs.extent(index) for all @c index values in @c
  /// [0,lhs.rank()) otherwise @c false.
  template <
      typename OtherIndexType,
      std::size_t... OtherExtents,
      constraints<std::enable_if_t<rank() == sizeof...(OtherExtents)>> = nullptr>
  friend constexpr auto operator==(extents const& lhs, extents<OtherIndexType, OtherExtents...> const& rhs) noexcept
      -> bool {
    for (rank_type index{}; index < rank(); ++index) {
      if (!cmp_equal(lhs.extent(index), rhs.extent(index))) {
        return false;
      }
    }
    return true;
  }
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  /// @brief Compare two extents objects with different ranks to see if they have the same values
  /// @tparam OtherIndexType The index type of the other extents type
  /// @tparam OtherExtents The extents of the other extents type
  /// @return @c false.
  template <
      typename OtherIndexType,
      std::size_t... OtherExtents,
      constraints<std::enable_if_t<rank() != sizeof...(OtherExtents)>> = nullptr>
  friend constexpr auto operator==(extents const&, extents<OtherIndexType, OtherExtents...> const&) noexcept -> bool {
    return false;
  }
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  /// @brief Compare two extents objects to see if they have different values
  /// @tparam OtherIndexType The index type of the other extents type
  /// @tparam OtherExtents The extents of the other extents type
  /// @param lhs The first @c extents object to compare
  /// @param rhs The second @c extents object to compare
  /// @return The inverse of @c lhs==rhs
  template <typename OtherIndexType, std::size_t... OtherExtents>
  friend constexpr auto operator!=(extents const& lhs, extents<OtherIndexType, OtherExtents...> const& rhs) noexcept
      -> bool {
    return !(lhs == rhs);
  }
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
};
// parasoft-end-suppress AUTOSAR-A14_5_1-a
// parasoft-end-suppress AUTOSAR-A12_1_5-a
// parasoft-end-suppress AUTOSAR-A10_1_1-a-2
// parasoft-end-suppress AUTOSAR-A2_10_1-e

namespace extents_detail {

/// @brief helper to determine the @c extents type with repeated
///   @c dynamic_extent s
/// @tparam IndexType @c extents index type
/// @tparam IndexSequence specialization of @c std::index_sequence
template <class IndexType, class IndexSequence>
class dextents_helper {};

/// @brief helper to determine the @c extents type with repeated
///   @c dynamic_extent s
/// @tparam IndexType @c extents index type
/// @tparam Is index sequence, values are ignored
template <class IndexType, std::size_t... Is>
class dextents_helper<IndexType, std::index_sequence<Is...>> {
 public:
  /// @brief specialization of @c extents
  using type = extents<IndexType, (static_cast<void>(Is), dynamic_extent)...>;
};

/// @brief helper to determine the @c extents type with repeated
///   @c dynamic_extent s
/// @tparam IndexType @c extents index type
/// @tparam Rank @c extents rank
///
/// @note This indirection is used to prevent IWYU from suggesting users of
///   @c dextents include a header for @c make_integer_sequence.
template <class IndexType, std::size_t Rank>
class dextents_helper<IndexType, std::integral_constant<std::size_t, Rank>>
    : public dextents_helper<IndexType, std::make_index_sequence<Rank>> {};

}  // namespace extents_detail

/// @brief Backport of @c std::dextents from C++23, a type representing the extents of a multidimensional array with all
/// dynamic extents
/// @tparam IndexType the index type
/// @tparam Rank The number of dimensions of the array
/// @pre @c IndexType must be an integral type other than @c bool
template <typename IndexType, std::size_t Rank>
using dextents = typename extents_detail::dextents_helper<IndexType, std::integral_constant<std::size_t, Rank>>::type;

/// @brief Helper variable to check if a provided type is an instantiation of @c extents
///
/// The value is @c true if the type is an instantiation of @c extents, @c false otherwise
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_extents_v = false;

/// @brief Helper variable to check if a provided type is an instantiation of @c extents
///
/// The value is @c true if the type is an instantiation of @c extents, @c false otherwise
/// @tparam IndexType the index type
/// @tparam Extents sequence of static extents
template <typename IndexType, std::size_t... Extents>
extern constexpr bool is_extents_v<extents<IndexType, Extents...>> = true;

namespace extents_detail {

/// @brief function object implementing make_extents
class make_extents_fn  //
{
 public:
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameter 'extents_values' is used"
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_C-MSC37-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: function does have a return"
  /// @brief Construct an @c extents object from the supplied extents values, using static extents if the arguments are
  /// instances of @c std::integral_constant and dynamic extents otherwise
  /// @tparam Integrals The types of the arguments
  /// @param extents_values The specified extents values
  /// @return An @c extents object with static extents with the specified values
  /// for every value in @c extents_values that is an instance of @c
  /// std::integral_constant, and dynamic extents for all other extents.
  template <typename... Integrals>
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters, also
  // they are named"
  constexpr auto operator()(Integrals&&... extents_values) const noexcept
      -> arene::base::extents<std::size_t, mdspan_detail::deduced_static_extent_v<remove_cvref_t<Integrals>>...> {
    return arene::base::extents<std::size_t, mdspan_detail::deduced_static_extent_v<remove_cvref_t<Integrals>>...>(
        std::forward<Integrals>(extents_values)...
    );
  }
  // parasoft-end-suppress CERT_C-EXP37-a
  // parasoft-end-suppress AUTOSAR-A8_4_2-a
  // parasoft-end-suppress CERT_C-MSC37-a
  // parasoft-end-suppress CERT_CPP-MSC52-a
  // parasoft-end-suppress AUTOSAR-A0_1_4-a
};
}  // namespace extents_detail

/// @def arene::base::make_extents
/// @copydoc arene::base::extents_detail::make_extents_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(extents_detail::make_extents_fn, make_extents);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_EXTENTS_HPP_
