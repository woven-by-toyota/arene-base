// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/file_handle.hpp"

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstddef>
#include <fstream>
#include <limits>
#include <string>
#include <type_traits>

#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/byte.hpp"
#include "arene/base/filesystem/error_code.hpp"
#include "arene/base/filesystem/path_string.hpp"
#include "arene/base/filesystem/temporary_directory.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/iterator.hpp"
#include "arene/base/result.hpp"
#include "arene/base/span.hpp"
#include "arene/base/string_view.hpp"

// NOLINTBEGIN(hicpp-vararg) This unit test explicitly calls platform APIs for file operations which are c-APIs.

namespace {
struct FileHandleFixture : testing::Test {
  arene::base::filesystem::temporary_directory const tmpdir;
  arene::base::filesystem::path_string const file_path;

  FileHandleFixture()
      : file_path(tmpdir.path() + "/test_file.dat") {}

  void write_test_data(std::string const& test_data) {
    std::filebuf file;
    ASSERT_NE(file.open(file_path.c_str(), std::ios::out | std::ios::binary), nullptr);

    auto const data_size = static_cast<std::streamsize>(test_data.size());
    auto const written = file.sputn(test_data.c_str(), data_size);
    ASSERT_EQ(written, test_data.size());
    ASSERT_NE(file.close(), nullptr);
  }
};

/// @test After constructing a `file_handle` with a handle returned from `open`, `read_at` can be used to read from the
/// file into a buffer.
TEST_F(FileHandleFixture, CanReadViaFileHandle) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_RDONLY | O_CLOEXEC));
  ASSERT_TRUE(handle.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const res = handle.read_at(buffer, 0);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size());
  ASSERT_EQ(read_buffer.data(), buffer.data());
  for (unsigned idx = 0; idx < test_data.size(); ++idx) {
    ASSERT_EQ(read_buffer[idx], arene::base::byte(test_data[idx])) << idx;
  }
}

/// @test After constructing a `file_handle` with a handle returned from `open`, `read_at` can be used to read from the
/// file into a buffer at a non-zero offset from the beginning of the file
TEST_F(FileHandleFixture, CanReadViaFileHandleWithNonZeroOffset) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_RDONLY | O_CLOEXEC));
  ASSERT_TRUE(handle.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 5U;
  auto const res = handle.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size() - offset);
  ASSERT_EQ(read_buffer.data(), buffer.data());
  for (unsigned i = offset; i < test_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i - offset], arene::base::byte(test_data[i])) << i;
  }
}

/// @test After constructing a `file_handle` with a handle returned from `open`, invoking `read_at` with an offset that
/// is the size of the file successfully reads zero bytes
TEST_F(FileHandleFixture, CanReadViaFileHandleAtEOF) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_RDONLY | O_CLOEXEC));
  ASSERT_TRUE(handle.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const offset = handle.size().value();
  auto const res = handle.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), 0);
  ASSERT_EQ(read_buffer.data(), buffer.data());
}

/// @test After constructing a `file_handle` with a handle returned from `open`, invoking `read_at` with a small buffer
/// correctly reads the data into the buffer
TEST_F(FileHandleFixture, CanReadViaFileHandleWithShortLength) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_RDONLY | O_CLOEXEC));
  ASSERT_TRUE(handle.valid());

  constexpr unsigned buffer_size = 3U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 5U;
  auto const res = handle.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), buffer.size());
  ASSERT_EQ(read_buffer.data(), buffer.data());
  for (unsigned i = 0; i < buffer.size(); ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte(test_data[i + offset])) << i;
  }
}

/// @test After constructing a `file_handle` with a handle returned from `open`, invoking `read_at` with a zero-size
/// buffer successfully reads no data into the buffer
TEST_F(FileHandleFixture, CanReadViaFileHandleWithEmptyBuffer) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_RDONLY | O_CLOEXEC));
  ASSERT_TRUE(handle.valid());

  std::array<arene::base::byte, 0U> buffer{};

  unsigned const offset = 5U;
  auto const res = handle.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), buffer.size());
  ASSERT_EQ(read_buffer.data(), buffer.data());
}

