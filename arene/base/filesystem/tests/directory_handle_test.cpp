// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/directory_handle.hpp"

#include <array>
#include <cerrno>
#include <fstream>
#include <set>
#include <string>
#include <system_error>

#include <gtest/gtest.h>

#include "arene/base/byte.hpp"
#include "arene/base/compare.hpp"
#include "arene/base/filesystem/open_flags.hpp"
#include "arene/base/filesystem/path_string.hpp"
#include "arene/base/filesystem/temporary_directory.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/result.hpp"
#include "arene/base/span.hpp"
#include "arene/base/string_view.hpp"

namespace {
void write_test_data(arene::base::null_terminated_string_view file_path, std::string const& test_data) {
  std::filebuf file;
  ASSERT_NE(file.open(file_path.c_str(), std::ios::out | std::ios::binary), nullptr);

  auto const data_size = static_cast<std::streamsize>(test_data.size());
  auto const written = file.sputn(test_data.c_str(), data_size);
  ASSERT_EQ(written, test_data.size());
  ASSERT_NE(file.close(), nullptr);
}

/// @test Invoking `valid` on a default-constructed `directory_handle` returns `false`
TEST(DirectoryHandle, DefaultConstructedHandleIsNotValid) {
  arene::base::filesystem::directory_handle const handle{};
  ASSERT_FALSE(handle.valid());
}

/// @test Invoking `open` with a valid directory path returns a `directory_handle` for which `valid` returns `true`
TEST(DirectoryHandle, CanOpenDirectory) {
  arene::base::filesystem::temporary_directory const tmpdir;

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  static_assert(noexcept(arene::base::filesystem::directory_handle::open(tmpdir.path())), "Must be noexcept");
  ASSERT_TRUE(open_result);
  ASSERT_TRUE(open_result.value().valid());
}

/// @test Constructing a `directory_handle` with an invalid `file_handle` throws `std::system_error`
TEST(DirectoryHandle, ItIsAnErrorToConstructWithAnInvalidHandle) {
  ASSERT_THROW(arene::base::filesystem::directory_handle{arene::base::filesystem::file_handle{}}, std::system_error);
}

/// @test Invoking `open` with a non-existent directory returns a `result` holding an `error_code` with a value of
/// `ENOENT`.
TEST(DirectoryHandle, OpenDirectoryReturnsErrorForNonExistentDir) {
  arene::base::filesystem::temporary_directory const tmpdir;

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path() + "/missing");
  ASSERT_FALSE(open_result);
  ASSERT_EQ(open_result.error().value(), ENOENT);
}

/// @test Invoking `open` with a file path returns a `result` holding an `error_code` with a value of `ENOTDIR`.
TEST(DirectoryHandle, OpenDirectoryReturnsErrorForFile) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_path = tmpdir.path() + "/some_file.dat";

  write_test_data(file_path, "test data");

  auto const open_result = arene::base::filesystem::directory_handle::open(file_path);
  ASSERT_FALSE(open_result);
  ASSERT_EQ(open_result.error().value(), ENOTDIR);
}

/// @test After invoking `open` to obtain a `directory_handle` for a directory, `open_file` can be invoked on that
/// handle to open a file, and obtain a `file_handle` which can be used to read that file.
TEST(DirectoryHandle, CanOpenFileInDirectory) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "some test data for the file";
  write_test_data(file_path, test_data);

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto open_file_result = dir_handle.open_file(file_name);
  ASSERT_TRUE(open_file_result);
  arene::base::filesystem::file_handle const file = std::move(open_file_result.value());
  ASSERT_TRUE(file.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const res = file.read_at(buffer, 0);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size());
  for (unsigned i = 0; i < test_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte(test_data[i])) << i;
  }
}

/// @test Invoking `open_file` with an absolute path returns an `error_code` with the value of `EINVAL` rather than a
/// `file_handle`.
TEST(DirectoryHandle, TryingToOpenFileWithAbsolutePathIsError) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "some test data for the file";
  write_test_data(file_path, test_data);

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto open_file_result = dir_handle.open_file(file_path);
  ASSERT_FALSE(open_file_result);
  ASSERT_EQ(open_file_result.error().value(), EINVAL);
}

