// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/indirect_result_t.hpp"

struct dereferenceable_without_value_type_iterator {
  auto operator*() const -> int&;
};

struct forwarding_callable {
  template <typename Arg>
  auto operator()(Arg&&) const -> Arg&&;
};

// This should fail to compile and list a constraint violation on is_indirectly_readable_v
using result = arene::base::indirect_result_t<forwarding_callable, dereferenceable_without_value_type_iterator>;
