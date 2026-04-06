// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/filesystem/directory_handle.hpp"
#include "arene/base/filesystem/error_code.hpp"
#include "arene/base/filesystem/file_handle.hpp"
#include "arene/base/filesystem/path_string.hpp"
#include "arene/base/filesystem/scoped_change_directory.hpp"
#include "arene/base/filesystem/temporary_directory.hpp"

namespace {

/// @test Ensure `filesystem::directory_handle` is exported
TEST(FileSystem, DirectoryHandle) { using ::arene::base::filesystem::directory_handle; }

/// @test Ensure `filesystem::file_handle` is exported
TEST(FileSystem, FileHandle) { using ::arene::base::filesystem::file_handle; }

/// @test Ensure `filesystem::error_code` is exported
TEST(FileSystem, ErrorCode) { using ::arene::base::filesystem::error_code; }

/// @test Ensure `filesystem::path_string` is exported
TEST(FileSystem, PathString) { using ::arene::base::filesystem::path_string; }

/// @test Ensure `filesystem::scoped_change_directory` is exported
TEST(FileSystem, ScopedChangeDirectory) { using ::arene::base::filesystem::scoped_change_directory; }

/// @test Ensure `filesystem::temporary_directory` is exported
TEST(FileSystem, TemporaryDirectory) { using ::arene::base::filesystem::temporary_directory; }

}  // namespace
