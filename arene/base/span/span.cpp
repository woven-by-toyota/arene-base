// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/span/span.hpp"

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace arene {
namespace base {
namespace span_detail {
/// @brief The @c size member for empty spans
constexpr std::integral_constant<std::size_t, 0> span_base_fixed_size_empty::size;
}  // namespace span_detail
}  // namespace base
}  // namespace arene
