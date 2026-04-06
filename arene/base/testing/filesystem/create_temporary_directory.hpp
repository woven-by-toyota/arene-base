// parasoft-suppress AUTOSAR-A2_8_1-a "The class is in a detail namespace; non-detail declaration matches the filename"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_FILESYSTEM_CREATE_TEMPORARY_DIRECTORY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_FILESYSTEM_CREATE_TEMPORARY_DIRECTORY_HPP_

#include <cstdlib>
#include <string>
#include <tuple>
#include <utility>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/filesystem/path_string.hpp"
#include "arene/base/filesystem/temporary_directory.hpp"
#include "arene/base/optional.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "False positive: these use trailing return types, not auto specifiers"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress CERT_C-POS34-a "Environment variables are Bazel's API for passing test directories"
// parasoft-begin-suppress AUTOSAR-M18_0_3-d "Environment variables are Bazel's API for passing test directories"
// parasoft-begin-suppress CERT_C-CON33-a "These thread-unsafe functions are only used in single-threaded tests"

namespace arene {
namespace base {
namespace testing {
namespace filesystem {

namespace detail {

/// @brief helper utility providing a RAII wrapper to restore an environment
/// variable at end of scope
///
class env_var_restorer {
  /// @brief name of the environment variable to restore upon going out of scope
  std::string var_name_;
  /// @brief value to write to @c var_name_ upon going out of scope
  arene::base::optional<std::string> old_val_;

 public:
  /// @brief store the current value of a target environment variable, including
  ///     unset ones
  /// @param var_name name of the target environment variable
  ///
  explicit env_var_restorer(std::string var_name)
      : var_name_(std::move(var_name)) {
    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    if (char const* const old_val{getenv(var_name_.c_str())}) {
      old_val_.emplace(old_val);
    }
  }

  /// @brief restore the target environment variable
  ///
  ~env_var_restorer() {
    if (old_val_) {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      std::ignore = setenv(var_name_.c_str(), old_val_->c_str(), 1);
    } else {
      // NOLINTNEXTLINE(concurrency-mt-unsafe)
      std::ignore = unsetenv(var_name_.c_str());
    }
  }

  /// @brief this utility cannot be copied or moved
  env_var_restorer(env_var_restorer const&) = delete;
  /// @brief this utility cannot be copied or moved
  env_var_restorer(env_var_restorer&&) = delete;
  /// @brief this utility cannot be copied or moved
  auto operator=(env_var_restorer const&) -> env_var_restorer& = delete;
  /// @brief this utility cannot be copied or moved
  auto operator=(env_var_restorer&&) -> env_var_restorer& = delete;
};
}  // namespace detail

/// @brief construct a temporary directory within the test runtime
///
/// Constructs a temporary directory with special handling when the test binary
/// is invoked with Bazel.
///
/// When a test is run under Bazel, Bazel defines the environment variable @c
/// TEST_TMPDIR. This is guaranteed by Bazel to be unique for each invocation of
/// <tt>bazel test</tt> and the directory where tests should create files.
///
/// This function creates a temporary directory under a root path where:
/// * the root path is @c TEST_TMPDIR if it is defined;
/// * otherwise, the root path is @c TMPDIR if it is defined;
/// * otherwise, the default root path used by @c temporary_directory
///
/// @return the created @c temporary_directory value
///
inline auto create_temporary_directory() -> arene::base::filesystem::temporary_directory {
  if (char const* const test_tmpdir{getenv("TEST_TMPDIR")}) {  // NOLINT(concurrency-mt-unsafe)
    // parasoft-begin-suppress AUTOSAR-M0_1_3-a "False positive: this is a scope guard, it's used by destroying it"
    detail::env_var_restorer const restorer{"TMPDIR"};
    // parasoft-end-suppress AUTOSAR-M0_1_3-a

    // parasoft-begin-suppress AUTOSAR-A27_0_4-d "This is a system API that always takes C-style strings"
    std::ignore = unsetenv("TMPDIR");  // NOLINT(concurrency-mt-unsafe)
    // parasoft-end-suppress AUTOSAR-A27_0_4-d

    return arene::base::filesystem::temporary_directory{arene::base::filesystem::path_string{test_tmpdir}};
  }

  return arene::base::filesystem::temporary_directory{};
}

}  // namespace filesystem
}  // namespace testing
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a
// parasoft-end-suppress CERT_C-EXP37-a
// parasoft-end-suppress AUTOSAR-M2_10_1-a
// parasoft-end-suppress CERT_C-POS34-a
// parasoft-end-suppress AUTOSAR-M18_0_3-d
// parasoft-end-suppress CERT_C-CON33-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_FILESYSTEM_CREATE_TEMPORARY_DIRECTORY_HPP_
