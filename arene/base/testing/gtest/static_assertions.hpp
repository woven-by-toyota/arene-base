// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_STATIC_ASSERTIONS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_STATIC_ASSERTIONS_HPP_

// IWYU pragma: private, include "arene/base/testing/gtest.hpp"
// IWYU pragma: friend "arene/base/testing/gtest/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest/static_assertions_config.hpp"

#if ARENE_IS_ON(ARENE_GTEST_STATIC_ASSERTIONS)
#include "arene/base/testing/gtest/detail/static_assertions_enabled.hpp"  // IWYU pragma: export
#else
#include "arene/base/testing/gtest/detail/static_assertions_disabled.hpp"  // IWYU pragma: export
#endif
// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_STATIC_ASSERTIONS_HPP_
