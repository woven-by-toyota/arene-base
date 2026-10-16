// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/tests/reduce_fwd.hpp"
#include "stdlib/include/numeric"
#include "testlibs/minitest/minitest.hpp"

template <>
constexpr auto reduce_test::config::version<reduce_test::config::override> =
    reduce_test::config::flavor::std_accumulate;
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
template <>
auto const reduce_test::config::function_under_test<reduce_test::config::override> = FUNCTION_LIFT(std::accumulate);

#include "arene/base/algorithm/tests/reduce.hpp"