/// @test Invoking `open_file` with an empty path returns an `error_code` with the value of `EINVAL` rather than a
/// `file_handle`.
TEST(DirectoryHandle, TryingToOpenFileWithEmptyPathIsError) {
  arene::base::filesystem::temporary_directory const tmpdir;

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto open_file_result = dir_handle.open_file(arene::base::null_terminated_string_view{});
  ASSERT_FALSE(open_file_result);
  ASSERT_EQ(open_file_result.error().value(), EINVAL);
}

/// @test If no flags are specified on the call to `open_file`, then attempting to write to the file returns an error
TEST(DirectoryHandle, CannotWriteIfFileOpenedForReading) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "some test data for the file";
  write_test_data(file_path, test_data);

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto open_file_result = dir_handle.open_file(file_name);
  ASSERT_TRUE(open_file_result);
  arene::base::filesystem::file_handle const file = std::move(open_file_result.value());
  ASSERT_TRUE(file.valid());

  ASSERT_FALSE(file.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), 0));
}

/// @test If `open_flags::read_write` is passed to `open_file` then the file can be both read from and written to.
TEST(DirectoryHandle, CanOpenFileInDirectoryForWriting) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  unsigned const base_size = 64;
  std::string const initial_data(base_size, 'A');
  std::string const test_data = "hello";
  write_test_data(file_path, initial_data);

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto open_file_result = dir_handle.open_file(file_name, arene::base::filesystem::open_flags::read_write);
  ASSERT_TRUE(open_file_result);
  arene::base::filesystem::file_handle const file = std::move(open_file_result.value());
  ASSERT_TRUE(file.valid());

  unsigned const write_offset = 10;
  auto write_result = file.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), write_offset);
  auto const remaining = write_result.value();
  ASSERT_EQ(remaining.size(), 0);

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 0;
  auto const res = file.read_at(buffer, offset);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), base_size);
  for (auto idx = 0U; idx < test_data.size(); ++idx) {
    ASSERT_EQ(read_buffer[idx + write_offset], arene::base::byte(test_data[idx])) << idx;
  }
  for (auto idx = 0U; idx < write_offset; ++idx) {
    ASSERT_EQ(read_buffer[idx], arene::base::byte('A')) << idx;
  }
  for (auto idx = write_offset + test_data.size(); idx < base_size; ++idx) {
    ASSERT_EQ(read_buffer[idx], arene::base::byte('A')) << idx;
  }
}

/// @test `create_file` can be used to create a new file in a directory, and returns a `file_handle` which can be used
/// to write to that file, such that a subsequent call to `open_file` can be used to read from that file.
TEST(DirectoryHandle, CanCreateFileInDirectory) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "hello";
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());
  auto create_file_result = dir_handle.create_file(file_name);
  static_assert(noexcept(dir_handle.create_file(file_name)), "Must be noexcept");
  ASSERT_TRUE(create_file_result);
  {
    arene::base::filesystem::file_handle const file = std::move(create_file_result.value());
    auto write_result = file.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), 0);
    auto const remaining = write_result.value();
    ASSERT_EQ(remaining.size(), 0);
  }
  arene::base::filesystem::file_handle const file2 = dir_handle.open_file(file_name).value();

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 0;
  auto const res = file2.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size());
  for (unsigned i = 0; i < test_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte(test_data[i])) << i;
  }
}

/// @test Invoking `create_file` with an empty path returns an `error_code` with the value `EINVAL` rather than a
/// `file_handle`
TEST(DirectoryHandle, TryingToCreateFileWithEmptyPathIsError) {
  arene::base::filesystem::temporary_directory const tmpdir;

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto create_file_result = dir_handle.create_file(arene::base::null_terminated_string_view{});
  ASSERT_FALSE(create_file_result);
  ASSERT_EQ(create_file_result.error().value(), EINVAL);
}

/// @test Invoking `create_file` with an absolute path returns an `error_code` with the value `EINVAL` rather than a
/// `file_handle`
TEST(DirectoryHandle, TryingToCreateFileWithAbsolutePathIsError) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  auto create_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(create_result);

  auto dir_handle = std::move(create_result.value());

  auto create_file_result = dir_handle.create_file(file_path);
  ASSERT_FALSE(create_file_result);
  ASSERT_EQ(create_file_result.error().value(), EINVAL);
}

