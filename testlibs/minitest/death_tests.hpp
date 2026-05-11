// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_DEATH_TESTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_DEATH_TESTS_HPP_

/// @file death_tests.hpp
///
/// A header which defines death test macros for minitest, either to do a test if supported or to deliberately break
/// compilation if not.

#ifdef MINITEST_DEATH_TESTS_ENABLED
#include "testlibs/minitest/death_tests_enabled.hpp"  // IWYU pragma: export
#else
#include "testlibs/minitest/death_tests_disabled.hpp"  // IWYU pragma: export
#endif

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_DEATH_TESTS_HPP_
