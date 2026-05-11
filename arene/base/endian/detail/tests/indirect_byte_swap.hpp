// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_TESTS_INDIRECT_BYTE_SWAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_TESTS_INDIRECT_BYTE_SWAP_HPP_

#include "arene/base/stdlib_choice/cstdint.hpp"

namespace arene {
namespace base {
namespace testing {

// Indirect versions of arene::base::byte_swap to force non-constexpr evaluation
auto indirect_byte_swap(std::uint16_t val) -> std::uint16_t;
auto indirect_byte_swap(std::uint32_t val) -> std::uint32_t;
auto indirect_byte_swap(std::uint64_t val) -> std::uint64_t;

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_TESTS_INDIRECT_BYTE_SWAP_HPP_
