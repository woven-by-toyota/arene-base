// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_DETAIL_IN_PLACE_ERROR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_DETAIL_IN_PLACE_ERROR_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/result/.*"

namespace arene {
namespace base {

/// @brief Tag type for constructing error results in place.
struct in_place_error_t {};

/// @brief Instance corresponding to name tag.
constexpr in_place_error_t in_place_error{};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_DETAIL_IN_PLACE_ERROR_HPP_