/// @test Invoking `create_file` with the path of a file that already exists returns an `error_code` with the value
/// `EEXIST` rather than a `file_handle`
TEST(DirectoryHandle, CannotCreateFileIfFileExists) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "hello";
  write_test_data(file_path, test_data);
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());
  auto create_file_result = dir_handle.create_file(file_name);
  ASSERT_FALSE(create_file_result);
  ASSERT_EQ(create_file_result.error().value(), EEXIST);
}

/// @test Invoking `create_file` with a path to a file that already exists and `create_flags::create_or_truncate`
/// truncates the file and returns a handle to it
TEST(DirectoryHandle, TruncateFileIfFileExists) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "hello";
  write_test_data(file_path, test_data);
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());
  auto create_file_result =
      dir_handle.create_file(file_name, arene::base::filesystem::create_flags::create_or_truncate);
  ASSERT_TRUE(create_file_result);
  ASSERT_EQ(dir_handle.get_file_stats(file_name).value().st_size, 0);
}

/// @test Invoking `create_file` with a path to a file that does not exist and `create_flags::create_or_truncate`
/// creates the file and returns a handle to it that can be used to write to it
TEST(DirectoryHandle, CanCreateFileWithTruncate) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "hello";
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());
  auto create_file_result =
      dir_handle.create_file(file_name, arene::base::filesystem::create_flags::create_or_truncate);
  ASSERT_TRUE(create_file_result);
  {
    arene::base::filesystem::file_handle const file = std::move(create_file_result.value());
    auto write_result = file.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), 0);
    auto const remaining = write_result.value();
    ASSERT_EQ(remaining.size(), 0);
  }
  arene::base::filesystem::file_handle const file2 = dir_handle.open_file(file_name).value();

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 0;
  auto const res = file2.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size());
  for (unsigned i = 0; i < test_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte(test_data[i])) << i;
  }
}

/// @test Invoking `create_file` with a path to a file that does not exist and `create_flags::create_or_open`
/// creates the file and returns a handle that can be used to write to the file
TEST(DirectoryHandle, CanCreateFileWithOpen) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "hello";
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());
  auto create_file_result = dir_handle.create_file(file_name, arene::base::filesystem::create_flags::create_or_open);
  ASSERT_TRUE(create_file_result);
  {
    arene::base::filesystem::file_handle const file = std::move(create_file_result.value());
    auto write_result = file.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), 0);
    auto const remaining = write_result.value();
    ASSERT_EQ(remaining.size(), 0);
  }
  arene::base::filesystem::file_handle const file2 = dir_handle.open_file(file_name).value();

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 0;
  auto const res = file2.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size());
  for (unsigned i = 0; i < test_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte(test_data[i])) << i;
  }
}

/// @test Invoking `create_file` with a path to a file that already exists and `create_flags::create_or_open`
/// returns a handle that can be used to read from the file
TEST(DirectoryHandle, CanOpenExistingFileWithCreate) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "hello";
  write_test_data(file_path, test_data);
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());
  auto create_file_result = dir_handle.create_file(file_name, arene::base::filesystem::create_flags::create_or_open);
  ASSERT_TRUE(create_file_result);
  ASSERT_EQ(dir_handle.get_file_stats(file_name).value().st_size, test_data.size());
  arene::base::filesystem::file_handle const file = std::move(create_file_result.value());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 0;
  auto const res = file.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size());
  for (unsigned i = 0; i < test_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte(test_data[i])) << i;
  }
}

/// @test `create_subdirectory` can be used to create a subdirectory, which can then be opened with `open` to read and
/// write files
TEST(DirectoryHandle, CanCreateSubdirectory) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"subdir"};
  arene::base::filesystem::path_string const subdir_path{tmpdir.path() + "/" + subdir_name};
  arene::base::filesystem::path_string const file_name{"file.dat"};
  arene::base::filesystem::path_string const file_path{subdir_path + "/" + file_name};

  std::string const test_data = "hello";
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto create_result = dir_handle.create_subdirectory(subdir_name);
  static_assert(noexcept(dir_handle.create_subdirectory(subdir_name)), "Must be noexcept");
  ASSERT_TRUE(create_result);
  auto subdir_result = arene::base::filesystem::directory_handle::open(subdir_path);
  ASSERT_TRUE(subdir_result);
  arene::base::filesystem::directory_handle const subdir_handle = std::move(subdir_result.value());
  ASSERT_TRUE(subdir_handle.valid());

  write_test_data(file_path, test_data);

  auto open_file_result = subdir_handle.open_file(file_name);
  ASSERT_TRUE(open_file_result);
  arene::base::filesystem::file_handle const file = std::move(open_file_result.value());
  ASSERT_TRUE(file.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const res = file.read_at(buffer, 0);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size());
  for (unsigned i = 0; i < test_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte(test_data[i])) << i;
  }
}

