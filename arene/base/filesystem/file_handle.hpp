// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_FILE_HANDLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_FILE_HANDLE_HPP_

#include <cstdint>
#include <utility>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/byte.hpp"
#include "arene/base/filesystem/error_code.hpp"
#include "arene/base/result.hpp"
#include "arene/base/span.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace filesystem {

// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "Delegating constructors would not reduce duplication"
/// @brief An RAII wrapper around a POSIX file handle with unique-ownership semantics.
class file_handle {
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "Using an int for compatibility with OS functions"
  /// @brief The OS file handle type
  using os_file_handle = int;
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property"
  /// @brief An invalid handle value
  static constexpr os_file_handle invalid_handle_value{-1};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  /// @brief Default construct with an invalid value
  /// @post <c> get_fd() == invalid_handle_value </c>
  file_handle() noexcept
      : fd_(invalid_handle_value) {}

  /// @brief Take ownership of the specified handle
  /// @param file_descriptor The file handle to own
  /// @post <c> get_fd() == file_description </c>
  explicit file_handle(os_file_handle const file_descriptor) noexcept
      : fd_(file_descriptor) {}

  /// @brief Close the handle if there is one
  /// @post If <c> valid() == true </c>, the file descriptor will be closed using underlying OS mechanisms.
  ~file_handle();

  /// @brief Transfer ownership of the handle from @c other to @c *this.
  /// @param other the other @c file_handle to take ownership from
  file_handle(file_handle&& other) noexcept
      : fd_(std::exchange(other.fd_, invalid_handle_value)) {}

  /// @brief Take ownership of the file handle from @c other.
  /// @param other the other @c file_handle to take ownership from
  /// @return @c *this
  /// @post If @c *this held a file handle before assignment, it is closed.
  /// @post @c get_fd() will return the value previously held by @c other
  auto operator=(file_handle&& other) noexcept -> file_handle& {
    file_handle temp{std::move(other)};
    std::swap(fd_, temp.fd_);
    return *this;
  }

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Not copyable
  file_handle(file_handle const&) = delete;
  /// @brief Not copyable
  auto operator=(file_handle const&) -> file_handle& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief Get the wrapped handle.
  /// @return the handle value.
  auto get_fd() const noexcept -> os_file_handle { return fd_; }

  /// @brief Release ownership of the handle, if held.
  /// @post <c> get_fd() == invalid_handle_value </c>.
  void release() noexcept { fd_ = invalid_handle_value; }

  /// @brief Check if the handle currently holds a file descriptor
  /// @return true if <c> get_fd() != invalid_handle_value </c>.
  /// @return false otherwise.
  auto valid() const noexcept -> bool { return fd_ != invalid_handle_value; }

  /// @brief Read from the file handle, starting at the specified position, into the provided buffer
  /// @param buffer A view onto a buffer to write the read bytes into into. The read will never be larger then
  ///             the provided span.
  /// @param position The position in the file to read from
  /// @return result<span<const byte>, error_code> A @c result holding either:
  ///          - A sub-span of @c buffer that is a view onto to the actual data read.
  ///          - An @c error_code indicating the reason for failure to read.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto read_at(span<byte> buffer, std::uint64_t const position) const noexcept -> result<span<byte>, error_code>;

  /// @brief Write data from the provided buffer to the file referenced by the handle, starting at the specified
  ///        position in the file.
  /// @param data The data to write
  /// @param position The position in the file to write at
  /// @return result<span<const byte>, error_code> A @c result holding either:
  ///          - A sub-span of @c data that refers to the remaining unwritten data for a successful write, if any.
  ///            If all data was written, this span will be empty.
  ///          - An @c error_code indicating the reason for failure to write.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto write_at(span<byte const> data, std::uint64_t const position) const noexcept
      -> result<span<byte const>, error_code>;

  /// @brief Read from the file handle, starting at the current sequential access position, into the provided buffer.
  ///        Updates the sequential access position to the end of the read data.
  /// @param buffer The buffer to read into. The read will never be larger then the provided buffer.
  /// @return result<span<byte>, error_code> A @c result holding either:
  ///          - A sub-span of @c buffer that is a view onto to the actual data read.
  ///          - An @c error_code indicating the reason for failure to read.
  auto sequential_read(span<byte> buffer) const noexcept -> result<span<byte>, error_code>;

  /// @brief Write data from the provided buffer to the file referenced by the handle, starting at the current
  ///        sequential access position. Updates the sequential access position to the end of the written data.
  /// @param data The data to write
  /// @return result<span<const byte>, error_code> A @c result holding either:
  ///          - A sub-span of @c data that refers to the remaining unwritten data for a successful write, if any.
  ///            If all data was written, this span will be empty.
  ///          - An @c error_code indicating the reason for failure to write.
  auto sequential_write(span<byte const> data) const noexcept -> result<span<byte const>, error_code>;

  /// @brief Move the sequential access position to the specified offset from the start of the file.
  /// @param position The position in the file to use for subsequent sequential access
  /// @return result<void, error_code> A @c result that is either:
  ///          - An empty value for success
  ///          - An @c error_code indicating the reason for failure if the seek failed.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto sequential_seek(std::uint64_t const position) const noexcept -> result<void, error_code>;

  /// @brief Flush the file handle
  /// @return result<void, error_code> A @c result that is either:
  ///          - An empty value for success
  ///          - An @c error_code indicating the reason for failure if the flush failed.
  auto flush() const noexcept -> result<void, error_code>;

  /// @brief Close the file handle, if there is one.
  /// @return result<void, error_code> A @c result that is either:
  ///          - An empty value for success.
  ///          - An @c error_code indicating the reason for failure if the handle could not be closed.
  auto close() noexcept -> result<void, error_code>;

  /// @brief Get the size of the open file
  /// @return A @c result holding the file size on success, or an @c error_code
  /// indicating the reason for failure.
  auto size() const noexcept -> result<std::uint64_t, error_code>;

 private:
  /// @brief The stored handle
  os_file_handle fd_;
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2

}  // namespace filesystem
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_FILE_HANDLE_HPP_
