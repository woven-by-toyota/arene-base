// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/not_fn.hpp"

namespace {
class non_moveable_function_object {
 public:
  non_moveable_function_object() = default;
  ~non_moveable_function_object() = default;

  non_moveable_function_object(non_moveable_function_object const&) = delete;
  non_moveable_function_object(non_moveable_function_object&&) = delete;
  auto operator=(non_moveable_function_object const&) -> non_moveable_function_object& = delete;
  auto operator=(non_moveable_function_object&&) -> non_moveable_function_object& = delete;
  auto operator()(int val) const -> bool { return val < 0; }
};

}  // namespace

auto main() -> int {
  non_moveable_function_object func{};
  auto const not_fn_func = ::arene::base::not_fn(func);
  return -1;
}
