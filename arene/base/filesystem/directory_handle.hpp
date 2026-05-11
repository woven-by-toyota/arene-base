// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_DIRECTORY_HANDLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_DIRECTORY_HANDLE_HPP_

#include <dirent.h>
#include <sys/stat.h>  // IWYU pragma: keep

#include <cstdint>
#include <utility>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/filesystem/error_code.hpp"
#include "arene/base/filesystem/file_handle.hpp"
#include "arene/base/filesystem/open_flags.hpp"
#include "arene/base/result.hpp"
#include "arene/base/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

namespace filesystem {

/// @brief Flags to specify what to do when attempting to create a file that already exists
enum class create_flags : std::uint8_t {
  /// @brief If the file already exists fail the creation
  create_or_fail,
  /// @brief If the file already exists, truncate it and open it
  create_or_truncate,
  /// @brief If the file already exists, open the existing file without altering it
  create_or_open
};

// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "Delegating constructors would not reduce duplication"
/// @brief Class to wrap a directory handle.
class directory_handle {
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "Using an int for compatibility with OS functions"
  /// @brief The OS directory handle type
  using os_directory_handle = int;
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2
 public:
  /// @brief Construct a directory handle that doesn't refer to a directory
  directory_handle() noexcept
      : raw_handle_{file_handle::invalid_handle_value},
        handle_{nullptr} {}

  /// @brief Obtain a directory handle from a file handle to the directory.
  /// @param source_handle The file handle for the directory
  /// @throws std::system_error if an error occurs
  explicit directory_handle(file_handle source_handle);

  /// @brief Close the directory handle
  ~directory_handle();

  /// @brief Transfer ownership of a directory handle from @c source to @c *this.
  /// @param source The handle to transfer ownership from
  directory_handle(directory_handle&& source) noexcept
      : raw_handle_(std::exchange(source.raw_handle_, file_handle::invalid_handle_value)),
        // parasoft-begin-suppress CERT_CPP-EXP57-b-3 "False positive: This is not a pointer cast"
        handle_(std::exchange(source.handle_, nullptr)) {}
  // parasoft-end-suppress CERT_CPP-EXP57-b-3

  /// @brief Transfer ownership of a directory handle from @c source to @c *this.
  /// Closes any previous directory handle owned by @c *this
  /// @param source The handle to transfer ownership from
  auto operator=(directory_handle&& source) noexcept -> directory_handle&;
  /// @brief Check if the handle has a valid value
  /// @return @c true if the handle is valid, @c false otherwise
  auto valid() const noexcept -> bool { return raw_handle_ != file_handle::invalid_handle_value; }

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Not copyable
  directory_handle(directory_handle const&) = delete;
  /// @brief Not copyable
  auto operator=(directory_handle const&) -> directory_handle& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief Rewind the directory handle to the start of the directory.
  /// @return A @c result holding a value on success, otherwise a @c result
  /// holding an error indicating the cause of failure.
  auto rewind() noexcept -> result<void, error_code>;

  /// @brief Advance to the next directory entry and return it. Ignores "." and ".."
  /// entries. Returns an @c ERANGE error if there are no more entries.
  /// @return A @c result holding the name of the next entry on success,
  /// otherwise a @c result holding an error indicating the cause of failure. An
  /// error of @c ERANGE indicates that the end of the directory has been
  /// reached.
  auto next_entry() noexcept -> result<null_terminated_string_view, error_code>;

  /// @brief Create a directory as a subdirectory of the directory referenced by @c
  /// *this, with the given name.
  /// @param dir_name The name of the new directory to create
  /// @return A @c result with a value if the directory was created, otherwise
  /// a @c result holding an @c error_code identifying the cause for the
  /// failure.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto create_subdirectory(null_terminated_string_view const dir_name) const noexcept -> result<void, error_code>;

  /// @brief Open a handle to the directory with the specified absolute path or path
  /// relative to the current working directory.
  /// @param path The path of the directory to retrieve the handle for
  /// @return A @c directory_handle for the directory, or @c nullopt if the
  /// directory doesn't exist
  /// @return A @c result holding a handle to the directory if the directory
  /// could be opened, otherwise a @c result holding an @c error_code
  /// identifying the cause for the failure.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  static auto open(null_terminated_string_view const path) noexcept -> result<directory_handle, error_code>;

  /// @brief Open a handle to the directory with the specified name relative to the
  /// directory referenced by @c *this.
  /// @param relative_name The name of the directory to retrieve the handle for
  /// @return A @c result holding a handle to the subdirectory if the directory
  /// could be opened, otherwise a @c result holding an @c error_code
  /// identifying the cause for the failure.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto open_subdirectory(null_terminated_string_view const relative_name) const noexcept
      -> result<directory_handle, error_code>;

