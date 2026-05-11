// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/detail/encode_counter.hpp"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <utility>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/expect.hpp"
#include "arene/base/contracts.hpp"
#include "arene/base/math.hpp"
#include "arene/base/span.hpp"
#include "arene/base/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A15_4_5-a-2 "Everything is documented in the header file"

namespace arene {
namespace base {
namespace filesystem {
namespace detail {

void encode_counter(std::uint32_t counter, span<arene::base::detail::character> const space) noexcept {
  constexpr std::uint32_t base{64U};
  // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is compile-time so has no side effects"
  static_assert(is_power_of_2(base), "Must be power of 2 base");
  // parasoft-end-suppress CERT_C-PRE31-c-3
  constexpr std::size_t base_bits{log2(base)};
  constexpr std::size_t required_chars{
      (static_cast<std::size_t>(CHAR_BIT) * sizeof(std::uint32_t) + (static_cast<std::size_t>(CHAR_BIT) - 1U)) /
      base_bits
  };
  static constexpr string_view chars{"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-"};
  // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is compile-time so has no side effects"
  static_assert(chars.size() == base, "Must be 64 chars");
  // parasoft-end-suppress CERT_C-PRE31-c-3
  ARENE_PRECONDITION(space.size() == required_chars);
  for (std::size_t index{0U}; index < required_chars; ++index) {
    std::uint32_t const char_index{counter % base};
    space[index] = chars[char_index];
    counter /= base;
  }
}

}  // namespace detail
}  // namespace filesystem
}  // namespace base
}  // namespace arene
