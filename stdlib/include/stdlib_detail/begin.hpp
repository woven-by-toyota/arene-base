// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_BEGIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_BEGIN_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <iterator>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A5_0_3-a "False positive: there is only 1 level of pointer indirection"
/// @brief Obtain a pointer to the first element of the array.
/// @tparam Element The element type of the array
/// @tparam Size The size of the array
/// @param array The array
/// @return A pointer to the first element of @c array
template <typename Element, size_t Size>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr auto begin(Element (&array)[Size]) noexcept -> Element* {
  return array;
}
// parasoft-end-suppress AUTOSAR-A5_0_3-a
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Obtain the result of calling @c cont.begin() on a container
/// @tparam Container The type of the container
/// @param cont The container object
/// @return The result of @c cont.begin()
template <typename Container>
constexpr auto begin(Container& cont) noexcept(noexcept(std::declval<Container&>().begin())) -> decltype(cont.begin()) {
  return cont.begin();
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Obtain the result of calling @c cont.begin() on a @c const container
/// @tparam Container The type of the container
/// @param cont The container object
/// @return The result of @c cont.begin()
template <typename Container>
constexpr auto begin(Container const& cont) noexcept(noexcept(std::declval<Container const&>().begin()))
    -> decltype(cont.begin()) {
  return cont.begin();
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Equivalent to invoking @c begin(cont) on a @c const container
/// @tparam Container The type of the container
/// @param cont The container object
/// @return The result of @c cont.begin()
// parasoft-begin-suppress AUTOSAR-A5_0_3 "False positive: No pointer indirection here"
template <typename Container>
constexpr auto cbegin(Container const& cont) noexcept(noexcept(::std::begin(std::declval<Container const&>())))
    -> decltype(::std::begin(cont)) {
  return ::std::begin(cont);
}
// parasoft-end-suppress AUTOSAR-A5_0_3
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_BEGIN_HPP_
