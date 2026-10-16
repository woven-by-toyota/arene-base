// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_TESTS_PROPERTY_CONFIG_ENV_HELPERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_TESTS_PROPERTY_CONFIG_ENV_HELPERS_HPP_

#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/null_terminated_string_view.hpp"
#include "arene/base/testing/property_config.hpp"

// NOTE: we *cannot* include <stdlib.h> here. Some toolchain layouts (the arene GCC toolchains, and clang+libstdc++
// configurations such as the build-filtered-repo CI environment) ship a C++ wrapper at <stdlib.h> that pulls in
// <cstdlib>; that header is replaced under the without_toolchain_stdlib build configuration with a stub that errors
// out. Forward-declaring the POSIX env-var APIs as extern "C" sidesteps the wrapper entirely. Mirrors the workaround
// in testlibs/minitest/libc_default/libc.cpp.
// NOLINTBEGIN(readability-redundant-declaration)
extern "C" {
auto getenv(char const*) -> char*;
auto setenv(char const*, char const*, int) -> int;
auto unsetenv(char const*) -> int;
}
// NOLINTEND(readability-redundant-declaration)

// NOLINTBEGIN(concurrency-mt-unsafe)
namespace arene {
namespace base {
namespace testing {
namespace tests {

/// @brief @c ::setenv accepting a @c null_terminated_string_view name; always overwrites.
/// @param name environment variable name
/// @param value new value
/// @return @c ::setenv 's return code
inline auto setenv(arene::base::null_terminated_string_view name, char const* value) noexcept -> int {
  constexpr int overwrite{1};

  return ::setenv(name.c_str(), value, overwrite);
}

/// @brief @c ::unsetenv accepting a @c null_terminated_string_view name.
/// @param name environment variable name
/// @return @c ::unsetenv 's return code
inline auto unsetenv(arene::base::null_terminated_string_view name) noexcept -> int { return ::unsetenv(name.c_str()); }

/// @brief RAII scope guard that restores an environment variable when destroyed.
///
/// The previous value, if any, is snapshotted into a fixed-capacity inline string. Inputs longer than @c max_value_size
/// trip an @c ARENE_PRECONDITION inside @c inline_string 's constructor.
class env_var_restorer {
 public:
  /// @brief Maximum length of a snapshotted environment-variable value.
  static constexpr std::size_t max_value_size{4096U};

 private:
  arene::base::null_terminated_string_view var_name_;
  arene::base::optional<arene::base::inline_string<max_value_size>> old_val_;

 public:
  /// @brief Capture the current value of @p var_name so it can be restored on scope exit.
  /// @param var_name environment variable name to snapshot
  explicit env_var_restorer(arene::base::null_terminated_string_view var_name) noexcept
      : var_name_(var_name) {
    char const* const old_val = ::getenv(var_name_.c_str());  // NOLINT(concurrency-mt-unsafe)
    if (old_val != nullptr) {
      old_val_.emplace(old_val);
    }
  }

  ~env_var_restorer() {
    if (old_val_) {
      std::ignore = setenv(var_name_, old_val_->c_str());
    } else {
      std::ignore = unsetenv(var_name_);
    }
  }

  env_var_restorer(env_var_restorer const&) = delete;
  env_var_restorer(env_var_restorer&&) = delete;
  auto operator=(env_var_restorer const&) -> env_var_restorer& = delete;
  auto operator=(env_var_restorer&&) -> env_var_restorer& = delete;
};

/// @brief scope guard that unsets all three property-config env vars and
///     restores them when destroyed.
class clear_property_env {
  env_var_restorer restore_required_successes_{arene::base::testing::required_successes_env_name};
  env_var_restorer restore_max_discards_{arene::base::testing::max_discards_env_name};
  env_var_restorer restore_seed_{arene::base::testing::seed_env_name};

 public:
  /// @brief Unset all three property-config env vars; the prior values are restored on destruction.
  clear_property_env() noexcept {
    std::ignore = unsetenv(arene::base::testing::required_successes_env_name);
    std::ignore = unsetenv(arene::base::testing::max_discards_env_name);
    std::ignore = unsetenv(arene::base::testing::seed_env_name);
  }
};

}  // namespace tests
}  // namespace testing
}  // namespace base
}  // namespace arene
// NOLINTEND(concurrency-mt-unsafe)

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_TESTS_PROPERTY_CONFIG_ENV_HELPERS_HPP_