/// @test After constructing a `file_handle` with a handle returned from `open`, invoking `read_at` with an offset
/// beyond the end of the file successfully reads zero bytes
TEST_F(FileHandleFixture, ReadingatOffsetBeyondEndOfFileReturnsEmptyBuffer) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_RDONLY | O_CLOEXEC));
  ASSERT_TRUE(handle.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const offset = test_data.size() + 10;
  auto const res = handle.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), 0);
  ASSERT_EQ(read_buffer.data(), buffer.data());
}

/// @test Invoking `read_at` on a default-constructed `file_handle` returns an `error_code` with the value `EBADF`
TEST(FileHandle, ReadingFromInvalidHandleIsError) {
  arene::base::filesystem::file_handle const handle{};
  ASSERT_FALSE(handle.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const res = handle.read_at(buffer, 0);
  ASSERT_FALSE(res);
  static_assert(
      std::is_same<
          decltype(res),
          arene::base::result<arene::base::span<arene::base::byte>, arene::base::filesystem::error_code> const>::value,
      "Correct type"
  );
  ASSERT_EQ(res.error().value(), EBADF);
}

/// @test After constructing a `file_handle` with a writable handle returned from `open`, invoking `write_at` writes the
/// supplied data to the file, so a subsequent call to `read_at` can read the data
TEST_F(FileHandleFixture, CanWriteThroughHandle) {
  std::string const test_data = "hello world";

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600));
  ASSERT_TRUE(handle.valid());

  auto write_result = handle.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), 0);
  ASSERT_TRUE(write_result);
  auto const remaining = write_result.value();
  ASSERT_EQ(remaining.size(), 0);
  ASSERT_EQ(remaining.data(), static_cast<void const*>(&test_data[test_data.size()]));

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 0;
  auto const res = handle.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size());
  for (unsigned idx = 0; idx < test_data.size(); ++idx) {
    ASSERT_EQ(read_buffer[idx], arene::base::byte(test_data[idx])) << idx;
  }
}

/// @test After constructing a `file_handle` with a writable handle returned from `open`, invoking `write_at` writes the
/// supplied data to the file at the specified offset, so a subsequent call to `read_at` can read the data. The bytes
/// read prior to the specified offset are zero.
TEST_F(FileHandleFixture, CanWriteThroughHandleToOffset) {
  std::string const test_data = "hello world";

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600));
  ASSERT_TRUE(handle.valid());

  unsigned const write_offset = 10;
  auto const write_result =
      handle.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), write_offset);
  ASSERT_TRUE(write_result);
  auto const remaining = write_result.value();
  ASSERT_EQ(remaining.size(), 0);
  ASSERT_EQ(remaining.data(), static_cast<void const*>(&test_data[test_data.size()]));

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 0;
  auto const res = handle.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), test_data.size() + write_offset);
  for (unsigned idx = 0; idx < test_data.size(); ++idx) {
    ASSERT_EQ(read_buffer[idx + write_offset], arene::base::byte(test_data[idx])) << idx;
  }
  for (unsigned idx = 0; idx < write_offset; ++idx) {
    ASSERT_EQ(read_buffer[idx], arene::base::byte{}) << idx;
  }
}

/// @test After constructing a `file_handle` with a writable handle returned from `open`, invoking `write_at` writes the
/// supplied data to the file at the specified offset, overwriting existing data at that offset, so a subsequent call to
/// `read_at` can read the data
TEST_F(FileHandleFixture, CanWriteThroughHandleToOffsetInExistingData) {
  unsigned const base_size = 64;
  std::string const initial_data(base_size, 'A');
  std::string const test_data = "hello world";

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600));
  ASSERT_TRUE(handle.valid());

  auto write_result = handle.write_at(arene::base::as_bytes(arene::base::span<char const>(initial_data)), 0);
  ASSERT_TRUE(write_result);
  ASSERT_EQ(write_result.value().size(), 0);

  unsigned const write_offset = 10;
  write_result = handle.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), write_offset);
  ASSERT_TRUE(write_result);
  auto const remaining = write_result.value();
  ASSERT_EQ(remaining.size(), 0);
  ASSERT_EQ(remaining.data(), static_cast<void const*>(&test_data[test_data.size()]));

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 0;
  auto const res = handle.read_at(buffer, offset);
  ASSERT_TRUE(res);
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

