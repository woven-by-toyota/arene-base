// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/bind_back.hpp"

namespace {
class non_moveable_object {
 public:
  non_moveable_object() = default;
  ~non_moveable_object() = default;

  non_moveable_object(non_moveable_object const&) = delete;
  non_moveable_object(non_moveable_object&&) = delete;
  auto operator=(non_moveable_object const&) -> non_moveable_object& = delete;
  auto operator=(non_moveable_object&&) -> non_moveable_object& = delete;
};

}  // namespace

auto main() -> int {
  return ::arene::base::bind_back([](non_moveable_object&&) {}, non_moveable_object{})() == 0;
}
