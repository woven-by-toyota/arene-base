// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/inner_product.hpp"

#include "arene/base/algorithm/tests/transform_reduce_fwd.hpp"

template <>
constexpr auto transform_reduce_test::config::version<transform_reduce_test::config::override> =
    transform_reduce_test::config::flavor::inner_product;
template <>
constexpr auto const& transform_reduce_test::config::function_under_test<transform_reduce_test::config::override> =
    arene::base::inner_product;

#include "arene/base/algorithm/tests/transform_reduce.hpp"  // IWYU pragma: keep
