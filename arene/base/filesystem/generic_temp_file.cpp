// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cerrno>

#include "arene/base/filesystem/directory_handle.hpp"

namespace arene {
namespace base {
namespace filesystem {

auto directory_handle::create_temporary_file_directly() const noexcept -> result<file_handle, error_code> {
  return {in_place_error, error_code{ENOTSUP}};
}

}  // namespace filesystem
}  // namespace base
}  // namespace arene
