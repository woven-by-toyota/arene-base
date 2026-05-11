// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/bind_front.hpp"

namespace {
class non_copyable_function_object {
 public:
  non_copyable_function_object() = default;
  ~non_copyable_function_object() = default;

  non_copyable_function_object(non_copyable_function_object const&) = delete;
  non_copyable_function_object(non_copyable_function_object&&) = default;
  auto operator=(non_copyable_function_object const&) -> non_copyable_function_object& = delete;
  auto operator=(non_copyable_function_object&&) -> non_copyable_function_object& = default;
  auto operator()(int val) const -> int { return val; }
};

}  // namespace

auto main() -> int {
  auto func = non_copyable_function_object{};
  return ::arene::base::bind_front(func, 10)() == 0;
}