/// @test Passing an absolute path to `create_subdirectory` returns an `error_code` with a value of `EINVAL`
TEST(DirectoryHandle, CreateSubdirectoryWithAbsolutePathIsAnError) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"subdir"};
  arene::base::filesystem::path_string const subdir_path{tmpdir.path() + "/" + subdir_name};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto create_result = dir_handle.create_subdirectory(subdir_path);
  ASSERT_FALSE(create_result);

  ASSERT_EQ(create_result.error().value(), EINVAL);
}

/// @test Invoking `create_subdirectory` with an empty path returns an `error_code` with a value of `EINVAL`
TEST(DirectoryHandle, CreateSubdirectoryWithEmptyPathIsAnError) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"subdir"};
  arene::base::filesystem::path_string const subdir_path{tmpdir.path() + "/" + subdir_name};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto create_result = dir_handle.create_subdirectory(arene::base::null_terminated_string_view{});
  ASSERT_FALSE(create_result);
  ASSERT_EQ(create_result.error().value(), EINVAL);
}

/// @test `open_subdirectory` can be used to open a subdirectory, and returns a `directory_handle` which can be used to
/// open files in that subdirectory
TEST(DirectoryHandle, CanOpenSubdirectory) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"subdir"};
  arene::base::filesystem::path_string const subdir_path{tmpdir.path() + "/" + subdir_name};
  arene::base::filesystem::path_string const file_name{"file.dat"};
  arene::base::filesystem::path_string const file_path{subdir_path + "/" + file_name};

  std::string const test_data = "hello";
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto create_result = dir_handle.create_subdirectory(subdir_name);
  ASSERT_TRUE(create_result);
  auto subdir_result = dir_handle.open_subdirectory(subdir_name);
  static_assert(noexcept(dir_handle.open_subdirectory(subdir_name)), "Must be noexcept");
  ASSERT_TRUE(subdir_result);
  arene::base::filesystem::directory_handle const subdir_handle = std::move(subdir_result.value());
  ASSERT_TRUE(subdir_handle.valid());

  write_test_data(file_path, test_data);

  auto open_file_result = subdir_handle.open_file(file_name);
  ASSERT_TRUE(open_file_result);
  arene::base::filesystem::file_handle const file = std::move(open_file_result.value());
  ASSERT_TRUE(file.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const res = file.read_at(buffer, 0);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size());
  for (unsigned i = 0; i < test_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte(test_data[i])) << i;
  }
}

/// @test The subdirectory name passed to `open_subdirectory` can have a leading `..`
TEST(DirectoryHandle, CanOpenSubdirectoryWithLeadingDotDot) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"..subdir"};
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto create_result = dir_handle.create_subdirectory(subdir_name);
  ASSERT_TRUE(create_result);
  auto subdir_result = dir_handle.open_subdirectory(subdir_name);
  static_assert(noexcept(dir_handle.open_subdirectory(subdir_name)), "Must be noexcept");
  ASSERT_TRUE(subdir_result);
  arene::base::filesystem::directory_handle const subdir_handle = std::move(subdir_result.value());
  ASSERT_TRUE(subdir_handle.valid());
}

/// @test Invoking `open_subdirectory` with an absolute path returns an `error_code` with a value of `EINVAL`
TEST(DirectoryHandle, CannotOpenSubdirectoryWithAbsolutePath) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"subdir"};
  arene::base::filesystem::path_string const subdir_path{tmpdir.path() + "/" + subdir_name};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto create_result = dir_handle.create_subdirectory(subdir_name);
  ASSERT_TRUE(create_result);
  auto subdir_result = dir_handle.open_subdirectory(subdir_path);
  ASSERT_FALSE(subdir_result);
  ASSERT_EQ(subdir_result.error().value(), EINVAL);
}

