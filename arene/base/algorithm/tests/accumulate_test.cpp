// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/accumulate.hpp"

#include "arene/base/algorithm/tests/reduce_fwd.hpp"

template <>
constexpr auto reduce_test::config::version<reduce_test::config::override> = reduce_test::config::flavor::accumulate;
template <>
constexpr auto const& reduce_test::config::function_under_test<reduce_test::config::override> = arene::base::accumulate;

#include "arene/base/algorithm/tests/reduce.hpp"  // IWYU pragma: keep
