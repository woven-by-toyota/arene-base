// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/tests/iota_fwd.hpp"
#include "stdlib/include/numeric"
#include "testlibs/minitest/minitest.hpp"

template <>
constexpr auto iota_test::config::std_version<iota_test::config::override> = true;
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
template <>
auto const iota_test::config::iota<iota_test::config::override> = FUNCTION_LIFT(std::iota);

#include "arene/base/algorithm/tests/iota.hpp"