/// @test Invoking `open_subdirectory` with a relative path that traverses up the tree beyond the directory referred to
/// by the `directory_handle` returns an `error_code` with a value of `EINVAL`
TEST(DirectoryHandle, CannotOpenSubdirectoryWithRelativePathToParent) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"subdir"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto create_result = dir_handle.create_subdirectory(subdir_name);
  ASSERT_TRUE(create_result);
  auto subdir_result = dir_handle.open_subdirectory(subdir_name + "/../..");
  ASSERT_FALSE(subdir_result);
  ASSERT_EQ(subdir_result.error().value(), EINVAL);
}

/// @test Invoking `create_subdirectory` with a relative path that traverses up the tree beyond the directory referred
/// to by the `directory_handle` returns an `error_code` with a value of `EINVAL`
TEST(DirectoryHandle, CannotCreateSubdirectoryWithRelativePathToOutside) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"../subdir"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto create_result = dir_handle.create_subdirectory(subdir_name);
  ASSERT_FALSE(create_result);
  ASSERT_EQ(create_result.error().value(), EINVAL);
}

/// @test Invoking `create_subdirectory` with a complex relative path that traverses up the tree beyond the directory
/// referred to by the `directory_handle` returns an `error_code` with a value of `EINVAL`
TEST(DirectoryHandle, CannotCreateSubdirectoryWithComplexRelativePathToOutside) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"././//../subdir"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  auto create_result = dir_handle.create_subdirectory(subdir_name);
  ASSERT_FALSE(create_result);
  ASSERT_EQ(create_result.error().value(), EINVAL);
}

/// @test Invoking `create_subdirectory` with a relative path within the sub-tree from the `directory_handle` creates
/// the subdirectory, which can then be opened with `open_subdirectory`
TEST(DirectoryHandle, CanCreateSubdirectoryWithRelativePathToInside) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir1{"subdir"};
  arene::base::filesystem::path_string const subdir2{"subdir/../subdir2"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  ASSERT_TRUE(dir_handle.create_subdirectory(subdir1));
  ASSERT_TRUE(dir_handle.create_subdirectory(subdir2));
  ASSERT_TRUE(dir_handle.open_subdirectory(subdir2));
}

/// @test Invoking `create_subdirectory` with a complex relative path within the sub-tree from the `directory_handle`
/// creates the subdirectory, which can then be opened with `open_subdirectory`
TEST(DirectoryHandle, CanCreateSubdirectoryWithComplexRelativePathToInside) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir1{"subdir"};
  arene::base::filesystem::path_string const subdir2{"subdir/.././/./subdir/././../subdir2"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  ASSERT_TRUE(dir_handle.create_subdirectory(subdir1));
  ASSERT_TRUE(dir_handle.create_subdirectory(subdir2));
  ASSERT_TRUE(dir_handle.open_subdirectory(subdir2));
}

/// @test Invoking `create_subdirectory` with a complex relative path that traverses down into subdirectories and then
/// back up the tree beyond the directory referred to by the `directory_handle` returns an `error_code` with a value of
/// `EINVAL`
TEST(DirectoryHandle, CannotCreateSubdirectoryWithRelativePathToOutsideViaInside) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir1{"subdir"};
  arene::base::filesystem::path_string const subdir2{"subdir/../../subdir2"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);
  auto dir_handle = std::move(open_result.value());

  ASSERT_TRUE(dir_handle.create_subdirectory(subdir1));
  ASSERT_FALSE(dir_handle.create_subdirectory(subdir2));
}

/// @test Invoking `get_file_stats` with an absolute path returns an `error_code` with a value of `EINVAL`
TEST(DirectoryHandle, CannotGetFileStatsForAbsolutePath) {
  arene::base::filesystem::temporary_directory const tmpdir;
  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  write_test_data(file_path, "test data");
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);
  auto dir_handle = std::move(open_result.value());
  auto stats_result = dir_handle.get_file_stats(file_path);
  ASSERT_FALSE(stats_result);
  ASSERT_EQ(stats_result.error().value(), EINVAL);
}

/// @test Invoking `get_file_stats` with a relative path returns information about the file
TEST(DirectoryHandle, CanGetFileStatsForRelativePath) {
  arene::base::filesystem::temporary_directory const tmpdir;
  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "some test data";
  write_test_data(file_path, test_data);
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);
  auto dir_handle = std::move(open_result.value());
  auto stats_result = dir_handle.get_file_stats(file_name);
  ASSERT_TRUE(stats_result);
  ASSERT_EQ(stats_result->st_size, test_data.size());
}

