// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file endian_test.cpp
/// @brief Provides unit tests to validate export of content in endian.hpp
///

#include "arene/base/endian.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Ensure `endian` is exported
TEST(Endian, Endian) { using arene::base::endian; }

/// @test Ensure `read_little_endian` is exported
TEST(Endian, ReadLittleEndian) { using arene::base::read_little_endian; }

/// @test Ensure `write_little_endian` is exported
TEST(Endian, WriteLittleEndian) { using arene::base::write_little_endian; }

/// @test Ensure `read_big_endian` is exported
TEST(Endian, ReadBigEndian) { using arene::base::read_big_endian; }

/// @test Ensure `write_big_endian` is exported
TEST(Endian, WriteBigEndian) { using arene::base::write_big_endian; }

}  // namespace
