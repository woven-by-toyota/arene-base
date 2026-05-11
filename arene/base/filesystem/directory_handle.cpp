// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/directory_handle.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdio>  // parasoft-suppress AUTOSAR-M27_0_1-a-2 "<cstdio> needed for renameat"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare.hpp"
#include "arene/base/contracts.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"
#include "arene/base/endian.hpp"
#include "arene/base/filesystem/detail/encode_counter.hpp"
#include "arene/base/filesystem/open_flags.hpp"
#include "arene/base/filesystem/path_string.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/result.hpp"
#include "arene/base/span.hpp"
#include "arene/base/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A15_4_5-a-2 "Everything is documented in the header file"

namespace arene {
namespace base {
namespace filesystem {

namespace {
/// @brief Check if the path is an acceptable relative path name. First, it must be a
/// non-empty path that does not begin with a leading slash (since that would
/// make it an absolute path). Then, it must not contain more ".." path segments
/// than the current path depth, since that would allow navigating outside the
/// subtree.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto is_valid_relative_path(null_terminated_string_view path) noexcept -> bool {
  if (path.empty() || (*path.begin() == '/')) {
    return false;
  }
  std::uint32_t down_count{0U};

  enum class states : std::uint8_t { first_char, one_dot, two_dots, general_segment };

  states state{states::first_char};

