// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/bind_back.hpp"

namespace {
class non_moveable_function_object {
 public:
  non_moveable_function_object() = default;
  ~non_moveable_function_object() = default;

  non_moveable_function_object(non_moveable_function_object const&) = delete;
  non_moveable_function_object(non_moveable_function_object&&) = delete;
  auto operator=(non_moveable_function_object const&) -> non_moveable_function_object& = delete;
  auto operator=(non_moveable_function_object&&) -> non_moveable_function_object& = delete;
  auto operator()(int val) const -> int { return val; }
};

}  // namespace

auto main() -> int {
  auto func = non_moveable_function_object{};
  return ::arene::base::bind_back(std::move(func), 10)() == 0;
}
