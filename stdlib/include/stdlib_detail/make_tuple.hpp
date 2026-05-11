// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAKE_TUPLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAKE_TUPLE_HPP_

// IWYU pragma: private, include <tuple>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "arene/base/type_traits/unwrap_reference.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"
// parasoft-begin-suppress AUTOSAR-A16_2_2-a "False positive: needed for tuple definition."
#include "stdlib/include/stdlib_detail/tuple.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace std {

// parasoft-begin-suppress CERT_C-EXP37-a "False-positive: All arguments are named"
/// @brief construct a tuple with types deduced from the provided arguments
/// @tparam Types types of the tuple members, deduced from @c args
/// @param args a pack of values used to initialize the tuple
/// @return A @c tuple with contents initialized using @c args
/// @note Tuple member types are deduced as value types, not references, unless
///       passed in as @c reference_wrapper instances
template <class... Types>
constexpr auto make_tuple(Types&&... args
) noexcept(noexcept(tuple<arene::base::unwrap_ref_decay_t<Types>...>(std::forward<Types>(args)...)))
    -> tuple<arene::base::unwrap_ref_decay_t<Types>...> {
  return tuple<arene::base::unwrap_ref_decay_t<Types>...>(std::forward<Types>(args)...);
}
// parasoft-end-suppress CERT_C-EXP37-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MAKE_TUPLE_HPP_
