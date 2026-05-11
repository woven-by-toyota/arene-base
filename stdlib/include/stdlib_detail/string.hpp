// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_STRING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_STRING_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "arene/base/detail/raw_c_string.hpp"
#include "stdlib/include/stdlib_detail/allocator.hpp"
#include "stdlib/include/stdlib_detail/char_traits.hpp"

// IWYU pragma: private, include <string>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

/// @brief Forward declaration of the basic string type. No definition is provided.
/// @tparam CharT The character type
/// @tparam Traits The character traits type
/// @tparam Allocator The allocator type
template <class CharT, class Traits = char_traits<CharT>, class Allocator = allocator<CharT>>
class basic_string;

/// @brief Alias for the common case of a string with @c char characters, standard traits and standard allocator
using string = basic_string<arene::base::detail::character>;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_STRING_HPP_
