// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

#include "arene/base/filesystem/temporary_directory.hpp"

#include <sys/stat.h>

// parasoft-begin-suppress AUTOSAR-M18_0_4-a "Time function only used as a monotonic counter"
#include <ctime>
// parasoft-end-suppress AUTOSAR-M18_0_4-a

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <utility>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array.hpp"
#include "arene/base/compiler_support/expect.hpp"
#include "arene/base/contracts.hpp"
#include "arene/base/endian.hpp"
#include "arene/base/filesystem/detail/encode_counter.hpp"
#include "arene/base/filesystem/error_code.hpp"
#include "arene/base/result.hpp"
#include "arene/base/span.hpp"
#include "arene/base/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace arene {
namespace base {
namespace filesystem {

namespace {

/// @brief A class for recursively removing a directory and its contents
class recursive_directory_remover {
 public:
  /// @brief Recursively remove a directory specified with a file handle and path relative to its parent.
  /// @param handle A handle to the directory to remove
  /// @param relative_path The name of the directory to remove relative to its parent
  /// @param parent_fd The raw file descriptor for the parent directory
  /// @pre @c *this must not have any left over state from a prior failed removal
  /// @throws std::system_error If an error occurs that prevents the directory being removed.
  /// @throws std::runtime_error If traversal of a directory handle fails or there are too many directories to remove.
  void recursively_remove_directory(
      directory_handle handle,
      null_terminated_string_view const relative_path,  // NOLINT(readability-avoid-const-params-in-decls) rule M3-9-1
      directory_handle const& parent_fd
  );

 private:
  /// @brief The stack entry: a handle to the directory and its name
  struct dir_stack_entry {
    // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is a struct, its members should be public"
    /// @brief The handle to the directory
    directory_handle dir_handle;
    /// @brief The name of the directory relative to its parent
    null_terminated_string_view relative_path;
    // parasoft-end-suppress AUTOSAR-M11_0_1-a
  };

  /// @brief The maximum number of nested directories that can be removed in a
  /// single pass.
  static constexpr std::uint32_t max_recursive_dir_stack_size{4096U};

  /// @brief Add a new handle to the stack.
  /// @param dir The handle of the directory to add
  /// @param relative_path The name of the directory relative to its parent.
  /// @throws std::runtime_error If there are too many directories to remove.
  void push(directory_handle dir, null_terminated_string_view relative_path);

  /// @brief Check if the stack is empty.
  /// @return @c true if the stack is empty, @c false otherwise.
  auto empty() const noexcept -> bool { return dir_stack_index_ == empty_flag; }

  // parasoft-begin-suppress AUTOSAR-A3_1_5-a "False positive: these are intended to be inlined"

  /// @brief Get a reference to the last added entry
  /// @pre Requires that the stack is not empty
  /// @return A reference to the last entry
  auto back() noexcept -> dir_stack_entry& {
    ARENE_PRECONDITION(!empty());
    return dir_stack_[dir_stack_index_];
  }

  /// @brief Get a reference to the last-but-one added entry
  /// @pre Requires that the stack has at least two elements
  /// @return A reference to the last-but-one entry
  auto previous() noexcept -> dir_stack_entry& {
    ARENE_PRECONDITION(size() > 1U);
    return dir_stack_[dir_stack_index_ - 1U];
  }

  /// @brief Get the number of entries in the stack
  /// @return The number of entries
  auto size() const noexcept -> std::size_t { return dir_stack_index_ + 1U; }

  /// @brief Remove the last element from the stack.
  /// @pre Requires that the stack is not empty
  void pop() noexcept {
    ARENE_PRECONDITION(!empty());
    dir_stack_[dir_stack_index_].dir_handle = {};
    --dir_stack_index_;
  }

  // parasoft-end-suppress AUTOSAR-A3_1_5-a

  /// @brief Remove all the entries from the given directory, or add a subdirectory to
  /// the stack.
  /// @param dir_handle A handle to the directory to process
  /// @throws std::runtime_error If traversal of a directory handle fails.
  /// @throws std::system_error If an error occurs that prevents inspecting or removing a directory.
  void remove_dir_entries_or_add_to_stack(directory_handle& dir_handle);

  /// @brief The value of @c dir_stack_index_ when the stack is empty
  static constexpr std::size_t empty_flag{std::numeric_limits<std::size_t>::max()};

