// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_DETAIL_DEATH_TESTS_ENABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_DETAIL_DEATH_TESTS_ENABLED_HPP_

#include "testlibs/minitest/source_location.hpp"

namespace testing {

/// @brief The result of forking the test runner process for a death test
enum class fork_result { child_process, parent_process, failed };

/// @brief A class which monitors the status of a death test across both the parent and child process
class death_monitor {
 public:
  /// @brief Construct a death_monitor
  /// @param location The file and line on which this assertion is taking place
  /// @param expected_message The message which the child process is expected to write to stderr
  /// @note Unlike GoogleTest, there is no non-trivial regex parsing on <c>expected_message</c>; effectively just strstr
  explicit death_monitor(source_location location, char const* expected_message) noexcept;

  /// @brief death_monitor can not be moved or copied (technically move could be defined if we needed it)
  /// @{
  death_monitor(death_monitor const&) = delete;
  death_monitor(death_monitor&&) = delete;
  auto operator=(death_monitor const&) -> death_monitor& = delete;
  auto operator=(death_monitor&&) -> death_monitor& = delete;
  /// @}

  /// @brief A non-trivial destructor is needed in order to close the pipe.
  ~death_monitor();

  /// @brief Fork the current process using whatever system API is available
  /// @return A result indicating whether execution is now in the parent or child process, or if a syscall failed
  auto fork() noexcept -> fork_result;

  /// @brief Report the result of the test. Must be run from the parent process, which will wait for the child process
  /// @return true if the test passed and execution should continue, false if not
  auto parent_report_result() noexcept -> bool;

  /// @brief For the child process, forcibly exit in cases where the expression under test did not cause a crash
  [[noreturn]] auto child_force_exit() const noexcept -> void;

 private:
  /// @brief The current file and line, used for writing assertion failure messages
  source_location source_location_;
  /// @brief The message which the child process is expected to write to stderr
  char const* expected_message_;
  /// @brief The process ID of the child, used to wait for it to exit.
  int child_pid_;
  /// @brief The pipe that the two processes will use to communicate error messages (could be the read or write side)
  int stderr_pipe_;
  /// @brief The pipe that the two processes will use to communicate execution errors (could be the read or write side)
  int meta_pipe_;
};

}  // namespace testing

/// @brief Macro to assert that an expression will cause abnormal termination with a particular message on @c stderr
/// @param expr An expression which is expected to result in the termination of the process with a non-0 exit code
/// @param message A message which is expected to be found in stderr; an empty string is always found
/// @note We do not support GoogleTest's regex parsing: we only check if @c message is a substring of @c stderr
// This macro can't use a helper function because the first argument is an expression so it can't be passed.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_DEATH(expr, message)                                                                          \
  {                                                                                                          \
    this->flush_output();                                                                                    \
    ::testing::death_monitor _minitest_death_monitor({__FILE__, __LINE__}, (message));                       \
    switch (_minitest_death_monitor.fork()) {                                                                \
      case ::testing::fork_result::child_process:                                                            \
        static_cast<void>(expr);                                                                             \
        _minitest_death_monitor.child_force_exit(); /* Force an exit if evaluating expr did not cause one */ \
      case ::testing::fork_result::parent_process:                                                           \
        if (!_minitest_death_monitor.parent_report_result()) {                                               \
          return;                                                                                            \
        }                                                                                                    \
        break;                                                                                               \
      case ::testing::fork_result::failed:                                                                   \
        break;                                                                                               \
    }                                                                                                        \
  }                                                                                                          \
  (+::testing::stream_insertion_sink{})

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_DETAIL_DEATH_TESTS_ENABLED_HPP_
