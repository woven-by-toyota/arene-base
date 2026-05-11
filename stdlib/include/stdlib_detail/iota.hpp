// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IOTA_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IOTA_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <numeric>
// IWYU pragma: friend "stdlib_detail/.*"
//
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/algorithm/iota.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_convertible.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief fills a range with successive increments of the starting value
/// @tparam ForwardIt the type of the ForwardIt
/// @tparam Value the type of the value
/// @param first ForwardIt to the beginning of the range
/// @param last ForwardIt to one past the end of the range
/// @param value initial value to fill with
///
/// Fills the range <c> [first, last) </c> with sequentially increasing
/// values, starting with @c value and repetitively evaluating
/// <c> ++value </c>.
///
/// @pre @c ForwardIt must satisfy the forward iterator requirements
/// @pre @c T is convertible to the value type of @c ForwardIt
/// @pre the expression <c> ++value </c> is well-formed
///
/// @note Complexity <br>
///   Exactly <c> distance(first, last) </c> increments and assignments.
///
template <
    class ForwardIt,
    class Value,
    arene::base::constraints<
      enable_if_t<arene::base::is_forward_iterator_v<ForwardIt>>,
      enable_if_t<is_convertible_v<Value, arene::base::algorithm_detail::iter_value_t<ForwardIt>>>,
      decltype(++declval<Value&>())
    > = nullptr>
auto iota(ForwardIt first, ForwardIt last, Value value) noexcept(  //
    arene::base::is_nothrow_invocable_v<arene::base::iota_detail::iota_impl_fn, ForwardIt&, ForwardIt&, Value&>
) -> void {
  arene::base::iota_detail::iota_impl_fn{}(first, last, value);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IOTA_HPP_
