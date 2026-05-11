// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_DETAIL_DEATH_TESTS_DISABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_DETAIL_DEATH_TESTS_DISABLED_HPP_

/// @brief Macro to assert that the given expression will cause abnormal termination with a particular message on stderr
/// @note The version in this header is disabled, so it will just skip the containing test case.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_DEATH(expr, message)                                         \
  static_assert(false, "ASSERT_DEATH is not implemented on this platform"); \
  (+::testing::stream_insertion_sink{})

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_DETAIL_DEATH_TESTS_DISABLED_HPP_
