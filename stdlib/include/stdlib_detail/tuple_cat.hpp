// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_CAT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_CAT_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <tuple>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/tuple_fwd.hpp"
#include "arene/base/tuple/tuple_cat.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace std {

namespace tuple_cat_detail {

/// @brief Check if a type is a std::tuple or a std::pair
/// @tparam Tuple The type to check
/// @return @c true if the @c Tuple is a @c std::tuple or @c std::pair otherwise @c false
template <class Tuple>
extern constexpr auto is_std_tuple_or_pair =
    arene::base::is_instantiation_of_v<arene::base::remove_cvref_t<Tuple>, std::tuple> ||
    arene::base::is_instantiation_of_v<arene::base::remove_cvref_t<Tuple>, std::pair>;

}  // namespace tuple_cat_detail

/// @brief concatenate tuples
/// @tparam Tuples types of the tuples to concatenate
/// @param tuples @c std::tuple objects to concatenate
/// @return @c std::tuple whose elements are the concatenation of @p tuples in order
///
/// @pre Every argument must be an @c std::tuple or @c std::pair
///   Passing non-@c std::tuple tuple-likes to @c std::tuple_cat is undefined
///   behavior per the standard; use @c arene::base::tuple_cat for those.
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
template <class... Tuples, class = decltype(arene::base::tuple_cat(std::declval<Tuples&&>()...))>
constexpr auto tuple_cat(Tuples&&... tuples
) noexcept(arene::base::is_nothrow_invocable_v<decltype(arene::base::tuple_cat), Tuples&&...>)
    -> decltype(arene::base::tuple_cat(std::forward<Tuples>(tuples)...)) {
  static_assert(
      arene::base::all_of_v<tuple_cat_detail::is_std_tuple_or_pair<Tuples>...>,
      "std::tuple_cat requires every argument to be a std::tuple or std::pair; "
      "use arene::base::tuple_cat to concatenate arbitrary tuple-like types."
  );
  return arene::base::tuple_cat(std::forward<Tuples>(tuples)...);
}
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

}  // namespace std

// parasoft-end-suppress AUTOSAR-A17_6_1-a-2
// parasoft-end-suppress CERT_CPP-DCL58-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_CAT_HPP_
