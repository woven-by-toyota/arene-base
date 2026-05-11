// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_TEMPORARY_DIRECTORY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_TEMPORARY_DIRECTORY_HPP_

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include <cstdint>
#include <utility>

#include "arene/base/filesystem/directory_handle.hpp"
#include "arene/base/filesystem/path_string.hpp"
#include "arene/base/inline_string.hpp"

namespace arene {
namespace base {
namespace filesystem {

/// @brief An RAII helper class to manage a temporary directory
///
/// The directory is created on construction, and recursively removed on destruction.
class temporary_directory {
 public:
  /// @brief Create a new temporary directory with a unique name.
  /// @param root parent directory which will contain the temporary directory
  /// @throws std::system_error If an error occurs that prevents inspecting or modifying a directory.
  ///
  /// If the @c TMPDIR environment variable is set, the temporary directory is created under the specified directory,
  /// otherwise it is created under @c root
  explicit temporary_directory(path_string root);

  /// @brief Create a new temporary directory with a unique name.
  ///
  /// If the @c TMPDIR environment variable is set, the temporary directory is created under the specified directory,
  /// otherwise it is created under @c /tmp
  temporary_directory()
      : temporary_directory{"/tmp"} {}

  /// @brief Remove the temporary directory and all its contents
  ~temporary_directory();

  /// @brief deleted copy constructor
  temporary_directory(temporary_directory const&) = delete;
  /// @brief deleted copy assignment operator
  auto operator=(temporary_directory const&) -> temporary_directory& = delete;

  /// @brief default move constructor
  temporary_directory(temporary_directory&&) = default;
  /// @brief default move assignment operator
  auto operator=(temporary_directory&&) -> temporary_directory& = default;

  /// @brief Get the full path of the temporary directory
  /// @return reference to the path string
  auto path() const noexcept -> path_string const& { return path_; }

 private:
  /// @brief Create a directory with a unique name under the current root.
  /// @post relative_path_ contains the name of the directory.
  /// @post handle_ contains a handle to the directory.
  /// @throws std::system_error if an error occurs.
  void create_uniquely_named_directory();

  /// @brief The length of the relative path segment used to construct the temporary directory name
  static constexpr std::uint32_t relative_path_length{16U};
  /// @brief The path of the temporary directory relative to the parent folder
  inline_string<relative_path_length> relative_path_;
  /// @brief The full path of the temporary directory
  path_string path_;
  /// @brief A handle to the parent directory
  directory_handle parent_handle_;
  /// @brief A handle to the temporary directory itself
  directory_handle handle_;
};

}  // namespace filesystem
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_TEMPORARY_DIRECTORY_HPP_
