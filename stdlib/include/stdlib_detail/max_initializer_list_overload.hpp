// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAX_INITIALIZER_LIST_OVERLOAD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAX_INITIALIZER_LIST_OVERLOAD_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/contracts.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/initializer_list.hpp"
#include "stdlib/include/stdlib_detail/internal_relative_find.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-M17_0_3-a "This is an implementation of the standard library function"

/// @brief Returns the largest value in the @c std::initializer_list
/// @tparam T The type of the values in the @c std::initializer_list
/// @tparam Compare The type of the comparison function
/// @param list The list of values to compare
/// @param comp The comparison function which returns true if the first parameter is less than the second
/// @return Returns the largest value in @c list using the comparison function @c comp to compare the values
///
/// @pre <tt> list.size() > 0 </tt>
/// @pre <tt> std::is_copy_constructible<T>::value == true </tt>
/// @pre The expression <tt>comp(a, b)</tt> must be convertible to @c bool
///     for every argument @c a, @c b of type <tt> T const& </tt>.
template <
    class T,
    class Compare,
    arene::base::constraints<
        std::enable_if_t<std::is_copy_constructible_v<T>>,
        std::enable_if_t<arene::base::is_compare_v<Compare&, T const&>>> = nullptr>
constexpr auto max(
    initializer_list<T> list,
    Compare comp
) noexcept(noexcept(comp(std::declval<T&>(), std::declval<T&>())) && std::is_nothrow_copy_constructible_v<T>) -> T {
  ARENE_PRECONDITION(list.size() > std::size_t{0});
  return *std::internal::relative_find(list.begin(), list.end(), arene::base::algorithm_detail::flip(comp));
}

/// @brief Returns the largest value in the @c std::initializer_list
/// @tparam T The type of the values in the @c std::initializer_list
/// @param list The list of values to compare
/// @return Returns the largest value in @c list
///
/// @pre <tt> list.size() > 0 </tt>
/// @pre <tt> std::is_copy_constructible<T>::value == true </tt>
/// @pre The expression <tt>comp(a, b)</tt> must be convertible to @c bool
///     for every argument @c a, @c b of type <tt> T const& </tt>.
template <
    class T,
    arene::base::constraints<
        std::enable_if_t<std::is_copy_constructible_v<T>>,
        std::enable_if_t<
            arene::base::is_compare_v<decltype(arene::base::algorithm_detail::operator_less)&, T const&>>> = nullptr>
constexpr auto max(initializer_list<T> list
) noexcept(noexcept(std::max(list, arene::base::algorithm_detail::operator_less))) -> T {
  return max(list, arene::base::algorithm_detail::operator_less);
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
// parasoft-end-suppress AUTOSAR-M17_0_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAX_INITIALIZER_LIST_OVERLOAD_HPP_
