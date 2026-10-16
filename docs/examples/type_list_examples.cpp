// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Examples for metafunctions in @c type_lists

#include <tuple>
#include <type_traits>

#include "arene/base/type_list.hpp"

namespace {

using arene::base::type_list;
using arene::base::type_lists::drop_t;
using arene::base::type_lists::reverse_t;
using arene::base::type_lists::take_t;

// clang-format off

//! [drop_example]
static_assert(
    std::is_same<
      std::tuple<double>,
      drop_t<std::tuple<int, char, double>, 2>
    >::value,
    "");

static_assert(
    std::is_same<
      type_list<>,
      drop_t<type_list<int, char, double>, 4>
    >::value,
    "");
//! [drop_example]

//! [take_example]
static_assert(
    std::is_same<
      std::tuple<int, char>,
      take_t<std::tuple<int, char, double>, 2>
    >::value,
    "");

static_assert(
    std::is_same<
      type_list<int, char, double>,
      take_t<type_list<int, char, double>, 4>
    >::value,
    "");
//! [take_example]

//! [reverse_example]
static_assert(
    std::is_same<
      std::tuple<double, char, int>,
      reverse_t<std::tuple<int, char, double>>
    >::value,
    "");

static_assert(
    std::is_same<
      type_list<>,
      reverse_t<type_list<>>
    >::value,
    "");
//! [reverse_example]

// clang-format on

}  // namespace

auto main() -> int {}
