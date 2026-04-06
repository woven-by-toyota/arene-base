// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/source_location.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `ARENE_GET_SRC_CODE_LOCATION_INFO` is declared
TEST(SourceLocation, AreneGetSrcCodeLocationInfoIsExported) {
#ifndef ARENE_GET_SRC_CODE_LOCATION_INFO
  static_assert(false, "Source location info not supported");
#endif
}

/// @test `src_line_info` is declared
TEST(SourceLocation, SrcLineInfoIsExported) { using ::arene::base::src_line_info; }

}  // namespace
