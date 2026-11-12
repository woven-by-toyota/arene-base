// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>

#include "arene/base/variant/variant.hpp"
#include "arene/base/variant/variant_cast.hpp"

using ::arene::base::variant;
using ::arene::base::variant_cast;

int main() {
  using variant_with_unique_ptr_and_conversion_alternative = variant<std::unique_ptr<int>, int*>;
  using expanded = variant<std::unique_ptr<int>, int*, std::int32_t>;

  auto const src = variant_with_unique_ptr_and_conversion_alternative{std::make_unique<int>(42)};
  auto const result = variant_cast<expanded>(src);
}
