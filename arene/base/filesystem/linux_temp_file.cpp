// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cerrno>
#include <utility>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/filesystem/directory_handle.hpp"
#include "arene/base/filesystem/error_code.hpp"
#include "arene/base/filesystem/file_handle.hpp"
#include "arene/base/result.hpp"
#include "arene/base/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "Everything is documented in the header file"

namespace arene {
namespace base {
namespace filesystem {

auto directory_handle::create_temporary_file_directly() const noexcept -> result<file_handle, error_code> {
  // parasoft-begin-suppress AUTOSAR-M5_0_21-a-2 "This is how the OS documentation intends these flags to be used"
  // parasoft-begin-suppress AUTOSAR-M5_0_4-a-2 "This is how the OS documentation intends these flags to be used"
  constexpr mode_t file_permissions{S_IRUSR | S_IWUSR};
  // parasoft-end-suppress AUTOSAR-M5_0_4-a-2
  // parasoft-end-suppress AUTOSAR-M5_0_21-a-2
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "openat uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "openat uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  constexpr null_terminated_string_view dot_path{"."};
  // parasoft-begin-suppress AUTOSAR-M5_0_21-a-2 "This is how the OS documentation intends these flags to be used"
  // parasoft-begin-suppress AUTOSAR-A15_1_5-a "False positive: openat does not throw"
  // parasoft-begin-suppress AUTOSAR-CERT_CPP-ERR59-a "False positive: openat does not throw"
  file_handle entry_handle{
      // NOLINTNEXTLINE(hicpp-vararg,hicpp-signed-bitwise)
      openat(raw_handle_, dot_path.c_str(), O_EXCL | O_TMPFILE | O_RDWR | O_CLOEXEC, file_permissions)
  };
  // parasoft-end-suppress AUTOSAR-CERT_CPP-ERR59-a
  // parasoft-end-suppress AUTOSAR-A15_1_5-a
  // parasoft-end-suppress AUTOSAR-M5_0_21-a-2
  if (entry_handle.valid()) {
    return {in_place_value, std::move(entry_handle)};
  }

  return {in_place_error, error_code::from_errno()};
}

}  // namespace filesystem
}  // namespace base
}  // namespace arene
