// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_OPEN_FLAGS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_OPEN_FLAGS_HPP_

#include <cstdint>

namespace arene {
namespace base {
namespace filesystem {

/// @brief An enumeration specifying choices for opening files
enum class open_flags : std::uint8_t {
  /// @brief Open in read-only mode
  read_only,
  /// @brief Open in read-write mode
  read_write
};

}  // namespace filesystem
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_OPEN_FLAGS_HPP_
