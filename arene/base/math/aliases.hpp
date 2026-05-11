// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ALIASES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ALIASES_HPP_

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace arene {
namespace base {

/// @brief An alias to float.
using f32_t = float;

static_assert(
    sizeof(f32_t) == sizeof(std::uint32_t),              //
    "f32_t (aka float) is not 4 bytes on this platform"  //
);
static_assert(
    std::numeric_limits<f32_t>::is_iec559,
    "f32_t (aka float) does not meet IEC559/IEEE754 requirements on this platform"
);

/// @brief An alias to double.
using f64_t = double;

static_assert(
    sizeof(f64_t) == sizeof(std::uint64_t),               //
    "f64_t (aka double) is not 8 bytes on this platform"  //
);
static_assert(
    std::numeric_limits<f64_t>::is_iec559,
    "f64_t (aka double) does not meet IEC559/IEEE754 requirements on this platform"
);

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ALIASES_HPP_
