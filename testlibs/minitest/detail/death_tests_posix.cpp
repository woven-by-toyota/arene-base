// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <errno.h>   // NOLINT(hicpp-deprecated-headers) This is used to test a standard library so it can't use std
#include <stdio.h>   // NOLINT(hicpp-deprecated-headers) This is used to test a standard library so it can't use std
#include <string.h>  // NOLINT(hicpp-deprecated-headers) This is used to test a standard library so it can't use std
#include <sys/wait.h>
#include <unistd.h>

#include "testlibs/minitest/assertion_strings.hpp"
#include "testlibs/minitest/cstring.hpp"
#include "testlibs/minitest/death_tests_enabled.hpp"
#include "testlibs/minitest/libc.hpp"

namespace testing {

/// @brief Forward declaration of the assertion failure function from minitest.hpp; we'll use it to report errors.
auto assertion_failure(char const* file, line_number line, assertion_strings const& assertion) noexcept -> bool;

namespace {

/// @brief Represents execution errors sent out-of-band by the child to the parent; more specific than just ::exit()
enum class child_error : unsigned char { syscall_error, did_not_exit, unknown_error };

/// @brief the size of each chunk that we'll read from the pipe between the two processes
constexpr ::size_t read_chunk_size{512UL};

/// @brief For the child process, register a failure with the test runner using the provided message
/// @param message A message which will be associated with this death assertion
auto parent_fail_with_message(source_location location, char const* message) noexcept -> void {
  if (errno != 0) {
    // NOLINTNEXTLINE(concurrency-mt-unsafe) Nothing in death tests are thread safe
    char const* errno_string = ::strerror(errno);
    errno = 0;
    assertion_failure(location.file, location.line, {message, "; errno message=", errno_string});
  } else {
    assertion_failure(location.file, location.line, {message});
  }
}

/// @brief For the child process, print a message and then fail.
/// @param location The source location at which this assertion is taking place
/// @param message A message which will be printed to stderr (which may end up piped to the parent process instead)
/// @param meta_pipe A handle to the "meta pipe" through which out-of-band errors can be sent to the parent process
[[noreturn]] auto child_syscall_error(int meta_pipe) noexcept -> void {
  child_error error_code = child_error::syscall_error;
  static_cast<void>(::write(meta_pipe, &error_code, sizeof(error_code)));
  ::testing::minitest::exit_program(static_cast<int>(error_code));  // Child process must exit in death tests
}

/// @brief Pull data off the indicated pipe until either the expected message is found or the pipe runs out of data
/// @param location The source location at which this assertion is taking place
/// @param expected_message Message to search for; searched for inside the pipe data as if by @c strstr
/// @param stderr_pipe File descriptor for the read side of a pipe containing @c stderr messages
/// @return @c true if @c expected_message was found inside of <c>stderr_pipe</c>, @c false if not
auto find_message_in_pipe(source_location location, char const* expected_message, int stderr_pipe) noexcept -> bool {
  if (cstring::size(expected_message) == 0UL) {
    // The empty message is always found, so now that we know it crashed, we don't actually need to look at stderr
    return true;
  }

  // The child process exited abnormally, so we need to check its stderr for the target message.
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) This is being used to test a standard library so it can't use std
  char child_stderr_storage[(2UL * read_chunk_size) + 1UL]{};
  auto* child_stderr = static_cast<char*>(child_stderr_storage);
  ::ssize_t bytes_read = ::read(stderr_pipe, child_stderr, 2UL * read_chunk_size);
  if (bytes_read < 0) {
    parent_fail_with_message(location, "could not read child process's error messages from pipe");
    return false;
  }

  auto child_stderr_len = static_cast<::size_t>(bytes_read);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) Need to index into this array; can't use std
  child_stderr[child_stderr_len] = '\0';
  while (bytes_read > 0) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) This C API only operates on pointers
    if (cstring::contains(child_stderr, expected_message)) {
      // Found the target string => the assertion passes
      return true;
    }

    // Did not find the target string but did read some bytes => try reading another chunk

    // If we're out of room to read more data, move the second half of the buffer to the first half
    if (child_stderr_len >= (2U * read_chunk_size)) {
      // Move the second chunk of the buffer back to the beginning so we have room for more data
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) This C API only operates on pointers
      ::memcpy(child_stderr, child_stderr + read_chunk_size, read_chunk_size);
      child_stderr_len -= read_chunk_size;
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) Need to index into this array; can't use std
      child_stderr[child_stderr_len] = '\0';
    }

    // Read a new chunk off the pipe and put it in the second half of the buffer
    // Try to read enough characters to get the buffer to 2*read_chunk_size
    // (in theory we should be requesting exactly 1 chunk, but we might have gotten less than that from the pipe before)
    ::size_t const this_read_size = (2UL * read_chunk_size) - child_stderr_len;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) This C API only operates on pointers
    bytes_read = ::read(stderr_pipe, &child_stderr[child_stderr_len], this_read_size);
    if (bytes_read < 0) {
      parent_fail_with_message(location, "could not read child process's error messages from pipe");
      return false;
    }
    child_stderr_len += static_cast<::size_t>(bytes_read);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) Need to index into this array; can't use std
    child_stderr[child_stderr_len] = '\0';
  }

  // Ran out of characters in the pipe without finding the target string => the assertion fails
  parent_fail_with_message(location, "child process failed but its stderr did not include the target string");
  return false;
}
}  // namespace

