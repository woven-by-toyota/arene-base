// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/tmp_stdlib_operator_new_delete.hpp"

auto operator delete(void*) -> void {}
auto operator delete(void*, unsigned int) -> void {}
auto operator delete[](void*) -> void {}
auto operator delete[](void*, unsigned int) -> void {}
auto operator new(decltype(sizeof(int))) -> void* { return nullptr; }
auto operator new[](decltype(sizeof(int))) -> void* { return nullptr; }
