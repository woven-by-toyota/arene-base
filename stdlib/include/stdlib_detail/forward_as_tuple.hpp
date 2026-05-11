// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_FORWARD_AS_TUPLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_FORWARD_AS_TUPLE_HPP_

// IWYU pragma: private, include <tuple>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "stdlib/include/stdlib_detail/forward.hpp"
// parasoft-begin-suppress AUTOSAR-A16_2_2-a "False positive: needed for tuple definition."
#include "stdlib/include/stdlib_detail/tuple.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: all parameters named"
/// @brief construct a tuple of references to the provided arguments suitable for forwarding as arguments to a function
/// call.
/// @tparam ArgTs types of the tuple members, deduced from @c args
/// @param args a pack of values used to initialize the tuple
/// @return std::tuple<Args&&...> A @c tuple constructed as if via @c std::tuple<Args&&...>(std::forward<Args>(args)...)
/// @note The tuple does not extend the lifetime of any temporaries passed to it. It is the caller's responsibility to
/// ensure that any references in the returned tuple remain valid for the lifetime of the tuple.
template <class... ArgTs>
constexpr auto forward_as_tuple(ArgTs&&... args) noexcept -> tuple<ArgTs&&...> {
  return tuple<ArgTs&&...>{::std::forward<ArgTs>(args)...};
}
// parasoft-end-suppress CERT_C-EXP37-a
// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_FORWARD_AS_TUPLE_HPP_
