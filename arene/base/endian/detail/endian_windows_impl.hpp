// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_ENDIAN_WINDOWS_IMPL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_ENDIAN_WINDOWS_IMPL_HPP_

// IWYU pragma: private, include "arene/base/endian.hpp"
// IWYU pragma: friend "arene/base/endian/.*"

#include "arene/base/stdlib_choice/cstdint.hpp"

namespace arene {
namespace base {
/// @brief Enumeration for the possible byte-orders of system architectures.
///
/// The underlying type and enumeration values of this enumeration are platform specific.
///
/// Note that on mixed-endian architectures it's possible that @c native does not equal either @c little or @c big. So
/// don't assume that just because <tt>native != big</tt> that <tt>native == little</tt>. Matches the C++20
/// @c std::endian enumeration.
enum class endian : std::uint8_t {
  little,          //< The little-endian byte-order.
  big,             //< The big-endian byte-order.
  native = little  //< The native byte-order of the system architecture; Windows is always little-endian
};
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_ENDIAN_WINDOWS_IMPL_HPP_