/// @test `get_file_stats` is `noexcept`
TEST(DirectoryHandle, GetFileStatsIsNoexcept) {
  static_assert(
      noexcept(std::declval<arene::base::filesystem::directory_handle&>().get_file_stats(
          std::declval<arene::base::null_terminated_string_view>()
      )),
      "Must be noexcept"
  );
}

/// @test Invoking `get_file_stats` for a missing file returns an error
TEST(DirectoryHandle, CannotGetFileStatsForMissingFile) {
  arene::base::filesystem::temporary_directory const tmpdir;
  arene::base::filesystem::path_string const file_name{"some_file.dat"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);
  auto dir_handle = std::move(open_result.value());
  auto stats_result = dir_handle.get_file_stats(file_name);
  ASSERT_FALSE(stats_result);
}

/// @test Invoking `unlink_entry` with a path to a file and `unlink_type::other` removes the file
TEST(DirectoryHandle, CanRemoveFile) {
  arene::base::filesystem::temporary_directory const tmpdir;
  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "some test data";
  write_test_data(file_path, test_data);
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);
  auto dir_handle = std::move(open_result.value());
  ASSERT_TRUE(dir_handle.unlink_entry(file_name, arene::base::filesystem::directory_handle::unlink_type::other));
  ASSERT_FALSE(dir_handle.open_file(file_name));
}

/// @test Invoking `unlink_entry` with an absolute path to a file returns an `error_code` with the value `EINVAL`
TEST(DirectoryHandle, CannotRemoveFileWithAbsolutePath) {
  arene::base::filesystem::temporary_directory const tmpdir;
  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "some test data";
  write_test_data(file_path, test_data);
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);
  auto dir_handle = std::move(open_result.value());
  auto unlink_result =
      dir_handle.unlink_entry(file_path, arene::base::filesystem::directory_handle::unlink_type::other);
  ASSERT_FALSE(unlink_result);
  ASSERT_EQ(unlink_result.error().value(), EINVAL);
  ASSERT_TRUE(dir_handle.open_file(file_name));
}

/// @test Invoking `unlink_entry` with the name of a subdirectory and `unlink_type::directory` removes the subdirectory
TEST(DirectoryHandle, CanRemoveSubdir) {
  arene::base::filesystem::temporary_directory const tmpdir;
  arene::base::filesystem::path_string const subdir{"some_subdir"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);
  auto dir_handle = std::move(open_result.value());

  ASSERT_TRUE(dir_handle.create_subdirectory(subdir));
  ASSERT_TRUE(dir_handle.unlink_entry(subdir, arene::base::filesystem::directory_handle::unlink_type::directory));
  ASSERT_FALSE(dir_handle.open_subdirectory(subdir));
}

/// @test Invoking `unlink_entry` with the name of a subdirectory and `unlink_type::other` returns an error
TEST(DirectoryHandle, CannotRemoveSubdirAsFile) {
  arene::base::filesystem::temporary_directory const tmpdir;
  arene::base::filesystem::path_string const subdir{"some_subdir"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);
  auto dir_handle = std::move(open_result.value());

  ASSERT_TRUE(dir_handle.create_subdirectory(subdir));
  ASSERT_FALSE(dir_handle.unlink_entry(subdir, arene::base::filesystem::directory_handle::unlink_type::other));
  ASSERT_TRUE(dir_handle.open_subdirectory(subdir));
}

/// @test `unlink_entry` is `noexcept`
TEST(DirectoryHandle, UnlinkIsNoexcept) {
  static_assert(
      noexcept(std::declval<arene::base::filesystem::directory_handle&>().unlink_entry(
          std::declval<arene::base::null_terminated_string_view>(),
          arene::base::filesystem::directory_handle::unlink_type::directory
      )),
      "Must be noexcept"
  );
}

/// @test `rename` can rename a file so it cannot be opened with the old name, and can be opened with the new name
TEST(DirectoryHandle, CanRenameAFile) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "some test data for the file";
  write_test_data(file_path, test_data);

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  arene::base::filesystem::path_string const new_name{"other_file.dat"};
  auto result = dir_handle.rename(file_name, new_name);
  ASSERT_TRUE(result);
  ASSERT_FALSE(dir_handle.open_file(file_name));
  ASSERT_TRUE(dir_handle.open_file(new_name));
}

