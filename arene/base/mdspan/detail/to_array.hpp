// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::mdspan_detail::to_array"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_TO_ARRAY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_TO_ARRAY_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/mdspan/detail/de_ice.hpp"
#include "arene/base/mdspan/detail/representable_cast.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/move.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object implementing to_array
class to_array_fn  //
{
  /// @brief helper for converting an @c extents to an @c array
  /// @tparam Extents extents IndexType
  /// @tparam Is index sequence used to access each extent value
  /// @param extents extents value to convert
  /// @return values converted to an @c array
  ///
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: Parameter name 'extents' is used"
  template <class Extents, std::size_t... Is>
  static constexpr auto impl(Extents const& extents, std::index_sequence<Is...>) noexcept  //
      -> array<typename Extents::index_type, Extents::rank()>                              //
  {
    // parasoft-begin-suppress AUTOSAR-M8_5_2-a-2 "False positive: correct initialization"
    return {extents.extent(Is)...};
    // parasoft-end-suppress AUTOSAR-M8_5_2-a-2
  }
  // parasoft-end-suppress AUTOSAR-A0_1_4-a

 public:
  /// @brief convert an @c extents to an @c array
  /// @tparam Extents extents IndexType
  /// @param extents extents value to convert
  /// @return @c array where the @c value_IndexType is <c> typename
  ///   Extents::index_type </c>, the size is @c Extents::rank(), and the value
  ///   at index @c i of the returned array is equal to @c extents.extent(i)
  ///
  template <class Extents>
  constexpr auto operator()(Extents const& extents) const noexcept  //
      -> array<typename Extents::index_type, Extents::rank()>       //
  {
    return impl(extents, std::make_index_sequence<Extents::rank()>{});
  }
};

/// @def arene::base::mdspan_detail::to_array
/// @copydoc arene::base::mdspan_detail::to_array_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(mdspan_detail::to_array_fn, to_array);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

/// @brief function object implementing to_array_of
/// @tparam IndexType desired array value type
template <class IndexType>
class to_array_of_fn  //
{
 public:
  static_assert(
      std::is_integral<IndexType>::value,
      "'to_array_of' is intended for use with 'mdspan' and is limited to use with integral types"
  );

  /// @brief creates an array by converting a parameter pack of integral-ish values
  /// @tparam Integrals integral IndexTypes convertible to @c IndexType
  /// @param integrals values with which to initialize an array
  /// @return @c array where the @c value_type is @c IndexType, the size is <c>
  ///   sizeof...(Integrals) </c>, and the value at index @c i of the
  ///   corresponding value in @c integrals
  ///
  /// @pre each element of @c integrals is representable in @c IndexType
  ///
  template <class... Integrals>
  constexpr auto operator()(Integrals... integrals) const noexcept  //
      -> array<IndexType, sizeof...(Integrals)>                     //
  {
    // parasoft-begin-suppress AUTOSAR-M8_5_2-a-2 "False positive: correct initialization"
    return {mdspan_detail::representable_cast<IndexType>(mdspan_detail::de_ice(std::move(integrals)))...};
    // parasoft-end-suppress AUTOSAR-M8_5_2-a-2
  }
};

/// @def arene::base::mdspan_detail::to_array_of
/// @tparam IndexType desired array value type
/// @copydoc arene::base::mdspan_detail::to_array_of_fn::operator()
template <class IndexType>
extern constexpr auto to_array_of = to_array_of_fn<IndexType>{};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_TO_ARRAY_HPP_
