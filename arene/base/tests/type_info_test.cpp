// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file type_info_test.cpp
/// @brief Provides unit tests to validate export of content in type_info.hpp
///

#include "arene/base/type_info.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `type_name_v` is declared
TEST(TypeInfo, TypeNameV) { using arene::base::type_name_v; }

}  // namespace