/// @test `rename` can rename a directory so it cannot be opened with the old name, and can be opened with the new name
TEST(DirectoryHandle, CanRenameADirectory) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"some_subdir"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  ASSERT_TRUE(dir_handle.create_subdirectory(subdir_name));

  arene::base::filesystem::path_string const new_name{"other_name"};
  auto result = dir_handle.rename(subdir_name, new_name);
  ASSERT_TRUE(result);
  ASSERT_FALSE(dir_handle.open_subdirectory(subdir_name));
  ASSERT_TRUE(dir_handle.open_subdirectory(new_name));
}

/// @test Invoking `rename` with the name of a missing file returns an `error_code` with the value `ENOENT`
TEST(DirectoryHandle, CannotRenameAMissingFile) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const file_name{"some_file.dat"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  arene::base::filesystem::path_string const new_name{"other_file.dat"};
  auto result = dir_handle.rename(file_name, new_name);
  ASSERT_FALSE(result);
  ASSERT_EQ(result.error().value(), ENOENT);
}

/// @test `rename` is `noexcept`
TEST(DirectoryHandle, RenameIsNoexcept) {
  static_assert(
      noexcept(std::declval<arene::base::filesystem::directory_handle&>().rename(
          std::declval<arene::base::null_terminated_string_view>(),
          std::declval<arene::base::null_terminated_string_view>()
      )),
      "Must be noexcept"
  );
}

/// @test Invoking `rename` with a path to a file in a subdirectory can move the file to a different relative path
/// within the tree
TEST(DirectoryHandle, CanRenameWithRelativePathInsideFolder) {
  arene::base::filesystem::temporary_directory const tmpdir;

  arene::base::filesystem::path_string const subdir_name{"some_subdir"};

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  ASSERT_TRUE(dir_handle.create_subdirectory(subdir_name));

  arene::base::filesystem::path_string const file_name{subdir_name + "/some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "some test data for the file";
  write_test_data(file_path, test_data);

  arene::base::filesystem::path_string const new_name{"other_file.dat"};
  auto result = dir_handle.rename(file_name, new_name);
  ASSERT_TRUE(result);
  ASSERT_FALSE(dir_handle.open_file(file_name));
  ASSERT_TRUE(dir_handle.open_file(new_name));
}

/// @test The old name for a file passed to `rename` can be an absolute path
TEST(DirectoryHandle, CanRenameFromAbsolutePath) {
  arene::base::filesystem::temporary_directory const tmpdir;

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "some test data for the file";
  write_test_data(file_path, test_data);

  arene::base::filesystem::path_string const new_name{"other_file.dat"};
  auto result = dir_handle.rename(file_path, new_name);
  ASSERT_FALSE(result);
}

/// @test The new name for a file passed to `rename` can be an absolute path
TEST(DirectoryHandle, CanRenameToAbsolutePath) {
  arene::base::filesystem::temporary_directory const tmpdir;

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());

  arene::base::filesystem::path_string const file_name{"some_file.dat"};
  arene::base::filesystem::path_string const file_path{tmpdir.path() + "/" + file_name};

  std::string const test_data = "some test data for the file";
  write_test_data(file_path, test_data);

  arene::base::filesystem::path_string const new_name{"other_file.dat"};
  auto result = dir_handle.rename(file_name, tmpdir.path() + "/" + new_name);
  ASSERT_FALSE(result);
}

/// @test `next_entry` can be used to iterate through the entries in a directory. When all the entries have been
/// exhausted, `next_entry` returns an `error_code` with the value `ERANGE`
TEST(DirectoryHandle, CanIterateThroughDirectoryElements) {
  std::set<arene::base::filesystem::path_string> entries;

  arene::base::filesystem::path_string const file1{"foo"};
  arene::base::filesystem::path_string const file2{"bar"};
  arene::base::filesystem::path_string const file3{"baz"};
  arene::base::filesystem::path_string const file4{"wibble"};

  arene::base::filesystem::temporary_directory const tmpdir;

  std::string const test_data = "hello world";

  write_test_data(tmpdir.path() + "/" + file1, test_data);
  write_test_data(tmpdir.path() + "/" + file2, test_data);
  write_test_data(tmpdir.path() + "/" + file3, test_data);
  write_test_data(tmpdir.path() + "/" + file4, test_data);

  unsigned const expected_count = 4;

  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);
  auto dir_handle = std::move(open_result.value());

  auto rewind_result = dir_handle.rewind();
  static_assert(noexcept(dir_handle.rewind()), "Must be noexcept");
  ASSERT_TRUE(rewind_result);
  for (unsigned i = 0; i < expected_count; ++i) {
    auto val = dir_handle.next_entry();
    static_assert(noexcept(dir_handle.next_entry()), "Must be noexcept");
    ASSERT_TRUE(val);
    entries.insert(arene::base::filesystem::path_string{val.value()});
  }

  auto final_val = dir_handle.next_entry();
  ASSERT_FALSE(final_val);
  ASSERT_EQ(final_val.error().value(), ERANGE);

  ASSERT_EQ(entries.size(), expected_count);

  ASSERT_EQ(entries.count(file1), 1);
  ASSERT_EQ(entries.count(file2), 1);
  ASSERT_EQ(entries.count(file3), 1);
  ASSERT_EQ(entries.count(file4), 1);
}

