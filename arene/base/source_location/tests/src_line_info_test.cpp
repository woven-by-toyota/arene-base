// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/source_location/src_line_info.hpp"

#include <gtest/gtest.h>

#include "arene/base/strings/null_terminated_string_view.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

constexpr auto prepare_for_the_trouble_and_make_it_double() -> arene::base::src_line_info {
  // we test the line and function name here, do NOT move this code
  return ARENE_GET_SRC_CODE_LOCATION_INFO();
}

/// @test Source information (line number, function name, and file path) is correct.
TEST(SrcLineInfo, TestSrcLineInfo) {
  constexpr auto func_info = prepare_for_the_trouble_and_make_it_double();

  STATIC_ASSERT_EQ(func_info.line, 17);

  // This assertion can not be static, because GCC8 won't use __func__ in constexpr.
  ASSERT_STREQ(func_info.function_name.c_str(), "prepare_for_the_trouble_and_make_it_double");

  constexpr arene::base::string_view filepath_view(func_info.filepath);
  STATIC_ASSERT_TRUE(filepath_view.find("src_line_info_test.cpp") != arene::base::string_view::npos);
}

}  // namespace
