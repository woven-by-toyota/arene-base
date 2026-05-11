// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/tests/sort_fwd.hpp"
#include "stdlib/include/algorithm"
#include "testlibs/minitest/minitest.hpp"

template <>
constexpr auto sort_test::config::version<sort_test::config::override> = sort_test::config::flavor::std_stable_sort;
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
template <>
auto const sort_test::config::function_under_test<sort_test::config::override> = FUNCTION_LIFT(std::stable_sort);

#include "arene/base/algorithm/tests/sort.hpp"
