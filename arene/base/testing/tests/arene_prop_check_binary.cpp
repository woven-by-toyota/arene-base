// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iostream>

#include "arene/base/inline_container/vector.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"

// NOLINTBEGIN(readability-identifier-length)
namespace {

using arene::base::inline_vector;

auto check_vector_sizes() -> ::arene::base::testing::property_summary {
  return ARENE_PROP_CHECK("vector sizes match", (), {
    auto const x = inline_vector<int, 2>(1);
    auto const y = inline_vector<unsigned int, 2>(1);
    ARENE_PROP_ASSERT(x.size() == y.size());
  });
}

auto check_vector_front() -> ::arene::base::testing::property_summary {
  return ARENE_PROP_CHECK("vector first elements match", (), {
    auto const x = inline_vector<int, 2>(1);
    auto const y = inline_vector<int, 2>(2);
    ARENE_PROP_ASSERT(x.front() == y.front());
  });
}

auto prng_is_available() -> ::arene::base::testing::property_summary {
  return ARENE_PROP_CHECK("prng_is_available", (), {
    auto const value = prng();
    ARENE_PROP_ASSERT(value == value);
  });
}

auto params_are_generated() -> ::arene::base::testing::property_summary {
  return ARENE_PROP_CHECK("params_are_generated", (int a), { ARENE_PROP_ASSERT(a == a); });
}

}  // namespace

auto main() -> int {
  std::cerr                            //
      << check_vector_sizes() << "\n"  //
      << check_vector_front() << "\n"  //
      << prng_is_available() << "\n"   //
      << params_are_generated() << "\n";
}

// NOLINTEND(readability-identifier-length)
