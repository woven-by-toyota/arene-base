// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_DYNAMIC_EXTENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_DYNAMIC_EXTENT_HPP_

// IWYU pragma: private, include "arene/base/span.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace arene {
namespace base {
/// @brief A special value for the Extent of a span to indicate that the span has dynamic extent
constexpr std::size_t dynamic_extent{std::numeric_limits<std::size_t>::max()};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_DYNAMIC_EXTENT_HPP_
