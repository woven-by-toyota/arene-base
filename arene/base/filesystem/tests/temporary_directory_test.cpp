// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/temporary_directory.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include "arene/base/filesystem/path_string.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/string_view.hpp"
#include "arene/base/testing/filesystem/create_temporary_directory.hpp"

namespace {

/// @test `temporary_directory` can be default-constructed
TEST(TemporaryDirectory, CanBeDefaultConstructed) {
  static_assert(
      std::is_default_constructible<arene::base::filesystem::temporary_directory>::value,
      "Can be default constructed"
  );
}

/// @test `path` on a default-constructed `temporary_directory` returns a valid path with RWX permissions
TEST(TemporaryDirectory, GetPathReturnsPathOfEmptyDirectory) {
  arene::base::filesystem::temporary_directory const tmpdir{};
  auto const& path = tmpdir.path();
  ASSERT_EQ(access(path.c_str(), R_OK | W_OK | X_OK), 0);
}

/// @test `path` on a default-constructed `temporary_directory` returns a path that is not empty
TEST(TemporaryDirectory, GetPathIsNotBlank) {
  arene::base::filesystem::temporary_directory const tmpdir{};
  auto const& path = tmpdir.path();
  ASSERT_NE(path, std::string());
}

/// @test `path` on a default-constructed `temporary_directory` returns a valid path that is different to that of
/// another `temporary_directory` instance
TEST(TemporaryDirectory, PathNameIsDifferentForEachInstance) {
  arene::base::filesystem::temporary_directory const tmpdir1{};
  arene::base::filesystem::temporary_directory const tmpdir2{};
  ASSERT_NE(tmpdir1.path(), tmpdir2.path());
}

namespace {

char const* const tmpdir_env_name = "TMPDIR";

}  // namespace

/// @test `path` on a default-constructed `temporary_directory` returns a path within the folder specified by the
/// `TMPDIR` environment variable
TEST(TemporaryDirectory, IfTmpDirEnvironmentVariableSetUseThatAsPrefix) {
  arene::base::testing::filesystem::detail::env_var_restorer const restorer(tmpdir_env_name);

  arene::base::filesystem::temporary_directory const outer{};

  std::string temp_path(outer.path().c_str());
  if (temp_path.back() != '/') {
    temp_path.push_back('/');
  }
  temp_path += "temp_dir_test_dir";
  auto const dir_permissions = 0700;
  ASSERT_EQ(mkdir(temp_path.c_str(), dir_permissions), 0);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  ASSERT_EQ(setenv(tmpdir_env_name, temp_path.c_str(), 1), 0);

  arene::base::filesystem::temporary_directory const nested{};
  std::string const nested_path = nested.path().c_str();

  ASSERT_NE(nested_path, temp_path);
  ASSERT_EQ(nested_path.substr(0, temp_path.size()), temp_path);
  ASSERT_EQ(nested_path[temp_path.size()], '/');
}

/// @test `path` on a default-constructed `temporary_directory` returns a path within `/tmp` if the `TMPDIR` environment
/// variable is not set
TEST(TemporaryDirectory, IfTmpDirEnvironmentVariableNotSetUseSlashTmp) {
  arene::base::testing::filesystem::detail::env_var_restorer const restorer(tmpdir_env_name);

  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  ASSERT_EQ(unsetenv(tmpdir_env_name), 0);

  arene::base::filesystem::temporary_directory const tmpdir{};
  auto const& tmpdir_path = tmpdir.path();

  std::string const tmp_prefix = "/tmp/";
  ASSERT_NE(tmpdir_path, tmp_prefix);
  ASSERT_TRUE(tmpdir_path.starts_with(tmp_prefix));
}

/// @test `path` on a `temporary_directory` with root param returns a path
/// within the folder specified by the `TMPDIR` environment variable
TEST(TemporaryDirectory, IfTmpDirEnvironmentVariableSetUseThatAsPrefixWithParameterizedRoot) {
  arene::base::testing::filesystem::detail::env_var_restorer const restorer(tmpdir_env_name);

  arene::base::filesystem::temporary_directory const outer{};

  std::string temp_path(outer.path().c_str());
  if (temp_path.back() != '/') {
    temp_path.push_back('/');
  }
  temp_path += "temp_dir_test_dir";
  auto const dir_permissions = 0700;
  ASSERT_EQ(mkdir(temp_path.c_str(), dir_permissions), 0);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  ASSERT_EQ(setenv(tmpdir_env_name, temp_path.c_str(), 1), 0);

  arene::base::filesystem::temporary_directory const nested{"invalid_root"};
  std::string const nested_path = nested.path().c_str();

  ASSERT_NE(nested_path, temp_path);
  ASSERT_EQ(nested_path.substr(0, temp_path.size()), temp_path);
  ASSERT_EQ(nested_path[temp_path.size()], '/');
}

/// @test `path` on a `temporary_directory` with root param returns a path
/// within root param if the `TMPDIR` environment variable is not set
TEST(TemporaryDirectory, IfTmpDirEnvironmentVariableNotSetUseSlashTmpWithParameterizedRoot) {
  arene::base::testing::filesystem::detail::env_var_restorer const restorer(tmpdir_env_name);

  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  ASSERT_EQ(unsetenv(tmpdir_env_name), 0);

  arene::base::filesystem::temporary_directory const outer{};

  arene::base::filesystem::path_string root_path{outer.path().c_str()};
  if (root_path.back() != '/') {
    root_path.push_back('/');
  }

  arene::base::filesystem::temporary_directory const nested{root_path};
  std::string const nested_path = nested.path().c_str();

  ASSERT_NE(nested_path, root_path);
  ASSERT_EQ(nested_path.substr(0, root_path.size()), root_path);
  ASSERT_EQ(nested_path[root_path.size() - 1], '/');
}

/// @test The `temporary_directory` default constructor throws if a directory cannot be created
TEST(TemporaryDirectory, TemporaryDirectoryThrowsIfCannotCreateDir) {
  arene::base::testing::filesystem::detail::env_var_restorer const restorer(tmpdir_env_name);

  arene::base::filesystem::temporary_directory const outer{};

  std::string temp_path(outer.path().c_str());
  if (temp_path.back() != '/') {
    temp_path.push_back('/');
  }
  temp_path += "temp_dir_test_dir";
  {
    std::ofstream file(temp_path.c_str());
    file << "dummy";
  }
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  ASSERT_EQ(setenv(tmpdir_env_name, temp_path.c_str(), 1), 0);

  ASSERT_THROW(arene::base::filesystem::temporary_directory{}, std::system_error);
}

/// @test When the `temporary_directory` object is destroyed, the directory is removed
TEST(TemporaryDirectory, DirectoryIsRemovedWhenObjectDestroyed) {
  arene::base::filesystem::path_string path;
  {
    arene::base::filesystem::temporary_directory const tmpdir =
        arene::base::testing::filesystem::create_temporary_directory();
    path = tmpdir.path();
    ASSERT_EQ(access(path.c_str(), R_OK | W_OK | X_OK), 0);
  }
  ASSERT_NE(access(path.c_str(), F_OK), 0);
}

/// @test When the `temporary_directory` object is destroyed, the directory is removed, including any contents
TEST(TemporaryDirectory, DirectoryRemovedWhenObjectDestroyedEvenIfNotEmpty) {
  arene::base::filesystem::path_string path;
  {
    arene::base::filesystem::temporary_directory const tmpdir =
        arene::base::testing::filesystem::create_temporary_directory();
    path = tmpdir.path();
    ASSERT_EQ(access(path.c_str(), R_OK | W_OK | X_OK), 0);

    std::ofstream file((path + "/foo").c_str());
    file << "dummy";
  }
  ASSERT_NE(access(path.c_str(), F_OK), 0);
}

/// @test When the `temporary_directory` object is destroyed, the directory is removed, including any contents and
/// nested folders
TEST(TemporaryDirectory, DirectoryRemovedWhenObjectDestroyedEvenIfNotEmptyWithNestedFolders) {
  arene::base::filesystem::path_string path;
  {
    arene::base::filesystem::temporary_directory const tmpdir =
        arene::base::testing::filesystem::create_temporary_directory();
    path = tmpdir.path();
    ASSERT_EQ(access(path.c_str(), R_OK | W_OK | X_OK), 0);

    arene::base::filesystem::path_string temp_path(path);
    if (temp_path.back() != '/') {
      temp_path.push_back('/');
    }
    temp_path += "temp_dir_test_dir";
    auto const dir_permissions = 0700;
    ASSERT_EQ(mkdir(temp_path.c_str(), dir_permissions), 0);
    std::ofstream file((temp_path + "/foo").c_str());
    file << "dummy";
  }
  ASSERT_NE(access(path.c_str(), F_OK), 0);
}

/// @test When the `temporary_directory` object is destroyed, the directory is not removed if any of the contents cannot
/// be removed
TEST(TemporaryDirectory, DirectoryNotRemovedIfFileCannotBeRemoved) {
  arene::base::filesystem::path_string path;
  {
    arene::base::filesystem::temporary_directory const tmpdir{};
    path = tmpdir.path();
    ASSERT_EQ(access(path.c_str(), R_OK | W_OK | X_OK), 0);

    std::ofstream file((path + "/foo").c_str());
    file << "dummy";

    ASSERT_EQ(chmod(path.c_str(), R_OK | X_OK), 0);
  }
  ASSERT_EQ(access(path.c_str(), F_OK), 0);
}

}  // namespace
