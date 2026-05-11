// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_DETAIL_IN_PLACE_VALUE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_DETAIL_IN_PLACE_VALUE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/result/.*"

namespace arene {
namespace base {

/// @brief Tag type for constructing value results in place.
struct in_place_value_t {};

/// @brief Instance corresponding to name tag.
constexpr in_place_value_t in_place_value{};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_DETAIL_IN_PLACE_VALUE_HPP_
