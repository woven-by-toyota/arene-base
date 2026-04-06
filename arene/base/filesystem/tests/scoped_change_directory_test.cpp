// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/scoped_change_directory.hpp"

// 'realpath' comes from stdlib.h not cstdlib
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdexcept>
#include <system_error>
#include <utility>

#include <gtest/gtest.h>

#include "arene/base/filesystem/temporary_directory.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/string_view.hpp"

namespace {
namespace test {

// obtain the current working directory as a path_string
auto cwd() -> arene::base::filesystem::path_string {
  arene::base::filesystem::path_string directory{};
  if (getcwd(directory.data(), directory.capacity()) != directory.data()) {
    throw std::runtime_error{"unable to determine cwd"};
  }
  return directory;
}

// resolve the realpath of a potential symlink
auto resolve(arene::base::filesystem::path_string path) -> arene::base::filesystem::path_string {
  arene::base::filesystem::path_string resolved{};
  if (realpath(path.data(), resolved.data()) != resolved.data()) {
    throw std::runtime_error{"unable to determine realpath"};
  }
  return resolved;
}

}  // namespace test

/// @test After constructing a `scoped_change_directory` object, the current directory has changed to the path supplied
/// to the constructor
TEST(ChangeDirectory, AfterConstructionCurrentDirectoryHasChanged) {
  arene::base::filesystem::temporary_directory const tempdir;
  arene::base::filesystem::scoped_change_directory const scoped_cwd(tempdir.path());

  // NOTE: 'getcwd' can return a resolved path on some platforms
  ASSERT_EQ(test::cwd(), test::resolve(tempdir.path()));
}

/// @test Constructing a `scoped_change_directory` with a path that doesn't exist throws `std::system_error`
TEST(ChangeDirectory, ChangingToNonExistentDirFails) {
  arene::base::filesystem::temporary_directory const tempdir;
  ASSERT_THROW(arene::base::filesystem::scoped_change_directory(tempdir.path() + "/missing"), std::system_error);
}

/// @test After destroying a `scoped_change_directory` object, the currnet directory is returned to what it was prior to
/// the construction
TEST(ChangeDirectory, AfterDestructionCurrentDirectoryRestored) {
  arene::base::filesystem::temporary_directory const tempdir;

  ASSERT_EQ(chdir(tempdir.path().c_str()), 0);
  ASSERT_EQ(test::cwd(), test::resolve(tempdir.path()));

  auto const subdir_path = tempdir.path() + "/subdir";
  auto const dir_perms = 0700;
  ASSERT_EQ(mkdir(subdir_path.c_str(), dir_perms), 0);
  {
    arene::base::filesystem::scoped_change_directory const scoped_cwd(subdir_path);
    ASSERT_EQ(test::cwd(), test::resolve(subdir_path));
  }
  ASSERT_EQ(test::cwd(), test::resolve(tempdir.path()));
}

}  // namespace
