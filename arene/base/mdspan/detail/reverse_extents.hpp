// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_REVERSE_EXTENTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_REVERSE_EXTENTS_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/mdspan/.*"

#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/type_list/reverse.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief reverses the extent indices of an @c extents specialization
/// @tparam E specialization of @c extents
///
/// Given <c>extents<IndexType, I0, I1, ..., In></c>, provides the member
/// typedef @c type equal to <c>extents<IndexType, In, ..., I1, I0></c>.
template <class E>
class reverse_extents;

/// @brief specialization for @c arene::base::extents
/// @tparam IndexType the index type
/// @tparam Is the extent values
template <class IndexType, std::size_t... Is>
class reverse_extents<extents<IndexType, Is...>> {
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  /// @brief helper to unpack reversed integral constants back into extents
  /// @return @c extents with static extents set by @c Js::value...
  template <class... Js>
  static auto de_ice_into_extents(type_list<Js...>) -> extents<IndexType, Js::value...>;
  // parasoft-end-suppress

 public:
  /// @brief the reversed extents type
  using type =
      decltype(de_ice_into_extents(type_lists::reverse_t<type_list<std::integral_constant<std::size_t, Is>...>>{}));
};

/// @brief helper alias for @c reverse_extents
/// @tparam E specialization of @c extents
template <class E>
using reverse_extents_t = typename reverse_extents<E>::type;

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_REVERSE_EXTENTS_HPP_