/// @test After constructing a `file_handle` with a writable handle returned from `open`, invoking `write_at` writes the
/// supplied data to the file at the specified offset, so a subsequent call to `read_at` can read the data. If data is
/// written at an offset beyond the previous end of the file, then the file is exended with zero bytes up to the
/// specified offset.
TEST_F(FileHandleFixture, CanWriteThroughHandleToOffsetBeyondEndOfFile) {
  unsigned const base_size = 64;
  std::string const initial_data(base_size, 'A');
  std::string const test_data = "hello world";

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600));
  ASSERT_TRUE(handle.valid());

  auto write_result = handle.write_at(arene::base::as_bytes(arene::base::span<char const>(initial_data)), 0);
  ASSERT_TRUE(write_result);
  ASSERT_EQ(write_result.value().size(), 0);

  auto const write_offset = 10U + base_size;
  write_result = handle.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), write_offset);
  ASSERT_TRUE(write_result);
  auto const remaining = write_result.value();
  ASSERT_EQ(remaining.size(), 0);
  ASSERT_EQ(remaining.data(), static_cast<void const*>(&test_data[test_data.size()]));

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  ASSERT_LT(write_offset + test_data.size(), buffer_size);
  unsigned const offset = 0;
  auto const res = handle.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), write_offset + test_data.size());
  for (auto i = 0U; i < test_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i + write_offset], arene::base::byte(test_data[i])) << i;
  }
  for (auto i = 0U; i < base_size; ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte('A')) << i;
  }
  for (auto i = base_size; i < write_offset; ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::to_byte(0)) << i;
  }
}

/// @test After constructing a `file_handle` with a read-only handle returned from `open`, invoking `write_at` returns
/// an `error_code` with the value `EBADF` and the file contents are unchanged
TEST_F(FileHandleFixture, CannotWriteIfFileIsOpenedReadOnly) {
  unsigned const base_size = 64;
  std::string const initial_data(base_size, 'A');
  std::string const test_data = "hello world";

  write_test_data(initial_data);

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_RDONLY | O_CLOEXEC));
  ASSERT_TRUE(handle.valid());

  auto write_result = handle.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), 0);
  ASSERT_FALSE(write_result);
  ASSERT_EQ(write_result.error().value(), EBADF);

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  unsigned const offset = 0;
  auto const res = handle.read_at(buffer, offset);
  ASSERT_TRUE(res);
  auto const read_buffer = res.value();
  ASSERT_EQ(read_buffer.size(), initial_data.size());
  for (unsigned i = 0; i < initial_data.size(); ++i) {
    ASSERT_EQ(read_buffer[i], arene::base::byte('A')) << i;
  }
}

/// @test Invoking `flush` on a default-constructed `file_handle` returns an error
TEST(FileHandle, CannotFlushIfNoHandle) {
  arene::base::filesystem::file_handle const handle{};

  static_assert(noexcept(handle.flush()), "Must be noexcept");
  static_assert(
      std::is_same<decltype(handle.flush()), arene::base::result<void, arene::base::filesystem::error_code>>::value,
      "Correct type"
  );
  auto const res = handle.flush();
  ASSERT_FALSE(res);
  ASSERT_EQ(res.error().value(), EBADF);
}

/// @test After writing to a file with `write_at`, data can be flushed with `flush`
TEST_F(FileHandleFixture, CanFlushWritableHandles) {
  std::string const test_data = "hello world";

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600));
  ASSERT_TRUE(handle.valid());
  ASSERT_TRUE(handle.flush());

  auto write_result = handle.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), 0);
  ASSERT_TRUE(write_result);
  ASSERT_TRUE(handle.flush());
}

/// @test `read_at` is `noexcept`
TEST(FileHandle, ReadingIsNoexcept) {
  static_assert(
      noexcept(std::declval<arene::base::filesystem::file_handle&>()
                   .read_at(std::declval<arene::base::span<arene::base::byte>>(), std::declval<std::uint64_t>())),
      "Must be noexcept"
  );
}