  // parasoft-begin-suppress AUTOSAR-A6_5_1-a-2 "False positive: null_terminated_string_view is not C++14 range"
  // NOLINTNEXTLINE(modernize-loop-convert)
  for (auto it = path.begin(); it != path.end(); ++it) {
    // parasoft-end-suppress AUTOSAR-A6_5_1-a-2
    arene::base::detail::character const current_char{*it};
    switch (state) {
      case states::first_char:
        if (current_char == '.') {
          state = states::one_dot;
        } else if (current_char == '/') {
          state = states::first_char;
        } else {
          state = states::general_segment;
        }
        break;
      case states::one_dot:
        if (current_char == '.') {
          state = states::two_dots;
        } else if (current_char == '/') {
          state = states::first_char;
        } else {
          state = states::general_segment;
        }
        break;
      case states::two_dots:
        if (current_char == '/') {
          if (down_count == 0U) {
            return false;
          }
          --down_count;
          state = states::first_char;
        } else {
          state = states::general_segment;
        }
        break;
      case states::general_segment:
        if (current_char == '/') {
          ++down_count;
          state = states::first_char;
        }
        break;
    }
  }
  if (state == states::two_dots) {
    return down_count != 0U;
  }
  return true;
}
}  // namespace

directory_handle::directory_handle(file_handle source_handle)
    : raw_handle_(source_handle.get_fd()),
      handle_(fdopendir(raw_handle_)) {
  if (handle_ == nullptr) {
    error_code::from_errno().throw_error();
  }
  source_handle.release();
}

directory_handle::~directory_handle() {
  if (handle_ != nullptr) {
    // There is nothing we can do to handle a failure here
    // parasoft-begin-suppress CERT_C-CON33-a "False positive: closedir is required to be safe in multithreaded context"
    static_cast<void>(closedir(handle_));
    // parasoft-end-suppress CERT_C-CON33-a
  }
}

auto directory_handle::operator=(directory_handle&& source) noexcept -> directory_handle& {
  directory_handle temp{std::move(source)};
  std::swap(temp.raw_handle_, raw_handle_);
  // parasoft-begin-suppress CERT_CPP-EXP57-b-2 "False positive: This is not a pointer cast"
  std::swap(temp.handle_, handle_);
  // parasoft-end-suppress CERT_CPP-EXP57-b-2
  return *this;
}

// parasoft-begin-suppress AUTOSAR-M9_3_3-a "False positive: Modifies the state"
auto directory_handle::rewind() noexcept -> result<void, error_code> {
  if (handle_ == nullptr) {
    return {in_place_error, EINVAL};
  }
  // parasoft-begin-suppress CERT_C-CON33-a "False positive: rewinddir is required to be safe in multithreaded context"
  rewinddir(handle_);
  // parasoft-end-suppress CERT_C-CON33-a
  return {};
}
// parasoft-end-suppress AUTOSAR-M9_3_3-a

// parasoft-begin-suppress AUTOSAR-M9_3_3-a "False positive: Modifies the state"
auto directory_handle::next_entry() noexcept -> result<null_terminated_string_view, error_code> {
  if (handle_ == nullptr) {
    return {in_place_error, EINVAL};
  }
  while (true) {
    // parasoft-begin-suppress AUTOSAR-M19_3_1-a "readdir uses errno to report errors"
    // parasoft-begin-suppress AUTOSAR-A17_1_1-a "readdir uses errno to report errors"
    errno = 0;
    // parasoft-end-suppress AUTOSAR-M19_3_1-a
    // parasoft-end-suppress AUTOSAR-A17_1_1-a
    // parasoft-begin-suppress CERT_C-CON33-a "False positive: readdir is required to be safe in multithreaded context"
    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    auto* const entry = readdir(handle_);
    // parasoft-end-suppress CERT_C-CON33-a
    if (entry == nullptr) {
      // parasoft-begin-suppress AUTOSAR-M19_3_1-a "readdir uses errno to report errors"
      // parasoft-begin-suppress AUTOSAR-A17_1_1-a "readdir uses errno to report errors"
      if (errno != 0) {
        // parasoft-end-suppress AUTOSAR-M19_3_1-a
        // parasoft-end-suppress AUTOSAR-A17_1_1-a
        return {in_place_error, error_code::from_errno()};
      }
      return {in_place_error, ERANGE};
    }
    null_terminated_string_view const entry_name{static_cast<arene::base::detail::character const*>(entry->d_name)};
    if (entry_name != "." && entry_name != "..") {
      return {in_place_value, entry_name};
    }
  }
}
// parasoft-end-suppress AUTOSAR-M9_3_3-a

auto directory_handle::create_subdirectory(null_terminated_string_view const dir_name) const noexcept
    -> result<void, error_code> {
  if (!is_valid_relative_path(dir_name)) {
    return {in_place_error, EINVAL};
  }
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "mkdirat uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "mkdirat uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  // parasoft-begin-suppress AUTOSAR-M5_0_21-a-2 "This is how the POSIX documentation intends these flags to be used"
  // parasoft-begin-suppress AUTOSAR-M5_0_4-a-2 "This is how the POSIX documentation intends these flags to be used"
  constexpr mode_t directory_permissions{S_IRWXU};
  // parasoft-end-suppress AUTOSAR-M5_0_4-a-2
  // parasoft-end-suppress AUTOSAR-M5_0_21-a-2
  if (mkdirat(raw_handle_, dir_name.c_str(), directory_permissions) != 0) {
    return {in_place_error, error_code::from_errno()};
  }
  return {};
}

auto directory_handle::open(null_terminated_string_view const path) noexcept -> result<directory_handle, error_code> {
  file_handle entry_handle{
      // parasoft-begin-suppress AUTOSAR-M5_0_21-a-2 "This is how the POSIX documentation intends these flags to be
      // used"
      // NOLINTNEXTLINE(hicpp-vararg,hicpp-signed-bitwise)
      openat(AT_FDCWD, path.c_str(), O_DIRECTORY | O_CLOEXEC)
      // parasoft-end-suppress AUTOSAR-M5_0_21-a-2
  };
  if (!entry_handle.valid()) {
    return {in_place_error, error_code::from_errno()};
  }
  return {in_place_value, std::move(entry_handle)};
}

auto directory_handle::open_subdirectory(null_terminated_string_view const relative_name) const noexcept
    -> result<directory_handle, error_code> {
  if (!is_valid_relative_path(relative_name)) {
    return {in_place_error, EINVAL};
  }
  file_handle entry_handle{
      // parasoft-begin-suppress AUTOSAR-M5_0_21-a-2 "This is how the POSIX documentation intends these flags to be
      // used"
      // NOLINTNEXTLINE(hicpp-vararg,hicpp-signed-bitwise)
      openat(raw_handle_, relative_name.c_str(), O_DIRECTORY | O_CLOEXEC)
      // parasoft-end-suppress AUTOSAR-M5_0_21-a-2
  };
  if (!entry_handle.valid()) {
    return {in_place_error, error_code::from_errno()};
  }
  return {in_place_value, std::move(entry_handle)};
}

auto directory_handle::get_file_stats(null_terminated_string_view const relative_name) const noexcept
    -> result<struct stat, error_code> {
  if (!is_valid_relative_path(relative_name)) {
    return {in_place_error, EINVAL};
  }
  struct stat file_info {};
  if (fstatat(raw_handle_, relative_name.c_str(), &file_info, AT_SYMLINK_NOFOLLOW) != 0) {
    return {in_place_error, error_code::from_errno()};
  }
  return {in_place_value, file_info};
}

// parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
auto directory_handle::unlink_entry(null_terminated_string_view const relative_name, unlink_type const type)
    const noexcept -> result<void, error_code> {
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2
  if (!is_valid_relative_path(relative_name)) {
    return {in_place_error, EINVAL};
  }
  if (unlinkat(raw_handle_, relative_name.c_str(), (type == unlink_type::directory) ? AT_REMOVEDIR : 0) != 0) {
    return {in_place_error, error_code::from_errno()};
  }
  return {};
}

auto directory_handle::open_file(null_terminated_string_view const relative_name) const noexcept
    -> result<file_handle, error_code> {
  return open_file(relative_name, open_flags::read_only);
}

namespace {
// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "Using an int for compatibility with OS functions"
/// @brief Type of file open/creation flags used for OS functions
using os_open_flags = int;
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
// parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
// parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
/// @brief Function to get the flag value to pass to @c openat based on the specified @c flags
/// @param flags The enum value specifying how the file should be opened
/// @return The value to pass to @c openat
auto get_open_flags(open_flags const flags) noexcept -> os_open_flags {
  // parasoft-begin-suppress AUTOSAR-M6_4_5-a-2 "All branches terminated as per permit M6-4-5#1"
  // parasoft-begin-suppress AUTOSAR-M6_4_3-a-2 "All branches terminated as per permit M6-4-3#1"
  switch (flags) {
    case open_flags::read_only:
      // parasoft-begin-suppress AUTOSAR-M5_0_21-a-2 "This is how the POSIX documentation intends these flags to be
      // used"
      // NOLINTNEXTLINE(hicpp-vararg,hicpp-signed-bitwise)
      return O_RDONLY | O_CLOEXEC;
    // parasoft-end-suppress AUTOSAR-M5_0_21-a-2
    case open_flags::read_write:
      // parasoft-begin-suppress AUTOSAR-M5_0_21-a-2 "This is how the POSIX documentation intends these flags to be
      // used"
      // NOLINTNEXTLINE(hicpp-vararg,hicpp-signed-bitwise)
      return O_RDWR | O_CLOEXEC;
    // parasoft-end-suppress AUTOSAR-M5_0_21-a-2
    default:
      ARENE_INVARIANT_UNREACHABLE("Invalid file open flags");
  }
  // parasoft-end-suppress AUTOSAR-M6_4_5-a-2
  // parasoft-end-suppress AUTOSAR-M6_4_3-a-2
}
// parasoft-end-suppress CERT_C-MSC37-a-2
// parasoft-end-suppress CERT_CPP-MSC52-a-2
// parasoft-end-suppress AUTOSAR-A8_4_2-a-2
}  // namespace

auto directory_handle::open_file(null_terminated_string_view const relative_name, open_flags const flags) const noexcept
    -> result<file_handle, error_code> {
  if (!is_valid_relative_path(relative_name)) {
    return {in_place_error, EINVAL};
  }
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "openat uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "openat uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  // NOLINTNEXTLINE(hicpp-vararg)
  file_handle entry_handle{openat(raw_handle_, relative_name.c_str(), get_open_flags(flags))};
  if (!entry_handle.valid()) {
    return {in_place_error, error_code::from_errno()};
  }

  return {in_place_value, std::move(entry_handle)};
}

auto directory_handle::create_file(null_terminated_string_view const relative_name) const noexcept
    -> result<file_handle, error_code> {
  return create_file(relative_name, create_flags::create_or_fail);
}

namespace {
// parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
// parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
// parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
/// @brief Helper function to obtain the flag value to pass to @c openat to create a file based on the specified @c
/// flags
/// @param flags The enumeration value specifying how to create the file
/// @return The value to pass to @c openat
auto get_creation_flags(create_flags const flags) noexcept -> os_open_flags {
  // parasoft-begin-suppress AUTOSAR-M5_0_21-a-2 "This is how the POSIX documentation intends these flags to be used"
  // NOLINTNEXTLINE(hicpp-vararg,hicpp-signed-bitwise)
  constexpr os_open_flags common_flags{O_CREAT | O_RDWR | O_CLOEXEC};
  // parasoft-end-suppress AUTOSAR-M5_0_21-a-2
  // parasoft-begin-suppress AUTOSAR-M6_4_5-a-2 "All branches terminated as per permit M6-4-5#1"
  // parasoft-begin-suppress AUTOSAR-M6_4_3-a-2 "All branches terminated as per permit M6-4-3#1"
  // parasoft-begin-suppress AUTOSAR-M5_0_21-a-2 "This is how the POSIX documentation intends these flags to be used"
  switch (flags) {
    case create_flags::create_or_fail:
      // NOLINTNEXTLINE(hicpp-vararg,hicpp-signed-bitwise)
      return common_flags | O_EXCL;
    case create_flags::create_or_truncate:
      // NOLINTNEXTLINE(hicpp-vararg,hicpp-signed-bitwise)
      return common_flags | O_TRUNC;
    case create_flags::create_or_open:
      return common_flags;
    default:
      ARENE_INVARIANT_UNREACHABLE("Invalid file creation flags");
  }
  // parasoft-end-suppress AUTOSAR-M5_0_21-a-2
  // parasoft-end-suppress AUTOSAR-M6_4_5-a-2
  // parasoft-end-suppress AUTOSAR-M6_4_3-a-2
}
// parasoft-end-suppress CERT_C-MSC37-a-2
// parasoft-end-suppress CERT_CPP-MSC52-a-2
// parasoft-end-suppress AUTOSAR-A8_4_2-a-2
}  // namespace

auto directory_handle::create_file(null_terminated_string_view relative_name, create_flags flags) const noexcept
    -> result<file_handle, error_code> {
  if (!is_valid_relative_path(relative_name)) {
    return {in_place_error, EINVAL};
  }
  // parasoft-begin-suppress AUTOSAR-M19_3_1-a "openat uses errno to report errors"
  // parasoft-begin-suppress AUTOSAR-A17_1_1-a "openat uses errno to report errors"
  errno = 0;
  // parasoft-end-suppress AUTOSAR-M19_3_1-a
  // parasoft-end-suppress AUTOSAR-A17_1_1-a
  // parasoft-begin-suppress AUTOSAR-M5_0_21-a-2 "This is how the POSIX documentation intends these flags to be used"
  // parasoft-begin-suppress AUTOSAR-M5_0_4-a-2 "This is how the POSIX documentation intends these flags to be used"
  constexpr mode_t file_permissions{S_IRUSR | S_IWUSR};
  // parasoft-end-suppress AUTOSAR-M5_0_4-a-2
  // parasoft-end-suppress AUTOSAR-M5_0_21-a-2
  // NOLINTNEXTLINE(hicpp-vararg)
  file_handle entry_handle{openat(raw_handle_, relative_name.c_str(), get_creation_flags(flags), file_permissions)};
  if (!entry_handle.valid()) {
    return {in_place_error, error_code::from_errno()};
  }

  return {in_place_value, std::move(entry_handle)};
}
auto directory_handle::rename(null_terminated_string_view const old_name, null_terminated_string_view const new_name)
    const noexcept -> result<void, error_code> {
  if ((!is_valid_relative_path(old_name)) || (!is_valid_relative_path(new_name))) {
    return {in_place_error, EINVAL};
  }
  if (renameat(raw_handle_, old_name.c_str(), raw_handle_, new_name.c_str()) != 0) {
    return {in_place_error, error_code::from_errno()};
  }
  return {};
}

// parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "False positive: every branch has a return"
// parasoft-begin-suppress CERT_CPP-MSC52-a-2 "False positive: every branch has a return"
// parasoft-begin-suppress CERT_C-MSC37-a-2 "False positive: every branch has a return"
auto directory_handle::create_temporary_file() const noexcept -> result<file_handle, error_code> {
  auto direct_file = create_temporary_file_directly();
  if (direct_file || (direct_file.error().value() != ENOTSUP)) {
    return direct_file;
  }
  // direct temp file not supported, create unique name
  constexpr string_view prefix{"arene_temp"};
  path_string relative_path{path_string{prefix} + "XXXXXX"};
  auto counter = static_cast<std::uint32_t>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
  counter = byte_swap(counter);
  auto counter_buffer = span<arene::base::detail::character>{relative_path}.subspan(prefix.length());
  while (true) {
    detail::encode_counter(counter, counter_buffer);
    auto create_result = create_file(relative_path);
    if (create_result) {
      auto unlink_result = unlink_entry(relative_path, unlink_type::other);
      if (!unlink_result) {
        return {in_place_error, unlink_result.error()};
      }
      return {in_place_value, std::move(create_result.value())};
    }
    if (create_result.error().value() != EEXIST) {
      return {in_place_error, create_result.error()};
    }
    ++counter;
  }
}
// parasoft-end-suppress CERT_C-MSC37-a-2
// parasoft-end-suppress CERT_CPP-MSC52-a-2
// parasoft-end-suppress AUTOSAR-A8_4_2-a-2

}  // namespace filesystem
}  // namespace base
}  // namespace arene
