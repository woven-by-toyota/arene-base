// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/testing/filesystem/create_temporary_directory.hpp"

#include <cstdlib>
#include <string>

#include <gtest/gtest.h>

namespace {

constexpr char const* env_var_name = "MY_TEST_VAR";

/// @test @c env_var_restorer restores an environment variable that is changed.
///
TEST(EnvVarRestorer, RestoresChangedVariable) {
  setenv(env_var_name, "42", 1);  // NOLINT(concurrency-mt-unsafe)

  {
    arene::base::testing::filesystem::detail::env_var_restorer const restore1{env_var_name};
    setenv(env_var_name, "0", 1);  // NOLINT(concurrency-mt-unsafe)

    ASSERT_STRNE(getenv(env_var_name), "42");  // NOLINT(concurrency-mt-unsafe)
  }

  ASSERT_STREQ(getenv(env_var_name), "42");  // NOLINT(concurrency-mt-unsafe)
}

/// @test @c env_var_restorer restores an environment variable that is unset.
///
TEST(EnvVarRestorer, RestoresUnsetVariable) {
  setenv(env_var_name, "42", 1);  // NOLINT(concurrency-mt-unsafe)

  {
    arene::base::testing::filesystem::detail::env_var_restorer const restore1{env_var_name};
    unsetenv(env_var_name);  // NOLINT(concurrency-mt-unsafe)

    ASSERT_EQ(getenv(env_var_name), nullptr);  // NOLINT(concurrency-mt-unsafe)
  }

  ASSERT_STREQ(getenv(env_var_name), "42");  // NOLINT(concurrency-mt-unsafe)
}

/// @test @c env_var_restorer unsets an environment variable that was never set but then is set within restorer scope.
///
TEST(EnvVarRestorer, UnsetsVariableThatIsSet) {
  unsetenv(env_var_name);  // NOLINT(concurrency-mt-unsafe)

  {
    arene::base::testing::filesystem::detail::env_var_restorer const restore1{env_var_name};
    setenv(env_var_name, "0", 1);  // NOLINT(concurrency-mt-unsafe)

    ASSERT_NE(getenv(env_var_name), nullptr);  // NOLINT(concurrency-mt-unsafe)
  }

  ASSERT_EQ(getenv(env_var_name), nullptr);  // NOLINT(concurrency-mt-unsafe)
}

/// @test @c env_var_restorer unsets an environment variable that was never set and also never set within restorer
/// scope.
///
TEST(EnvVarRestorer, UnsetsVariable) {
  unsetenv(env_var_name);  // NOLINT(concurrency-mt-unsafe)

  {
    arene::base::testing::filesystem::detail::env_var_restorer const restore1{env_var_name};
    ASSERT_EQ(getenv(env_var_name), nullptr);  // NOLINT(concurrency-mt-unsafe)
  }

  ASSERT_EQ(getenv(env_var_name), nullptr);  // NOLINT(concurrency-mt-unsafe)
}

/// @test @c create_temporary_directory uses @c TEST_TMPDIR if it is defined and @c TMPDIR is defined
///
TEST(CreateTemporaryDirectory, UsesTestTmpdirIfDefined) {
  arene::base::testing::filesystem::detail::env_var_restorer const restore1{"TEST_TMPDIR"};
  arene::base::testing::filesystem::detail::env_var_restorer const restore2{"TMPDIR"};

  unsetenv("TEST_TMPDIR");  // NOLINT(concurrency-mt-unsafe)
  unsetenv("TMPDIR");       // NOLINT(concurrency-mt-unsafe)

  arene::base::filesystem::temporary_directory const test_tmpdir{};
  arene::base::filesystem::temporary_directory const tmpdir{};

  ASSERT_NE(test_tmpdir.path(), tmpdir.path());

  setenv("TEST_TMPDIR", test_tmpdir.path().c_str(), 1);  // NOLINT(concurrency-mt-unsafe)
  setenv("TMPDIR", tmpdir.path().c_str(), 1);            // NOLINT(concurrency-mt-unsafe)

  arene::base::filesystem::temporary_directory const nested =
      arene::base::testing::filesystem::create_temporary_directory();

  ASSERT_NE(nested.path(), test_tmpdir.path());
  ASSERT_EQ(nested.path().substr(0, test_tmpdir.path().size()), test_tmpdir.path());
  ASSERT_EQ(nested.path()[test_tmpdir.path().size()], '/');
}

/// @test @c create_temporary_directory uses @c TEST_TMPDIR if it is defined and @c TMPDIR is not defined
///
TEST(CreateTemporaryDirectory, UsesTestTmpdirIfDefinedAndTmpdirNotDefined) {
  arene::base::testing::filesystem::detail::env_var_restorer const restore1{"TEST_TMPDIR"};
  arene::base::testing::filesystem::detail::env_var_restorer const restore2{"TMPDIR"};

  unsetenv("TEST_TMPDIR");  // NOLINT(concurrency-mt-unsafe)
  unsetenv("TMPDIR");       // NOLINT(concurrency-mt-unsafe)

  arene::base::filesystem::temporary_directory const test_tmpdir{};

  setenv("TEST_TMPDIR", test_tmpdir.path().c_str(), 1);  // NOLINT(concurrency-mt-unsafe)

  arene::base::filesystem::temporary_directory const nested =
      arene::base::testing::filesystem::create_temporary_directory();

  ASSERT_NE(nested.path(), test_tmpdir.path());
  ASSERT_EQ(nested.path().substr(0, test_tmpdir.path().size()), test_tmpdir.path());
  ASSERT_EQ(nested.path()[test_tmpdir.path().size()], '/');
}

/// @test @c create_temporary_directory uses @c TMPDIR if @c TEST_TMPDIR is not defined and @c TMPDIR is defined
///
TEST(CreateTemporaryDirectory, UsesTmpdirIfTestTmpdirNotDefined) {
  arene::base::testing::filesystem::detail::env_var_restorer const restore1{"TEST_TMPDIR"};
  arene::base::testing::filesystem::detail::env_var_restorer const restore2{"TMPDIR"};

  unsetenv("TEST_TMPDIR");  // NOLINT(concurrency-mt-unsafe)
  unsetenv("TMPDIR");       // NOLINT(concurrency-mt-unsafe)

  arene::base::filesystem::temporary_directory const tmpdir{};

  setenv("TMPDIR", tmpdir.path().c_str(), 1);  // NOLINT(concurrency-mt-unsafe)

  arene::base::filesystem::temporary_directory const nested =
      arene::base::testing::filesystem::create_temporary_directory();

  ASSERT_NE(nested.path(), tmpdir.path());
  ASSERT_EQ(nested.path().substr(0, tmpdir.path().size()), tmpdir.path());
  ASSERT_EQ(nested.path()[tmpdir.path().size()], '/');
}

/// @test @c create_temporary_directory creates in @c /tmp if neither @c TEST_TMPDIR nor @c TMPDIR are defined
///
TEST(CreateTemporaryDirectory, UsesTmpIfNeitherTmpdirEnvironmentVariablesAreDefined) {
  arene::base::testing::filesystem::detail::env_var_restorer const restore1{"TEST_TMPDIR"};
  arene::base::testing::filesystem::detail::env_var_restorer const restore2{"TMPDIR"};

  unsetenv("TEST_TMPDIR");  // NOLINT(concurrency-mt-unsafe)
  unsetenv("TMPDIR");       // NOLINT(concurrency-mt-unsafe)

  arene::base::filesystem::temporary_directory const nested =
      arene::base::testing::filesystem::create_temporary_directory();

  std::string const tmp_prefix = "/tmp/";
  ASSERT_NE(nested.path(), tmp_prefix);
  ASSERT_EQ(nested.path().substr(0, tmp_prefix.size()), tmp_prefix);
}

}  // namespace