/// @test `write_at` is `noexcept`
TEST(FileHandle, WritingIsNoexcept) {
  static_assert(
      noexcept(std::declval<arene::base::filesystem::file_handle&>().write_at(
          std::declval<arene::base::span<arene::base::byte const>>(),
          std::declval<std::uint64_t>()
      )),
      "Must be noexcept"
  );
}

/// @test After invoking `close` on a `file_handle`, `valid` returns `false`
TEST_F(FileHandleFixture, AfterCloseHandleIsNotValid) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  arene::base::filesystem::file_handle handle(::open(file_path.c_str(), O_RDONLY | O_CLOEXEC));
  ASSERT_TRUE(handle.valid());
  ASSERT_TRUE(handle.close());
  ASSERT_FALSE(handle.valid());
}

/// @test After invoking `close`, the raw file handle is invalid, so if it is passed to another `file_handle`,
/// operations on that `file_handle` will return an error.
TEST_F(FileHandleFixture, AfterCloseUnderlyingHandleIsClosed) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  int const raw_handle = ::open(file_path.c_str(), O_RDONLY | O_CLOEXEC);
  arene::base::filesystem::file_handle handle(raw_handle);
  ASSERT_TRUE(handle.valid());
  ASSERT_TRUE(handle.close());

  arene::base::filesystem::file_handle const handle2(raw_handle);
  ASSERT_TRUE(handle2.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const res = handle2.read_at(buffer, 0);
  ASSERT_FALSE(res);
}

/// @test After destroying a `file_handle`, the raw file handle is invalid, so if it is passed to another
/// `file_handle`, operations on that `file_handle` will return an error.
TEST_F(FileHandleFixture, DestructorClosesUnderlyingHandle) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  int const raw_handle = ::open(file_path.c_str(), O_RDONLY | O_CLOEXEC);
  { arene::base::filesystem::file_handle const handle(raw_handle); }

  arene::base::filesystem::file_handle const handle2(raw_handle);
  ASSERT_TRUE(handle2.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const res = handle2.read_at(buffer, 0);
  ASSERT_FALSE(res);
}

/// @test `close` returns an error if the raw file handle is invalid
TEST_F(FileHandleFixture, CloseReportsErrors) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  int const raw_handle = ::open(file_path.c_str(), O_RDONLY | O_CLOEXEC);
  { arene::base::filesystem::file_handle const handle(raw_handle); }

  arene::base::filesystem::file_handle handle2(raw_handle);
  ASSERT_TRUE(handle2.valid());

  auto const close_result = handle2.close();
  ASSERT_FALSE(close_result);
  ASSERT_EQ(close_result.error().value(), EBADF);
}

/// @test `close` is `noexcept`
TEST(FileHandle, CloseIsNoexcept) {
  static_assert(noexcept(std::declval<arene::base::filesystem::file_handle&>().close()), "Must be noexcept");
}

/// @test Invoking `close` on a default-constructed `file_handle` is OK
TEST(FileHandle, CloseDoesNothingOnInvalidHandle) { ASSERT_TRUE(arene::base::filesystem::file_handle{}.close()); }

/// @test `size` returns the size of the file referenced by the `file_handle`
TEST_F(FileHandleFixture, CanGetFileSize) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_RDONLY | O_CLOEXEC));
  ASSERT_TRUE(handle.valid());

  auto const size_result = handle.size();
  ASSERT_TRUE(size_result);
  ASSERT_EQ(size_result.value(), test_data.size());
  static_assert(noexcept(handle.size()), "Must be noexcept");
  static_assert(
      std::is_same<decltype(handle.size()), arene::base::result<std::uint64_t, arene::base::filesystem::error_code>>::
          value,
      "Must be right type"
  );
}

/// @test Invoking `size` on a default-constructed `file_handle` returns an `error_code`
TEST(FileHandle, GetFileSizeFailsOnInvalidHandle) {
  arene::base::filesystem::file_handle const handle{};
  ASSERT_FALSE(handle.valid());

  auto const size_result = handle.size();
  ASSERT_FALSE(size_result);
  ASSERT_EQ(size_result.error().value(), EBADF);
}

