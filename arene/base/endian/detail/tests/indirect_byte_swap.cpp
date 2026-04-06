// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/endian/detail/tests/indirect_byte_swap.hpp"

#include "arene/base/endian/detail/byte_swap16_bswap16.hpp"
#include "arene/base/endian/detail/byte_swap32_bswap32.hpp"
#include "arene/base/endian/detail/byte_swap64_bswap64.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace arene {
namespace base {
namespace testing {

auto indirect_byte_swap(std::uint16_t val) -> std::uint16_t { return arene::base::byte_swap_detail::byte_swap(val); }

auto indirect_byte_swap(std::uint32_t val) -> std::uint32_t { return arene::base::byte_swap_detail::byte_swap(val); }

auto indirect_byte_swap(std::uint64_t val) -> std::uint64_t { return arene::base::byte_swap_detail::byte_swap(val); }

}  // namespace testing
}  // namespace base
}  // namespace arene
