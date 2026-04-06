// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/scoped_change_directory.hpp"

#include <unistd.h>

#include <type_traits>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/string_view.hpp"
#include "error_code.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace filesystem {

/// @brief Save the working directory and set it to the new path
/// @param new_path The new working directory
/// @throws std::system_error if an error occurs setting or retrieving the working directory
scoped_change_directory::scoped_change_directory(null_terminated_string_view const new_path) {
  if (getcwd(old_dir_.data(), old_dir_.size()) != old_dir_.data()) {
    error_code::from_errno().throw_error();
  }
  if (chdir(new_path.c_str()) != 0) {
    error_code::from_errno().throw_error();
  }
}

/// @brief Restore the old directory
scoped_change_directory::~scoped_change_directory() {
  // We cannot throw exception here because the implicitly-declared destructor/ is noexcept(true) in C++11.
  static_cast<void>(chdir(old_dir_.data()));
}

}  // namespace filesystem
}  // namespace base
}  // namespace arene