/// @test `size` returns the size of the file referenced by the `file_handle` including freshly-written data
TEST_F(FileHandleFixture, FileSizeIncludesWrittenData) {
  std::string const test_data = "hello world";

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600));
  ASSERT_TRUE(handle.valid());

  unsigned const write_offset = 10;
  auto const write_result =
      handle.write_at(arene::base::as_bytes(arene::base::span<char const>(test_data)), write_offset);
  ASSERT_TRUE(write_result);
  auto const remaining = write_result.value();
  ASSERT_EQ(remaining.size(), 0);

  auto const size_result = handle.size();
  ASSERT_TRUE(size_result);
  ASSERT_EQ(size_result.value(), test_data.size() + write_offset);
}

/// @test After move-assignment the source `file_handle` is no longer `valid`, and the destination is now `valid`, and
/// `get_fd` returns the raw file handle of the source
TEST_F(FileHandleFixture, AfterMoveAssignmentHandleIsTransferred) {
  arene::base::filesystem::file_handle handle(::open(file_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600));
  ASSERT_TRUE(handle.valid());

  auto const raw_handle = handle.get_fd();
  arene::base::filesystem::file_handle handle2{};
  handle2 = std::move(handle);

  ASSERT_TRUE(handle2.valid());
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move,bugprone-use-after-move,hicpp-invalid-access-moved)
  ASSERT_FALSE(handle.valid());
  ASSERT_EQ(handle2.get_fd(), raw_handle);
}

/// @test Invoking `read_at` with a position that is larger than the maximum permitted value of `off_t` returns an
/// `error_code` with the value `EOVERFLOW`
TEST_F(FileHandleFixture, ReadingWithOutOfRangePositionFails) {
  std::string const test_data = "hello world";

  write_test_data(test_data);

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_RDONLY | O_CLOEXEC));
  ASSERT_TRUE(handle.valid());

  constexpr unsigned buffer_size = 100U;
  std::array<arene::base::byte, buffer_size> buffer{};

  auto const res = handle.read_at(buffer, static_cast<std::uint64_t>(std::numeric_limits<::off_t>::max()) + 1);
  ASSERT_FALSE(res);
  ASSERT_EQ(res.error().value(), EOVERFLOW);
}

/// @test Invoking `write_at` with a position that is larger than the maximum permitted value of `off_t` returns an
/// `error_code` with the value `EOVERFLOW`
TEST_F(FileHandleFixture, WritingWithOutOfRangePositionFails) {
  std::string const test_data = "hello world";

  arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600));
  ASSERT_TRUE(handle.valid());

  auto const write_result = handle.write_at(
      arene::base::as_bytes(arene::base::span<char const>(test_data)),
      static_cast<std::uint64_t>(std::numeric_limits<::off_t>::max()) + 1
  );

  ASSERT_FALSE(write_result);
  ASSERT_EQ(write_result.error().value(), EOVERFLOW);
}

/// @test `sequential_write` and `sequential_read` can be used to write to and read from a non-seekable pipe
TEST(FileHandle, WritingToPipe) {
  constexpr arene::base::string_view test_data{"hello world"};
  arene::base::array<int, 2> handles{};
  // NOLINTNEXTLINE(android-cloexec-pipe)
  ASSERT_EQ(pipe(handles.data()), 0);

  arene::base::filesystem::file_handle const read_handle(handles[0]);
  arene::base::filesystem::file_handle const write_handle(handles[1]);

  auto const write_result =
      write_handle.sequential_write(arene::base::as_bytes(arene::base::span<char const>(test_data)));

  ASSERT_TRUE(write_result);

  arene::base::array<arene::base::byte, test_data.size()> buffer{};

  auto const read_result = read_handle.sequential_read(buffer);

  ASSERT_TRUE(read_result);

  ASSERT_EQ(read_result.value().size(), test_data.size());
  for (std::size_t idx = 0; idx < test_data.size(); ++idx) {
    ASSERT_EQ(static_cast<char>(buffer[idx]), test_data[idx]);
  }
}

