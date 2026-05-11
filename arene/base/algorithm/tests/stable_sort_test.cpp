// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/stable_sort.hpp"

#include "arene/base/algorithm/tests/sort_fwd.hpp"

template <>
constexpr auto sort_test::config::version<sort_test::config::override> = sort_test::config::flavor::stable_sort;
template <>
constexpr auto const& sort_test::config::function_under_test<sort_test::config::override> = arene::base::stable_sort;

#include "arene/base/algorithm/tests/sort.hpp"  // IWYU pragma: keep
