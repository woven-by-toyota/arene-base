// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/path_string.hpp"

#include <climits>
#include <cstddef>
#include <utility>

#include <gtest/gtest.h>

#include "arene/base/array.hpp"

namespace {

using ::arene::base::array;
using ::arene::base::inline_string;
using ::arene::base::filesystem::path_buffer;
using ::arene::base::filesystem::path_string;

template <std::size_t N>
constexpr auto size(array<char, N>) -> std::size_t {
  return N;
}

template <std::size_t N>
constexpr auto size(inline_string<N>) -> std::size_t {
  return N;
}

/// @test The capacity of `path_string` is `PATH_MAX`
TEST(PathString, IsSizePathMax) {
  path_string const str{};
  ASSERT_TRUE(size(str) == PATH_MAX);
}
/// @test The size of `path_buffer` is `PATH_MAX`
TEST(PathBuffer, IsSizePathMax) {
  path_buffer const buffer{};
  ASSERT_TRUE(size(buffer) == PATH_MAX);
}

}  // namespace