/// @test `sequential_write` can be used to write data to the end of the file, without needing to specify the
/// destination offset. `sequential_read` can be used to read the contents of a file in chunks from beginning to end,
/// without needing to remember the offset to read from
TEST_F(FileHandleFixture, SequentialWritingToFile) {
  constexpr arene::base::string_view test_data1{"hello world"};
  constexpr arene::base::string_view test_data2{"goodbye"};

  {
    arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600));
    ASSERT_TRUE(handle.valid());

    {
      auto const write_result =
          handle.sequential_write(arene::base::as_bytes(arene::base::span<char const>(test_data1)));
      ASSERT_TRUE(write_result);
      auto const remaining = write_result.value();
      ASSERT_EQ(remaining.size(), 0);
      ASSERT_EQ(remaining.data(), static_cast<void const*>(arene::base::next(test_data1.data(), test_data1.size())));
    }
    {
      auto const write_result =
          handle.sequential_write(arene::base::as_bytes(arene::base::span<char const>(test_data2)));

      ASSERT_TRUE(write_result);
      auto const remaining2 = write_result.value();
      ASSERT_EQ(remaining2.size(), 0);
      ASSERT_EQ(remaining2.data(), static_cast<void const*>(arene::base::next(test_data2.data(), test_data2.size())));
    }
  }

  {
    arene::base::filesystem::file_handle const handle2(::open(file_path.c_str(), O_RDWR | O_CLOEXEC, 0600));
    ASSERT_TRUE(handle2.valid());

    constexpr unsigned buffer_size = test_data1.size();
    std::array<arene::base::byte, buffer_size> buffer{};
    {
      auto const res = handle2.sequential_read(buffer);
      ASSERT_TRUE(res);
      auto const read_buffer = res.value();
      ASSERT_EQ(read_buffer.size(), test_data1.size());
      for (unsigned i = 0; i < test_data1.size(); ++i) {
        ASSERT_EQ(read_buffer[i], arene::base::byte(test_data1[i])) << i;
      }
    }
    {
      auto const res2 = handle2.sequential_read(buffer);
      ASSERT_TRUE(res2);
      auto const read_buffer2 = res2.value();
      EXPECT_EQ(read_buffer2.size(), test_data2.size());
      for (unsigned i = 0; i < test_data2.size(); ++i) {
        EXPECT_EQ(read_buffer2[i], arene::base::byte(test_data2[i])) << i;
      }
    }
  }
}

/// @test `sequential_seek` can be used to adjust the position at which to write the next chunk of data with
/// `sequential_write`
TEST_F(FileHandleFixture, SequentialWritingToFileWithSeek) {
  constexpr arene::base::string_view test_data1{"hello world"};
  constexpr arene::base::string_view test_data2{"goodbye"};

  {
    arene::base::filesystem::file_handle const handle(::open(file_path.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600));
    ASSERT_TRUE(handle.valid());

    {
      auto write_result = handle.sequential_write(arene::base::as_bytes(arene::base::span<char const>(test_data1)));
      ASSERT_TRUE(write_result);
      auto const remaining = write_result.value();
      ASSERT_EQ(remaining.size(), 0);
      ASSERT_EQ(remaining.data(), static_cast<void const*>(arene::base::next(test_data1.data(), test_data1.size())));
    }
    ASSERT_TRUE(handle.sequential_seek(2));
    {
      auto write_result = handle.sequential_write(arene::base::as_bytes(arene::base::span<char const>(test_data2)));

      ASSERT_TRUE(write_result);
      auto const remaining2 = write_result.value();
      ASSERT_EQ(remaining2.size(), 0);
      ASSERT_EQ(remaining2.data(), static_cast<void const*>(arene::base::next(test_data2.data(), test_data2.size())));
    }
  }

  {
    arene::base::filesystem::file_handle const handle2(::open(file_path.c_str(), O_RDWR | O_CLOEXEC, 0600));
    ASSERT_TRUE(handle2.valid());

    constexpr unsigned buffer_size = 100U;
    std::array<arene::base::byte, buffer_size> buffer{};
    auto const res = handle2.sequential_read(buffer);
    ASSERT_TRUE(res);
    auto const read_buffer = res.value();

    constexpr arene::base::string_view expected{"hegoodbyeld"};

    ASSERT_EQ(read_buffer.size(), expected.size());
    for (unsigned i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(read_buffer[i], arene::base::byte(expected[i])) << i;
    }
  }
}

}  // namespace

// NOLINTEND(hicpp-vararg)
