// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_TMP_STDLIB_OPERATOR_NEW_DELETE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_TMP_STDLIB_OPERATOR_NEW_DELETE_HPP_

// These aren't defined when we provide `//stdlib`, but it probably gets
// included since arene toolchains *always* pass an option to link against the
// C++ standard library that cannot be disabled.
// @{
auto operator delete(void*) -> void;
auto operator delete(void*, unsigned int) -> void;
auto operator delete[](void*) -> void;
auto operator delete[](void*, unsigned int) -> void;
auto operator new(decltype(sizeof(int))) -> void*;
auto operator new[](decltype(sizeof(int))) -> void*;
// @}
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_TMP_STDLIB_OPERATOR_NEW_DELETE_HPP_