/// @test Invoking `rewind` on a default-constructed `directory_handle` returns an `error_code` with the value `EINVAL`
TEST(DirectoryHandle, RewindReturnsErrorOnInvalidHandle) {
  arene::base::filesystem::directory_handle handle{};
  auto res = handle.rewind();
  ASSERT_FALSE(res);
  ASSERT_EQ(res.error().value(), EINVAL);
}

/// @test Invoking `next_entry` on a default-constructed `directory_handle` returns an `error_code` with the value
/// `EINVAL`
TEST(DirectoryHandle, NextEntryReturnsErrorOnInvalidHandle) {
  arene::base::filesystem::directory_handle handle{};
  auto res = handle.next_entry();
  ASSERT_FALSE(res);
  ASSERT_EQ(res.error().value(), EINVAL);
}

/// @test Invoking `next_entry` on a `directory_handle` that refers to a directory that has been deleted returns an
/// error.
TEST(DirectoryHandle, IteratingThroughDeletedDirIsError) {
  arene::base::filesystem::directory_handle handle{};

  {
    arene::base::filesystem::temporary_directory const tmpdir;

    auto open_res = arene::base::filesystem::directory_handle::open(tmpdir.path());
    ASSERT_TRUE(open_res);
    handle = std::move(open_res.value());
  }

  auto res = handle.next_entry();
  ASSERT_FALSE(res);
}

/// @test `create_temporary_file` returns a `file_handle` that can be used to read from and write the file, but the file
/// is not present in the directory
TEST(DirectoryHandle, CanCreateTemporaryFileInDirectory) {
  arene::base::filesystem::temporary_directory const tmpdir;

  std::string const test_data = "hello";
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());
  auto create_file_result = dir_handle.create_temporary_file();
  static_assert(noexcept(dir_handle.create_temporary_file()), "Must be noexcept");
  if (!create_file_result) {
    EXPECT_STREQ(create_file_result.error().message().c_str(), "");
    ASSERT_EQ(create_file_result.error().value(), 0);
  }
  ASSERT_TRUE(create_file_result);
  arene::base::filesystem::file_handle const file = std::move(create_file_result.value());

  unsigned const offset = 0;

  auto write_result = file.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), offset);
  auto const remaining = write_result.value();
  ASSERT_EQ(remaining.size(), 0);

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const res = file.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size());
  for (unsigned i = 0; i < test_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte(test_data[i])) << i;
  }

  ASSERT_TRUE(dir_handle.rewind());
  ASSERT_FALSE(dir_handle.next_entry());
}

/// @test If `create_temporary_file` is invoked on a `directory_handle` that refers to a directory that the current user
/// does not have permissions to write to, then it returns an error
TEST(DirectoryHandle, CannotCreateTemporaryFileInUnwritableDirectory) {
  arene::base::filesystem::temporary_directory const tmpdir;

  std::string const test_data = "hello";
  auto open_result = arene::base::filesystem::directory_handle::open(tmpdir.path());
  ASSERT_TRUE(open_result);

  auto dir_handle = std::move(open_result.value());
  ASSERT_EQ(chmod(tmpdir.path().c_str(), 0), 0);
  auto create_file_result = dir_handle.create_temporary_file();
  static_assert(noexcept(dir_handle.create_temporary_file()), "Must be noexcept");
  ASSERT_FALSE(create_file_result);
  ASSERT_STRNE(create_file_result.error().message().c_str(), "");
  ASSERT_NE(create_file_result.error().value(), 0);
}

}  // namespace
