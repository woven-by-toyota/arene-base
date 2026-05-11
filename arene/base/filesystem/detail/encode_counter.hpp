// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_DETAIL_ENCODE_COUNTER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_DETAIL_ENCODE_COUNTER_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/filesystem/.*"

#include <cstdint>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/span.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace filesystem {

namespace detail {
/// @brief Encode a 32-bit value as a 6-character base-64 character in the provided space.
/// @param counter The value to encode
/// @param space The destination
/// @pre Requires that the buffer is exactly the right size, else @c ARENE_PRECONDITION violation.
// NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
void encode_counter(std::uint32_t counter, span<arene::base::detail::character> const space) noexcept;

}  // namespace detail
}  // namespace filesystem
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_DETAIL_ENCODE_COUNTER_HPP_
