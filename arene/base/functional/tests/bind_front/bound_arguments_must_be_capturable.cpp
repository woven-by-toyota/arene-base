// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/bind_front.hpp"

namespace {
class non_copyable_object {
 public:
  non_copyable_object() = default;
  ~non_copyable_object() = default;

  non_copyable_object(non_copyable_object const&) = delete;
  non_copyable_object(non_copyable_object&&) = default;
  auto operator=(non_copyable_object const&) -> non_copyable_object& = delete;
  auto operator=(non_copyable_object&&) -> non_copyable_object& = default;
};

}  // namespace

auto main() -> int {
  auto non_copyable = non_copyable_object{};
  return ::arene::base::bind_front([](non_copyable_object) {}, non_copyable)() == 0;
}
