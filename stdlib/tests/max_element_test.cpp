// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/tests/max_element_fwd.hpp"
#include "stdlib/include/algorithm"
#include "testlibs/minitest/minitest.hpp"

template <>
constexpr auto max_element_test::config::version<max_element_test::config::override> =
    max_element_test::config::flavor::std_max_element;
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
template <>
auto const max_element_test::config::function_under_test<max_element_test::config::override> =
    FUNCTION_LIFT(std::max_element);

#include "arene/base/algorithm/tests/max_element.hpp"