  /// @brief The array of entries
  array<dir_stack_entry, max_recursive_dir_stack_size> dir_stack_;
  /// @brief The index of the last element if there is one, @c empty_flag otherwise.
  std::size_t dir_stack_index_{empty_flag};
};

/// @brief Recursively remove a directory specified with a file handle and path relative to its parent.
/// @param handle A handle to the directory to remove
/// @param relative_path The name of the directory to remove relative to its parent
/// @param parent_fd The raw file descriptor for the parent directory
/// @pre @c *this must not have any left over state from a prior failed removal
/// @throws std::system_error If an error occurs that prevents the directory being removed.
/// @throws std::runtime_error If there are too many directories to remove.
void recursive_directory_remover::recursively_remove_directory(
    directory_handle handle,
    null_terminated_string_view const relative_path,
    directory_handle const& parent_fd
) {
  ARENE_PRECONDITION(empty());
  push(std::move(handle), relative_path);
  while (!empty()) {
    auto const& parent_dir_fd = (size() > 1U) ? previous().dir_handle : parent_fd;
    // parasoft-begin-suppress AUTOSAR-M3_4_1-a "False positive: 'dir_handle' must be captured here"
    auto& dir_handle = back().dir_handle;
    // parasoft-end-suppress AUTOSAR-M3_4_1-a

    auto unlink_result = parent_dir_fd.unlink_entry(back().relative_path, directory_handle::unlink_type::directory);
    if (unlink_result || (unlink_result.error().value() == ENOENT)) {
      pop();
    } else {
      auto const error = unlink_result.error().value();
      if ((error == EEXIST) || (error == ENOTEMPTY)) {
        remove_dir_entries_or_add_to_stack(dir_handle);
      } else {
        unlink_result.error().throw_error();
      }
    }
  }
}

/// @brief Add a new handle to the stack.
/// @param dir The handle of the directory to add
/// @param relative_path The name of the directory relative to its parent.
/// @throws std::runtime_error If the hard-coded recursion limit is reached while there are still directories to remove.
void recursive_directory_remover::push(directory_handle dir, null_terminated_string_view relative_path) {
  if (dir_stack_index_ == (max_recursive_dir_stack_size - 1U)) {
    throw std::runtime_error("Max recursive count reached removing directories");
  }
  ++dir_stack_index_;
  dir_stack_[dir_stack_index_] = {std::move(dir), relative_path};
}

/// @brief Remove all the entries from the given directory, or add a subdirectory to
/// the stack.
/// @param dir_handle A handle to the directory to process
/// @throws std::runtime_error If traversal of a directory handle fails.
/// @throws std::system_error If an error occurs that prevents inspecting or removing a directory.
void recursive_directory_remover::remove_dir_entries_or_add_to_stack(directory_handle& dir_handle) {
  if (!dir_handle.rewind()) {
    throw std::runtime_error("Unable to process directory");
  }
  while (auto entry = dir_handle.next_entry()) {
    null_terminated_string_view const entry_name{entry.value()};
    auto file_info = dir_handle.get_file_stats(entry_name);
    if (!file_info) {
      if (file_info.error().value() == ENOENT) {
        break;
      }
      file_info.error().throw_error();
    }
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    if (S_ISDIR(file_info->st_mode)) {
      auto new_dir_handle = dir_handle.open_subdirectory(entry_name);
      if (!new_dir_handle) {
        if (new_dir_handle.error().value() == ENOENT) {
          break;
        }
        new_dir_handle.error().throw_error();
      }
      push(std::move(*new_dir_handle), entry_name);
      break;
    }
    auto unlink_result = dir_handle.unlink_entry(entry_name, directory_handle::unlink_type::other);
    if ((!unlink_result) && (unlink_result.error().value() != ENOENT)) {
      auto const error = unlink_result.error().value();
      if ((error == EEXIST) || (error == ENOTEMPTY)) {
        break;
      }
      unlink_result.error().throw_error();
    }
  }
}

/// @brief Remove a directory and all its contents.
///
/// The directory to be removed is specified by the given file handle and file name, relative to the directory
/// identified by the parent handle.
/// @param handle A handle to the directory to be removed
/// @param relative_path The name of the directory to remove relative to the specified parent directory
/// @param parent_fd The file handle of the parent directory
/// @throws std::runtime_error If traversal of a directory handle fails or there are too many directories to remove.
/// @throws std::system_error if a filesystem error occurs.
void recursively_remove_directory(
    directory_handle handle,
    null_terminated_string_view const relative_path,
    directory_handle const& parent_fd
) {
  recursive_directory_remover dir_stack;

  dir_stack.recursively_remove_directory(std::move(handle), relative_path, parent_fd);
}

/// @brief Get the root path for the creation of temporary files.
/// @param root path to use if @c TMPDIR is not set
/// @return path_string If the environment variable @c TMPDIR is specified, the value, otherwise @c root
auto get_temporary_dir_root_path(path_string const root) -> path_string {
  path_string root_path;
  // parasoft-begin-suppress CERT_C-POS34-a "Environment variables are the only way provided to get this information"
  // parasoft-begin-suppress AUTOSAR-M18_0_3-d "Environment variables are the only way provided to get this information"
  // parasoft-begin-suppress CERT_C-CON33-a "This thread-unsafe function is only used in single-threaded test apps"
  // parasoft-begin-suppress AUTOSAR-M3_4_1-a "False positive: 'tmpdir' is declared as locally as possible"
  // parasoft-begin-suppress AUTOSAR-A27_0_4-d "This is a system API that always takes C-style strings"
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b "False positive: this API returns char*, we're not choosing the type"
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  if (char const* const tmpdir{getenv("TMPDIR")}) {
    root_path = tmpdir;
  } else {
    root_path = root;
  }
  return root_path;
  // parasoft-end-suppress CERT_C-POS34-a
  // parasoft-end-suppress AUTOSAR-M18_0_3-d
  // parasoft-end-suppress CERT_C-CON33-a
  // parasoft-end-suppress AUTOSAR-M3_4_1-a
  // parasoft-end-suppress AUTOSAR-A27_0_4-d
  // parasoft-end-suppress AUTOSAR-A3_9_1-b
}

/// @brief Open a directory handle to the given path, throwing an exception on failure
/// @param root_path Path to a directory to open
/// @return A @c directory_handle to @c root_path if it could be opened
/// @throws std::runtime_error If traversal of a directory handle fails.
/// @throws std::system_error If an error occurs that prevents inspecting or modifying a directory.
auto open_directory_or_throw(path_string const root_path) -> directory_handle {
  auto open_result = directory_handle::open(root_path);

  if (open_result.has_error()) {
    if (open_result.error().value() == ENOENT) {
      throw std::runtime_error("Parent directory missing");
    }
    open_result.error().throw_error();
  }

  return std::move(*open_result);
}
}  // namespace

// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "Everything is documented in the header file"

/// @brief Create a directory with a unique name under the current root.
/// @post relative_path_ contains the name of the directory.
/// @post handle_ contains a handle to the directory.
/// @throws std::system_error if an error occurs.
void temporary_directory::create_uniquely_named_directory() {
  constexpr string_view prefix{"arene_temp"};
  relative_path_ = prefix;
  relative_path_ += "XXXXXX";
  // parasoft-begin-suppress CERT_C-MSC33-a "'time' function is only being used as a monotonic counter"
  std::uint32_t counter{static_cast<std::uint32_t>(time(nullptr))};
  // parasoft-end-suppress CERT_C-MSC33-a
  counter = byte_swap(counter);
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b "False positive: these are the characters of a string, not numbers"
  auto counter_buffer = span<char>{relative_path_.data(), relative_path_.size()}.subspan(prefix.length());
  // parasoft-end-suppress AUTOSAR-A3_9_1-b
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while)
  do {
    detail::encode_counter(counter, counter_buffer);
    if (auto create_result = parent_handle_.create_subdirectory(relative_path_)) {
      if (auto handle = parent_handle_.open_subdirectory(relative_path_)) {
        handle_ = std::move(*handle);
      }
    } else if (create_result.error().value() == EEXIST) {
      ++counter;
    } else {
      create_result.error().throw_error();
    }
  } while (!handle_.valid());
}

/// @brief Create a new temporary directory with a unique name.
/// @param root parent directory which will contain the temporary directory
/// @throws std::system_error If an error occurs that prevents inspecting or modifying a directory.
///
/// If the @c TMPDIR environment variable is set, the temporary directory is created under the specified directory,
/// otherwise it is created under @c root
temporary_directory::temporary_directory(path_string root)
    : relative_path_{},
      path_{get_temporary_dir_root_path(root)},
      parent_handle_{open_directory_or_throw(path_)},
      handle_{} {
  ARENE_INVARIANT(!path_.empty());

  create_uniquely_named_directory();
  if (path_.back() != '/') {
    std::ignore = path_.append('/');
  }
  std::ignore = path_.append(relative_path_);
}

/// @brief Remove the temporary directory and all its contents
temporary_directory::~temporary_directory() {
  try {
    recursively_remove_directory(std::move(handle_), relative_path_, parent_handle_);
  } catch (std::system_error& e) {
    std::cerr << "Filesystem error cleaning up temp dir: " << e.what() << std::endl;
  } catch (std::runtime_error& e) {
    std::cerr << "Error cleaning up temp dir: " << e.what() << std::endl;
  }
}

}  // namespace filesystem
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a
