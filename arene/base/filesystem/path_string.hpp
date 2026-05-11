// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_PATH_STRING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_PATH_STRING_HPP_

#include <climits>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/inline_string.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace filesystem {

/// @brief An inline_string with storage large enough to hold a file path
using path_string = inline_string<PATH_MAX>;

/// @brief A raw buffer with storage large enough to hold a file path
using path_buffer = array<arene::base::detail::character, PATH_MAX>;

}  // namespace filesystem
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_PATH_STRING_HPP_
