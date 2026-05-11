// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/tests/transform_reduce_fwd.hpp"
#include "stdlib/include/numeric"
#include "testlibs/minitest/minitest.hpp"

template <>
constexpr auto transform_reduce_test::config::version<transform_reduce_test::config::override> =
    transform_reduce_test::config::flavor::std_inner_product;
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
template <>
auto const transform_reduce_test::config::function_under_test<transform_reduce_test::config::override> =
    FUNCTION_LIFT(std::inner_product);

#include "arene/base/algorithm/tests/transform_reduce.hpp"
