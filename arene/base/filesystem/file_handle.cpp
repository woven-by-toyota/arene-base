// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/file_handle.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstdio>  // parasoft-suppress AUTOSAR-M27_0_1-a-2 "Using <cstdio> as needed for SEEK_SET"
#include <limits>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/result.hpp"
#include "arene/base/span.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "Everything is documented in the header file"

namespace arene {
namespace base {
namespace filesystem {

constexpr file_handle::os_file_handle file_handle::invalid_handle_value;
namespace {
/// @brief The maximum permitted file position for @c read_at and @c write_at
constexpr std::uint64_t max_file_position{static_cast<std::uint64_t>(std::numeric_limits<::off_t>::max())};
}  // namespace

static_assert(
    sizeof(::off_t) == sizeof(std::uint64_t),
    "off_t is not 64bits, this might cause bugs with files > 2GB, "
    "please enforce large file support with _FILE_OFFSET_BITS=64 "
);

file_handle::~file_handle() {
  // There is nothing we can do to handle a failed close in a dtor
  static_cast<void>(close());
}

auto file_handle::read_at(span<byte> buffer, std::uint64_t const position) const noexcept
    -> result<span<byte>, error_code> {
  if (position > max_file_position) {
    return error_result(error_code{EOVERFLOW});
  }
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "pread uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "pread uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  ::ssize_t const read_size{::pread(fd_, buffer.data(), buffer.size(), static_cast<::off_t>(position))};

  if (read_size < 0) {
    return error_result(error_code::from_errno());
  }
  return value_result(buffer.subspan(0U, static_cast<std::size_t>(read_size)));
}

auto file_handle::write_at(span<byte const> data, std::uint64_t const position) const noexcept
    -> result<span<byte const>, error_code> {
  if (position > max_file_position) {
    return error_result(error_code{EOVERFLOW});
  }
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "pwrite uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "pwrite uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  ::ssize_t const write_size{::pwrite(fd_, data.data(), data.size(), static_cast<::off_t>(position))};

  if (write_size < 0) {
    return error_result(error_code::from_errno());
  }
  return value_result(data.subspan(static_cast<std::uint64_t>(write_size)));
}

auto file_handle::flush() const noexcept -> result<void, error_code> {
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "fsync uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "fsync uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  if (::fsync(fd_) != 0) {
    return error_result(error_code::from_errno());
  }
  return value_result();
}

auto file_handle::close() noexcept -> result<void, error_code> {
  if (fd_ != invalid_handle_value) {
    if (::close(fd_) != 0) {
      return error_result(error_code::from_errno());
    }
    fd_ = invalid_handle_value;
  }
  return value_result();
}

auto file_handle::size() const noexcept -> result<std::uint64_t, error_code> {
  struct stat info {};
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "fstat uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "fstat uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  if (::fstat(fd_, &info) != 0) {
    return error_result(error_code::from_errno());
  }
  if (info.st_size < 0) {
    return error_result(error_code{ERANGE});
  }
  return value_result(static_cast<std::uint64_t>(info.st_size));
}

auto file_handle::sequential_write(span<byte const> data) const noexcept -> result<span<byte const>, error_code> {
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "write uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "write uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  ::ssize_t const write_size{::write(fd_, data.data(), data.size())};

  if (write_size < 0) {
    return error_result(error_code::from_errno());
  }
  return value_result(data.subspan(static_cast<std::uint64_t>(write_size)));
}

auto file_handle::sequential_read(span<byte> buffer) const noexcept -> result<span<byte>, error_code> {
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "read uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "read uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  ::ssize_t const read_size{::read(fd_, buffer.data(), buffer.size())};

  if (read_size < 0) {
    return error_result(error_code::from_errno());
  }
  return value_result(buffer.subspan(0U, static_cast<std::size_t>(read_size)));
}

auto file_handle::sequential_seek(std::uint64_t const position) const noexcept -> result<void, error_code> {
  if (position > max_file_position) {
    return error_result(error_code{EOVERFLOW});
  }
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "lseek uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "lseek uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  ::off_t const current_position{::lseek(fd_, static_cast<::off_t>(position), SEEK_SET)};
  if (current_position < 0) {
    return error_result(error_code::from_errno());
  }
  return value_result();
}

}  // namespace filesystem
}  // namespace base
}  // namespace arene
