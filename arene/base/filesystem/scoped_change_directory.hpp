// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_SCOPED_CHANGE_DIRECTORY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_SCOPED_CHANGE_DIRECTORY_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/filesystem/path_string.hpp"
#include "arene/base/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

namespace filesystem {

/// @brief A RAII helper to temporarily change the working directory of the process.
///
/// The working directory is saved on construction, before being set to the new path. It is then restored to the old
/// value on destruction.
///
/// @note Use of this on multiple threads is inherently racy as there is only one working directory for the whole
///       process.
class scoped_change_directory {
 public:
  /// @brief Save the working directory and set it to the new path
  /// @param new_path The new working directory
  /// @throws std::system_error if an error occurs setting or retrieving the working directory
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  explicit scoped_change_directory(null_terminated_string_view const new_path);

  /// @brief Restore the working directory to the value saved on construction if possible.
  ~scoped_change_directory();

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Not copyable
  scoped_change_directory(scoped_change_directory const&) = delete;
  /// @brief Not copyable
  auto operator=(scoped_change_directory const&) -> scoped_change_directory& = delete;
  /// @brief Not movable
  scoped_change_directory(scoped_change_directory&&) = delete;
  /// @brief Not movable
  auto operator=(scoped_change_directory&&) -> scoped_change_directory& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

 private:
  /// @brief Buffer for previous working directory
  path_buffer old_dir_{};
};

}  // namespace filesystem
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_SCOPED_CHANGE_DIRECTORY_HPP_