  /// @brief Get the file stats for the directory entry with the specified name
  /// relative to the directory referenced by @c *this.
  /// @param relative_name The name of the directory entry to check
  /// @return The file stats for the directory entry, or @c nullopt if the
  /// directory entry doesn't exist
  /// @return A @c result holding the file stats for the directory entry if
  /// they could be retrieved, otherwise a @c result holding an @c error_code
  /// identifying the cause for the failure.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto get_file_stats(null_terminated_string_view const relative_name) const noexcept
      -> result<struct stat, error_code>;

  /// @brief Flag type to indicate what is being unlinked
  enum class unlink_type : std::uint8_t {
    /// @brief A directory is being unlinked
    directory,
    /// @brief Something that is not a directory is being unlinked
    other
  };

  /// @brief Remove a directory entry with the specified name relative to the directory
  /// referenced by @c *this.
  /// @param relative_name The name of the directory entry to check
  /// @param type A flag indicating if the directory entry is thought to
  /// be a directory (@c unlink_type::directory), or not (@c unlink_type::other)
  /// @return A @c result with a value if the directory entry was removed,
  /// otherwise a @c result holding an @c error_code identifying the cause for
  /// the failure.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto unlink_entry(null_terminated_string_view const relative_name, unlink_type const type) const noexcept
      -> result<void, error_code>;
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Open a read-only handle to the file with the specified name relative to
  /// the directory referenced by @c *this.
  /// @param relative_name The name of the file to open
  /// @return A @c result holding a handle to the file if the file could be
  /// opened, otherwise a @c result holding an @c error_code identifying the
  /// cause for the failure.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto open_file(null_terminated_string_view const relative_name) const noexcept -> result<file_handle, error_code>;

  /// @brief Open a handle to the file with the specified name relative to the
  /// directory referenced by @c *this, with permissions specified by the @c
  /// flags
  /// @param relative_name The name of the file to open
  /// @param flags The flags to use for opening the file
  /// @return A @c result holding a handle to the file if the file could be
  /// opened, otherwise a @c result holding an @c error_code identifying the
  /// cause for the failure.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto open_file(null_terminated_string_view const relative_name, open_flags const flags) const noexcept
      -> result<file_handle, error_code>;

  /// @brief Create a file with the specified name relative to the directory referenced
  /// by @c *this and return a read-write handle to the new file. Fails if a
  /// file with the specified name already exists.
  /// @param relative_name The name of the file to create
  /// @return A @c result holding a handle to the file if the file could be
  /// created, otherwise a @c result holding an @c error_code identifying the
  /// cause for the failure.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto create_file(null_terminated_string_view const relative_name) const noexcept -> result<file_handle, error_code>;

  /// @brief Create a file or open an existing file with the specified name relative to
  /// the directory referenced by @c *this and return a read-write handle to the
  /// new file. The behaviour when there is already a file with the specified
  /// name is specified by the @c flags parameter.
  /// @param relative_name The name of the file to create
  /// @param flags The creation flags
  /// @return A @c result holding a handle to the file if the file could be
  /// created, otherwise a @c result holding an @c error_code identifying the
  /// cause for the failure.
  auto create_file(null_terminated_string_view relative_name, create_flags flags) const noexcept
      -> result<file_handle, error_code>;

  /// @brief Create a temporary file with no name (or a transient random name) in the
  /// directory referenced by @c *this and return a read-write handle to the new
  /// file. Fails if a temporary file could not be created.
  /// @return A @c result holding a handle to the file if the file could be
  /// created, otherwise a @c result holding an @c error_code identifying the
  /// cause for the failure.
  auto create_temporary_file() const noexcept -> result<file_handle, error_code>;

  /// @brief Rename a file or directory from its existing name relative to the
  /// directory referenced by @c *this to the specified new name relative to the
  /// directory referenced by @c *this.
  /// @param old_name The existing name of the file or directory
  /// @param new_name The desired name of the file or directory
  /// @return A @c result holding a value if the file or directory could be
  /// renamed, otherwise a @c result holding an @c error_code identifying the
  /// cause for the failure.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  auto rename(null_terminated_string_view const old_name, null_terminated_string_view const new_name) const noexcept
      -> result<void, error_code>;

 private:
  /// @brief Internal function to create a temporary file with no name in the
  /// directory referenced by @c *this and return a read-write handle to the new
  /// file. Fails if a temporary file could not be created or the OS does not provide the necessary facilities.
  /// @return A @c result holding a handle to the file if the file could be
  /// created, otherwise a @c result holding an @c error_code identifying the
  /// cause for the failure. The error code is @c ENOTSUP if the OS does not provide the necessary facilities.
  auto create_temporary_file_directly() const noexcept -> result<file_handle, error_code>;

  /// @brief The raw file handle for the directory
  os_directory_handle raw_handle_;
  /// @brief The directory handle itself
  DIR* handle_;
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2

}  // namespace filesystem
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_DIRECTORY_HANDLE_HPP_