/// @brief Construct a death_monitor
/// @param file The file in which this assertion is taking place
/// @param line The line on which this assertion is taking place
/// @param expected_message The message which the child process is expected to write to stderr
/// @note Unlike GoogleTest, there is no non-trivial regex parsing on <c>expected_message</c>; effectively just strstr
death_monitor::death_monitor(source_location location, char const* expected_message) noexcept
    : source_location_(location),
      expected_message_(expected_message),
      child_pid_{-1},
      stderr_pipe_{-1},
      meta_pipe_{-1} {}

death_monitor::~death_monitor() {
  if (stderr_pipe_ >= 0) {
    ::close(stderr_pipe_);
  }
  if (meta_pipe_ >= 0) {
    ::close(meta_pipe_);
  }
}

/// @brief Fork the current process using whatever system API is available
/// @return A result indicating whether execution is now in the parent or child process, or if a syscall failed
auto death_monitor::fork() noexcept -> fork_result {
  if (cstring::size(expected_message_) > read_chunk_size) {
    parent_fail_with_message(source_location_, "expected message is too long to check for; it could get cut off");
    return fork_result::failed;
  }

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) This is being used to test a standard library so it can't use std
  int stderr_pipe_fds[2];
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) This is being used to test a standard library so it can't use std
  int meta_pipe_fds[2];
  if (::pipe(static_cast<int*>(stderr_pipe_fds)) == -1 || ::pipe(static_cast<int*>(meta_pipe_fds)) == -1) {
    parent_fail_with_message(source_location_, "could not open pipe between parent and child processes");
    return fork_result::failed;
  }

  ::pid_t const fork_result = ::fork();

  if (fork_result > 0) {
    // The current process is the parent.
    child_pid_ = fork_result;
    stderr_pipe_ = stderr_pipe_fds[0];  // Save the read side of the stderr pipe
    meta_pipe_ = meta_pipe_fds[0];      // Save the read side of the meta pipe

    if (::close(stderr_pipe_fds[1]) == -1 || ::close(meta_pipe_fds[1]) == -1) {
      parent_fail_with_message(source_location_, "parent process failed to close its unneeded pipes");
    }

    return fork_result::parent_process;
  }

  if (fork_result == 0) {
    // The current process is the child.
    stderr_pipe_ = stderr_pipe_fds[1];  // Save the write side of the stderr pipe
    meta_pipe_ = meta_pipe_fds[1];      // Save the write side of the meta pipe

    if (::close(stderr_pipe_fds[0]) == -1 || ::close(meta_pipe_fds[0]) == -1) {
      child_syscall_error(meta_pipe_);
    }

    // Redirect stdout to /dev/null and stderr into the pipe; the test logic only cares about stderr.
    if (::freopen("/dev/null", "w", stdout) == nullptr) {
      child_syscall_error(meta_pipe_);
    }
    if (::dup2(stderr_pipe_, STDERR_FILENO) == -1) {
      child_syscall_error(meta_pipe_);
    }

    return fork_result::child_process;
  }

  parent_fail_with_message(source_location_, "could not fork test runner process");
  return fork_result::failed;
}

/// @brief Report the result of the test. Must be run from the parent process, which will wait for the child process.
auto death_monitor::parent_report_result() noexcept -> bool {
  int wait_status{0};
  if (::waitpid(child_pid_, &wait_status, 0) != child_pid_) {
    parent_fail_with_message(source_location_, "could not wait for child process");
    return false;
  }

  // The child process must have either exited with a non-0 exit status or by catching a signal (any signal is fine).
  if (WIFEXITED(wait_status)) {
    if (WEXITSTATUS(wait_status) == 0) {
      parent_fail_with_message(source_location_, "exited with status 0 instead of an error");
      return false;
    }
  } else if (!WIFSIGNALED(wait_status)) {
    parent_fail_with_message(source_location_, "child process did not exit at all (this should not be possible)");
    return false;
  }

  // Check that the child process didn't send any out-of-band error codes over the meta pipe.
  child_error maybe_error_code{child_error::unknown_error};
  ::ssize_t const bytes_read = ::read(meta_pipe_, &maybe_error_code, sizeof(maybe_error_code));
  // Ideally bytes_read == 0 so we don't enter either of these blocks. In that case the child sent no error codes.
  if (bytes_read < 0) {
    parent_fail_with_message(source_location_, "could not read child process's execution error pipe");
    return false;
  }
  if (bytes_read > 0) {
    switch (maybe_error_code) {
      case child_error::syscall_error:
        parent_fail_with_message(source_location_, "child process failed to acquire resources from the OS");
        return false;
      case child_error::did_not_exit:
        parent_fail_with_message(source_location_, "did not exit");
        return false;
      default:
        parent_fail_with_message(source_location_, "child process failed with an unknown error");
        return false;
    }
  }

  return find_message_in_pipe(source_location_, expected_message_, stderr_pipe_);
}

[[noreturn]] auto death_monitor::child_force_exit() const noexcept -> void {
  child_error error_code = child_error::did_not_exit;
  static_cast<void>(::write(meta_pipe_, &error_code, sizeof(error_code)));
  ::testing::minitest::exit_program(static_cast<int>(error_code));  // Child process must exit in death tests
}

}  // namespace testing
